/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: filtering_algorithm_util.h
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/

#ifndef FILTERING_ALGORITHM_UTIL_H
#define FILTERING_ALGORITHM_UTIL_H

#include <math.h>
#include "filtering_algorithm_top.h"


// helper functions
void set_coord_type_vector_item(coord_type_vector *a, const coord_type b, const uint idx);
void set_coord_type_vector_ext_item(coord_type_vector_ext *a, const coord_type_ext b, const uint idx);
coord_type get_coord_type_vector_item(const coord_type_vector a, const uint idx);
coord_type_ext get_coord_type_vector_ext_item(const coord_type_vector_ext a, const uint idx);

data_type conv_long_to_short(data_type_ext p);
data_type_ext conv_short_to_long(data_type p);
mul_input_type saturate_mul_input(coord_type_ext val);
coord_type_ext fi_mul(coord_type_ext op1, coord_type_ext op2);
coord_type_ext tree_adder(coord_type_ext *input_array); // function overloading
coord_type_ext tree_adder(coord_type_ext *input_array,const uint m);
void dot_product(data_type_ext p1,data_type_ext p2, coord_type_ext *r);
void compute_distance(data_type_ext p1, data_type_ext p2, coord_type_ext *dist);
void tooFar_fi(data_type closest_cand, data_type cand, data_type bnd_lo, data_type bnd_hi, bool *too_far);


#endif  /* FILTERING_ALGORITHM_UTIL_H */
