/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: filtering_algorithm_tb.cpp
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "../source/filtering_algorithm_top.h"
#include "../source/filtering_algorithm_util.h"
#include "build_kdTree.h"


// recursively split the kd-tree into P sub-trees (P is parallelism degree)
void recursive_split(uint p,
                    uint n,
                    data_type bnd_lo,
                    data_type bnd_hi,
                    uint *idx,
                    data_type *data_points,
                    uint *i,
                    uint *ofs,
                    node_pointer *root,
                    kdTree_type *heap,
                    kdTree_type *tree_image,
                    node_pointer *tree_image_addr,
                    uint n0,
                    uint k,
                    double std_dev)
{
    if (p==P) {
        printf("Sub-tree %d: %d data points\n",*i,n);
        node_pointer rt = buildkdTree(data_points, idx, n, &bnd_lo, &bnd_hi, *i*HEAP_SIZE/2/P, heap);
        root[*i] = rt;
        uint offset = *ofs;
        readout_tree(true, n0, k, std_dev, rt, heap, offset, tree_image, tree_image_addr);
        *i = *i + 1;
        *ofs = *ofs + 2*n-1;
    } else {
        uint cdim;
        coord_type cval;
        uint n_lo;
        split_bounding_box(data_points, idx, n, &bnd_lo, &bnd_hi, &n_lo, &cdim, &cval);
        // update bounding box
        data_type new_bnd_hi = bnd_hi;
        data_type new_bnd_lo = bnd_lo;
        set_coord_type_vector_item(&new_bnd_hi.value,cval,cdim);
        set_coord_type_vector_item(&new_bnd_lo.value,cval,cdim);

        recursive_split(p*2, n_lo, bnd_lo, new_bnd_hi, idx, data_points,i,ofs,root, heap,tree_image,tree_image_addr,n0,k,std_dev);
        recursive_split(p*2, n-n_lo, new_bnd_lo, bnd_hi, idx+n_lo, data_points,i,ofs,root, heap,tree_image,tree_image_addr,n0,k,std_dev);
    }

}



int main()
{

    const uint n = 128; // 16384
    const uint k = 4;   // 128
    const double std_dev = 0.75; //0.20


    uint *idx = new uint[N];
    data_type *data_points = new data_type[N];
    uint *cntr_indices = new uint[K];
    kdTree_type *heap = new kdTree_type[HEAP_SIZE];
    data_type *initial_centre_positions= new data_type[K];

    // read data points from file
    if (read_data_points(n,k,std_dev,data_points,idx) == false)
        return 1;

    // read intial centre from file (random placement
    if (read_initial_centres(n,k,std_dev,initial_centre_positions,cntr_indices) == false)
        return 1;

    // print initial centres
    printf("Initial centres\n");
    for (uint i=0; i<k; i++) {
        printf("%d: ",i);
        for (uint d=0; d<D-1; d++) {
            printf("%d ",get_coord_type_vector_item(initial_centre_positions[i].value, d).to_int());
        }
        printf("%d\n",get_coord_type_vector_item(initial_centre_positions[i].value, D-1).to_int());
    }


    // compute axis-aligned hyper rectangle enclosing all data points
    data_type bnd_lo, bnd_hi;
    compute_bounding_box(data_points, idx, n, &bnd_lo, &bnd_hi);

    node_pointer root[P];
    kdTree_type *tree_image = new kdTree_type[HEAP_SIZE];
    node_pointer *tree_image_addr = new node_pointer[HEAP_SIZE];
    uint z=0;
    uint ofs=0;
    recursive_split(1, n, bnd_lo, bnd_hi, idx, data_points,&z,&ofs,root,heap,tree_image,tree_image_addr,n,k,std_dev);

    data_type clusters_out[K];
    coord_type_ext distortion_out[K];

    // FIXME: get automatic co-simulation working

    /*
    for (uint i=0; i<P; i++) {
    	root[i] = 0;
    }
    for (uint i=0; i<2*n-1; i++) {
    	tree_image[i].bnd_hi = bnd_hi;
    	tree_image[i].bnd_lo = bnd_lo;
    	tree_image[i].count.VAL = i;
    	tree_image[i].idx = NULL;
    	tree_image[i].left.VAL = 0;
    	tree_image[i].right.VAL = 0;
    	tree_image[i].midPoint = bnd_hi;
    	tree_image[i].sum_sq.VAL = 0;
    	tree_image[i].wgtCent = conv_short_to_long(bnd_hi);
    	tree_image_addr[i].VAL = i;
    }
	*/

    filtering_algorithm_top(tree_image,tree_image_addr,initial_centre_positions,2*n-1-1-(P-1),k-1,root,distortion_out,clusters_out);

    // print initial centres
    printf("New centres after clustering\n");
    for (uint i=0; i<k; i++) {
        printf("%d: ",i);
        for (uint d=0; d<D-1; d++) {
            printf("%d ",get_coord_type_vector_item(clusters_out[i].value, d).to_int());
        }
        printf("%d\n",get_coord_type_vector_item(clusters_out[i].value, D-1).to_int());
    }


    delete[] idx;
    delete[] data_points;
    delete[] initial_centre_positions;
    delete[] cntr_indices;

    delete[] heap;
    delete[] tree_image;
    delete[] tree_image_addr;


    return 0;
}
