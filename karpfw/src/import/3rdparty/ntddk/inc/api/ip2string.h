/*++ BUILD Version: 0006    // Increment this if a change has global effects

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    ip2string.h

Abstract:

    Include file for NT runtime routines that are callable by both
    kernel mode code in the executive and user mode code in various
    NT subsystems.

    This module contains definitions that are exposed in kit headers
    for use by external developers. Splited from ntrtl_x.h

Author:

    Zhihui Chen (stevewo) 03-May-2006

Environment:

    These routines are dynamically linked in the caller's executable and
    are callable in either kernel mode or user mode.


--*/

#ifndef __IP2STRING__
#define __IP2STRING__
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//
//  Some simple Rtl routines for IP address <-> string literal conversion
//

struct in_addr;
struct in6_addr;

NTSYSAPI
PSTR
NTAPI
RtlIpv4AddressToStringA (
    __in const struct in_addr *Addr,
    __out_ecount(16) PSTR S
    );

NTSYSAPI
PSTR
NTAPI
RtlIpv6AddressToStringA (
    __in const struct in6_addr *Addr,
    __out_ecount(46) PSTR S
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv4AddressToStringExA(
    __in const struct in_addr *Address,
    __in USHORT Port,
    __out_ecount_part(*AddressStringLength, *AddressStringLength) PSTR AddressString,
    __inout PULONG AddressStringLength
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv6AddressToStringExA(
    __in const struct in6_addr *Address,
    __in ULONG ScopeId,
    __in USHORT Port,
    __out_ecount_part(*AddressStringLength, *AddressStringLength) PSTR AddressString,
    __inout PULONG AddressStringLength
    );

NTSYSAPI
PWSTR
NTAPI
RtlIpv4AddressToStringW (
    __in const struct in_addr *Addr,
    __out_ecount(16) PWSTR S
    );

NTSYSAPI
PWSTR
NTAPI
RtlIpv6AddressToStringW (
    __in const struct in6_addr *Addr,
    __out_ecount(46) PWSTR S
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv4AddressToStringExW(
    __in const struct in_addr *Address,
    __in USHORT Port,
    __out_ecount_part(*AddressStringLength, *AddressStringLength) PWSTR AddressString,
    __inout PULONG AddressStringLength
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv6AddressToStringExW(
    __in const struct in6_addr *Address,
    __in ULONG ScopeId,
    __in USHORT Port,
    __out_ecount_part(*AddressStringLength, *AddressStringLength) PWSTR AddressString,
    __inout PULONG AddressStringLength
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv4StringToAddressA (
    IN PCSTR S,
    IN BOOLEAN Strict,
    OUT PCSTR *Terminator,
    OUT struct in_addr *Addr
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv6StringToAddressA (
    IN PCSTR S,
    OUT PCSTR *Terminator,
    OUT struct in6_addr *Addr
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv4StringToAddressExA (
    IN PCSTR AddressString,
    IN BOOLEAN Strict,
    OUT struct in_addr *Address,
    OUT PUSHORT Port
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv6StringToAddressExA (
    IN PCSTR AddressString,
    OUT struct in6_addr *Address,
    OUT PULONG ScopeId,
    OUT PUSHORT Port
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv4StringToAddressW (
    IN PCWSTR S,
    IN BOOLEAN Strict,
    OUT LPCWSTR *Terminator,
    OUT struct in_addr *Addr
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv6StringToAddressW (
    IN PCWSTR S,
    OUT PCWSTR *Terminator,
    OUT struct in6_addr *Addr
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv4StringToAddressExW (
    IN PCWSTR AddressString,
    IN BOOLEAN Strict,
    OUT struct in_addr *Address,
    OUT PUSHORT Port
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv6StringToAddressExW (
    IN PCWSTR AddressString,
    OUT struct in6_addr *Address,
    OUT PULONG ScopeId,
    OUT PUSHORT Port
    );

#ifdef UNICODE
#define RtlIpv4AddressToString RtlIpv4AddressToStringW
#define RtlIpv6AddressToString RtlIpv6AddressToStringW
#define RtlIpv4StringToAddress RtlIpv4StringToAddressW
#define RtlIpv6StringToAddress RtlIpv6StringToAddressW
#define RtlIpv6StringToAddressEx RtlIpv6StringToAddressExW
#define RtlIpv4AddressToStringEx RtlIpv4AddressToStringExW
#define RtlIpv6AddressToStringEx RtlIpv6AddressToStringExW
#define RtlIpv4StringToAddressEx RtlIpv4StringToAddressExW
#else
#define RtlIpv4AddressToString RtlIpv4AddressToStringA
#define RtlIpv6AddressToString RtlIpv6AddressToStringA
#define RtlIpv4StringToAddress RtlIpv4StringToAddressA
#define RtlIpv6StringToAddress RtlIpv6StringToAddressA
#define RtlIpv6StringToAddressEx RtlIpv6StringToAddressExA
#define RtlIpv4AddressToStringEx RtlIpv4AddressToStringExA
#define RtlIpv6AddressToStringEx RtlIpv6AddressToStringExA
#define RtlIpv4StringToAddressEx RtlIpv4StringToAddressExA
#endif // UNICODE

#ifdef __cplusplus
}       // extern "C"
#endif

#endif // __IP2STRING__

