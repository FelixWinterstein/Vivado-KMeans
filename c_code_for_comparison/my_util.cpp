/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: my_util.cpp
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
//#include <stdbool.h>
#include <math.h>
#include "my_util.h"

#define mytype coord_type

    //data_type point;
    coord_type_short count_short;
    data_type wgtCent;
    coord_type sum_sq;
    data_type_short midPoint_short;
    data_type_short bnd_lo_short;
    data_type_short bnd_hi_short;
    uint ileft, iright;
    uint pointAddr;

void conv_tree_node_to_int(kdTree_type in, uint *out_lo_lo, uint *out_lo_hi, uint *out_hi_lo, uint *out_hi_hi)
{
    uint tmp_lo, tmp_hi;
    
    // 1st
    tmp_lo = in.ileft;
    tmp_hi = in.iright;
    out_lo_lo[0] = tmp_lo;
    out_lo_hi[0] = tmp_hi;
    tmp_lo = in.pointAddr;
    tmp_hi = in.count_short;
    out_hi_lo[0] = tmp_lo;  
    out_hi_hi[0] = tmp_hi;
    
    //2nd
    tmp_lo = in.sum_sq;
    tmp_hi = in.midPoint_short.value[0];
    out_lo_lo[1] = tmp_lo;
    out_lo_hi[1] = tmp_hi;
    tmp_lo = in.midPoint_short.value[1];
    tmp_hi = in.midPoint_short.value[2];
    out_hi_lo[1] = tmp_lo;  
    out_hi_hi[1] = tmp_hi;
    
    //3rd
    tmp_lo = in.bnd_lo_short.value[0];
    tmp_hi = in.bnd_lo_short.value[1];
    out_lo_lo[2] = tmp_lo;
    out_lo_hi[2] = tmp_hi;
    tmp_lo = in.bnd_lo_short.value[2];
    tmp_hi = in.bnd_hi_short.value[0];
    out_hi_lo[2] = tmp_lo;  
    out_hi_hi[2] = tmp_hi;
    
    //4th
    tmp_lo = in.bnd_hi_short.value[1];
    tmp_hi = in.bnd_hi_short.value[2];
    out_lo_lo[3] = tmp_lo;
    out_lo_hi[3] = tmp_hi;
    tmp_lo = in.wgtCent.value[0];
    tmp_hi = in.wgtCent.value[1];
    out_hi_lo[3] = tmp_lo;  
    out_hi_hi[3] = tmp_hi;  
    
    //5th
    tmp_lo = in.wgtCent.value[2];
    tmp_hi = 0;
    out_lo_lo[4] = tmp_lo;
    out_lo_hi[4] = tmp_hi;
    tmp_lo = 0;
    tmp_hi = 0;
    out_hi_lo[4] = tmp_lo;  
    out_hi_hi[4] = tmp_hi;  
    
    /*
    out[0] = in.ileft;
    out[0] = in.iright;
    out[2] = in.pointAddr; 
    out[3] = in.count_short; 
    out[4] = in.sum_sq; 

    for (uint d=0; d<D; d++) {
        out[5+0*D+d] = in.midPoint_short.value[d];
    }    
    for (uint d=0; d<D; d++) {
        out[5+1*D+d] = in.bnd_lo_short.value[d];
    }    
    for (uint d=0; d<D; d++) {
        out[5+2*D+d] = in.bnd_hi_short.value[d];
    }    
    for (uint d=0; d<D; d++) {
        out[5+3*D+d] = in.wgtCent.value[d];
    }    
    */
}

