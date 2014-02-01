/*
 * File:   filtering_algorithm_top_v2.cpp
 * Author: Felix Winterstein
 *
 * Created on 25 April 2013, 10:52
 */


#include "filtering_algorithm_top.h"
#include "filtering_algorithm_util.h"
#include "dyn_mem_alloc.h"
#include "stack.h"
#include "trace_address.h"

#include "ap_utils.h"
#include <hls_stream.h>



#ifndef __SYNTHESIS__
#include <stdio.h>
#include <stdlib.h>
#endif

#ifdef V2

/*
template<typename T, uint SIZE, uint PAR> class my_mem
{
public:
	T* get_object(const uint p)
	{
		if (p == 0) {
			return &m0[0];
		} else if (p == 1) {
			return &m1[0];
		} else if (p == 2) {
			return &m2[0];
		} else if (p == 3) {
			return &m3[0];
		}
	}
private:
	T m0[SIZE/PAR];
	T m1[SIZE/PAR];
	T m2[SIZE/PAR];
	T m3[SIZE/PAR];
};
*/

#ifndef PARALLELISE

// tree memory
kdTree_type tree_node_int_memory[N];
kdTree_leaf_type tree_node_leaf_memory[N];
// centre positions
data_type centre_positions[K];
//tree search output
centre_type filt_centres_out[K];

#else
// tree memory
//my_mem<kdTree_type,N,P> tree_node_int_memory;
//my_mem<kdTree_leaf_type,N,P> tree_node_leaf_memory;
kdTree_type tree_node_int_memory[N];
kdTree_leaf_type tree_node_leaf_memory[N];
// centre positions
data_type centre_positions[K][P];
//tree search output
centre_type filt_centres_out[K][P];

#endif

template<typename __STREAM_T__> class my_stream
{
public:
	hls::stream<__STREAM_T__> &get_object(const uint p)
	{
		if (p == 0) {
			return s0;
		} else if (p == 1) {
			return s1;
		} else if (p == 2) {
			return s2;
		} else {
			return s3;
		}
	}
private:
	hls::stream<__STREAM_T__> s0;
	hls::stream<__STREAM_T__> s1;
	hls::stream<__STREAM_T__> s2;
	hls::stream<__STREAM_T__> s3;
};

my_stream<node_pointer> u_stream;
my_stream<centre_list_pointer> C_stream;
my_stream<centre_list_pointer> newC_stream;
my_stream<centre_index_type> k_stream;
my_stream<bool> d_stream;
my_stream<bool> alloc_full_stream;
my_stream<bool> alloc_full_stream2;
my_stream<node_pointer> u_stream2;
my_stream<centre_list_pointer> C_stream2;
my_stream<centre_list_pointer> newC_stream2;
my_stream<centre_index_type> k_stream2;
my_stream<bool> deadend_stream;
my_stream<bool> d_stream2;
my_stream<centre_index_type> final_index_stream;
my_stream<coord_type_ext> sum_sq_stream;


#ifndef __SYNTHESIS__
uint visited_nodes;
uint max_heap_utilisation;
#endif




