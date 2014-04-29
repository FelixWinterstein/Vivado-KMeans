/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: clustering.h
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/

#ifndef __clustering_H__
#define __clustering_H__

#ifdef __cplusplus
    extern "C" {
#endif


#include <stdio.h>
#include <stdlib.h>
//#include <stdbool.h>
#include <time.h>
#include <time.h>
#include "my_util.h"
#include "lloyds.h"
//#include "build_kdTree_it.h"
#include "build_kdTree.h" 
#include "filter_it.h"

void buildTree(uint n, uint k, data_type_short *data_set, uint *root, kdTree_type *tree_memory);
void clustering(uint n, uint k, centre_type *initial_centres, data_type_short *data_points, kdTree_type *tree_memory, uint root, data_type_short *output_array, centre_type *centre_output);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __clustering_H__ */
