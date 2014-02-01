/*
 * File:   filtering_algorithm_top.cpp
 * Author: Felix Winterstein
 *
 * Created on 25 April 2013, 10:52
 */

#include "filtering_algorithm_util.h"
#include "dyn_mem_alloc.h"
#include "stack.h"
#include "trace_address.h"

#include "ap_utils.h"

#ifdef V1

#ifndef __SYNTHESIS__
#include <stdio.h>
#include <stdlib.h>
#endif


// global array for the tree (keep heap local to this file)
kdTree_type tree_node_int_memory[N];
kdTree_leaf_type tree_node_leaf_memory[N];

data_type centre_positions[K];

centre_heap_type centre_heap[SCRATCHPAD_SIZE];
centre_list_pointer centre_freelist[SCRATCHPAD_SIZE];
centre_list_pointer centre_next_free_location;

centre_list_pointer heap_utilisation;

#ifndef __SYNTHESIS__
uint visited_nodes;
#endif


void filtering_algorithm_top(	volatile kdTree_type *node_data,
								volatile node_pointer *node_address,
								volatile data_type *cntr_pos_init,
								node_pointer n,
								centre_index_type k,
								node_pointer root,
								volatile coord_type_ext *distortion_out,
								volatile data_type *clusters_out)
{
	#pragma AP interface ap_none register port=n
	#pragma AP interface ap_none register port=k
	#pragma AP interface ap_none register port=root

	#pragma AP data_pack variable=node_data // recursively pack struct kdTree_type
	#pragma AP data_pack variable=cntr_pos_init // pack struct data_type
	#pragma AP data_pack variable=clusters_out // pack struct data_type
	#pragma AP data_pack variable=tree_node_int_memory
	#pragma AP data_pack variable=tree_node_leaf_memory

	#pragma AP resource variable=centre_freelist core=RAM_2P_LUTRAM
	#pragma AP resource variable=centre_heap core=RAM_2P_BRAM
	#pragma AP resource variable=centre_positions core=RAM_2P_BRAM
	#pragma AP resource variable=tree_node_leaf_memory core=RAM_2P_BRAM
	#pragma AP resource variable=tree_node_int_memory core=RAM_2P_BRAM


	init_tree_node_memory(node_data,node_address,n);

	centre_type filt_centres_out[K];
	data_type new_centre_positions[K];

	it_loop: for (uint l=0; l<L; l++) {
		#pragma AP loop_tripcount min=1 max=1 avg=1
		#ifndef __SYNTHESIS__
		visited_nodes = 0;
		#endif

		if (l==0) {
			for (centre_index_type i=0; i<=k; i++) {
				#pragma AP pipeline II=1
				#pragma AP loop_tripcount min=128 max=128 avg=128
				centre_positions[i].value = cntr_pos_init[i].value;
				if (i==k) {
					break;
				}
			}
		} else {
			for (centre_index_type i=0; i<=k; i++) {
				#pragma AP pipeline II=1
				#pragma AP loop_tripcount min=128 max=128 avg=128
				centre_positions[i] = new_centre_positions[i];
				if (i == k) {
					break;
				}
			}
		}
		filter(root, k, filt_centres_out);

		#ifndef __SYNTHESIS__
		printf("%d: visited nodes: %d\n",0,visited_nodes);
		#endif

		// re-init centre positions
		update_centres(filt_centres_out, k, new_centre_positions);

	}


	output_loop: for (centre_index_type i=0; i<=k; i++) {
		#pragma AP pipeline II=1
		#pragma AP loop_tripcount min=128 max=128 avg=128
		distortion_out[i] = filt_centres_out[i].sum_sq;
		clusters_out[i].value = new_centre_positions[i].value;
		if (i==k) {
			break;
		}
	}
}


