/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: filter_it.cpp
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
//#include <stdbool.h>
#include <math.h>
#include "filter_it.h"
#include "dyn_mem_alloc.h"

uint stack_counter;

// push pointer to tree node onto stack
uint push_node(stack_type *stack, uint sp, uint u)
{
    //stack_ptr new_node = malloc(sizeof(stack_type));   //new stack_type;
    //new_node->u = u;    
    //new_node->next = sp;
    //sp = new_node; 
    stack[sp].u = u;
    
    return sp+1;
}

// pop pointer to tree node from stack
uint pop_node(stack_type *stack, uint sp, uint *u)
{
    //stack_ptr tmp = sp;
    
    *u = stack[sp-1].u;
    
    //sp = sp->next;
    //free(tmp);    //delete tmp;
    //tmp = 0;
    
    return sp-1;
}

// push list pointer onto stack
uint cpush_node(cstack_type *cstack, uint sp, centre_set_ptr l, uint k, bool redundant, centre_set_ptr default_set, uint default_k)
{

    if (stack_counter > STACK_BOUND-2) {
        cstack[sp].list = default_set;
        cstack[sp].k = default_k;
        cstack[sp].redundant = false;
    } else {
        cstack[sp].list = l;
        cstack[sp].k = k;
        cstack[sp].redundant = redundant;
        stack_counter++; 
    }
    
    /*
    cstack_ptr new_entry = malloc(sizeof(cstack_type));   //new cstack_type;
    if (stack_counter > bound_index_list_size) {
        new_entry->list = default_set;
        new_entry->k = k;
    } else {
        new_entry->list = l;
        new_entry->k = k;        
    }
    new_entry->next = sp;
    sp = new_entry; 
    
    stack_counter++;
    */
    
    return sp+1;
}

// pop list pointer from stack
uint cpop_node(cstack_type *cstack, uint sp, centre_set_ptr *l, uint *k, bool *redundant)
{          
    
    *l = cstack[sp-1].list;
    *k = cstack[sp-1].k;
    *redundant = cstack[sp-1].redundant;
    stack_counter--;
    /*
    cstack_ptr tmp = sp;

    *l = tmp->list; // this might be a bit confusing... it is actually a pointer to a pointer
    *k = tmp->k;
        *redundant = tmp->redundant;
    // dispose first
    sp = sp->next;
    free(tmp);  //delete tmp;

    stack_counter--;
    */
    
    return sp-1;
}


