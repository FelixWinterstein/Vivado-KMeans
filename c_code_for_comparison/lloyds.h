/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: lloyds.cpp
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/

#ifndef LLOYDS_H
#define LLOYDS_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "my_util.h"
    
    
void lloyds(data_type_short *points, centre_type *centres, uint k, uint n, bool last_run, data_type_short *output_array);

#ifdef  __cplusplus
}
#endif

#endif  /* FILTER_IT_H */

