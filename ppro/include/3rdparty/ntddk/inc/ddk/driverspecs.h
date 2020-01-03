/*****************************************************************************\
*                                                                             *
* DriverSpecs.h - markers for documenting the semantics of driver APIs        *
*                 See also <SpecStrings.h>                                    *
*                                                                             *
* Version 1.1.00                                                              *
*                                                                             *
* Copyright (c) Microsoft Corporation. All rights reserved.                   *
*                                                                             *
\*****************************************************************************/

/*****************************************************************************\
* NOTE																		  *
* NOTE																		  *
* NOTE																		  *
*   The macro bodies in this file are subject to change without notice.       *
*   Attempting to use the annotations in the macro bodies directly is not     *
*   supported.																  *
* NOTE																		  *
* NOTE																		  *
* NOTE																		  *
\*****************************************************************************/


#include "SpecStrings.h"
#if _MSC_VER > 1000 // [
#pragma once
#endif // ]

#ifdef  __cplusplus // [
extern "C" {
#endif  // ]


#if (_MSC_VER >= 1000) && !defined(__midl) && defined(_PREFAST_) // [
#define __drv_declspec(x) __declspec(x)
#define __$drv_group(annotes)												\
	  __drv_declspec("SAL_begin") annotes __drv_declspec("SAL_end")
#define __drv_nop(x) x
#else
#define __drv_declspec(x)
#define __$drv_group(x)
#endif

#if (_MSC_VER >= 1000) && !defined(__midl) && defined(_PREFAST_) // [

	// ---------------------------------------------------------------------
	// Processing mode selection:
	//
	// __internal_kernel_driver
	//
	// Flag for headers that indicates a probable driver.
	// This should only be coded in headers that are normally used 
	// as the "primary" header for a class of drivers.  It sets the
	// default to kernel mode driver.
	//
	#define __internal_kernel_driver 										\
		typedef int __drv_declspec("SAL_internal_kernel_driver") 			\
					__prefast_flag_kernel_driver_mode;

	//
	// __kernel_code
	// __kernel_driver
	// __user_driver
	// __user_code
	//
	// Flags for compilation units that indicated specifically what kind of 
	// code it is.
	// These should be coded as early as possible in any compilation unit 
	// (.c/.cpp file) that doesn't get the correct default.  Whether before
	// or after __internal_kernel_driver
	// 
	// Indicate that the code is kernel, but not driver, code.
	#define __kernel_code 													\
		typedef int __drv_declspec("SAL_kernel")  							\
					__drv_declspec("SAL_nodriver")							\
					__prefast_flag_kernel_driver_mode;

	// Indicate that the code is kernel, driver, code.
	#define __kernel_driver 												\
		typedef int __drv_declspec("SAL_kernel")   							\
					__drv_declspec("SAL_driver")  							\
					__prefast_flag_kernel_driver_mode;

	// Indicate that the code is a user mode driver.
	#define __user_driver 													\
		typedef int __drv_declspec("SAL_nokernel")							\
					__drv_declspec("SAL_driver")   							\
					__prefast_flag_kernel_driver_mode;

	// Indicate that the code is ordinary user mode code.
	#define __user_code 													\
		typedef int __drv_declspec("SAL_nokernel") 							\
					__drv_declspec("SAL_nodriver") 							\
					__prefast_flag_kernel_driver_mode;

#else

	#define __internal_kernel_driver
	#define __kernel_code 
	#define __kernel_driver
	#define __user_driver
	#define __user_code

#endif


#ifdef	__cplusplus
}
#endif