void conv_int_to_tree_node(uint *in_lo_lo, uint *in_lo_hi, uint *in_hi_lo, uint *in_hi_hi, kdTree_type *out)
{
    
    uint tmp_lo, tmp_hi;
    
    // 1st
    tmp_lo = in_lo_lo[0];
    tmp_hi = in_lo_hi[0];
    out->ileft = tmp_lo;
    out->iright = tmp_hi;
    tmp_lo = in_hi_lo[0];
    tmp_hi = in_hi_hi[0];
    out->pointAddr = tmp_lo;
    out->count_short= tmp_hi;
    
    // 2nd
    tmp_lo = in_lo_lo[1];
    tmp_hi = in_lo_hi[1];
    out->sum_sq = tmp_lo;
    out->midPoint_short.value[0] = tmp_hi;
    tmp_lo = in_hi_lo[1];
    tmp_hi = in_hi_hi[1];
    out->midPoint_short.value[1] = tmp_lo;
    out->midPoint_short.value[2] = tmp_hi;  
    
    // 3rd
    tmp_lo = in_lo_lo[2];
    tmp_hi = in_lo_hi[2];
    out->bnd_lo_short.value[0] = tmp_lo;
    out->bnd_lo_short.value[1] = tmp_hi;
    tmp_lo = in_hi_lo[2];
    tmp_hi = in_hi_hi[2];
    out->bnd_lo_short.value[2] = tmp_lo;
    out->bnd_hi_short.value[0] = tmp_hi;
    
    // 4th
    tmp_lo = in_lo_lo[3];
    tmp_hi = in_lo_hi[3];
    out->bnd_hi_short.value[1] = tmp_lo;
    out->bnd_hi_short.value[2] = tmp_hi;
    tmp_lo = in_hi_lo[3];
    tmp_hi = in_hi_hi[3];
    out->wgtCent.value[0] = tmp_lo;
    out->wgtCent.value[1] = tmp_hi; 
    
    // 5th
    tmp_lo = in_lo_lo[4];
    out->wgtCent.value[2] = tmp_lo;
    
    /*
    out->ileft = in[0];
    out->iright = in[1];
    out->pointAddr = in[2];
    out->count_short = in[3];
    out->sum_sq = in[4];
    

    for (uint d=0; d<D; d++) {
        out->midPoint_short.value[d] = in[5+0*D+d];
    }    
    for (uint d=0; d<D; d++) {
        out->bnd_lo_short.value[d] = in[5+1*D+d];
    }    
    for (uint d=0; d<D; d++) {
        out->bnd_hi_short.value[d] = in[5+2*D+d];
    }    
    for (uint d=0; d<D; d++) {
        out->wgtCent.value[d] = in[5+3*D+d];
    }
    */
}



void make_data_points_file_name(char *result, uint n, uint k, uint d, double std_dev,uint index)
{    
    sprintf(result,"simulation/data_points_N%d_K%d_D%d_s%.2f.mat",n,k,d,std_dev,index); 
}

void make_initial_centres_file_name(char *result, uint n, uint k, uint d, double std_dev, uint index)
{    
    sprintf(result,"simulation/initial_centres_N%d_K%d_D%d_s%.2f_%d.mat",n,k,d,std_dev,index);
}


void make_clustering_centres_file_name(char *result, uint n, uint k, uint d, double std_dev, uint index)
{
    sprintf(result,"simulation/clustering_centres_N%d_K%d_D%d_s%.2f_%d.mat",n,k,d,std_dev,index);   
}

void make_data_set_info_file_name(char *result, uint n, uint k, uint d, double std_dev,uint index)
{
    sprintf(result,"simulation/data_set_info_N%d_K%d_D%d_s%.2f_%d.mat",n,k,d,std_dev,index);   
}


bool read_data_points(data_type_short *points, uint n, uint k, double std_dev, uint fidx)
{
    FILE *fp;    
    char filename[256];
    make_data_points_file_name(filename,n,k,D,std_dev,fidx);
    fp=fopen(filename, "r");
    char tmp[16];
    
    if (fp==NULL)
        return false;
    
    for (uint d=0; d<D; d++) {
        for (uint i=0;i<n;i++) {
            if (fgets(&tmp[0],16,fp) == 0) {
                fclose(fp);
                return false;                
            } else {
                //printf("%s\n",tmp);
                points[i].value[d] = (mytype)atoi(tmp); // assume coord_type==int                
            }
        }
    }
    
    fclose(fp);
    
    return true;
}


