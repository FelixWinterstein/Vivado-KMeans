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
#include <stdbool.h>
#include <math.h>
#include "build_kdTree.h"



#define mytype short int

void make_data_points_file_name(char *result, uint n, uint k, uint d, double std_dev)
{
    sprintf(result,"data_points_N%d_K%d_D%d_s%.2f.mat",n,k,d,std_dev);
}

void make_initial_centres_file_name(char *result, uint n, uint k, uint d, double std_dev, uint index)
{
    sprintf(result,"initial_centres_N%d_K%d_D%d_s%.2f_%d.mat",n,k,d,std_dev,index);
}


void make_clustering_centres_file_name(char *result, uint n, uint k, uint d, double std_dev, uint index)
{
    sprintf(result,"output_data/clustering_centres_N%d_K%d_D%d_s%.2f_%d.mat",n,k,d,std_dev,index);
}


// read input file
bool read_data_points(uint n, uint k, double std_dev, data_type* points, uint* index)
{

    FILE *fp;
    char filename[256];
    make_data_points_file_name(filename,n,k,D,std_dev);
    fp=fopen(filename, "r");

    if (fp == 0) {
        printf("failed to open file\n");
        return false;
    }
    char tmp[16];

    for (uint j=0; j<D; j++) {
        for (uint i=0;i<n;i++) {

            if (fgets(tmp,16,fp) == 0) {
                fclose(fp);
                return false;
            } else {
                //printf("%s\n",tmp);
                //points[i].value[j]=(mytype)atoi(tmp); // assume coord_type==short int
                coord_type b;
                b.VAL = (mytype)atoi(tmp); // assume coord_type==short int
                set_coord_type_vector_item(&points[i].value, b, j);
            }
            /*
            coord_type b;
            b.VAL = j*10; // assume coord_type==short int
            set_coord_type_vector_item(&points[i].value, b, j);
            */
        }
    }

    for (uint i=0;i<n;i++) {
        *(index+i) = i;
    }

    fclose(fp);

    return true;
}

// read input file
bool read_initial_centres(uint n, uint k, double std_dev, data_type *initial_centre_positions, uint* centr_idx)
{

    FILE *fp;
    char filename[256];
    make_initial_centres_file_name(filename,n,k,D,std_dev,FILE_INDEX);
    fp=fopen(filename, "r");
    if (fp == 0) {
        printf("failed to open file\n");
        return false;
    }
    char tmp[16];

    for (uint j=0; j<D; j++) {
        for (uint i=0;i<k;i++) {

            if (fgets(tmp,16,fp) == 0) {
                fclose(fp);
                return false;
            } else {
                //printf("%s\n",tmp);
                //initial_centre_positions[i].value[j] = (mytype)atoi(tmp); // assume coord_type==short int
                coord_type b;
                b.VAL = (mytype)atoi(tmp); // assume coord_type==short int
                set_coord_type_vector_item(&initial_centre_positions[i].value, b, j);
            }

        	/*
            coord_type b;
            b.VAL = 1; // assume coord_type==short int
            set_coord_type_vector_item(&initial_centre_positions[i].value, b, j);
            */
        }
    }

    fclose(fp);

    return true;
}


