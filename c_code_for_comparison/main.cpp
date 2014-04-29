/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: main.cpp
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
//#include <stdbool.h>
#include <time.h>
#include <time.h>
#include <unistd.h>
#include "my_util.h"
#include "lloyds.h"
#include "filter_it.h"
#include "clustering.h"


int main(int argc, char** argv)
{
    uint k = 128;
    uint n = 16384;  
    const double std_dev = 0.20;
    const uint fidx = 1;      
    
    // works for linux!
    char path[FILENAME_MAX];
    if (!getcwd(path, sizeof(path))){
        printf("ERROR: getcwd failed\n");
        return EXIT_SUCCESS;  
    }
    printf("Working directory is: %s\n",path);

    // allocate input arrays 
    data_type_short *data_points = new data_type_short[NMAX];            
    centre_type *initial_centres = new centre_type[KMAX];
    uint *centr_idx = new uint[KMAX];
           
    // read data points file
    if (read_data_points(data_points, n, k, std_dev, fidx) == false) {
        printf("ERROR: Could not find data points file\n");
        return EXIT_SUCCESS;  
    }
        
    // read initial centres file
    if (read_initial_centres(initial_centres, centr_idx, n, k, std_dev, fidx) == false) {
        printf("ERROR: Could not find data points file\n");
        return EXIT_SUCCESS;  
    }
            
    // allocate output arrays
    data_type_short *data_output= new data_type_short[NMAX];
    centre_type *centre_output = new centre_type[KMAX]; 
    kdTree_type *tree_memory = new kdTree_type[2*NMAX];
    

    // build kd-tree and run clustering
    uint root;
    #ifdef FILTERING_ALGO
    buildTree(n, k, data_points, &root, tree_memory);
    #endif        
    clustering(n, k, initial_centres, data_points, tree_memory, root, data_output, centre_output);
    
    #ifdef VERBOSE
    // Clustering result 
    printf("New centres after clustering\n");     
    for (uint i=0; i<k; i++) {
        printf("%d: ",i);
        for (uint d=0; d<D-1; d++) {
            printf("%d ",centre_output[i].position_short.value[d]);
        }
        printf("%d\n",centre_output[i].position_short.value[D-1]);
    }
    #endif
    
    //write_data_set_info(tree_memory_lo_lo, tree_memory_lo_hi, tree_memory_hi_lo, tree_memory_hi_hi, fidx);

    // free allocated memory
    delete data_points;
    delete data_output;
    delete centre_output;
    delete tree_memory;
    delete initial_centres;
    delete centr_idx;       

    return (EXIT_SUCCESS);
}

