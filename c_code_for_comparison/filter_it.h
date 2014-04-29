/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: filter_it.h
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/

#ifndef FILTER_IT_H
#define FILTER_IT_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "my_util.h"
    
    
// stack types for building the kd-tree iteratively
typedef struct stack_record stack_type;
typedef struct stack_record* stack_ptr;

struct stack_record {
    uint u;
    //stack_ptr next;
};

// stack types for managing the centre lists
typedef struct cstack_record cstack_type;
typedef struct cstack_record* cstack_ptr;

// centre stack
struct cstack_record {
    centre_set_ptr list;
    uint k;
    bool redundant;
    //cstack_ptr next;
};
    
void filter_it( uint root, 
                kdTree_type *tree_memory,
                centre_type *centres,
                uint k,
                bool last_run, 
                data_type_short *output_array);

#ifdef  __cplusplus
}
#endif

#endif  /* FILTER_IT_H */