bool read_initial_centres(centre_type* centres, uint* centr_idx, uint n, uint k, double std_dev, uint fidx)
{
    FILE *fp;
    char filename[256];
    make_initial_centres_file_name(filename,n,k,D,std_dev,fidx);
    fp=fopen(filename, "r");
    char tmp[16];
    
    if (fp==NULL)
        return false;    
    
    for (uint j=0; j<D; j++) {
        for (uint i=0;i<k;i++) {
            if (fgets(&tmp[0],16,fp) == 0) {
                fclose(fp);
                return false;                
            } else {
                //printf("%s\n",tmp);
                (centres+i)->position_short.value[j]=(mytype)atoi(tmp); // assume coord_type==int
            }
        }
    } 
    
    for (uint i=0; i<k; i++) { 
        *(centr_idx+i) = i;
        (centres+i)->count_short = 0;
        (centres+i)->sum_sq = 0;
        for (uint j=0; j<D; j++) {
            (centres+i)->wgtCent.value[j] = 0;
        }        
    }
    
    fclose(fp);
    
    return true;
}



bool write_new_centres(centre_type* centres, uint* centr_idx, uint n, uint k, double std_dev, uint fidx)
{
    FILE *fp;
    char filename[256];
    make_clustering_centres_file_name(filename,n,k,D,std_dev,fidx);    
    fp=fopen(filename, "w");
    
    if (fp==NULL)
        return false;    
    
    for (uint j=0; j<D; j++) {
        for (uint i=0;i<k;i++) {            
            fprintf(fp,"%d\n",(centres+i)->position_short.value[j]);    
        }
    }

    fclose(fp);
    
    return true;
}




void write_tree_node_to_file(kdTree_ptr u, FILE *fp)
{
    fprintf(fp,"%d ",0);
    fprintf(fp,"%d ",0);
    fprintf(fp,"%d ",0);
    fprintf(fp,"%d ",u->count_short);
    fprintf(fp,"%d ",u->sum_sq);
    
    for (uint d=0; d<D; d++) {
        fprintf(fp,"%d ",u->bnd_lo_short.value[d]);
    }
    for (uint d=0; d<D; d++) {
        fprintf(fp,"%d ",u->bnd_hi_short.value[d]);
    }    
    for (uint d=0; d<D; d++) {
        fprintf(fp,"%d ",u->midPoint_short.value[d]);
    }
    for (uint d=0; d<D; d++) {
        fprintf(fp,"%d ",u->wgtCent.value[d]);
    } 
    
    fprintf(fp,"%d\n",u->pointAddr);
}

void generate_initial_centres(data_type_short *points, centre_type* centres, uint* centr_idx, uint n, uint k, uint seed)
{

    srand(seed);
    
    for (uint i=0;i<k;i++) {
        uint rnd_index = rand() % n; 
        for (uint d=0; d<D; d++) {
            (centres+i)->position_short.value[d] = points[rnd_index].value[d];
        }
    } 
    
    for (uint i=0; i<k; i++) { 
        *(centr_idx+i) = i;
        (centres+i)->count_short = 0;
        (centres+i)->sum_sq = 0;
        for (uint j=0; j<D; j++) {
            (centres+i)->wgtCent.value[j] = 0;
        }        
    }
}




coord_type fi_add(coord_type op1, coord_type op2)
{
    coord_type result = op1+op2;
    result = result >> 1;
    return result;
}

coord_type fi_sub(coord_type op1, coord_type op2)
{
    coord_type result = op1-op2;
    result = result >> 1;
    return result;
}

coord_type fi_mul(coord_type op1, coord_type op2)
{
    coord_type tmp_op1 = saturate(op1);
    coord_type tmp_op2 = saturate(op2);
    coord_type result = tmp_op1*tmp_op2;
    result = result >> FRACTIONAL_BITS;
    return result;
}

