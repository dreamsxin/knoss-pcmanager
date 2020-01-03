#pragma warning(disable:4214)   // bit field types other than int

#pragma warning(disable:4201)   // nameless struct/union
#pragma warning(disable:4115)   // named type definition in parentheses
#pragma warning(disable:4127)   // conditional expression is constant
#pragma warning(disable:4054)   // cast of function pointer to PVOID
#pragma warning(disable:4244)   // conversion from 'int' to 'BOOLEAN', possible loss of data

#define NTSTRSAFE_LIB
#define KANTIARP_POOL_TAG 'KSFW'

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <initguid.h>
#include <ndis.h>
#include <ntstrsafe.h>
#ifdef __cplusplus
}
#endif // __cplusplus

#include <evntrace.h>
#include "netwatch.h"
#include "main.h"
#include "antiarp/antiarpioctls.h"
#include "antiarp/arpcommondef.h"
#include "cpprt.h"
#include "wdmmap.h"
#include "dlist.h"
#include "kthread.h"
#include "kndisfirewall.h"
#include "math.h"
