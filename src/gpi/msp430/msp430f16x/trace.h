/***************************************************************************************************
 ***************************************************************************************************
 *
 *	Copyright (c) 2018, Networked Embedded Systems Lab, TU Dresden
 *	All rights reserved.
 *
 *	Redistribution and use in source and binary forms, with or without
 *	modification, are permitted provided that the following conditions are met:
 *		* Redistributions of source code must retain the above copyright
 *		  notice, this list of conditions and the following disclaimer.
 *		* Redistributions in binary form must reproduce the above copyright
 *		  notice, this list of conditions and the following disclaimer in the
 *		  documentation and/or other materials provided with the distribution.
 *		* Neither the name of the NES Lab or TU Dresden nor the
 *		  names of its contributors may be used to endorse or promote products
 *		  derived from this software without specific prior written permission.
 *
 *	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 *	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***********************************************************************************************//**
 *
 *	@file					gpi/msp430f16x/trace.h
 *
 *	@brief					TRACE settings for MSP430F16x
 *
 *	@version				$Id: d30e78dcd7cbd319534dda7a9dd27690ebf97f34 $
 *	@date					TODO
 *
 *	@author					Carsten Herrmann
 *
 ***************************************************************************************************

 	@details
	
	TODO

 **************************************************************************************************/

#ifndef __GPI_MSP430F16x_TRACE_H__
#define __GPI_MSP430F16x_TRACE_H__

//**************************************************************************************************
//***** Includes ***********************************************************************************

#include "gpi/platform_spec.h"

#include <msp430.h>

// post includes see below

//**************************************************************************************************
//***** Global (Public) Defines and Consts *********************************************************



//**************************************************************************************************
//***** Local (Private) Defines and Consts *********************************************************

// select whether TRACE functions internally use a DSR (delayed service routine)
// pro: better timing when using TRACE on interrupt level
// con: uses an interrupt (interrupts must be enabled, "asynchronous" execution)
#ifndef GPI_TRACE_USE_DSR
	#define GPI_TRACE_USE_DSR			1
#endif

//**************************************************************************************************
//***** Forward Class and Struct Declarations ******************************************************



//**************************************************************************************************
//***** Global Typedefs and Class Declarations *****************************************************



//**************************************************************************************************
//***** Global Variables ***************************************************************************



//**************************************************************************************************
//***** Prototypes of Global Functions *************************************************************

#ifdef __cplusplus
	extern "C" {
#endif



#ifdef __cplusplus
	}
#endif

//**************************************************************************************************
//***** Implementations of Inline Functions ********************************************************

#if GPI_TRACE_USE_DSR

	#define GPI_TRACE_DSR_VECTOR				TIMERA1_VECTOR
	
	static inline void gpi_trace_trigger_dsr()	{ TACCTL1 |= CCIS0;	}
	static inline void gpi_trace_mask_dsr()		{ TACCTL1 &= ~CCIE;	}
	static inline void gpi_trace_unmask_dsr()	{ TACCTL1 &= ~(CCIS0 | CCIFG); TACCTL1 |= CCIE;	}

#endif

//**************************************************************************************************
//***** Post Includes ******************************************************************************

// include the generic file not before the specific settings made above
#include GPI_PLATFORM_PATH(../msp430_common/trace.h)

//**************************************************************************************************
//**************************************************************************************************

#endif // __GPI_MSP430F16x_TRACE_H__