// find min/max in one dimension
void find_min_max(data_type_short *points, uint *idx , uint dim, uint n, coord_type *ret_min, coord_type *ret_max)
{    
    coord_type min = get_coord(points,idx,0,dim);
    coord_type max = get_coord(points,idx,0,dim);
    coord_type tmp;
    // inefficient way of searching the min/max
    for (int i=0; i<n; i++) {
        tmp = get_coord(points,idx,i,dim);        
        if (tmp < min) {
            min = tmp;
        }        
        if (tmp >= max) {
            max = tmp;
        }
    }

    *ret_min = min;
    *ret_max = max;
}


// inner product of p1 and p2
void dot_product(data_type p1,data_type p2, coord_type *r)
{
    coord_type tmp = 0;
    for (uint d=0;d<D;d++) {
        tmp += p1.value[d]*p2.value[d];       
    }
    *r = tmp;    
}


// inner product of p1 and p2
void dot_product_fi(data_type p1,data_type p2, coord_type *r)
{
    coord_type tmp = 0;
    for (uint d=0;d<D;d++) {
        coord_type tmp_op1 = saturate(p1.value[d]);
        coord_type tmp_op2 = saturate(p2.value[d]);
        coord_type tmp_mul = tmp_op1*tmp_op2;
        tmp += tmp_mul;       
    }
    *r = (tmp);    
}


// inner product of p1 and p2
void dot_product_fi_short(data_type_short p1,data_type_short p2, coord_type *r)
{
    data_type tmp_p1 = conv_short_to_long(p1);
    data_type tmp_p2 = conv_short_to_long(p2);
    coord_type tmp = 0;
    for (uint d=0;d<D;d++) {          
        
        coord_type tmp_op1 = saturate(tmp_p1.value[d]);
        coord_type tmp_op2 = saturate(tmp_p2.value[d]);
        coord_type tmp_mul = tmp_op1*tmp_op2;
        tmp += tmp_mul;       
    }
    *r = (tmp);    
}


// inner product of p1 and p2
void dot_product_fi_mixed(data_type_short p1,data_type p2, coord_type *r)
{
    data_type tmp_p1 = conv_short_to_long(p1);
    coord_type tmp = 0;
    for (uint d=0;d<D;d++) {
            
        coord_type tmp_op1 = saturate(tmp_p1.value[d]);
        coord_type tmp_op2 = saturate(p2.value[d]);           
        
        coord_type tmp_mul = tmp_op1*tmp_op2;
        tmp += tmp_mul;       
    }
    *r = (tmp);    
}



// bounding box is characterised by two points: low and high corner
void compute_bounding_box(data_type_short *points, uint *idx, uint n, data_type_short *bnd_lo, data_type_short *bnd_hi)
{
    coord_type max;
    coord_type min;
    for (uint i=0;i<D;i++) {
        find_min_max(points,idx,i,n,&min,&max);
        bnd_lo->value[i] = min;
        bnd_hi->value[i] = max;
    }
}