void filtering_algorithm_top(	volatile kdTree_type *node_data,
								volatile node_pointer *node_address,
								volatile data_type *cntr_pos_init,
								node_pointer n,
								centre_index_type k,
								volatile node_pointer *root,
								volatile coord_type_ext *distortion_out,
								volatile data_type *clusters_out)
{
	#pragma AP interface ap_none register port=n
	#pragma AP interface ap_none register port=k
	//#pragma AP interface ap_none register port=root

	node_pointer root_array[P];

	#pragma AP data_pack variable=node_data
	#pragma AP data_pack variable=cntr_pos_init
	#pragma AP data_pack variable=clusters_out
	#pragma AP data_pack variable=tree_node_int_memory
	#pragma AP data_pack variable=tree_node_leaf_memory
	//#pragma AP data_pack variable=centre_heap

	#pragma AP resource variable=centre_positions core=RAM_2P_BRAM
	#pragma AP resource variable=tree_node_leaf_memory core=RAM_2P_BRAM
	#pragma AP resource variable=tree_node_int_memory core=RAM_2P_BRAM

	#pragma HLS array_partition variable=root_array complete

	for (uint p=0; p<P; p++) {
		root_array[p] = root[p];
	}

	init_tree_node_memory(node_data,node_address,n);


	data_type new_centre_positions[K];
	#pragma AP data_pack variable=filt_centres_out

	#ifdef PARALLELISE
		centre_type filt_centres_out_reduced[K];
		#pragma AP data_pack variable=filt_centres_out_reduced

		#pragma HLS array_partition variable=tree_node_int_memory block factor=2
		#pragma HLS array_partition variable=tree_node_leaf_memory block factor=2
		#pragma HLS array_partition variable=centre_positions complete dim=2
		#pragma HLS array_partition variable=filt_centres_out complete dim=2

	#endif

	it_loop: for (uint l=0; l<L; l++) {
		#pragma AP loop_tripcount min=1 max=1 avg=1

		#ifndef __SYNTHESIS__
		visited_nodes = 0;
		max_heap_utilisation = 0;
		#endif

		for (centre_index_type i=0; i<=k; i++) {
			#pragma AP loop_tripcount min=128 max=128 avg=128
			#ifndef PARALLELISE
				data_type tmp_pos;
				if (l==0) {
					tmp_pos = cntr_pos_init[i];
				} else {
					tmp_pos = new_centre_positions[i];
				}
				centre_positions[i] = tmp_pos;
			#else
				data_type tmp_pos;
				if (l==0) {
					tmp_pos = cntr_pos_init[i];
				} else {
					tmp_pos = new_centre_positions[i];
				}
				for (uint p=0; p<P; p++) {
					#pragma HLS unroll
					#pragma HLS dependence variable=centre_positions inter false
					centre_positions[i][p] = tmp_pos;
				}
			#endif
			if (i==k) {
				break;
			}
		}

		for (uint p=0; p<P; p++) {
			#pragma HLS unroll
			if (p == 0) {
				filter<0>(root_array[p],k,p);
			}
			if (p == 1) {
				filter<1>(root_array[p],k,p);
			}
			if (p == 2) {
				filter<2>(root_array[p],k,p);
			}
			if (p == 3) {
				filter<3>(root_array[p],k,p);
			}

			#pragma HLS dependence variable=filt_centres_out inter false
			#pragma HLS dependence variable=centre_positions inter false
			#pragma HLS dependence variable=tree_node_leaf_memory inter false
			#pragma HLS dependence variable=tree_node_int_memory inter false
			#pragma HLS dependence variable=root_array inter false

			#pragma HLS dependence variable=u_stream inter false
			#pragma HLS dependence variable=C_stream inter false
			#pragma HLS dependence variable=newC_stream inter false
			#pragma HLS dependence variable=k_stream inter false
			#pragma HLS dependence variable=d_stream inter false
			#pragma HLS dependence variable=alloc_full_stream inter false
			#pragma HLS dependence variable=alloc_full_stream2 inter false
			#pragma HLS dependence variable=u_stream2 inter false
			#pragma HLS dependence variable=C_stream2 inter false
			#pragma HLS dependence variable=newC_stream2 inter false
			#pragma HLS dependence variable=k_stream2 inter false
			#pragma HLS dependence variable=deadend_stream inter false
			#pragma HLS dependence variable=d_stream2 inter false
			#pragma HLS dependence variable=final_index_stream inter false
			#pragma HLS dependence variable=sum_sq_stream inter false

		}


		#ifdef PARALLELISE

			for(centre_index_type i=0; i<=k; i++) {
				#pragma HLS loop_tripcount min=128 max=128 avg=128
				#pragma HLS pipeline II=1

				coord_type_ext arr_count[P];
				coord_type_ext arr_sum_sq[P];
				coord_type_vector_ext arr_wgtCent[P];
				#pragma HLS array_partition variable=arr_count complete
				#pragma HLS array_partition variable=arr_sum_sq complete
				#pragma HLS array_partition variable=arr_wgtCent complete

				for (uint p=0; p<P; p++) {
					#pragma HLS unroll
					#pragma HLS dependence variable=arr_count inter false
					#pragma HLS dependence variable=arr_sum_sq inter false
					#pragma HLS dependence variable=arr_wgtCent inter false
					arr_count[p] = ((coord_type_ext)filt_centres_out[i][p].count);
					arr_sum_sq[p] = (filt_centres_out[i][p].sum_sq);
					arr_wgtCent[p] = (filt_centres_out[i][p].wgtCent.value);
				}

				filt_centres_out_reduced[i].count = tree_adder(arr_count,P);
				filt_centres_out_reduced[i].sum_sq = tree_adder(arr_sum_sq,P);
				coord_type_vector_ext tmp_sum;
				for (uint d=0; d<D; d++) {
					#pragma HLS unroll
					coord_type_ext tmp_a[P];
					for (uint p=0; p<P; p++) {
						#pragma HLS unroll
						#pragma HLS dependence variable=tmp_a inter false
						tmp_a[p] = get_coord_type_vector_ext_item(arr_wgtCent[p],d);
					}
					coord_type_ext tmp = tree_adder(tmp_a,P);
					set_coord_type_vector_ext_item(&tmp_sum,tmp,d);
				}
				filt_centres_out_reduced[i].wgtCent.value = tmp_sum;

				if (i==k) {
					break;
				}
			}

		#endif

		#ifndef __SYNTHESIS__
		printf("%d: visited nodes: %d\n",0,visited_nodes);
		printf("%d: max heap utilisation: %d\n",0,max_heap_utilisation);
		#endif

		// re-init centre positions
		#ifndef PARALLELISE
			update_centres(filt_centres_out, k, new_centre_positions);
		#else
			update_centres(filt_centres_out_reduced, k, new_centre_positions);
		#endif
	}


	output_loop: for (centre_index_type i=0; i<=k; i++) {
		#pragma AP pipeline II=1
		#pragma AP loop_tripcount min=128 max=128 avg=128

		#ifndef PARALLELISE
			distortion_out[i] = filt_centres_out[i].sum_sq;
		#else
			distortion_out[i] = filt_centres_out_reduced[i].sum_sq;
		#endif
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
		#pragma AP pipeline II=8
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

		} else {


			ap_uint<NODE_POINTER_BITWIDTH-1> tmp_node_address_short;
			tmp_node_address_short = tmp_node_address.range(NODE_POINTER_BITWIDTH-2,0);

			tree_node_leaf_memory[ tmp_node_address_short].wgtCent.value =  tmp_node.wgtCent.value;
			tree_node_leaf_memory[ tmp_node_address_short].sum_sq =  tmp_node.sum_sq;

		}

		if (i==n) {
			break;
		}
	}
}
/*
void init_tree_node_memory(volatile kdTree_type *node_data, volatile node_pointer *node_address, node_pointer n)
{
	#pragma AP inline

	kdTree_type *tree_node_int_memory_r0 = tree_node_int_memory.get_object(0);
	kdTree_leaf_type *tree_node_leaf_memory_r0 = tree_node_leaf_memory.get_object(0);
	kdTree_type *tree_node_int_memory_r1 = tree_node_int_memory.get_object(1);
	kdTree_leaf_type *tree_node_leaf_memory_r1 = tree_node_leaf_memory.get_object(1);
	kdTree_type *tree_node_int_memory_r2 = tree_node_int_memory.get_object(2);
	kdTree_leaf_type *tree_node_leaf_memory_r2 = tree_node_leaf_memory.get_object(2);
	kdTree_type *tree_node_int_memory_r3 = tree_node_int_memory.get_object(3);
	kdTree_leaf_type *tree_node_leaf_memory_r3 = tree_node_leaf_memory.get_object(3);

	init_nodes_loop: for (node_pointer i=0; i<=n; i++) {
		#pragma AP loop_tripcount min=16384 max=16384 avg=16384
		#pragma AP pipeline II=8
		node_pointer tmp_node_address = node_address[i];
		kdTree_type tmp_node;
		tmp_node = node_data[i];

		ap_uint<NODE_POINTER_BITWIDTH-1> tmp_node_address_short;
		tmp_node_address_short = tmp_node_address.range(NODE_POINTER_BITWIDTH-2,0);

		if (tmp_node_address.get_bit(NODE_POINTER_BITWIDTH-1) == false) {

			ap_uint<2> selector = tmp_node_address_short.range(NODE_POINTER_BITWIDTH-2,NODE_POINTER_BITWIDTH-2-2+1);

			ap_uint<NODE_POINTER_BITWIDTH-1-2> tmp_node_address_short_short;
			tmp_node_address_short_short = tmp_node_address_short.range(NODE_POINTER_BITWIDTH-2-2,0);

			if (selector == 0) {
				tree_node_int_memory_r0[ tmp_node_address_short_short].bnd_hi.value = tmp_node.bnd_hi.value;
				tree_node_int_memory_r0[ tmp_node_address_short_short].bnd_lo.value = tmp_node.bnd_lo.value;
				tree_node_int_memory_r0[ tmp_node_address_short_short].count = tmp_node.count;
				tree_node_int_memory_r0[ tmp_node_address_short_short].midPoint.value = tmp_node.midPoint.value;
				tree_node_int_memory_r0[ tmp_node_address_short_short].wgtCent.value = tmp_node.wgtCent.value;
				tree_node_int_memory_r0[ tmp_node_address_short_short].sum_sq = tmp_node.sum_sq;
				tree_node_int_memory_r0[ tmp_node_address_short_short].left = tmp_node.left;
				tree_node_int_memory_r0[ tmp_node_address_short_short].right = tmp_node.right;
			} else if (selector == 1) {
				tree_node_int_memory_r1[ tmp_node_address_short_short].bnd_hi.value = tmp_node.bnd_hi.value;
				tree_node_int_memory_r1[ tmp_node_address_short_short].bnd_lo.value = tmp_node.bnd_lo.value;
				tree_node_int_memory_r1[ tmp_node_address_short_short].count = tmp_node.count;
				tree_node_int_memory_r1[ tmp_node_address_short_short].midPoint.value = tmp_node.midPoint.value;
				tree_node_int_memory_r1[ tmp_node_address_short_short].wgtCent.value = tmp_node.wgtCent.value;
				tree_node_int_memory_r1[ tmp_node_address_short_short].sum_sq = tmp_node.sum_sq;
				tree_node_int_memory_r1[ tmp_node_address_short_short].left = tmp_node.left;
				tree_node_int_memory_r1[ tmp_node_address_short_short].right = tmp_node.right;
			} else if (selector == 2) {
				tree_node_int_memory_r2[ tmp_node_address_short_short].bnd_hi.value = tmp_node.bnd_hi.value;
				tree_node_int_memory_r2[ tmp_node_address_short_short].bnd_lo.value = tmp_node.bnd_lo.value;
				tree_node_int_memory_r2[ tmp_node_address_short_short].count = tmp_node.count;
				tree_node_int_memory_r2[ tmp_node_address_short_short].midPoint.value = tmp_node.midPoint.value;
				tree_node_int_memory_r2[ tmp_node_address_short_short].wgtCent.value = tmp_node.wgtCent.value;
				tree_node_int_memory_r2[ tmp_node_address_short_short].sum_sq = tmp_node.sum_sq;
				tree_node_int_memory_r2[ tmp_node_address_short_short].left = tmp_node.left;
				tree_node_int_memory_r2[ tmp_node_address_short_short].right = tmp_node.right;
			} else if (selector == 3) {
				tree_node_int_memory_r3[ tmp_node_address_short_short].bnd_hi.value = tmp_node.bnd_hi.value;
				tree_node_int_memory_r3[ tmp_node_address_short_short].bnd_lo.value = tmp_node.bnd_lo.value;
				tree_node_int_memory_r3[ tmp_node_address_short_short].count = tmp_node.count;
				tree_node_int_memory_r3[ tmp_node_address_short_short].midPoint.value = tmp_node.midPoint.value;
				tree_node_int_memory_r3[ tmp_node_address_short_short].wgtCent.value = tmp_node.wgtCent.value;
				tree_node_int_memory_r3[ tmp_node_address_short_short].sum_sq = tmp_node.sum_sq;
				tree_node_int_memory_r3[ tmp_node_address_short_short].left = tmp_node.left;
				tree_node_int_memory_r3[ tmp_node_address_short_short].right = tmp_node.right;
			}


		} else {

			ap_uint<2> selector = tmp_node_address_short.range(NODE_POINTER_BITWIDTH-2,NODE_POINTER_BITWIDTH-2-2+1);

			ap_uint<NODE_POINTER_BITWIDTH-1-2> tmp_node_address_short_short;
			tmp_node_address_short_short = tmp_node_address_short.range(NODE_POINTER_BITWIDTH-2-2,0);

			if (selector == 0) {
				tree_node_leaf_memory_r0[ tmp_node_address_short_short].wgtCent.value =  tmp_node.wgtCent.value;
				tree_node_leaf_memory_r0[ tmp_node_address_short_short].sum_sq =  tmp_node.sum_sq;
			} else if (selector == 1) {
				tree_node_leaf_memory_r1[ tmp_node_address_short_short].wgtCent.value =  tmp_node.wgtCent.value;
				tree_node_leaf_memory_r1[ tmp_node_address_short_short].sum_sq =  tmp_node.sum_sq;
			} else if (selector == 2) {
				tree_node_leaf_memory_r2[ tmp_node_address_short_short].wgtCent.value =  tmp_node.wgtCent.value;
				tree_node_leaf_memory_r2[ tmp_node_address_short_short].sum_sq =  tmp_node.sum_sq;
			} else if (selector == 3) {
				tree_node_leaf_memory_r3[ tmp_node_address_short_short].wgtCent.value =  tmp_node.wgtCent.value;
				tree_node_leaf_memory_r3[ tmp_node_address_short_short].sum_sq =  tmp_node.sum_sq;
			}
		}

		if (i==n) {
			break;
		}
	}
}
*/


