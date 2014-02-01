/**********************************************************************
* Felix Winterstein, Imperial College London
*
* File: trace_address.cpp
*
* Revision 1.01
* Additional Comments: distributed under a BSD license, see LICENSE.txt
*
**********************************************************************/


#include "trace_address.h"

// mark an address as written, read, or read for the second time
void trace_address(centre_list_pointer address, bool write, bool *rdy_for_deletion, bool *trace_buffer)
{
	#pragma AP latency max=1
	#pragma AP inline

	bool tmp_val = trace_buffer[address];

	if (write == true) {
		trace_buffer[address] = true;
	} else {
		if (address != 0) {
			trace_buffer[address] = false;
		}
	}

	if ( (write == false) && (tmp_val == false) ) {
		*rdy_for_deletion = true;
	} else {
		*rdy_for_deletion = false;
	}

}
