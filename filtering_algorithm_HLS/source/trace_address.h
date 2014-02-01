/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: trace_address.h
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/


#ifndef TRACE_ADDRESS_H
#define TRACE_ADDRESS_H

#include "filtering_algorithm_top.h"

void trace_address(centre_list_pointer address, bool write, bool *rdy_for_deletion, bool *trace_buffer);


#endif