// kernel function of the filtering algorithm
void filter_it( uint root, 
                kdTree_type *tree_memory,
                centre_type *centres,
                uint k,
                bool last_run, 
                data_type_short *output_array)
{
    centre_type centre_buffer[KMAX];    
    stack_type stack[NMAX];
    cstack_type cstack[NMAX];
       
    centre_set_type centre_set_heap[CENTRE_SET_HEAP_BOUND];
    uint freelist[CENTRE_SET_HEAP_BOUND];
    uint next_free_location;        
    
    //Initialisation    
    
    #ifdef CUSTOM_DYN_ALLOC
    init_allocator<uint>(freelist, &next_free_location, CENTRE_SET_HEAP_BOUND-1);
    #endif        
    
    for(uint i=0; i<k; i++) {
        for (uint d=0; d<D; d++) {
            centre_buffer[i].wgtCent.value[d] = 0;
        }
        centre_buffer[i].count_short = 0;
        centre_buffer[i].sum_sq = 0;
    }    

    uint visited_nodes = 0;
    uint node_centre_pairs = 0;
    
    centre_set_ptr cntr_idxs;
    centre_set_type *cntr_idxs_ptr;
    
    #ifdef CUSTOM_DYN_ALLOC    
    cntr_idxs = malloc<uint>(freelist, &next_free_location);        
    cntr_idxs_ptr = make_pointer<centre_set_type>(centre_set_heap, cntr_idxs);
    #else
    cntr_idxs = new centre_set_type;
    cntr_idxs_ptr = cntr_idxs;
    #endif
        
    for(uint i=0; i<k; i++) {
        centre_buffer[i] = centres[i];
        cntr_idxs_ptr->idx[i] = i;
    }
    
    uint st = 0;
    uint cst = 0;
    stack_counter = 0;
    uint max_stack_counter = 0;
    uint centre_set_counter = 0;
    
    //End of Initialisation
    
    st=push_node(stack,st,root);
    cst = cpush_node(cstack, cst, cntr_idxs, k, false, cntr_idxs, k); // first set (default set) never gets freed
    
    while (st != 0) {
        
        // fetch head of stack
        uint u;
        st = pop_node(stack,st,&u);                        
        centre_set_ptr tmp_cntr_idxs;
        centre_set_type *tmp_cntr_idxs_ptr;
        uint tmp_k;
        bool redundant;
        cst = cpop_node(cstack,cst,&tmp_cntr_idxs,&tmp_k,&redundant);
        
        #ifdef CUSTOM_DYN_ALLOC
        tmp_cntr_idxs_ptr = make_pointer<centre_set_type>(centre_set_heap, tmp_cntr_idxs);
        #else
        tmp_cntr_idxs_ptr = tmp_cntr_idxs;
        #endif
        
        // buffer tree node
        kdTree_type tmp_u;
        tmp_u = tree_memory[u];     
              
        data_type_short tmp_centre_positions[KMAX];
        for (uint i=0; i<tmp_k; i++) {
            tmp_centre_positions[i] = centre_buffer[tmp_cntr_idxs_ptr->idx[i]].position_short;           
        }           
        
        // find  closest centre to various points
        data_type comp_point; 
        if ( (tmp_u.ileft == 0) && (tmp_u.iright == 0) ) {
            comp_point = tmp_u.wgtCent;
        } else {
            comp_point = conv_short_to_long(tmp_u.midPoint_short);
        }
        
        uint tmp_search_idx;
        data_type_short tmp_search_centre;        
        closest_to_point_direct_fi_mixed(comp_point, tmp_centre_positions, tmp_k, &tmp_search_idx, &tmp_search_centre); 
        data_type_short z_star = tmp_search_centre;
        uint idx_closest = tmp_search_idx;                

        centre_set_ptr new_cntr_idxs;
        centre_set_type *new_cntr_idxs_ptr;
        
        bool centre_set_bound_reached = centre_set_counter >= CENTRE_SET_HEAP_BOUND-2;

        uint new_k=0; 

        // allocate a new list
        if (!centre_set_bound_reached ) {
            #ifdef CUSTOM_DYN_ALLOC
            new_cntr_idxs = malloc<uint>(freelist, &next_free_location);        
            new_cntr_idxs_ptr = make_pointer(centre_set_heap, new_cntr_idxs);
            #else
            new_cntr_idxs = new centre_set_type;
            new_cntr_idxs_ptr = new_cntr_idxs;
            #endif
            centre_set_counter++;       

            // and copy candidates that survive pruning into it                            
            // candidate pruning   
            for (uint i=0; i<tmp_k; i++) {
                bool too_far;
                tooFar_fi_short(z_star, tmp_centre_positions[i], tmp_u.bnd_lo_short, tmp_u.bnd_hi_short, &too_far);
                if ( too_far==false ) {
                    new_cntr_idxs_ptr->idx[new_k] = tmp_cntr_idxs_ptr->idx[i];
                    new_k++;
                }
            }  
        }      
        
        // update sum_sq of centre
        //coord_type tmp1_1, tmp2_1;
        coord_type tmp1_2, tmp2_2;        
        
        data_type tmp_wgtCent = tmp_u.wgtCent;
        for (uint d=0; d<D; d++) {
            tmp_wgtCent.value[d] = tmp_wgtCent.value[d]>>FRACTIONAL_BITS;
        }
        
        // z_star == tmp_centre_positions[idx_closest] !
        dot_product_fi_mixed(z_star,tmp_wgtCent,&tmp1_2);
        dot_product_fi_short(z_star,z_star,&tmp2_2);
        
        coord_type tmp1, tmp2;
        uint tmp_idx;       
        
        tmp1 = tmp1_2;
        tmp2 = tmp2_2>>FRACTIONAL_BITS;
        tmp_idx = idx_closest;
        
    
        // compute distortion
        coord_type_short tmp_count = tmp_u.count_short;
        coord_type tmp3 = saturate(tmp2)*saturate(tmp_count);
        coord_type tmp_sum_sq = tmp_u.sum_sq+tmp3;
        tmp_sum_sq = tmp_sum_sq-2*tmp1;
        
        
        uint tmp_final_idx = tmp_cntr_idxs_ptr->idx[tmp_idx]; 

        // write back
        if ( ((tmp_u.ileft == 0) && (tmp_u.iright == 0)) || ((new_k == 1) && (last_run == false)) ) {        
            
            // weighted centroid of this centre
            for (uint d=0; d<D; d++) {
                centre_buffer[tmp_final_idx].wgtCent.value[d] += tmp_u.wgtCent.value[d];
            }
            // update number of points assigned to centre
            centre_buffer[tmp_final_idx].count_short += tmp_u.count_short;
            centre_buffer[tmp_final_idx].sum_sq += tmp_sum_sq; 
            
            if (!centre_set_bound_reached ) {
                #ifdef CUSTOM_DYN_ALLOC
                free<uint>(freelist, &next_free_location, new_cntr_idxs);
                #else
                delete new_cntr_idxs;
                #endif  
                centre_set_counter--;   
            }                   
            
            if (last_run == true) {    
                //printf("%d: %d\n",tmp_u.pointAddr,tmp_u.sum_sq);
                output_array[tmp_u.pointAddr] = z_star;
            }
        
        } else {
            uint left_child = tmp_u.ileft;  
            uint right_child = tmp_u.iright;  

            // push children onto stack                        
            st = push_node(stack,st,right_child);
            st = push_node(stack,st,left_child);

            // push centre lists for both children onto stack 
            if (!centre_set_bound_reached) {
                cst = cpush_node(cstack,cst,new_cntr_idxs,new_k,true, cntr_idxs,k);
                cst = cpush_node(cstack,cst,new_cntr_idxs,new_k,false, cntr_idxs,k);
            } else {
                cst = cpush_node(cstack,cst,cntr_idxs,k,false, cntr_idxs,k);
                cst = cpush_node(cstack,cst,cntr_idxs,k,false, cntr_idxs,k);
            }
        }
        
        // delete if centre set was used twice
        if (redundant == true) {
            #ifdef CUSTOM_DYN_ALLOC
            free<uint>(freelist, &next_free_location, tmp_cntr_idxs);
            #else
            delete tmp_cntr_idxs;
            #endif
            centre_set_counter--;
        }
        
        
        if (stack_counter > max_stack_counter)
            max_stack_counter = stack_counter;
        
        visited_nodes++;
        node_centre_pairs += tmp_k;
        
        
    }
    
    for(uint i=0; i<k; i++) {
        centres[i] = centre_buffer[i];
    }
    
    
}
