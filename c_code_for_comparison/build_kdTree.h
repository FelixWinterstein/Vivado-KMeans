/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: build_kdTree.h
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/

#ifndef BUILD_KDTREE_H
#define BUILD_KDTREE_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "my_util.h"

uint buildkdTree(data_type_short *points, uint *idx, uint n, data_type_short *bnd_lo, data_type_short *bnd_hi, uint *heap_ptr, kdTree_type *tree_memory);


#ifdef  __cplusplus
}
#endif


#endif  /* BUILD_KDTREE_H */
