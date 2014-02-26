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

#include "../source/filtering_algorithm_util.h"
#include "../source/stack.h"
#include "../source/dyn_mem_alloc.h"


//file IO
bool read_data_points(uint n, uint k, double std_dev, data_type* points, uint* index);
bool read_initial_centres(uint n, uint k, double std_dev, data_type *initial_centre_positions, uint* centr_idx);


//tree build-up
void find_min_max(data_type* points, uint *idx , uint dim, uint n, coord_type *ret_min, coord_type *ret_max);
void compute_bounding_box(data_type* points, uint *idx, uint n, data_type *bnd_lo, data_type *bnd_hi);
void split_bounding_box(data_type* points, uint *idx, uint n, data_type *bnd_lo, data_type *bnd_hi, uint *n_lo, uint *cdim, coord_type *cval);
void setup_tree_node(data_type* points, uint *idx, uint n, data_type bnd_lo, data_type bnd_hi, kdTree_ptr u);
node_pointer buildkdTree(data_type* points, uint *idx, uint n, data_type *bnd_lo, data_type *bnd_hi,node_pointer root_offset, kdTree_type *heap);
void dot_product_tb(data_type_ext p1,data_type_ext p2, coord_type_ext *r);
void update_sums(node_pointer root, data_type* points, kdTree_type *heap);
void scale_sums(node_pointer root, kdTree_type *heap);
void readout_tree(bool write2file, uint n, uint k, double std_dev, node_pointer root, kdTree_type *heap, uint offset, kdTree_ptr image, node_pointer *image_addr);

//helper macros
#define get_coord(points, indx, idx, dim) ( get_coord_type_vector_item((points+*(indx+idx))->value,dim) )
#define coord_swap(indx, i1, i2) { uint tmp = *(indx+i1);\
                                    *(indx+i1) = *(indx+i2);\
                                    *(indx+i2) = tmp; }



#endif  /* BUILD_KDTREE_H */
