/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: stack.cpp
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/


#include "stack.h"

cstack_record_type& cstack_record_type::operator=(const cstack_record_type& a)
{
	list = a.list;
	k = a.k;
	return *this;
}


void init_stack(uint *stack_pointer, uint *cstack_pointer)
{
	#pragma HLS inline
	stack_pointer = 0;
	cstack_pointer = 0;
}

// push pointer to tree node pointer onto stack
uint push_node(stack_record u, uint *stack_pointer, stack_record* stack_array)
{
	uint tmp = *stack_pointer;
    stack_array[tmp] = u;
    tmp++;
    *stack_pointer=tmp;
    return tmp;
}

// pop pointer to tree node pointer from stack
uint pop_node(stack_record *u, uint *stack_pointer, stack_record* stack_array)
{
	uint tmp = *stack_pointer-1;
    *u = stack_array[tmp];
    *stack_pointer = tmp;
    return tmp;
}

// look up head of node stack
uint lookahead_node(stack_record *u, uint *stack_pointer, stack_record* stack_array)
{
	uint tmp = *stack_pointer-1;
    *u = stack_array[tmp];
    return tmp;
}

// push data onto cstack
uint push_centre_set(centre_list_pointer list, centre_index_type k, uint *cstack_pointer, cstack_record_type *cstack_array)
{
	cstack_record_type tmp;
	tmp.list = list;
	tmp.k = k;
	uint tmp1 = *cstack_pointer;
    cstack_array[tmp1] = tmp;
    tmp1++;
    *cstack_pointer = tmp1;
    return tmp1;
}

// pop from cstack
uint pop_centre_set(centre_list_pointer *list, centre_index_type *k, uint *cstack_pointer, cstack_record_type *cstack_array)
{
	uint tmp_cstack_pointer = *cstack_pointer-1;
	cstack_record_type tmp;
	tmp = cstack_array[tmp_cstack_pointer];
	*list = tmp.list;
	*k = tmp.k;
    *cstack_pointer = tmp_cstack_pointer;
    return tmp_cstack_pointer;
}