/*
 * The splitting routine is essentially a median search,
 * i.e. finding the median and split the array about it.
 * There are several algorithms for the median search
 * (an overview is given at http://ndevilla.free.fr/median/median/index.html):
 * - AHU (1)
 * - WIRTH (2)
 * - QUICKSELECT (3)
 * - TORBEN (4)
 * (1) and (2) are essentially the same in recursive and non recursive versions.
 * (2) is among the fastest in sequential programs.
 * (3) is similar to what quicksort uses and is as fast as (2).
 * Both (2) and (3) require permuting array elements.
 * (4) is significantly slower but only reads the array without modifying it.
 * The implementation below is a simplified version of (2).
 
*/
void split_bounding_box(data_type_short *points, uint *idx, uint n, data_type_short *bnd_lo, data_type_short *bnd_hi, uint *n_lo, uint *cdim, coord_type *cval)
{
    // search for dimension with longest egde
    coord_type longest_egde = bnd_hi->value[0] - bnd_lo->value[0];    
    uint dim = 0;
    
    for (uint d=0; d<D; d++) {        
        coord_type tmp = bnd_hi->value[d] - bnd_lo->value[d];
        if (longest_egde < tmp) {
            longest_egde = tmp;
            dim = d;
        }            
    }

    *cdim = dim;
    
    coord_type ideal_threshold = (bnd_hi->value[dim] + bnd_lo->value[dim]) / 2;    
    coord_type min,max;
    
    find_min_max(points,(idx+0),dim,n,&min,&max);
    
    coord_type threshold = ideal_threshold;
    
    if (ideal_threshold < min) {
        threshold = min;
    } else if (ideal_threshold > max) {
        threshold = max;
    }
    
    *cval = threshold;
    
    // Wirth's method
    int l = 0;
    int r = n-1;
       
    for(;;) {               // partition points[0..n-1]
    while (l < n && get_coord(points,idx+0,l,dim) < threshold) {
            l++;
        }
    while (r >= 0 && get_coord(points,idx+0,r,dim) >= threshold) {
            r--;
        }
    if (l > r) break; // avoid this
    coord_swap(idx+0,l,r);
    l++; r--;
    }
    
    uint br1 = l;           // now: data_points[0..br1-1] < threshold <= data_points[br1..n-1]
    r = n-1;
    for(;;) {               // partition pa[br1..n-1] about threshold
    while (l < n && get_coord(points,idx+0,l,dim) <= threshold) {
            l++;
        }
    while (r >= br1 && get_coord(points,idx+0,r,dim) > threshold) {
            r--;
        }
    if (l > r) break; // avoid this
    coord_swap(idx+0,l,r);
    l++; r--;
    }
    uint br2 = l;           // now: points[br1..br2-1] == threshold < points[br2..n-1]
    if (ideal_threshold < min) *n_lo = 0+1;
    else if (ideal_threshold > max) *n_lo = n-1;
    else if (br1 > n/2) *n_lo = br1;
    else if (br2 < n/2) *n_lo = br2;
    else *n_lo = n/2;
}

data_type_short conv_long_to_short(data_type p)
{
    data_type_short result;
    for (uint d=0; d<D; d++) {
        result.value[d] = (coord_type_short)p.value[d];
    }
    return result;    
}


data_type conv_short_to_long(data_type_short p)
{
    data_type result;
    for (uint d=0; d<D; d++) {
        result.value[d] = (coord_type)p.value[d];
    }
    return result;    
}


// compute the Euclidean distance between p1 and p2
void compute_distance(data_type p1, data_type p2, coord_type *dist)
{
        
    coord_type tmp_dist = 0;
    
    for (uint i=0; i<D; i++) {
        coord_type tmp = p1.value[i]-p2.value[i];
        coord_type tmp_mul = tmp*tmp;
        
        tmp_dist += tmp_mul;
    }
    *dist = tmp_dist;
}


// compute the Euclidean distance between p1 and p2
void compute_distance_fi(data_type p1, data_type p2, coord_type *dist)
{
        
    coord_type tmp_dist = 0;
    
    for (uint i=0; i<D; i++) {
        coord_type tmp = p1.value[i]-p2.value[i];
        coord_type tmp_mul = fi_mul(tmp,tmp);
        
        tmp_dist += tmp_mul;
    }
    *dist = (tmp_dist);
}

// compute the Euclidean distance between p1 and p2
void compute_distance_fi_short(data_type_short p1, data_type_short p2, coord_type *dist)
{
        
    coord_type tmp_dist = 0;    
    data_type tmp_p1 = conv_short_to_long(p1); 
    data_type tmp_p2 = conv_short_to_long(p2);    
    
    for (uint d=0; d<D; d++) {
        coord_type tmp = tmp_p1.value[d]-tmp_p2.value[d];
        coord_type tmp_mul = fi_mul(tmp,tmp);
        
        tmp_dist += tmp_mul;
    }
    *dist = (tmp_dist);
}


void compute_distance_fi_mixed(data_type p1, data_type_short p2, coord_type *dist)
{
        
    coord_type tmp_dist = 0;    
    data_type tmp_p2 = conv_short_to_long(p2);    
    
    for (uint d=0; d<D; d++) {
        coord_type tmp = p1.value[d]-tmp_p2.value[d];
        coord_type tmp_mul = fi_mul(tmp,tmp);
        
        tmp_dist += tmp_mul;
    }
    *dist = (tmp_dist);
}