void init_tree_node_memory(volatile kdTree_type *node_data, volatile node_pointer *node_address, node_pointer n)
{
	#pragma AP inline

	init_nodes_loop: for (node_pointer i=0; i<=n; i++) {
		#pragma AP loop_tripcount min=16384 max=16384 avg=16384
		#pragma AP pipeline II=1
		node_pointer tmp_node_address = node_address[i];
		kdTree_type tmp_node;
		tmp_node = node_data[i];

		if (tmp_node_address.get_bit(NODE_POINTER_BITWIDTH-1) == false) {

			ap_uint<NODE_POINTER_BITWIDTH-1> tmp_node_address_short;
			tmp_node_address_short = tmp_node_address.range(NODE_POINTER_BITWIDTH-2,0);

			tree_node_int_memory[ tmp_node_address_short].bnd_hi.value = tmp_node.bnd_hi.value;
			tree_node_int_memory[ tmp_node_address_short].bnd_lo.value = tmp_node.bnd_lo.value;
			tree_node_int_memory[ tmp_node_address_short].count = tmp_node.count;
			tree_node_int_memory[ tmp_node_address_short].midPoint.value = tmp_node.midPoint.value;
			tree_node_int_memory[ tmp_node_address_short].wgtCent.value = tmp_node.wgtCent.value;
			tree_node_int_memory[ tmp_node_address_short].sum_sq = tmp_node.sum_sq;
			tree_node_int_memory[ tmp_node_address_short].left = tmp_node.left;
			tree_node_int_memory[ tmp_node_address_short].right = tmp_node.right;
			//tree_node_int_memory[ tmp_node_address_short] = tmp_node;
		} else {

			ap_uint<NODE_POINTER_BITWIDTH-1> tmp_node_address_short;
			tmp_node_address_short = tmp_node_address.range(NODE_POINTER_BITWIDTH-2,0);

			// needs to be done by hand
			tree_node_leaf_memory[ tmp_node_address_short].wgtCent.value =  tmp_node.wgtCent.value;
			tree_node_leaf_memory[ tmp_node_address_short].sum_sq =  tmp_node.sum_sq;

		}

		if (i==n) {
			break;
		}
	}
}


void update_centres(centre_type *centres_in,centre_index_type k, data_type *centres_positions_out)
{
	//#pragma AP inline
	centre_update_loop: for (centre_index_type i=0; i<=k; i++) {
		#pragma AP loop_tripcount min=128 max=128 avg=128
		#pragma AP pipeline II=2

		coord_type tmp_count = centres_in[i].count;
		if ( tmp_count == 0 )
			tmp_count = 1;

		data_type_ext tmp_wgtCent = centres_in[i].wgtCent;
		data_type tmp_new_pos;
		for (uint d=0; d<D; d++) {
			#pragma AP unroll
			coord_type_ext tmp_div_ext = (get_coord_type_vector_ext_item(tmp_wgtCent.value,d) / tmp_count); //let's see what it does with that...
			coord_type tmp_div = (coord_type) tmp_div_ext;
			#pragma AP resource variable=tmp_div core=DivnS
			set_coord_type_vector_item(&tmp_new_pos.value,tmp_div,d);
		}
		centres_positions_out[i] = tmp_new_pos;
    	if (i==k) {
    		break;
    	}
	}
}


