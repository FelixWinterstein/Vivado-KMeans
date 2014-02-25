/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: filtering_algorithm_top.cpp
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/


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


/*****************************************************
 * NOTE:
 * THIS FILE CONTAINS TWO VERSIONS OF CODE (ENABLED VIA #define OPTIMISED_VERSION in filtering_algorithm_top.h)
 * THE ORIGINAL VERSION IS A STRAIGHTFORWARD IMPLEMENTATION OF THE FILTERING ALGORITHM,
 * BUT IT IS NOT PARALLELISABLE AND THE RESULTING SYNTHESISED RTL DESIGN IS BY FAR NOT AS EFFICIENT AS ITS HAND-WRITTEN COUNTERPART.
 * THE OPTIMISED VERSION IS CONTAINS SUBSTANTIAL MODIFICATION AND IS MORE DIFFICULT TO READ. THESE ARE:
 * - MANUAL PARTITIONING OF THE TREE, CENTRE LISTS, AND STACK DATA STRUCTURES
 * - MANUAL INSTANTIATION OF PARALLEL INSTANCES OF FUNCTION filter
 * - MANUAL FLATTENING OF THE MAIN TREE SEARCH LOOP IN FUNCTION filter USING A STATE MACHINE
 * - MANUAL LOOP DISTRIBUTION WITHIN THE MAIN TREE SEARCH LOOP AND MANUAL INSERTION OF FIFO STREAMING BUFFERS FOR EFFICIENT PIPELINING
 * SEE THE FPT2013 PAPER FOR MORE INFORMATION
 ****************************************************/



#ifdef OPTIMISED_VERSION

/********************** optimised version *******************************/



#ifndef PARALLELISE

// tree memory (leafs and intermediate nodes)
kdTree_type tree_node_int_memory[HEAP_SIZE/2];
kdTree_leaf_type tree_node_leaf_memory[HEAP_SIZE/2];
// centre positions
data_type centre_positions[K];
// tree search output
centre_type filt_centres_out[K];

#else // parallelisation with degree P
// tree memory
kdTree_type tree_node_int_memory[P][HEAP_SIZE/2/P];
kdTree_leaf_type tree_node_leaf_memory[P][HEAP_SIZE/2/P];
// centre positions
data_type centre_positions[K][P];
//tree search output
centre_type filt_centres_out[K][P];

#endif

// define stream type (it will synthesise into a fifo buffer)
// we currently support a max P of 4, i.e. get_object statically selects s0-s3
// the reason for the beast below is the fact that I had trouble defining an array of streams
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

// instantiate streams used in function filter (they have to be declared globally)
// these will synthesise into fifo buffers
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

// some statistics collected during C simulation
#ifndef __SYNTHESIS__
uint visited_nodes;
uint max_heap_utilisation;
#endif