// find min/max in one dimension
void find_min_max(data_type* points, uint *idx , uint dim, uint n, coord_type *ret_min, coord_type *ret_max)
{
    coord_type min = get_coord(points,idx,0,dim);
    coord_type max = get_coord(points,idx,0,dim);
    coord_type tmp;
    // inefficient way of searching the min/max
    for (uint i=0; i<n; i++) {
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



// bounding box is characterised by two points: low and high corner
void compute_bounding_box(data_type* points, uint *idx, uint n, data_type *bnd_lo, data_type *bnd_hi)
{
    coord_type max;
    coord_type min;
    for (uint i=0;i<D;i++) {
        find_min_max(points,idx,i,n,&min,&max);
        set_coord_type_vector_item(&bnd_lo->value, min, i);
        set_coord_type_vector_item(&bnd_hi->value, max, i);
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
void split_bounding_box(data_type* points, uint *idx, uint n, data_type *bnd_lo, data_type *bnd_hi, uint *n_lo, uint *cdim, coord_type *cval)
{
    // search for dimension with longest egde
    coord_type longest_egde = get_coord_type_vector_item(bnd_hi->value,0) - get_coord_type_vector_item(bnd_lo->value,0);
    uint dim = 0;

    for (uint d=0; d<D; d++) {
        coord_type tmp = get_coord_type_vector_item(bnd_hi->value,d) - get_coord_type_vector_item(bnd_lo->value,d);
        if (longest_egde < tmp) {
            longest_egde = tmp;
            dim = d;
        }
    }

    *cdim = dim;

    coord_type ideal_threshold = (get_coord_type_vector_item(bnd_hi->value,dim) + get_coord_type_vector_item(bnd_lo->value,dim)) / 2;
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




// setup the basic properties of a tree node
void setup_tree_node(data_type* points, uint *idx, uint n, data_type bnd_lo, data_type bnd_hi, kdTree_ptr u)
{
    // compute cell mid point
    data_type tmp_mid;
    for (uint d=0;d<D;d++) {
        coord_type tmp1 = get_coord_type_vector_item(bnd_lo.value, d);
        coord_type tmp2 = get_coord_type_vector_item(bnd_hi.value, d);
        coord_type tmp3 = (tmp1+tmp2) / 2;
        set_coord_type_vector_item(&tmp_mid.value,tmp3,d);
    }

    // set the basic stuff (everything but the sums)
    u->midPoint = tmp_mid;
    u->count = (coord_type)n; // a bit dirty
    u->bnd_lo = bnd_lo;
    u->bnd_hi = bnd_hi;
    u->idx = idx;
    u->left = NULL_PTR;
    u->right = NULL_PTR;
}


// build up a kd-tree from a set of data points
node_pointer buildkdTree(data_type* points, uint *idx, uint n, data_type *bnd_lo, data_type *bnd_hi, node_pointer root_offset, kdTree_type *heap)
{
    uint debug_counter = 0;
    uint debug_leaf_counter = 0;
    uint debug_int_counter = 0;

    //define stack data structure
    stack_record bt_stack_array[N]; //STACK_SIZE=N
    uint bt_stack_pointer;

    uint dummy_pointer;
    init_stack(&bt_stack_pointer, &dummy_pointer);

    //node_pointer freelist[HEAP_SIZE];
    //node_pointer next_free_location;

    //init_allocator<node_pointer>(freelist, &next_free_location, HEAP_SIZE-1);
    node_pointer rel_int_node_addr  = (0 & ~(1<<(NODE_POINTER_BITWIDTH-1))) + root_offset;
    node_pointer rel_leaf_node_addr = (0 | (1<<(NODE_POINTER_BITWIDTH-1))) + root_offset;

    //node_pointer root = malloc<node_pointer>(freelist, &next_free_location);
    //kdTree_ptr root_ptr = make_pointer<kdTree_type>(heap, (uint)root);
    node_pointer root = rel_int_node_addr;
    kdTree_ptr root_ptr = make_pointer<kdTree_type>(heap, (uint)root);
    rel_int_node_addr++;

    debug_int_counter++;

    setup_tree_node(points, idx, n, *bnd_lo, *bnd_hi, root_ptr);

    uint stack_length = push_node(root, &bt_stack_pointer, bt_stack_array);

    while (stack_length != 0) {
        debug_counter++;
        // fetch head of stack
        node_pointer u;
        stack_length = pop_node(&u, &bt_stack_pointer, bt_stack_array);
        kdTree_ptr u_ptr = make_pointer<kdTree_type>(heap, (uint)u);

        //printf("%d %d %d %d\n",debug_counter-1,u.to_uint(),u_ptr->left.to_uint(),u_ptr->right.to_uint());

        uint count = (uint)u_ptr->count;
        uint *idx = u_ptr->idx;
        data_type bnd_lo = u_ptr->bnd_lo;
        data_type bnd_hi = u_ptr->bnd_hi;

        if (count>1) { // not a leaf node!

            // split point set
            uint cdim;
            coord_type cval;
            uint n_lo;
            split_bounding_box(points, idx, count, &bnd_lo, &bnd_hi, &n_lo, &cdim, &cval);

            // create new children

            node_pointer left_child;
            node_pointer right_child;
            if (n_lo == 1) {
                left_child = rel_leaf_node_addr;
                rel_leaf_node_addr++;
                debug_leaf_counter++;
            } else {
                left_child = rel_int_node_addr;
                rel_int_node_addr++;
                debug_int_counter++;
            }
            if (count-n_lo == 1) {
                right_child = rel_leaf_node_addr;
                rel_leaf_node_addr++;
                debug_leaf_counter++;
            } else {
                right_child = rel_int_node_addr;
                rel_int_node_addr++;
                debug_int_counter++;
            }
            kdTree_ptr left_child_ptr = make_pointer<kdTree_type>(heap, (uint)left_child);
            kdTree_ptr right_child_ptr = make_pointer<kdTree_type>(heap, (uint)right_child);

            // link parent and children
            u_ptr->left = left_child;
            u_ptr->right = right_child;

            // update bounding box
            data_type new_bnd_hi = bnd_hi;
            data_type new_bnd_lo = bnd_lo;

            set_coord_type_vector_item(&new_bnd_hi.value,cval,cdim);
            set_coord_type_vector_item(&new_bnd_lo.value,cval,cdim);

            // setup children
            setup_tree_node(points, idx,n_lo,bnd_lo,new_bnd_hi,left_child_ptr);
            setup_tree_node(points, idx+n_lo,count-n_lo,new_bnd_lo,bnd_hi,right_child_ptr);

            // push children onto stack
            stack_length = push_node(right_child,&bt_stack_pointer, bt_stack_array);
            stack_length = push_node(left_child,&bt_stack_pointer, bt_stack_array);
        }

    }

    update_sums(root, points, heap);
    scale_sums(root,heap);

    return root;
}


void dot_product_tb(data_type_ext p1,data_type_ext p2, coord_type_ext *r)
{
    coord_type_ext tmp = 0;
    for (uint d=0;d<D;d++) {
        tmp += get_coord_type_vector_item(p1.value,d)*get_coord_type_vector_item(p2.value,d);
    }
    *r = tmp;
}


// updates the wgtCent and sum_sq fields of every node
// problem: this requires post-order traversal
void update_sums(node_pointer root, data_type* points, kdTree_type *heap)
{
    //define stack data structure
    stack_record bt_stack_array[N]; //STACK_SIZE=N
    uint bt_stack_pointer;

    // re-init stack
    uint dummy_pointer;
    init_stack(&bt_stack_pointer, &dummy_pointer);

    uint counter = 1;

    node_pointer prev = NULL_PTR;
    kdTree_ptr prev_ptr;

    uint stack_length = push_node(root,&bt_stack_pointer, bt_stack_array);

    while (stack_length != 0) {

        // fetch head of stack (without removing it)
        node_pointer curr;
        lookahead_node(&curr,&bt_stack_pointer, bt_stack_array);
        kdTree_ptr curr_ptr = make_pointer<kdTree_type>(heap, (uint)curr);

        if (prev != NULL_PTR)
            prev_ptr = make_pointer<kdTree_type>(heap, (uint)prev);

        //is prev parent of curr?
        if ( (prev == NULL_PTR) || (prev_ptr->left == curr) || (prev_ptr->right == curr)) {
            if (curr_ptr->left != NULL_PTR) {
                stack_length = push_node(curr_ptr->left,&bt_stack_pointer, bt_stack_array);
            } else if (curr_ptr->right != NULL_PTR) {
                stack_length = push_node(curr_ptr->right,&bt_stack_pointer, bt_stack_array);
            }
        } else if (curr_ptr->left == prev) {
            if (curr_ptr->right != NULL_PTR) {
                stack_length = push_node(curr_ptr->right, &bt_stack_pointer, bt_stack_array);
            }
        } else {
            // remove curr from stack (could be a dummy read as well)
            node_pointer dummy;
            stack_length = pop_node(&curr, &bt_stack_pointer, bt_stack_array);

            data_type_ext tmp_wgtCent;
            coord_type_ext tmp_sum_sq;

            node_pointer lc = curr_ptr->left;
            node_pointer rc = curr_ptr->right;
            kdTree_ptr lc_ptr = make_pointer<kdTree_type>(heap, (uint)lc);
            kdTree_ptr rc_ptr = make_pointer<kdTree_type>(heap, (uint)rc);

            if ( (lc == NULL_PTR) && (rc == NULL_PTR) ) { //leaf node?
                tmp_wgtCent = conv_short_to_long(points[*(curr_ptr->idx)]);
                dot_product_tb(tmp_wgtCent,tmp_wgtCent,&tmp_sum_sq);
            } else if ( (lc != NULL_PTR) && (rc == NULL_PTR) ) {
                tmp_wgtCent = lc_ptr->wgtCent;
                tmp_sum_sq = lc_ptr->sum_sq;
            } else if ( (lc == NULL_PTR) && (rc != NULL_PTR) ) {
                tmp_wgtCent = rc_ptr->wgtCent;
                tmp_sum_sq = rc_ptr->sum_sq;
            } else {
                for (uint d=0; d<D; d++) {
                    coord_type_ext tmp = get_coord_type_vector_ext_item(lc_ptr->wgtCent.value,d) + get_coord_type_vector_ext_item(rc_ptr->wgtCent.value,d);
                    set_coord_type_vector_ext_item(&tmp_wgtCent.value,tmp,d);
                }
                tmp_sum_sq = lc_ptr->sum_sq + rc_ptr->sum_sq;
            }
            curr_ptr->wgtCent = tmp_wgtCent;
            curr_ptr->sum_sq = tmp_sum_sq;

            //printf("%d %d, %d\n",curr_ptr->count,curr_ptr->sum_sq,counter++);
        }

        // update prev
        prev = curr;
    }
}


// traverse the tree in pre-order and scale the sum_sq-field of each tree node
void scale_sums(node_pointer root, kdTree_type *heap)
{
    //define stack data structure
    stack_record bt_stack_array[N]; //STACK_SIZE=N
    uint bt_stack_pointer;

    // re-init stack
    uint dummy_pointer;
    init_stack(&bt_stack_pointer, &dummy_pointer);

    uint stack_length = push_node(root, &bt_stack_pointer, bt_stack_array);

    while (stack_length != 0) {

        // fetch head of stack
        node_pointer u;
        stack_length = pop_node(&u,&bt_stack_pointer, bt_stack_array);
        kdTree_ptr u_ptr = make_pointer<kdTree_type>(heap, (uint)u);

        u_ptr->sum_sq = u_ptr->sum_sq >> MUL_FRACTIONAL_BITS;
        //for (uint d=0; d<D; d++) {
        //    u_ptr->wgtCent.value[d] /= 1;
        //}

        if ((u_ptr->left != NULL_PTR) || (u_ptr->right != NULL_PTR)) {

            node_pointer left_child = u_ptr->left;
            node_pointer right_child = u_ptr->right;
            //kdTree_ptr left_child_ptr = make_pointer<kdTree_type>(heap, (uint)left_child);
            //kdTree_ptr right_child_ptr = make_pointer<kdTree_type>(heap, (uint)right_child);

            // push children onto stack
            stack_length = push_node(right_child, &bt_stack_pointer, bt_stack_array);
            stack_length = push_node(left_child, &bt_stack_pointer, bt_stack_array);
        }
    }
}


// write all fields of a tree node into a file
void write_tree_node_to_file(kdTree_type u, node_pointer idx, FILE *fp)
{
    fprintf(fp,"%d ",(uint)idx);
    fprintf(fp,"%d ",(uint)u.left);
    fprintf(fp,"%d ",(uint)u.right);
    fprintf(fp,"%d ",(int)u.count);
    fprintf(fp,"%d ",(int)u.sum_sq);

    for (uint d=0; d<D; d++) {
        fprintf(fp,"%d ",(int)get_coord_type_vector_item(u.bnd_lo.value,d));
    }
    for (uint d=0; d<D; d++) {
        fprintf(fp,"%d ",(int)get_coord_type_vector_item(u.bnd_hi.value,d));
    }
    for (uint d=0; d<D; d++) {
        fprintf(fp,"%d ",(int)get_coord_type_vector_item(u.midPoint.value,d));
    }
    for (uint d=0; d<D-1; d++) {
        fprintf(fp,"%d ",(int)get_coord_type_vector_ext_item(u.wgtCent.value,d));
    }
    fprintf(fp,"%d\n",(int)get_coord_type_vector_ext_item(u.wgtCent.value,D-1));
}

void make_tree_data_file_name(char *result, uint n, uint k, uint d, double std_dev)
{
    sprintf(result,"tree_data_N%d_K%d_D%d_s%.2f.mat",n,k,d,std_dev);
}


// traverse the kd-tree in pre-order and write the tree node data to a file
void readout_tree(bool write2file, uint n, uint k, double std_dev, node_pointer root, kdTree_type *heap, uint offset, kdTree_ptr image, node_pointer *image_addr)
{
    //define stack data structure
    stack_record bt_stack_array[N]; //STACK_SIZE=N
    uint bt_stack_pointer;

    // re-init stack
    uint dummy_pointer;
    init_stack(&bt_stack_pointer, &dummy_pointer);

    uint stack_length = push_node(root, &bt_stack_pointer, bt_stack_array);
    uint counter = offset;

    FILE *fp;

    if (write2file) {

        char filename[256];
        make_tree_data_file_name(filename,n,k,D,std_dev);
        if (offset == 0)
            fp = fopen(filename, "w");
        else
            fp = fopen(filename, "a");
    }

    while (stack_length != 0) {

        // fetch head of stack
        node_pointer u;
        stack_length = pop_node(&u,&bt_stack_pointer, bt_stack_array);
        kdTree_ptr u_ptr = make_pointer<kdTree_type>(heap, (uint)u);

        image_addr[counter] = u;
        image[counter] = *u_ptr;

        if (write2file) {
            write_tree_node_to_file(*u_ptr, u, fp);
        }

        counter++;

        if ((u_ptr->left != NULL_PTR) || (u_ptr->right != NULL_PTR)) {

            node_pointer left_child = u_ptr->left;
            node_pointer right_child = u_ptr->right;

            // push children onto stack
            stack_length = push_node(right_child, &bt_stack_pointer, bt_stack_array);
            stack_length = push_node(left_child, &bt_stack_pointer, bt_stack_array);
        }
    }

    //readout_heapimage<node_pointer,kdTree_type>(heap, image, heapsize);

    if (write2file) {

        fclose(fp);
    }

}



