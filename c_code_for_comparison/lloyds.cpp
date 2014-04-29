/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: lloyds.cpp
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
//#include <stdbool.h>
#include <math.h>
#include "lloyds.h"


// kernel function of lloyd's algorithm
void lloyds(data_type_short *points, centre_type *centres, uint k, uint n, bool last_run, data_type_short *output_array)
{    

    
    data_type_short centre_positions[KMAX];
    
    for (uint i=0; i<k; i++) {
         centre_positions[i] = centres[i].position_short;
    }
    
    // consider all data points
    for (uint i=0; i<n; i++) {
        
        uint idx;        
        data_type_short closest_centre; 
        
        // search for closest centre to this point
        closest_to_point_direct_fi_short(points[i], centre_positions, k, &idx, &closest_centre);
        
        coord_type tmp_dist;
        compute_distance_fi_short(points[i], closest_centre, &tmp_dist); 

        centres[idx].count_short++;

        // update centre buffer with info of closest centre
        for (uint d=0; d<D; d++) {
            centres[idx].wgtCent.value[d] += points[i].value[d];
        }
        
        centres[idx].sum_sq += tmp_dist;
        
        if (last_run == true) {
            output_array[i] = closest_centre;
        }
    }

}
