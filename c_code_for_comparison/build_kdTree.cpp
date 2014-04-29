/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: build_kdTree.cpp
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "build_kdTree.h"


// post-order recursive tree built-up
uint buildkdTree(data_type_short *points, uint *idx, uint n, data_type_short *bnd_lo, data_type_short *bnd_hi, uint *heap_ptr, kdTree_type *tree_memory)
{
    //printf("%d: %d\n",*heap_ptr,n);
    
    //*heap_ptr = *heap_ptr+1;
        
    if (n == 1) {
        // leaf node
        
        //compute sum of squares for this point
        coord_type tmp_sum_sq = 0;
        for(uint d=0; d<D; d++) {
            coord_type tmp = get_coord(points,idx,0,d);
            tmp_sum_sq += tmp*tmp;
        }
        
        kdTree_type leaf_node; 
        
        // set up node
        leaf_node.bnd_hi_short = *bnd_hi;
        leaf_node.bnd_lo_short = *bnd_lo;
        leaf_node.midPoint_short = *bnd_lo;
        leaf_node.ileft = 0;
        leaf_node.iright = 0;
        leaf_node.wgtCent = conv_short_to_long(points[*(idx+0)]); // this is just the point itself
        leaf_node.sum_sq = tmp_sum_sq;
        leaf_node.count_short = 1;
        leaf_node.pointAddr = *(idx+0);             
                     
        uint tmp_ptr = *heap_ptr+1;        
        *heap_ptr = tmp_ptr;                
        tree_memory[tmp_ptr] = leaf_node;        
        return tmp_ptr;
                
        /*
        kdTree_type *leaf_node_ptr = malloc(sizeof(kdTree_type));
        *leaf_node_ptr = leaf_node;
        return leaf_node_ptr;
        */        
        
    } else {
        // intermediate node    

        uint n_lo;
        uint cdim;
        coord_type cval;
        uint left, right;
        
        split_bounding_box(points, idx, n, bnd_lo, bnd_hi, &n_lo, &cdim, &cval);
                
        
        coord_type hv = bnd_hi->value[cdim];
        coord_type lv = bnd_lo->value[cdim];

        //left subtree
        bnd_hi->value[cdim] = cval;
        left = buildkdTree(points, idx,n_lo, bnd_lo, bnd_hi, heap_ptr, tree_memory);
        bnd_hi->value[cdim] = hv;

        //right subtree
        bnd_lo->value[cdim] = cval;
        right = buildkdTree(points, idx+n_lo,n-n_lo, bnd_lo, bnd_hi, heap_ptr, tree_memory);
        bnd_lo->value[cdim] = lv;
        
        kdTree_type int_node;
        

        data_type tmp_wgtCent;
        coord_type tmp_sum_sq = 0;
        
        kdTree_type tmp_left, tmp_right;        
        
        tmp_left = tree_memory[left];
        tmp_right = tree_memory[right];
        
        // compute sum
        for (uint d=0; d<D; d++) {
            tmp_wgtCent.value[d] = tmp_left.wgtCent.value[d] + tmp_right.wgtCent.value[d];
        }
        tmp_sum_sq = tmp_left.sum_sq + tmp_right.sum_sq;
        
        
        // compute cell mid point
        data_type_short tmp_mid;
        for (uint d=0;d<D;d++) {
            tmp_mid.value[d] = (bnd_lo->value[d]+bnd_hi->value[d]) / 2;
        }
        
        // set up node
        int_node.midPoint_short = tmp_mid;
        int_node.wgtCent = tmp_wgtCent;
        int_node.sum_sq = tmp_sum_sq;
        int_node.ileft  = left;
        int_node.iright = right;
        int_node.bnd_hi_short = *bnd_hi;
        int_node.bnd_lo_short = *bnd_lo;
        int_node.count_short = n;
        int_node.pointAddr = 0;  
    
        
        uint tmp_ptr = *heap_ptr+1;        
        *heap_ptr = tmp_ptr;                  
        tree_memory[tmp_ptr] = int_node;
        return tmp_ptr;
        
        /*
        kdTree_type *int_node_ptr = malloc(sizeof(kdTree_type));
        *int_node_ptr = int_node;        
        return int_node_ptr;
        */ 
                
    }
    
}