void update_centres(centre_type *centres_in,centre_index_type k, data_type *centres_positions_out)
{

	centre_update_loop: for (centre_index_type i=0; i<=k; i++) {
		#pragma AP loop_tripcount min=128 max=128 avg=128
		#pragma AP pipeline II=1
		centre_type tmp_cent = Reg(centres_in[i]);
		coord_type tmp_count = tmp_cent.count;
		if ( tmp_count == 0 )
			tmp_count = 1;

		data_type_ext tmp_wgtCent = tmp_cent.wgtCent;
		data_type tmp_new_pos;
		for (uint d=0; d<D; d++) {
			#pragma AP unroll
			coord_type_ext tmp_div_ext = (get_coord_type_vector_ext_item(tmp_wgtCent.value,d) / tmp_count); //let's see what it does with that...
			coord_type tmp_div = (coord_type) tmp_div_ext;
			#pragma AP resource variable=tmp_div core=DivnS
			set_coord_type_vector_item(&tmp_new_pos.value,Reg(tmp_div),d);
		}
		centres_positions_out[i] = tmp_new_pos;
    	if (i==k) {
    		break;
    	}
	}
}


template<uint par>void filter (node_pointer root,
			 	 	 	 	   centre_index_type k,
			 	 	 	 	   const uint p)
{
	#pragma AP function_instantiate variable=p

	centre_type centre_buffer[K];
	#pragma AP data_pack variable=centre_buffer
	#pragma AP resource variable=centre_buffer core=RAM_2P_LUTRAM

	// stack pointers
	uint stack_pointer = 0;
	uint cstack_pointer = 0;

	//stack
	stack_record stack_array[N/P]; //STACK_SIZE=N
	cstack_record_type cstack_array[N/P];
	#pragma AP resource variable=stack_array core=RAM_2P_BRAM
	#pragma AP resource variable=cstack_array core=RAM_2P_BRAM

	// heap pointers
	centre_list_pointer centre_next_free_location;
	centre_list_pointer heap_utilisation;

	// scratchpad heap
	centre_heap_type centre_heap[SCRATCHPAD_SIZE];
	centre_list_pointer centre_freelist[SCRATCHPAD_SIZE];
	#pragma AP resource variable=centre_freelist core=RAM_2P_LUTRAM
	#pragma AP resource variable=centre_heap core=RAM_2P_BRAM

	// trace buffer
	bool trace_buffer[SCRATCHPAD_SIZE];
	#pragma AP resource variable=trace_buffer core=RAM_2P_LUTRAM

	//init_stack(&stack_pointer, &cstack_pointer);

	init_allocator<centre_list_pointer>(&centre_freelist[0*SCRATCHPAD_SIZE], &centre_next_free_location, SCRATCHPAD_SIZE-2);
	heap_utilisation = 1;

	// allocate first centre list
	centre_list_pointer centre_list_idx = malloc<centre_list_pointer>(&centre_freelist[0*SCRATCHPAD_SIZE], &centre_next_free_location);
	centre_heap_type *centre_list_idx_ptr =  make_pointer<centre_heap_type>(&centre_heap[0*SCRATCHPAD_SIZE], (uint)centre_list_idx);


	// init centre buffer
	init_centre_buffer_loop: for(centre_index_type i=0; i<=k; i++) {
		#pragma AP pipeline II=1
		#pragma AP loop_tripcount min=128 max=128 avg=128
		centre_buffer[i].count = 0;
		centre_buffer[i].sum_sq = 0;
		centre_buffer[i].wgtCent.value = 0;
		if (i==k) {
			break;
		}
	}

	init_centre_list_loop: for(centre_index_type i=0; i<=k; i++) {
		#pragma AP pipeline II=1
		#pragma AP loop_tripcount min=128 max=128 avg=128
		centre_list_idx_ptr->idx[i] = i;
		if (i==k) {
			break;
		}
	}

	hls::stream<node_pointer> & u_stream_0 = u_stream.get_object(par);
	hls::stream<centre_list_pointer> & C_stream_0 = C_stream.get_object(par);
	hls::stream<centre_list_pointer> & newC_stream_0 = newC_stream.get_object(par);
	hls::stream<centre_index_type> & k_stream_0 = k_stream.get_object(par);
	hls::stream<bool> & d_stream_0 = d_stream.get_object(par);
	hls::stream<bool> & alloc_full_stream_0 = alloc_full_stream.get_object(par);
	hls::stream<bool> & alloc_full_stream2_0 = alloc_full_stream2.get_object(par);
	hls::stream<node_pointer> & u_stream2_0 = u_stream2.get_object(par);
	hls::stream<centre_list_pointer> & C_stream2_0 = C_stream2.get_object(par);
	hls::stream<centre_list_pointer> & newC_stream2_0 = newC_stream2.get_object(par);
	hls::stream<centre_index_type> & k_stream2_0 = k_stream2.get_object(par);
	hls::stream<bool> & deadend_stream_0 = deadend_stream.get_object(par);
	hls::stream<bool> & d_stream2_0 = d_stream2.get_object(par);
	hls::stream<centre_index_type> & final_index_stream_0 = final_index_stream.get_object(par);
	hls::stream<coord_type_ext> & sum_sq_stream_0 = sum_sq_stream.get_object(par);

	// must be in line with CHANNEL_DEPTH
	#pragma AP array_stream variable=u_stream_0 depth=32
	#pragma AP array_stream variable=C_stream_0 depth=32
	#pragma AP array_stream variable=newC_stream_0 depth=32
	#pragma AP array_stream variable=k_stream_0 depth=32
	#pragma AP array_stream variable=d_stream_0 depth=32
	#pragma AP array_stream variable=alloc_full_stream_0 depth=32
	#pragma AP array_stream variable=alloc_full_stream2_0 depth=32
	#pragma AP array_stream variable=u_stream2_0 depth=32
	#pragma AP array_stream variable=C_stream2_0 depth=32
	#pragma AP array_stream variable=newC_stream2_0 depth=32
	#pragma AP array_stream variable=k_stream2_0 depth=32
	#pragma AP array_stream variable=deadend_stream_0 depth=32
	#pragma AP array_stream variable=d_stream2_0 depth=32
	#pragma AP array_stream variable=final_index_stream_0 depth=32
	#pragma AP array_stream variable=sum_sq_stream_0 depth=32


	uint node_stack_length = push_node(root, &stack_pointer, stack_array);
	uint cntr_stack_length = push_centre_set(centre_list_idx,k, &cstack_pointer, cstack_array);
	bool dummy_rdy_for_deletion;
	trace_address(centre_list_idx, true, &dummy_rdy_for_deletion,trace_buffer);


	tree_search_loop: while (node_stack_length != 0) {
		#pragma AP loop_tripcount min=20359 max=32767 avg=20359

	    enum {phase0, phase1, phase2, phase3} state = phase0;

	    uint summed_k = 0;

		fetch_loop: for (uint stack_item_count=0; (stack_item_count<CHANNEL_DEPTH-1) && (node_stack_length != 0); stack_item_count++) {

			#pragma AP pipeline II=2

			#ifndef __SYNTHESIS__
				visited_nodes++;
			#endif

			node_pointer u;
			centre_list_pointer tmp_cntr_list;
			centre_list_pointer tmp_new_centre_list_idx;
			centre_index_type tmp_k;
			bool rdy_for_deletion;
			kdTree_type tmp_u;

			// fetch head of stack
			node_stack_length = pop_node(&u, &stack_pointer, stack_array);
			cntr_stack_length = pop_centre_set(&tmp_cntr_list,&tmp_k, &cstack_pointer, cstack_array);
			trace_address(tmp_cntr_list, false, &rdy_for_deletion,trace_buffer);

			// allocate new centre list
			//printf("%d ",heap_utilisation.VAL);

			#ifndef __SYNTHESIS__
				if (heap_utilisation>max_heap_utilisation) {
					max_heap_utilisation = heap_utilisation;
				}
			#endif

			bool tmp_alloc_full;
			if (heap_utilisation < SCRATCHPAD_SIZE-2) {
				tmp_new_centre_list_idx = malloc<centre_list_pointer>(&centre_freelist[0*SCRATCHPAD_SIZE], &centre_next_free_location);
				heap_utilisation++;
				tmp_alloc_full = false;
			} else {
				tmp_new_centre_list_idx = 0;
				tmp_alloc_full = true;
			}

			//printf("%d(%d) ",tmp_new_centre_list_idx.VAL,tmp_alloc_full);

			summed_k = summed_k + 2*(uint(tmp_k)+1)+2;

			//printf("%d ",tmp_k.VAL);

			u_stream_0.write_nb(u);
			C_stream_0.write_nb(tmp_cntr_list);
			newC_stream_0.write_nb(tmp_new_centre_list_idx);
			k_stream_0.write_nb(tmp_k);
			d_stream_0.write_nb(rdy_for_deletion);
			alloc_full_stream_0.write_nb(tmp_alloc_full);
		}
		//printf("\n");

		uint inner_iteration = 0;

		node_pointer u;
		centre_list_pointer tmp_cntr_list;
		centre_index_type tmp_k;
		bool rdy_for_deletion;
		bool alloc_full;
		kdTree_type tmp_u;
		data_type_ext comp_point;
		data_type tmp_centre_positions[K];
		centre_index_type tmp_centre_indices[K];
		centre_heap_type *tmp_cntr_list_ptr;
		centre_list_pointer tmp_new_centre_list_idx;
		centre_heap_type *new_centre_list_idx_ptr;
		centre_index_type tmp_final_idx;
		data_type z_star;
		coord_type_ext tmp_min_dist;
		centre_index_type new_k;
		centre_index_type tmp_new_idx;

		centre_index_type counter0;
		centre_index_type counter1;

		processing_loop: while (inner_iteration < summed_k) {

			#pragma AP pipeline II=1

			if (state == phase0) {

				u_stream_0.read_nb(u);
				C_stream_0.read_nb(tmp_cntr_list);
				newC_stream_0.read_nb(tmp_new_centre_list_idx);
				k_stream_0.read_nb(tmp_k);
				d_stream_0.read_nb(rdy_for_deletion);
				alloc_full_stream_0.read_nb(alloc_full);

				u_stream2_0.write_nb(u);
				C_stream2_0.write_nb(tmp_cntr_list);
				newC_stream2_0.write_nb(tmp_new_centre_list_idx);
				d_stream2_0.write_nb(rdy_for_deletion);
				alloc_full_stream2_0.write_nb(alloc_full);

				if (u.get_bit(NODE_POINTER_BITWIDTH-1) == false) {
					ap_uint<NODE_POINTER_BITWIDTH-1> u_short;
					u_short = u.range(NODE_POINTER_BITWIDTH-2,0);
					ap_uint<NODE_POINTER_BITWIDTH-2> u_short_short;
					u_short_short = u.range(NODE_POINTER_BITWIDTH-3,0);
					//kdTree_type *tree_node_int_memory_r = tree_node_int_memory.get_object(par);
					//kdTree_type *u_ptr = make_pointer<kdTree_type>(tree_node_int_memory_r, (uint)u_short_short);
					kdTree_type *u_ptr = make_pointer<kdTree_type>(&tree_node_int_memory[N/P*p], (uint)u_short_short);
					tmp_u = *u_ptr;
				} else {
					ap_uint<NODE_POINTER_BITWIDTH-1> u_short;
					u_short = u.range(NODE_POINTER_BITWIDTH-2,0);
					ap_uint<NODE_POINTER_BITWIDTH-2> u_short_short;
					u_short_short = u.range(NODE_POINTER_BITWIDTH-3,0);
					//kdTree_leaf_type *tree_node_leaf_memory_r = tree_node_leaf_memory.get_object(par);
					//kdTree_leaf_type *u_leaf_ptr = make_pointer<kdTree_leaf_type>(tree_node_leaf_memory_r, (uint)u_short_short);
					kdTree_leaf_type *u_leaf_ptr = make_pointer<kdTree_leaf_type>(&tree_node_leaf_memory[N/P*p], (uint)u_short_short);
					tmp_u.wgtCent = u_leaf_ptr->wgtCent;
					tmp_u.sum_sq = u_leaf_ptr->sum_sq;
					tmp_u.bnd_hi.value = 0;
					tmp_u.bnd_lo.value = 0;
					tmp_u.count = 1;
					tmp_u.left = NULL_PTR;
					tmp_u.right = NULL_PTR;
					tmp_u.midPoint.value = 0;
				}

				if ( (tmp_u.left == NULL_PTR) && (tmp_u.right == NULL_PTR) ) {
					comp_point = tmp_u.wgtCent;
				} else {
					comp_point = conv_short_to_long(tmp_u.midPoint);
				}

				tmp_cntr_list_ptr = make_pointer<centre_heap_type>(&centre_heap[0*SCRATCHPAD_SIZE], (uint)tmp_cntr_list);
				#pragma AP dependence variable=tmp_cntr_list_ptr inter false
				#pragma AP dependence variable=tmp_cntr_list inter false

				new_centre_list_idx_ptr =  make_pointer<centre_heap_type>(&centre_heap[0*SCRATCHPAD_SIZE], (uint)tmp_new_centre_list_idx);
				#pragma AP dependence variable=new_centre_list_idx_ptr inter false
				#pragma AP dependence variable=tmp_new_centre_list_idx inter false

				counter0 = 0;
				state = phase1;

			} else if (state == phase1) {


				centre_index_type tmp_idx = tmp_cntr_list_ptr->idx[counter0];
				#ifdef PARALLELISE
				data_type tmp_pos = centre_positions[tmp_idx][p];
				#else
				data_type tmp_pos = centre_positions[tmp_idx];
				#endif

				tmp_centre_indices[counter0] = tmp_idx;
				tmp_centre_positions[counter0] = tmp_pos;

				coord_type_ext tmp_dist;
				compute_distance(conv_short_to_long(tmp_pos), comp_point, &tmp_dist);

				if ((tmp_dist < tmp_min_dist) || (counter0==0)) {
					tmp_min_dist = tmp_dist;
					tmp_final_idx = tmp_idx;
					z_star = tmp_pos;
				}

				if (counter0==tmp_k) {
					new_k=(1<<CNTR_INDEX_BITWIDTH)-1;
					tmp_new_idx=0;
					counter1 = 0;
					state = phase2;
				}

				counter0++;

			} else if (state == phase2) {

				bool too_far;
				tooFar_fi(z_star, tmp_centre_positions[counter1], tmp_u.bnd_lo, tmp_u.bnd_hi, &too_far);
				if ( too_far==false ) {
					if (alloc_full == false) {
						new_centre_list_idx_ptr->idx[tmp_new_idx] = tmp_centre_indices[counter1];
					}
					tmp_new_idx++;
					new_k++;
				}

				if (counter1==tmp_k) {
					state = phase3;
				}

				counter1++;

			} else if (state == phase3) {

				// some scaling...
				data_type_ext tmp_wgtCent = tmp_u.wgtCent;
				for (uint d=0; d<D; d++) {
					#pragma AP unroll
					coord_type_ext tmp = get_coord_type_vector_ext_item(tmp_wgtCent.value,d);
					set_coord_type_vector_ext_item(&tmp_wgtCent.value,tmp >> MUL_FRACTIONAL_BITS,d);
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

				bool tmp_deadend;
				if ((new_k == 0) || ( (tmp_u.left == NULL_PTR) && (tmp_u.right == NULL_PTR) )) {
					tmp_deadend = true;
				} else {
					tmp_deadend = false;
				}

				k_stream2_0.write_nb(new_k);
				deadend_stream_0.write_nb(tmp_deadend);
				final_index_stream_0.write_nb(tmp_final_idx);
				sum_sq_stream_0.write_nb(tmp_sum_sq);

				state = phase0;
			}
			inner_iteration++;
		}

		write_back_loop: while(u_stream2_0.empty() == false) {

			#pragma AP pipeline II=3

			node_pointer u;
			centre_list_pointer tmp_cntr_list;
			bool rdy_for_deletion;
			kdTree_type tmp_u;
			bool tmp_deadend;
			bool alloc_full;
			coord_type_ext tmp_sum_sq;
			centre_index_type new_k;
			centre_list_pointer tmp_new_centre_list_idx;
			centre_index_type tmp_final_idx;

			u_stream2_0.read_nb(u);
			C_stream2_0.read_nb(tmp_cntr_list);
			newC_stream2_0.read_nb(tmp_new_centre_list_idx);
			d_stream2_0.read_nb(rdy_for_deletion);
			k_stream2_0.read_nb(new_k);
			deadend_stream_0.read_nb(tmp_deadend);
			final_index_stream_0.read_nb(tmp_final_idx);
			sum_sq_stream_0.read_nb(tmp_sum_sq);
			alloc_full_stream2_0.read_nb(alloc_full);

			// make the assertion above explicit
			if (u.get_bit(NODE_POINTER_BITWIDTH-1) == false) {
				ap_uint<NODE_POINTER_BITWIDTH-1> u_short;
				u_short = u.range(NODE_POINTER_BITWIDTH-2,0);
				ap_uint<NODE_POINTER_BITWIDTH-2> u_short_short;
				u_short_short = u.range(NODE_POINTER_BITWIDTH-3,0);
				//kdTree_type *tree_node_int_memory_r = tree_node_int_memory.get_object(par);
				//kdTree_type *u_ptr = make_pointer<kdTree_type>(tree_node_int_memory_r, (uint)u_short_short);
				kdTree_type *u_ptr = make_pointer<kdTree_type>(&tree_node_int_memory[N/P*p], (uint)u_short_short);
				tmp_u = *u_ptr;
			} else {
				ap_uint<NODE_POINTER_BITWIDTH-1> u_short;
				u_short = u.range(NODE_POINTER_BITWIDTH-2,0);
				ap_uint<NODE_POINTER_BITWIDTH-2> u_short_short;
				u_short_short = u.range(NODE_POINTER_BITWIDTH-3,0);
				//kdTree_leaf_type *tree_node_leaf_memory_r = tree_node_leaf_memory.get_object(par);
				//kdTree_leaf_type *u_leaf_ptr = make_pointer<kdTree_leaf_type>(tree_node_leaf_memory_r, (uint)u_short_short);
				kdTree_leaf_type *u_leaf_ptr = make_pointer<kdTree_leaf_type>(&tree_node_leaf_memory[N/P*p], (uint)u_short_short);
				tmp_u.wgtCent = u_leaf_ptr->wgtCent;
				tmp_u.sum_sq = u_leaf_ptr->sum_sq;
				tmp_u.bnd_hi.value = 0;
				tmp_u.bnd_lo.value = 0;
				tmp_u.count = 1;
				tmp_u.left = NULL_PTR;
				tmp_u.right = NULL_PTR;
				tmp_u.midPoint.value = 0;
			}

			// free list that has been read twice
			if (rdy_for_deletion == true) {
				free<centre_list_pointer>(&centre_freelist[0*SCRATCHPAD_SIZE], &centre_next_free_location, tmp_cntr_list);
				heap_utilisation--;
				//printf("%d ",true);
			}


			centre_type tmp_centre_buffer_item = centre_buffer[tmp_final_idx];

			// write back
			if ( tmp_deadend == true ) {
				// weighted centroid of this centre
				for (uint d=0; d<D; d++) {
					#pragma AP unroll
					coord_type_ext tmp1 = get_coord_type_vector_ext_item(tmp_centre_buffer_item.wgtCent.value,d);
					coord_type_ext tmp2 = get_coord_type_vector_ext_item(tmp_u.wgtCent.value,d);
					set_coord_type_vector_ext_item(&tmp_centre_buffer_item.wgtCent.value,Reg(tmp1)+Reg(tmp2),d);
				}
				// update number of points assigned to centre
				coord_type tmp1 =  tmp_u.count;
				coord_type tmp2 =  tmp_centre_buffer_item.count; //centre_buffer[tmp_final_centre_index].count;
				tmp_centre_buffer_item.count = Reg(tmp1) + Reg(tmp2);
				coord_type_ext tmp3 =  tmp_sum_sq;
				coord_type_ext tmp4 =  tmp_centre_buffer_item.sum_sq;
				tmp_centre_buffer_item.sum_sq  = Reg(tmp3) + Reg(tmp4);

				centre_buffer[tmp_final_idx] = (tmp_centre_buffer_item);

				if (alloc_full == false) {
					free<centre_list_pointer>(&centre_freelist[0*SCRATCHPAD_SIZE], &centre_next_free_location, tmp_new_centre_list_idx);
					heap_utilisation--;
					//printf("%d ",true);
				}

			} else {

				centre_list_pointer new_centre_list_idx;
				centre_index_type new_k_to_stack;

				if ( alloc_full == false) {
					new_centre_list_idx = tmp_new_centre_list_idx;
					new_k_to_stack = new_k;
				} else {
					new_centre_list_idx = 0;
					new_k_to_stack = k;
				}

				bool dummy_rdy_for_deletion;
				trace_address(new_centre_list_idx, true, &dummy_rdy_for_deletion, trace_buffer);

				node_pointer left_child = tmp_u.left;
				node_pointer right_child = tmp_u.right;

				// push children onto stack
				node_stack_length = push_node(right_child, &stack_pointer, stack_array);
				node_stack_length = push_node(left_child, &stack_pointer, stack_array);

				// push centre lists for both children onto stack
				cntr_stack_length = push_centre_set(new_centre_list_idx,new_k_to_stack, &cstack_pointer, cstack_array);
				cntr_stack_length = push_centre_set(new_centre_list_idx,new_k_to_stack, &cstack_pointer, cstack_array);
			}
		}
		//printf("\n");
	}

	// readout centres
	read_out_centres_loop: for(centre_index_type i=0; i<=k; i++) {
		#pragma AP pipeline II=1
		#pragma AP loop_tripcount min=128 max=128 avg=128
		#ifdef PARALLELISE
		filt_centres_out[i][p] = centre_buffer[i];
		#else
		filt_centres_out[i] = centre_buffer[i];
		#endif
		if (i==k) {
			 break;
		}
	}
}



#endif











/********************* Not quite as old **************************/

/*
 void filter (node_pointer root,
			 centre_index_type k,
             uint p)
{
	#pragma AP function_instantiate variable=p

	centre_type centre_buffer[K];
	#pragma AP data_pack variable=centre_buffer
	#pragma AP resource variable=centre_buffer core=RAM_2P_LUTRAM

	// stack pointers
	uint stack_pointer = 0;
	uint cstack_pointer = 0;

	//stack
	stack_record stack_array[N/P]; //STACK_SIZE=N
	cstack_record_type cstack_array[N/P];
	#pragma AP resource variable=stack_array core=RAM_2P_BRAM
	#pragma AP resource variable=cstack_array core=RAM_2P_BRAM

	// heap pointers
	centre_list_pointer centre_next_free_location;
	centre_list_pointer heap_utilisation;

	// scratchpad heap
	centre_heap_type centre_heap[SCRATCHPAD_SIZE];
	centre_list_pointer centre_freelist[SCRATCHPAD_SIZE];
	#pragma AP resource variable=centre_freelist core=RAM_2P_LUTRAM
	#pragma AP resource variable=centre_heap core=RAM_2P_BRAM

	// trace buffer
	bool trace_buffer[SCRATCHPAD_SIZE];
	#pragma AP resource variable=trace_buffer core=RAM_2P_LUTRAM

	//init_stack(&stack_pointer, &cstack_pointer);

	init_allocator<centre_list_pointer>(&centre_freelist[0*SCRATCHPAD_SIZE], &centre_next_free_location, SCRATCHPAD_SIZE-2);
	heap_utilisation = 1;

	// allocate first centre list
	centre_list_pointer centre_list_idx = malloc<centre_list_pointer>(&centre_freelist[0*SCRATCHPAD_SIZE], &centre_next_free_location);
	centre_heap_type *centre_list_idx_ptr =  make_pointer<centre_heap_type>(&centre_heap[0*SCRATCHPAD_SIZE], (uint)centre_list_idx);


	// init centre buffer
	init_centre_buffer_loop: for(centre_index_type i=0; i<=k; i++) {
		#pragma AP pipeline II=1
		#pragma AP loop_tripcount min=128 max=128 avg=128
		centre_buffer[i].count = 0;
		centre_buffer[i].sum_sq = 0;
		centre_buffer[i].wgtCent.value = 0;
		if (i==k) {
			break;
		}
	}

	init_centre_list_loop: for(centre_index_type i=0; i<=k; i++) {
		#pragma AP pipeline II=1
		#pragma AP loop_tripcount min=128 max=128 avg=128
		centre_list_idx_ptr->idx[i] = i;
		if (i==k) {
			break;
		}
	}

	uint node_stack_length = push_node(root, &stack_pointer, stack_array);
	uint cntr_stack_length = push_centre_set(centre_list_idx,k, &cstack_pointer, cstack_array);

	tree_search_loop: while (node_stack_length != 0) {
		#pragma AP loop_tripcount min=20359 max=32767 avg=20359
		#ifndef __SYNTHESIS__
			visited_nodes++;
		#endif


		fetch_loop: while (node_stack_length != 0) {
			node_pointer u;
			centre_list_pointer tmp_cntr_list;
			centre_index_type tmp_k;
			bool rdy_for_deletion;

			// fetch head of stack
			node_stack_length = pop_node(&u, &stack_pointer, stack_array);
			cntr_stack_length = pop_centre_set(&tmp_cntr_list,&tmp_k, &cstack_pointer, cstack_array);
			trace_address(tmp_cntr_list, false, &rdy_for_deletion,trace_buffer);

			u_stream.write_nb(u);
			C_stream.write_nb(tmp_cntr_list);
			k_stream.write_nb(tmp_k);
			d_stream.write_nb(rdy_for_deletion);
		}

		processing_loop: while (u_stream.empty()==false) {

			node_pointer u;
			centre_list_pointer tmp_cntr_list;
			centre_index_type tmp_k;
			bool rdy_for_deletion;

			u_stream.read_nb(u);
			C_stream.read_nb(tmp_cntr_list);
			k_stream.read_nb(tmp_k);
			d_stream.read_nb(rdy_for_deletion);

			// make the assertion above explicit
			kdTree_type tmp_u;
			if (u.get_bit(NODE_POINTER_BITWIDTH-1) == false) {
				ap_uint<NODE_POINTER_BITWIDTH-1> u_short;
				u_short = u.range(NODE_POINTER_BITWIDTH-2,0);
				ap_uint<NODE_POINTER_BITWIDTH-2> u_short_short;
				u_short_short = u.range(NODE_POINTER_BITWIDTH-3,0);
				kdTree_type *u_ptr = make_pointer<kdTree_type>(&tree_node_int_memory[N/P*p], (uint)u_short_short);
				tmp_u = *u_ptr;
			} else {
				ap_uint<NODE_POINTER_BITWIDTH-1> u_short;
				u_short = u.range(NODE_POINTER_BITWIDTH-2,0);
				ap_uint<NODE_POINTER_BITWIDTH-2> u_short_short;
				u_short_short = u.range(NODE_POINTER_BITWIDTH-3,0);
				kdTree_leaf_type *u_leaf_ptr = make_pointer<kdTree_leaf_type>(&tree_node_leaf_memory[N/P*p], (uint)u_short_short);
				tmp_u.wgtCent = u_leaf_ptr->wgtCent;
				tmp_u.sum_sq = u_leaf_ptr->sum_sq;
				tmp_u.bnd_hi.value = 0;
				tmp_u.bnd_lo.value = 0;
				tmp_u.count = 1;
				tmp_u.left = NULL_PTR;
				tmp_u.right = NULL_PTR;
				tmp_u.midPoint.value = 0;
			}

			data_type_ext comp_point;
			if ( (tmp_u.left == NULL_PTR) && (tmp_u.right == NULL_PTR) ) {
				comp_point = tmp_u.wgtCent;
			} else {
				comp_point = conv_short_to_long(tmp_u.midPoint);
			}

			centre_heap_type *tmp_cntr_list_ptr = make_pointer<centre_heap_type>(&centre_heap[0*SCRATCHPAD_SIZE], (uint)tmp_cntr_list);

			data_type tmp_centre_positions[K];
			centre_index_type tmp_centre_indices[K];
			bool tmp_deadend;
			kdTree_type tmp_u_out;
			centre_index_type tmp_final_centre_index;
			coord_type_ext tmp_sum_sq_out;
			centre_index_type tmp_k_out;

			// allocate new centre list
			centre_list_pointer tmp_new_centre_list_idx;
			tmp_new_centre_list_idx = malloc<centre_list_pointer>(&centre_freelist[0*SCRATCHPAD_SIZE], &centre_next_free_location);
			centre_heap_type *new_centre_list_idx_ptr =  make_pointer<centre_heap_type>(&centre_heap[0*SCRATCHPAD_SIZE], (uint)tmp_new_centre_list_idx);
			heap_utilisation++;

			centre_index_type tmp_final_idx;
			data_type z_star;
			coord_type_ext tmp_min_dist;

			minsearch_loop: for (centre_index_type i=0; i<=tmp_k; i++) {
				#pragma AP loop_tripcount min=2 max=128 avg=5
				#pragma AP pipeline II=1

				centre_index_type tmp_idx = tmp_cntr_list_ptr->idx[i];
				data_type tmp_pos = centre_positions[K*p+tmp_cntr_list_ptr->idx[i]];

				tmp_centre_indices[i] = tmp_idx;
				tmp_centre_positions[i] = tmp_pos;

				coord_type_ext tmp_dist;
				compute_distance(conv_short_to_long(tmp_pos), comp_point, &tmp_dist);

				if ((tmp_dist < tmp_min_dist) || (i==0)) {
					tmp_min_dist = tmp_dist;
					tmp_final_idx = tmp_idx;
					z_star = tmp_pos;
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
					new_centre_list_idx_ptr->idx[tmp_new_idx] = tmp_centre_indices[ i];
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
				set_coord_type_vector_ext_item(&tmp_wgtCent.value,tmp >> MUL_FRACTIONAL_BITS,d);
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
				free<centre_list_pointer>(&centre_freelist[0*SCRATCHPAD_SIZE], &centre_next_free_location, tmp_cntr_list);
				heap_utilisation--;
			}

			// write back
			if ( tmp_deadend == true ) {
				// weighted centroid of this centre
				for (uint d=0; d<D; d++) {
					#pragma AP unroll
					coord_type_ext tmp1 = get_coord_type_vector_ext_item(centre_buffer[tmp_final_centre_index].wgtCent.value,d);
					coord_type_ext tmp2 = get_coord_type_vector_ext_item(tmp_u_out.wgtCent.value,d);
					set_coord_type_vector_ext_item(&centre_buffer[tmp_final_centre_index].wgtCent.value,Reg(tmp1)+Reg(tmp2),d);
				}
				// update number of points assigned to centre
				coord_type tmp1 =  tmp_u_out.count;
				coord_type tmp2 =  centre_buffer[tmp_final_centre_index].count;
				centre_buffer[tmp_final_centre_index].count = Reg(tmp1) + Reg(tmp2);
				coord_type_ext tmp3 =  tmp_sum_sq_out;
				coord_type_ext tmp4 =  centre_buffer[tmp_final_centre_index].sum_sq;
				centre_buffer[tmp_final_centre_index].sum_sq  = Reg(tmp3) + Reg(tmp4);

				free<centre_list_pointer>(&centre_freelist[0*SCRATCHPAD_SIZE], &centre_next_free_location, tmp_new_centre_list_idx);
				heap_utilisation--;

			} else {

				centre_list_pointer new_centre_list_idx;
				centre_index_type new_k_to_stack;

				if (heap_utilisation < SCRATCHPAD_SIZE-1) {

					new_centre_list_idx = tmp_new_centre_list_idx;
					new_k_to_stack = tmp_k_out;

				} else {
					new_centre_list_idx = 0;
					new_k_to_stack = k;
					free<centre_list_pointer>(&centre_freelist[0*SCRATCHPAD_SIZE], &centre_next_free_location, tmp_new_centre_list_idx);
					heap_utilisation--;
				}
				bool dummy_rdy_for_deletion;
				trace_address(new_centre_list_idx, true, &dummy_rdy_for_deletion, trace_buffer);

				node_pointer left_child = tmp_u_out.left;
				node_pointer right_child = tmp_u_out.right;

				// push children onto stack
				node_stack_length = push_node(right_child, &stack_pointer, stack_array);
				node_stack_length = push_node(left_child, &stack_pointer, stack_array);

				// push centre lists for both children onto stack
				cntr_stack_length = push_centre_set(new_centre_list_idx,new_k_to_stack, &cstack_pointer, cstack_array);
				cntr_stack_length = push_centre_set(new_centre_list_idx,new_k_to_stack, &cstack_pointer, cstack_array);
			}
		}
	}

	// readout centres
	read_out_centres_loop: for(centre_index_type i=0; i<=k; i++) {
		#pragma AP pipeline II=1
		#pragma AP loop_tripcount min=128 max=128 avg=128
		filt_centres_out[p*K+i] = centre_buffer[i];
		if (i==k) {
			 break;
		}
	}
}

#endif

*/













/********************* Very old **********************************/

/*

#include "filtering_algorithm_top.h"
#include "filtering_algorithm_util.h"
#include "dyn_mem_alloc.h"
#include "stack.h"
#include "trace_address.h"

#include "ap_utils.h"
#include <hls_stream.h>



#ifndef __SYNTHESIS__
#include <stdio.h>
#include <stdlib.h>
#endif

#ifdef V2

// global array for the tree (keep heap local to this file)
kdTree_type tree_node_int_memory[N];
kdTree_leaf_type tree_node_leaf_memory[N];

data_type centre_positions[K];

centre_heap_type centre_heap[SCRATCHPAD_SIZE];
centre_list_pointer centre_freelist[SCRATCHPAD_SIZE];
centre_list_pointer centre_next_free_location;

centre_list_pointer heap_utilisation;

//tree search output
centre_type filt_centres_out[K];

hls::stream<node_pointer> node_pointer_stream;
hls::stream<centre_list_pointer> centre_list_pointer_stream;
hls::stream<centre_index_type> centre_list_size_stream;

hls::stream<centre_index_type> k_stream1,k_stream2,k_stream3;
hls::stream<kdTree_type> u_stream1,u_stream2,u_stream3;
hls::stream<centre_list_pointer> cntr_list_stream1,cntr_list_stream2,cntr_list_stream3;
hls::stream<centre_list_pointer> new_cntr_list_stream1;
hls::stream<data_type> centre_pos_stream;
hls::stream<centre_index_type> centre_idx_stream;
hls::stream<coord_type_ext> mindist_stream;
hls::stream<centre_index_type> newk_stream;
hls::stream<centre_index_type> final_idx_stream1,final_idx_stream2;
hls::stream<data_type> zstar_stream1,zstar_stream2;
hls::stream<bool> rdy_for_deletion_stream1,rdy_for_deletion_stream2,rdy_for_deletion_stream3;

#ifndef __SYNTHESIS__
uint visited_nodes;
#endif


void filtering_algorithm_top(	volatile kdTree_type *node_data,
								volatile node_pointer *node_address,
								volatile data_type *cntr_pos_init,
								node_pointer n,
								centre_index_type k,
								volatile node_pointer *root,
								volatile coord_type_ext *distortion_out,
								volatile data_type *clusters_out)
{
	#pragma AP interface ap_none register port=n
	#pragma AP interface ap_none register port=k
	#pragma AP interface ap_none register port=root

	#pragma AP data_pack variable=node_data
	#pragma AP data_pack variable=cntr_pos_init
	#pragma AP data_pack variable=clusters_out
	#pragma AP data_pack variable=tree_node_int_memory
	#pragma AP data_pack variable=tree_node_leaf_memory

	#pragma AP resource variable=centre_freelist core=RAM_2P_LUTRAM
	#pragma AP resource variable=centre_heap core=RAM_2P_BRAM
	#pragma AP resource variable=centre_positions core=RAM_2P_BRAM
	#pragma AP resource variable=tree_node_leaf_memory core=RAM_2P_BRAM
	#pragma AP resource variable=tree_node_int_memory core=RAM_2P_BRAM

	init_tree_node_memory(node_data,node_address,n);

	data_type new_centre_positions[K];
	node_pointer root_array[P];

	#pragma HLS array_partition variable=root_array complete

	for (uint p=0; p<P; p++) {
		root_array[p] = root[p];
	}

	//#pragma AP dataflow

	it_loop: for (uint l=0; l<L; l++) {
		#pragma AP loop_tripcount min=1 max=1 avg=1

		#ifndef __SYNTHESIS__
		visited_nodes = 0;
		#endif

		for (centre_index_type i=0; i<=k; i++) {
			#pragma AP pipeline II=1
			#pragma AP loop_tripcount min=128 max=128 avg=128

			data_type tmp_pos;
			if (l==0) {
				tmp_pos = cntr_pos_init[i];
			} else {
				tmp_pos = new_centre_positions[i];
			}
			centre_positions[i] = tmp_pos;
			if (i==k) {
				break;
			}
		}

		filter(root_array[0], k, 0);

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
		#pragma AP pipeline II=8
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




void filter(node_pointer root,
			 centre_index_type k,
             uint p)
{

	//#pragma AP inline

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

	// stack pointers
	uint stack_pointer = 0;
	uint cstack_pointer = 0;

	//stack
	stack_record stack_array[N/P]; //STACK_SIZE=N
	cstack_record_type cstack_array[N/P];
	#pragma AP resource variable=stack_array core=RAM_2P_BRAM
	#pragma AP resource variable=cstack_array core=RAM_2P_BRAM

	// trace buffer
	bool trace_buffer[SCRATCHPAD_SIZE];
	#pragma AP resource variable=trace_buffer core=RAM_2P_LUTRAM

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


	uint node_stack_length = push_node(root, &stack_pointer, stack_array);
	uint cntr_stack_length = push_centre_set(centre_list_idx,k, &cstack_pointer, cstack_array);

	main_loop: while(node_stack_length != 0) {

		uint k_counter =0;

		stack_fetch_loop: while(node_stack_length != 0) {
			#pragma AP pipeline II=1

			// fetch head of stack
			node_pointer u_1;
			centre_list_pointer tmp_cntr_list_1;
			centre_index_type tmp_k_1;

			// fetch head of stack
			node_pointer u;
			node_stack_length = pop_node(&u, &stack_pointer, stack_array);

			centre_list_pointer tmp_cntr_list;
			centre_index_type tmp_k;
			cntr_stack_length = pop_centre_set(&tmp_cntr_list_1,&tmp_k_1, &cstack_pointer, cstack_array);

			node_pointer_stream.write(u_1);
			centre_list_pointer_stream.write(tmp_cntr_list_1);
			centre_list_size_stream.write(tmp_k_1);

			k_counter++;
		}

		centre_index_type i1;
		centre_index_type i2;
		uint i3 = 0;

		bool phase_0=true;
		bool phase_1;
		bool phase_2;
		bool phase_3;

		uint counter1 = 0;

		#pragma AP array_stream variable=node_pointer_stream depth=256
		#pragma AP array_stream variable=centre_list_pointer_stream depth=256
		#pragma AP array_stream variable=centre_list_size_stream depth=256


		#pragma AP array_stream variable=centre_pos_stream depth=512
		#pragma AP array_stream variable=centre_idx_stream depth=512


		tree_search_loop: while(i3 < k_counter) {
			#pragma AP loop_tripcount min=20359 max=32767 avg=20359
			#pragma AP pipeline II=2

			#pragma AP dependence variable=node_pointer_stream inter false
			#pragma AP dependence variable=centre_list_pointer_stream inter false
			#pragma AP dependence variable=centre_list_size_stream inter false
			#pragma AP dependence variable=k_stream1 inter false
			#pragma AP dependence variable=k_stream2 inter false
			#pragma AP dependence variable=k_stream3 inter false
			#pragma AP dependence variable=u_stream1 inter false
			#pragma AP dependence variable=u_stream2 inter false
			#pragma AP dependence variable=u_stream3 inter false
			#pragma AP dependence variable=cntr_list_stream1 inter false
			#pragma AP dependence variable=cntr_list_stream2 inter false
			#pragma AP dependence variable=cntr_list_stream3 inter false
			#pragma AP dependence variable=new_cntr_list_stream1 inter false
			#pragma AP dependence variable=centre_pos_stream inter false
			#pragma AP dependence variable=centre_idx_stream inter false
			#pragma AP dependence variable=mindist_stream inter false
			#pragma AP dependence variable=newk_stream inter false
			#pragma AP dependence variable=final_idx_stream1 inter false
			#pragma AP dependence variable=final_idx_stream2 inter false
			#pragma AP dependence variable=zstar_stream1 inter false
			#pragma AP dependence variable=zstar_stream2 inter false
			#pragma AP dependence variable=rdy_for_deletion_stream1 inter false
			#pragma AP dependence variable=rdy_for_deletion_stream2 inter false
			#pragma AP dependence variable=rdy_for_deletion_stream3 inter false

			#pragma AP dependence variable=i1 inter false
			#pragma AP dependence variable=i2 inter false
			#pragma AP dependence variable=i3 inter false


			#pragma AP dependence variable=phase_0 intra true
			#pragma AP dependence variable=phase_1 intra true
			#pragma AP dependence variable=phase_2 intra true
			#pragma AP dependence variable=phase_3 intra true

			//#pragma AP dependence class=array intra true
			//#pragma AP dependence class=pointer intra true

			if (phase_0) {
			//Phase0Region: {

				static centre_index_type tmp_k;
				static node_pointer u;
				static centre_list_pointer tmp_cntr_list;
				kdTree_type tmp_u;
				#pragma AP dependence variable=tmp_k inter false
				#pragma AP dependence variable=u inter false
				#pragma AP dependence variable=tmp_cntr_list inter false
				#pragma AP dependence variable=tmp_u inter false

				//if (counter1==0) {
				node_pointer_stream.read_nb(u);
				centre_list_pointer_stream.read_nb(tmp_cntr_list);
				centre_list_size_stream.read_nb(tmp_k);
				//} else {
				//	if (counter1 == tmp_k)
				//		counter1 = 0;
				//}
				//counter1++;

				#ifndef __SYNTHESIS__
					visited_nodes++;
				#endif

				bool rdy_for_deletion;
				trace_address(tmp_cntr_list, false, &rdy_for_deletion,trace_buffer);


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

				u_stream1.write_nb(tmp_u);
				cntr_list_stream1.write_nb(tmp_cntr_list);
				k_stream1.write_nb(tmp_k);
				rdy_for_deletion_stream1.write_nb(rdy_for_deletion);

				i1=0;

				phase_0 = false;
				phase_1 = true;
				phase_2 = false;
				phase_3 = false;

			}

			if (phase_1) {
			//Phase1Region: {

				centre_index_type tmp_k;
				centre_list_pointer tmp_cntr_list;
				kdTree_type tmp_u;
				centre_heap_type *tmp_cntr_list_ptr;
				centre_index_type tmp_idx;
				data_type tmp_pos;
				data_type z_star;
				centre_index_type tmp_final_idx;
				bool rdy_for_deletion;

				#pragma AP dependence variable=tmp_k inter false
				#pragma AP dependence variable=tmp_cntr_list inter false
				#pragma AP dependence variable=tmp_u inter false
				//#pragma AP dependence variable=tmp_cntr_list_ptr inter false //!!!!
				#pragma AP dependence pointer inter false
				#pragma AP dependence variable=tmp_idx inter false
				#pragma AP dependence variable=tmp_pos inter false
				#pragma AP dependence variable=z_star inter false
				#pragma AP dependence variable=tmp_final_idx inter false
				#pragma AP dependence variable=rdy_for_deletion inter false

				if (!u_stream1.empty()) {

					u_stream1.read_nb(tmp_u);
					cntr_list_stream1.read_nb(tmp_cntr_list);
					k_stream1.read_nb(tmp_k);
					rdy_for_deletion_stream1.read_nb(rdy_for_deletion);

					u_stream2.write_nb(tmp_u);
					k_stream2.write_nb(tmp_k);
					cntr_list_stream2.write_nb(tmp_cntr_list);
					rdy_for_deletion_stream2.write_nb(rdy_for_deletion);
				}

				data_type_ext comp_point;
				if ( (tmp_u.left == NULL_PTR) && (tmp_u.right == NULL_PTR) ) {
					comp_point = tmp_u.wgtCent;
				} else {
					comp_point = conv_short_to_long(tmp_u.midPoint);
				}


				tmp_cntr_list_ptr = make_pointer<centre_heap_type>(centre_heap, (uint)tmp_cntr_list);
				tmp_idx = tmp_cntr_list_ptr->idx[i1];
				tmp_pos = centre_positions[tmp_idx];

				coord_type_ext tmp_dist;
				coord_type_ext tmp_min_dist;
				compute_distance(conv_short_to_long(tmp_pos), comp_point, &tmp_dist);

				//mindist_stream.read_nb(tmp_min_dist);

				if ((tmp_dist < tmp_min_dist) || (i1==0)) {
					tmp_min_dist = tmp_dist;
					tmp_final_idx = tmp_idx;
					z_star = tmp_pos;
				}
				//mindist_stream.write_nb(tmp_min_dist);

				centre_idx_stream.write(tmp_idx);
				centre_pos_stream.write(tmp_pos);

				if (i1==tmp_k) {
					i2=0;

					zstar_stream1.write_nb(z_star);
					final_idx_stream1.write_nb(tmp_final_idx);

					phase_0 = false;
					phase_1 = false;
					phase_2 = true;
					phase_3 = false;
				}

				i1++;
			}

			if (phase_2) {
			//Phase2Region: {

				centre_index_type tmp_k;
				centre_index_type tmp_final_idx;
				centre_list_pointer tmp_cntr_list;
				kdTree_type tmp_u;
				centre_index_type new_k;
				centre_index_type tmp_new_idx;
				centre_index_type tmp_idx;
				data_type tmp_pos;
				data_type z_star;
				centre_list_pointer new_centre_list_idx;
				centre_heap_type *new_centre_list_idx_ptr;
				bool rdy_for_deletion;

				#pragma AP dependence variable=tmp_k inter false
				#pragma AP dependence variable=tmp_final_idx inter false
				#pragma AP dependence variable=tmp_cntr_list inter false
				#pragma AP dependence variable=tmp_u inter false
				#pragma AP dependence variable=new_k inter false
				#pragma AP dependence variable=tmp_new_idx inter false
				#pragma AP dependence variable=tmp_idx inter false
				#pragma AP dependence variable=tmp_pos inter false
				#pragma AP dependence variable=z_star inter false
				#pragma AP dependence variable=new_centre_list_idx inter false
				//#pragma AP dependence variable=new_centre_list_idx_ptr inter false
				#pragma AP dependence pointer inter false
				#pragma AP dependence variable=rdy_for_deletion inter false

				if (!u_stream2.empty()) {

					u_stream2.read_nb(tmp_u);
					k_stream2.read_nb(tmp_k);
					zstar_stream1.read_nb(z_star);
					final_idx_stream1.read_nb(tmp_final_idx);
					cntr_list_stream2.read_nb(tmp_cntr_list);
					rdy_for_deletion_stream2.read_nb(rdy_for_deletion);

					new_k=(1<<CNTR_INDEX_BITWIDTH)-1;
					tmp_new_idx=0;

					new_centre_list_idx = malloc<centre_list_pointer>(centre_freelist, &centre_next_free_location);
					new_centre_list_idx_ptr =  make_pointer<centre_heap_type>(centre_heap, (uint)new_centre_list_idx);
					heap_utilisation++;

					u_stream3.write_nb(tmp_u);
					zstar_stream2.write_nb(z_star);
					final_idx_stream2.write_nb(tmp_final_idx);
					k_stream3.write_nb(tmp_k);
					new_cntr_list_stream1.write_nb(new_centre_list_idx);
					cntr_list_stream3.write_nb(tmp_cntr_list);
					rdy_for_deletion_stream3.write_nb(rdy_for_deletion);
				}

				centre_idx_stream.read(tmp_idx);
				centre_pos_stream.read(tmp_pos);

				bool too_far;
				tooFar_fi(z_star,tmp_pos, tmp_u.bnd_lo, tmp_u.bnd_hi, &too_far);
				if ( too_far==false ) {
					centre_heap[new_centre_list_idx].idx[tmp_new_idx] = tmp_idx;
					tmp_new_idx++;
					new_k++;
				}

				if (i2 == tmp_k) {
					phase_0 = false;
					phase_1 = false;
					phase_2 = false;
					phase_3 = true;
					newk_stream.write_nb(new_k);
				}
				i2++;
			}

			if (phase_3) {
			//Phase3Region: {

				kdTree_type tmp_u;
				centre_index_type tmp_k;
				data_type z_star;
				centre_index_type new_k;
				centre_index_type tmp_final_idx;
				centre_list_pointer new_centre_list_idx;
				centre_list_pointer tmp_cntr_list;
				bool rdy_for_deletion;

				#pragma AP dependence variable=tmp_u inter false
				#pragma AP dependence variable=tmp_k inter false
				#pragma AP dependence variable=z_star inter false
				#pragma AP dependence variable=new_k inter false
				#pragma AP dependence variable=tmp_final_idx inter false
				#pragma AP dependence variable=new_centre_list_idx inter false
				#pragma AP dependence variable=tmp_cntr_list inter false
				#pragma AP dependence variable=rdy_for_deletion inter false

				if (!u_stream3.empty()) {

					u_stream3.read_nb(tmp_u);
					k_stream3.read_nb(tmp_k);
					zstar_stream2.read_nb(z_star);
					newk_stream.read_nb(new_k);
					final_idx_stream2.read_nb(tmp_final_idx);
					new_cntr_list_stream1.read_nb(new_centre_list_idx);
					cntr_list_stream3.read_nb(tmp_cntr_list);
					rdy_for_deletion_stream3.read_nb(rdy_for_deletion);
				}

				data_type_ext tmp_wgtCent;
				for (uint d=0; d<D; d++) {
					#pragma AP unroll
					coord_type_ext tmp = get_coord_type_vector_ext_item(tmp_u.wgtCent.value,d);
					set_coord_type_vector_ext_item(&tmp_wgtCent.value,tmp >> MUL_FRACTIONAL_BITS,d);
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

				bool tmp_deadend;
				if ((new_k == 0) || ( (tmp_u.left == NULL_PTR) && (tmp_u.right == NULL_PTR) )) {
					tmp_deadend = true;
				} else {
					tmp_deadend = false;
				}

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
						coord_type_ext tmp1 = get_coord_type_vector_ext_item(centre_buffer[tmp_final_idx].wgtCent.value,d);
						coord_type_ext tmp2 = get_coord_type_vector_ext_item(tmp_u.wgtCent.value,d);
						set_coord_type_vector_ext_item(&centre_buffer[tmp_final_idx].wgtCent.value,tmp1+tmp2,d);
					}
					// update number of points assigned to centre
					coord_type tmp1 =  tmp_u.count;
					coord_type tmp2 =  centre_buffer[tmp_final_idx].count;
					centre_buffer[tmp_final_idx].count = tmp1 + tmp2;
					coord_type_ext tmp3 =  tmp_sum_sq;
					coord_type_ext tmp4 =  centre_buffer[tmp_final_idx].sum_sq;
					centre_buffer[tmp_final_idx].sum_sq  = tmp3 + tmp4;

					free<centre_list_pointer>(centre_freelist, &centre_next_free_location, new_centre_list_idx);
					heap_utilisation--;


				} else {

					centre_index_type new_k_to_stack;

					if (heap_utilisation < SCRATCHPAD_SIZE-1) {

						new_k_to_stack = new_k;

					} else {
						free<centre_list_pointer>(centre_freelist, &centre_next_free_location, new_centre_list_idx);
						heap_utilisation--;
						new_centre_list_idx = 0;
						new_k_to_stack = k;
					}


					bool dummy_rdy_for_deletion;
					trace_address(new_centre_list_idx, true, &dummy_rdy_for_deletion,trace_buffer);

					node_pointer left_child = tmp_u.left;
					node_pointer right_child = tmp_u.right;

					// push children onto stack
					node_stack_length = push_node(right_child, &stack_pointer, stack_array);
					node_stack_length = push_node(left_child, &stack_pointer, stack_array);

					// push centre lists for both children onto stack
					cntr_stack_length = push_centre_set(new_centre_list_idx,new_k_to_stack, &cstack_pointer, cstack_array);
					cntr_stack_length = push_centre_set(new_centre_list_idx,new_k_to_stack, &cstack_pointer, cstack_array);
				}
				phase_0 = true;
				phase_1 = false;
				phase_2 = false;
				phase_3 = false;
				i3++;
			}

		}
	}

	// readout centres
	read_out_centres_loop: for(centre_index_type i=0; i<=k; i++) {
		#pragma AP pipeline II=1
		#pragma AP loop_tripcount min=128 max=128 avg=128
		filt_centres_out[p*K+i] = centre_buffer[i];
		if (i==k) {
			 break;
		}
	}
}


#endif

*/
