/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: lloyds_algorithm_tb.cpp
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "../source/lloyds_algorithm_top.h"
#include "../source/lloyds_algorithm_util.h"
#include "tb_io.h"


uint idx[N];
data_type data_points[N];
data_type initial_centre_positions[K];
uint cntr_indices[K];


int main()
{
    // these parameters must match the input data files
    const uint n = 128;
    const uint k = 4;
    const double std_dev = 0.75;

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

    data_type clusters_out[K];
    coord_type_ext distortion_out[K];

    // run K-means clustering using Lloyd's algorithm
    lloyds_algorithm_top(data_points,initial_centre_positions,n-1,k-1,distortion_out,clusters_out);


    // print cluster centres
    printf("New centres after clustering\n");
    for (uint i=0; i<k; i++) {
        printf("%d: ",i);
        for (uint d=0; d<D-1; d++) {
            printf("%d ",get_coord_type_vector_item(clusters_out[i].value, d).to_int());
        }
        printf("%d\n",get_coord_type_vector_item(clusters_out[i].value, D-1).to_int());
    }


    return 0;
}
