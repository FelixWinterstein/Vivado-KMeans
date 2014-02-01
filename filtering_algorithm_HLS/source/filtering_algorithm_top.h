/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: filtering_algorithm_top.h
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/

#ifndef FILTERING_ALGORITHM_TOP_H
#define	FILTERING_ALGORITHM_TOP_H

#include <math.h>
#include "ap_int.h" // custom data types


#define D 3			// data dimensionality
#define N 32768   	// max number of data points
#define K 256    	// max number of centres
#define L 6 		// max number of outer clustering iterations

//If P>1, #define PARALLELISE must be enabled!! If P==1, #define PARALLELISE must be disabled!!
#define PARALLELISE // enable this if parallelism degree P is >1, disable otherwise
#define P 4		// parallelism degree (currently, max P = 4!).

#define OPTIMISED_VERSION  // see note in filtering_algorithm_top.cpp

#define HEAP_SIZE 2*N		// max size of heap memory for the kd-tree (2*n tree nodes)
#define SCRATCHPAD_SIZE 256	// max number of centre lists that can be allocated in the scratchpad heap
#define CHANNEL_DEPTH 32 	// fifo buffer depth, used for OPTIMISED_VERSION only

#define COORD_BITWIDTH 16
#define COORD_BITWITDH_EXT 32
#define NODE_POINTER_BITWIDTH 16 	// log2(2*N)
#define CNTR_INDEX_BITWIDTH 8 		// log2(K)
#define CNTR_LIST_INDEX_BITWIDTH 14 // log2(N)

// pointer types to tree nodes and centre lists
typedef ap_uint<NODE_POINTER_BITWIDTH> node_pointer;
typedef ap_uint<CNTR_LIST_INDEX_BITWIDTH> centre_list_pointer;
typedef ap_uint<CNTR_INDEX_BITWIDTH> centre_index_type;

// force register insertion in the generated RTL for some signals
#define FORCE_REGISTERS

typedef unsigned int uint;
typedef ap_int<COORD_BITWIDTH> coord_type;
typedef ap_int<D*COORD_BITWIDTH> coord_type_vector;
typedef ap_int<COORD_BITWITDH_EXT> coord_type_ext;
typedef ap_int<D*COORD_BITWITDH_EXT> coord_type_vector_ext;

// ... used for saturation
#define MAX_FIXED_POINT_VAL_EXT (1<<(COORD_BITWITDH_EXT-1))-1

//bit width definitions for multiplication
#define MUL_INTEGER_BITS 12
#define MUL_FRACTIONAL_BITS 6
#define MUL_MAX_VAL (1<<(MUL_INTEGER_BITS+MUL_FRACTIONAL_BITS-1))-1
#define MUL_MIN_VAL -1*(1<<(MUL_INTEGER_BITS+MUL_FRACTIONAL_BITS-1))
typedef ap_int<MUL_INTEGER_BITS+MUL_FRACTIONAL_BITS> mul_input_type;


// this should be always 1
#define FILE_INDEX 1

// data point types
struct data_type {
	//coord_type value[D];
	coord_type_vector value;
    data_type& operator=(const data_type& a);
    data_type& operator=(const volatile data_type& a);
};


// data point types ext
struct data_type_ext {
	coord_type_vector_ext value;
    data_type_ext& operator=(const data_type_ext& a);
    data_type_ext& operator=(const volatile data_type_ext& a);
};



// tree node types
struct kdTree_type {
	data_type_ext wgtCent;
	data_type midPoint;
    data_type bnd_lo;
    data_type bnd_hi;
    coord_type_ext sum_sq;
    coord_type count;
    node_pointer left, right;
	#ifndef __SYNTHESIS__
    uint *idx;
	#endif
    kdTree_type& operator=(const kdTree_type& a);
    kdTree_type& operator=(const volatile kdTree_type& a);
};
typedef kdTree_type* kdTree_ptr;


// tree leaf node type
struct kdTree_leaf_type {
	data_type_ext wgtCent;
    coord_type_ext sum_sq;
    kdTree_leaf_type& operator=(const kdTree_leaf_type& a);
    kdTree_leaf_type& operator=(const volatile kdTree_leaf_type& a);
};



// centre types
struct centre_type {
    data_type_ext wgtCent; // sum of all points assigned to this centre
    coord_type_ext sum_sq; // sum of norm of all points assigned to this centre
    coord_type count;
    centre_type& operator=(const centre_type& a);
    centre_type& operator=(const volatile centre_type& a);
};
typedef centre_type* centre_ptr;

// centre list idx heap
struct centre_heap_type {
	centre_index_type idx[K];
};


#ifdef FORCE_REGISTERS
template<class T>
T Reg(T in) {
        #pragma AP INLINE off
        #pragma AP INTERFACE port=return register
        return in;
}
#else
template<class T>
T Reg(T in) {
        #pragma AP INLINE
        return in;
}
#endif



void filtering_algorithm_top(	volatile kdTree_type *node_data,
								volatile node_pointer *node_address,
								volatile data_type *cntr_pos_init,
								node_pointer n,
								centre_index_type k,
								volatile node_pointer *root,
								volatile coord_type_ext *distortion_out,
								volatile data_type *clusters_out);

void init_tree_node_memory(volatile kdTree_type *node_data, volatile node_pointer *node_address, node_pointer n);

void update_centres(centre_type *centres_in,centre_index_type k, data_type *centres_positions_out);

#ifndef OPTIMISED_VERSION
	void filter (node_pointer root,
				 centre_index_type k,
				 centre_type *centres_out);
#else
	template<uint par>void filter(node_pointer root,
				 	 	 	 	  centre_index_type k);
#endif

#endif	/* FILTERING_ALGORITHM_TOP_H */