// find centre closest to a given point
void closest_to_point(data_type p, centre_type *cntrs, uint *cntr_idxs , uint k, uint *idx)
{
    coord_type min_dist;
    uint min_idx = 0;
    compute_distance(conv_short_to_long(cntrs[cntr_idxs[0]].position_short), p, &min_dist);

    for (uint i=1; i<k; i++) {
        coord_type tmp_dist;
        compute_distance(conv_short_to_long(cntrs[cntr_idxs[i]].position_short), p, &tmp_dist);
        if (tmp_dist < min_dist) {
            min_dist = tmp_dist;
            min_idx = i;
        }
    }
    *idx = min_idx;
}


// find centre closest to a given point (direct deliver value and index)
void closest_to_point_direct_fi(data_type p, data_type *cntr_positions , uint k, uint *idx, data_type *c_position)
{
    coord_type min_dist;
    uint min_idx;
    data_type best_z;          
    
    min_idx = 0;
    best_z = cntr_positions[0];       
    
    compute_distance_fi(p, cntr_positions[0], &min_dist);

    for (uint i=1; i<k; i++) {
        coord_type tmp_dist;
        compute_distance_fi(p, cntr_positions[i], &tmp_dist);
        if (tmp_dist < min_dist) {
            min_dist = tmp_dist;
            min_idx = i;
            best_z = cntr_positions[i];
        }
    }
    *idx = min_idx;
    *c_position = best_z;
}


// find centre closest to a given point (direct deliver value and index)
void closest_to_point_direct_fi_short(data_type_short p, data_type_short *cntr_positions , uint k, uint *idx, data_type_short *c_position)
{
    coord_type min_dist;
    uint min_idx;
    data_type_short best_z;          
    
    
    min_idx = 0;
    best_z = cntr_positions[0];       
    
    compute_distance_fi_short(p, cntr_positions[0], &min_dist);

    for (uint i=1; i<k; i++) {
        coord_type tmp_dist;
        compute_distance_fi_short(p, cntr_positions[i], &tmp_dist);
        if (tmp_dist < min_dist) {
            min_dist = tmp_dist;
            min_idx = i;
            best_z = cntr_positions[i];
        }
    }
    *idx = min_idx;
    *c_position = best_z;
}


// find centre closest to a given point (direct deliver value and index)
void closest_to_point_direct_fi_mixed(data_type p, data_type_short *cntr_positions , uint k, uint *idx, data_type_short *c_position)
{
    coord_type min_dist;
    uint min_idx;
    data_type_short best_z;          
    
    min_idx = 0;
    best_z = cntr_positions[0];       
    
    compute_distance_fi_mixed(p, cntr_positions[0], &min_dist);

    for (uint i=1; i<k; i++) {
        coord_type tmp_dist;
        compute_distance_fi_mixed(p, cntr_positions[i], &tmp_dist);
        if (tmp_dist < min_dist) {
            min_dist = tmp_dist;
            min_idx = i;
            best_z = cntr_positions[i];
        }
    }
    *idx = min_idx;
    *c_position = best_z;
}


// check whether any point of bounding box is closer to z than to z*
void tooFar(data_type closest_cand, data_type cand, data_type bnd_lo, data_type bnd_hi, bool *too_far)
{
    /*
    coord_type boxDot = 0;
    coord_type ccDot;
    
    compute_distance(cand,closest_cand,&ccDot);   
    
    for (uint i = 0; i<D; i++) {
        coord_type ccComp = cand.value[i] - closest_cand.value[i];      
    if (ccComp > 0) { 
            boxDot += (bnd_hi.value[i] - closest_cand.value[i]) * ccComp;
    }
    else {                                  
            boxDot += (bnd_lo.value[i] - closest_cand.value[i]) * ccComp;
    }
    }
    *too_far = (ccDot > 2*boxDot);
    */
    
    // David Mount's code
    coord_type boxDot = 0;
    coord_type ccDot = 0;
            
    for (uint i = 0; i<D; i++) {
        coord_type ccComp = cand.value[i] - closest_cand.value[i];
    ccDot += ccComp * ccComp;       
    if (ccComp > 0) { 
            boxDot += (bnd_hi.value[i] - closest_cand.value[i]) * ccComp;
    }
    else {                                  
            boxDot += (bnd_lo.value[i] - closest_cand.value[i]) * ccComp;
    }        
    }
    *too_far = (ccDot > 2*boxDot);

}