void filter (node_pointer root,
			centre_index_type k,
			centre_type *centres_out)
{

    centre_type centre_buffer[K];
	#pragma AP resource variable=centre_buffer core=RAM_2P_LUTRAM

    // init centre buffer
    init_centre_buffer_loop: for(centre_index_type i=0; i<=k; i++) {
    	#pragma AP pipeline II=1
		#pragma AP loop_tripcount min=128 max=128 avg=128
        centre_buffer[i].count = 0;
        centre_buffer[i].sum_sq = 0;
        for (uint d=0; d<D; d++) {
			#pragma AP unroll
        	set_coord_type_vector_ext_item(&centre_buffer[i].wgtCent.value,0,d);
        }
    	if (i==k) {
    		break;
    	}
    }


    init_allocator<centre_list_pointer>(centre_freelist, &centre_next_free_location, SCRATCHPAD_SIZE-2);

    // allocate first centre list
    centre_list_pointer centre_list_idx = malloc<centre_list_pointer>(centre_freelist, &centre_next_free_location);
    centre_heap_type *centre_list_idx_ptr =  make_pointer<centre_heap_type>(centre_heap, (uint)centre_list_idx);

    heap_utilisation = 1;

    init_centre_list_loop: for(centre_index_type i=0; i<=k; i++) {
		#pragma AP pipeline II=1
		#pragma AP loop_tripcount min=128 max=128 avg=128
    	centre_list_idx_ptr->idx[i] = i;
    	if (i==k) {
    		break;
    	}
    }


    init_stack();
    uint node_stack_length = push_node(root);
    uint cntr_stack_length = push_centre_set(centre_list_idx,k);


    tree_search_loop: while (node_stack_length != 0) {
		#pragma AP loop_tripcount min=20359 max=32767 avg=20359
		#ifndef __SYNTHESIS__
    	visited_nodes++;
		#endif

        // fetch head of stack
        node_pointer u;
        node_stack_length = pop_node(&u);

        kdTree_type tmp_u;
		if (u.get_bit(NODE_POINTER_BITWIDTH-1) == false) {
			ap_uint<NODE_POINTER_BITWIDTH-1> u_short;
			u_short = u.range(NODE_POINTER_BITWIDTH-2,0);
			kdTree_type *u_ptr = make_pointer<kdTree_type>(tree_node_int_memory, (uint)u_short);
			tmp_u = *u_ptr;
		} else {
			ap_uint<NODE_POINTER_BITWIDTH-1> u_short;
			u_short = u.range(NODE_POINTER_BITWIDTH-2,0);
			kdTree_leaf_type *u_leaf_ptr = make_pointer<kdTree_leaf_type>(tree_node_leaf_memory, (uint)u_short);
	        tmp_u.wgtCent = u_leaf_ptr->wgtCent;
	        tmp_u.sum_sq = u_leaf_ptr->sum_sq;
	        tmp_u.bnd_hi.value = 0;
	        tmp_u.bnd_lo.value = 0;
	        tmp_u.count = 1;
	        tmp_u.left = NULL_PTR;
	        tmp_u.right = NULL_PTR;
	        tmp_u.midPoint.value = 0;
		}

        centre_list_pointer tmp_cntr_list;
        centre_index_type tmp_k;
        cntr_stack_length = pop_centre_set(&tmp_cntr_list,&tmp_k);
        centre_heap_type *tmp_cntr_list_ptr = make_pointer<centre_heap_type>(centre_heap, (uint)tmp_cntr_list);

        bool rdy_for_deletion;
        trace_address(tmp_cntr_list, false, &rdy_for_deletion);



        data_type tmp_centre_positions[K];
        centre_index_type tmp_centre_indices[K];
        fetch_centres_loop: for (centre_index_type i=0; i<=tmp_k; i++) {
			#pragma AP loop_tripcount min=2 max=128 avg=5
			#pragma AP pipeline II=1
        	tmp_centre_indices[i] = tmp_cntr_list_ptr->idx[i];
            tmp_centre_positions[i] = centre_positions[tmp_cntr_list_ptr->idx[i]];
        	if (i==tmp_k) {
        		break;
        	}
        }

        bool tmp_deadend;
        kdTree_type tmp_u_out;
        centre_index_type tmp_centre_indices_out[K];
        centre_index_type tmp_final_centre_index;
        coord_type_ext tmp_sum_sq_out;
        centre_index_type tmp_k_out;

        data_type_ext comp_point;
        if ( (tmp_u.left == NULL_PTR) && (tmp_u.right == NULL_PTR) ) {
            comp_point = tmp_u.wgtCent;
        } else {
            comp_point = conv_short_to_long(tmp_u.midPoint);
        }

        centre_index_type tmp_final_idx;
        data_type z_star;
    	coord_type_ext tmp_min_dist;

    	minsearch_loop: for (centre_index_type i=0; i<=tmp_k; i++) {
    		#pragma AP loop_tripcount min=2 max=128 avg=5
    		#pragma AP pipeline II=1

    		coord_type_ext tmp_dist;
    		data_type position = tmp_centre_positions[i];
    		compute_distance(conv_short_to_long(position), comp_point, &tmp_dist);

    		if ((tmp_dist < tmp_min_dist) || (i==0)) {
    			tmp_min_dist = tmp_dist;
    			tmp_final_idx = tmp_centre_indices[i];
    			z_star = position;
    		}

    		if (i==tmp_k) {
    			break;
    		}
    	}


        //copy candidates that survive pruning into new list
        centre_index_type new_k=(1<<CNTR_INDEX_BITWIDTH)-1;
        //centre_index_type new_k=0;
        centre_index_type tmp_new_idx=0;

        tooFar_loop: for (centre_index_type i=0; i<=tmp_k; i++) {
    		#pragma AP loop_tripcount min=2 max=128 avg=5
    		#pragma AP pipeline II=1
            bool too_far;
            tooFar_fi(z_star, tmp_centre_positions[i], tmp_u.bnd_lo, tmp_u.bnd_hi, &too_far);
            if ( too_far==false ) {
            	tmp_centre_indices_out[ tmp_new_idx] = tmp_centre_indices[ i];
                tmp_new_idx++;
                new_k++;
            }
        	if (i==tmp_k) {
        		break;
        	}
        }

        // some scaling...
        data_type_ext tmp_wgtCent = tmp_u.wgtCent;
        for (uint d=0; d<D; d++) {
    		#pragma AP unroll
        	coord_type_ext tmp = get_coord_type_vector_ext_item(tmp_wgtCent.value,d);
        	tmp = tmp >> MUL_FRACTIONAL_BITS;
        	set_coord_type_vector_ext_item(&tmp_wgtCent.value,tmp,d);
        }

        // z_star == tmp_centre_positions[idx_closest] !
        // update sum_sq of centre
        coord_type_ext tmp1_2, tmp2_2;
        data_type_ext tmp_z_star = conv_short_to_long(z_star);
        dot_product(tmp_z_star,tmp_wgtCent,&tmp1_2);
        dot_product(tmp_z_star,tmp_z_star ,&tmp2_2);
        coord_type_ext tmp1, tmp2;
        tmp1 = tmp1_2<<1;
        tmp2 = tmp2_2>>MUL_FRACTIONAL_BITS;
        coord_type tmp_count = tmp_u.count;
        coord_type_ext tmp2_sat = saturate_mul_input(tmp2);
        coord_type_ext tmp_count_sat = saturate_mul_input(tmp_count);
        coord_type_ext tmp3 = tmp2_sat*tmp_count_sat;
        coord_type_ext tmp_sum_sq1 = tmp_u.sum_sq+tmp3;
        coord_type_ext tmp_sum_sq = tmp_sum_sq1-tmp1;
    	#pragma AP resource variable=tmp3 core=MulnS

        //bool tmp_deadend;
        if ((new_k == 0) || ( (tmp_u.left == NULL_PTR) && (tmp_u.right == NULL_PTR) )) {
        	tmp_deadend = true;
        } else {
        	tmp_deadend = false;
        }

        tmp_final_centre_index = tmp_final_idx;
        tmp_k_out = new_k;
        tmp_u_out = tmp_u;
        tmp_sum_sq_out = tmp_sum_sq;

        // free list that has been read twice
        if (rdy_for_deletion == true) {
        	free<centre_list_pointer>(centre_freelist, &centre_next_free_location, tmp_cntr_list);
        	heap_utilisation--;
        }

        // write back
        if ( tmp_deadend == true ) {

            // weighted centroid of this centre
            for (uint d=0; d<D; d++) {
				#pragma AP unroll
            	coord_type_ext tmp1 = get_coord_type_vector_ext_item(centre_buffer[tmp_final_centre_index].wgtCent.value,d);
            	coord_type_ext tmp2 = get_coord_type_vector_ext_item(tmp_u_out.wgtCent.value,d);
            	set_coord_type_vector_ext_item(&centre_buffer[tmp_final_centre_index].wgtCent.value,tmp1+tmp2,d);
            }
            // update number of points assigned to centre
            coord_type tmp1 =  tmp_u_out.count;
            coord_type tmp2 =  centre_buffer[tmp_final_centre_index].count;
            centre_buffer[tmp_final_centre_index].count = tmp1 + tmp2;
            coord_type_ext tmp3 =  tmp_sum_sq_out;
            coord_type_ext tmp4 =  centre_buffer[tmp_final_centre_index].sum_sq;
            centre_buffer[tmp_final_centre_index].sum_sq  = tmp3 + tmp4;

        } else {

            // allocate new centre list
        	centre_list_pointer new_centre_list_idx;
        	centre_index_type new_k;

        	if (heap_utilisation < SCRATCHPAD_SIZE-1) {

				new_centre_list_idx = malloc<centre_list_pointer>(centre_freelist, &centre_next_free_location);
				centre_heap_type *new_centre_list_idx_ptr =  make_pointer<centre_heap_type>(centre_heap, (uint)new_centre_list_idx);
				heap_utilisation++;

				// write new centre indices into it
				write_back_centres_loop: for(centre_index_type i=0; i<=tmp_k_out; i++) {
					#pragma AP pipeline II=1
					#pragma AP loop_tripcount min=2 max=128 avg=5
					new_centre_list_idx_ptr->idx[i] = tmp_centre_indices_out[i];
					if (i==tmp_k_out) {
						break;
					}
				}

				new_k = tmp_k_out;

        	} else {
        		new_centre_list_idx = 0;
        		new_k = k;
        	}
            bool dummy_rdy_for_deletion;
            trace_address(new_centre_list_idx, true, &dummy_rdy_for_deletion);

            node_pointer left_child = tmp_u_out.left;
            node_pointer right_child = tmp_u_out.right;

            // push children onto stack
            node_stack_length = push_node(right_child);
            node_stack_length = push_node(left_child);

            // push centre lists for both children onto stack
            cntr_stack_length = push_centre_set(new_centre_list_idx,new_k);
            cntr_stack_length = push_centre_set(new_centre_list_idx,new_k);
        }
    }

    // readout centres
    read_out_centres_loop: for(centre_index_type i=0; i<=k; i++) {
		#pragma AP pipeline II=1
		#pragma AP loop_tripcount min=128 max=128 avg=128
    	centres_out[i] = centre_buffer[i];
    	if (i==k) {
    		break;
    	}
    }
}

#endif
