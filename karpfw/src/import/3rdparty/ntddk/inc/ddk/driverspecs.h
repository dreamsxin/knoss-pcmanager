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

/*****************************************************************************\
* Many annotations are context dependent.  They only apply to certain versions
* of Windows, or only to certain classes of driver.  These rules can be written
* using something like __drv_when(NTDDI_VERSION >= NTDDI_WINXP, ...)
* which causes the rule only to apply to Windows XP and later.  Many of these
* symbols are already defined in various Windows headers.
*
* To facilitate using this sort of conditional rule, we collect here the
* various known symbols that are (or reasonably might) be used in such
* a conditional annotation.  Some are speculative in that the symbol has
* not yet been defined because there are no known uses of it yet.
*
* Where the symbol already exists its relevant header is
* noted below (excluding the "really well known" ones).
*
* Each symbol is listed with the currently known possible values.
*
* Some symbols are marked as #define symbols -- they are used with #ifdef
* operators only.  To use them in __drv_when, use something like
* __drv_when(__drv_defined(NT), ...).
*
* WDK Version (copied for convenience from sdkddkver.h)
*     NTDDI_VERSION: NTDDI_WIN2K NTDDI_WIN2KSP1 NTDDI_WIN2KSP2 NTDDI_WIN2KSP3 
*                    NTDDI_WIN2KSP4 NTDDI_WINXP NTDDI_WINXPSP1 NTDDI_WINXPSP2 
*					 NTDDI_WS03 NTDDI_WS03SP1 NTDDI_VISTA
*     The WDK version is taken as the WDM version as well.
*
* OS Version: (copied for convenience from sdkddkver.h)
*     _WIN32_WINNT: _WIN32_WINNT_NT4 _WIN32_WINNT_WIN2K _WIN32_WINNT_WINXP 
*                   _WIN32_WINNT_WS03 _WIN32_WINNT_LONGHORN
*     WINVER: 0x030B 0x0400 0x0500 0x0600
*     NT (#define symbol)
* (sdkddkver.h also defines symbols for IE versions should they be needed.)
*
* Compiler Version:
*	  _MSC_VER: too many to list.
*	  _MSC_FULL_VER: too many to list.
*
* KMDF Version:  (Currently defined/used only in makefiles.)
*     KMDF_VERSION: 1     
*
* UMDF Version:  (Currently defined/used only in makefiles.)
*     UMDF_VERSION: 1     
*
* Architecture kinds:
*     __WIN64 (#define symbols)
*     _X86_
*     _AMD64_
*     _IA64_ 
*
* Machine Architectures:
*     _M_IX86
*     _M_AMD64
*     _M_IA64
*
* Driver Kind (NYI: "not yet implemented")
*   Typically these will be defined in the most-common header for a 
*   particular driver (or in individual source files if appropriate).
*   These are not intended to necessarily be orthogonal: more than one might 
*   apply to a particular driver.
*     _DRIVER_TYPE_BUS: 1                // NYI
*     _DRIVER_TYPE_FUNCTIONAL: 1         // NYI
*     _DRIVER_TYPE_MINIPORT: 1           // NYI
*     _DRIVER_TYPE_STORAGE: 1            // NYI
*     _DRIVER_TYPE_DISPLAY: 1            // NYI
*     _DRIVER_TYPE_FILESYSTEM: 1
*     _DRIVER_TYPE_FILESYSTEM_FILTER: 1
*
* NDIS driver version: (see ndis.h for much more detail.)
*   These can be used to both identify an NDIS driver and to check the version.
*     NDIS40 NDIS50 NDIS51 NDIS60 (#defined symbols)
*     NDIS_PROTOCOL_MAJOR_VERSION.NDIS_PROTOCOL_MINOR_VERSION: 4.0 5.0 5.1 6.0
*     And many others in ndis.h (including MINIPORT)
*
\*****************************************************************************/

#include "SpecStrings.h"
#include "sdv_driverspecs.h"
#if _MSC_VER > 1000 // [
#pragma once
#endif // ]

