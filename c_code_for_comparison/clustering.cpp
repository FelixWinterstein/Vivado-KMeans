/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: clustering.cpp
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/

#include "clustering.h"
#include <time.h>
#include <sys/time.h>


// wrapper function for buildkdTree
void buildTree(uint n, uint k, data_type_short *data_points, uint *root, kdTree_type *tree_memory)
{           
    //uint *index = malloc(NMAX*sizeof(uint));
    uint index[NMAX];
    
    for (uint i=0; i<n; i++) {
        index[i] = i;
    }

    // compute axis-aligned hyper rectangle enclosing all data points
    data_type_short bnd_lo, bnd_hi;
    compute_bounding_box(data_points, index, n, &bnd_lo, &bnd_hi);
    
    uint heap_ptr = 0;

    // build up data structure 
    uint tmp_root = buildkdTree(data_points, index, n, &bnd_lo, &bnd_hi, &heap_ptr, tree_memory);

    *root = tmp_root;
        
    //free(index);
}


// implements the k-means clustering
void clustering(uint n, uint k, centre_type *initial_centres, data_type_short *data_points, kdTree_type *tree_memory, uint root, data_type_short *output_array, centre_type *centre_output)
{

    centre_type centres[KMAX];  
    
    // sample KMAX initial centres randomly from the data points
    //generate_initial_centres(data_points, centres, centr_idx,n,k,15);     
    
    uint iteration = 0; 
    coord_type total_distortion = 0;
    coord_type prev_total_distortion = -1;

    for(uint i=0; i<k; i++) {
        centres[i].count_short = initial_centres[i].count_short;
        centres[i].position_short = initial_centres[i].position_short;        
    }
    
    // start timer
    clock_t t;
    t = clock();
    struct timeval start, end;
    unsigned long long usecs;
    GETTIME(start); 
       
    
    // main clustering loop (runs L+1 times)
    for ( iteration = 0; iteration <= L; iteration++ ) {

        bool last_run = false;
        if (iteration == L )
            last_run = true; // this works only for filter_it !!!        

        // select algorithm
        #ifdef FILTERING_ALGO
        filter_it(root, tree_memory, centres, k, false, NULL);
        #else
        lloyds(data_points, centres, k, n, false, NULL);
        #endif                      
        
        coord_type tmp_distortion = 0;        
        
        // update centres
        for (uint i=0; i<k; i++) {           
            tmp_distortion += centres[i].sum_sq;    
            
            int tmp_count_short = centres[i].count_short;     
            
            for (uint d=0; d<D; d++) {
                if (tmp_count_short != 0) {
                    centres[i].position_short.value[d] = (coord_type_short)(centres[i].wgtCent.value[d]/((coord_type)tmp_count_short));
                }

                // reset centre information for next iteration
                centres[i].wgtCent.value[d] = 0;
                centres[i].count_short = 0;
                centres[i].sum_sq = 0;
            }      
        }
        
        // update total distortion per iteration
        prev_total_distortion = total_distortion;
        total_distortion = tmp_distortion;
    }   


    for (uint i=0; i<k; i++) {
        centre_output[i] = centres[i];
    }           
     
    // stop timer
    GETTIME(end);
    usecs = GETUSEC(end,start);
    t = clock() - t; 
        
    //printf("Filtering time (using GETTIME): %d us\n",usecs);
    
}