// top-level function
void filtering_algorithm_top(   volatile kdTree_type *node_data,
                                volatile node_pointer *node_address,
                                volatile data_type *cntr_pos_init,
                                node_pointer n,
                                centre_index_type k,
                                volatile node_pointer *root,
                                volatile coord_type_ext *distortion_out,
                                volatile data_type *clusters_out)
{
    // set the interface properties
    #pragma HLS interface ap_none register port=n
    #pragma HLS interface ap_none register port=k
    #pragma HLS interface ap_fifo port=node_data depth=256
    #pragma HLS interface ap_fifo port=node_address depth=256
    #pragma HLS interface ap_fifo port=cntr_pos_init depth=256
    #pragma HLS interface ap_fifo port=root depth=256
    #pragma HLS interface ap_fifo port=distortion_out depth=256
    #pragma HLS interface ap_fifo port=clusters_out depth=256

    // we have P roots for P sub-trees
    node_pointer root_array[P];

    // pack all items of a struct into a single data word
    #pragma HLS data_pack variable=node_data
    /*
    #pragma HLS data_pack variable=cntr_pos_init
    #pragma HLS data_pack variable=clusters_out
    */
    #pragma HLS data_pack variable=tree_node_int_memory
    #pragma HLS data_pack variable=tree_node_leaf_memory
    //#pragma HLS data_pack variable=centre_heap
    #pragma HLS data_pack variable=filt_centres_out

    // specify the type of memory instantiated for these arrays: two-port block ram
    #pragma HLS resource variable=centre_positions core=RAM_2P_BRAM
    #pragma HLS resource variable=tree_node_leaf_memory core=RAM_1P_BRAM
    #pragma HLS resource variable=tree_node_int_memory core=RAM_1P_BRAM

    #pragma HLS array_partition variable=root_array complete

    // load root pointers into internal registers
    for (uint p=0; p<P; p++) {
        root_array[p] = root[p];
    }

    // load kd-tree data from interface into internal memory
    init_tree_node_memory(node_data,node_address,n);

    #ifdef PARALLELISE
        centre_type filt_centres_out_reduced[K];
        #pragma HLS data_pack variable=filt_centres_out_reduced

        #pragma HLS array_partition variable=tree_node_int_memory complete dim=1
        #pragma HLS array_partition variable=tree_node_leaf_memory complete dim=1
        #pragma HLS array_partition variable=centre_positions complete dim=2
        #pragma HLS array_partition variable=filt_centres_out complete dim=2

    #endif

    data_type new_centre_positions[K];

    // iterate over a constant number of outer clustering iterations
    it_loop: for (uint l=0; l<L; l++) {

        #ifndef __SYNTHESIS__
        visited_nodes = 0;
        max_heap_utilisation = 0;
        #endif

        // in the first iteration, load centre_positions from the interface, otherwise from new_centre_positions
        for (centre_index_type i=0; i<=k; i++) {
            #ifndef PARALLELISE
                data_type tmp_pos;
                if (l==0) {
                    tmp_pos.value = cntr_pos_init[i].value;
                } else {
                    tmp_pos.value = new_centre_positions[i].value;
                }
                centre_positions[i] = tmp_pos;
            #else
                data_type tmp_pos;
                if (l==0) {
                    tmp_pos.value = cntr_pos_init[i].value;
                } else {
                    tmp_pos.value = new_centre_positions[i].value;
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

        // run the clustering kernel (tree traversal)
        for (uint p=0; p<P; p++) {
            #pragma HLS unroll
            if (p == 0) {
                filter<0>(root_array[p],k);
            }
            #if P>1
            if (p == 1) {
                filter<1>(root_array[p],k);
            }
            #endif
            #if P>2
            if (p == 2) {
                filter<2>(root_array[p],k);
            }
            #endif
            #if P>3
            if (p == 3) {
                filter<3>(root_array[p],k);
            }
            #endif
            // help the scheduler by declaring inter-iteration dependencies for these variables as false
            #pragma HLS dependence variable=filt_centres_out inter false
            #pragma HLS dependence variable=centre_positions inter false
            #pragma HLS dependence variable=tree_node_leaf_memory inter false
            #pragma HLS dependence variable=tree_node_int_memory inter false
            #pragma HLS dependence variable=root_array inter false
            // help the scheduler by declaring inter-iteration dependencies for these variables as false
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

        // if we have parallel tree searches, we need to perform a reduction after all units are done
        #ifdef PARALLELISE

            for(centre_index_type i=0; i<=k; i++) {
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
        printf("Visited nodes: %d, max. scatchpad heap utilisation: %d\n",visited_nodes,max_heap_utilisation);
        #endif

        // re-init centre positions
        #ifndef PARALLELISE
            update_centres(filt_centres_out, k, new_centre_positions);
        #else
            update_centres(filt_centres_out_reduced, k, new_centre_positions);
        #endif
    }


    // write clustering output: new cluster centres and distortion
    output_loop: for (centre_index_type i=0; i<=k; i++) {
        #pragma HLS pipeline II=1

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
    #pragma HLS inline

    init_nodes_loop: for (node_pointer i=0; i<=n; i++) {
        #pragma HLS pipeline II=8 // todo: find out why we can't set it to one here
        node_pointer tmp_node_address = node_address[i];
        kdTree_type tmp_node;
        tmp_node = node_data[i];

        bool leaf_node = tmp_node_address.get_bit(NODE_POINTER_BITWIDTH-1);
        #ifdef PARALLELISE
        ap_uint<8> bank_selector = tmp_node_address.range(NODE_POINTER_BITWIDTH-2,NODE_POINTER_BITWIDTH-2-ceil(log2(P))+1);
        #else
        ap_uint<8> bank_selector = 0;
        #endif
        ap_uint<NODE_POINTER_BITWIDTH-1-0> tmp_node_address_short;
        tmp_node_address_short = tmp_node_address.range(NODE_POINTER_BITWIDTH-2-ceil(log2(P)),0);

        if (leaf_node == false) {
            #ifdef PARALLELISE
            kdTree_type *tree_node_int_memory_r = tree_node_int_memory[bank_selector];
            tree_node_int_memory_r[ tmp_node_address_short].bnd_hi.value = tmp_node.bnd_hi.value;
            tree_node_int_memory_r[ tmp_node_address_short].bnd_lo.value = tmp_node.bnd_lo.value;
            tree_node_int_memory_r[ tmp_node_address_short].count = tmp_node.count;
            tree_node_int_memory_r[ tmp_node_address_short].midPoint.value = tmp_node.midPoint.value;
            tree_node_int_memory_r[ tmp_node_address_short].wgtCent.value = tmp_node.wgtCent.value;
            tree_node_int_memory_r[ tmp_node_address_short].sum_sq = tmp_node.sum_sq;
            tree_node_int_memory_r[ tmp_node_address_short].left = tmp_node.left;
            tree_node_int_memory_r[ tmp_node_address_short].right = tmp_node.right;
            #else
            tree_node_int_memory[ tmp_node_address_short].bnd_hi.value = tmp_node.bnd_hi.value;
            tree_node_int_memory[ tmp_node_address_short].bnd_lo.value = tmp_node.bnd_lo.value;
            tree_node_int_memory[ tmp_node_address_short].count = tmp_node.count;
            tree_node_int_memory[ tmp_node_address_short].midPoint.value = tmp_node.midPoint.value;
            tree_node_int_memory[ tmp_node_address_short].wgtCent.value = tmp_node.wgtCent.value;
            tree_node_int_memory[ tmp_node_address_short].sum_sq = tmp_node.sum_sq;
            tree_node_int_memory[ tmp_node_address_short].left = tmp_node.left;
            tree_node_int_memory[ tmp_node_address_short].right = tmp_node.right;
            #endif

        } else {
            #ifdef PARALLELISE
            kdTree_leaf_type *tree_node_leaf_memory_r = tree_node_leaf_memory[bank_selector];
            tree_node_leaf_memory_r[ tmp_node_address_short].wgtCent.value =  tmp_node.wgtCent.value;
            tree_node_leaf_memory_r[ tmp_node_address_short].sum_sq =  tmp_node.sum_sq;
            #else
            tree_node_leaf_memory[ tmp_node_address_short].wgtCent.value =  tmp_node.wgtCent.value;
            tree_node_leaf_memory[ tmp_node_address_short].sum_sq =  tmp_node.sum_sq;
            #endif
        }

        if (i==n) {
            break;
        }
    }
}



// update the new centre positions after one outer clustering iteration
void update_centres(centre_type *centres_in,centre_index_type k, data_type *centres_positions_out)
{

    centre_update_loop: for (centre_index_type i=0; i<=k; i++) {
        #pragma HLS pipeline II=1
        centre_type tmp_cent = Reg(centres_in[i]);
        coord_type tmp_count = tmp_cent.count;
        if ( tmp_count == 0 )
            tmp_count = 1;

        data_type_ext tmp_wgtCent = tmp_cent.wgtCent;
        data_type tmp_new_pos;
        for (uint d=0; d<D; d++) {
            #pragma HLS unroll
            coord_type_ext tmp_div_ext = (get_coord_type_vector_ext_item(tmp_wgtCent.value,d) / tmp_count); //let's see what it does with that...
            coord_type tmp_div = (coord_type) tmp_div_ext;
            #pragma HLS resource variable=tmp_div core=DivnS
            set_coord_type_vector_item(&tmp_new_pos.value,Reg(tmp_div),d);
        }
        centres_positions_out[i] = tmp_new_pos;
        if (i==k) {
            break;
        }
    }
}


// main clustering kernel
template<uint par>void filter (node_pointer root,
                               centre_index_type k)
{

    centre_type centre_buffer[K];
    #pragma HLS data_pack variable=centre_buffer
    #pragma HLS resource variable=centre_buffer core=RAM_2P_LUTRAM

    // stack pointers
    uint stack_pointer = 0;
    uint cstack_pointer = 0;

    //stack
    stack_record stack_array[N/P];
    cstack_record_type cstack_array[N/P];

    // specify the type of memory instantiated for these arrays: two-port block ram
    #pragma HLS resource variable=stack_array core=RAM_2P_BRAM
    #pragma HLS resource variable=cstack_array core=RAM_2P_BRAM

    // scratchpad heap pointers
    centre_list_pointer centre_next_free_location;
    centre_list_pointer heap_utilisation;

    // scratchpad heap
    centre_heap_type centre_heap[SCRATCHPAD_SIZE];
    centre_list_pointer centre_freelist[SCRATCHPAD_SIZE];
    // specify the type of memory instantiated for these arrays: two-port lut ram and block ram
    #pragma HLS resource variable=centre_freelist core=RAM_2P_LUTRAM
    #pragma HLS resource variable=centre_heap core=RAM_2P_BRAM

    // trace buffer (mark a heap address if it was read for the second time)
    bool trace_buffer[SCRATCHPAD_SIZE];
    #pragma HLS resource variable=trace_buffer core=RAM_2P_LUTRAM

    // init the dynamic memory allocator for scratchpad heap
    init_allocator<centre_list_pointer>(&centre_freelist[0*SCRATCHPAD_SIZE], &centre_next_free_location, SCRATCHPAD_SIZE-2);
    heap_utilisation = 1;

    // allocate first centre list
    centre_list_pointer centre_list_idx = malloc<centre_list_pointer>(&centre_freelist[0*SCRATCHPAD_SIZE], &centre_next_free_location);
    centre_heap_type *centre_list_idx_ptr =  make_pointer<centre_heap_type>(&centre_heap[0*SCRATCHPAD_SIZE], (uint)centre_list_idx);

    // init centre buffer
    init_centre_buffer_loop: for(centre_index_type i=0; i<=k; i++) {
        #pragma HLS pipeline II=1
        centre_buffer[i].count = 0;
        centre_buffer[i].sum_sq = 0;
        centre_buffer[i].wgtCent.value = 0;
        if (i==k) {
            break;
        }
    }

    // init allocated centre list
    init_centre_list_loop: for(centre_index_type i=0; i<=k; i++) {
        #pragma HLS pipeline II=1
        centre_list_idx_ptr->idx[i] = i;
        if (i==k) {
            break;
        }
    }

    // set references to the globally declared stream variables (according to parameter par)
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

    // define the depth of the internal streaming fifo buffers, this must be in line with CHANNEL_DEPTH
    #pragma HLS array_stream variable=u_stream_0 depth=32
    #pragma HLS array_stream variable=C_stream_0 depth=32
    #pragma HLS array_stream variable=newC_stream_0 depth=32
    #pragma HLS array_stream variable=k_stream_0 depth=32
    #pragma HLS array_stream variable=d_stream_0 depth=32
    #pragma HLS array_stream variable=alloc_full_stream_0 depth=32
    #pragma HLS array_stream variable=alloc_full_stream2_0 depth=32
    #pragma HLS array_stream variable=u_stream2_0 depth=32
    #pragma HLS array_stream variable=C_stream2_0 depth=32
    #pragma HLS array_stream variable=newC_stream2_0 depth=32
    #pragma HLS array_stream variable=k_stream2_0 depth=32
    #pragma HLS array_stream variable=deadend_stream_0 depth=32
    #pragma HLS array_stream variable=d_stream2_0 depth=32
    #pragma HLS array_stream variable=final_index_stream_0 depth=32
    #pragma HLS array_stream variable=sum_sq_stream_0 depth=32

    // push pointers to tree root node and first centre list onto the stack
    uint node_stack_length = push_node(root, &stack_pointer, stack_array);
    uint cntr_stack_length = push_centre_set(centre_list_idx,k, &cstack_pointer, cstack_array);

    // mark scratchpad heap address as written
    bool dummy_rdy_for_deletion;
    trace_address(centre_list_idx, true, &dummy_rdy_for_deletion,trace_buffer);


    // main tree search loop
    tree_search_loop: while (node_stack_length != 0) {

        // init state machine
        enum {phase0, phase1, phase2, phase3} state = phase0;

        uint summed_k = 0;

        // pop CHANNEL_DEPTH items from stack (if there are) and store the data in the fifo buffers
        fetch_loop: for (uint stack_item_count=0; (stack_item_count<CHANNEL_DEPTH-1) && (node_stack_length != 0); stack_item_count++) {

            #pragma HLS pipeline II=2

            #ifndef __SYNTHESIS__
                visited_nodes++;
            #endif

            // local vars
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

            // some statistics
            #ifndef __SYNTHESIS__
                if (heap_utilisation>max_heap_utilisation) {
                    max_heap_utilisation = heap_utilisation;
                }
            #endif

            // allocate a new centre list if scratch heap isn't full
            bool tmp_alloc_full;
            if (heap_utilisation < SCRATCHPAD_SIZE-2) {
                tmp_new_centre_list_idx = malloc<centre_list_pointer>(&centre_freelist[0*SCRATCHPAD_SIZE], &centre_next_free_location);
                heap_utilisation++;
                tmp_alloc_full = false;
            } else {
                tmp_new_centre_list_idx = 0;
                tmp_alloc_full = true;
            }

            // calculate the number of iterations of the following loop
            summed_k = summed_k + 2*(uint(tmp_k)+1)+2;

            // write data into fifo buffers (to be used by the following loop)
            u_stream_0.write_nb(u);
            C_stream_0.write_nb(tmp_cntr_list);
            newC_stream_0.write_nb(tmp_new_centre_list_idx);
            k_stream_0.write_nb(tmp_k);
            d_stream_0.write_nb(rdy_for_deletion);
            alloc_full_stream_0.write_nb(tmp_alloc_full);
        }

        // declare local vars
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

        // main processing loop: retrieve data from the fifo buffers and process it
        processing_loop: while (inner_iteration < summed_k) {

            #pragma HLS pipeline II=1

            if (state == phase0) {

                // read data from fifo buffers (stored in the previous loop)
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

                // if MSB if the node pointer is not set, read from intermediate nodes memory,
                // read from leaf nodes memory otherwise
                if (u.get_bit(NODE_POINTER_BITWIDTH-1) == false) {

                    ap_uint<NODE_POINTER_BITWIDTH-1-0> u_short;
                    u_short = u.range(NODE_POINTER_BITWIDTH-2-uint(ceil(log2(P))),0);
                    #ifdef PARALLELISE
                    kdTree_type *u_ptr = make_pointer<kdTree_type>(tree_node_int_memory[par], (uint)u_short);
                    #else
                    kdTree_type *u_ptr = make_pointer<kdTree_type>(tree_node_int_memory, (uint)u_short);
                    #endif
                    tmp_u = *u_ptr;
                } else {

                    ap_uint<NODE_POINTER_BITWIDTH-1-0> u_short;
                    u_short = u.range(NODE_POINTER_BITWIDTH-2-uint(ceil(log2(P))),0);
                    #ifdef PARALLELISE
                    kdTree_leaf_type *u_leaf_ptr = make_pointer<kdTree_leaf_type>(tree_node_leaf_memory[par], (uint)u_short);
                    #else
                    kdTree_leaf_type *u_leaf_ptr = make_pointer<kdTree_leaf_type>(tree_node_leaf_memory, (uint)u_short);
                    #endif
                    tmp_u.wgtCent = u_leaf_ptr->wgtCent;
                    tmp_u.sum_sq = u_leaf_ptr->sum_sq;
                    // set all fields irrelevant for leaf nodes to zero
                    tmp_u.bnd_hi.value = 0;
                    tmp_u.bnd_lo.value = 0;
                    tmp_u.count = 1;
                    tmp_u.left = NULL_PTR;
                    tmp_u.right = NULL_PTR;
                    tmp_u.midPoint.value = 0;
                }

                // leaf node?
                if ( (tmp_u.left == NULL_PTR) && (tmp_u.right == NULL_PTR) ) {
                    comp_point = tmp_u.wgtCent;
                } else {
                    comp_point = conv_short_to_long(tmp_u.midPoint);
                }

                // get pointer to centre list
                tmp_cntr_list_ptr = make_pointer<centre_heap_type>(&centre_heap[0*SCRATCHPAD_SIZE], (uint)tmp_cntr_list);
                #pragma HLS dependence variable=tmp_cntr_list_ptr inter false
                #pragma HLS dependence variable=tmp_cntr_list inter false

                // get pointer to newly allocated centre list (still to be filled with content)
                new_centre_list_idx_ptr =  make_pointer<centre_heap_type>(&centre_heap[0*SCRATCHPAD_SIZE], (uint)tmp_new_centre_list_idx);
                #pragma HLS dependence variable=new_centre_list_idx_ptr inter false
                #pragma HLS dependence variable=tmp_new_centre_list_idx inter false

                counter0 = 0;
                state = phase1;

            } else if (state == phase1) {

                // select centre by centre in this phase
                centre_index_type tmp_idx = tmp_cntr_list_ptr->idx[counter0];
                #ifdef PARALLELISE
                data_type tmp_pos = centre_positions[tmp_idx][par];
                #else
                data_type tmp_pos = centre_positions[tmp_idx];
                #endif

                // write back
                tmp_centre_indices[counter0] = tmp_idx;
                tmp_centre_positions[counter0] = tmp_pos;

                // compute Euclidean distance between centre and tree node's bounding box mid point
                coord_type_ext tmp_dist;
                compute_distance(conv_short_to_long(tmp_pos), comp_point, &tmp_dist);

                // find the centre with smalles distance
                if ((tmp_dist < tmp_min_dist) || (counter0==0)) {
                    tmp_min_dist = tmp_dist;
                    tmp_final_idx = tmp_idx;
                    z_star = tmp_pos;
                }

                // state transition
                if (counter0==tmp_k) {
                    new_k=(1<<CNTR_INDEX_BITWIDTH)-1;
                    tmp_new_idx=0;
                    counter1 = 0;
                    state = phase2;
                }

                counter0++;

            } else if (state == phase2) {

                // determine whether a sub-tree will be pruned
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
                    #pragma HLS unroll
                    coord_type_ext tmp = get_coord_type_vector_ext_item(tmp_wgtCent.value,d);
                    set_coord_type_vector_ext_item(&tmp_wgtCent.value,tmp >> MUL_FRACTIONAL_BITS,d);
                }

                // z_star == tmp_centre_positions[idx_closest] !
                // update sum_sq of centre (to calculate the distortion)
                coord_type_ext tmp1_2, tmp2_2;
                data_type_ext tmp_z_star = conv_short_to_long(z_star);
                dot_product(tmp_z_star,tmp_wgtCent,&tmp1_2);
                dot_product(tmp_z_star,tmp_z_star ,&tmp2_2);
                coord_type_ext tmp1, tmp2;
                tmp1 = tmp1_2<<1;
                tmp2 = tmp2_2>>MUL_FRACTIONAL_BITS;
                // this looks ugly, but Vivado wants it this way
                coord_type tmp_count = tmp_u.count;
                coord_type_ext tmp2_sat = saturate_mul_input(tmp2);
                coord_type_ext tmp_count_sat = saturate_mul_input(tmp_count);
                coord_type_ext tmp3 = tmp2_sat*tmp_count_sat;
                coord_type_ext tmp_sum_sq1 = tmp_u.sum_sq+tmp3;
                coord_type_ext tmp_sum_sq = tmp_sum_sq1-tmp1;
                #pragma HLS resource variable=tmp3 core=MulnS

                // final decision whether sub-tree will be pruned
                bool tmp_deadend;
                if ((new_k == 0) || ( (tmp_u.left == NULL_PTR) && (tmp_u.right == NULL_PTR) )) {
                    tmp_deadend = true;
                } else {
                    tmp_deadend = false;
                }

                // write output into fifo buffer and go back to phase0
                k_stream2_0.write_nb(new_k);
                deadend_stream_0.write_nb(tmp_deadend);
                final_index_stream_0.write_nb(tmp_final_idx);
                sum_sq_stream_0.write_nb(tmp_sum_sq);

                state = phase0;
            }
            inner_iteration++;
        }

        write_back_loop: while(u_stream2_0.empty() == false) {

            #pragma HLS pipeline II=3

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

            // read fifo buffers filled by previous loop
            u_stream2_0.read_nb(u);
            C_stream2_0.read_nb(tmp_cntr_list);
            newC_stream2_0.read_nb(tmp_new_centre_list_idx);
            d_stream2_0.read_nb(rdy_for_deletion);
            k_stream2_0.read_nb(new_k);
            deadend_stream_0.read_nb(tmp_deadend);
            final_index_stream_0.read_nb(tmp_final_idx);
            sum_sq_stream_0.read_nb(tmp_sum_sq);
            alloc_full_stream2_0.read_nb(alloc_full);

            // if MSB if the node pointer is not set, read from intermediate nodes memory,
            // read from leaf nodes memory otherwise
            if (u.get_bit(NODE_POINTER_BITWIDTH-1) == false) {

                ap_uint<NODE_POINTER_BITWIDTH-1-0> u_short;
                u_short = u.range(NODE_POINTER_BITWIDTH-2-uint(ceil(log2(P))),0);
                #ifdef PARALLELISE
                kdTree_type *u_ptr = make_pointer<kdTree_type>(tree_node_int_memory[par], (uint)u_short);
                #else
                kdTree_type *u_ptr = make_pointer<kdTree_type>(tree_node_int_memory, (uint)u_short);
                #endif
                tmp_u = *u_ptr;
            } else {

                ap_uint<NODE_POINTER_BITWIDTH-1-0> u_short;
                u_short = u.range(NODE_POINTER_BITWIDTH-2-uint(ceil(log2(P))),0);
                #ifdef PARALLELISE
                kdTree_leaf_type *u_leaf_ptr = make_pointer<kdTree_leaf_type>(tree_node_leaf_memory[par], (uint)u_short);
                #else
                kdTree_leaf_type *u_leaf_ptr = make_pointer<kdTree_leaf_type>(tree_node_leaf_memory, (uint)u_short);
                #endif
                tmp_u.wgtCent = u_leaf_ptr->wgtCent;
                tmp_u.sum_sq = u_leaf_ptr->sum_sq;
                // set all fields irrelevant for leaf nodes to zero
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
            }

            centre_type tmp_centre_buffer_item = centre_buffer[tmp_final_idx];

            // write back (update centre buffer)
            if ( tmp_deadend == true ) {
                // weighted centroid of this centre
                for (uint d=0; d<D; d++) {
                    #pragma HLS unroll
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

                // free centre list if sub-tree is pruned (heap-allocated data are not needed anymore)
                if (alloc_full == false) {
                    free<centre_list_pointer>(&centre_freelist[0*SCRATCHPAD_SIZE], &centre_next_free_location, tmp_new_centre_list_idx);
                    heap_utilisation--;
                }

            } else {

                centre_list_pointer new_centre_list_idx;
                centre_index_type new_k_to_stack;

                // return full-size set by default if heap was full (new centre list could not be allocated)
                if ( alloc_full == false) {
                    new_centre_list_idx = tmp_new_centre_list_idx;
                    new_k_to_stack = new_k;
                } else {
                    new_centre_list_idx = 0;
                    new_k_to_stack = k;
                }

                // mark new_centre_list_idx address as written
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
    }

    // readout centres
    read_out_centres_loop: for(centre_index_type i=0; i<=k; i++) {
        #pragma HLS pipeline II=1
        #ifdef PARALLELISE
        filt_centres_out[i][par] = centre_buffer[i];
        #else
        filt_centres_out[i] = centre_buffer[i];
        #endif
        if (i==k) {
             break;
        }
    }
}




#else



/********************** original, non optimised version *******************************/



// global array for the tree (keep heap local to this file)
// tree memory (leafs and intermediate nodes)
kdTree_type tree_node_int_memory[HEAP_SIZE/2];
kdTree_leaf_type tree_node_leaf_memory[HEAP_SIZE/2];
// centre positions
data_type centre_positions[K];
// scratchpad heap memory for centre lists
centre_heap_type centre_heap[SCRATCHPAD_SIZE];
centre_list_pointer centre_freelist[SCRATCHPAD_SIZE];
centre_list_pointer centre_next_free_location;
centre_list_pointer heap_utilisation;

// some statistics collected during C simulation
#ifndef __SYNTHESIS__
uint visited_nodes;
#endif


// top-level function
void filtering_algorithm_top(   volatile kdTree_type *node_data,
                                volatile node_pointer *node_address,
                                volatile data_type *cntr_pos_init,
                                node_pointer n,
                                centre_index_type k,
                                volatile node_pointer *root,
                                volatile coord_type_ext *distortion_out,
                                volatile data_type *clusters_out)
{

    // set the interface properties
    #pragma HLS interface ap_none register port=n
    #pragma HLS interface ap_none register port=k
    #pragma HLS interface ap_fifo port=node_data depth=256
    #pragma HLS interface ap_fifo port=node_address depth=256
    #pragma HLS interface ap_fifo port=cntr_pos_init depth=256
    #pragma HLS interface ap_fifo port=root depth=256
    #pragma HLS interface ap_fifo port=distortion_out depth=256
    #pragma HLS interface ap_fifo port=clusters_out depth=256

    // pack all items of a struct into a single data word
    #pragma HLS data_pack variable=node_data // recursively pack struct kdTree_type
	/*
    #pragma HLS data_pack variable=cntr_pos_init
    #pragma HLS data_pack variable=clusters_out
    */
    #pragma HLS data_pack variable=tree_node_int_memory
    #pragma HLS data_pack variable=tree_node_leaf_memory

    // specify the type of memory instantiated for these arrays: two-port block ram and lut ram
    #pragma HLS resource variable=centre_freelist core=RAM_2P_LUTRAM
    #pragma HLS resource variable=centre_heap core=RAM_2P_BRAM
    #pragma HLS resource variable=centre_positions core=RAM_2P_BRAM
    #pragma HLS resource variable=tree_node_leaf_memory core=RAM_2P_BRAM
    #pragma HLS resource variable=tree_node_int_memory core=RAM_2P_BRAM

    // the array is just for compatibility reasons
    node_pointer t_root = root[0];

    init_tree_node_memory(node_data,node_address,n);

    centre_type filt_centres_out[K];
    data_type new_centre_positions[K];

    // iterate over a constant number of outer clustering iterations
    it_loop: for (uint l=0; l<L; l++) {
        #ifndef __SYNTHESIS__
        visited_nodes = 0;
        #endif

        // in the first iteration, load centre_positions from the interface, otherwise from new_centre_positions
        if (l==0) {
            for (centre_index_type i=0; i<=k; i++) {
                #pragma HLS pipeline II=1
                centre_positions[i].value = cntr_pos_init[i].value;
                if (i==k) {
                    break;
                }
            }
        } else {
            for (centre_index_type i=0; i<=k; i++) {
                #pragma HLS pipeline II=1
                centre_positions[i].value = new_centre_positions[i].value;
                if (i == k) {
                    break;
                }
            }
        }

        // run main clustering kernel (tree search)
        filter(t_root, k, filt_centres_out);

        #ifndef __SYNTHESIS__
        printf("%d: visited nodes: %d\n",0,visited_nodes);
        #endif

        // re-init centre positions
        update_centres(filt_centres_out, k, new_centre_positions);

    }

    // write clustering output: new cluster centres and distortion
    output_loop: for (centre_index_type i=0; i<=k; i++) {
        #pragma HLS pipeline II=1
        distortion_out[i] = filt_centres_out[i].sum_sq;
        clusters_out[i].value = new_centre_positions[i].value;
        if (i==k) {
            break;
        }
    }
}


// load data points from interface into internal memory
void init_tree_node_memory(volatile kdTree_type *node_data, volatile node_pointer *node_address, node_pointer n)
{
    #pragma HLS inline

    init_nodes_loop: for (node_pointer i=0; i<=n; i++) {
        #pragma HLS pipeline II=1
        node_pointer tmp_node_address = node_address[i];
        kdTree_type tmp_node;
        tmp_node = node_data[i];

        // if MSB if the node pointer is not set, use intermediate nodes memory,
        // use leaf nodes memory otherwise
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


// update the new centre positions after one outer clustering iteration
void update_centres(centre_type *centres_in,centre_index_type k, data_type *centres_positions_out)
{
    //#pragma HLS inline
    centre_update_loop: for (centre_index_type i=0; i<=k; i++) {
        #pragma HLS pipeline II=2

        coord_type tmp_count = centres_in[i].count;
        if ( tmp_count == 0 )
            tmp_count = 1;

        data_type_ext tmp_wgtCent = centres_in[i].wgtCent;
        data_type tmp_new_pos;
        for (uint d=0; d<D; d++) {
            #pragma HLS unroll
            coord_type_ext tmp_div_ext = (get_coord_type_vector_ext_item(tmp_wgtCent.value,d) / tmp_count); //let's see what it does with that...
            coord_type tmp_div = (coord_type) tmp_div_ext;
            #pragma HLS resource variable=tmp_div core=DivnS
            set_coord_type_vector_item(&tmp_new_pos.value,tmp_div,d);
        }
        centres_positions_out[i] = tmp_new_pos;
        if (i==k) {
            break;
        }
    }
}


// main clustering kernel
void filter (node_pointer root,
            centre_index_type k,
            centre_type *centres_out)
{

    centre_type centre_buffer[K];
    #pragma HLS resource variable=centre_buffer core=RAM_2P_LUTRAM

    // init centre buffer
    init_centre_buffer_loop: for(centre_index_type i=0; i<=k; i++) {
        #pragma HLS pipeline II=1
        centre_buffer[i].count = 0;
        centre_buffer[i].sum_sq = 0;
        for (uint d=0; d<D; d++) {
            #pragma HLS unroll
            set_coord_type_vector_ext_item(&centre_buffer[i].wgtCent.value,0,d);
        }
        if (i==k) {
            break;
        }
    }

    // init dynamic memory allocator for centre lists scratchpad heap
    init_allocator<centre_list_pointer>(centre_freelist, &centre_next_free_location, SCRATCHPAD_SIZE-2);

    // allocate first centre list
    centre_list_pointer centre_list_idx = malloc<centre_list_pointer>(centre_freelist, &centre_next_free_location);
    centre_heap_type *centre_list_idx_ptr =  make_pointer<centre_heap_type>(centre_heap, (uint)centre_list_idx);

    heap_utilisation = 1;

    // init allocated centre list
    init_centre_list_loop: for(centre_index_type i=0; i<=k; i++) {
        #pragma HLS pipeline II=1
        centre_list_idx_ptr->idx[i] = i;
        if (i==k) {
            break;
        }
    }

    // trace buffer (mark a heap address if it was read for the second time)
    bool trace_buffer[SCRATCHPAD_SIZE];
    #pragma HLS resource variable=trace_buffer core=RAM_2P_LUTRAM

    // stack pointers
    uint stack_pointer = 0;
    uint cstack_pointer = 0;

    //stack
    stack_record stack_array[N];
    cstack_record_type cstack_array[N];


    // push pointers to tree root node and first centre list onto the stack
    uint node_stack_length = push_node(root, &stack_pointer, stack_array);
    uint cntr_stack_length = push_centre_set(centre_list_idx,k, &cstack_pointer, cstack_array);

    // main tree search loop
    tree_search_loop: while (node_stack_length != 0) {

        #ifndef __SYNTHESIS__
        visited_nodes++;
        #endif

        // fetch head of stack
        node_pointer u;
        node_stack_length = pop_node(&u, &stack_pointer, stack_array);

        // if MSB if the node pointer is not set, read from intermediate nodes memory,
        // read from leaf nodes memory otherwise
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

        // pop pointer to centre list from stack
        centre_list_pointer tmp_cntr_list;
        centre_index_type tmp_k;
        cntr_stack_length = pop_centre_set(&tmp_cntr_list,&tmp_k, &cstack_pointer, cstack_array);

        // get pointer to centre list
        centre_heap_type *tmp_cntr_list_ptr = make_pointer<centre_heap_type>(centre_heap, (uint)tmp_cntr_list);

        bool rdy_for_deletion;
        trace_address(tmp_cntr_list, false, &rdy_for_deletion,trace_buffer);

        data_type tmp_centre_positions[K];
        centre_index_type tmp_centre_indices[K];
        fetch_centres_loop: for (centre_index_type i=0; i<=tmp_k; i++) {
            #pragma HLS pipeline II=1
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

        // leaf node?
        data_type_ext comp_point;
        if ( (tmp_u.left == NULL_PTR) && (tmp_u.right == NULL_PTR) ) {
            comp_point = tmp_u.wgtCent;
        } else {
            comp_point = conv_short_to_long(tmp_u.midPoint);
        }

        centre_index_type tmp_final_idx;
        data_type z_star;
        coord_type_ext tmp_min_dist;

        // find centre with smallest distance to z_star
        minsearch_loop: for (centre_index_type i=0; i<=tmp_k; i++) {
            #pragma HLS pipeline II=1

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

        // determine whether a sub-tree will be pruned
        tooFar_loop: for (centre_index_type i=0; i<=tmp_k; i++) {
            #pragma HLS pipeline II=1
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
            #pragma HLS unroll
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
        #pragma HLS resource variable=tmp3 core=MulnS

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
        // final decision whether sub-tree will be pruned
        if ( tmp_deadend == true ) {

            // weighted centroid of this centre
            for (uint d=0; d<D; d++) {
                #pragma HLS unroll
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
                    #pragma HLS pipeline II=1
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
            trace_address(new_centre_list_idx, true, &dummy_rdy_for_deletion,trace_buffer);

            node_pointer left_child = tmp_u_out.left;
            node_pointer right_child = tmp_u_out.right;

            // push children onto stack
            node_stack_length = push_node(right_child, &stack_pointer, stack_array);
            node_stack_length = push_node(left_child, &stack_pointer, stack_array);

            // push centre lists for both children onto stack
            cntr_stack_length = push_centre_set(new_centre_list_idx,new_k, &cstack_pointer, cstack_array);
            cntr_stack_length = push_centre_set(new_centre_list_idx,new_k, &cstack_pointer, cstack_array);
        }
    }

    // readout centres
    read_out_centres_loop: for(centre_index_type i=0; i<=k; i++) {
        #pragma HLS pipeline II=1
        centres_out[i] = centre_buffer[i];
        if (i==k) {
            break;
        }
    }
}

#endif
