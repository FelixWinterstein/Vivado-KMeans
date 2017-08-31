/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: my_util.h
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/

#ifndef MY_UTIL_H
#define MY_UTIL_H

#ifdef  __cplusplus
extern "C" {
#endif

#define FILTERING_ALGO
#define VERBOSE
#define CUSTOM_DYN_ALLOC

#define D 3     // data dimensionality
#define NMAX 32768   // number of data points
#define KMAX 256   // number of centres
#define FILE_INDEX 1
#define L 30   // max number of iterations    

#define CENTRE_SET_HEAP_BOUND 256
#define STACK_BOUND 16384
    
#define FRACTIONAL_BITS 6
#define INTEGER_BITS 12
#define MAX_FIXED_POINT_VAL pow(2,INTEGER_BITS+FRACTIONAL_BITS-1)-1
#define MIN_FIXED_POINT_VAL -1*pow(2,INTEGER_BITS+FRACTIONAL_BITS-1)


typedef unsigned int uint;
typedef int coord_type;
//typedef short int coord_type_short;
typedef int coord_type_short;
 

// data point types
struct point_type {
    coord_type value[D];    
};
typedef struct point_type data_type;

// data point types short
struct point_type_short {
    coord_type_short value[D];    
};
typedef struct point_type_short data_type_short;

// centre types
struct c_type {
    data_type_short position_short;    
    data_type wgtCent; // sum of all points assigned to this centre
    coord_type sum_sq; // sum of norm of all points assigned to this centre
    coord_type_short count_short;
};
typedef struct c_type centre_type;

// tree node types
typedef struct kdTree kdTree_type;
typedef struct kdTree* kdTree_ptr;
struct kdTree {
    //data_type point;
    coord_type_short count_short;
    data_type wgtCent;
    coord_type sum_sq;
    data_type_short midPoint_short;
    data_type_short bnd_lo_short;
    data_type_short bnd_hi_short;
    uint ileft, iright;
    uint pointAddr;
};


typedef struct c_set_type centre_set_type;
#ifdef CUSTOM_DYN_ALLOC
typedef uint centre_set_ptr;
#else
typedef struct c_set_type* centre_set_ptr;
#endif
struct c_set_type {
    uint idx[KMAX];
};

// conversion
void conv_tree_node_to_int(kdTree_type in, uint *out_lo_lo, uint *out_lo_hi, uint *out_hi_lo, uint *out_hi_hi);
void conv_int_to_tree_node(uint *in_lo_lo, uint *in_lo_hi, uint *in_hi_lo, uint *in_hi_hi, kdTree_type *out);

//file IO
bool read_data_points(data_type_short *points, uint n, uint k, double std_dev, uint fidx);
bool write_new_centres(centre_type* centres, uint* centr_idx, uint n, uint k, double std_dev, uint fidx);

void write_tree_node_to_file(kdTree_ptr u, FILE *fp);
void generate_initial_centres(data_type_short *points, centre_type* centres, uint* centr_idx, uint n, uint k, uint seed);
bool read_initial_centres(centre_type* centres, uint* centr_idx, uint n, uint k, double std_dev, uint fidx);

// helper functions
void find_min_max(data_type_short *points, uint *idx , uint dim, uint n, coord_type *ret_min, coord_type *ret_max);

void dot_product(data_type p1,data_type p2, coord_type *r);
void dot_product_fi(data_type p1,data_type p2, coord_type *r);
void dot_product_fi_short(data_type_short p1,data_type_short p2, coord_type *r);
void dot_product_fi_mixed(data_type_short p1,data_type p2, coord_type *r);

void compute_bounding_box(data_type_short *points, uint *idx, uint n, data_type_short *bnd_lo, data_type_short *bnd_hi);

void split_bounding_box(data_type_short *points, uint *idx, uint n, data_type_short *bnd_lo, data_type_short *bnd_hi, uint *n_lo, uint *cdim, coord_type *cval);
void split_bounding_box_simple(data_type *bnd_lo, data_type *bnd_hi, uint *cdim, coord_type *cval);

void compute_distance(data_type p1, data_type p2, coord_type *dist);
void compute_distance_fi(data_type p1, data_type p2, coord_type *dist);
void compute_distance_fi_short(data_type_short p1, data_type_short p2, coord_type *dist);
void compute_distance_fi_mixed(data_type p1, data_type_short p2, coord_type *dist);

void closest_to_point(data_type p, centre_type *cntrs, uint *cntr_idxs , uint k, uint *idx);
void closest_to_point_direct_fi(data_type p, data_type *cntr_positions , uint k, uint *idx, data_type *c_position);
void closest_to_point_direct_fi_short(data_type_short p, data_type_short *cntr_positions , uint k, uint *idx, data_type_short *c_position);
void closest_to_point_direct_fi_mixed(data_type p, data_type_short *cntr_positions , uint k, uint *idx, data_type_short *c_position);

void tooFar(data_type closest_cand, data_type cand, data_type bnd_lo, data_type bnd_hi, bool *too_far);
void tooFar_fi(data_type closest_cand, data_type cand, data_type bnd_lo, data_type bnd_hi, bool *too_far);
void tooFar_fi_short(data_type_short closest_cand, data_type_short cand, data_type_short bnd_lo, data_type_short bnd_hi, bool *too_far);

coord_type saturate(coord_type val);
data_type_short conv_long_to_short(data_type p);
data_type conv_short_to_long(data_type_short p);

//helper macros
#define get_coord(points, indx, idx, dim) ( (points+*(indx+idx))->value[dim] )
#define coord_swap(indx, i1, i2) { uint tmp = *(indx+i1);\
                                    *(indx+i1) = *(indx+i2);\
                                    *(indx+i2) = tmp; }

#define GETTIME(t) gettimeofday(&t, NULL)
#define GETUSEC(e,s) (e.tv_sec*1e6 + e.tv_usec) - (s.tv_sec*1e6 + s.tv_usec) 



#ifdef  __cplusplus
}
#endif

#endif  /* MY_UTIL_H */