#if !defined(_MSC_EXTENSIONS) && !defined(_PREFAST_) && !defined(OACR)

	#define __internal_kernel_driver /* nothing */

#else

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
#endif // ]


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

	// core macros: these provide syntatic wrappers to make other uses
	// simpler.
	// (Note: right now we can't safely use the ellipsis (...) macro
	// syntax.  If we could then '##__drv_nop(annotes)' below could be 
	// simply 'annotes', and we could code __$drv_group as __$drv_group(...) 
	// in the "expands to nothing" case.)
	//
	// For example:
	//	 __drv_in(__drv_nonconstant __setsIRQL)
	
	#define __drv_deref(annotes) __deref __$drv_group(##__drv_nop(annotes))
	#define __drv_in(annotes) __pre __$drv_group(##__drv_nop(annotes))
	#define __drv_in_deref(annotes) __pre __deref __$drv_group(##__drv_nop(annotes))
	#define __drv_out(annotes) __post __$drv_group(##__drv_nop(annotes))
	#define __drv_out_deref(annotes) __post __deref __$drv_group(##__drv_nop(annotes))
	#define __drv_when(cond, annotes)										\
	  __drv_declspec("SAL_when(" SPECSTRINGIZE(cond) ")") __$drv_group(##__drv_nop(annotes))
	#define __drv_at(expr,annotes)\
	  __drv_declspec("SAL_at(" SPECSTRINGIZE(expr) ")") __$drv_group(##__drv_nop(annotes))

	#define __drv_fun(annotes) __drv_at(return,##__drv_nop(annotes))
	#define __drv_ret(annotes) __drv_at(return,##__drv_nop(annotes))
	#define __drv_arg(expr,annotes) __drv_at(expr,##__drv_nop(annotes))
	#define __drv_unit(p)													\
	  typedef int __$drv_unit_##p __prefast_flag_kernel_driver_mode;

	// Internal macros for convenience
	#define __$drv_unit_internal_kernel_driver								\
		 __drv_declspec("SAL_internal_kernel_driver")

	//
	// __drv_unit
	//
	// Flags for compilation units that indicated specifically what kind of
	// code it is.
	// These should be coded as early as possible in any compilation unit
	// (.c/.cpp file) that doesn't get the correct default.	 Whether before
	// or after __internal_kernel_driver is immaterial as long as it will
	// successfully parse.
	//
	// Indicate that the code is kernel, but not driver, code.
	#define __$drv_unit_kernel_code											\
			__drv_declspec("SAL_kernel")  __drv_declspec("SAL_nodriver")

	// Indicate that the code is kernel, driver, code.
	#define __$drv_unit_kernel_driver										\
			__drv_declspec("SAL_kernel") __drv_declspec("SAL_driver")

	// Indicate that the code is a user mode driver.
	#define __$drv_unit_user_driver											\
			__drv_declspec("SAL_nokernel") __drv_declspec("SAL_driver")

	// Indicate that the code is ordinary user mode code.
	#define __$drv_unit_user_code											\
			__drv_declspec("SAL_nokernel")	__drv_declspec("SAL_nodriver")


	// These are needed for backwards compatability.
	#ifndef __internal_kernel_driver

	#define __internal_kernel_driver   __drv_unit(internal_kernel_driver)
	#define __kernel_code			   __drv_unit(kernel_code)
	#define __kernel_driver			   __drv_unit(kernel_driver)
	#define __user_driver			   __drv_unit(user_driver)
	#define __user_code				   __drv_unit(user_code)

	#endif

	// ---------------------------------------------------------------------
	// Syntatic utilities:
	// 
	// Needed to make the annotations convenient to use.
	//
	// So we can use a macro name that might be used in #ifdef context,
	// where it's defined with no value.  
	// This should only be used inside a __drv_when condition.
	//
	#define __drv_defined(x) macroDefined$( #x )

	// ---------------------------------------------------------------------
	// Callback properties:
	//
	// __drv_functionClass(x)
	//
	// Flag that the  the annotated function
	// is a member of that function class.	Some class names are recognized
	// by PREfast itself for special treatment.
	// This can be tested by the condition function inFunctionClass$()
	//
	#define __drv_functionClass(x)									     	\
		__post __drv_out(__drv_declspec("SAL_functionClass("#x")"))


	// ---------------------------------------------------------------------
	// Specialized properties:
	//
	// __drv_innerFunctionProperty(x)
	//
	// Flag that the annotated object has/needs the named property.
	// This should always be used with an enclosing macro that provides
	// the pre or post annotation, as needed.
	//
	#define __drv_innerFunctionProperty(x)									\
		__drv_declspec("SAL_property("SPECSTRINGIZE(x)")")


	// ---------------------------------------------------------------------
	// Resources:
	// 
	// __drv_acquiresResource(kind)
	// __drv_releasesResource(kind)
	// __drv_acquiresResourceGlobal(kind,param)
	// __drv_releasesResourceGlobal(kind,param)
	// __drv_mustHold(kind)
	// __drv_neverHold(kind)
	// __drv_mustHoldGlobal(kind,param)
	// __drv_neverHoldGlobal(kind,param)
	//
	// Flag that the annotated parameter acquires a resource of type kind.
	//
	#define __drv_acquiresResource(kind)									\
		__post __drv_declspec("SAL_acquire("#kind")")

	//
	// Flag that the annotated parameter releases a resource of type kind.
	//
	#define __drv_releasesResource(kind)									\
		__post __drv_declspec("SAL_release("#kind")")

	//
	// Flag that the annotated object acquires a global (otherwise anonymous)
	// resource of type kind named by param.
	//
	#define __drv_innerAcquiresGlobal(kind, param)							\
		__post __drv_declspec("SAL_acquireGlobal("#kind","					\
												 SPECSTRINGIZE(param\t)")")
	#define __drv_acquiresResourceGlobal(kind,param)						\
		__drv_innerAcquiresGlobal(kind, param)
	//
	// Flag that the annotated object acquires a global (otherwise anonymous)
	// resource of type kind named by param.
	//
	#define __drv_innerReleasesGlobal(kind, param)							\
		__post __drv_declspec("SAL_releaseGlobal(" #kind ","				\
												 SPECSTRINGIZE(param\t)")")
	#define __drv_releasesResourceGlobal(kind, param)						\
		__drv_innerReleasesGlobal(kind, param)
											  
	//
	// Flag that the annotated parameter must hold a resource of type kind
	//
	#define __drv_mustHold(kind)											\
		__pre __drv_declspec("SAL_mustHold("#kind")")

	//
	// Flag that the annotated object must hold a global resource
	// of type kind named by param.
	//
	#define __drv_innerMustHoldGlobal(kind, param)							\
		__pre __drv_declspec("SAL_mustHoldGlobal("#kind","					\
												 SPECSTRINGIZE(param\t)")")
	#define __drv_mustHoldGlobal(kind,param)								\
		__drv_innerMustHoldGlobal(kind, param)

	//
	// Flag that the annotated parameter must never hold a resource of type kind
	//
	#define __drv_neverHold(kind)											\
		__pre __drv_declspec("SAL_neverHold("#kind")")

	//
	// Flag that the annotated object must never hold a global resource
	// of type kind named by param.
	//
	#define __drv_innerNeverHoldGlobal(kind, param)							\
		__pre __drv_declspec("SAL_neverHoldGlobal("#kind","					\
												 SPECSTRINGIZE(param\t)")")
	#define __drv_neverHoldGlobal(kind,param)								\
		__drv_innerNeverHoldGlobal(kind, param)


	// ---------------------------------------------------------------------
	// Maintenance of IRQL values
	//
	// __drv_setsIRQL(irql)
	// __drv_raisesIRQL(irql)
	// __drv_requiresIRQL(irql)
	// __drv_maxIRQL(irql)
	// __drv_minIRQL(irql)
	// __drv_savesIRQL
	// __drv_restoresIRQL
	// __drv_savesIRQLGlobal(kind,param)
	// __drv_restoresIRQLGlobal(kind,param)
	// __drv_minFunctionIRQL(irql)
	// __drv_maxFunctionIRQL(irql)
	// __drv_useCancelIRQL
	// __drv_sameIRQL

	// 
	// The funciton exits at IRQL irql
	//
	#define __drv_setsIRQL(irql)											\
		__post __drv_declspec("SAL_IRQL("SPECSTRINGIZE(irql)")")

	// 
	// The funciton exits at IRQL irql, but this may only raise the irql.
	//
	#define __drv_raisesIRQL(irql)											\
		__post __drv_declspec("SAL_raiseIRQL("SPECSTRINGIZE(irql)")")

	// 
	// The called function must be entered at IRQL level
	//
	#define __drv_requiresIRQL(irql)										\
		__pre __drv_declspec("SAL_IRQL("SPECSTRINGIZE(irql)")")


	// 
	// The maximum IRQL at which the function may be called.
	//
	#define __drv_maxIRQL(irql)												\
		__pre __drv_declspec("SAL_maxIRQL("SPECSTRINGIZE(irql)")")
	// 
	// The minimum IRQL at which the function may be called.
	//
	#define __drv_minIRQL(irql)												\
		__pre __drv_declspec("SAL_minIRQL("SPECSTRINGIZE(irql)")")

	// 
	// The current IRQL is saved in the annotated parameter
	//
	#define __drv_savesIRQL __post __drv_declspec("SAL_saveIRQL")

	// 
	// The current IRQL is saved in the (otherwise anonymous) global object
	// identified by kind and further refined by param.
	//
	#define __drv_savesIRQLGlobal(kind,param)								\
		__post __drv_declspec("SAL_saveIRQLGlobal("#kind","					\
										   SPECSTRINGIZE(param\t)")")

	// 
	// The current IRQL is restored from the annotated parameter
	//
	#define __drv_restoresIRQL __post __drv_declspec("SAL_restoreIRQL")

	// 
	// The current IRQL is restored from the (otherwise anonymous) global object
	// identified by kind and further refined by param.
	//
	#define __drv_restoresIRQLGlobal(kind,param)							\
		__post __drv_declspec("SAL_restoreIRQLGlobal("#kind","				\
										   SPECSTRINGIZE(param\t)")")

	// The minimum IRQL to which the function can lower itself.	 The IRQL
	// at entry is assumed to be that value unless overridden.
	#define __drv_minFunctionIRQL(irql)\
		__pre __drv_innerFunctionProperty(minFunctionIrql=irql) 

	// The maximum IRQL to which the function can raise itself.
	#define __drv_maxFunctionIRQL(irql)\
		__pre __drv_innerFunctionProperty(maxFunctionIrql=irql) 

	// The function must exit with the same IRQL it was entered with.
	// (It may change it but it must restore it.)
	#define __drv_sameIRQL\
		__post __drv_declspec("SAL_sameIRQL")

	// The annotated parameter contains the cancelIRQL, which will be restored
	// by the called function.

	#define __drv_useCancelIRQL												\
		__post __drv_innerFunctionProperty(UseCancelIrql)

    // The annotated function clears the requirement that DoInitializeing
	// is cleared (or not).
	#define __drv_clearDoInit(yesNo)										\
		__post __drv_innerFunctionProperty(clearDoInit=yesNo)

	// ---------------------------------------------------------------------
	// Function and out parameter return values.
	//
	// __drv_valueIs(<list>)
	//
	// The function being annotated will return each of the specified values
	// during simulation.  The items in the list are <relational op><constant>,
	// e.g. ==0 or <0.
	// This is a ; separated list of values.  The internal parser will accept
	// a comma-separated list.  In the future __VA_ARGS__ could be used.
	// See the documentation for use of this.
	//
	#define __drv_valueIs(arglist)											\
			__post __drv_declspec("SAL_return("SPECSTRINGIZE(arglist)")")

	// ---------------------------------------------------------------------
	// Additional parameter checking.
	//
	// __drv_constant
	// __drv_nonConstant
	// __drv_strictTypeMatch(mode)
	// __drv_strictType(type,mode)
	//
	// The actual parameter must evaluate to a constant (not a const).
	//
	#define __drv_constant __pre __drv_declspec("SAL_constant")

	//
	// The actual parameter may never evaluate to a numeric constant 
	// (exclusive of a const symbol).
	//
	#define __drv_nonConstant __pre __drv_declspec("SAL_nonConstant")

	//
	// The actual parameter must match the type of the annotated formal
	// within the specifications set by mode.
	//
	#define __drv_strictTypeMatch(mode)										\
		__pre __drv_declspec("SAL_strictTypeMatch("SPECSTRINGIZE(mode)")")

	//
	// The actual parameter must match the type of typename (below) 
	// within the specifications set by mode.
	//
	#define __drv_strictType(typename,mode)									\
		__pre __drv_declspec("SAL_strictType("SPECSTRINGIZE(typename)","	\
											  SPECSTRINGIZE(mode)")")
	//
	//    The following modes are defined:
		#define __drv_typeConst   0    // constants of that type
		#define __drv_typeCond    1    // plus ?:
		#define __drv_typeBitset  2    // plus all operators
		#define __drv_typeExpr    3    // plus literal constants
	// 
	// The actual parameter must be data (not a pointer).  Used to
	// prevent passing pointers to pointers when pointers to structures
	// are needed (because &pXXX is a common error when pXXX is 
	// intended).
	#define __drv_notPointer  __pre __drv_declspec("SAL_mayBePointer(never)")
	//
	// Convenience for the most common form of the above.
	#define __drv_isObjectPointer __drv_deref(__drv_notPointer)

	// ---------------------------------------------------------------------
	// Memory management
	//
	// __drv_aliasesMem
	// __drv_allocatesMem
	// __drv_freesMem
	//
	// The annotated parameter is "kept" by the function, creating an
	// alias, and relieving any obligation to free the object.
	//
	#define __drv_aliasesMem __post __drv_declspec("SAL_IsAliased")

	//
	// Allocate/release memory-like objects.
	// Kind is unused, but should be "mem" for malloc/free
	// and "object" for new/delete.
	#define __drv_allocatesMem(kind) __post __drv_declspec("SAL_NeedsRelease")
	#define __drv_freesMem(kind)	 __post __drv_declspec("SAL_IsReleased")

	// ---------------------------------------------------------------------
	// Additional diagnostics
	//
	// __drv_preferredFunction
	// __drv_reportError
	//
	//
	// Function 'func' should be used for reason 'why'.	 Often used
	// conditionally.
	//
	#define __drv_preferredFunction(func,why)								\
		__pre __drv_declspec(												\
			"SAL_preferredFunction("SPECSTRINGIZE(func)","					\
											 #why ")")

	//
	// The error given by 'why' was detected.  Used conditionally.
	//
	#define __drv_reportError(why)											\
		__pre __drv_declspec("SAL_error(" #why ")")

	// ---------------------------------------------------------------------
	// Floating point save/restore:
	//
	// __drv_floatSaved
	// __drv_floatRestored
	// __drv_floatUsed
	//
	// The floating point hardware was saved (available to kernel)
	#define __drv_floatSaved __post __drv_declspec("SAL_floatSaved")

	//
	// The floating point hardware was restored (no longer available)
	#define __drv_floatRestored __post __drv_declspec("SAL_floatRestored")

	//
	// The function uses floating point.  Functions with floating point
	// in their type signature get this automatically.
	#define __drv_floatUsed __post __drv_declspec("SAL_floatUsed")

	// ---------------------------------------------------------------------
	// Usage:
	// 
	// __drv_interlocked
	// __drv_inTry
	// __drv_notInTry
	//
	// The parameter is used for interlocked instructions.
	#define __drv_interlocked __pre __drv_declspec("SAL_interlocked")

	// The function must be called inside a try block
	#define __drv_inTry __pre __drv_declspec("SAL_inTry")

	// The function must not be called inside a try block
	#define __drv_notInTry __pre __drv_declspec("SAL_notInTry")

	// ---------------------------------------------------------------------
	// FormatString:
	//
	// kind can be "printf", "scanf", "strftime" or "FormatMessage".
	#define __drv_formatString(kind)\
		__drv_declspec("SAL_IsFormatString(" #kind ")")
	
	// ---------------------------------------------------------------------
	// SDV support: see the SDV documentation for details

	// Identify dispatch callback types
	#define __drv_dispatchType(kindlist)\
		__drv_declspec("SAL_dispatchType("\
							#kindlist ")" )

	// Identify dispatch callback types - special case
	#define __drv_dispatchType_other\
		__drv_declspec("SAL_dispatchType(IRP_MJ_OTHER)" )

	// Identify completion callback types
	#define __drv_completionType(kindlist)\
		__drv_declspec("SAL_completionType("\
							#kindlist ")" )

	// Identify callback types (FDO or PDO)
	#define __drv_callbackType(kind)\
		__drv_declspec("SAL_callbackType("\
							#kind ")" )
	// ---------------------------------------------------------------------
	// Composite:

	//
	// Exclusive Resources
	#define __drv_acquiresExclusiveResource(kind)				\
		__drv_neverHold(kind)									\
		__drv_acquiresResource(kind)

	#define __drv_releasesExclusiveResource(kind)				\
		__drv_mustHold(kind)									\
		__drv_releasesResource(kind)

	#define __drv_acquiresExclusiveResourceGlobal(kind, param)	\
		__drv_neverHoldGlobal(kind, param)						\
		__drv_acquiresResourceGlobal(kind, param)

	#define __drv_releasesExclusiveResourceGlobal(kind, param)	\
		__drv_mustHoldGlobal(kind, param)						\
		__drv_releasesResourceGlobal(kind, param)

	//
	// CancelSpinLock
	#define __drv_acquiresCancelSpinLock						\
		__drv_innerNeverHoldGlobal(CancelSpinLock,)				\
		__drv_innerAcquiresGlobal(CancelSpinLock,)

	#define __drv_releasesCancelSpinLock						\
		__drv_innerMustHoldGlobal(CancelSpinLock,)				\
		__drv_innerReleasesGlobal(CancelSpinLock,)

	#define __drv_mustHoldCancelSpinLock						\
		__drv_innerMustHoldGlobal(CancelSpinLock,)

	#define __drv_neverHoldCancelSpinLock						\
		__drv_innerNeverHoldGlobal(CancelSpinLock,)


	//
	// CriticalRegion
	#define __drv_acquiresCriticalRegion						\
		__drv_innerNeverHoldGlobal(CriticalRegion,)				\
		__drv_innerAcquiresGlobal(CriticalRegion,)

	#define __drv_releasesCriticalRegion						\
		__drv_innerMustHoldGlobal(CriticalRegion,)				\
		__drv_innerReleasesGlobal(CriticalRegion,)

	#define __drv_mustHoldCriticalRegion						\
		__drv_innerMustHoldGlobal(CriticalRegion,)

	#define __drv_neverHoldCriticalRegion						\
		__drv_innerNeverHoldGlobal(CriticalRegion,)


	// Passing the cancel Irql to a utility function
	#define __drv_isCancelIRQL\
		__drv_useCancelIRQL													\
		__drv_at(return, __drv_innerMustHoldGlobal(CancelSpinLock,)			\
				 __drv_innerReleasesGlobal(CancelSpinLock,)					\
				 __drv_minFunctionIRQL(DISPATCH_LEVEL)						\
				 __drv_requiresIRQL(DISPATCH_LEVEL))


#endif

#ifdef	__cplusplus
}
#endif

