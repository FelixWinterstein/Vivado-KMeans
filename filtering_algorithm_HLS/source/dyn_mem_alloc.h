/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: dyn_mem_alloc.h
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/


#ifndef _DYN_MEM_ALLOC_H_
#define _DYN_MEM_ALLOC_H_

#define NULL_PTR HEAP_SIZE-1



template <class address_type>
address_type malloc(address_type* flist, address_type* next_free_location)
{
    #pragma AP inline
    address_type address = *next_free_location;
    *next_free_location = flist[(uint)address];

    return address;
}

template <class address_type>
void free(address_type* flist, address_type* next_free_location, address_type address)
{
    #pragma AP inline
    flist[(uint)address] = *next_free_location;
    *next_free_location = address;
}

template <class address_type>
void init_allocator(address_type* flist, address_type* next_free_location, const address_type heapsize)
{
    #pragma AP inline
    init_allocator_loop: for (address_type i=0; i<=heapsize; i++) {
        #pragma AP pipeline II=1
        flist[(uint)i] = i+1;
        if (i==heapsize) {
            break;
        }
    }
    *next_free_location = 0;
}


template <class data_type>
data_type* make_pointer(data_type* mem, unsigned int offset) // offset must be int here (not address_type)
{
    #pragma AP inline
    return (mem+offset); //return pointer
}

template <class address_type, class data_type>
void readout_heapimage(data_type* mem, data_type* image, const uint heapsize)
{
    for (uint i=0; i<=heapsize; i++) {
        data_type tmp = mem[i];
        image[i] = tmp;
        if (i==heapsize) {
            break;
        }
    }
}

#endif
