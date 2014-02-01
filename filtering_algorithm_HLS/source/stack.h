/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: stack.h
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/


#ifndef STACK_H
#define STACK_H

#include "filtering_algorithm_top.h"
//#include "filtering_algorithm_top_v2.h"

#define STACK_SIZE N

// node
typedef node_pointer stack_record;

// centre stack
struct cstack_record_type {
    centre_list_pointer list;
    centre_index_type k;
    cstack_record_type& operator=(const cstack_record_type& a);
};


void init_stack(uint *stack_pointer, uint *cstack_pointer);
uint push_node(stack_record u, uint *stack_pointer, stack_record* stack_array);
uint pop_node(stack_record *u, uint *stack_pointer, stack_record* stack_array);
uint lookahead_node(stack_record *u, uint *stack_pointer, stack_record* stack_array);
uint push_centre_set(centre_list_pointer list, centre_index_type k, uint *cstack_pointer, cstack_record_type *cstack_array);
uint pop_centre_set(centre_list_pointer *list, centre_index_type *k, uint *cstack_pointer, cstack_record_type *cstack_array);

#endif