// check whether any point of bounding box is closer to z than to z*
// this is a modified version of David Mount's code (http://www.cs.umd.edu/~mount/Projects/KMeans/ )
void tooFar_fi(data_type closest_cand, data_type cand, data_type bnd_lo, data_type bnd_hi, bool *too_far)
{

    coord_type boxDot = 0;
    coord_type ccDot = 0;
            
    for (uint i = 0; i<D; i++) {
        coord_type ccComp = cand.value[i] - closest_cand.value[i];
        coord_type tmp_mul = fi_mul(ccComp,ccComp);
    ccDot += tmp_mul;       
    if (ccComp > 0) { 
            coord_type tmp_diff2 = (bnd_hi.value[i] - closest_cand.value[i]);
            boxDot += fi_mul(tmp_diff2,ccComp);
    }
    else {  
            coord_type tmp_diff2 = (bnd_lo.value[i] - closest_cand.value[i]);
            boxDot += fi_mul(tmp_diff2,ccComp);
    }        
    }
    *too_far = (ccDot > 2*boxDot);
}


// check whether any point of bounding box is closer to z than to z*
// this is a modified version of David Mount's code (http://www.cs.umd.edu/~mount/Projects/KMeans/ )
void tooFar_fi_short(data_type_short closest_cand, data_type_short cand, data_type_short bnd_lo, data_type_short bnd_hi, bool *too_far)
{
    coord_type boxDot = 0;
    coord_type ccDot = 0;
    
    data_type tmp_closest_cand  = conv_short_to_long(closest_cand);
    data_type tmp_cand          = conv_short_to_long(cand);
    data_type tmp_bnd_lo        = conv_short_to_long(bnd_lo);
    data_type tmp_bnd_hi        = conv_short_to_long(bnd_hi);
            
    for (uint i = 0; i<D; i++) {
        coord_type ccComp = tmp_cand.value[i] - tmp_closest_cand.value[i];
        coord_type tmp_mul = fi_mul(ccComp,ccComp);
    ccDot += tmp_mul;       
    if (ccComp > 0) { 
            coord_type tmp_diff2 = (tmp_bnd_hi.value[i] - tmp_closest_cand.value[i]);
            boxDot += fi_mul(tmp_diff2,ccComp);
    }
    else {  
            coord_type tmp_diff2 = (tmp_bnd_lo.value[i] - tmp_closest_cand.value[i]);
            boxDot += fi_mul(tmp_diff2,ccComp);
    }        
    }
    *too_far = (ccDot > 2*boxDot);
}



// split bounding box independently of data points (saegusa's approach)
void split_bounding_box_simple(data_type *bnd_lo, data_type *bnd_hi, uint *cdim, coord_type *cval)
{
    // search for dimension with longest egde
    coord_type longest_egde = bnd_hi->value[0] - bnd_lo->value[0];   
    uint dim = 0;
    
    for (uint d=0; d<D; d++) {        
        coord_type tmp = bnd_hi->value[d] - bnd_lo->value[d];
        if (longest_egde < tmp) {
            longest_egde = tmp;
            dim = d;
        }            
    }
    if (longest_egde <= 0) {
        printf("oh oh\n");
    }
    
    *cdim = dim;
    
    coord_type ideal_threshold = (bnd_hi->value[dim] + bnd_lo->value[dim]) / 2;

    *cval = ideal_threshold;    
}


coord_type saturate(coord_type val)
{
    if (val > MAX_FIXED_POINT_VAL) {
        val = MAX_FIXED_POINT_VAL;
    } else if (val < MIN_FIXED_POINT_VAL) {
        val = MIN_FIXED_POINT_VAL;
    }
    return val;
}
