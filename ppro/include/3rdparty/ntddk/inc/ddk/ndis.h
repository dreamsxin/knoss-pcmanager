/*++ BUILD Version: ????    // Increment this if a change has global effects

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    ndis.h

Abstract:

    This module defines the structures, macros, and functions available
    to NDIS drivers.

Revision History:

--*/

#if !defined(_NDIS_)
#define _NDIS_

#if !defined(NDIS_WDM)
#define NDIS_WDM        0
#endif


//
// Set BINARY_COMPATIBLE to 0 if it is not defined. Error out if user tries
// to build a Win9x binary compatible driver using this DDK.
//

#if !defined(BINARY_COMPATIBLE)
#define BINARY_COMPATIBLE 0
#else
#if (BINARY_COMPATIBLE != 0)
#error "can not build Win9x binary compatible drivers, please remove the definition for BINARY_COMPATIBLE or set it to 0"
#endif
#endif


//
// BEGIN INTERNAL DEFINITIONS
//


#include <ntddk.h>

//
// END INTERNAL DEFINITIONS
//
// The following definitions may be used by NDIS drivers, except as noted.
//

#ifndef __NET_PNP__
#define __NET_PNP__

#pragma once

//
//  PnP and PM event codes that can be indicated up to transports
//  and clients.
//
typedef enum _NET_PNP_EVENT_CODE
{
    NetEventSetPower,
    NetEventQueryPower,
    NetEventQueryRemoveDevice,
    NetEventCancelRemoveDevice,
    NetEventReconfigure,
    NetEventBindList,
    NetEventBindsComplete,
    NetEventPnPCapabilities,
    NetEventPause,
    NetEventRestart,
    NetEventPortActivation,
    NetEventPortDeactivation,
    NetEventIMReEnableDevice,
    NetEventMaximum
} NET_PNP_EVENT_CODE, *PNET_PNP_EVENT_CODE;

//
//  Networking PnP event indication structure.
//
typedef struct _NET_PNP_EVENT
{
    //
    //  Event code describing action to take.
    //
    NET_PNP_EVENT_CODE  NetEvent;

    //
    //  Event specific data.
    //
    PVOID               Buffer;

    //
    //  Length of event specific data.
    //
    ULONG               BufferLength;

    //
    //  Reserved values are for use by respective components only.
    //
    //  Note: these reserved areas must be pointer aligned.
    //  

    ULONG_PTR           NdisReserved[4];
    ULONG_PTR           TransportReserved[4];
    ULONG_PTR           TdiReserved[4];
    ULONG_PTR           TdiClientReserved[4];
} NET_PNP_EVENT, *PNET_PNP_EVENT;

//
//  The following structure defines the device power states.
//
typedef enum _NET_DEVICE_POWER_STATE
{
    NetDeviceStateUnspecified = 0,
    NetDeviceStateD0,
    NetDeviceStateD1,
    NetDeviceStateD2,
    NetDeviceStateD3,
    NetDeviceStateMaximum
} NET_DEVICE_POWER_STATE, *PNET_DEVICE_POWER_STATE;

#endif // __NET_PNP__

#pragma once

//
// Indicate that we're building for NT. NDIS_NT is always used for
// miniport builds.
//

#define NDIS_NT 1

#if defined(NDIS_DOS)
#undef NDIS_DOS
#endif

//
// Define status codes and event log codes.
//

#include <ntstatus.h>
#include <netevent.h>

//
// Define a couple of extra types.
//

#if !defined(_WINDEF_)      // these are defined in windows.h too
typedef signed int INT, *PINT;
typedef unsigned int UINT, *PUINT;
#endif

typedef UNICODE_STRING NDIS_STRING, *PNDIS_STRING;

//
// Portability extentions
//

#define NDIS_INIT_FUNCTION(_F)      alloc_text(INIT,_F)
#define NDIS_PAGABLE_FUNCTION(_F)   alloc_text(PAGE,_F)
#define NDIS_PAGEABLE_FUNCTION(_F)  alloc_text(PAGE,_F)

//
// This file contains the definition of an NDIS_OID as
// well as #defines for all the current OID values.
//

//
// Define NDIS_STATUS and NDIS_HANDLE here
//
typedef PVOID NDIS_HANDLE, *PNDIS_HANDLE;

typedef int NDIS_STATUS, *PNDIS_STATUS; // note default size

#if (!defined(NDIS_WRAPPER))

#if defined (NTDDI_VERSION)
#if (NTDDI_VERSION > NTDDI_LONGHORN)
#error "unsupported NTDDI_VERSION"
#endif
#endif

//
//
// error out if driver has defined these values
//
#if (defined(NDIS_MINIPORT_MAJOR_VERSION) || \
    (defined(NDIS_MINIPORT_MINOR_VERSION)) || \
    (defined(NDIS_PROTOCOL_MAJOR_VERSION)) || \
    (defined(NDIS_PROTOCOL_MINOR_VERSION)) || \
    (defined(NDIS_FILTER_MAJOR_VERSION)) || \
    (defined(NDIS_FILTER_MINOR_VERSION)))
#error "Driver is re-defining NDIS reserved macros"
#endif

#if (defined(NDIS_MINIPORT_DRIVER))
//
// for Miniports versions 5.0 and up, provide a consistent way to match
// Ndis version in their characteristics with their makefile defines
//
#if (defined(NDIS60_MINIPORT))
#define NDIS_MINIPORT_MAJOR_VERSION 6
#define NDIS_MINIPORT_MINOR_VERSION 0
#elif (defined(NDIS51_MINIPORT))
#define NDIS_MINIPORT_MAJOR_VERSION 5
#define NDIS_MINIPORT_MINOR_VERSION 1
#elif (defined(NDIS50_MINIPORT))
#define NDIS_MINIPORT_MAJOR_VERSION 5
#define NDIS_MINIPORT_MINOR_VERSION 0
#else
#error ("Only NDIS miniport drivers with version >= 5 are supported")
#endif

//
// disallow invalid major/minor combination
//
#if ((NDIS_MINIPORT_MAJOR_VERSION == 6) && (NDIS_MINIPORT_MINOR_VERSION != 0))
#error "Invalid Miniport version"
#elif ((NDIS_MINIPORT_MAJOR_VERSION == 5) && \
       (NDIS_MINIPORT_MINOR_VERSION != 1) && \
       (NDIS_MINIPORT_MINOR_VERSION != 0))
#error "Invalid Miniport major/minor version"
#endif



//
// make sure the target platform is consistent with miniport version
//
#if  ((NDIS_MINIPORT_MAJOR_VERSION == 6) && (NTDDI_VERSION != NTDDI_LONGHORN))
#error "Wrong NDIS or DDI version specified"
#elif ((NDIS_MINIPORT_MAJOR_VERSION == 5) && \
       (((NDIS_MINIPORT_MINOR_VERSION == 1) && (NTDDI_VERSION < NTDDI_WINXP)) || \
         ((NDIS_MINIPORT_MINOR_VERSION == 0) && (NTDDI_VERSION < NTDDI_WIN2K))))
#error "Wrong NDIS or DDI version specified"
#endif


#endif // NDIS_MINIPORT_DRIVER


#if (defined(NDIS30))
#error "Only NDIS Protocol drivers versions >= 4 are supported"
#endif

//
// for protocol versions 4.0 and up, provide a consistent way to match
// Ndis version in their characteristics with their makefile defines
//
//
// a protocol only or filter driver
//

#if (defined(NDIS60))
#define NDIS_PROTOCOL_MAJOR_VERSION 6
#define NDIS_PROTOCOL_MINOR_VERSION 0
#define NDIS_FILTER_MAJOR_VERSION 6
#define NDIS_FILTER_MINOR_VERSION 0
#elif (defined(NDIS51))
#define NDIS_PROTOCOL_MAJOR_VERSION 5
#define NDIS_PROTOCOL_MINOR_VERSION 1
#elif (defined(NDIS50))
#define NDIS_PROTOCOL_MAJOR_VERSION 5
#define NDIS_PROTOCOL_MINOR_VERSION 0
#elif(defined(NDIS40))
#define NDIS_PROTOCOL_MAJOR_VERSION 4
#define NDIS_PROTOCOL_MINOR_VERSION 0
#endif // (defined(NDIS60))

#if (!defined(NDIS_MINIPORT_DRIVER) && !defined (NDIS_PROTOCOL_MAJOR_VERSION))
//
// if it is not a miniport and 
// NDIS_PROTOCOL_MAJOR_VERSION is not defined, define it here
// 
#define NDIS40
#define NDIS_PROTOCOL_MAJOR_VERSION 4
#define NDIS_PROTOCOL_MINOR_VERSION 0
#endif // (!defined(NDIS_MINIPORT_DRIVER) && !defined (NDIS_PROTOCOL_MAJOR_VERSION))

#if defined (NDIS_PROTOCOL_MAJOR_VERSION)
//
// disallow invalid major/minor combination
//
#if ((NDIS_PROTOCOL_MAJOR_VERSION == 6) && \
     (NDIS_PROTOCOL_MINOR_VERSION != 0))
#error "Invalid Protocol version"
#elif ((NDIS_PROTOCOL_MAJOR_VERSION == 5) && \
     (NDIS_PROTOCOL_MINOR_VERSION != 1) && (NDIS_PROTOCOL_MINOR_VERSION != 0))
#error "Invalid Protocol version"
#elif ((NDIS_PROTOCOL_MAJOR_VERSION == 4) && (NDIS_PROTOCOL_MINOR_VERSION != 0))
#error "Invalid Protocol major/minor version"
#endif

//
// make sure the target platform is consistent with protocol version
// but don't stop NDIS protocol drivers older than 6.0 from specifying NTDDI_LONGHORN
//
#if  ((NDIS_PROTOCOL_MAJOR_VERSION == 6) && (NTDDI_VERSION < NTDDI_LONGHORN))
#error "Wrong NDIS or DDI version specified"
#endif
#endif // defined (NDIS_PROTOCOL_MAJOR_VERSION)
#endif // !NDIS_WRAPPER


//
// identify Legacy miniport drivers
//
#if !defined(NDIS_LEGACY_MINIPORT)
#if ((defined(NDIS_MINIPORT_DRIVER) && (NDIS_MINIPORT_MAJOR_VERSION < 6)) || NDIS_WRAPPER)
#define NDIS_LEGACY_MINIPORT    1
#else
#define NDIS_LEGACY_MINIPORT    0
#endif // ((defined(NDIS_MINIPORT_DRIVER) && (NDIS_MINIPORT_MAJOR_VERSION < 6)) || NDIS_WRAPPER)
#endif // !defined(NDIS_LEGACY_MINIPORT)


//
// identify Legacy protocol drivers
//
#if !defined(NDIS_LEGACY_PROTOCOL)
#if ((defined(NDIS_PROTOCOL_MAJOR_VERSION) && (NDIS_PROTOCOL_MAJOR_VERSION < 6)) || NDIS_WRAPPER)
#define NDIS_LEGACY_PROTOCOL    1
#else
#define NDIS_LEGACY_PROTOCOL    0
#endif // ((defined(NDIS_PROTOCOL_MAJOR_VERSION) && (NDIS_PROTOCOL_MAJOR_VERSION < 6)) || NDIS_WRAPPER)
#endif // !defined(NDIS_LEGACY_PROTOCOL)


//
// use something to identify legacy (pre NDIS 6 drivers) + NDIS itself
//
#if !defined(NDIS_LEGACY_DRIVER)
#if  (NDIS_LEGACY_MINIPORT || NDIS_LEGACY_PROTOCOL || NDIS_WRAPPER)
#define NDIS_LEGACY_DRIVER     1
#else
#define NDIS_LEGACY_DRIVER     0
#endif  // either protocol is legacy or miniport is legacy or this is NDIS
#endif // !defined(NDIS_LEGACY_DRIVER) 

//
// and something to identify new (LH and up) drivers + NDIS itself
//
#if !defined(NDIS_SUPPORT_NDIS6)
#if  ((defined (NDIS_MINIPORT_MAJOR_VERSION) && (NDIS_MINIPORT_MAJOR_VERSION >= 6)) || \
    (defined (NDIS60)) || NDIS_WRAPPER)
#define NDIS_SUPPORT_NDIS6      1
#else
#define NDIS_SUPPORT_NDIS6      0
#endif
#endif // !defined(NDIS_SUPPORT_NDIS6)

#include <ntddndis.h>

//
// Ndis defines for configuration manager data structures
//
typedef CM_MCA_POS_DATA NDIS_MCA_POS_DATA, *PNDIS_MCA_POS_DATA;
typedef CM_EISA_SLOT_INFORMATION NDIS_EISA_SLOT_INFORMATION, *PNDIS_EISA_SLOT_INFORMATION;
typedef CM_EISA_FUNCTION_INFORMATION NDIS_EISA_FUNCTION_INFORMATION, *PNDIS_EISA_FUNCTION_INFORMATION;

//
// Define an exported function.
//
#if defined(NDIS_WRAPPER)
#define EXPORT
#else
#define EXPORT DECLSPEC_IMPORT
#endif

#if NDIS_SUPPORT_NDIS6
typedef struct _NDIS_GENERIC_OBJECT
{
    NDIS_OBJECT_HEADER  Header;
    PVOID               Caller;
    PVOID               CallersCaller;
    PDRIVER_OBJECT      DriverObject;
} NDIS_GENERIC_OBJECT, *PNDIS_GENERIC_OBJECT;

EXPORT
PNDIS_GENERIC_OBJECT
NdisAllocateGenericObject(
    PDRIVER_OBJECT          DriverObject OPTIONAL,
    ULONG                   Tag,
    USHORT                  Size
    );

EXPORT
VOID
NdisFreeGenericObject(
    IN PNDIS_GENERIC_OBJECT   NdisObject
    );

#endif // NDIS_SUPPORT_NDIS6


//
// Memory manipulation functions.
//
#define NdisMoveMemory(Destination, Source, Length) RtlCopyMemory(Destination, Source, Length)
#define NdisZeroMemory(Destination, Length)         RtlZeroMemory(Destination, Length)
#define NdisEqualMemory(Source1, Source2, Length)   RtlEqualMemory(Source1, Source2, Length)
#define NdisFillMemory(Destination, Length, Fill)   RtlFillMemory(Destination, Length, Fill)
#define NdisRetrieveUlong(Destination, Source)      RtlRetrieveUlong(Destination, Source)
#define NdisStoreUlong(Destination, Value)          RtlStoreUlong(Destination, Value)

#define NDIS_STRING_CONST(x)    {sizeof(L##x)-2, sizeof(L##x), L##x}

//
// On a RISC machine, I/O mapped memory can't be accessed with
// the Rtl routines.
//
#if defined(_M_IX86) || defined(_M_AMD64)

#define NdisMoveMappedMemory(Destination,Source,Length) RtlCopyMemory(Destination,Source,Length)
#define NdisZeroMappedMemory(Destination,Length)        RtlZeroMemory(Destination,Length)

#elif defined(_M_IA64)

#define NdisMoveMappedMemory(Destination,Source,Length)                     \
{                                                                           \
    PUCHAR _Src = (Source);                                                 \
    PUCHAR _Dest = (Destination);                                           \
    PUCHAR _End = _Dest + (Length);                                         \
    while (_Dest < _End)                                                    \
    {                                                                       \
        *_Dest++ = *_Src++;                                                 \
    }                                                                       \
}

#define NdisZeroMappedMemory(Destination,Length)                            \
{                                                                           \
    PUCHAR _Dest = (Destination);                                           \
    PUCHAR _End = _Dest + (Length);                                         \
    while (_Dest < _End)                                                    \
    {                                                                       \
        *_Dest++ = 0;                                                       \
    }                                                                       \
}
#endif


#define NdisMoveToMappedMemory(Destination,Source,Length)                   \
                            NdisMoveMappedMemory(Destination,Source,Length)
#define NdisMoveFromMappedMemory(Destination,Source,Length)                 \
                            NdisMoveMappedMemory(Destination,Source,Length)


//
// definition of the basic spin lock structure
//

typedef struct _NDIS_SPIN_LOCK
{
    KSPIN_LOCK  SpinLock;
    KIRQL       OldIrql;
} NDIS_SPIN_LOCK, * PNDIS_SPIN_LOCK;


//
// definition of the ndis event structure
//
typedef struct _NDIS_EVENT
{
    KEVENT      Event;
} NDIS_EVENT, *PNDIS_EVENT;

#pragma warning(push)
#pragma warning(disable:4115) // named type definition in parenthesis
typedef VOID    (*NDIS_PROC)(struct _NDIS_WORK_ITEM *, PVOID);
#pragma warning(pop)


//
// Definition of an ndis work-item
//
typedef struct _NDIS_WORK_ITEM
{
    PVOID           Context;
    NDIS_PROC       Routine;
    UCHAR           WrapperReserved[8*sizeof(PVOID)];
} NDIS_WORK_ITEM, *PNDIS_WORK_ITEM;

#define NdisInterruptLatched            Latched
#define NdisInterruptLevelSensitive     LevelSensitive
typedef KINTERRUPT_MODE NDIS_INTERRUPT_MODE, *PNDIS_INTERRUPT_MODE;

//
// Configuration definitions
//

//
// Possible data types
//

typedef enum _NDIS_PARAMETER_TYPE
{
    NdisParameterInteger,
    NdisParameterHexInteger,
    NdisParameterString,
    NdisParameterMultiString,
    NdisParameterBinary
} NDIS_PARAMETER_TYPE, *PNDIS_PARAMETER_TYPE;

typedef struct
{
    USHORT          Length;
    PVOID           Buffer;
} BINARY_DATA;

//
// To store configuration information
//
typedef struct _NDIS_CONFIGURATION_PARAMETER
{
    NDIS_PARAMETER_TYPE ParameterType;
    union
    {
        ULONG           IntegerData;
        NDIS_STRING     StringData;
        BINARY_DATA     BinaryData;
    } ParameterData;
} NDIS_CONFIGURATION_PARAMETER, *PNDIS_CONFIGURATION_PARAMETER;


//
// Definitions for the "ProcessorType" keyword
//
typedef enum _NDIS_PROCESSOR_TYPE
{
    NdisProcessorX86,
    NdisProcessorMips,
    NdisProcessorAlpha,
    NdisProcessorPpc,
    NdisProcessorAmd64,
    NdisProcessorIA64
} NDIS_PROCESSOR_TYPE, *PNDIS_PROCESSOR_TYPE;

//
// Definitions for the "Environment" keyword
//
typedef enum _NDIS_ENVIRONMENT_TYPE
{
    NdisEnvironmentWindows,
    NdisEnvironmentWindowsNt
} NDIS_ENVIRONMENT_TYPE, *PNDIS_ENVIRONMENT_TYPE;


//
// Possible Hardware Architecture. Define these to
// match the HAL INTERFACE_TYPE enum.
//
typedef enum _NDIS_INTERFACE_TYPE
{
    NdisInterfaceInternal = Internal,
    NdisInterfaceIsa = Isa,
    NdisInterfaceEisa = Eisa,
    NdisInterfaceMca = MicroChannel,
    NdisInterfaceTurboChannel = TurboChannel,
    NdisInterfacePci = PCIBus,
    NdisInterfacePcMcia = PCMCIABus,
    NdisInterfaceCBus = CBus,
    NdisInterfaceMPIBus = MPIBus,
    NdisInterfaceMPSABus = MPSABus,
    NdisInterfaceProcessorInternal = ProcessorInternal,
    NdisInterfaceInternalPowerBus = InternalPowerBus,
    NdisInterfacePNPISABus = PNPISABus,
    NdisInterfacePNPBus = PNPBus,
    NdisInterfaceUSB,
    NdisInterfaceIrda,
    NdisInterface1394,
    NdisMaximumInterfaceType
} NDIS_INTERFACE_TYPE, *PNDIS_INTERFACE_TYPE;

//
// Stuff for PCI configuring
//

typedef CM_PARTIAL_RESOURCE_LIST NDIS_RESOURCE_LIST, *PNDIS_RESOURCE_LIST;


//
// The structure passed up on a WAN_LINE_UP indication
//

typedef struct _NDIS_WAN_LINE_UP
{
    IN ULONG                LinkSpeed;          // 100 bps units
    IN ULONG                MaximumTotalSize;   // suggested max for send packets
    IN NDIS_WAN_QUALITY     Quality;
    IN USHORT               SendWindow;         // suggested by the MAC
    IN UCHAR                RemoteAddress[6];
    IN OUT UCHAR            LocalAddress[6];
    IN ULONG                ProtocolBufferLength;   // Length of protocol info buffer
    IN PUCHAR               ProtocolBuffer;     // Information used by protocol
    IN USHORT               ProtocolType;       // Protocol ID
    IN OUT NDIS_STRING      DeviceName;
} NDIS_WAN_LINE_UP, *PNDIS_WAN_LINE_UP;


//
// The structure passed up on a WAN_LINE_DOWN indication
//

typedef struct _NDIS_WAN_LINE_DOWN
{
    IN UCHAR    RemoteAddress[6];
    IN UCHAR    LocalAddress[6];
} NDIS_WAN_LINE_DOWN, *PNDIS_WAN_LINE_DOWN;

//
// The structure passed up on a WAN_FRAGMENT indication
//

typedef struct _NDIS_WAN_FRAGMENT
{
    IN UCHAR    RemoteAddress[6];
    IN UCHAR    LocalAddress[6];
} NDIS_WAN_FRAGMENT, *PNDIS_WAN_FRAGMENT;

//
// The structure passed up on a WAN_GET_STATS indication
//

typedef struct _NDIS_WAN_GET_STATS
{
    IN  UCHAR   LocalAddress[6];
    OUT ULONG   BytesSent;
    OUT ULONG   BytesRcvd;
    OUT ULONG   FramesSent;
    OUT ULONG   FramesRcvd;
    OUT ULONG   CRCErrors;                      // Serial-like info only
    OUT ULONG   TimeoutErrors;                  // Serial-like info only
    OUT ULONG   AlignmentErrors;                // Serial-like info only
    OUT ULONG   SerialOverrunErrors;            // Serial-like info only
    OUT ULONG   FramingErrors;                  // Serial-like info only
    OUT ULONG   BufferOverrunErrors;            // Serial-like info only
    OUT ULONG   BytesTransmittedUncompressed;   // Compression info only
    OUT ULONG   BytesReceivedUncompressed;      // Compression info only
    OUT ULONG   BytesTransmittedCompressed;     // Compression info only
    OUT ULONG   BytesReceivedCompressed;        // Compression info only
} NDIS_WAN_GET_STATS, *PNDIS_WAN_GET_STATS;


//
// Ndis Buffer is actually an Mdl
//
typedef MDL NDIS_BUFFER, *PNDIS_BUFFER;

#if NDIS_LEGACY_DRIVER
struct _NDIS_PACKET;
typedef NDIS_HANDLE PNDIS_PACKET_POOL;

//
//
// wrapper-specific part of a packet
//
typedef struct _NDIS_PACKET_PRIVATE
{
    UINT                PhysicalCount;  // number of physical pages in packet.
    UINT                TotalLength;    // Total amount of data in the packet.
    PNDIS_BUFFER        Head;           // first buffer in the chain
    PNDIS_BUFFER        Tail;           // last buffer in the chain

    // if Head is NULL the chain is empty; Tail doesn't have to be NULL also

    PNDIS_PACKET_POOL   Pool;           // so we know where to free it back to
    UINT                Count;
    ULONG               Flags;
    BOOLEAN             ValidCounts;
    UCHAR               NdisPacketFlags;    // See fPACKET_xxx bits below
    USHORT              NdisPacketOobOffset;
} NDIS_PACKET_PRIVATE, * PNDIS_PACKET_PRIVATE;

//
// The bits define the bits in the Flags
//
#define NDIS_FLAGS_PROTOCOL_ID_MASK             0x0000000F  // The low 4 bits are defined for protocol-id
                                                            // The values are defined in ntddndis.h
#define NDIS_FLAGS_MULTICAST_PACKET             0x00000010  // don't use
#define NDIS_FLAGS_RESERVED2                    0x00000020  // don't use
#define NDIS_FLAGS_RESERVED3                    0x00000040  // don't use
#define NDIS_FLAGS_DONT_LOOPBACK                0x00000080  // Write only
#define NDIS_FLAGS_IS_LOOPBACK_PACKET           0x00000100  // Read only
#define NDIS_FLAGS_LOOPBACK_ONLY                0x00000200  // Write only
#define NDIS_FLAGS_RESERVED4                    0x00000400  // don't use
#define NDIS_FLAGS_DOUBLE_BUFFERED              0x00000800  // used by ndis
#define NDIS_FLAGS_SENT_AT_DPC                  0x00001000  // the protocol sent this packet at DPC
#define NDIS_FLAGS_USES_SG_BUFFER_LIST          0x00002000  // used by Ndis
#define NDIS_FLAGS_USES_ORIGINAL_PACKET         0x00004000  // used by Ndis
#define NDIS_FLAGS_ALLOCATED_XFER_DATA_PKT      0x00008000  // used by Ndis
#define NDIS_FLAGS_PADDED                       0x00010000  // used by NDIS

//
// Low-bits in the NdisPacketFlags are reserved by NDIS Wrapper for internal use
//
#define fPACKET_WRAPPER_RESERVED                0x3F
#define fPACKET_CONTAINS_MEDIA_SPECIFIC_INFO    0x40
#define fPACKET_ALLOCATED_BY_NDIS               0x80

#endif // NDIS_LEGACY_DRIVER

//
// Definition for layout of the media-specific data. More than one class of media-specific
// information can be tagged onto a packet.
//
typedef enum _NDIS_CLASS_ID
{
    NdisClass802_3Priority,
    NdisClassWirelessWanMbxMailbox,
    NdisClassIrdaPacketInfo,
    NdisClassAtmAALInfo

} NDIS_CLASS_ID;

typedef struct _MEDIA_SPECIFIC_INFORMATION
{
    UINT            NextEntryOffset;
    NDIS_CLASS_ID   ClassId;
    UINT            Size;
    UCHAR           ClassInformation[1];

} MEDIA_SPECIFIC_INFORMATION, *PMEDIA_SPECIFIC_INFORMATION;

#if NDIS_LEGACY_DRIVER

typedef struct _NDIS_PACKET_OOB_DATA
{
    union
    {
        ULONGLONG   TimeToSend;
        ULONGLONG   TimeSent;
    };
    ULONGLONG       TimeReceived;
    UINT            HeaderSize;
    UINT            SizeMediaSpecificInfo;
    PVOID           MediaSpecificInformation;

    NDIS_STATUS     Status;
} NDIS_PACKET_OOB_DATA, *PNDIS_PACKET_OOB_DATA;

#define NDIS_GET_PACKET_PROTOCOL_TYPE(_Packet_) ((_Packet_)->Private.Flags & NDIS_PROTOCOL_ID_MASK)

#define NDIS_OOB_DATA_FROM_PACKET(_p)                                   \
                        (PNDIS_PACKET_OOB_DATA)((PUCHAR)(_p) +          \
                        (_p)->Private.NdisPacketOobOffset)
#define NDIS_GET_PACKET_HEADER_SIZE(_Packet)                            \
                        ((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +    \
                        (_Packet)->Private.NdisPacketOobOffset))->HeaderSize
#define NDIS_GET_PACKET_STATUS(_Packet)                                 \
                        ((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +    \
                        (_Packet)->Private.NdisPacketOobOffset))->Status
#define NDIS_GET_PACKET_TIME_TO_SEND(_Packet)                           \
                        ((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +    \
                        (_Packet)->Private.NdisPacketOobOffset))->TimeToSend
#define NDIS_GET_PACKET_TIME_SENT(_Packet)                              \
                        ((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +    \
                        (_Packet)->Private.NdisPacketOobOffset))->TimeSent
#define NDIS_GET_PACKET_TIME_RECEIVED(_Packet)                          \
                        ((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +    \
                        (_Packet)->Private.NdisPacketOobOffset))->TimeReceived
#define NDIS_GET_PACKET_MEDIA_SPECIFIC_INFO(_Packet,                    \
                                            _pMediaSpecificInfo,        \
                                            _pSizeMediaSpecificInfo)    \
{                                                                       \
    if (!((_Packet)->Private.NdisPacketFlags & fPACKET_ALLOCATED_BY_NDIS) ||\
        !((_Packet)->Private.NdisPacketFlags & fPACKET_CONTAINS_MEDIA_SPECIFIC_INFO))\
    {                                                                   \
        *(_pMediaSpecificInfo) = NULL;                                  \
        *(_pSizeMediaSpecificInfo) = 0;                                 \
    }                                                                   \
    else                                                                \
    {                                                                   \
        *(_pMediaSpecificInfo) =((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +\
                    (_Packet)->Private.NdisPacketOobOffset))->MediaSpecificInformation;\
        *(_pSizeMediaSpecificInfo) = ((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +\
                    (_Packet)->Private.NdisPacketOobOffset))->SizeMediaSpecificInfo;\
    }                                                                   \
}

#define NDIS_SET_PACKET_HEADER_SIZE(_Packet, _HdrSize)                  \
                        ((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +    \
                        (_Packet)->Private.NdisPacketOobOffset))->HeaderSize = (_HdrSize)
#define NDIS_SET_PACKET_STATUS(_Packet, _Status)                        \
                        ((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +    \
                        (_Packet)->Private.NdisPacketOobOffset))->Status = (_Status)
#define NDIS_SET_PACKET_TIME_TO_SEND(_Packet, _TimeToSend)              \
                        ((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +    \
                        (_Packet)->Private.NdisPacketOobOffset))->TimeToSend = (_TimeToSend)
#define NDIS_SET_PACKET_TIME_SENT(_Packet, _TimeSent)                   \
                        ((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +    \
                        (_Packet)->Private.NdisPacketOobOffset))->TimeSent = (_TimeSent)
#define NDIS_SET_PACKET_TIME_RECEIVED(_Packet, _TimeReceived)           \
                        ((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +    \
                        (_Packet)->Private.NdisPacketOobOffset))->TimeReceived = (_TimeReceived)
#define NDIS_SET_PACKET_MEDIA_SPECIFIC_INFO(_Packet,                    \
                                            _MediaSpecificInfo,         \
                                            _SizeMediaSpecificInfo)     \
{                                                                       \
    if ((_Packet)->Private.NdisPacketFlags & fPACKET_ALLOCATED_BY_NDIS) \
    {                                                                   \
        (_Packet)->Private.NdisPacketFlags |= fPACKET_CONTAINS_MEDIA_SPECIFIC_INFO;\
        ((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +                    \
                                          (_Packet)->Private.NdisPacketOobOffset))->MediaSpecificInformation = (_MediaSpecificInfo);\
        ((PNDIS_PACKET_OOB_DATA)((PUCHAR)(_Packet) +                    \
                                          (_Packet)->Private.NdisPacketOobOffset))->SizeMediaSpecificInfo = (_SizeMediaSpecificInfo);\
    }                                                                   \
}

//
// packet definition
//
typedef struct _NDIS_PACKET
{
    NDIS_PACKET_PRIVATE Private;

    union
    {
        struct                  // For Connection-less miniports
        {
            UCHAR   MiniportReserved[2*sizeof(PVOID)];
            UCHAR   WrapperReserved[2*sizeof(PVOID)];
        };

        struct
        {
            //
            // For de-serialized miniports. And by implication conn-oriented miniports.
            //
            UCHAR   MiniportReservedEx[3*sizeof(PVOID)];
            UCHAR   WrapperReservedEx[sizeof(PVOID)];
        };

        struct
        {
            UCHAR   MacReserved[4*sizeof(PVOID)];
        };
    };

    ULONG_PTR       Reserved[2];            // For compatibility with Win95
    UCHAR           ProtocolReserved[1];

} NDIS_PACKET, *PNDIS_PACKET, **PPNDIS_PACKET;

#endif // NDIS_LEGACY_DRIVER

//
//  NDIS per-packet information.
//
typedef enum _NDIS_PER_PACKET_INFO
{
    TcpIpChecksumPacketInfo,
    IpSecPacketInfo,
    TcpLargeSendPacketInfo,
    ClassificationHandlePacketInfo,
    NdisReserved,
    ScatterGatherListPacketInfo,
    Ieee8021QInfo,
    OriginalPacketInfo,
    PacketCancelId,
    OriginalNetBufferList,
    CachedNetBufferList,
    ShortPacketPaddingInfo,
    MaxPerPacketInfo
} NDIS_PER_PACKET_INFO, *PNDIS_PER_PACKET_INFO;

#if NDIS_LEGACY_DRIVER
typedef struct _NDIS_PACKET_EXTENSION
{
    PVOID       NdisPacketInfo[MaxPerPacketInfo];
} NDIS_PACKET_EXTENSION, *PNDIS_PACKET_EXTENSION;

#define NDIS_PACKET_EXTENSION_FROM_PACKET(_P)       ((PNDIS_PACKET_EXTENSION)((PUCHAR)(_P) + (_P)->Private.NdisPacketOobOffset + sizeof(NDIS_PACKET_OOB_DATA)))
#define NDIS_PER_PACKET_INFO_FROM_PACKET(_P, _Id)   ((PNDIS_PACKET_EXTENSION)((PUCHAR)(_P) + (_P)->Private.NdisPacketOobOffset + sizeof(NDIS_PACKET_OOB_DATA)))->NdisPacketInfo[(_Id)]
#define NDIS_GET_ORIGINAL_PACKET(_P)                NDIS_PER_PACKET_INFO_FROM_PACKET(_P, OriginalPacketInfo)
#define NDIS_SET_ORIGINAL_PACKET(_P, _OP)           NDIS_PER_PACKET_INFO_FROM_PACKET(_P, OriginalPacketInfo) = _OP
#define NDIS_GET_PACKET_CANCEL_ID(_P)               NDIS_PER_PACKET_INFO_FROM_PACKET(_P, PacketCancelId)
#define NDIS_SET_PACKET_CANCEL_ID(_P, _cId)         NDIS_PER_PACKET_INFO_FROM_PACKET(_P, PacketCancelId) = _cId


//
// Ndis 5.1 entry points for setting/gettign packet's CancelId and cancelling send packets
//

/*
EXPORT
VOID
NdisSetPacketCancelId(
    IN  PNDIS_PACKET    Packet,
    IN  PVOID           CancelId
    );
*/
#define  NdisSetPacketCancelId(_Packet, _CancelId) NDIS_SET_PACKET_CANCEL_ID(_Packet, _CancelId)

/*
EXPORT
PVOID
NdisGetPacketCancelId(
    IN  PNDIS_PACKET    Packet
    );
*/
#define  NdisGetPacketCancelId(_Packet) NDIS_GET_PACKET_CANCEL_ID(_Packet)


typedef struct _NDIS_PACKET_STACK
{
    ULONG_PTR   IMReserved[2];
    ULONG_PTR   NdisReserved[4];
} NDIS_PACKET_STACK, *PNDIS_PACKET_STACK;


#endif // NDIS_LEGACY_DRIVER

//
//  Per-packet information for TcpIpChecksumPacketInfo.
//
typedef struct _NDIS_TCP_IP_CHECKSUM_PACKET_INFO
{
    union
    {
        struct
        {
            ULONG   NdisPacketChecksumV4:1;
            ULONG   NdisPacketChecksumV6:1;
            ULONG   NdisPacketTcpChecksum:1;
            ULONG   NdisPacketUdpChecksum:1;
            ULONG   NdisPacketIpChecksum:1;
        } Transmit;

        struct
        {
            ULONG   NdisPacketTcpChecksumFailed:1;
            ULONG   NdisPacketUdpChecksumFailed:1;
            ULONG   NdisPacketIpChecksumFailed:1;
            ULONG   NdisPacketTcpChecksumSucceeded:1;
            ULONG   NdisPacketUdpChecksumSucceeded:1;
            ULONG   NdisPacketIpChecksumSucceeded:1;
            ULONG   NdisPacketLoopback:1;
        } Receive;

        ULONG   Value;
    };
} NDIS_TCP_IP_CHECKSUM_PACKET_INFO, *PNDIS_TCP_IP_CHECKSUM_PACKET_INFO;


//
//  Per-packet information for Ieee8021QInfo.
//
typedef struct _NDIS_PACKET_8021Q_INFO
{
    union
    {
        struct
        {
            UINT32      UserPriority:3;         // 802.1p priority
            UINT32      CanonicalFormatId:1;    // always 0
            UINT32      VlanId:12;              // VLAN Identification
            UINT32      Reserved:16;            // set to 0
        }   TagHeader;

        PVOID  Value;
    };
} NDIS_PACKET_8021Q_INFO, *PNDIS_PACKET_8021Q_INFO;


#if NDIS_LEGACY_DRIVER
//
//  Old definitions, to be obsoleted.
//
#define Ieee8021pPriority   Ieee8021QInfo
typedef UINT                IEEE8021PPRIORITY;

//
// WAN Packet. This is used by WAN miniports only. This is the legacy model.
// Co-Ndis is the preferred model for WAN miniports
//
typedef struct _NDIS_WAN_PACKET
{
    LIST_ENTRY          WanPacketQueue;
    PUCHAR              CurrentBuffer;
    ULONG               CurrentLength;
    PUCHAR              StartBuffer;
    PUCHAR              EndBuffer;
    PVOID               ProtocolReserved1;
    PVOID               ProtocolReserved2;
    PVOID               ProtocolReserved3;
    PVOID               ProtocolReserved4;
    PVOID               MacReserved1;
    PVOID               MacReserved2;
    PVOID               MacReserved3;
    PVOID               MacReserved4;
} NDIS_WAN_PACKET, *PNDIS_WAN_PACKET;

//
// Routines to get/set packet flags
//

/*++

UINT
NdisGetPacketFlags(
    IN  PNDIS_PACKET    Packet
    );

--*/

#define NdisGetPacketFlags(_Packet)                 ((_Packet)->Private.Flags)

#define NDIS_PACKET_FIRST_NDIS_BUFFER(_Packet)      ((_Packet)->Private.Head)
#define NDIS_PACKET_LAST_NDIS_BUFFER(_Packet)       ((_Packet)->Private.Tail)
#define NDIS_PACKET_VALID_COUNTS(_Packet)           ((_Packet)->Private.ValidCounts)


/*++

VOID
NdisSetPacketFlags(
    IN  PNDIS_PACKET Packet,
    IN  UINT Flags
    );

--*/

#define NdisSetPacketFlags(_Packet, _Flags)     (_Packet)->Private.Flags |= (_Flags)
#define NdisClearPacketFlags(_Packet, _Flags)   (_Packet)->Private.Flags &= ~(_Flags)

#endif // NDIS_LEGACY_DRIVER

//
// Request types used by NdisRequest; constants are added for
// all entry points in the MAC, for those that want to create
// their own internal requests.
//

#if NDIS_SUPPORT_NDIS6
typedef enum _NDIS_REQUEST_TYPE
{
    NdisRequestQueryInformation,
    NdisRequestSetInformation,
    NdisRequestQueryStatistics,
    NdisRequestOpen,
    NdisRequestClose,
    NdisRequestSend,
    NdisRequestTransferData,
    NdisRequestReset,
    NdisRequestGeneric1,
    NdisRequestGeneric2,
    NdisRequestGeneric3,
    NdisRequestGeneric4,
    NdisRequestMethod,
} NDIS_REQUEST_TYPE, *PNDIS_REQUEST_TYPE;
#else
typedef enum _NDIS_REQUEST_TYPE
{
    NdisRequestQueryInformation,
    NdisRequestSetInformation,
    NdisRequestQueryStatistics,
    NdisRequestOpen,
    NdisRequestClose,
    NdisRequestSend,
    NdisRequestTransferData,
    NdisRequestReset,
    NdisRequestGeneric1,
    NdisRequestGeneric2,
    NdisRequestGeneric3,
    NdisRequestGeneric4,
} NDIS_REQUEST_TYPE, *PNDIS_REQUEST_TYPE;
#endif // NDIS_SUPPORT_NDIS6


#if NDIS_LEGACY_DRIVER

//
// Structure of requests sent via NdisRequest
//

typedef struct _NDIS_REQUEST
{
    UCHAR               MacReserved[4*sizeof(PVOID)];
    NDIS_REQUEST_TYPE   RequestType;
    union _DATA
    {
        struct _QUERY_INFORMATION
        {
            NDIS_OID    Oid;
            PVOID       InformationBuffer;
            UINT        InformationBufferLength;
            UINT        BytesWritten;
            UINT        BytesNeeded;
        } QUERY_INFORMATION;

        struct _SET_INFORMATION
        {
            NDIS_OID    Oid;
            PVOID       InformationBuffer;
            UINT        InformationBufferLength;
            UINT        BytesRead;
            UINT        BytesNeeded;
        } SET_INFORMATION;

    } DATA;
#if (defined(NDIS50) || defined(NDIS51) || defined(NDIS50_MINIPORT) || defined(NDIS51_MINIPORT))
    UCHAR               NdisReserved[9*sizeof(PVOID)];
    union
    {
        UCHAR           CallMgrReserved[2*sizeof(PVOID)];
        UCHAR           ProtocolReserved[2*sizeof(PVOID)];
    };
    UCHAR               MiniportReserved[2*sizeof(PVOID)];
#endif
} NDIS_REQUEST, *PNDIS_REQUEST;

#endif

//
// NDIS Address Family definitions.
//
typedef ULONG           NDIS_AF, *PNDIS_AF;
#define CO_ADDRESS_FAMILY_Q2931             ((NDIS_AF)0x1)  // ATM
#define CO_ADDRESS_FAMILY_PSCHED            ((NDIS_AF)0x2)  // Packet scheduler
#define CO_ADDRESS_FAMILY_L2TP              ((NDIS_AF)0x3)
#define CO_ADDRESS_FAMILY_IRDA              ((NDIS_AF)0x4)
#define CO_ADDRESS_FAMILY_1394              ((NDIS_AF)0x5)
#define CO_ADDRESS_FAMILY_PPP               ((NDIS_AF)0x6)
#define CO_ADDRESS_FAMILY_INFINIBAND        ((NDIS_AF)0x7)
#define CO_ADDRESS_FAMILY_TAPI              ((NDIS_AF)0x800)
#define CO_ADDRESS_FAMILY_TAPI_PROXY        ((NDIS_AF)0x801)

//
// The following is OR'ed with the base AF to denote proxy support
//
#define CO_ADDRESS_FAMILY_PROXY             0x80000000


//
//  Address family structure registered/opened via
//      NdisCmRegisterAddressFamily
//      NdisClOpenAddressFamily
//
typedef struct
{
    NDIS_AF                     AddressFamily;  // one of the CO_ADDRESS_FAMILY_xxx values above
    ULONG                       MajorVersion;   // the major version of call manager
    ULONG                       MinorVersion;   // the minor version of call manager
} CO_ADDRESS_FAMILY, *PCO_ADDRESS_FAMILY;

//
// Definition for a SAP
//
typedef struct
{
    ULONG                       SapType;
    ULONG                       SapLength;
    UCHAR                       Sap[1];
} CO_SAP, *PCO_SAP;

//
// Definitions for physical address.
//

typedef PHYSICAL_ADDRESS NDIS_PHYSICAL_ADDRESS, *PNDIS_PHYSICAL_ADDRESS;
typedef struct _NDIS_PHYSICAL_ADDRESS_UNIT
{
    NDIS_PHYSICAL_ADDRESS       PhysicalAddress;
    UINT                        Length;
} NDIS_PHYSICAL_ADDRESS_UNIT, *PNDIS_PHYSICAL_ADDRESS_UNIT;


/*++

ULONG
NdisGetPhysicalAddressHigh(
    IN  NDIS_PHYSICAL_ADDRESS   PhysicalAddress
    );

--*/

#define NdisGetPhysicalAddressHigh(_PhysicalAddress)            \
        ((_PhysicalAddress).HighPart)

/*++

VOID
NdisSetPhysicalAddressHigh(
    IN  NDIS_PHYSICAL_ADDRESS   PhysicalAddress,
    IN  ULONG                   Value
    );

--*/

#define NdisSetPhysicalAddressHigh(_PhysicalAddress, _Value)    \
     ((_PhysicalAddress).HighPart) = (_Value)


/*++

ULONG
NdisGetPhysicalAddressLow(
    IN  NDIS_PHYSICAL_ADDRESS PhysicalAddress
    );

--*/

#define NdisGetPhysicalAddressLow(_PhysicalAddress)             \
    ((_PhysicalAddress).LowPart)


/*++

VOID
NdisSetPhysicalAddressLow(
    IN  NDIS_PHYSICAL_ADDRESS   PhysicalAddress,
    IN  ULONG                   Value
    );

--*/

#define NdisSetPhysicalAddressLow(_PhysicalAddress, _Value)     \
    ((_PhysicalAddress).LowPart) = (_Value)

//
// Macro to initialize an NDIS_PHYSICAL_ADDRESS constant
//

#define NDIS_PHYSICAL_ADDRESS_CONST(_Low, _High)                \
    { (ULONG)(_Low), (LONG)(_High) }

//
// block used for references...
//
typedef struct _REFERENCE
{
    KSPIN_LOCK                  SpinLock;
    USHORT                      ReferenceCount;
    BOOLEAN                     Closing;
} REFERENCE, * PREFERENCE;

//
// Types of Memory (not mutually exclusive)
//

#define NDIS_MEMORY_CONTIGUOUS      0x00000001
#define NDIS_MEMORY_NONCACHED       0x00000002

//
// Open options
//

//
// This flag has been deprecated
//
#pragma deprecated(NDIS_OPEN_RECEIVE_NOT_REENTRANT)
#define NDIS_OPEN_RECEIVE_NOT_REENTRANT         0x00000001

//
// NDIS_STATUS values used in status indication
//

#define NDIS_STATUS_ONLINE                      ((NDIS_STATUS)0x40010003L)
#define NDIS_STATUS_RESET_START                 ((NDIS_STATUS)0x40010004L)
#define NDIS_STATUS_RESET_END                   ((NDIS_STATUS)0x40010005L)
#define NDIS_STATUS_RING_STATUS                 ((NDIS_STATUS)0x40010006L)
#define NDIS_STATUS_CLOSED                      ((NDIS_STATUS)0x40010007L)
#define NDIS_STATUS_WAN_LINE_UP                 ((NDIS_STATUS)0x40010008L)
#define NDIS_STATUS_WAN_LINE_DOWN               ((NDIS_STATUS)0x40010009L)
#define NDIS_STATUS_WAN_FRAGMENT                ((NDIS_STATUS)0x4001000AL)
#define NDIS_STATUS_MEDIA_CONNECT               ((NDIS_STATUS)0x4001000BL)
#define NDIS_STATUS_MEDIA_DISCONNECT            ((NDIS_STATUS)0x4001000CL)
#define NDIS_STATUS_HARDWARE_LINE_UP            ((NDIS_STATUS)0x4001000DL)
#define NDIS_STATUS_HARDWARE_LINE_DOWN          ((NDIS_STATUS)0x4001000EL)
#define NDIS_STATUS_INTERFACE_UP                ((NDIS_STATUS)0x4001000FL)
#define NDIS_STATUS_INTERFACE_DOWN              ((NDIS_STATUS)0x40010010L)
#define NDIS_STATUS_MEDIA_BUSY                  ((NDIS_STATUS)0x40010011L)
#define NDIS_STATUS_MEDIA_SPECIFIC_INDICATION   ((NDIS_STATUS)0x40010012L)
#define NDIS_STATUS_WW_INDICATION               NDIS_STATUS_MEDIA_SPECIFIC_INDICATION
#define NDIS_STATUS_LINK_SPEED_CHANGE           ((NDIS_STATUS)0x40010013L)
#define NDIS_STATUS_WAN_GET_STATS               ((NDIS_STATUS)0x40010014L)
#define NDIS_STATUS_WAN_CO_FRAGMENT             ((NDIS_STATUS)0x40010015L)
#define NDIS_STATUS_WAN_CO_LINKPARAMS           ((NDIS_STATUS)0x40010016L)

//
// new status indication codes used by NDIS 6 drivers
//
#if NDIS_SUPPORT_NDIS6
#define NDIS_STATUS_LINK_STATE                  ((NDIS_STATUS)0x40010017L)
#define NDIS_STATUS_NETWORK_CHANGE              ((NDIS_STATUS)0x40010018L)
#define NDIS_STATUS_MEDIA_SPECIFIC_INDICATION_EX ((NDIS_STATUS)0x40010019L)
#define NDIS_STATUS_PORT_STATE                  ((NDIS_STATUS)0x40010022L)
#define NDIS_STATUS_OPER_STATUS                 ((NDIS_STATUS)0x40010023L)
#define NDIS_STATUS_PACKET_FILTER               ((NDIS_STATUS)0x40010024L)

//
// offload specific status indication codes
//
#define NDIS_STATUS_OFFLOAD_PAUSE               ((NDIS_STATUS)0x40020001L)
#define NDIS_STATUS_UPLOAD_ALL                  ((NDIS_STATUS)0x40020002L)
#define NDIS_STATUS_OFFLOAD_RESUME              ((NDIS_STATUS)0x40020003L)
#define NDIS_STATUS_OFFLOAD_PARTIAL_SUCCESS     ((NDIS_STATUS)0x40020004L)
#define NDIS_STATUS_OFFLOAD_STATE_INVALID       ((NDIS_STATUS)0x40020005L)
#define NDIS_STATUS_TASK_OFFLOAD_CURRENT_CONFIG ((NDIS_STATUS)0x40020006L)
#define NDIS_STATUS_TASK_OFFLOAD_HARDWARE_CAPABILITIES ((NDIS_STATUS)0x40020007L)
#define NDIS_STATUS_OFFLOAD_ENCASPULATION_CHANGE ((NDIS_STATUS)0x40020008L)
#define NDIS_STATUS_TCP_CONNECTION_OFFLOAD_HARDWARE_CAPABILITIES ((NDIS_STATUS)0x4002000BL)

#define NDIS_STATUS_OFFLOAD_IM_RESERVED1        ((NDIS_STATUS)0x40020100L)
#define NDIS_STATUS_OFFLOAD_IM_RESERVED2        ((NDIS_STATUS)0x40020101L)
#define NDIS_STATUS_OFFLOAD_IM_RESERVED3        ((NDIS_STATUS)0x40020102L)

//
// 802.11 specific status indication codes
//
#define NDIS_STATUS_DOT11_SCAN_CONFIRM                  ((NDIS_STATUS)0x40030000L)
#define NDIS_STATUS_DOT11_MPDU_MAX_LENGTH_CHANGED       ((NDIS_STATUS)0x40030001L)
#define NDIS_STATUS_DOT11_ASSOCIATION_START             ((NDIS_STATUS)0x40030002L)
#define NDIS_STATUS_DOT11_ASSOCIATION_COMPLETION        ((NDIS_STATUS)0x40030003L)
#define NDIS_STATUS_DOT11_CONNECTION_START              ((NDIS_STATUS)0x40030004L)
#define NDIS_STATUS_DOT11_CONNECTION_COMPLETION         ((NDIS_STATUS)0x40030005L)
#define NDIS_STATUS_DOT11_ROAMING_START                 ((NDIS_STATUS)0x40030006L)
#define NDIS_STATUS_DOT11_ROAMING_COMPLETION            ((NDIS_STATUS)0x40030007L)
#define NDIS_STATUS_DOT11_DISASSOCIATION                ((NDIS_STATUS)0x40030008L)
#define NDIS_STATUS_DOT11_TKIPMIC_FAILURE               ((NDIS_STATUS)0x40030009L)
#define NDIS_STATUS_DOT11_PMKID_CANDIDATE_LIST          ((NDIS_STATUS)0x4003000AL)
#define NDIS_STATUS_DOT11_PHY_STATE_CHANGED             ((NDIS_STATUS)0x4003000BL)
#define NDIS_STATUS_DOT11_LINK_QUALITY                  ((NDIS_STATUS)0x4003000CL)
#endif NDIS_SUPPORT_NDIS6

//
// status codes returned by drivers
//
#define NDIS_STATUS_SUCCESS                     ((NDIS_STATUS)STATUS_SUCCESS)
#define NDIS_STATUS_PENDING                     ((NDIS_STATUS)STATUS_PENDING)
#define NDIS_STATUS_NOT_RECOGNIZED              ((NDIS_STATUS)0x00010001L)
#define NDIS_STATUS_NOT_COPIED                  ((NDIS_STATUS)0x00010002L)
#define NDIS_STATUS_NOT_ACCEPTED                ((NDIS_STATUS)0x00010003L)
#define NDIS_STATUS_CALL_ACTIVE                 ((NDIS_STATUS)0x00010007L)
#define NDIS_STATUS_INDICATION_REQUIRED         ((NDIS_STATUS)STATUS_NDIS_INDICATION_REQUIRED)
#define NDIS_STATUS_NOT_RESETTABLE              ((NDIS_STATUS)0x80010001L)
#define NDIS_STATUS_SOFT_ERRORS                 ((NDIS_STATUS)0x80010003L)
#define NDIS_STATUS_HARD_ERRORS                 ((NDIS_STATUS)0x80010004L)
#define NDIS_STATUS_BUFFER_OVERFLOW             ((NDIS_STATUS)STATUS_BUFFER_OVERFLOW)
#define NDIS_STATUS_FAILURE                     ((NDIS_STATUS)STATUS_UNSUCCESSFUL)
#define NDIS_STATUS_RESOURCES                   ((NDIS_STATUS)STATUS_INSUFFICIENT_RESOURCES)
#define NDIS_STATUS_CLOSING                     ((NDIS_STATUS)0xC0010002L)
#define NDIS_STATUS_BAD_VERSION                 ((NDIS_STATUS)0xC0010004L)
#define NDIS_STATUS_BAD_CHARACTERISTICS         ((NDIS_STATUS)0xC0010005L)
#define NDIS_STATUS_ADAPTER_NOT_FOUND           ((NDIS_STATUS)0xC0010006L)
#define NDIS_STATUS_OPEN_FAILED                 ((NDIS_STATUS)0xC0010007L)
#define NDIS_STATUS_DEVICE_FAILED               ((NDIS_STATUS)0xC0010008L)
#define NDIS_STATUS_MULTICAST_FULL              ((NDIS_STATUS)0xC0010009L)
#define NDIS_STATUS_MULTICAST_EXISTS            ((NDIS_STATUS)0xC001000AL)
#define NDIS_STATUS_MULTICAST_NOT_FOUND         ((NDIS_STATUS)0xC001000BL)
#define NDIS_STATUS_REQUEST_ABORTED             ((NDIS_STATUS)0xC001000CL)
#define NDIS_STATUS_RESET_IN_PROGRESS           ((NDIS_STATUS)0xC001000DL)
#define NDIS_STATUS_CLOSING_INDICATING          ((NDIS_STATUS)0xC001000EL)
#define NDIS_STATUS_NOT_SUPPORTED               ((NDIS_STATUS)STATUS_NOT_SUPPORTED)
#define NDIS_STATUS_INVALID_PACKET              ((NDIS_STATUS)0xC001000FL)
#define NDIS_STATUS_OPEN_LIST_FULL              ((NDIS_STATUS)0xC0010010L)
#define NDIS_STATUS_ADAPTER_NOT_READY           ((NDIS_STATUS)0xC0010011L)
#define NDIS_STATUS_ADAPTER_NOT_OPEN            ((NDIS_STATUS)0xC0010012L)
#define NDIS_STATUS_NOT_INDICATING              ((NDIS_STATUS)0xC0010013L)
#define NDIS_STATUS_INVALID_LENGTH              ((NDIS_STATUS)0xC0010014L)
#define NDIS_STATUS_INVALID_DATA                ((NDIS_STATUS)0xC0010015L)
#define NDIS_STATUS_BUFFER_TOO_SHORT            ((NDIS_STATUS)0xC0010016L)
#define NDIS_STATUS_INVALID_OID                 ((NDIS_STATUS)0xC0010017L)
#define NDIS_STATUS_ADAPTER_REMOVED             ((NDIS_STATUS)0xC0010018L)
#define NDIS_STATUS_UNSUPPORTED_MEDIA           ((NDIS_STATUS)0xC0010019L)
#define NDIS_STATUS_GROUP_ADDRESS_IN_USE        ((NDIS_STATUS)0xC001001AL)
#define NDIS_STATUS_FILE_NOT_FOUND              ((NDIS_STATUS)0xC001001BL)
#define NDIS_STATUS_ERROR_READING_FILE          ((NDIS_STATUS)0xC001001CL)
#define NDIS_STATUS_ALREADY_MAPPED              ((NDIS_STATUS)0xC001001DL)
#define NDIS_STATUS_RESOURCE_CONFLICT           ((NDIS_STATUS)0xC001001EL)
#define NDIS_STATUS_NO_CABLE                    ((NDIS_STATUS)0xC001001FL)
#define NDIS_STATUS_INVALID_DEVICE_REQUEST      ((NDIS_STATUS)STATUS_INVALID_DEVICE_REQUEST)
#define NDIS_STATUS_NETWORK_UNREACHABLE         ((NDIS_STATUS)STATUS_NETWORK_UNREACHABLE)

//
// CO-NDIS specific
//
#define NDIS_STATUS_INVALID_SAP                 ((NDIS_STATUS)0xC0010020L)
#define NDIS_STATUS_SAP_IN_USE                  ((NDIS_STATUS)0xC0010021L)
#define NDIS_STATUS_INVALID_ADDRESS             ((NDIS_STATUS)0xC0010022L)
#define NDIS_STATUS_VC_NOT_ACTIVATED            ((NDIS_STATUS)0xC0010023L)
#define NDIS_STATUS_DEST_OUT_OF_ORDER           ((NDIS_STATUS)0xC0010024L)  // cause 27
#define NDIS_STATUS_VC_NOT_AVAILABLE            ((NDIS_STATUS)0xC0010025L)  // cause 35,45
#define NDIS_STATUS_CELLRATE_NOT_AVAILABLE      ((NDIS_STATUS)0xC0010026L)  // cause 37
#define NDIS_STATUS_INCOMPATABLE_QOS            ((NDIS_STATUS)0xC0010027L)  // cause 49
#define NDIS_STATUS_AAL_PARAMS_UNSUPPORTED      ((NDIS_STATUS)0xC0010028L)  // cause 93
#define NDIS_STATUS_NO_ROUTE_TO_DESTINATION     ((NDIS_STATUS)0xC0010029L)  // cause 3

//
// 802.5 specific
//
#define NDIS_STATUS_TOKEN_RING_OPEN_ERROR       ((NDIS_STATUS)0xC0011000L)


//
// new status codes used in NDIS 6
//
#if NDIS_SUPPORT_NDIS6
#define NDIS_STATUS_SEND_ABORTED                ((NDIS_STATUS)STATUS_NDIS_REQUEST_ABORTED)
#define NDIS_STATUS_PAUSED                      ((NDIS_STATUS)STATUS_NDIS_PAUSED)
#define NDIS_STATUS_INTERFACE_NOT_FOUND         ((NDIS_STATUS)STATUS_NDIS_INTERFACE_NOT_FOUND)
#define NDIS_STATUS_INVALID_PARAMETER           ((NDIS_STATUS)STATUS_INVALID_PARAMETER)
#define NDIS_STATUS_UNSUPPORTED_REVISION        ((NDIS_STATUS)STATUS_NDIS_UNSUPPORTED_REVISION)
#define NDIS_STATUS_INVALID_PORT                ((NDIS_STATUS)STATUS_NDIS_INVALID_PORT)
#define NDIS_STATUS_INVALID_PORT_STATE          ((NDIS_STATUS)STATUS_NDIS_INVALID_PORT_STATE)
#define NDIS_STATUS_INVALID_STATE               ((NDIS_STATUS)STATUS_INVALID_DEVICE_STATE)
#define NDIS_STATUS_MEDIA_DISCONNECTED          ((NDIS_STATUS)STATUS_NDIS_MEDIA_DISCONNECTED)
#define NDIS_STATUS_LOW_POWER_STATE             ((NDIS_STATUS)STATUS_NDIS_LOW_POWER_STATE)

#define NDIS_STATUS_DOT11_AUTO_CONFIG_ENABLED   ((NDIS_STATUS)STATUS_NDIS_DOT11_AUTO_CONFIG_ENABLED)
#define NDIS_STATUS_DOT11_MEDIA_IN_USE          ((NDIS_STATUS)STATUS_NDIS_DOT11_MEDIA_IN_USE)
#define NDIS_STATUS_DOT11_POWER_STATE_INVALID   ((NDIS_STATUS)STATUS_NDIS_DOT11_POWER_STATE_INVALID)


//
// status codes for offload operations
//
#define NDIS_STATUS_UPLOAD_IN_PROGRESS                  ((NDIS_STATUS)0xC0231001L)
#define NDIS_STATUS_REQUEST_UPLOAD                      ((NDIS_STATUS)0xC0231002L)
#define NDIS_STATUS_UPLOAD_REQUESTED                    ((NDIS_STATUS)0xC0231003L)
#define NDIS_STATUS_OFFLOAD_TCP_ENTRIES                 ((NDIS_STATUS)0xC0231004L)
#define NDIS_STATUS_OFFLOAD_PATH_ENTRIES                ((NDIS_STATUS)0xC0231005L)
#define NDIS_STATUS_OFFLOAD_NEIGHBOR_ENTRIES            ((NDIS_STATUS)0xC0231006L)
#define NDIS_STATUS_OFFLOAD_IP_ADDRESS_ENTRIES          ((NDIS_STATUS)0xC0231007L)
#define NDIS_STATUS_OFFLOAD_HW_ADDRESS_ENTRIES          ((NDIS_STATUS)0xC0231008L)
#define NDIS_STATUS_OFFLOAD_VLAN_ENTRIES                ((NDIS_STATUS)0xC0231009L)
#define NDIS_STATUS_OFFLOAD_TCP_XMIT_BUFFER             ((NDIS_STATUS)0xC023100AL)
#define NDIS_STATUS_OFFLOAD_TCP_RCV_BUFFER              ((NDIS_STATUS)0xC023100BL)
#define NDIS_STATUS_OFFLOAD_TCP_RCV_WINDOW              ((NDIS_STATUS)0xC023100CL)
#define NDIS_STATUS_OFFLOAD_VLAN_MISMATCH               ((NDIS_STATUS)0xC023100DL)
#define NDIS_STATUS_OFFLOAD_DATA_NOT_ACCEPTED           ((NDIS_STATUS)0xC023100EL)
#define NDIS_STATUS_OFFLOAD_POLICY                      ((NDIS_STATUS)0xC023100FL)
#define NDIS_STATUS_OFFLOAD_DATA_PARTIALLY_ACCEPTED     ((NDIS_STATUS)0xC0231010L)
#define NDIS_STATUS_OFFLOAD_REQUEST_RESET               ((NDIS_STATUS)0xC0231011L)
#endif // NDIS_SUPPORT_NDIS6


//
// used in error logging
//

#define NDIS_ERROR_CODE ULONG

#define NDIS_ERROR_CODE_RESOURCE_CONFLICT           EVENT_NDIS_RESOURCE_CONFLICT
#define NDIS_ERROR_CODE_OUT_OF_RESOURCES            EVENT_NDIS_OUT_OF_RESOURCE
#define NDIS_ERROR_CODE_HARDWARE_FAILURE            EVENT_NDIS_HARDWARE_FAILURE
#define NDIS_ERROR_CODE_ADAPTER_NOT_FOUND           EVENT_NDIS_ADAPTER_NOT_FOUND
#define NDIS_ERROR_CODE_INTERRUPT_CONNECT           EVENT_NDIS_INTERRUPT_CONNECT
#define NDIS_ERROR_CODE_DRIVER_FAILURE              EVENT_NDIS_DRIVER_FAILURE
#define NDIS_ERROR_CODE_BAD_VERSION                 EVENT_NDIS_BAD_VERSION
#define NDIS_ERROR_CODE_TIMEOUT                     EVENT_NDIS_TIMEOUT
#define NDIS_ERROR_CODE_NETWORK_ADDRESS             EVENT_NDIS_NETWORK_ADDRESS
#define NDIS_ERROR_CODE_UNSUPPORTED_CONFIGURATION   EVENT_NDIS_UNSUPPORTED_CONFIGURATION
#define NDIS_ERROR_CODE_INVALID_VALUE_FROM_ADAPTER  EVENT_NDIS_INVALID_VALUE_FROM_ADAPTER
#define NDIS_ERROR_CODE_MISSING_CONFIGURATION_PARAMETER EVENT_NDIS_MISSING_CONFIGURATION_PARAMETER
#define NDIS_ERROR_CODE_BAD_IO_BASE_ADDRESS         EVENT_NDIS_BAD_IO_BASE_ADDRESS
#define NDIS_ERROR_CODE_RECEIVE_SPACE_SMALL         EVENT_NDIS_RECEIVE_SPACE_SMALL
#define NDIS_ERROR_CODE_ADAPTER_DISABLED            EVENT_NDIS_ADAPTER_DISABLED


#define NdisAllocateSpinLock(_SpinLock) KeInitializeSpinLock(&(_SpinLock)->SpinLock)

#define NdisFreeSpinLock(_SpinLock)

#define NdisAcquireSpinLock(_SpinLock)  KeAcquireSpinLock(&(_SpinLock)->SpinLock, &(_SpinLock)->OldIrql)

#define NdisReleaseSpinLock(_SpinLock)  KeReleaseSpinLock(&(_SpinLock)->SpinLock,(_SpinLock)->OldIrql)

#define NdisDprAcquireSpinLock(_SpinLock)                                   \
{                                                                           \
    KeAcquireSpinLockAtDpcLevel(&(_SpinLock)->SpinLock);                    \
}

#define NdisDprReleaseSpinLock(_SpinLock) KeReleaseSpinLockFromDpcLevel(&(_SpinLock)->SpinLock)

#define NdisGetCurrentSystemTime(_pSystemTime)                              \
    {                                                                       \
        KeQuerySystemTime(_pSystemTime);                                    \
    }

//
// Interlocked support functions
//

#define NdisInterlockedIncrement(Addend)    InterlockedIncrement(Addend)

#define NdisInterlockedDecrement(Addend)    InterlockedDecrement(Addend)

#define NdisInterlockedAddUlong(_Addend, _Increment, _SpinLock) \
    ExInterlockedAddUlong(_Addend, _Increment, &(_SpinLock)->SpinLock)

#define NdisInterlockedInsertHeadList(_ListHead, _ListEntry, _SpinLock) \
    ExInterlockedInsertHeadList(_ListHead, _ListEntry, &(_SpinLock)->SpinLock)

#define NdisInterlockedInsertTailList(_ListHead, _ListEntry, _SpinLock) \
    ExInterlockedInsertTailList(_ListHead, _ListEntry, &(_SpinLock)->SpinLock)

#define NdisInterlockedRemoveHeadList(_ListHead, _SpinLock) \
    ExInterlockedRemoveHeadList(_ListHead, &(_SpinLock)->SpinLock)

#define NdisInterlockedPushEntryList(ListHead, ListEntry, Lock) \
    ExInterlockedPushEntryList(ListHead, ListEntry, &(Lock)->SpinLock)

#define NdisInterlockedPopEntryList(ListHead, Lock) \
    ExInterlockedPopEntryList(ListHead, &(Lock)->SpinLock)


#ifndef MAXIMUM_PROCESSORS
#ifdef _WIN64
#define MAXIMUM_PROCESSORS 64
#else
#define MAXIMUM_PROCESSORS 32
#endif
#endif

typedef union _NDIS_RW_LOCK_REFCOUNT
{
    UINT                        RefCount;
    UCHAR                       cacheLine[16];  // One refCount per cache line
} NDIS_RW_LOCK_REFCOUNT;

typedef struct _NDIS_RW_LOCK
{
    union
    {
        struct
        {
            KSPIN_LOCK          SpinLock;
            PVOID               Context;
        };
        UCHAR                   Reserved[16];
    };

    NDIS_RW_LOCK_REFCOUNT       RefCount[MAXIMUM_PROCESSORS];
} NDIS_RW_LOCK, *PNDIS_RW_LOCK;

typedef struct _LOCK_STATE
{
    USHORT                      LockState;
    KIRQL                       OldIrql;
} LOCK_STATE, *PLOCK_STATE;


EXPORT
VOID
NdisInitializeReadWriteLock(
    IN  PNDIS_RW_LOCK           Lock
    );


EXPORT
VOID
NdisAcquireReadWriteLock(
    IN  PNDIS_RW_LOCK           Lock,
    IN  BOOLEAN                 fWrite,         // TRUE -> Write, FALSE -> Read
    IN  PLOCK_STATE             LockState
    );


EXPORT
VOID
NdisReleaseReadWriteLock(
    IN  PNDIS_RW_LOCK           Lock,
    IN  PLOCK_STATE             LockState
    );

EXPORT
VOID
NdisDprAcquireReadWriteLock(
    IN  PNDIS_RW_LOCK           Lock,
    IN  BOOLEAN                 fWrite,         // TRUE -> Write, FALSE -> Read
    IN  PLOCK_STATE             LockState
    );

EXPORT
VOID
NdisDprReleaseReadWriteLock(
    IN  PNDIS_RW_LOCK           Lock,
    IN  PLOCK_STATE             LockState
    );

#define NdisInterlockedAddLargeStatistic(_Addend, _Increment)   \
    ExInterlockedAddLargeStatistic((PLARGE_INTEGER)_Addend, _Increment)

//
// S-List support
//

#define NdisInterlockedPushEntrySList(SListHead, SListEntry, Lock) \
    ExInterlockedPushEntrySList(SListHead, SListEntry, &(Lock)->SpinLock)

#define NdisInterlockedPopEntrySList(SListHead, Lock) \
    ExInterlockedPopEntrySList(SListHead, &(Lock)->SpinLock)

#define NdisInterlockedFlushSList(SListHead) ExInterlockedFlushSList(SListHead)

#define NdisInitializeSListHead(SListHead)  ExInitializeSListHead(SListHead)

#define NdisQueryDepthSList(SListHead)  ExQueryDepthSList(SListHead)

EXPORT
VOID
NdisGetCurrentProcessorCpuUsage(
    OUT PULONG                  pCpuUsage
    );

EXPORT
VOID
NdisGetCurrentProcessorCounts(
    OUT PULONG                  pIdleCount,
    OUT PULONG                  pKernelAndUser,
    OUT PULONG                  pIndex
    );


#if NDIS_LEGACY_DRIVER

/*
NdisGetSystemUpTime is deprecated, use NdisGetSystemUpTimeEx instead.
*/
DECLSPEC_DEPRECATED_DDK
EXPORT
VOID
NdisGetSystemUpTime(
    OUT PULONG                  pSystemUpTime
    );

#endif // NDIS_LEGACY_DRIVER

//
// List manipulation
//

/*++

VOID
NdisInitializeListHead(
    IN  PLIST_ENTRY ListHead
    );

--*/
#define NdisInitializeListHead(_ListHead) InitializeListHead(_ListHead)

//
// Configuration Requests
//

#if NDIS_LEGACY_DRIVER

EXPORT
VOID
NdisOpenConfiguration(
    OUT PNDIS_STATUS            Status,
    OUT PNDIS_HANDLE            ConfigurationHandle,
    IN  NDIS_HANDLE             WrapperConfigurationContext
    );

#endif // NDIS_LEGACY_DRIVER

EXPORT
VOID
NdisOpenConfigurationKeyByName(
    OUT PNDIS_STATUS            Status,
    IN  NDIS_HANDLE             ConfigurationHandle,
    IN  PNDIS_STRING            SubKeyName,
    OUT PNDIS_HANDLE            SubKeyHandle
    );

EXPORT
VOID
NdisOpenConfigurationKeyByIndex(
    OUT PNDIS_STATUS            Status,
    IN  NDIS_HANDLE             ConfigurationHandle,
    IN  ULONG                   Index,
    OUT PNDIS_STRING            KeyName,
    OUT PNDIS_HANDLE            KeyHandle
    );

EXPORT
VOID
NdisReadConfiguration(
    OUT PNDIS_STATUS            Status,
    OUT PNDIS_CONFIGURATION_PARAMETER *ParameterValue,
    IN  NDIS_HANDLE             ConfigurationHandle,
    IN  PNDIS_STRING            Keyword,
    IN  NDIS_PARAMETER_TYPE     ParameterType
    );

EXPORT
VOID
NdisWriteConfiguration(
    OUT PNDIS_STATUS            Status,
    IN  NDIS_HANDLE             ConfigurationHandle,
    IN  PNDIS_STRING            Keyword,
    IN  PNDIS_CONFIGURATION_PARAMETER ParameterValue
    );

EXPORT
VOID
NdisCloseConfiguration(
    IN  NDIS_HANDLE             ConfigurationHandle
    );

EXPORT
VOID
NdisReadNetworkAddress(
    OUT PNDIS_STATUS            Status,
    OUT PVOID *                 NetworkAddress,
    OUT PUINT                   NetworkAddressLength,
    IN  NDIS_HANDLE             ConfigurationHandle
    );


#if NDIS_LEGACY_MINIPORT

EXPORT
ULONG
NdisReadPciSlotInformation(
    IN  NDIS_HANDLE             NdisAdapterHandle,
    IN  ULONG                   SlotNumber,
    IN  ULONG                   Offset,
    IN  PVOID                   Buffer,
    IN  ULONG                   Length
    );

EXPORT
ULONG
NdisWritePciSlotInformation(
    IN  NDIS_HANDLE             NdisAdapterHandle,
    IN  ULONG                   SlotNumber,
    IN  ULONG                   Offset,
    IN  PVOID                   Buffer,
    IN  ULONG                   Length
    );

EXPORT
ULONG
NdisReadPcmciaAttributeMemory(
    IN NDIS_HANDLE              NdisAdapterHandle,
    IN ULONG                    Offset,
    IN PVOID                    Buffer,
    IN ULONG                    Length
    );

EXPORT
ULONG
NdisWritePcmciaAttributeMemory(
    IN NDIS_HANDLE              NdisAdapterHandle,
    IN ULONG                    Offset,
    IN PVOID                    Buffer,
    IN ULONG                    Length
    );

#endif // NDIS_LEGACY_MINIPORT


#if NDIS_LEGACY_DRIVER
//
// Buffer Pool
//

EXPORT
VOID
NdisAllocateBufferPool(
    OUT PNDIS_STATUS            Status,
    OUT PNDIS_HANDLE            PoolHandle,
    IN  UINT                    NumberOfDescriptors
    );

EXPORT
VOID
NdisFreeBufferPool(
    IN  NDIS_HANDLE             PoolHandle
    );


EXPORT
VOID
NdisAllocateBuffer(
    OUT PNDIS_STATUS            Status,
    OUT PNDIS_BUFFER *          Buffer,
    IN  NDIS_HANDLE             PoolHandle,
    IN  PVOID                   VirtualAddress,
    IN  UINT                    Length
    );

#define NdisFreeBuffer(Buffer)  IoFreeMdl(Buffer)
#endif


EXPORT
VOID
NdisCopyBuffer(
    OUT PNDIS_STATUS            Status,
    OUT PNDIS_BUFFER *          Buffer,
    IN  NDIS_HANDLE             PoolHandle,
    IN  PVOID                   MemoryDescriptor,
    IN  UINT                    Offset,
    IN  UINT                    Length
    );


//
//  VOID
//  NdisCopyLookaheadData(
//      IN  PVOID               Destination,
//      IN  PVOID               Source,
//      IN  ULONG               Length,
//      IN  ULONG               ReceiveFlags
//      );
//

#if defined(_M_IX86) || defined(_M_AMD64)
#define NdisCopyLookaheadData(_Destination, _Source, _Length, _MacOptions)  \
        RtlCopyMemory(_Destination, _Source, _Length)
#else
#define NdisCopyLookaheadData(_Destination, _Source, _Length, _MacOptions)  \
    {                                                                       \
        if ((_MacOptions) & NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA)            \
        {                                                                   \
            RtlCopyMemory(_Destination, _Source, _Length);                  \
        }                                                                   \
        else                                                                \
        {                                                                   \
            PUCHAR _Src = (PUCHAR)(_Source);                                \
            PUCHAR _Dest = (PUCHAR)(_Destination);                          \
            PUCHAR _End = _Dest + (_Length);                                \
            while (_Dest < _End)                                            \
            {                                                               \
                *_Dest++ = *_Src++;                                         \
            }                                                               \
        }                                                                   \
    }
#endif

#if NDIS_LEGACY_DRIVER
//
// Packet Pool
//
EXPORT
VOID
NdisAllocatePacketPool(
    OUT PNDIS_STATUS            Status,
    OUT PNDIS_HANDLE            PoolHandle,
    IN  UINT                    NumberOfDescriptors,
    IN  UINT                    ProtocolReservedLength
    );

EXPORT
VOID
NdisAllocatePacketPoolEx(
    OUT PNDIS_STATUS            Status,
    OUT PNDIS_HANDLE            PoolHandle,
    IN  UINT                    NumberOfDescriptors,
    IN  UINT                    NumberOfOverflowDescriptors,
    IN  UINT                    ProtocolReservedLength
    );

EXPORT
VOID
NdisSetPacketPoolProtocolId(
    IN  NDIS_HANDLE             PacketPoolHandle,
    IN  UINT                    ProtocolId
    );

EXPORT
UINT
NdisPacketPoolUsage(
    IN  NDIS_HANDLE             PoolHandle
    );

EXPORT
UINT
NdisPacketSize(
    IN  UINT                    ProtocolReservedSize
    );

EXPORT
NDIS_HANDLE
NdisGetPoolFromPacket(
    IN  PNDIS_PACKET            Packet
    );

EXPORT
PNDIS_PACKET_STACK
NdisIMGetCurrentPacketStack(
    IN  PNDIS_PACKET            Packet,
    OUT BOOLEAN *               StacksRemaining
    );

EXPORT
VOID
NdisFreePacketPool(
    IN  NDIS_HANDLE             PoolHandle
    );

EXPORT
VOID
NdisFreePacket(
    IN  PNDIS_PACKET            Packet
    );

EXPORT
VOID
NdisDprFreePacket(
    IN  PNDIS_PACKET            Packet
    );

EXPORT
VOID
NdisDprFreePacketNonInterlocked(
    IN  PNDIS_PACKET            Packet
    );


EXPORT
VOID
NdisAllocatePacket(
    OUT PNDIS_STATUS            Status,
    OUT PNDIS_PACKET *          Packet,
    IN  NDIS_HANDLE             PoolHandle
    );

EXPORT
VOID
NdisDprAllocatePacket(
    OUT PNDIS_STATUS            Status,
    OUT PNDIS_PACKET *          Packet,
    IN  NDIS_HANDLE             PoolHandle
    );

EXPORT
VOID
NdisDprAllocatePacketNonInterlocked(
    OUT PNDIS_STATUS            Status,
    OUT PNDIS_PACKET *          Packet,
    IN  NDIS_HANDLE             PoolHandle
    );

// VOID
// NdisReinitializePacket(
//  IN OUT PNDIS_PACKET         Packet
//  );
#define NdisReinitializePacket(Packet)                                      \
{                                                                           \
    (Packet)->Private.Head = (PNDIS_BUFFER)NULL;                            \
    (Packet)->Private.ValidCounts = FALSE;                                  \
}

#endif // NDIS_LEGACY_DRIVER

#define NdisFreeBuffer(Buffer)  IoFreeMdl(Buffer)

#if NDIS_LEGACY_DRIVER
#define NdisQueryBuffer(_Buffer, _VirtualAddress, _Length)                  \
{                                                                           \
    if (ARGUMENT_PRESENT(_VirtualAddress))                                  \
    {                                                                       \
        *(PVOID *)(_VirtualAddress) = MmGetSystemAddressForMdl(_Buffer);    \
    }                                                                       \
    *(_Length) = MmGetMdlByteCount(_Buffer);                                \
}
#endif // NDIS_LEGACY_DRIVER

#define NdisQueryBufferSafe(_Buffer, _VirtualAddress, _Length, _Priority)   \
{                                                                           \
    if (ARGUMENT_PRESENT(_VirtualAddress))                                  \
    {                                                                       \
        *(PVOID *)(_VirtualAddress) = MmGetSystemAddressForMdlSafe(_Buffer, _Priority); \
    }                                                                       \
    *(_Length) = MmGetMdlByteCount(_Buffer);                                \
}

#define NdisQueryBufferOffset(_Buffer, _Offset, _Length)                    \
{                                                                           \
    *(_Offset) = MmGetMdlByteOffset(_Buffer);                               \
    *(_Length) = MmGetMdlByteCount(_Buffer);                                \
}


#if NDIS_LEGACY_DRIVER

#define NdisGetFirstBufferFromPacket(_Packet,                               \
                                     _FirstBuffer,                          \
                                     _FirstBufferVA,                        \
                                     _FirstBufferLength,                    \
                                     _TotalBufferLength)                    \
    {                                                                       \
        PNDIS_BUFFER    _pBuf;                                              \
                                                                            \
        _pBuf = (_Packet)->Private.Head;                                    \
        *(_FirstBuffer) = _pBuf;                                            \
        if (_pBuf)                                                          \
        {                                                                   \
            *(_FirstBufferVA) = MmGetSystemAddressForMdl(_pBuf);            \
            *(_FirstBufferLength) =                                         \
            *(_TotalBufferLength) = MmGetMdlByteCount(_pBuf);               \
            for (_pBuf = _pBuf->Next;                                       \
                 _pBuf != NULL;                                             \
                 _pBuf = _pBuf->Next)                                       \
            {                                                               \
                *(_TotalBufferLength) += MmGetMdlByteCount(_pBuf);          \
            }                                                               \
        }                                                                   \
        else                                                                \
        {                                                                   \
            *(_FirstBufferVA) = 0;                                          \
            *(_FirstBufferLength) = 0;                                      \
            *(_TotalBufferLength) = 0;                                      \
        }                                                                   \
    }

#define NdisGetFirstBufferFromPacketSafe(_Packet,                           \
                                         _FirstBuffer,                      \
                                         _FirstBufferVA,                    \
                                         _FirstBufferLength,                \
                                         _TotalBufferLength,                \
                                         _Priority)                         \
    {                                                                       \
        PNDIS_BUFFER    _pBuf;                                              \
                                                                            \
        _pBuf = (_Packet)->Private.Head;                                    \
        *(_FirstBuffer) = _pBuf;                                            \
        if (_pBuf)                                                          \
        {                                                                   \
            *(_FirstBufferVA) = MmGetSystemAddressForMdlSafe(_pBuf, _Priority); \
            *(_FirstBufferLength) = *(_TotalBufferLength) = MmGetMdlByteCount(_pBuf); \
            for (_pBuf = _pBuf->Next;                                       \
                 _pBuf != NULL;                                             \
                 _pBuf = _pBuf->Next)                                       \
            {                                                               \
                *(_TotalBufferLength) += MmGetMdlByteCount(_pBuf);          \
            }                                                               \
        }                                                                   \
        else                                                                \
        {                                                                   \
            *(_FirstBufferVA) = 0;                                          \
            *(_FirstBufferLength) = 0;                                      \
            *(_TotalBufferLength) = 0;                                      \
        }                                                                   \
    }

#endif // NDIS_LEGACY_DRIVER

#define NDIS_BUFFER_TO_SPAN_PAGES(_Buffer)                                  \
    (MmGetMdlByteCount(_Buffer)==0 ?                                        \
                1 :                                                         \
                (ADDRESS_AND_SIZE_TO_SPAN_PAGES(                            \
                        MmGetMdlVirtualAddress(_Buffer),                    \
                        MmGetMdlByteCount(_Buffer))))

#define NdisGetBufferPhysicalArraySize(Buffer, ArraySize)                   \
    (*(ArraySize) = NDIS_BUFFER_TO_SPAN_PAGES(Buffer))


/*++

NDIS_BUFFER_LINKAGE(
    IN  PNDIS_BUFFER            Buffer
    );

--*/

#define NDIS_BUFFER_LINKAGE(Buffer) ((Buffer)->Next)


#if NDIS_LEGACY_DRIVER

/*++

VOID
NdisRecalculatePacketCounts(
    IN OUT PNDIS_PACKET         Packet
    );

--*/

#define NdisRecalculatePacketCounts(Packet)                                 \
{                                                                           \
    {                                                                       \
        PNDIS_BUFFER TmpBuffer = (Packet)->Private.Head;                    \
        if (TmpBuffer)                                                      \
        {                                                                   \
            while (TmpBuffer->Next)                                         \
            {                                                               \
                TmpBuffer = TmpBuffer->Next;                                \
            }                                                               \
            (Packet)->Private.Tail = TmpBuffer;                             \
        }                                                                   \
        (Packet)->Private.ValidCounts = FALSE;                              \
    }                                                                       \
}


/*++

VOID
NdisChainBufferAtFront(
    IN OUT PNDIS_PACKET         Packet,
    IN OUT PNDIS_BUFFER         Buffer
    );

--*/

#define NdisChainBufferAtFront(Packet, Buffer)                              \
{                                                                           \
    PNDIS_BUFFER TmpBuffer = (Buffer);                                      \
                                                                            \
    for (;;)                                                                \
    {                                                                       \
        if (TmpBuffer->Next == (PNDIS_BUFFER)NULL)                          \
            break;                                                          \
        TmpBuffer = TmpBuffer->Next;                                        \
    }                                                                       \
    if ((Packet)->Private.Head == NULL)                                     \
    {                                                                       \
        (Packet)->Private.Tail = TmpBuffer;                                 \
    }                                                                       \
    TmpBuffer->Next = (Packet)->Private.Head;                               \
    (Packet)->Private.Head = (Buffer);                                      \
    (Packet)->Private.ValidCounts = FALSE;                                  \
}

/*++

VOID
NdisChainBufferAtBack(
    IN OUT PNDIS_PACKET         Packet,
    IN OUT PNDIS_BUFFER         Buffer
    );

--*/

#define NdisChainBufferAtBack(Packet, Buffer)                               \
{                                                                           \
    PNDIS_BUFFER TmpBuffer = (Buffer);                                      \
                                                                            \
    for (;;)                                                                \
    {                                                                       \
        if (TmpBuffer->Next == NULL)                                        \
            break;                                                          \
        TmpBuffer = TmpBuffer->Next;                                        \
    }                                                                       \
    if ((Packet)->Private.Head != NULL)                                     \
    {                                                                       \
        (Packet)->Private.Tail->Next = (Buffer);                            \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        (Packet)->Private.Head = (Buffer);                                  \
    }                                                                       \
    (Packet)->Private.Tail = TmpBuffer;                                     \
    (Packet)->Private.ValidCounts = FALSE;                                  \
}

EXPORT
VOID
NdisUnchainBufferAtFront(
    IN OUT PNDIS_PACKET         Packet,
    OUT PNDIS_BUFFER *          Buffer
    );

EXPORT
VOID
NdisUnchainBufferAtBack(
    IN OUT PNDIS_PACKET         Packet,
    OUT PNDIS_BUFFER *          Buffer
    );


/*++

VOID
NdisQueryPacket(
    IN  PNDIS_PACKET            _Packet,
    OUT PUINT                   _PhysicalBufferCount OPTIONAL,
    OUT PUINT                   _BufferCount OPTIONAL,
    OUT PNDIS_BUFFER *          _FirstBuffer OPTIONAL,
    OUT PUINT                   _TotalPacketLength OPTIONAL
    );

--*/

#pragma warning(push)
#pragma warning(disable:4127)
__inline
VOID
NdisQueryPacket(
    IN  PNDIS_PACKET            _Packet,
    OUT PUINT                   _PhysicalBufferCount OPTIONAL,
    OUT PUINT                   _BufferCount OPTIONAL,
    OUT PNDIS_BUFFER *          _FirstBuffer OPTIONAL,
    OUT PUINT                   _TotalPacketLength OPTIONAL
    )
{
    if ((_FirstBuffer) != NULL)
    {
        PNDIS_BUFFER * __FirstBuffer = (_FirstBuffer);
        *(__FirstBuffer) = (_Packet)->Private.Head;
    }
    if ((_TotalPacketLength) || (_BufferCount) || (_PhysicalBufferCount))
    {
        if (!(_Packet)->Private.ValidCounts)
        {
            PNDIS_BUFFER TmpBuffer = (_Packet)->Private.Head;
            UINT PTotalLength = 0, PPhysicalCount = 0, PAddedCount = 0;
            UINT PacketLength, Offset;

            while (TmpBuffer != (PNDIS_BUFFER)NULL)
            {
                NdisQueryBufferOffset(TmpBuffer, &Offset, &PacketLength);
                PTotalLength += PacketLength;
                PPhysicalCount += (UINT)NDIS_BUFFER_TO_SPAN_PAGES(TmpBuffer);
                ++PAddedCount;
                TmpBuffer = TmpBuffer->Next;
            }
            (_Packet)->Private.Count = PAddedCount;
            (_Packet)->Private.TotalLength = PTotalLength;
            (_Packet)->Private.PhysicalCount = PPhysicalCount;
            (_Packet)->Private.ValidCounts = TRUE;
        }

        if (_PhysicalBufferCount)
        {
            PUINT __PhysicalBufferCount = (_PhysicalBufferCount);
            *(__PhysicalBufferCount) = (_Packet)->Private.PhysicalCount;
        }
        if (_BufferCount)
        {
            PUINT __BufferCount = (_BufferCount);
            *(__BufferCount) = (_Packet)->Private.Count;
        }
        if (_TotalPacketLength)
        {
            PUINT __TotalPacketLength = (_TotalPacketLength);
            *(__TotalPacketLength) = (_Packet)->Private.TotalLength;
        }
    }
}
#pragma warning(pop)

/*++

VOID
NdisQueryPacketLength(
    IN  PNDIS_PACKET            _Packet,
    OUT PUINT                   _TotalPacketLength OPTIONAL
    );

--*/

#define NdisQueryPacketLength(_Packet,                                      \
                              _TotalPacketLength)                           \
{                                                                           \
    if (!(_Packet)->Private.ValidCounts)                                    \
    {                                                                       \
        NdisQueryPacket(_Packet, NULL, NULL, NULL, _TotalPacketLength);     \
    }                                                                       \
    else *(_TotalPacketLength) = (_Packet)->Private.TotalLength;            \
}

#endif // NDIS_LEGACY_DRIVER


/*++

VOID
NdisGetNextBuffer(
    IN  PNDIS_BUFFER            CurrentBuffer,
    OUT PNDIS_BUFFER *          NextBuffer
    );

--*/

#define NdisGetNextBuffer(CurrentBuffer, NextBuffer)                        \
{                                                                           \
    *(NextBuffer) = (CurrentBuffer)->Next;                                  \
}


#define NdisAdjustBufferLength(Buffer, Length)  (((Buffer)->ByteCount) = (Length))


#if NDIS_SUPPORT_NDIS6
/*
VOID
NdisAdjustMdlLength(
    IN  PMDL                    Mdl,
    IN  UINT                    Length
    );

*/
#define NdisAdjustMdlLength(_Mdl, _Length)  (((_Mdl)->ByteCount) = (_Length))
#endif // NDIS_SUPPORT_NDIS6

#if NDIS_LEGACY_DRIVER

EXPORT
VOID
NdisCopyFromPacketToPacket(
    IN  PNDIS_PACKET            Destination,
    IN  UINT                    DestinationOffset,
    IN  UINT                    BytesToCopy,
    IN  PNDIS_PACKET            Source,
    IN  UINT                    SourceOffset,
    OUT PUINT                   BytesCopied
    );

EXPORT
VOID
NdisCopyFromPacketToPacketSafe(
    IN  PNDIS_PACKET            Destination,
    IN  UINT                    DestinationOffset,
    IN  UINT                    BytesToCopy,
    IN  PNDIS_PACKET            Source,
    IN  UINT                    SourceOffset,
    OUT PUINT                   BytesCopied,
    IN  MM_PAGE_PRIORITY        Priority
    );

DECLSPEC_DEPRECATED_DDK
EXPORT
NDIS_STATUS
NdisAllocateMemory(
    OUT PVOID *                 VirtualAddress,
    IN  UINT                    Length,
    IN  UINT                    MemoryFlags,
    IN  NDIS_PHYSICAL_ADDRESS   HighestAcceptableAddress
    );

#endif // NDIS_LEGACY_DRIVER

EXPORT
NDIS_STATUS
NdisAllocateMemoryWithTag(
    OUT PVOID *                 VirtualAddress,
    IN  UINT                    Length,
    IN  ULONG                   Tag
    );

EXPORT
VOID
NdisFreeMemory(
    IN  PVOID                   VirtualAddress,
    IN  UINT                    Length,
    IN  UINT                    MemoryFlags
    );


/*++
VOID
NdisStallExecution(
    IN  UINT                    MicrosecondsToStall
    )
--*/

#define NdisStallExecution(MicroSecondsToStall)     KeStallExecutionProcessor(MicroSecondsToStall)

EXPORT
VOID
NdisInitializeEvent(
    IN  PNDIS_EVENT             Event
);

EXPORT
VOID
NdisSetEvent(
    IN  PNDIS_EVENT             Event
);

EXPORT
VOID
NdisResetEvent(
    IN  PNDIS_EVENT             Event
);

EXPORT
BOOLEAN
NdisWaitEvent(
    IN  PNDIS_EVENT             Event,
    IN  UINT                    msToWait
);


#if NDIS_LEGACY_DRIVER
/*++
VOID
NdisInitializeWorkItem(
    IN  PNDIS_WORK_ITEM         WorkItem,
    IN  NDIS_PROC               Routine,
    IN  PVOID                   Context
    );
--*/

#define NdisInitializeWorkItem(_WI_, _R_, _C_)  \
    {                                           \
        (_WI_)->Context = _C_;                  \
        (_WI_)->Routine = _R_;                  \
    }

EXPORT
NDIS_STATUS
NdisScheduleWorkItem(
    IN  PNDIS_WORK_ITEM         WorkItem
    );

#endif // NDIS_LEGACY_DRIVER

//
// Simple I/O support
//

EXPORT
VOID
NdisOpenFile(
    OUT PNDIS_STATUS            Status,
    OUT PNDIS_HANDLE            FileHandle,
    OUT PUINT                   FileLength,
    IN  PNDIS_STRING            FileName,
    IN  NDIS_PHYSICAL_ADDRESS   HighestAcceptableAddress
    );

EXPORT
VOID
NdisCloseFile(
    IN  NDIS_HANDLE             FileHandle
    );

EXPORT
VOID
NdisMapFile(
    OUT PNDIS_STATUS            Status,
    OUT PVOID *                 MappedBuffer,
    IN  NDIS_HANDLE             FileHandle
    );

EXPORT
VOID
NdisUnmapFile(
    IN  NDIS_HANDLE             FileHandle
    );


//
// Portability extensions
//

/*++
VOID
NdisFlushBuffer(
    IN  PNDIS_BUFFER            Buffer,
    IN  BOOLEAN                 WriteToDevice
    )
--*/

#define NdisFlushBuffer(Buffer,WriteToDevice)                               \
        KeFlushIoBuffers((Buffer),!(WriteToDevice), TRUE)

EXPORT
ULONG
NdisGetSharedDataAlignment(
    VOID
    );


//
// Write Port
//

/*++
VOID
NdisWritePortUchar(
    IN  NDIS_HANDLE             NdisAdapterHandle,
    IN  ULONG                   Port,
    IN  UCHAR                   Data
    )
--*/
#define NdisWritePortUchar(Handle,Port,Data)                                \
        WRITE_PORT_UCHAR((PUCHAR)(NDIS_PORT_TO_PORT(Handle,Port)),(UCHAR)(Data))

/*++
VOID
NdisWritePortUshort(
    IN  NDIS_HANDLE             NdisAdapterHandle,
    IN  ULONG                   Port,
    IN  USHORT                  Data
    )
--*/
#define NdisWritePortUshort(Handle,Port,Data)                               \
        WRITE_PORT_USHORT((PUSHORT)(NDIS_PORT_TO_PORT(Handle,Port)),(USHORT)(Data))


/*++
VOID
NdisWritePortUlong(
    IN  NDIS_HANDLE             NdisAdapterHandle,
    IN  ULONG                   Port,
    IN  ULONG                   Data
    )
--*/
#define NdisWritePortUlong(Handle,Port,Data)                                \
        WRITE_PORT_ULONG((PULONG)(NDIS_PORT_TO_PORT(Handle,Port)),(ULONG)(Data))


//
// Write Port Buffers
//

/*++
VOID
NdisWritePortBufferUchar(
    IN  NDIS_HANDLE             NdisAdapterHandle,
    IN  ULONG                   Port,
    IN  PUCHAR                  Buffer,
    IN  ULONG                   Length
    )
--*/
#define NdisWritePortBufferUchar(Handle,Port,Buffer,Length)                 \
        NdisRawWritePortBufferUchar(NDIS_PORT_TO_PORT((Handle),(Port)),(Buffer),(Length))

/*++
VOID
NdisWritePortBufferUshort(
    IN  NDIS_HANDLE             NdisAdapterHandle,
    IN  ULONG                   Port,
    IN  PUSHORT                 Buffer,
    IN  ULONG                   Length
    )
--*/
#define NdisWritePortBufferUshort(Handle,Port,Buffer,Length)                \
        NdisRawWritePortBufferUshort(NDIS_PORT_TO_PORT((Handle),(Port)),(Buffer),(Length))


/*++
VOID
NdisWritePortBufferUlong(
    IN  NDIS_HANDLE             NdisAdapterHandle,
    IN  ULONG                   Port,
    IN  PULONG                  Buffer,
    IN  ULONG                   Length
    )
--*/
#define NdisWritePortBufferUlong(Handle,Port,Buffer,Length)                 \
        NdisRawWritePortBufferUlong(NDIS_PORT_TO_PORT((Handle),(Port)),(Buffer),(Length))


//
// Read Ports
//

/*++
VOID
NdisReadPortUchar(
    IN  NDIS_HANDLE             NdisAdapterHandle,
    IN  ULONG                   Port,
    OUT PUCHAR                  Data
    )
--*/
#define NdisReadPortUchar(Handle,Port, Data)                                \
        NdisRawReadPortUchar(NDIS_PORT_TO_PORT((Handle),(Port)),(Data))

/*++
VOID
NdisReadPortUshort(
    IN  NDIS_HANDLE             NdisAdapterHandle,
    IN  ULONG                   Port,
    OUT PUSHORT                 Data
    )
--*/
#define NdisReadPortUshort(Handle,Port,Data)                                \
        NdisRawReadPortUshort(NDIS_PORT_TO_PORT((Handle),(Port)),(Data))


/*++
VOID
NdisReadPortUlong(
    IN  NDIS_HANDLE             NdisAdapterHandle,
    IN  ULONG                   Port,
    OUT PULONG                  Data
    )
--*/
#define NdisReadPortUlong(Handle,Port,Data)                                 \
        NdisRawReadPortUlong(NDIS_PORT_TO_PORT((Handle),(Port)),(Data))

//
// Read Buffer Ports
//

/*++
VOID
NdisReadPortBufferUchar(
    IN  NDIS_HANDLE             NdisAdapterHandle,
    IN  ULONG                   Port,
    OUT PUCHAR                  Buffer,
    IN  ULONG                   Length
    )
--*/
#define NdisReadPortBufferUchar(Handle,Port,Buffer,Length)                  \
        NdisRawReadPortBufferUchar(NDIS_PORT_TO_PORT((Handle),(Port)),(Buffer),(Length))

/*++
VOID
NdisReadPortBufferUshort(
    IN  NDIS_HANDLE             NdisAdapterHandle,
    IN  ULONG                   Port,
    OUT PUSHORT                 Buffer,
    IN  ULONG                   Length
    )
--*/
#define NdisReadPortBufferUshort(Handle,Port,Buffer,Length)                 \
        NdisRawReadPortBufferUshort(NDIS_PORT_TO_PORT((Handle),(Port)),(Buffer),(Length))

/*++
VOID
NdisReadPortBufferUlong(
    IN  NDIS_HANDLE             NdisAdapterHandle,
    IN  ULONG                   Port,
    OUT PULONG                  Buffer,
    IN  ULONG                   Length
    )
--*/
#define NdisReadPortBufferUlong(Handle,Port,Buffer)                         \
        NdisRawReadPortBufferUlong(NDIS_PORT_TO_PORT((Handle),(Port)),(Buffer),(Length))

//
// Raw Routines
//

//
// Write Port Raw
//

/*++
VOID
NdisRawWritePortUchar(
    IN  ULONG_PTR               Port,
    IN  UCHAR                   Data
    )
--*/
#define NdisRawWritePortUchar(Port,Data)                                    \
        WRITE_PORT_UCHAR((PUCHAR)(Port),(UCHAR)(Data))

/*++
VOID
NdisRawWritePortUshort(
    IN  ULONG_PTR               Port,
    IN  USHORT                  Data
    )
--*/
#define NdisRawWritePortUshort(Port,Data)                                   \
        WRITE_PORT_USHORT((PUSHORT)(Port),(USHORT)(Data))

/*++
VOID
NdisRawWritePortUlong(
    IN  ULONG_PTR               Port,
    IN  ULONG                   Data
    )
--*/
#define NdisRawWritePortUlong(Port,Data)                                    \
        WRITE_PORT_ULONG((PULONG)(Port),(ULONG)(Data))


//
// Raw Write Port Buffers
//

/*++
VOID
NdisRawWritePortBufferUchar(
    IN  ULONG_PTR               Port,
    IN  PUCHAR                  Buffer,
    IN  ULONG                   Length
    )
--*/
#define NdisRawWritePortBufferUchar(Port,Buffer,Length) \
        WRITE_PORT_BUFFER_UCHAR((PUCHAR)(Port),(PUCHAR)(Buffer),(Length))

/*++
VOID
NdisRawWritePortBufferUshort(
    IN  ULONG_PTR               Port,
    IN  PUSHORT                 Buffer,
    IN  ULONG                   Length
    )
--*/
#if defined(_M_IX86) || defined(_M_AMD64)
#define NdisRawWritePortBufferUshort(Port,Buffer,Length)                    \
        WRITE_PORT_BUFFER_USHORT((PUSHORT)(Port),(PUSHORT)(Buffer),(Length))
#else
#define NdisRawWritePortBufferUshort(Port,Buffer,Length)                    \
{                                                                           \
        ULONG_PTR _Port = (ULONG_PTR)(Port);                                \
        PUSHORT _Current = (Buffer);                                        \
        PUSHORT _End = _Current + (Length);                                 \
        for ( ; _Current < _End; ++_Current)                                \
        {                                                                   \
            WRITE_PORT_USHORT((PUSHORT)_Port,*(UNALIGNED USHORT *)_Current);\
        }                                                                   \
}
#endif


/*++
VOID
NdisRawWritePortBufferUlong(
    IN  ULONG_PTR               Port,
    IN  PULONG                  Buffer,
    IN  ULONG                   Length
    )
--*/
#if defined(_M_IX86) || defined(_M_AMD64)
#define NdisRawWritePortBufferUlong(Port,Buffer,Length)                     \
        WRITE_PORT_BUFFER_ULONG((PULONG)(Port),(PULONG)(Buffer),(Length))
#else
#define NdisRawWritePortBufferUlong(Port,Buffer,Length)                     \
{                                                                           \
        ULONG_PTR _Port = (ULONG_PTR)(Port);                                \
        PULONG _Current = (Buffer);                                         \
        PULONG _End = _Current + (Length);                                  \
        for ( ; _Current < _End; ++_Current)                                \
        {                                                                   \
            WRITE_PORT_ULONG((PULONG)_Port,*(UNALIGNED ULONG *)_Current);   \
        }                                                                   \
}
#endif


//
// Raw Read Ports
//

/*++
VOID
NdisRawReadPortUchar(
    IN  ULONG_PTR               Port,
    OUT PUCHAR                  Data
    )
--*/
#define NdisRawReadPortUchar(Port, Data) \
        *(Data) = READ_PORT_UCHAR((PUCHAR)(Port))

/*++
VOID
NdisRawReadPortUshort(
    IN  ULONG_PTR               Port,
    OUT PUSHORT                 Data
    )
--*/
#define NdisRawReadPortUshort(Port,Data) \
        *(Data) = READ_PORT_USHORT((PUSHORT)(Port))

/*++
VOID
NdisRawReadPortUlong(
    IN  ULONG_PTR               Port,
    OUT PULONG                  Data
    )
--*/
#define NdisRawReadPortUlong(Port,Data) \
        *(Data) = READ_PORT_ULONG((PULONG)(Port))


//
// Raw Read Buffer Ports
//

/*++
VOID
NdisRawReadPortBufferUchar(
    IN  ULONG_PTR               Port,
    OUT PUCHAR                  Buffer,
    IN  ULONG                   Length
    )
--*/
#define NdisRawReadPortBufferUchar(Port,Buffer,Length)                      \
        READ_PORT_BUFFER_UCHAR((PUCHAR)(Port),(PUCHAR)(Buffer),(Length))


/*++
VOID
NdisRawReadPortBufferUshort(
    IN  ULONG_PTR               Port,
    OUT PUSHORT                 Buffer,
    IN  ULONG                   Length
    )
--*/
#if defined(_M_IX86) || defined(_M_AMD64)
#define NdisRawReadPortBufferUshort(Port,Buffer,Length)                     \
        READ_PORT_BUFFER_USHORT((PUSHORT)(Port),(PUSHORT)(Buffer),(Length))
#else
#define NdisRawReadPortBufferUshort(Port,Buffer,Length)                     \
{                                                                           \
        ULONG_PTR _Port = (ULONG_PTR)(Port);                                \
        PUSHORT _Current = (Buffer);                                        \
        PUSHORT _End = _Current + (Length);                                 \
        for ( ; _Current < _End; ++_Current)                                \
        {                                                                   \
            *(UNALIGNED USHORT *)_Current = READ_PORT_USHORT((PUSHORT)_Port); \
        }                                                                   \
}
#endif


/*++
VOID
NdisRawReadPortBufferUlong(
    IN  ULONG_PTR               Port,
    OUT PULONG                  Buffer,
    IN  ULONG                   Length
    )
--*/
#if defined(_M_IX86) || defined(_M_AMD64)
#define NdisRawReadPortBufferUlong(Port,Buffer,Length)                      \
        READ_PORT_BUFFER_ULONG((PULONG)(Port),(PULONG)(Buffer),(Length))
#else
#define NdisRawReadPortBufferUlong(Port,Buffer,Length)                      \
{                                                                           \
        ULONG_PTR _Port = (ULONG_PTR)(Port);                                \
        PULONG _Current = (Buffer);                                         \
        PULONG _End = _Current + (Length);                                  \
        for ( ; _Current < _End; ++_Current)                                \
        {                                                                   \
            *(UNALIGNED ULONG *)_Current = READ_PORT_ULONG((PULONG)_Port);  \
        }                                                                   \
}
#endif


//
// Write Registers
//

/*++
VOID
NdisWriteRegisterUchar(
    IN  PUCHAR                  Register,
    IN  UCHAR                   Data
    )
--*/

#if defined(_M_IX86) || defined(_M_AMD64)
#define NdisWriteRegisterUchar(Register,Data)                               \
        WRITE_REGISTER_UCHAR((Register),(Data))
#else
#define NdisWriteRegisterUchar(Register,Data)                               \
    {                                                                       \
        WRITE_REGISTER_UCHAR((Register),(Data));                            \
        READ_REGISTER_UCHAR(Register);                                      \
    }
#endif

/*++
VOID
NdisWriteRegisterUshort(
    IN  PUCHAR                  Register,
    IN  USHORT                  Data
    )
--*/

#if defined(_M_IX86) || defined(_M_AMD64)
#define NdisWriteRegisterUshort(Register,Data)                              \
        WRITE_REGISTER_USHORT((Register),(Data))
#else
#define NdisWriteRegisterUshort(Register,Data)                              \
    {                                                                       \
        WRITE_REGISTER_USHORT((Register),(Data));                           \
        READ_REGISTER_USHORT(Register);                                     \
    }
#endif

/*++
VOID
NdisWriteRegisterUlong(
    IN  PUCHAR                  Register,
    IN  ULONG                   Data
    )
--*/

#if defined(_M_IX86) || defined(_M_AMD64)
#define NdisWriteRegisterUlong(Register,Data)   WRITE_REGISTER_ULONG((Register),(Data))
#else
#define NdisWriteRegisterUlong(Register,Data)                               \
    {                                                                       \
        WRITE_REGISTER_ULONG((Register),(Data));                            \
        READ_REGISTER_ULONG(Register);                                      \
    }
#endif

/*++
VOID
NdisReadRegisterUchar(
    IN  PUCHAR                  Register,
    OUT PUCHAR                  Data
    )
--*/
#if defined(_M_IX86)
#define NdisReadRegisterUchar(Register,Data)    \
        _ReadWriteBarrier();                    \
        *(Data) = *((volatile UCHAR * const)(Register));
#else
#define NdisReadRegisterUchar(Register,Data)    *(Data) = READ_REGISTER_UCHAR((PUCHAR)(Register))
#endif

/*++
VOID
NdisReadRegisterUshort(
    IN  PUSHORT                 Register,
    OUT PUSHORT                 Data
    )
--*/
#if defined(_M_IX86)
#define NdisReadRegisterUshort(Register,Data)   \
        _ReadWriteBarrier();                    \
        *(Data) = *((volatile USHORT * const)(Register))
#else
#define NdisReadRegisterUshort(Register,Data)   *(Data) = READ_REGISTER_USHORT((PUSHORT)(Register))
#endif

/*++
VOID
NdisReadRegisterUlong(
    IN  PULONG                  Register,
    OUT PULONG                  Data
    )
--*/
#if defined(_M_IX86)
#define NdisReadRegisterUlong(Register,Data)    \
        _ReadWriteBarrier();                    \
        *(Data) = *((volatile ULONG * const)(Register))
#else
#define NdisReadRegisterUlong(Register,Data)    *(Data) = READ_REGISTER_ULONG((PULONG)(Register))
#endif

#define NdisEqualString(_String1, _String2, _CaseInsensitive)               \
            RtlEqualUnicodeString(_String1, _String2, _CaseInsensitive)

#define NdisEqualUnicodeString(_String1, _String2, _CaseInsensitive)        \
            RtlEqualUnicodeString(_String1, _String2, _CaseInsensitive)

EXPORT
VOID __cdecl
NdisWriteErrorLogEntry(
    IN  NDIS_HANDLE             NdisAdapterHandle,
    IN  NDIS_ERROR_CODE         ErrorCode,
    IN  ULONG                   NumberOfErrorValues,
    ...
    );

EXPORT
VOID
NdisInitializeString(
    OUT PNDIS_STRING    Destination,
    IN  PUCHAR          Source
    );

#define NdisFreeString(String) NdisFreeMemory((String).Buffer, (String).MaximumLength, 0)

#define NdisPrintString(String) DbgPrint("%ls",(String).Buffer)


/*++

VOID
NdisCreateLookaheadBufferFromSharedMemory(
    IN  PVOID                   pSharedMemory,
    IN  UINT                    LookaheadLength,
    OUT PVOID *                 pLookaheadBuffer
    );

--*/

#define NdisCreateLookaheadBufferFromSharedMemory(_S, _L, _B)   ((*(_B)) = (_S))

/*++

VOID
NdisDestroyLookaheadBufferFromSharedMemory(
    IN  PVOID                   pLookaheadBuffer
    );

--*/

#define NdisDestroyLookaheadBufferFromSharedMemory(_B)


//
// The following declarations are shared between ndismac.h and ndismini.h. They
// are meant to be for internal use only. They should not be used directly by
// miniport drivers.
//

//
// declare these first since they point to each other
//

typedef struct _NDIS_WRAPPER_HANDLE     NDIS_WRAPPER_HANDLE, *PNDIS_WRAPPER_HANDLE;
typedef struct _NDIS_PROTOCOL_BLOCK     NDIS_PROTOCOL_BLOCK, *PNDIS_PROTOCOL_BLOCK;
typedef struct _NDIS_OPEN_BLOCK         NDIS_OPEN_BLOCK, *PNDIS_OPEN_BLOCK;
typedef struct _NDIS_M_DRIVER_BLOCK     NDIS_M_DRIVER_BLOCK, *PNDIS_M_DRIVER_BLOCK;
typedef struct _NDIS_MINIPORT_BLOCK     NDIS_MINIPORT_BLOCK,*PNDIS_MINIPORT_BLOCK;
typedef struct _CO_CALL_PARAMETERS      CO_CALL_PARAMETERS, *PCO_CALL_PARAMETERS;
typedef struct _CO_MEDIA_PARAMETERS     CO_MEDIA_PARAMETERS, *PCO_MEDIA_PARAMETERS;
typedef struct _NDIS_CALL_MANAGER_CHARACTERISTICS *PNDIS_CALL_MANAGER_CHARACTERISTICS;
typedef struct _NDIS_OFFLOAD NDIS_OFFLOAD, *PNDIS_OFFLOAD;
typedef struct _NDIS_AF_LIST            NDIS_AF_LIST, *PNDIS_AF_LIST;
typedef struct _X_FILTER                ETH_FILTER, *PETH_FILTER;
typedef struct _X_FILTER                TR_FILTER, *PTR_FILTER;
typedef struct _X_FILTER                NULL_FILTER, *PNULL_FILTER;

#if NDIS_SUPPORT_NDIS6

typedef USHORT NET_FRAME_TYPE, *PNET_FRAME_TYPE;

#endif NDIS_SUPPORT_NDIS6



//
// Timers.
//

typedef
VOID
(*PNDIS_TIMER_FUNCTION) (
    IN  PVOID                   SystemSpecific1,
    IN  PVOID                   FunctionContext,
    IN  PVOID                   SystemSpecific2,
    IN  PVOID                   SystemSpecific3
    );

typedef struct _NDIS_TIMER
{
    KTIMER      Timer;
    KDPC        Dpc;
} NDIS_TIMER, *PNDIS_TIMER;

EXPORT
VOID
NdisInitializeTimer(
    IN OUT PNDIS_TIMER          Timer,
    IN  PNDIS_TIMER_FUNCTION    TimerFunction,
    IN  PVOID                   FunctionContext
    );

VOID
NdisCancelTimer(
    IN  PNDIS_TIMER             Timer,
    OUT PBOOLEAN                TimerCancelled
    );

EXPORT
VOID
NdisSetTimer(
    IN  PNDIS_TIMER             Timer,
    IN  UINT                    MillisecondsToDelay
    );


EXPORT
VOID
NdisSetPeriodicTimer(
    IN  PNDIS_TIMER             NdisTimer,
    IN  UINT                    MillisecondsPeriod
    );

EXPORT
VOID
NdisSetTimerEx(
    IN  PNDIS_TIMER             Timer,
    IN  UINT                    MillisecondsToDelay,
    IN  PVOID                   FunctionContext
    );


//
// System processor count
//

EXPORT
CCHAR
NdisSystemProcessorCount(
    VOID
    );

 
EXPORT
PVOID
NdisGetRoutineAddress(
    IN PNDIS_STRING     NdisRoutineName
    );

EXPORT
UINT
NdisGetVersion(
    VOID
    );


//
// Ansi/Unicode support routines
//


#define NdisInitAnsiString(_as, s)              RtlInitString(_as, s)
#define NdisInitUnicodeString(_us, s)           RtlInitUnicodeString(_us, s)
#define NdisAnsiStringToUnicodeString(_us, _as) RtlAnsiStringToUnicodeString(_us, _as, FALSE)
#define NdisUnicodeStringToAnsiString(_as, _us) RtlUnicodeStringToAnsiString(_as, _us, FALSE)
#define NdisUpcaseUnicodeString(_d, _s)         RtlUpcaseUnicodeString(_d, _s, FALSE)


//
// Non-paged lookaside list support routines
//

#define NdisInitializeNPagedLookasideList(_L, _AR, _FR, _Fl, _S, _T, _D) \
                ExInitializeNPagedLookasideList(_L, _AR, _FR, _Fl, _S, _T, _D)

#define NdisDeleteNPagedLookasideList(_L)           ExDeleteNPagedLookasideList(_L)
#define NdisAllocateFromNPagedLookasideList(_L)     ExAllocateFromNPagedLookasideList(_L)
#define NdisFreeToNPagedLookasideList(_L, _E)       ExFreeToNPagedLookasideList(_L, _E)

#if NDIS_LEGACY_DRIVER
/*
NdisSetPacketStatus is deprecated. use NDIS_SET_PACKET_STATUS macro
*/
DECLSPEC_DEPRECATED_DDK
EXPORT
VOID
NdisSetPacketStatus(
    IN          PNDIS_PACKET    Packet,
    IN          NDIS_STATUS     Status,
    IN          NDIS_HANDLE     Handle,
    IN          ULONG           Code
    );

#endif

#define NDIS_MAX_EVENT_LOG_DATA_SIZE ((ERROR_LOG_MAXIMUM_SIZE - sizeof(IO_ERROR_LOG_PACKET) + sizeof(ULONG)) & ~3)

#ifdef _WIN64
#define NDIS_MAX_PROCESSOR_COUNT   64
#else
#define NDIS_MAX_PROCESSOR_COUNT   32
#endif


#if NDIS_SUPPORT_NDIS6

#define NDIS_RECEIVE_SCALE_CAPABILITIES_REVISION_1     1

//
// What kind the receive scale capabilities the miniport can support, miniport drivers return
// some of these flags as CapabilitiesFlags in a structure _NDIS_RECEIVE_SCALE_CAPABILITIES 
// when they get a query about theire receive side scale capabilities.
//
#define NDIS_RSS_CAPS_MESSAGE_SIGNALED_INTERRUPTS                    0x01000000
#define NDIS_RSS_CAPS_CLASSIFICATION_AT_ISR                          0x02000000
#define NDIS_RSS_CAPS_CLASSIFICATION_AT_DPC                          0x04000000

//
// What kind of hash field type the miniport can support
//
#define NDIS_RSS_CAPS_HASH_TYPE_TCP_IPV4                             0x00000100
#define NDIS_RSS_CAPS_HASH_TYPE_TCP_IPV6                             0x00000200
#define NDIS_RSS_CAPS_HASH_TYPE_TCP_IPV6_EX                          0x00000400


//
// the following structure defines the Receive scale capabilities of the miniport
//
typedef struct _NDIS_RECEIVE_SCALE_CAPABILITIES
{
    NDIS_OBJECT_HEADER  Header;
    ULONG   CapabilitiesFlags;
    ULONG   NumberOfInterruptMessages;
    ULONG   NumberOfReceiveQueues;

} NDIS_RECEIVE_SCALE_CAPABILITIES, *PNDIS_RECEIVE_SCALE_CAPABILITIES;
#define NDIS_SIZEOF_RECEIVE_SCALE_CAPABILITIES_REVISION_1   \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_RECEIVE_SCALE_CAPABILITIES, NumberOfReceiveQueues)

//
// What hash functions does NDIS support
//
#define NdisHashFunctionToeplitz                            0x00000001 // supported hash function 1 -- Main RSS hash function 
#define NdisHashFunctionReserved1                           0x00000002 // supported hash function 2
#define NdisHashFunctionReserved2                           0x00000004 // supported hash function 3
#define NdisHashFunctionReserved3                           0x00000008 // supported hash function 4

#define NDIS_HASH_FUNCTION_MASK                             0x000000FF
#define NDIS_HASH_TYPE_MASK                                 0x00FFFF00

#define NDIS_RSS_HASH_FUNC_FROM_HASH_INFO(_HashInfo)  \
        ((_HashInfo) & (NDIS_HASH_FUNCTION_MASK))

#define NDIS_RSS_HASH_TYPE_FROM_HASH_INFO(_HashInfo)  \
        ((_HashInfo) & (NDIS_HASH_TYPE_MASK))

#define NDIS_RSS_HASH_INFO_FROM_TYPE_AND_FUNC(_HashType, _HashFunction) \
        ((_HashType) | (_HashFunction))

//
// What kind of hash field type the protocol what the miniport to do
//
#define NDIS_HASH_IPV4                                      0x00000100
#define NDIS_HASH_TCP_IPV4                                  0x00000200
#define NDIS_HASH_IPV6                                      0x00000400
#define NDIS_HASH_IPV6_EX                                   0x00000800
#define NDIS_HASH_TCP_IPV6                                  0x00001000
#define NDIS_HASH_TCP_IPV6_EX                               0x00002000

//
// Flags to denote the parameters that are kept unmodified.
//
#define NDIS_RSS_PARAM_FLAG_BASE_CPU_UNCHANGED              0x0001
#define NDIS_RSS_PARAM_FLAG_HASH_INFO_UNCHANGED             0x0002
#define NDIS_RSS_PARAM_FLAG_ITABLE_UNCHANGED                0x0004
#define NDIS_RSS_PARAM_FLAG_HASH_KEY_UNCHANGED              0x0008
#define NDIS_RSS_PARAM_FLAG_DISABLE_RSS                     0x0010

#define NDIS_RSS_INDIRECTION_TABLE_SIZE_REVISION_1          128
#define NDIS_RSS_HASH_SECRET_KEY_SIZE_REVISION_1            40

//
// Maximum indirection table and private key sizes for the current version.
//
#define NDIS_RSS_INDIRECTION_TABLE_MAX_SIZE_REVISION_1      128
#define NDIS_RSS_HASH_SECRET_KEY_MAX_SIZE_REVISION_1        40


//
// used in OID_GEN_RECEIVE_SCALE_PARAMETERS
//
#define NDIS_RECEIVE_SCALE_PARAMETERS_REVISION_1     1

typedef struct _NDIS_RECEIVE_SCALE_PARAMETERS
{
    NDIS_OBJECT_HEADER      Header;

    // Qualifies the rest of the information.
    USHORT                  Flags;

    // The base CPU number to do receive processing.
    USHORT                  BaseCpuNumber;

    // This describes the hash function and type being enabled.
    ULONG                   HashInformation;

    // The size of indirection table array.
    USHORT                  IndirectionTableSize;    
    // The offset of the indirection table from the beginning of this structure.
    ULONG                   IndirectionTableOffset;  

    // The size of the secret key.
    USHORT                  HashSecretKeySize;   
    // The offset of the secret key from the beginning of this structure.
    ULONG                   HashSecretKeyOffset; 

    // The hash map table is a CCHAR array.
} NDIS_RECEIVE_SCALE_PARAMETERS, *PNDIS_RECEIVE_SCALE_PARAMETERS;

#define NDIS_SIZEOF_RECEIVE_SCALE_PARAMETERS_REVISION_1    \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_RECEIVE_SCALE_PARAMETERS, HashSecretKeyOffset)


//
// Used in OID_GEN_RECEIVE_HASH
//
#define NDIS_RECEIVE_HASH_FLAG_ENABLE_HASH                      0x00000001
#define NDIS_RECEIVE_HASH_FLAG_HASH_INFO_UNCHANGED              0x00000002
#define NDIS_RECEIVE_HASH_FLAG_HASH_KEY_UNCHANGED               0x00000004

#define NDIS_RECEIVE_HASH_PARAMETERS_REVISION_1                 1

typedef struct _NDIS_RECEIVE_HASH_PARAMETERS
{
    NDIS_OBJECT_HEADER      Header;

    // Qualifies the rest of the information.
    ULONG                   Flags;

    // This describes the hash function and type being enabled.
    ULONG                   HashInformation;

    // The size of the secret key.
    USHORT                  HashSecretKeySize;
    
    // The offset of the secret key from the beginning of this structure.
    ULONG                   HashSecretKeyOffset; 
} NDIS_RECEIVE_HASH_PARAMETERS, *PNDIS_RECEIVE_HASH_PARAMETERS;

#define NDIS_SIZEOF_RECEIVE_HASH_PARAMETERS_REVISION_1    \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_RECEIVE_HASH_PARAMETERS, HashSecretKeyOffset)


typedef  struct _NDIS_RESTART_ATTRIBUTES NDIS_RESTART_ATTRIBUTES, *PNDIS_RESTART_ATTRIBUTES;

typedef  struct _NDIS_RESTART_ATTRIBUTES
{  
    PNDIS_RESTART_ATTRIBUTES   Next;
    NDIS_OID                   Oid;
    ULONG                      DataLength;
    DECLSPEC_ALIGN(MEMORY_ALLOCATION_ALIGNMENT) UCHAR  Data[1];
}NDIS_RESTART_ATTRIBUTES, *PNDIS_RESTART_ATTRIBUTES;


 
//
// used in all NDIS drivers
//
typedef
NDIS_STATUS
(*SET_OPTIONS_HANDLER)(
    IN  NDIS_HANDLE             NdisDriverHandle,
    IN  NDIS_HANDLE             DriverContext
    );


#endif // NDIS_SUPPORT_NDIS6

#if NDIS_LEGACY_DRIVER
typedef
NDIS_STATUS
(*WAN_SEND_HANDLER)(
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  NDIS_HANDLE             LinkHandle,
    IN  PVOID                   Packet
    );

typedef
NDIS_STATUS
(*SEND_HANDLER)(
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  PNDIS_PACKET            Packet
    );

typedef
NDIS_STATUS
(*TRANSFER_DATA_HANDLER)(
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  NDIS_HANDLE             MacReceiveContext,
    IN  UINT                    ByteOffset,
    IN  UINT                    BytesToTransfer,
    OUT PNDIS_PACKET            Packet,
    OUT PUINT                   BytesTransferred
    );

typedef
NDIS_STATUS
(*RESET_HANDLER)(
    IN  NDIS_HANDLE             NdisBindingHandle
    );

typedef
NDIS_STATUS
(*REQUEST_HANDLER)(
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  PNDIS_REQUEST           NdisRequest
    );

typedef
VOID
(*SEND_PACKETS_HANDLER)(
    IN NDIS_HANDLE              MiniportAdapterContext,
    IN PPNDIS_PACKET            PacketArray,
    IN UINT                     NumberOfPackets
    );

#endif // NDIS_LEGACY_DRIVER

//
// NDIS object types created by NDIS drivers
//


#if NDIS_SUPPORT_NDIS6

#define NDIS_CURRENT_PROCESSOR_NUMBER KeGetCurrentProcessorNumber()

#define NDIS_CURRENT_IRQL() KeGetCurrentIrql()

#define NDIS_RAISE_IRQL_TO_DISPATCH(_pIrql_)     KeRaiseIrql(DISPATCH_LEVEL, _pIrql_)

#define NDIS_LOWER_IRQL(_OldIrql_, _CurIrql_)                   \
{                                                               \
    if (_OldIrql_ != _CurIrql_) KeLowerIrql(_OldIrql_);         \
}

typedef KMUTEX      NDIS_MUTEX, *PNDIS_MUTEX;

#define NDIS_INIT_MUTEX(_M_)                KeInitializeMutex(_M_, 0xFFFF)
#define NDIS_RELEASE_MUTEX(_M_)             KeReleaseMutex(_M_, FALSE)

#define NDIS_WAIT_FOR_MUTEX(_M_)            KeWaitForSingleObject(_M_,      \
                                                                  Executive,\
                                                                  KernelMode,\
                                                                  FALSE,    \
                                                                  NULL)     \


#define NDIS_CONFIGURATION_OBJECT_REVISION_1               1

//
// Flags for NdisOpenConfigurationEx
//
#define NDIS_CONFIG_FLAG_FILTER_INSTANCE_CONFIGURATION      0x00000001

typedef struct _NDIS_CONFIGURATION_OBJECT
{
    NDIS_OBJECT_HEADER          Header;
    NDIS_HANDLE                 NdisHandle;
    ULONG                       Flags;
} NDIS_CONFIGURATION_OBJECT, *PNDIS_CONFIGURATION_OBJECT;

#define NDIS_SIZEOF_CONFIGURATION_OBJECT_REVISION_1  \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_CONFIGURATION_OBJECT, Flags)

EXPORT
NDIS_STATUS
NdisOpenConfigurationEx(
    IN      PNDIS_CONFIGURATION_OBJECT     ConfigObject,
    OUT     PNDIS_HANDLE                   ConfigurationHandle
    );

EXPORT
PVOID
NdisAllocateMemoryWithTagPriority(
    IN  NDIS_HANDLE             NdisHandle,
    IN  UINT                    Length,
    IN  ULONG                   Tag,
    IN  EX_POOL_PRIORITY        Priority
    );


typedef struct _NDIS_DRIVER_OPTIONAL_HANDLERS
{
    NDIS_OBJECT_HEADER  Header;
} NDIS_DRIVER_OPTIONAL_HANDLERS, *PNDIS_DRIVER_OPTIONAL_HANDLERS;

EXPORT
NDIS_STATUS
NdisSetOptionalHandlers(
    NDIS_HANDLE                     NdisDriverHandle,
    PNDIS_DRIVER_OPTIONAL_HANDLERS  OptionalHandlers
    );

//
// Flags used in NET_PNP_EVENT_NOTIFICATION Flags field
//

#define NET_EVENT_HALT_MINIPORT_ON_LOW_POWER      0x00000001

#define NET_PNP_EVENT_NOTIFICATION_REVISION_1   1

typedef struct _NET_PNP_EVENT_NOTIFICATION
{
    //
    // Caller must set Header to
    //     Type = NDIS_OBJECT_TYPE_DEFAULT
    //     Revision = NET_PNP_EVENT_NOTIFICATION_REVISION_1
    //     Size = sizeof(_NET_PNP_EVENT_NOTIFICATION)
    //
    NDIS_OBJECT_HEADER     Header;
    
    NDIS_PORT_NUMBER       PortNumber;

    NET_PNP_EVENT          NetPnPEvent;
    ULONG                  Flags;

   
} NET_PNP_EVENT_NOTIFICATION, *PNET_PNP_EVENT_NOTIFICATION;

#define NDIS_SIZEOF_NET_PNP_EVENT_NOTIFICATION_REVISION_1   \
        RTL_SIZEOF_THROUGH_FIELD(NET_PNP_EVENT_NOTIFICATION, NetPnPEvent)

//
// new request and status structures
//

#define  NDIS_OID_REQUEST_REVISION_1             1
#define  NDIS_OID_REQUEST_TIMEOUT_INFINITE       0
#define  NDIS_OID_REQUEST_NDIS_RESERVED_SIZE     16

typedef struct _NDIS_OID_REQUEST
{
    //
    // Caller must set Header to
    //     Header.Type = NDIS_OBJECT_TYPE_OID_REQUEST
    //     Header.Revision = NDIS_OID_REQUEST_REVISION_1
    //     Header.Size = sizeof(_NDIS_OID_REQUEST)
    //
    NDIS_OBJECT_HEADER          Header;
    NDIS_REQUEST_TYPE           RequestType;
    NDIS_PORT_NUMBER            PortNumber;
    UINT                        Timeout; // in Seconds
    PVOID                       RequestId;
    NDIS_HANDLE                 RequestHandle;

    //
    // OID - Information
    //
    union _REQUEST_DATA
    {
        struct _QUERY
        {
            NDIS_OID    Oid;
            PVOID       InformationBuffer;
            UINT        InformationBufferLength;
            UINT        BytesWritten;
            UINT        BytesNeeded;
        } QUERY_INFORMATION;
    
        struct _SET
        {
            NDIS_OID    Oid;
            PVOID       InformationBuffer;
            UINT        InformationBufferLength;
            UINT        BytesRead;
            UINT        BytesNeeded;
        } SET_INFORMATION;
    
        struct _METHOD
        {
            NDIS_OID            Oid;
            PVOID               InformationBuffer;
            ULONG               InputBufferLength;
            ULONG               OutputBufferLength;
            ULONG               MethodId;
            UINT                BytesWritten;
            UINT                BytesRead;
            UINT                BytesNeeded;
        } METHOD_INFORMATION;
    } DATA;
    //
    // NDIS Reserved
    //
    UCHAR       NdisReserved[NDIS_OID_REQUEST_NDIS_RESERVED_SIZE * sizeof(PVOID)];
    UCHAR       MiniportReserved[2*sizeof(PVOID)];
    UCHAR       SourceReserved[2*sizeof(PVOID)];
    UCHAR       SupportedRevision;
    UCHAR       Reserved1;
    USHORT      Reserved2;

}NDIS_OID_REQUEST, *PNDIS_OID_REQUEST;

#define NDIS_SIZEOF_OID_REQUEST_REVISION_1     \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_OID_REQUEST, SupportedRevision)
//
// Macros to set, clear and test NDIS_STATUS_INDICATION Flags
//
#define NDIS_STATUS_INDICATION_SET_FLAG(_StatusIndication, _F)     \
    ((_StatusIndication)->Flags |= (_F))
    
#define NDIS_STATUS_INDICATION_TEST_FLAG(_StatusIndication, _F)    \
    (((_StatusIndication)->Flags & (_F)) != 0)

#define NDIS_STATUS_INDICATION_CLEAR_FLAG(_StatusIndication, _F)   \
    ((_StatusIndication)->Flags &= ~(_F))

#define  NDIS_STATUS_INDICATION_FLAGS_NDIS_RESERVED    0xFFF

//
// Public flags for NDIS_STATUS_INDICATION
//
#define  NDIS_STATUS_INDICATION_FLAGS_MEDIA_CONNECT_TO_CONNECT		0x1000

#define  NDIS_STATUS_INDICATION_REVISION_1             1

typedef struct _NDIS_STATUS_INDICATION 
{
    NDIS_OBJECT_HEADER      Header;
    NDIS_HANDLE             SourceHandle;
    NDIS_PORT_NUMBER        PortNumber;
    NDIS_STATUS             StatusCode;
    ULONG                   Flags;             
    NDIS_HANDLE             DestinationHandle;
    PVOID                   RequestId; 
    PVOID                   StatusBuffer;
    ULONG                   StatusBufferSize;
    GUID                    Guid;               // optional and valid only if StatusCode = NDIS_STATUS_MEDIA_SPECIFIC_INDICATION
    PVOID                   NdisReserved[4];
}NDIS_STATUS_INDICATION, *PNDIS_STATUS_INDICATION;

#define NDIS_SIZEOF_STATUS_INDICATION_REVISION_1      \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_STATUS_INDICATION, NdisReserved)


//
// Generic Timer support
//

#define NDIS_TIMER_CHARACTERISTICS_REVISION_1 1

typedef struct _NDIS_TIMER_CHARACTERISTICS
{
    NDIS_OBJECT_HEADER                  Header;
    ULONG                               AllocationTag;
    PNDIS_TIMER_FUNCTION                TimerFunction;
    PVOID                               FunctionContext;
} NDIS_TIMER_CHARACTERISTICS, *PNDIS_TIMER_CHARACTERISTICS;

#define NDIS_SIZEOF_TIMER_CHARACTERISTICS_REVISION_1      \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_TIMER_CHARACTERISTICS, FunctionContext)

#define NDIS_RESTART_GENERAL_ATTRIBUTES_REVISION_1            1

typedef struct _NDIS_RESTART_GENERAL_ATTRIBUTES
{
    NDIS_OBJECT_HEADER              Header;
    ULONG                           MtuSize;
    ULONG64                         MaxXmitLinkSpeed;
    ULONG64                         MaxRcvLinkSpeed;
    ULONG                           LookaheadSize;
    ULONG                           MacOptions;
    ULONG                           SupportedPacketFilters;
    ULONG                           MaxMulticastListSize;
    PNDIS_RECEIVE_SCALE_CAPABILITIES RecvScaleCapabilities;
    NET_IF_ACCESS_TYPE              AccessType;
    ULONG                           Flags;       
    NET_IF_CONNECTION_TYPE          ConnectionType;
    ULONG                           SupportedStatistics;
    ULONG                           DataBackFillSize;
    ULONG                           ContextBackFillSize;
    PNDIS_OID                       SupportedOidList;
    ULONG                           SupportedOidListLength;
}NDIS_RESTART_GENERAL_ATTRIBUTES, *PNDIS_RESTART_GENERAL_ATTRIBUTES;

#define NDIS_SIZEOF_RESTART_GENERAL_ATTRIBUTES_REVISION_1    \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_RESTART_GENERAL_ATTRIBUTES, SupportedOidListLength)

EXPORT
NDIS_STATUS
NdisAllocateTimerObject(
    IN  NDIS_HANDLE                     NdisHandle,
    IN  PNDIS_TIMER_CHARACTERISTICS     TimerCharacteristics,
    OUT PNDIS_HANDLE                    pTimerObject
    );

EXPORT
BOOLEAN
NdisSetTimerObject(
    IN  NDIS_HANDLE                     TimerObject,
    IN  LARGE_INTEGER                   DueTime,
    IN  LONG                            MillisecondsPeriod OPTIONAL,
    IN  PVOID                           FunctionContext OPTIONAL
    );

EXPORT
BOOLEAN
NdisCancelTimerObject(
    IN  NDIS_HANDLE                     TimerObject
    );

EXPORT
VOID
NdisFreeTimerObject(
    IN  NDIS_HANDLE                     TimerObject
    );

EXPORT
NDIS_STATUS
NdisAllocateCloneOidRequest(
    IN NDIS_HANDLE         SourceHandle,
    IN PNDIS_OID_REQUEST   OidRequest,
    IN UINT                PoolTag,
    OUT PNDIS_OID_REQUEST* ClonedOidRequest
    );

EXPORT    
VOID
NdisFreeCloneOidRequest(
    IN NDIS_HANDLE         SourceHandle,
    IN PNDIS_OID_REQUEST   Request
    );
    
EXPORT   
VOID
NdisGetSystemUpTimeEx(
    OUT PLARGE_INTEGER                  pSystemUpTime
    );

typedef enum _NDIS_PROCESSOR_VENDOR
{
    NdisProcessorVendorUnknown,
    NdisProcessorVendorGenuinIntel,
    NdisProcessorVendorAuthenticAMD
} NDIS_PROCESSOR_VENDOR, *PNDIS_PROCESSOR_VENDOR;

typedef struct _NDIS_PROCESSOR_INFO
{
    ULONG   CpuNumber;
    ULONG   PhysicalPackageId;
    ULONG   CoreId;
    ULONG   HyperThreadID;
}NDIS_PROCESSOR_INFO, *PNDIS_PROCESSOR_INFO;

#define NDIS_SYSTEM_PROCESSOR_INFO_REVISION_1 1

typedef struct _NDIS_SYSTEM_PROCESSOR_INFO
{
    NDIS_OBJECT_HEADER      Header;
    ULONG                   Flags;
    NDIS_PROCESSOR_VENDOR   ProcessorVendor;
    ULONG                   NumPhysicalPackages;
    ULONG                   NumCores;    
    ULONG                   NumCoresPerPhysicalPackage;
    ULONG                   MaxHyperThreadingCpusPerCore;
    ULONG                   RssBaseCpu;
    ULONG                   RssCpuCount;
    PUCHAR                  RssProcessors;
    NDIS_PROCESSOR_INFO     CpuInfo[MAXIMUM_PROCESSORS];    
}NDIS_SYSTEM_PROCESSOR_INFO, *PNDIS_SYSTEM_PROCESSOR_INFO;

#define NDIS_SIZEOF_SYSTEM_PROCESSOR_INFO_REVISION_1    \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_SYSTEM_PROCESSOR_INFO, CpuInfo)

EXPORT
NDIS_STATUS
NdisGetProcessorInformation(
    IN PNDIS_SYSTEM_PROCESSOR_INFO      SystemProcessorInfo
    );

//
// Pause reason used in NDIS_MINIPORT_PAUSE_PARAMETERS, NDIS_FILTER_PAUSE_PARAMETERS
// and NDIS_PROTOCOL_PAUSE_PARAMETERS
//
#define NDIS_PAUSE_NDIS_INTERNAL                     0x00000001
#define NDIS_PAUSE_LOW_POWER                         0x00000002
#define NDIS_PAUSE_BIND_PROTOCOL                     0x00000004
#define NDIS_PAUSE_UNBIND_PROTOCOL                   0x00000008
#define NDIS_PAUSE_ATTACH_FILTER                     0x00000010
#define NDIS_PAUSE_DETACH_FILTER                     0x00000020
#define NDIS_PAUSE_FILTER_RESTART_STACK              0x00000040
#define NDIS_PAUSE_MINIPORT_DEVICE_REMOVE            0x00000080

#endif // NDIS_SUPPORT_NDIS6

#if  (NTDDI_VERSION == NTDDI_LONGHORN)
#if (NDIS_LEGACY_DRIVER || NDIS_SUPPORT_NDIS6)
//
// Flags used in NDIS_FILTER_INTERFACE
//
#define NDIS_FILTER_INTERFACE_IM_FILTER        0x00000001
#define NDIS_FILTER_INTERFACE_LW_FILTER        0x00000002

#define NDIS_FILTER_INTERFACE_REVISION_1  1

typedef struct _NDIS_FILTER_INTERFACE
{
    
    NDIS_OBJECT_HEADER       Header;
    ULONG                    Flags;
    ULONG                    FilterType;
    ULONG                    FilterRunType;
    NET_IFINDEX              IfIndex;
    NET_LUID                 NetLuid;
    NDIS_STRING              FilterClass;
    NDIS_STRING              FilterInstanceName;
} NDIS_FILTER_INTERFACE, *PNDIS_FILTER_INTERFACE;
    
#define NDIS_SIZEOF_FILTER_INTERFACE_REVISION_1      \
    RTL_SIZEOF_THROUGH_FIELD(NDIS_FILTER_INTERFACE, FilterInstanceName)


#define NDIS_ENUM_FILTERS_REVISION_1           1

typedef struct _NDIS_ENUM_FILTERS
{
    NDIS_OBJECT_HEADER              Header;
    ULONG                           Flags;
    ULONG                           NumberOfFilters;
    ULONG                           OffsetFirstFilter;
    NDIS_FILTER_INTERFACE           Filter[1];
} NDIS_ENUM_FILTERS, *PNDIS_ENUM_FILTERS;

#define NDIS_SIZEOF_ENUM_FILTERS_REVISION_1      \
    RTL_SIZEOF_THROUGH_FIELD(NDIS_ENUM_FILTERS, Filter)




EXPORT
NDIS_STATUS
NdisEnumerateFilterModules(
    IN  NDIS_HANDLE         NdisHandle,
    IN  PVOID               InterfaceBuffer,
    IN  ULONG               InterfaceBufferLength,
    IN  OUT PULONG          BytesNeeded,
    IN  OUT PULONG          BytesWritten
    );

//
// The NdisRegisterDeviceEx is used by NDIS 5 and 6 drivers on NTDDI_LONGHORN
// These drivers use NdisRegisterDeviceEx to use the new security features
// not available in NdisRegisterDevice or NdisMRegisterDevice
//
#define NDIS_DEVICE_OBJECT_ATTRIBUTES_REVISION_1    1

typedef struct _NDIS_DEVICE_OBJECT_ATTRIBUTES
{
    NDIS_OBJECT_HEADER          Header;
    PNDIS_STRING                DeviceName;
    PNDIS_STRING                SymbolicName;
    PDRIVER_DISPATCH*           MajorFunctions;
    ULONG                       ExtensionSize;
    PCUNICODE_STRING            DefaultSDDLString;
    LPCGUID                     DeviceClassGuid;
} NDIS_DEVICE_OBJECT_ATTRIBUTES, *PNDIS_DEVICE_OBJECT_ATTRIBUTES;

#define NDIS_SIZEOF_DEVICE_OBJECT_ATTRIBUTES_REVISION_1    \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_DEVICE_OBJECT_ATTRIBUTES, DeviceClassGuid)

EXPORT
NDIS_STATUS
NdisRegisterDeviceEx(
    IN  NDIS_HANDLE                     NdisObjectHandle,
    IN  PNDIS_DEVICE_OBJECT_ATTRIBUTES  DeviceObjectAttributes,
    OUT PDEVICE_OBJECT *                pDeviceObject,
    OUT PNDIS_HANDLE                    NdisDeviceHandle
    );

EXPORT
VOID
NdisDeregisterDeviceEx(
    IN  NDIS_HANDLE                    NdisDeviceHandle
    );

EXPORT
PVOID
NdisGetDeviceReservedExtension(
    IN  PDEVICE_OBJECT                    DeviceObject
    );

//
// This structure is used by NDIS 5.x drivers that want to use 
// NdisRegisterDeviceEx
//
#define NDIS_OBJECT_TYPE_DRIVER_WRAPPER_REVISION_1            1
typedef struct _NDIS_DRIVER_WRAPPER_HANDLE
{
    //
    // Caller must set Header to
    //     Header.Type = NDIS_OBJECT_TYPE_DRIVER_WRAPPER_OBJECT
    //     Header.Revision = NDIS_OBJECT_TYPE_DRIVER_WRAPPER_REVISION_1
    //     Header.Size = sizeof(NDIS_DRIVER_WRAPPER_HANDLE)
    //
    NDIS_OBJECT_HEADER          Header;
    NDIS_HANDLE                 NdisWrapperHandle;

} NDIS_DRIVER_WRAPPER_HANDLE, *PNDIS_DRIVER_WRAPPER_HANDLE;

#define NDIS_SIZEOF_DRIVER_WRAPPER_HANDLE_REVISION_1      \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_DRIVER_WRAPPER_HANDLE, NdisWrapperHandle)

#endif // NDIS_SUPPORT_NDIS6 || NDIS_LEGACY_DRIVER
#endif // NTDDI_VERSION == NTDDI_LONGHORN


#pragma once

#if (NDIS_SUPPORT_NDIS6 || NDIS60)

//
// NET_BUFFER data structures, APIs and macros
//

typedef struct _NET_BUFFER NET_BUFFER, *PNET_BUFFER;
typedef struct _NET_BUFFER_LIST_CONTEXT NET_BUFFER_LIST_CONTEXT, *PNET_BUFFER_LIST_CONTEXT;
typedef struct _NET_BUFFER_LIST NET_BUFFER_LIST, *PNET_BUFFER_LIST;

typedef union _NET_BUFFER_DATA_LENGTH
{
    ULONG   DataLength;
    SIZE_T  stDataLength;
} NET_BUFFER_DATA_LENGTH, *PNET_BUFFER_DATA_LENGTH;
    
    
typedef struct _NET_BUFFER_DATA
{
    PNET_BUFFER Next;
    PMDL        CurrentMdl;
    ULONG       CurrentMdlOffset;
#ifdef __cplusplus
    NET_BUFFER_DATA_LENGTH NbDataLength;
#else
    NET_BUFFER_DATA_LENGTH;
#endif
    PMDL        MdlChain;
    ULONG       DataOffset;
} NET_BUFFER_DATA, *PNET_BUFFER_DATA;

typedef union _NET_BUFFER_HEADER
{
#ifdef __cplusplus
    NET_BUFFER_DATA NetBufferData;
#else
    NET_BUFFER_DATA;
#endif
    SLIST_HEADER    Link;

} NET_BUFFER_HEADER, *PNET_BUFFER_HEADER;

typedef struct _NET_BUFFER
{

#ifdef __cplusplus
    NET_BUFFER_HEADER NetBufferHeader;
#else
    NET_BUFFER_HEADER;
#endif

    USHORT          ChecksumBias;
    USHORT          Reserved;
    NDIS_HANDLE     NdisPoolHandle;
    DECLSPEC_ALIGN(MEMORY_ALLOCATION_ALIGNMENT)PVOID NdisReserved[2];
    DECLSPEC_ALIGN(MEMORY_ALLOCATION_ALIGNMENT)PVOID ProtocolReserved[6];
    DECLSPEC_ALIGN(MEMORY_ALLOCATION_ALIGNMENT)PVOID MiniportReserved[4];
    NDIS_PHYSICAL_ADDRESS   NdisReserved1;
}NET_BUFFER, *PNET_BUFFER;

#pragma warning(push)
#pragma warning(disable:4200)   // nonstandard extension used : zero-sized array in struct/union

typedef struct _NET_BUFFER_LIST_CONTEXT
{
    PNET_BUFFER_LIST_CONTEXT    Next;
    USHORT                      Size;
    USHORT                      Offset;
    DECLSPEC_ALIGN(MEMORY_ALLOCATION_ALIGNMENT)     UCHAR      ContextData[];
} NET_BUFFER_LIST_CONTEXT, *PNET_BUFFER_LIST_CONTEXT;

#pragma warning(pop)

typedef enum _NDIS_NET_BUFFER_LIST_INFO
{
    TcpIpChecksumNetBufferListInfo,
    TcpOffloadBytesTransferred = TcpIpChecksumNetBufferListInfo,
    IPsecOffloadV1NetBufferListInfo,
    TcpLargeSendNetBufferListInfo,
    TcpReceiveNoPush = TcpLargeSendNetBufferListInfo,
    ClassificationHandleNetBufferListInfo,
    Ieee8021QNetBufferListInfo,
    NetBufferListCancelId,
    MediaSpecificInformation,
    NetBufferListFrameType,
    NetBufferListProtocolId = NetBufferListFrameType,
    NetBufferListHashValue,
    NetBufferListHashInfo,
    WfpNetBufferListInfo,
    MaxNetBufferListInfo
} NDIS_NET_BUFFER_LIST_INFO, *PNDIS_NET_BUFFER_LIST_INFO;

typedef struct _NET_BUFFER_LIST_DATA
{
    PNET_BUFFER_LIST    Next;           // Next NetBufferList in the chain
    PNET_BUFFER         FirstNetBuffer; // First NetBuffer on this NetBufferList
} NET_BUFFER_LIST_DATA, *PNET_BUFFER_LIST_DATA;

typedef union _NET_BUFFER_LIST_HEADER
{
#ifdef __cplusplus
    NET_BUFFER_LIST_DATA NetBufferListData;
#else
    NET_BUFFER_LIST_DATA;
#endif
    SLIST_HEADER            Link;           // used in SLIST of free NetBuffers in the block
} NET_BUFFER_LIST_HEADER, *PNET_BUFFER_LIST_HEADER;


typedef struct _NET_BUFFER_LIST
{

#ifdef __cplusplus
    NET_BUFFER_LIST_HEADER      NetBufferListHeader;
#else
    NET_BUFFER_LIST_HEADER;
#endif

    PNET_BUFFER_LIST_CONTEXT    Context;
    PNET_BUFFER_LIST            ParentNetBufferList;
    NDIS_HANDLE                 NdisPoolHandle;
    DECLSPEC_ALIGN(MEMORY_ALLOCATION_ALIGNMENT)PVOID NdisReserved[2];
    DECLSPEC_ALIGN(MEMORY_ALLOCATION_ALIGNMENT)PVOID ProtocolReserved[4];
    DECLSPEC_ALIGN(MEMORY_ALLOCATION_ALIGNMENT)PVOID MiniportReserved[2];
    PVOID                       Scratch;
    NDIS_HANDLE                 SourceHandle;
    ULONG                       NblFlags;   // public flags
    LONG                        ChildRefCount;
    ULONG                       Flags;      // private flags used by NDIs, protocols, miniport, etc.
    NDIS_STATUS                 Status;
    PVOID                       NetBufferListInfo[MaxNetBufferListInfo];
} NET_BUFFER_LIST, *PNET_BUFFER_LIST;




#ifdef __cplusplus
#define NET_BUFFER_NEXT_NB(_NB)                     ((_NB)->NetBufferHeader.NetBufferData.Next)
#define NET_BUFFER_FIRST_MDL(_NB)                   ((_NB)->NetBufferHeader.NetBufferData.MdlChain)
#define NET_BUFFER_DATA_LENGTH(_NB)                 ((_NB)->NetBufferHeader.NetBufferData.NbDataLength.DataLength)
#define NET_BUFFER_DATA_OFFSET(_NB)                 ((_NB)->NetBufferHeader.NetBufferData.DataOffset)
#define NET_BUFFER_CURRENT_MDL(_NB)                 ((_NB)->NetBufferHeader.NetBufferData.CurrentMdl)
#define NET_BUFFER_CURRENT_MDL_OFFSET(_NB)          ((_NB)->NetBufferHeader.NetBufferData.CurrentMdlOffset)
#else
#define NET_BUFFER_NEXT_NB(_NB)                     ((_NB)->Next)
#define NET_BUFFER_FIRST_MDL(_NB)                   ((_NB)->MdlChain)
#define NET_BUFFER_DATA_LENGTH(_NB)                 ((_NB)->DataLength)
#define NET_BUFFER_DATA_OFFSET(_NB)                 ((_NB)->DataOffset)
#define NET_BUFFER_CURRENT_MDL(_NB)                 ((_NB)->CurrentMdl)
#define NET_BUFFER_CURRENT_MDL_OFFSET(_NB)          ((_NB)->CurrentMdlOffset)
#endif

#define NET_BUFFER_PROTOCOL_RESERVED(_NB)           ((_NB)->ProtocolReserved)
#define NET_BUFFER_MINIPORT_RESERVED(_NB)           ((_NB)->MiniportReserved)
#define NET_BUFFER_CHECKSUM_BIAS(_NB)               ((_NB)->ChecksumBias)

#ifdef __cplusplus
#define NET_BUFFER_LIST_NEXT_NBL(_NBL)              ((_NBL)->NetBufferListHeader.NetBufferListData.Next)
#define NET_BUFFER_LIST_FIRST_NB(_NBL)              ((_NBL)->NetBufferListHeader.NetBufferListData.FirstNetBuffer)
#else
#define NET_BUFFER_LIST_NEXT_NBL(_NBL)              ((_NBL)->Next)
#define NET_BUFFER_LIST_FIRST_NB(_NBL)              ((_NBL)->FirstNetBuffer)
#endif

#define NET_BUFFER_LIST_FLAGS(_NBL)                 ((_NBL)->Flags)
#define NET_BUFFER_LIST_PROTOCOL_RESERVED(_NBL)     ((_NBL)->ProtocolReserved)
#define NET_BUFFER_LIST_MINIPORT_RESERVED(_NBL)     ((_NBL)->MiniportReserved)
#define NET_BUFFER_LIST_CONTEXT_DATA_START(_NBL)    ((PUCHAR)(((_NBL)->Context)+1)+(_NBL)->Context->Offset)
#define NET_BUFFER_LIST_CONTEXT_DATA_SIZE(_NBL)     (((_NBL)->Context)->Size)

#define NET_BUFFER_LIST_INFO(_NBL, _Id)             ((_NBL)->NetBufferListInfo[(_Id)])
#define NET_BUFFER_LIST_STATUS(_NBL)                ((_NBL)->Status)


#define NDIS_GET_NET_BUFFER_LIST_CANCEL_ID(_NBL)     (NET_BUFFER_LIST_INFO(_NBL, NetBufferListCancelId))
#define NDIS_SET_NET_BUFFER_LIST_CANCEL_ID(_NBL, _CancelId)            \
    NET_BUFFER_LIST_INFO(_NBL, NetBufferListCancelId) = _CancelId


//
//  Per-NBL information for Ieee8021QNetBufferListInfo.
//
typedef struct _NDIS_NET_BUFFER_LIST_8021Q_INFO
{
    union
    {
        struct
        {
            UINT32      UserPriority:3;         // 802.1p priority
            UINT32      CanonicalFormatId:1;    // always 0
            UINT32      VlanId:12;              // VLAN Identification
            UINT32      Reserved:16;            // set to 0 for ethernet
        }TagHeader;
        
        struct
        {
            UINT32      UserPriority:3;         // 802.1p priority
            UINT32      CanonicalFormatId:1;    // always 0
            UINT32      VlanId:12;              // VLAN Identification
            UINT32      WMMInfo:4;              
            UINT32      Reserved:12;            // set to 0 for wireless lan
            
        }WLanTagHeader;

        PVOID  Value;
    };
} NDIS_NET_BUFFER_LIST_8021Q_INFO, *PNDIS_NET_BUFFER_LIST_8021Q_INFO;

typedef struct _NDIS_NET_BUFFER_LIST_MEDIA_SPECIFIC_INFO
{
    union
    {
        PVOID  MediaSpecificInfo;
        PVOID  NativeWifiSpecificInfo;

        PVOID  Value;
    };
    
} NDIS_NET_BUFFER_LIST_MEDIA_SPECIFIC_INFO, *PNDIS_NET_BUFFER_LIST_MEDIA_SPECIFIC_INFO;

typedef struct _NDIS_NBL_MEDIA_MEDIA_SPECIFIC_INFORMATION NDIS_NBL_MEDIA_SPECIFIC_INFORMATION, *PNDIS_NBL_MEDIA_SPECIFIC_INFORMATION;


struct _NDIS_NBL_MEDIA_MEDIA_SPECIFIC_INFORMATION
{
    PNDIS_NBL_MEDIA_SPECIFIC_INFORMATION NextEntry;
    ULONG                                Tag;
    DECLSPEC_ALIGN(MEMORY_ALLOCATION_ALIGNMENT) UCHAR  Data[1];
}; 

#define NDIS_NBL_ADD_MEDIA_SPECIFIC_INFO(_NBL, _MediaSpecificInfo)                         \
    {                                                                                      \
        PNDIS_NBL_MEDIA_SPECIFIC_INFORMATION HeadEntry = NULL;                             \
        if (NET_BUFFER_LIST_INFO((_NBL), MediaSpecificInformation) != NULL)                \
        {                                                                                  \
            HeadEntry = (PNDIS_NBL_MEDIA_SPECIFIC_INFORMATION)(NET_BUFFER_LIST_INFO((_NBL), MediaSpecificInformation));           \
        }                                                                                  \
        NET_BUFFER_LIST_INFO((_NBL), MediaSpecificInformation) = (_MediaSpecificInfo);     \
        (_MediaSpecificInfo)->NextEntry = HeadEntry;                                       \
    }            

#define NDIS_NBL_REMOVE_MEDIA_SPECIFIC_INFO(_NBL, _MediaSpecificInfo)                      \
    {                                                                                      \
        PNDIS_NBL_MEDIA_SPECIFIC_INFORMATION *HeadEntry;                                   \
        HeadEntry = (PNDIS_NBL_MEDIA_SPECIFIC_INFORMATION *)&(NET_BUFFER_LIST_INFO((_NBL), MediaSpecificInformation));             \
        for (; *HeadEntry != NULL; HeadEntry = &(*HeadEntry)->NextEntry)                   \
        {                                                                                  \
            if ((*HeadEntry)->Tag == (_MediaSpecificInfo)->Tag)                            \
            {                                                                              \
                *HeadEntry = (*HeadEntry)->NextEntry;                                      \
                break;                                                                     \
            }                                                                              \
        }                                                                                  \
    }                                                                                              

#define NDIS_NBL_GET_MEDIA_SPECIFIC_INFO(_NBL, _Tag, _MediaSpecificInfo)                   \
    {                                                                                      \
        PNDIS_NBL_MEDIA_SPECIFIC_INFORMATION HeadEntry;                                    \
        (_MediaSpecificInfo) = NULL;                                                       \
        HeadEntry = (PNDIS_NBL_MEDIA_SPECIFIC_INFORMATION)(NET_BUFFER_LIST_INFO((_NBL), MediaSpecificInformation));                \
        for (; HeadEntry != NULL; HeadEntry = HeadEntry->NextEntry)                        \
        {                                                                                  \
            if (HeadEntry->Tag == (_Tag))                                                  \
            {                                                                              \
                (_MediaSpecificInfo) = HeadEntry;                                          \
                break;                                                                     \
            }                                                                              \
        }                                                                                  \
    }


//
// The following macros are used by miniport driver and protocol driver to set and get
// the hash value, hash type and hash function.
//
#define NET_BUFFER_LIST_SET_HASH_TYPE(_NBL, _HashType)           \
    (NET_BUFFER_LIST_INFO(_NBL, NetBufferListHashInfo)  = \
    UlongToPtr(((PtrToUlong (NET_BUFFER_LIST_INFO(_NBL, NetBufferListHashInfo)) & (~NDIS_HASH_TYPE_MASK)) | ((_HashType) & (NDIS_HASH_TYPE_MASK)))))

#define NET_BUFFER_LIST_SET_HASH_FUNCTION(_NBL, _HashFunction)   \
    (NET_BUFFER_LIST_INFO(_NBL, NetBufferListHashInfo)  = \
    UlongToPtr(((PtrToUlong(NET_BUFFER_LIST_INFO(_NBL, NetBufferListHashInfo)) & (~NDIS_HASH_FUNCTION_MASK)) | ((_HashFunction) & (NDIS_HASH_FUNCTION_MASK)))))

#define NET_BUFFER_LIST_SET_HASH_VALUE(_NBL, _HashValue)         \
    (NET_BUFFER_LIST_INFO(_NBL, NetBufferListHashValue) = UlongToPtr(_HashValue))

#define NET_BUFFER_LIST_GET_HASH_TYPE(_NBL)                      \
    (PtrToUlong(NET_BUFFER_LIST_INFO(_NBL, NetBufferListHashInfo)) & (NDIS_HASH_TYPE_MASK))

#define NET_BUFFER_LIST_GET_HASH_FUNCTION(_NBL)                  \
    (PtrToUlong(NET_BUFFER_LIST_INFO(_NBL, NetBufferListHashInfo)) & (NDIS_HASH_FUNCTION_MASK))

#define NET_BUFFER_LIST_GET_HASH_VALUE(_NBL)                     \
    (PtrToUlong(NET_BUFFER_LIST_INFO(_NBL, NetBufferListHashValue)))

#define NdisSetNetBufferListProtocolId(_NBL,_ProtocolId)   \
    *((PUCHAR)(&NET_BUFFER_LIST_INFO(_NBL, NetBufferListProtocolId))) = _ProtocolId


#define NdisQueryMdl(_Mdl, _VirtualAddress, _Length, _Priority)             \
{                                                                           \
    if (ARGUMENT_PRESENT(_VirtualAddress))                                  \
    {                                                                       \
        *(PVOID *)(_VirtualAddress) = MmGetSystemAddressForMdlSafe(_Mdl, _Priority); \
    }                                                                       \
    *(_Length) = MmGetMdlByteCount(_Mdl);                                   \
}

#define NdisQueryMdlOffset(_Mdl, _Offset, _Length)                          \
{                                                                           \
    *(_Offset) = MmGetMdlByteOffset(_Mdl);                                  \
    *(_Length) = MmGetMdlByteCount(_Mdl);                                   \
}

#define NDIS_MDL_TO_SPAN_PAGES(_Mdl)                                        \
    (MmGetMdlByteCount(_Mdl)==0 ?                                           \
                1 :                                                         \
                (ADDRESS_AND_SIZE_TO_SPAN_PAGES(                            \
                        MmGetMdlVirtualAddress(_Mdl),                       \
                        MmGetMdlByteCount(_Mdl))))

#define NdisGetMdlPhysicalArraySize(_Mdl, _ArraySize)                       \
    (*(_ArraySize) = NDIS_MDL_TO_SPAN_PAGES(_Mdl))


#define NDIS_MDL_LINKAGE(_Mdl) ((_Mdl)->Next)

#define NdisGetNextMdl(_CurrentMdl, _NextMdl)                               \
{                                                                           \
    *(_NextMdl) = (_CurrentMdl)->Next;                                      \
}


UCHAR
NdisGetNetBufferListProtocolId(
    IN PNET_BUFFER_LIST NetBufferList
    );

VOID
NdisAdjustNetBufferCurrentMdl(
    IN  PNET_BUFFER NetBuffer
    );


//
// The flags that can be set at NET_BUFFER_LIST.Flags are defined below. 
//
#define NBL_FLAGS_PROTOCOL_RESERVED                 0xFFF00000
#define NBL_FLAGS_MINIPORT_RESERVED                 0x0000F000
#define NBL_FLAGS_SCRATCH                           0x000F0000
#define NBL_FLAGS_NDIS_RESERVED                     0x00000FFF


#define NBL_TEST_FLAG(_NBL, _F)                     (((_NBL)->Flags & (_F)) != 0)
#define NBL_SET_FLAG(_NBL, _F)                      ((_NBL)->Flags |= (_F))
#define NBL_CLEAR_FLAG(_NBL, _F)                    ((_NBL)->Flags &= ~(_F))

#define NBL_SET_PROTOCOL_RSVD_FLAG(_NBL, _F)        ((_NBL)->Flags |= ((_F) & NBL_FLAGS_PROTOCOL_RESERVED))
#define NBL_CLEAR_PROTOCOL_RSVD_FLAG(_NBL, _F)      ((_NBL)->Flags &= ~((_F) & NBL_FLAGS_PROTOCOL_RESERVED))
#define NBL_TEST_PROTOCOL_RSVD_FLAG(_NBL, _F)       (((_NBL)->Flags & ((_F) & NBL_FLAGS_PROTOCOL_RESERVED)) != 0)


//
// Define some flags for protocols' own use
//
#define NBL_PROT_RSVD_FLAGS                     NBL_FLAGS_PROTOCOL_RESERVED
#define NBL_SET_PROT_RSVD_FLAG(_NBL, _F)        NBL_SET_PROTOCOL_RSVD_FLAG(_NBL,_F)
#define NBL_CLEAR_PROT_RSVD_FLAG(_NBL, _F)      NBL_CLEAR_PROTOCOL_RSVD_FLAG(_NBL, _F)
#define NBL_TEST_PROT_RSVD_FLAG(_NBL, _F)       NBL_TEST_PROTOCOL_RSVD_FLAG(_NBL, _F)


//
// Flags used in NBL->NblFlags
// 0x01 to 0xFF are reserved for use by NDIS
//
#define NDIS_NBL_FLAGS_SEND_READ_ONLY                   0x00000001
#define NDIS_NBL_FLAGS_RECV_READ_ONLY                   0x00000002
#define NDIS_NBL_FLAGS_NDIS_RESERVED1                   0x00000100  
#define NDIS_NBL_FLAGS_NDIS_RESERVED2                   0x00000200  
#define NDIS_NBL_FLAGS_NDIS_RESERVED3                   0x00000400  
#define NDIS_NBL_FLAGS_NDIS_RESERVED4                   0x00000800  
#define NDIS_NBL_FLAGS_NDIS_RESERVED5                   0x00001000  
#define NDIS_NBL_FLAGS_NDIS_RESERVED6                   0x00002000  
#define NDIS_NBL_FLAGS_NDIS_RESERVED7                   0x00004000  
#define NDIS_NBL_FLAGS_IS_LOOPBACK_PACKET               0x00008000  // The NBL is loopback NBL.

#define NdisTestNblFlag(_NBL, _F)                   (((_NBL)->NblFlags & (_F)) != 0)
#define NdisTestNblFlags(_NBL, _F)                  (((_NBL)->NblFlags & (_F)) == (_F))
#define NdisSetNblFlag(_NBL, _F)                    ((_NBL)->NblFlags |= (_F))
#define NdisClearNblFlag(_NBL, _F)                  ((_NBL)->NblFlags &= ~(_F))



#define NET_BUFFER_LIST_POOL_PARAMETERS_REVISION_1 1

typedef struct _NET_BUFFER_LIST_POOL_PARAMETERS
{
    //
    // Set ObjectHeader.Type to NDIS_OBJECT_TYPE_DEFAULT   
    //
    NDIS_OBJECT_HEADER      Header;
    UCHAR                   ProtocolId;
    BOOLEAN                 fAllocateNetBuffer;
    USHORT                  ContextSize;
    ULONG                   PoolTag;
    ULONG                   DataSize;
}NET_BUFFER_LIST_POOL_PARAMETERS, *PNET_BUFFER_LIST_POOL_PARAMETERS;

#define NDIS_SIZEOF_NET_BUFFER_LIST_POOL_PARAMETERS_REVISION_1   \
        RTL_SIZEOF_THROUGH_FIELD(NET_BUFFER_LIST_POOL_PARAMETERS, DataSize)

#define NET_BUFFER_POOL_PARAMETERS_REVISION_1 1

typedef struct _NET_BUFFER_POOL_PARAMETERS 
{
    //
    // Set ObjectHeader.Type to NDIS_OBJECT_TYPE_DEFAULT
    //
    NDIS_OBJECT_HEADER      Header;
    ULONG                   PoolTag;
    ULONG                   DataSize;
}NET_BUFFER_POOL_PARAMETERS,*PNET_BUFFER_POOL_PARAMETERS; 

#define NDIS_SIZEOF_NET_BUFFER_POOL_PARAMETERS_REVISION_1   \
        RTL_SIZEOF_THROUGH_FIELD(NET_BUFFER_POOL_PARAMETERS, DataSize);

//
// Prototypes of the MDL allocation and free callback.
//

typedef
PMDL
(*NET_BUFFER_ALLOCATE_MDL_HANDLER)(
    IN OUT PULONG               BufferSize
    );

typedef
VOID
(*NET_BUFFER_FREE_MDL_HANDLER)(
    IN  PMDL                    Mdl
    );

EXPORT
NDIS_HANDLE
NdisAllocateNetBufferPool(
    IN NDIS_HANDLE                  NdisHandle,
    IN PNET_BUFFER_POOL_PARAMETERS  Parameters
    );

EXPORT
VOID
NdisFreeNetBufferPool(
    IN  NDIS_HANDLE             PoolHandle
    );

EXPORT
PNET_BUFFER
NdisAllocateNetBuffer(
    IN  NDIS_HANDLE             PoolHandle,
    IN  PMDL                    MdlChain,
    IN  ULONG                   DataOffset,
    IN  SIZE_T                  DataLength
    );

EXPORT
VOID
NdisFreeNetBuffer(
    IN  PNET_BUFFER             NetBuffer
    );


EXPORT
PNET_BUFFER
NdisAllocateNetBufferMdlAndData(
    IN  NDIS_HANDLE              PoolHandle
    );
    
    
EXPORT
NDIS_HANDLE
NdisAllocateNetBufferListPool(
    IN  NDIS_HANDLE                         NdisHandle,
    IN  PNET_BUFFER_LIST_POOL_PARAMETERS    Parameters                    
    );

EXPORT
VOID
NdisFreeNetBufferListPool(
    IN  NDIS_HANDLE             PoolHandle
    );

EXPORT
PNET_BUFFER_LIST
NdisAllocateNetBufferList(
    IN  NDIS_HANDLE             PoolHandle,
    IN  USHORT                  ContextSize,
    IN  USHORT                  ContextBackFill
    );


EXPORT
VOID
NdisFreeNetBufferList(
    IN  PNET_BUFFER_LIST        NetBufferList
    );

EXPORT
NDIS_STATUS
NdisRetreatNetBufferDataStart(
    IN  PNET_BUFFER                      NetBuffer,
    IN  ULONG                            DataOffsetDelta,
    IN  ULONG                            DataBackFill OPTIONAL,
    IN  NET_BUFFER_ALLOCATE_MDL_HANDLER  AllocateMdlHandler OPTIONAL
    );

EXPORT
VOID
NdisAdvanceNetBufferDataStart(
    IN  PNET_BUFFER                  NetBuffer,
    IN  ULONG                        DataOffsetDelta,
    IN  BOOLEAN                      FreeMdl,
    IN  NET_BUFFER_FREE_MDL_HANDLER  FreeMdlHandler OPTIONAL
    );

EXPORT
NDIS_STATUS
NdisRetreatNetBufferListDataStart(
    IN  PNET_BUFFER_LIST                 NetBufferList,
    IN  ULONG                            DataOffsetDelta,
    IN  ULONG                            DataBackFill,
    IN  NET_BUFFER_ALLOCATE_MDL_HANDLER  AllocateMdlHandler OPTIONAL,
    IN  NET_BUFFER_FREE_MDL_HANDLER      FreeMdlHandler OPTIONAL
    );

EXPORT
VOID
NdisAdvanceNetBufferListDataStart(
    IN  PNET_BUFFER_LIST              NetBufferList,
    IN  ULONG                         DataOffsetDelta,
    IN  BOOLEAN                       FreeMdl,
    IN  NET_BUFFER_FREE_MDL_HANDLER   FreeMdlMdlHandler OPTIONAL
    );

EXPORT
NDIS_STATUS
NdisAllocateNetBufferListContext(
    IN  PNET_BUFFER_LIST        NetBufferList,
    IN  USHORT                  ContextSize,
    IN  USHORT                  ContextBackFill,
    IN  ULONG                   PoolTag
    );

EXPORT
VOID
NdisFreeNetBufferListContext(
    IN  PNET_BUFFER_LIST        NetBufferList,
    IN  USHORT                  ContextSize
    );

EXPORT
PNET_BUFFER_LIST
NdisAllocateCloneNetBufferList(
    IN PNET_BUFFER_LIST         OriginalNetBufferList,
    IN NDIS_HANDLE              NetBufferListPoolHandle OPTIONAL, 
    IN NDIS_HANDLE              NetBufferPoolHandle OPTIONAL, 
    IN ULONG                    AllocateCloneFlags
    );

EXPORT
VOID
NdisFreeCloneNetBufferList(
    IN PNET_BUFFER_LIST         CloneNetBufferList,
    IN ULONG                    FreeCloneFlags
    );

EXPORT
PNET_BUFFER_LIST
NdisAllocateFragmentNetBufferList(
    IN  PNET_BUFFER_LIST        OriginalNetBufferList,
    IN  NDIS_HANDLE             NetBufferListPool OPTIONAL,
    IN  NDIS_HANDLE             NetBufferPool OPTIONAL,
    IN  ULONG                   StartOffset,
    IN  ULONG                   MaximumLength,
    IN  ULONG                   DataOffsetDelta,
    IN  ULONG                   DataBackFill,
    IN  ULONG                   AllocateFragmentFlags
    );

EXPORT
VOID
NdisFreeFragmentNetBufferList(
    IN  PNET_BUFFER_LIST        FragmentNetBufferList,
    IN  ULONG                   DataOffsetDelta,
    IN  ULONG                   FreeFragmentFlags
    );

EXPORT
PNET_BUFFER_LIST
NdisAllocateReassembledNetBufferList(
    IN  PNET_BUFFER_LIST        FagmentNetBufferList,
    IN  NDIS_HANDLE             NetBufferAndNetBufferListPoolHandle OPTIONAL,
    IN  ULONG                   StartOffset,
    IN  ULONG                   DataOffsetDelta,
    IN  ULONG                   DataBackFill,
    IN  ULONG                   AllocateReassembleFlags
    );

EXPORT
VOID
NdisFreeReassembledNetBufferList(
    IN  PNET_BUFFER_LIST        ReassembledNetBufferList,
    IN  ULONG                   DataOffsetDelta,
    IN  ULONG                   FreeReassembleFlags
    );

EXPORT
PNET_BUFFER_LIST
NdisAllocateNetBufferAndNetBufferList(
    IN  NDIS_HANDLE             PoolHandle,
    IN  USHORT                  ContextSize,
    IN  USHORT                  ContextBackFill,
    IN  PMDL                    MdlChain,
    IN  ULONG                   DataOffset,
    IN  SIZE_T                  DataLength
    );

EXPORT
PVOID
NdisGetDataBuffer(
    IN PNET_BUFFER  NetBuffer,
    IN ULONG        BytesNeeded,
    IN PVOID        Storage,
    IN UINT         AlignMultiple,
    IN UINT         AlignOffset
    );


EXPORT
NDIS_HANDLE
NdisGetPoolFromNetBufferList(
    IN PNET_BUFFER_LIST     NetBufferList
    );

EXPORT
NDIS_HANDLE
NdisGetPoolFromNetBuffer(
    IN PNET_BUFFER          NetBuffer
    );

EXPORT
NDIS_STATUS
NdisCopyFromNetBufferToNetBuffer(
    IN PNET_BUFFER         Destination,
    IN ULONG               DestinationOffset,
    IN ULONG               BytesToCopy,
    IN PNET_BUFFER         Source,
    IN ULONG               SourceOffset,
    OUT PULONG             BytesCopied
    );

EXPORT
VOID
NdisCopySendNetBufferListInfo(
    IN PNET_BUFFER_LIST DestNetBufferList,
    IN PNET_BUFFER_LIST SrcNetBufferList
    );

EXPORT
VOID
NdisCopyReceiveNetBufferListInfo(
    IN PNET_BUFFER_LIST DestNetBufferList,
    IN PNET_BUFFER_LIST SrcNetBufferList
    );
    
EXPORT
ULONG
NdisQueryNetBufferPhysicalCount(
    IN  PNET_BUFFER             NetBuffer
    );

EXPORT
PMDL
NdisAllocateMdl(
    IN  NDIS_HANDLE             NdisHandle,
    IN  PVOID                   VirtualAddress,
    IN  UINT                    Length
    );

EXPORT
VOID
NdisFreeMdl(
    IN PMDL                     Mdl
    );

//
// The following flags are used in the Send APIs such as Ndis(F)SendNetBufferLists
// and MINIPORT_SEND_NET_BUFFER_LISTS_HANDLER
//
#define NDIS_SEND_FLAGS_DISPATCH_LEVEL                  0x00000001
#define NDIS_SEND_FLAGS_CHECK_FOR_LOOPBACK              0x00000002

#define NDIS_TEST_SEND_FLAG(_Flags, _Fl)            (((_Flags) & (_Fl)) == (_Fl))
#define NDIS_SET_SEND_FLAG(_Flags, _Fl)             ((_Flags) |= (_Fl))

#define NDIS_TEST_SEND_AT_DISPATCH_LEVEL(_Flags)             \
        NDIS_TEST_SEND_FLAG((_Flags), NDIS_SEND_FLAGS_DISPATCH_LEVEL)

//
// The following flags are used in the SendComplete APIs such as 
// NdisMSendNetBufferListsComplete
//
#define NDIS_SEND_COMPLETE_FLAGS_DISPATCH_LEVEL          0x00000001


#define NDIS_TEST_SEND_COMPLETE_FLAG(_Flags, _Fl)   (((_Flags) & (_Fl)) == (_Fl))
#define NDIS_SET_SEND_COMPLETE_FLAG(_Flags, _Fl)    ((_Flags) |= (_Fl))

#define NDIS_TEST_SEND_COMPLETE_AT_DISPATCH_LEVEL(_Flags)      \
        NDIS_TEST_SEND_COMPLETE_FLAG((_Flags), NDIS_SEND_COMPLETE_FLAGS_DISPATCH_LEVEL)

//
// The following flags are used in the IndicateReceive APIs such 
// as NdisMIndicateReceiveNetBufferLists and RECEIVE_NET_BUFFER_LISTS_HANDLER
//
#define NDIS_RECEIVE_FLAGS_DISPATCH_LEVEL               0x00000001
#define NDIS_RECEIVE_FLAGS_RESOURCES                    0x00000002
#define NDIS_RECEIVE_FLAGS_SINGLE_ETHER_TYPE            0x00000100
#define NDIS_RECEIVE_FLAGS_SINGLE_VLAN                  0x00000200
#define NDIS_RECEIVE_FLAGS_PERFECT_FILTERED             0x00000400

#define NDIS_TEST_RECEIVE_FLAG(_Flags, _Fl)         (((_Flags) & (_Fl)) == (_Fl))
#define NDIS_SET_RECEIVE_FLAG(_Flags, _Fl)          ((_Flags) |= (_Fl))

#define NDIS_TEST_RECEIVE_AT_DISPATCH_LEVEL(_Flags)      \
        NDIS_TEST_RECEIVE_FLAG((_Flags), NDIS_RECEIVE_FLAGS_DISPATCH_LEVEL)
        
#define NDIS_TEST_RECEIVE_CANNOT_PEND(_Flags)         \
        NDIS_TEST_RECEIVE_FLAG((_Flags), NDIS_RECEIVE_FLAGS_RESOURCES)

#define NDIS_TEST_RECEIVE_CAN_PEND(_Flags)         \
            (((_Flags) & NDIS_RECEIVE_FLAGS_RESOURCES) == 0)
//
// The following flags are used in the ReturnNetBufferLists APIs such 
// as NdisReturnNetBufferLists and MINIPORT_RETURN_NET_BUFFER_LISTS
//
#define NDIS_RETURN_FLAGS_DISPATCH_LEVEL               0x00000001

#define NDIS_TEST_RETURN_FLAG(_Flags, _Fl)          (((_Flags) & (_Fl)) == (_Fl))
#define NDIS_SET_RETURN_FLAG(_Flags, _Fl)           ((_Flags) |= (_Fl))

#define NDIS_TEST_RETURN_AT_DISPATCH_LEVEL(_Flags)      \
        NDIS_TEST_RETURN_FLAG((_Flags),NDIS_RETURN_FLAGS_DISPATCH_LEVEL)                


//
// The following flags are used in the NdisAllocateCloneNetBufferList 
// and NdisFreeCloneNetBufferList
//

#define NDIS_CLONE_FLAGS_RESERVED                     0x00000001
#define NDIS_CLONE_FLAGS_USE_ORIGINAL_MDLS            0x00000002

#define NDIS_TEST_CLONE_FLAG(_Flags, _Fl)          (((_Flags) & (_Fl)) == (_Fl))
#define NDIS_SET_CLONE_FLAG(_Flags, _Fl)           ((_Flags) |= (_Fl))
#define NDIS_CLEAR_CLONE_FLAG(_Flags, _Fl)         ((_Flags) &= ~(_Fl))


#endif // (NDIS_SUPPORT_NDIS6 || NDIS60)


#pragma once

#define NDIS_TASK_OFFLOAD_VERSION 1

#define MAX_HASHES          4
#define TRUNCATED_HASH_LEN  12

#define CRYPTO_SUCCESS                      0
#define CRYPTO_GENERIC_ERROR                1
#define CRYPTO_TRANSPORT_AH_AUTH_FAILED     2
#define CRYPTO_TRANSPORT_ESP_AUTH_FAILED    3
#define CRYPTO_TUNNEL_AH_AUTH_FAILED        4
#define CRYPTO_TUNNEL_ESP_AUTH_FAILED       5
#define CRYPTO_INVALID_PACKET_SYNTAX        6
#define CRYPTO_INVALID_PROTOCOL             7

#if NDIS_LEGACY_DRIVER
typedef struct _NDIS_IPSEC_PACKET_INFO
{
    union
    {
        struct
        {
            NDIS_HANDLE OffloadHandle;
            NDIS_HANDLE NextOffloadHandle;

        } Transmit;

        struct
        {
            ULONG   SA_DELETE_REQ:1;
            ULONG   CRYPTO_DONE:1;
            ULONG   NEXT_CRYPTO_DONE:1;
            ULONG   CryptoStatus;
        } Receive;
    };
} NDIS_IPSEC_PACKET_INFO, *PNDIS_IPSEC_PACKET_INFO;
#endif // NDIS_LEGACY_DRIVER

#if (NDIS_SUPPORT_NDIS6 || NDIS60)
typedef struct _NDIS_IPSEC_OFFLOAD_V1_NET_BUFFER_LIST_INFO
{
    union
    {
        struct
        {
            NDIS_HANDLE OffloadHandle;

        } Transmit;

        struct
        {
            USHORT   SaDeleteReq:1;
            USHORT   CryptoDone:1;
            USHORT   NextCryptoDone:1;
            USHORT   Pad:13;
            USHORT   CryptoStatus;
        } Receive;
    };
} NDIS_IPSEC_OFFLOAD_V1_NET_BUFFER_LIST_INFO, *PNDIS_IPSEC_OFFLOAD_V1_NET_BUFFER_LIST_INFO;
#endif // (NDIS_SUPPORT_NDIS6 || NDIS60)

#if NDIS_LEGACY_DRIVER
//
//  The following defines are used in the Task field above to define
//  the type of task offloading necessary.
//
typedef enum _NDIS_TASK
{
    TcpIpChecksumNdisTask,
    IpSecNdisTask,
    TcpLargeSendNdisTask,
    MaxNdisTask
} NDIS_TASK, *PNDIS_TASK;


typedef enum _NDIS_ENCAPSULATION
{
    UNSPECIFIED_Encapsulation,
    NULL_Encapsulation,
    IEEE_802_3_Encapsulation,
    IEEE_802_5_Encapsulation,
    LLC_SNAP_ROUTED_Encapsulation,
    LLC_SNAP_BRIDGED_Encapsulation

} NDIS_ENCAPSULATION;

//
// Encapsulation header format
//
typedef struct _NDIS_ENCAPSULATION_FORMAT
{
    NDIS_ENCAPSULATION  Encapsulation;              // Encapsulation type
    struct
    {
        ULONG   FixedHeaderSize:1;
        ULONG   Reserved:31;
    } Flags;

    ULONG    EncapsulationHeaderSize;               // Encapsulation header size

} NDIS_ENCAPSULATION_FORMAT,*PNDIS_ENCAPSULATION_FORMAT;


//
// OFFLOAD header structure for OID_TCP_TASK_OFFLOAD
//
typedef struct _NDIS_TASK_OFFLOAD_HEADER
{
    ULONG       Version;                            // set to NDIS_TASK_OFFLOAD_VERSION
    ULONG       Size;                               // Size of this structure
    ULONG       Reserved;                           // Reserved for future use
    ULONG       OffsetFirstTask;                    // Offset to the first
    NDIS_ENCAPSULATION_FORMAT  EncapsulationFormat; // Encapsulation information.
                                                    // NDIS_TASK_OFFLOAD structure(s)

} NDIS_TASK_OFFLOAD_HEADER, *PNDIS_TASK_OFFLOAD_HEADER;


//
//  Task offload Structure, which follows the above header in ndis query
//
typedef struct _NDIS_TASK_OFFLOAD
{
    ULONG       Version;                            //  NDIS_TASK_OFFLOAD_VERSION
    ULONG       Size;                               //  Size of this structure. Used for version checking.
    NDIS_TASK   Task;                               //  Task.
    ULONG       OffsetNextTask;                     //  Offset to the next NDIS_TASK_OFFLOAD
    ULONG       TaskBufferLength;                   //  Length of the task offload information.
    UCHAR       TaskBuffer[1];                      //  The task offload information.
} NDIS_TASK_OFFLOAD, *PNDIS_TASK_OFFLOAD;

//
//  Offload structure for NDIS_TASK_TCP_IP_CHECKSUM
//
typedef struct _NDIS_TASK_TCP_IP_CHECKSUM
{
    struct
    {
        ULONG       IpOptionsSupported:1;
        ULONG       TcpOptionsSupported:1;
        ULONG       TcpChecksum:1;
        ULONG       UdpChecksum:1;
        ULONG       IpChecksum:1;
    } V4Transmit;

    struct
    {
        ULONG       IpOptionsSupported:1;
        ULONG       TcpOptionsSupported:1;
        ULONG       TcpChecksum:1;
        ULONG       UdpChecksum:1;
        ULONG       IpChecksum:1;
    } V4Receive;


    struct
    {
        ULONG       IpOptionsSupported:1;           // This field implies IpExtensionHeaders support
        ULONG       TcpOptionsSupported:1;
        ULONG       TcpChecksum:1;
        ULONG       UdpChecksum:1;

    } V6Transmit;

    struct
    {
        ULONG       IpOptionsSupported:1;           // This field implies IpExtensionHeaders support
        ULONG       TcpOptionsSupported:1;
        ULONG       TcpChecksum:1;
        ULONG       UdpChecksum:1;

    } V6Receive;


} NDIS_TASK_TCP_IP_CHECKSUM, *PNDIS_TASK_TCP_IP_CHECKSUM;

//
//  Off-load structure for NDIS_TASK_TCP_LARGE_SEND
//
#define NDIS_TASK_TCP_LARGE_SEND_V0         0

typedef struct _NDIS_TASK_TCP_LARGE_SEND
{
    ULONG     Version;                      // set to NDIS_TASK_TCP_LARGE_SEND_V0
    ULONG     MaxOffLoadSize;
    ULONG     MinSegmentCount;
    BOOLEAN   TcpOptions;
    BOOLEAN   IpOptions;

} NDIS_TASK_TCP_LARGE_SEND, *PNDIS_TASK_TCP_LARGE_SEND;


typedef struct _NDIS_TASK_IPSEC
{
    struct
    {
        ULONG   AH_ESP_COMBINED;
        ULONG   TRANSPORT_TUNNEL_COMBINED;
        ULONG   V4_OPTIONS;
        ULONG   RESERVED;
    } Supported;

    struct
    {
        ULONG   MD5:1;
        ULONG   SHA_1:1;
        ULONG   Transport:1;
        ULONG   Tunnel:1;
        ULONG   Send:1;
        ULONG   Receive:1;
    } V4AH;

    struct
    {
        ULONG   DES:1;
        ULONG   RESERVED:1;
        ULONG   TRIPLE_DES:1;
        ULONG   NULL_ESP:1;
        ULONG   Transport:1;
        ULONG   Tunnel:1;
        ULONG   Send:1;
        ULONG   Receive:1;
    } V4ESP;

} NDIS_TASK_IPSEC, *PNDIS_TASK_IPSEC;
#endif // NDIS_LEGACY_DRIVER

//
// flags used in NDIS_TASK_IPSEC->Supprted.RESERVED and
// NDIS_IPSEC_OFFLOAD_V1->Supported.Flags
// 
#define IPSEC_TPT_UDPESP_ENCAPTYPE_IKE                 0x00000001
#define IPSEC_TUN_UDPESP_ENCAPTYPE_IKE                 0x00000002
#define IPSEC_TPTOVERTUN_UDPESP_ENCAPTYPE_IKE          0x00000004
#define IPSEC_TPT_UDPESP_OVER_PURE_TUN_ENCAPTYPE_IKE   0x00000008
#define IPSEC_TPT_UDPESP_ENCAPTYPE_OTHER               0x00000010
#define IPSEC_TUN_UDPESP_ENCAPTYPE_OTHER               0x00000020
#define IPSEC_TPTOVERTUN_UDPESP_ENCAPTYPE_OTHER        0x00000040
#define IPSEC_TPT_UDPESP_OVER_PURE_TUN_ENCAPTYPE_OTHER 0x00000080


#if (NDIS_SUPPORT_NDIS6 || NDIS60)

//
// The definitions for the NDIS_TCP_LARGE_SEND_OFFLOAD_NET_BUFFER_LIST_INFO.Transmit.Type
// A miniport will use this definition to identify what offload type to use
// on the NetBufferList 
//
#define NDIS_TCP_LARGE_SEND_OFFLOAD_V1_TYPE     0
#define NDIS_TCP_LARGE_SEND_OFFLOAD_V2_TYPE     1

//
// IP protocol version encoded in IPVersion field of
// NDIS_TCP_LARGE_SEND_OFFLOAD_NET_BUFFER_LIST_INFO->LsoV2Transmit.IPVersion
//
#define NDIS_TCP_LARGE_SEND_OFFLOAD_IPv4     0
#define NDIS_TCP_LARGE_SEND_OFFLOAD_IPv6     1

//
// The maximum length of the headers (MAC+IP+IP option or extension headers+TCP+TCP options)
// when stack does large send offload. If header is bigger than this value, it will not do
// LSO
// 
#define NDIS_LARGE_SEND_OFFLOAD_MAX_HEADER_LENGTH   128

//
// This structure is used in the OOB TcpLargeSendNetBufferListInfo.
//
typedef struct _NDIS_TCP_LARGE_SEND_OFFLOAD_NET_BUFFER_LIST_INFO
{
    union
    {
        struct
        {   
            ULONG    Unused:30;
            ULONG    Type:1;
            ULONG    Reserved2:1;
        } Transmit;

        struct
        {   
            ULONG    MSS:20;
            ULONG    TcpHeaderOffset:10;
            ULONG    Type:1;
            ULONG    Reserved2:1;
        } LsoV1Transmit;

        struct
        {
            ULONG    TcpPayload:30;
            ULONG    Type:1;
            ULONG    Reserved2:1;
        } LsoV1TransmitComplete;

        struct
        {
            ULONG    MSS:20;
            ULONG    TcpHeaderOffset:10;
            ULONG    Type:1;
            ULONG    IPVersion:1;
        } LsoV2Transmit;

        struct
        {
            ULONG    Reserved:30;
            ULONG    Type:1;
            ULONG    Reserved2:1;
        } LsoV2TransmitComplete;

        PVOID Value;
    };
} NDIS_TCP_LARGE_SEND_OFFLOAD_NET_BUFFER_LIST_INFO,*PNDIS_TCP_LARGE_SEND_OFFLOAD_NET_BUFFER_LIST_INFO;

//
//  Per-NetBufferList information for TcpIpChecksumNetBufferListInfo.
//
typedef struct _NDIS_TCP_IP_CHECKSUM_NET_BUFFER_LIST_INFO
{
    union
    {
        struct
        {
            ULONG   IsIPv4:1;
            ULONG   IsIPv6:1;
            ULONG   TcpChecksum:1;
            ULONG   UdpChecksum:1;
            ULONG   IpHeaderChecksum:1;
            ULONG   Reserved:11;
            ULONG   TcpHeaderOffset:10;
        } Transmit;

        struct
        {
            ULONG   TcpChecksumFailed:1;
            ULONG   UdpChecksumFailed:1;
            ULONG   IpChecksumFailed:1;
            ULONG   TcpChecksumSucceeded:1;
            ULONG   UdpChecksumSucceeded:1;
            ULONG   IpChecksumSucceeded:1;
            ULONG   Loopback:1;
        } Receive;

        PVOID   Value;
    };
} NDIS_TCP_IP_CHECKSUM_NET_BUFFER_LIST_INFO, *PNDIS_TCP_IP_CHECKSUM_NET_BUFFER_LIST_INFO;


//
// used in NDIS_STATUS_OFFLOAD_ENCASPULATION_CHANGE status indication and
// and OID_OFFLOAD_ENCAPSULATION OID request
//
#define NDIS_OFFLOAD_ENCAPSULATION_REVISION_1          1

typedef struct _NDIS_OFFLOAD_ENCAPSULATION
{
    //
    // Header.Type = NDIS_OBJECT_TYPE_OFFLOAD_ENCAPSULATION;
    // Header.Size = sizeof(NDIS_OFFLOAD_ENCAPSULATION);
    // Header.Revision  = NDIS_OFFLOAD_ENCAPSULATION_REVISION_1;
    //
    
    NDIS_OBJECT_HEADER                     Header;

    struct
    {
        //
        // A Protocol sets Enable to NDIS_OFFLOAD_SET_ON if it is enabling IPv4 LSO,
        // or XSum offloads
        // otherwise it is set to NDIS_OFFLOAD_SET_NO_CHANGE
        //
        ULONG                               Enabled;

        //
        // If Enabled is TRUE, a Protocol must set this to 
        // either NDIS_ENCAPSULATION_IEEE_802_3 
        // or NDIS_ENCAPSULATION_IEEE_LLC_SNAP_ROUTED         
        //
        ULONG                               EncapsulationType;

        //
        // If Enabled is TRUE, a protocol must set this field to the 
        // HeaderSize it uses
        //
        ULONG                               HeaderSize;
    } IPv4;


    struct
    {
        //
        // A Protocol sets Enable to NDIS_OFFLOAD_SET_ON if it is enabling IPv6 LSO,
        // or XSum offloads
        // otherwise it is set to NDIS_OFFLOAD_SET_NO_CHANGE
        //
        ULONG                               Enabled;
        //
        // If Enabled is TRUE, a Protocol must set this to 
        // either NDIS_ENCAPSULATION_IEEE_802_3 
        // or NDIS_ENCAPSULATION_IEEE_LLC_SNAP_ROUTED         
        //
        ULONG                               EncapsulationType;

        //
        // If Enabled is TRUE, a protocol must set this field to the 
        // HeaderSize it uses
        //
        ULONG                               HeaderSize;
    } IPv6;

}NDIS_OFFLOAD_ENCAPSULATION, *PNDIS_OFFLOAD_ENCAPSULATION;

#define NDIS_SIZEOF_OFFLOAD_ENCAPSULATION_REVISION_1 RTL_SIZEOF_THROUGH_FIELD(NDIS_OFFLOAD_ENCAPSULATION, IPv6.HeaderSize)

#endif // (NDIS_SUPPORT_NDIS6 || NDIS60)



#pragma once

#if NDIS_LEGACY_PROTOCOL

//
// Function types for NDIS_PROTOCOL_CHARACTERISTICS
//
 
typedef
VOID
(*OPEN_ADAPTER_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  NDIS_STATUS             Status,
    IN  NDIS_STATUS             OpenErrorStatus
    );

typedef
VOID
(*CLOSE_ADAPTER_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  NDIS_STATUS             Status
    );

typedef
VOID
(*RESET_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  NDIS_STATUS             Status
    );
 
typedef
VOID
(*REQUEST_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  PNDIS_REQUEST           NdisRequest,
    IN  NDIS_STATUS             Status
    );

typedef
VOID
(*STATUS_HANDLER)(
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  NDIS_STATUS             GeneralStatus,
    IN  PVOID                   StatusBuffer,
    IN  UINT                    StatusBufferSize
    );
 
typedef
VOID
(*STATUS_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             ProtocolBindingContext
    );

typedef
VOID
(*SEND_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  PNDIS_PACKET            Packet,
    IN  NDIS_STATUS             Status
    );

typedef
VOID
(*WAN_SEND_COMPLETE_HANDLER) (
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  PNDIS_WAN_PACKET        Packet,
    IN  NDIS_STATUS             Status
    );

typedef
VOID
(*TRANSFER_DATA_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  PNDIS_PACKET            Packet,
    IN  NDIS_STATUS             Status,
    IN  UINT                    BytesTransferred
    );

typedef
VOID
(*WAN_TRANSFER_DATA_COMPLETE_HANDLER)(
    VOID
    );

typedef
NDIS_STATUS
(*RECEIVE_HANDLER)(
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  NDIS_HANDLE             MacReceiveContext,
    IN  PVOID                   HeaderBuffer,
    IN  UINT                    HeaderBufferSize,
    IN  PVOID                   LookAheadBuffer,
    IN  UINT                    LookaheadBufferSize,
    IN  UINT                    PacketSize
    );

typedef
NDIS_STATUS
(*WAN_RECEIVE_HANDLER)(
    IN  NDIS_HANDLE             NdisLinkHandle,
    IN  PUCHAR                  Packet,
    IN  ULONG                   PacketSize
    );

typedef
VOID
(*RECEIVE_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             ProtocolBindingContext
    );

//
// Function types extensions for NDIS 4.0 Protocols
//
typedef
INT
(*RECEIVE_PACKET_HANDLER)(
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  PNDIS_PACKET            Packet
    );

typedef
VOID
(*BIND_HANDLER)(
    OUT PNDIS_STATUS            Status,
    IN  NDIS_HANDLE             BindContext,
    IN  PNDIS_STRING            DeviceName,
    IN  PVOID                   SystemSpecific1,
    IN  PVOID                   SystemSpecific2
    );

typedef
VOID
(*UNBIND_HANDLER)(
    OUT PNDIS_STATUS            Status,
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  NDIS_HANDLE             UnbindContext
    );
 
typedef
NDIS_STATUS
(*PNP_EVENT_HANDLER)(
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  PNET_PNP_EVENT          NetPnPEvent
    );


typedef
VOID
(*UNLOAD_PROTOCOL_HANDLER)(
    VOID
    );

//
// Protocol characteristics for NDIS 4.0 protocols
//
typedef struct _NDIS40_PROTOCOL_CHARACTERISTICS
{
    UCHAR                           MajorNdisVersion;
    UCHAR                           MinorNdisVersion;
    USHORT                          Filler;
    union
    {
        UINT                        Reserved;
        UINT                        Flags;
    };
    OPEN_ADAPTER_COMPLETE_HANDLER   OpenAdapterCompleteHandler;
    CLOSE_ADAPTER_COMPLETE_HANDLER  CloseAdapterCompleteHandler;
    union
    {
        SEND_COMPLETE_HANDLER       SendCompleteHandler;
        WAN_SEND_COMPLETE_HANDLER   WanSendCompleteHandler;
    };
    union
    {
     TRANSFER_DATA_COMPLETE_HANDLER TransferDataCompleteHandler;
     WAN_TRANSFER_DATA_COMPLETE_HANDLER WanTransferDataCompleteHandler;
    };

    RESET_COMPLETE_HANDLER          ResetCompleteHandler;
    REQUEST_COMPLETE_HANDLER        RequestCompleteHandler;
    union
    {
        RECEIVE_HANDLER             ReceiveHandler;
        WAN_RECEIVE_HANDLER         WanReceiveHandler;
    };
    RECEIVE_COMPLETE_HANDLER        ReceiveCompleteHandler;
    STATUS_HANDLER                  StatusHandler;
    STATUS_COMPLETE_HANDLER         StatusCompleteHandler;
    NDIS_STRING                     Name;

    //
    // Start of NDIS 4.0 extensions.
    //
    RECEIVE_PACKET_HANDLER          ReceivePacketHandler;

    //
    // PnP protocol entry-points
    //
    BIND_HANDLER                    BindAdapterHandler;
    UNBIND_HANDLER                  UnbindAdapterHandler;
    PNP_EVENT_HANDLER               PnPEventHandler;
    UNLOAD_PROTOCOL_HANDLER         UnloadHandler;

} NDIS40_PROTOCOL_CHARACTERISTICS;

#endif

#if NDIS_LEGACY_DRIVER

//
// NDIS 5.0 co-NDIS Protocol handler proto-types - used by clients as well as call manager modules
//
typedef
VOID
(*CO_SEND_COMPLETE_HANDLER)(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             ProtocolVcContext,
    IN  PNDIS_PACKET            Packet
    );

typedef
VOID
(*CO_STATUS_HANDLER)(
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  NDIS_HANDLE             ProtocolVcContext   OPTIONAL,
    IN  NDIS_STATUS             GeneralStatus,
    IN  PVOID                   StatusBuffer,
    IN  UINT                    StatusBufferSize
    );

typedef
UINT
(*CO_RECEIVE_PACKET_HANDLER)(
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  NDIS_HANDLE             ProtocolVcContext,
    IN  PNDIS_PACKET            Packet
    );

typedef
NDIS_STATUS
(*CO_REQUEST_HANDLER)(
    IN  NDIS_HANDLE             ProtocolAfContext,
    IN  NDIS_HANDLE             ProtocolVcContext       OPTIONAL,
    IN  NDIS_HANDLE             ProtocolPartyContext    OPTIONAL,
    IN OUT PNDIS_REQUEST        NdisRequest
    );

typedef
VOID
(*CO_REQUEST_COMPLETE_HANDLER)(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             ProtocolAfContext       OPTIONAL,
    IN  NDIS_HANDLE             ProtocolVcContext       OPTIONAL,
    IN  NDIS_HANDLE             ProtocolPartyContext    OPTIONAL,
    IN  PNDIS_REQUEST           NdisRequest
    );

#endif // NDIS_LEGACY_DRIVER

#if (NDIS_SUPPORT_NDIS6 || defined(NDIS60))
typedef
NDIS_STATUS
(*CO_OID_REQUEST_HANDLER)(
    IN  NDIS_HANDLE             ProtocolAfContext,
    IN  NDIS_HANDLE             ProtocolVcContext       OPTIONAL,
    IN  NDIS_HANDLE             ProtocolPartyContext    OPTIONAL,
    IN OUT PNDIS_OID_REQUEST    OidRequest
    );


typedef
VOID
(*CO_OID_REQUEST_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             ProtocolAfContext       OPTIONAL,
    IN  NDIS_HANDLE             ProtocolVcContext       OPTIONAL,
    IN  NDIS_HANDLE             ProtocolPartyContext    OPTIONAL,
    IN  PNDIS_OID_REQUEST       OidRequest,
    IN  NDIS_STATUS             Status
    );
#endif // (NDIS_SUPPORT_NDIS6 || defined(NDIS60))

//
// CO_CREATE_VC_HANDLER and CO_DELETE_VC_HANDLER are synchronous calls
// the following APIs are used by NDIS 6 protocols as well as NDIS 5 protocols
//
typedef
NDIS_STATUS
(*CO_CREATE_VC_HANDLER)(
    IN  NDIS_HANDLE             ProtocolAfContext,
    IN  NDIS_HANDLE             NdisVcHandle,
    OUT PNDIS_HANDLE            ProtocolVcContext
    );

typedef
NDIS_STATUS
(*CO_DELETE_VC_HANDLER)(
    IN  NDIS_HANDLE             ProtocolVcContext
    );

typedef
VOID
(*CO_AF_REGISTER_NOTIFY_HANDLER)(
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  PCO_ADDRESS_FAMILY      AddressFamily
    );

#if NDIS_LEGACY_PROTOCOL

typedef struct _NDIS50_PROTOCOL_CHARACTERISTICS
{
#ifdef __cplusplus
    NDIS40_PROTOCOL_CHARACTERISTICS Ndis40Chars;
#else
    NDIS40_PROTOCOL_CHARACTERISTICS;
#endif

    //
    // Placeholders for protocol extensions for PnP/PM etc.
    //
    PVOID                           ReservedHandlers[4];

    //
    // Start of NDIS 5.0 extensions.
    //

    CO_SEND_COMPLETE_HANDLER        CoSendCompleteHandler;
    CO_STATUS_HANDLER               CoStatusHandler;
    CO_RECEIVE_PACKET_HANDLER       CoReceivePacketHandler;
    CO_AF_REGISTER_NOTIFY_HANDLER   CoAfRegisterNotifyHandler;

} NDIS50_PROTOCOL_CHARACTERISTICS;

#endif // NDIS_LEGACY_PROTOCOL

#if (NDIS_SUPPORT_NDIS6 || defined(NDIS60))

//
// CONDIS 6.0 protocol's entry points
//

typedef
VOID
(*CO_RECEIVE_NET_BUFFER_LISTS_HANDLER)(
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  NDIS_HANDLE             ProtocolVcContext,
    IN  PNET_BUFFER_LIST        NetBufferLists,
    IN  ULONG                   NumberOfNetBufferLists,
    IN  ULONG                   ReceiveFlags
    );

typedef
VOID
(*CO_SEND_NET_BUFFER_LISTS_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             ProtocolVcContext,
    IN  PNET_BUFFER_LIST        NetBufferLists,
    IN  ULONG                   SendCompleteFlags
    );

typedef
VOID
(*CO_STATUS_HANDLER_EX)(
    IN NDIS_HANDLE              ProtocolBindingContext,
    IN NDIS_HANDLE              ProtocolVcContext,
    IN PNDIS_STATUS_INDICATION  StatusIndication
    );
//
// CoNDIS 6 Client handler
//
typedef
NDIS_STATUS
(*CL_NOTIFY_CLOSE_AF_HANDLER)(
    IN NDIS_HANDLE            ClientAfContext
    );

typedef
VOID
(*CL_OPEN_AF_COMPLETE_HANDLER_EX)(
    IN  NDIS_HANDLE             ProtocolAfContext,
    IN  NDIS_HANDLE             NdisAfHandle,
    IN  NDIS_STATUS             Status
    );

//
// CoNDIS 6 Call manager handler
//
typedef
VOID
(*CM_NOTIFY_CLOSE_AF_COMPLETE_HANDLER)(
    IN NDIS_HANDLE           CallMgrAfContext,
    IN NDIS_STATUS           Status 
    );

EXPORT
VOID
NdisDeregisterProtocolDriver(
    IN  NDIS_HANDLE             NdisProtocolHandle
    );


#endif // (NDIS_SUPPORT_NDIS6 || defined(NDIS60))


#if NDIS_LEGACY_PROTOCOL

EXPORT
VOID
NdisDeregisterProtocol(
    OUT PNDIS_STATUS            Status,
    IN  NDIS_HANDLE             NdisProtocolHandle
    );


EXPORT
VOID
NdisOpenAdapter(
    OUT PNDIS_STATUS            Status,
    OUT PNDIS_STATUS            OpenErrorStatus,
    OUT PNDIS_HANDLE            NdisBindingHandle,
    OUT PUINT                   SelectedMediumIndex,
    IN  PNDIS_MEDIUM            MediumArray,
    IN  UINT                    MediumArraySize,
    IN  NDIS_HANDLE             NdisProtocolHandle,
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  PNDIS_STRING            AdapterName,
    IN  UINT                    OpenOptions,
    IN  PSTRING                 AddressingInformation OPTIONAL
    );


EXPORT
VOID
NdisCloseAdapter(
    OUT PNDIS_STATUS            Status,
    IN  NDIS_HANDLE             NdisBindingHandle
    );


EXPORT
VOID
NdisCompleteBindAdapter(
    IN   NDIS_HANDLE            BindAdapterContext,
    IN   NDIS_STATUS            Status,
    IN   NDIS_STATUS            OpenStatus
    );

EXPORT
VOID
NdisCompleteUnbindAdapter(
    IN   NDIS_HANDLE            UnbindAdapterContext,
    IN   NDIS_STATUS            Status
    );

EXPORT
VOID
NdisSetProtocolFilter(
    OUT PNDIS_STATUS            Status,
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  RECEIVE_HANDLER         ReceiveHandler,
    IN  RECEIVE_PACKET_HANDLER  ReceivePacketHandler,
    IN  NDIS_MEDIUM             Medium,
    IN  UINT                    Offset,
    IN  UINT                    Size,
    IN  PUCHAR                  Pattern
    );

EXPORT
VOID
NdisGetDriverHandle(
    IN  NDIS_HANDLE             NdisBindingHandle,
    OUT PNDIS_HANDLE            NdisDriverHandle
    );

EXPORT
VOID
NdisOpenProtocolConfiguration(
    OUT PNDIS_STATUS            Status,
    OUT PNDIS_HANDLE            ConfigurationHandle,
    IN  PNDIS_STRING            ProtocolSection
    );

#endif // NDIS_LEGACY_PROTOCOL

EXPORT
VOID
NdisReEnumerateProtocolBindings(
    IN  NDIS_HANDLE             NdisProtocolHandle
    );

EXPORT
NDIS_STATUS
NdisWriteEventLogEntry(
    IN  PVOID                   LogHandle,
    IN  NDIS_STATUS             EventCode,
    IN  ULONG                   UniqueEventValue,
    IN  USHORT                  NumStrings,
    IN  PVOID                   StringsList    OPTIONAL,
    IN  ULONG                   DataSize,
    IN  PVOID                   Data           OPTIONAL
    );

#if NDIS_LEGACY_PROTOCOL

//
//  The following routine is used by transports to complete pending
//  network PnP events.
//
EXPORT
VOID
NdisCompletePnPEvent(
    IN  NDIS_STATUS     Status,
    IN  NDIS_HANDLE     NdisBindingHandle,
    IN  PNET_PNP_EVENT  NetPnPEvent
    );

#endif // NDIS_LEGACY_PROTOCOL

#if (NDIS_SUPPORT_NDIS6 || defined(NDIS60))
EXPORT
VOID
NdisCompleteNetPnPEvent(
    IN  NDIS_HANDLE     NdisBindingHandle,
    IN  PNET_PNP_EVENT_NOTIFICATION NetPnPEventNotification,
    IN  NDIS_STATUS     Status
    );
#endif //  (NDIS_SUPPORT_NDIS6 || defined(NDIS60))

//
//  The following routine is used by a transport to query the localized
//  friendly instance name of the adapter that they are bound to. There
//  are two variations of this, one uses the binding handle and the other
//  the binding context. Some transports need this before they bind - like
//  TCP/IP for instance.
//
EXPORT
NDIS_STATUS
NdisQueryAdapterInstanceName(
    OUT PNDIS_STRING    pAdapterInstanceName,
    IN  NDIS_HANDLE     NdisBindingHandle
    );

EXPORT
NDIS_STATUS
NdisQueryBindInstanceName(
    OUT PNDIS_STRING    pAdapterInstanceName,
    IN  NDIS_HANDLE     BindingContext
    );

//
// The following is used by TDI/NDIS interface as part of Network PnP.
// For use by TDI alone.
//
typedef
NTSTATUS
(*TDI_REGISTER_CALLBACK)(
    IN  PUNICODE_STRING         DeviceName,
    OUT HANDLE  *               TdiHandle
    );

typedef
NTSTATUS
(*TDI_PNP_HANDLER)(
    IN  PUNICODE_STRING         UpperComponent,
    IN  PUNICODE_STRING         LowerComponent,
    IN  PUNICODE_STRING         BindList,
    IN  PVOID                   ReconfigBuffer,
    IN  UINT                    ReconfigBufferSize,
    IN  UINT                    Operation
    );

EXPORT
VOID
NdisRegisterTdiCallBack(
    IN  TDI_REGISTER_CALLBACK   RegisterCallback,
    IN  TDI_PNP_HANDLER         PnPHandler
    );

EXPORT
VOID
NdisDeregisterTdiCallBack(
    VOID
    );

#if NDIS_LEGACY_PROTOCOL

VOID
NdisReset(
    OUT PNDIS_STATUS            Status,
    IN  NDIS_HANDLE             NdisBindingHandle
    );

VOID
NdisRequest(
    OUT PNDIS_STATUS            Status,
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  PNDIS_REQUEST           NdisRequest
    );

#ifdef __cplusplus

#define NdisSend(Status, NdisBindingHandle, Packet)                                     \
{                                                                                       \
    *(Status) =                                                                         \
        (((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->NdisCommonOpenBlock.SendHandler)(     \
            ((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->NdisCommonOpenBlock.BindingHandle, \
        (Packet));                                                                      \
}

#define NdisSendPackets(NdisBindingHandle, PacketArray, NumberOfPackets)                \
{                                                                                       \
    (((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->NdisCommonOpenBlock.SendPacketsHandler)(  \
        (PNDIS_OPEN_BLOCK)(NdisBindingHandle),                                          \
        (PacketArray),                                                                  \
        (NumberOfPackets));                                                             \
}


#define NdisTransferData(Status,                                                        \
                         NdisBindingHandle,                                             \
                         MacReceiveContext,                                             \
                         ByteOffset,                                                    \
                         BytesToTransfer,                                               \
                         Packet,                                                        \
                         BytesTransferred)                                              \
{                                                                                       \
    *(Status) =                                                                         \
        (((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->NdisCommonOpenBlock.TransferDataHandler)( \
            ((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->NdisCommonOpenBlock.BindingHandle, \
            (MacReceiveContext),                                                        \
            (ByteOffset),                                                               \
            (BytesToTransfer),                                                          \
            (Packet),                                                                   \
            (BytesTransferred));                                                        \
}

#else

#define NdisSend(Status, NdisBindingHandle, Packet)                         \
{                                                                           \
    *(Status) =                                                             \
        (((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->SendHandler)(             \
            ((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->BindingHandle,         \
        (Packet));                                                          \
}


#define NdisSendPackets(NdisBindingHandle, PacketArray, NumberOfPackets)    \
{                                                                           \
    (((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->SendPacketsHandler)(          \
        (PNDIS_OPEN_BLOCK)(NdisBindingHandle),                              \
        (PacketArray),                                                      \
        (NumberOfPackets));                                                 \
}

#define NdisTransferData(Status,                                            \
                         NdisBindingHandle,                                 \
                         MacReceiveContext,                                 \
                         ByteOffset,                                        \
                         BytesToTransfer,                                   \
                         Packet,                                            \
                         BytesTransferred)                                  \
{                                                                           \
    *(Status) =                                                             \
        (((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->TransferDataHandler)(     \
            ((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->BindingHandle,         \
            (MacReceiveContext),                                            \
            (ByteOffset),                                                   \
            (BytesToTransfer),                                              \
            (Packet),                                                       \
            (BytesTransferred));                                            \
}

#endif // ifdef __cplusplus
#endif // NDIS_LEGACY_PROTOCOL

#if NDIS_LEGACY_PROTOCOL
//
// Routines to access packet flags
//

/*++

VOID
NdisSetSendFlags(
    IN  PNDIS_PACKET            Packet,
    IN  UINT                    Flags
    );

--*/

#define NdisSetSendFlags(_Packet,_Flags)    (_Packet)->Private.Flags = (_Flags)

/*++

VOID
NdisQuerySendFlags(
    IN  PNDIS_PACKET            Packet,
    OUT PUINT                   Flags
    );

--*/

#define NdisQuerySendFlags(_Packet,_Flags)  *(_Flags) = (_Packet)->Private.Flags

#endif // NDIS_LEGACY_PROTOCOL

#if NDIS_LEGACY_DRIVER
//
// The following is the minimum size of packets a miniport must allocate
// when it indicates packets via NdisMIndicatePacket or NdisMCoIndicatePacket
//
#define PROTOCOL_RESERVED_SIZE_IN_PACKET    (4 * sizeof(PVOID))
#endif // NDIS_LEGACY_DRIVER


#ifdef __cplusplus
#define WanMiniportSend(Status,                                                         \
                        NdisBindingHandle,                                              \
                        NdisLinkHandle,                                                 \
                        WanPacket)                                                      \
{                                                                                       \
    *(Status) =                                                                         \
        ((((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->NdisCommonOpenBlock.WanSendHandler))( \
            ((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->NdisCommonOpenBlock.BindingHandle, \
            (NdisLinkHandle),                                                           \
            (PNDIS_PACKET)(WanPacket));                                                 \
}

#else
#define WanMiniportSend(Status,                                             \
                        NdisBindingHandle,                                  \
                        NdisLinkHandle,                                     \
                        WanPacket)                                          \
{                                                                           \
    *(Status) =                                                             \
        ((((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->WanSendHandler))(        \
            ((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->BindingHandle,         \
            (NdisLinkHandle),                                               \
            (PNDIS_PACKET)(WanPacket));                                     \
}

#endif


#if NDIS_LEGACY_PROTOCOL

EXPORT
VOID
NdisReturnPackets(
    IN  PNDIS_PACKET    *       PacketsToReturn,
    IN  UINT                    NumberOfPackets
    );

EXPORT
PNDIS_PACKET
NdisGetReceivedPacket(
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  NDIS_HANDLE             MacContext
    );

#endif // NDIS_LEGACY_PROTOCOL

//
// Macros to portably manipulate NDIS buffers.
//

#define NdisBufferLength(Buffer)                            MmGetMdlByteCount(Buffer)
#define NdisBufferVirtualAddress(_Buffer)                   MmGetSystemAddressForMdl(_Buffer)
#define NdisBufferVirtualAddressSafe(_Buffer, _Priority)    MmGetSystemAddressForMdlSafe(_Buffer, _Priority)



#if NDIS_LEGACY_PROTOCOL

EXPORT
VOID
NdisCancelSendPackets(
    IN  NDIS_HANDLE     NdisBindingHandle,
    IN  PVOID           CancelId
    );


EXPORT
NDIS_STATUS
NdisQueryPendingIOCount(
    IN      PVOID       NdisBindingHandle,
    IN OUT  PULONG      IoCount
    );

#endif // NDIS_LEGACY_PROTOCOL

EXPORT
UCHAR
NdisGeneratePartialCancelId(
    VOID
    );


#if (NDIS_SUPPORT_NDIS6 || defined(NDIS60))

//
// NDIS 6.0 protocol's entry points
//
typedef
VOID
(*OID_REQUEST_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  PNDIS_OID_REQUEST       OidRequest,
    IN  NDIS_STATUS             Status
    );

typedef
VOID
(*RECEIVE_NET_BUFFER_LISTS_HANDLER)(
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  PNET_BUFFER_LIST        NetBufferLists,
    IN  NDIS_PORT_NUMBER        PortNumber,
    IN  ULONG                   NumberOfNetBufferLists,
    IN  ULONG                   ReceiveFlags
    );

typedef
VOID
(*SEND_NET_BUFFER_LISTS_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  PNET_BUFFER_LIST        NetBufferList,
    IN  ULONG                   SendCompleteFlags
    );

//
// structure passed to protocol's BIND_HANDLER_EX
//
#define NDIS_BIND_PARAMETERS_REVISION_1     1

typedef struct _NDIS_BIND_PARAMETERS
{
    NDIS_OBJECT_HEADER              Header;
    PNDIS_STRING                    ProtocolSection;
    PNDIS_STRING                    AdapterName;
    PDEVICE_OBJECT                  PhysicalDeviceObject;
    NDIS_MEDIUM                     MediaType;
    ULONG                           MtuSize;
    ULONG64                         MaxXmitLinkSpeed;
    ULONG64                         XmitLinkSpeed;
    ULONG64                         MaxRcvLinkSpeed;
    ULONG64                         RcvLinkSpeed;
    NDIS_MEDIA_CONNECT_STATE        MediaConnectState;
    NDIS_MEDIA_DUPLEX_STATE         MediaDuplexState;
    ULONG                           LookaheadSize;
    PNDIS_PNP_CAPABILITIES          PowerManagementCapabilities;
    ULONG                           SupportedPacketFilters;
    ULONG                           MaxMulticastListSize;
    USHORT                          MacAddressLength;
    UCHAR                           CurrentMacAddress[NDIS_MAX_PHYS_ADDRESS_LENGTH];
    NDIS_PHYSICAL_MEDIUM            PhysicalMediumType;
    PNDIS_RECEIVE_SCALE_CAPABILITIES RcvScaleCapabilities;
    NET_LUID                        BoundIfNetluid;
    NET_IFINDEX                     BoundIfIndex;
    NET_LUID                        LowestIfNetluid;
    NET_IFINDEX                     LowestIfIndex;
    NET_IF_ACCESS_TYPE              AccessType; // NET_IF_ACCESS_BROADCAST for a typical ethernet adapter
    NET_IF_DIRECTION_TYPE           DirectionType; // NET_IF_DIRECTION_SENDRECEIVE for a typical ethernet adapter
    NET_IF_CONNECTION_TYPE          ConnectionType; // NET_IF_CONNECTION_DEDICATED for a typical ethernet adapter
    NET_IFTYPE                      IfType; // IF_TYPE_ETHERNET_CSMACD for a typical ethernet adapter (regardless of speed)
    BOOLEAN                         IfConnectorPresent; // RFC 2665 TRUE if physical adapter
    PNDIS_PORT                      ActivePorts; 
    ULONG                           DataBackFillSize;
    ULONG                           ContextBackFillSize;
    ULONG                           MacOptions;
    NET_IF_COMPARTMENT_ID           CompartmentId;
    PNDIS_OFFLOAD                   DefaultOffloadConfiguration;
    PNDIS_TCP_CONNECTION_OFFLOAD    TcpConnectionOffloadCapabilities;
    PNDIS_STRING                    BoundAdapterName;
}NDIS_BIND_PARAMETERS, *PNDIS_BIND_PARAMETERS;

#define NDIS_SIZEOF_BIND_PARAMETERS_REVISION_1     \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_BIND_PARAMETERS, BoundAdapterName)

typedef
NDIS_STATUS
(*BIND_HANDLER_EX)(
    IN  NDIS_HANDLE             ProtocolDriverContext,
    IN  NDIS_HANDLE             BindContext,
    IN  PNDIS_BIND_PARAMETERS   BindParameters
    );

typedef
NDIS_STATUS
(*UNBIND_HANDLER_EX)(
    IN  NDIS_HANDLE             UnbindContext,
    IN  NDIS_HANDLE             ProtocolBindingContext
    );

typedef
VOID
(*OPEN_ADAPTER_COMPLETE_HANDLER_EX)(
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  NDIS_STATUS             Status
    );

typedef
VOID
(*CLOSE_ADAPTER_COMPLETE_HANDLER_EX)(
    IN  NDIS_HANDLE             ProtocolBindingContext
    );


typedef
VOID
(*STATUS_HANDLER_EX)(
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  PNDIS_STATUS_INDICATION StatusIndication
    );

typedef
NDIS_STATUS
(*NET_PNP_EVENT_HANDLER)(
    IN  NDIS_HANDLE                 ProtocolBindingContext,
    IN  PNET_PNP_EVENT_NOTIFICATION NetPnPEventNotification
    );

typedef
VOID
(*UNINSTALL_PROTOCOL_HANDLER)(
    VOID
    );


#define NDIS_PROTOCOL_CO_CHARACTERISTICS_REVISION_1     1

typedef struct _NDIS_PROTOCOL_CO_CHARACTERISTICS
{
    NDIS_OBJECT_HEADER                      Header;     // Header.Type = NDIS_OBJECT_TYPE_PROTOCOL_CO_CHARACTERISTICS
    ULONG                                   Flags;
    CO_STATUS_HANDLER_EX                    CoStatusHandlerEx;
    CO_AF_REGISTER_NOTIFY_HANDLER           CoAfRegisterNotifyHandler;
    CO_RECEIVE_NET_BUFFER_LISTS_HANDLER     CoReceiveNetBufferListsHandler;
    CO_SEND_NET_BUFFER_LISTS_COMPLETE_HANDLER CoSendNetBufferListsCompleteHandler;
} NDIS_PROTOCOL_CO_CHARACTERISTICS, *PNDIS_PROTOCOL_CO_CHARACTERISTICS;

#define NDIS_SIZEOF_PROTOCOL_CO_CHARACTERISTICS_REVISION_1    \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_PROTOCOL_CO_CHARACTERISTICS, CoSendNetBufferListsCompleteHandler)

#define NDIS_PROTOCOL_DRIVER_CHARACTERISTICS_REVISION_1     1

typedef struct _NDIS_PROTOCOL_DRIVER_CHARACTERISTICS
{
    NDIS_OBJECT_HEADER                      Header;
    UCHAR                                   MajorNdisVersion;
    UCHAR                                   MinorNdisVersion;
    UCHAR                                   MajorDriverVersion;
    UCHAR                                   MinorDriverVersion;
    ULONG                                   Flags;
    NDIS_STRING                             Name;
    SET_OPTIONS_HANDLER                     SetOptionsHandler;
    BIND_HANDLER_EX                         BindAdapterHandlerEx;
    UNBIND_HANDLER_EX                       UnbindAdapterHandlerEx;
    OPEN_ADAPTER_COMPLETE_HANDLER_EX        OpenAdapterCompleteHandlerEx;
    CLOSE_ADAPTER_COMPLETE_HANDLER_EX       CloseAdapterCompleteHandlerEx;
    NET_PNP_EVENT_HANDLER                   NetPnPEventHandler;
    UNINSTALL_PROTOCOL_HANDLER              UninstallHandler;
    OID_REQUEST_COMPLETE_HANDLER            OidRequestCompleteHandler;
    STATUS_HANDLER_EX                       StatusHandlerEx;
    RECEIVE_NET_BUFFER_LISTS_HANDLER        ReceiveNetBufferListsHandler;
    SEND_NET_BUFFER_LISTS_COMPLETE_HANDLER  SendNetBufferListsCompleteHandler;
} NDIS_PROTOCOL_DRIVER_CHARACTERISTICS, *PNDIS_PROTOCOL_DRIVER_CHARACTERISTICS;

#define NDIS_SIZEOF_PROTOCOL_DRIVER_CHARACTERISTICS_REVISION_1    \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_PROTOCOL_DRIVER_CHARACTERISTICS, SendNetBufferListsCompleteHandler)

#endif // (NDIS_SUPPORT_NDIS6 || defined(NDIS60))


#if NDIS_LEGACY_PROTOCOL

#if (defined(NDIS50) || defined(NDIS51))
typedef NDIS50_PROTOCOL_CHARACTERISTICS  NDIS_PROTOCOL_CHARACTERISTICS;
#else
typedef NDIS40_PROTOCOL_CHARACTERISTICS  NDIS_PROTOCOL_CHARACTERISTICS;
#endif


typedef NDIS_PROTOCOL_CHARACTERISTICS *PNDIS_PROTOCOL_CHARACTERISTICS;

EXPORT
VOID
NdisRegisterProtocol(
    OUT PNDIS_STATUS            Status,
    OUT PNDIS_HANDLE            NdisProtocolHandle,
    IN  PNDIS_PROTOCOL_CHARACTERISTICS ProtocolCharacteristics,
    IN  UINT                    CharacteristicsLength
    );

#endif // NDIS_LEGACY_PROTOCOL

#if (NDIS_SUPPORT_NDIS6 || defined(NDIS60))

EXPORT
NDIS_STATUS
NdisRegisterProtocolDriver(
    IN  NDIS_HANDLE             ProtocolDriverContext,
    IN  PNDIS_PROTOCOL_DRIVER_CHARACTERISTICS ProtocolCharacteristics,
    OUT PNDIS_HANDLE            NdisProtocolHandle
    );

EXPORT
VOID
NdisSendNetBufferLists(
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  PNET_BUFFER_LIST        NetBufferLists,
    IN  NDIS_PORT_NUMBER        PortNumber,
    IN  ULONG                   SendFlags
    );

EXPORT
VOID
NdisReturnNetBufferLists(
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  PNET_BUFFER_LIST        NetBufferLists,
    IN  ULONG                   ReturnFlags
    );

EXPORT
VOID
NdisCancelSendNetBufferLists(
    IN  NDIS_HANDLE     NdisBindingHandle,
    IN  PVOID           CancelId
    );

EXPORT
NDIS_STATUS
NdisOidRequest(
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  PNDIS_OID_REQUEST       OidRequest
    );

EXPORT
VOID
NdisCancelOidRequest(
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  PVOID                   RequestId
    );


EXPORT
NDIS_STATUS
NdisUnbindAdapter(
    IN  NDIS_HANDLE                     NdisBindingHandle
    );


//
// structure passed to NdisOpenAdapterEx
//

#define NDIS_OPEN_PARAMETERS_REVISION_1     1
typedef struct _NDIS_OPEN_PARAMETERS
{
    NDIS_OBJECT_HEADER          Header;
    PNDIS_STRING                AdapterName;
    PNDIS_MEDIUM                MediumArray;
    UINT                        MediumArraySize;
    PUINT                       SelectedMediumIndex;
    PNET_FRAME_TYPE             FrameTypeArray;
    UINT                        FrameTypeArraySize;
} NDIS_OPEN_PARAMETERS, *PNDIS_OPEN_PARAMETERS;

#define NDIS_SIZEOF_OPEN_PARAMETERS_REVSION_1    \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_OPEN_PARAMETERS, FrameTypeArraySize)

EXPORT
NDIS_STATUS
NdisOpenAdapterEx(
    IN  NDIS_HANDLE             NdisProtocolHandle,
    IN  NDIS_HANDLE             ProtocolBindingContext,
    IN  PNDIS_OPEN_PARAMETERS   OpenParameters,
    IN  NDIS_HANDLE             BindContext,
    OUT PNDIS_HANDLE            NdisBindingHandle
    );

EXPORT
VOID
NdisCompleteBindAdapterEx(
    IN  NDIS_HANDLE             BindAdapterContext,
    IN  NDIS_STATUS             Status
    );

EXPORT
NDIS_STATUS
NdisCloseAdapterEx(
    IN  NDIS_HANDLE             NdisBindingHandle
    );

EXPORT
VOID
NdisCompleteUnbindAdapterEx(
    IN  NDIS_HANDLE             UnbindContext
    );



#define NDIS_PROTOCOL_PAUSE_PARAMETERS_REVISION_1     1
    
typedef struct _NDIS_PROTOCOL_PAUSE_PARAMETERS
{
    NDIS_OBJECT_HEADER          Header;
    ULONG                       Flags;
    ULONG                       PauseReason;
} NDIS_PROTOCOL_PAUSE_PARAMETERS, *PNDIS_PROTOCOL_PAUSE_PARAMETERS;

#define NDIS_SIZEOF_PROTOCOL_PAUSE_PARAMETERS_REVISION_1    \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_PROTOCOL_PAUSE_PARAMETERS, Flags)


#define NDIS_PROTOCOL_RESTART_PARAMETERS_REVISION_1     1

typedef struct _NDIS_PROTOCOL_RESTART_PARAMETERS
{
    NDIS_OBJECT_HEADER          Header;
    PUCHAR                      FilterModuleNameBuffer;
    ULONG                       FilterModuleNameBufferLength;
    PNDIS_RESTART_ATTRIBUTES    RestartAttributes;
    NET_IFINDEX                 BoundIfIndex;  
    NET_LUID                    BoundIfNetluid;    
    ULONG                       Flags;
} NDIS_PROTOCOL_RESTART_PARAMETERS, *PNDIS_PROTOCOL_RESTART_PARAMETERS;

#define NDIS_SIZEOF_PROTOCOL_RESTART_PARAMETERS_REVISION_1    \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_PROTOCOL_RESTART_PARAMETERS, Flags)

#endif // (NDIS_SUPPORT_NDIS6 || defined(NDIS60))


//
// needed for macros used by legacy protocols
//
#if !defined(NDIS_WRAPPER)
#if NDIS_LEGACY_PROTOCOL
    typedef struct _NDIS_COMMON_OPEN_BLOCK
    {
        PVOID                       MacHandle;          // needed for backward compatibility
     
        NDIS_HANDLE                 BindingHandle;
        NDIS_HANDLE                 Reserved6;
        NDIS_HANDLE                 Reserved7;
        NDIS_HANDLE                 Reserved8;
    
        PVOID                       Reserved9;
        NDIS_HANDLE                 Reserved10;
    
        NDIS_HANDLE                 Reserved11;
        BOOLEAN                     Reserved12;
        BOOLEAN                     Reserved2;
        BOOLEAN                     Reserved3;
        BOOLEAN                     Reserved4;
        PVOID                       Reserved13;
        KSPIN_LOCK                  Reserved5;
        NDIS_HANDLE                 Reserved14;
    
        //
        // These are referenced by the macros used by protocols to call.
        // All of the ones referenced by the macros are internal NDIS handlers for the miniports
        //
        union
        {
            SEND_HANDLER            SendHandler;
            WAN_SEND_HANDLER        WanSendHandler;
        };
        TRANSFER_DATA_HANDLER       TransferDataHandler;
    
        //
        // These are referenced internally by NDIS
        //
        SEND_COMPLETE_HANDLER       SendCompleteHandler;
        TRANSFER_DATA_COMPLETE_HANDLER TransferDataCompleteHandler;
        RECEIVE_HANDLER             ReceiveHandler;
        RECEIVE_COMPLETE_HANDLER    ReceiveCompleteHandler;
        WAN_RECEIVE_HANDLER         WanReceiveHandler;
        REQUEST_COMPLETE_HANDLER    RequestCompleteHandler;
    
        //
        // NDIS 4.0 extensions
        //
        RECEIVE_PACKET_HANDLER      ReceivePacketHandler;
        SEND_PACKETS_HANDLER        SendPacketsHandler;
    
        //
        // More Cached Handlers
        //
        RESET_HANDLER               ResetHandler;
        REQUEST_HANDLER             RequestHandler;
        RESET_COMPLETE_HANDLER      ResetCompleteHandler;
        STATUS_HANDLER              StatusHandler;
        STATUS_COMPLETE_HANDLER     StatusCompleteHandler;
    
    }NDIS_COMMON_OPEN_BLOCK, *PNDIS_COMMON_OPEN_BLOCK;
    //
    // one of these per open on an adapter/protocol
    //
    struct _NDIS_OPEN_BLOCK
    {
#ifdef __cplusplus
        NDIS_COMMON_OPEN_BLOCK NdisCommonOpenBlock;
#else
        NDIS_COMMON_OPEN_BLOCK;
#endif
    
    };

#endif // NDIS_LEGACY_PROTOCOL
#endif // NDIS_WRAPPER




#pragma once

#include <xfilter.h>

#define NDIS_M_MAX_LOOKAHEAD 526


#if NDIS_LEGACY_MINIPORT

//
// Function types for NDIS_MINIPORT_CHARACTERISTICS
//


typedef
BOOLEAN
(*W_CHECK_FOR_HANG_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext
    );

typedef
VOID
(*W_DISABLE_INTERRUPT_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext
    );

typedef
VOID
(*W_ENABLE_INTERRUPT_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext
    );

typedef
VOID
(*W_HALT_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext
    );

typedef
VOID
(*W_HANDLE_INTERRUPT_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext
    );

typedef
NDIS_STATUS
(*W_INITIALIZE_HANDLER)(
    OUT PNDIS_STATUS            OpenErrorStatus,
    OUT PUINT                   SelectedMediumIndex,
    IN  PNDIS_MEDIUM            MediumArray,
    IN  UINT                    MediumArraySize,
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  NDIS_HANDLE             WrapperConfigurationContext
    );

typedef
VOID
(*W_ISR_HANDLER)(
    OUT PBOOLEAN                InterruptRecognized,
    OUT PBOOLEAN                QueueMiniportHandleInterrupt,
    IN  NDIS_HANDLE             MiniportAdapterContext
    );

typedef
NDIS_STATUS
(*W_QUERY_INFORMATION_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  NDIS_OID                Oid,
    IN  PVOID                   InformationBuffer,
    IN  ULONG                   InformationBufferLength,
    OUT PULONG                  BytesWritten,
    OUT PULONG                  BytesNeeded
    );

typedef
NDIS_STATUS
(*W_RECONFIGURE_HANDLER)(
    OUT PNDIS_STATUS            OpenErrorStatus,
    IN  NDIS_HANDLE             MiniportAdapterContext  OPTIONAL,
    IN  NDIS_HANDLE             WrapperConfigurationContext
    );

typedef
NDIS_STATUS
(*W_RESET_HANDLER)(
    OUT PBOOLEAN                AddressingReset,
    IN  NDIS_HANDLE             MiniportAdapterContext
    );

typedef
NDIS_STATUS
(*W_SEND_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  PNDIS_PACKET            Packet,
    IN  UINT                    Flags
    );

typedef
NDIS_STATUS
(*WM_SEND_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  NDIS_HANDLE             NdisLinkHandle,
    IN  PNDIS_WAN_PACKET        Packet
    );

typedef
NDIS_STATUS
(*W_SET_INFORMATION_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  NDIS_OID                Oid,
    IN  PVOID                   InformationBuffer,
    IN  ULONG                   InformationBufferLength,
    OUT PULONG                  BytesRead,
    OUT PULONG                  BytesNeeded
    );

typedef
NDIS_STATUS
(*W_TRANSFER_DATA_HANDLER)(
    OUT PNDIS_PACKET            Packet,
    OUT PUINT                   BytesTransferred,
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  NDIS_HANDLE             MiniportReceiveContext,
    IN  UINT                    ByteOffset,
    IN  UINT                    BytesToTransfer
    );

typedef
NDIS_STATUS
(*WM_TRANSFER_DATA_HANDLER)(
    VOID
    );

//
// Definition for shutdown handler
//

typedef
VOID
(*ADAPTER_SHUTDOWN_HANDLER) (
    IN  PVOID ShutdownContext
    );

//
// Miniport extensions for NDIS 4.0
//
typedef
VOID
(*W_RETURN_PACKET_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  PNDIS_PACKET            Packet
    );

//
// NDIS 4.0 extension
//
typedef
VOID
(*W_SEND_PACKETS_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  PPNDIS_PACKET           PacketArray,
    IN  UINT                    NumberOfPackets
    );

typedef
VOID
(*W_ALLOCATE_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  PVOID                   VirtualAddress,
    IN  PNDIS_PHYSICAL_ADDRESS  PhysicalAddress,
    IN  ULONG                   Length,
    IN  PVOID                   Context
    );

#endif // NDIS_LEGACY_MINIPORT


//
// W_CO_CREATE_VC_HANDLER is a synchronous call
//
typedef
NDIS_STATUS
(*W_CO_CREATE_VC_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  NDIS_HANDLE             NdisVcHandle,
    OUT PNDIS_HANDLE            MiniportVcContext
    );

typedef
NDIS_STATUS
(*W_CO_DELETE_VC_HANDLER)(
    IN  NDIS_HANDLE             MiniportVcContext
    );

typedef
NDIS_STATUS
(*W_CO_ACTIVATE_VC_HANDLER)(
    IN  NDIS_HANDLE             MiniportVcContext,
    IN OUT PCO_CALL_PARAMETERS  CallParameters
    );

typedef
NDIS_STATUS
(*W_CO_DEACTIVATE_VC_HANDLER)(
    IN  NDIS_HANDLE             MiniportVcContext
    );

#if NDIS_LEGACY_MINIPORT

typedef
VOID
(*W_CO_SEND_PACKETS_HANDLER)(
    IN  NDIS_HANDLE             MiniportVcContext,
    IN  PPNDIS_PACKET           PacketArray,
    IN  UINT                    NumberOfPackets
    );

typedef
NDIS_STATUS
(*W_CO_REQUEST_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  NDIS_HANDLE             MiniportVcContext   OPTIONAL,
    IN OUT PNDIS_REQUEST        NdisRequest
    );

#endif // NDIS_LEGACY_MINIPORT


#if NDIS_SUPPORT_NDIS6

//
// CONDIS 6.0 handlers
//

typedef
VOID
(*W_CO_SEND_NET_BUFFER_LISTS_HANDLER)(
    IN  NDIS_HANDLE             MiniportVcContext,
    IN  PNET_BUFFER_LIST        NetBufferLists,
    IN  ULONG                   SendFlags
    );


typedef 
NDIS_STATUS
(*W_CO_OID_REQUEST_HANDLER)(
    IN NDIS_HANDLE           MiniportAdapterContext,
    IN NDIS_HANDLE           MiniportVcContext OPTIONAL,
    IN OUT PNDIS_OID_REQUEST NdisRequest
    );

#endif // NDIS_SUPPORT_NDIS6

typedef enum _NDIS_DEVICE_PNP_EVENT
{
    NdisDevicePnPEventQueryRemoved,
    NdisDevicePnPEventRemoved,
    NdisDevicePnPEventSurpriseRemoved,
    NdisDevicePnPEventQueryStopped,
    NdisDevicePnPEventStopped,
    NdisDevicePnPEventPowerProfileChanged,
#if NDIS_SUPPORT_NDIS6
    NdisDevicePnPEventFilterListChanged,
#endif // NDIS_SUPPORT_NDIS6
    NdisDevicePnPEventMaximum
} NDIS_DEVICE_PNP_EVENT, *PNDIS_DEVICE_PNP_EVENT;

//
// power profiles
//
typedef enum _NDIS_POWER_PROFILE
{
    NdisPowerProfileBattery,
    NdisPowerProfileAcOnLine
} NDIS_POWER_PROFILE, *PNDIS_POWER_PROFILE;

#if NDIS_LEGACY_MINIPORT

typedef struct _NDIS50_MINIPORT_CHARACTERISTICS
{   
    UCHAR                       MajorNdisVersion;
    UCHAR                       MinorNdisVersion;
    USHORT                      Filler;
    UINT                        Reserved;
    W_CHECK_FOR_HANG_HANDLER    CheckForHangHandler;
    W_DISABLE_INTERRUPT_HANDLER DisableInterruptHandler;
    W_ENABLE_INTERRUPT_HANDLER  EnableInterruptHandler;
    W_HALT_HANDLER              HaltHandler;
    W_HANDLE_INTERRUPT_HANDLER  HandleInterruptHandler;
    W_INITIALIZE_HANDLER        InitializeHandler;
    W_ISR_HANDLER               ISRHandler;
    W_QUERY_INFORMATION_HANDLER QueryInformationHandler;
    W_RECONFIGURE_HANDLER       ReconfigureHandler;
    W_RESET_HANDLER             ResetHandler;
    union
    {
        W_SEND_HANDLER          SendHandler;
        WM_SEND_HANDLER         WanSendHandler;
    };
    W_SET_INFORMATION_HANDLER   SetInformationHandler;
    union
    {
        W_TRANSFER_DATA_HANDLER TransferDataHandler;
        WM_TRANSFER_DATA_HANDLER WanTransferDataHandler;
    };

    //
    // Extensions for NDIS 4.0
    //
    W_RETURN_PACKET_HANDLER     ReturnPacketHandler;
    W_SEND_PACKETS_HANDLER      SendPacketsHandler;
    W_ALLOCATE_COMPLETE_HANDLER AllocateCompleteHandler;
    
    //
    // Extensions for NDIS 5.0
    //
    W_CO_CREATE_VC_HANDLER      CoCreateVcHandler;
    W_CO_DELETE_VC_HANDLER      CoDeleteVcHandler;
    W_CO_ACTIVATE_VC_HANDLER    CoActivateVcHandler;
    W_CO_DEACTIVATE_VC_HANDLER  CoDeactivateVcHandler;
    W_CO_SEND_PACKETS_HANDLER   CoSendPacketsHandler;
    W_CO_REQUEST_HANDLER        CoRequestHandler;
} NDIS50_MINIPORT_CHARACTERISTICS;

#if (((NDIS_MINIPORT_MAJOR_VERSION == 5) &&  (NDIS_MINIPORT_MINOR_VERSION == 1)) || NDIS_WRAPPER)
//
// Miniport extensions for NDIS 5.1
//
typedef VOID
(*W_CANCEL_SEND_PACKETS_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  PVOID                   CancelId
    );

typedef VOID
(*W_PNP_EVENT_NOTIFY_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  NDIS_DEVICE_PNP_EVENT   DevicePnPEvent,
    IN  PVOID                   InformationBuffer,
    IN  ULONG                   InformationBufferLength
    );

typedef VOID
(*W_MINIPORT_SHUTDOWN_HANDLER) (
    IN  NDIS_HANDLE                     MiniportAdapterContext
    );

#endif // (((NDIS_MINIPORT_MAJOR_VERSION == 5) &&  (NDIS_MINIPORT_MINOR_VERSION == 1)) || NDIS_WRAPPER)

#endif // NDIS_LEGACY_MINIPORT

#if NDIS_SUPPORT_NDIS6
typedef
BOOLEAN
(*MINIPORT_ISR_HANDLER)(
    IN  NDIS_HANDLE             MiniportInterruptContext,
    OUT PBOOLEAN                QueueDefaultInterruptDpc,
    OUT PULONG                  TargetProcessors
    );

typedef
VOID
(*MINIPORT_INTERRUPT_DPC_HANDLER)(
    IN  NDIS_HANDLE             MiniportInterruptContext,
    IN  PVOID                   MiniportDpcContext,
    IN  PVOID                   NdisReserved1,
    IN  PVOID                   NdisReserved2
    );

typedef
VOID
(*MINIPORT_DISABLE_INTERRUPT_HANDLER)(
    IN  NDIS_HANDLE             MiniportInterruptContext
    );

typedef
VOID
(*MINIPORT_ENABLE_INTERRUPT_HANDLER)(
    IN  NDIS_HANDLE             MiniportInterruptContext
    );

//
// MSI support handlers
//
typedef
BOOLEAN
(*MINIPORT_MSI_ISR_HANDLER)(
    IN  NDIS_HANDLE             MiniportInterruptContext,
    IN  ULONG                   MessageId,
    OUT PBOOLEAN                QueueDefaultInterruptDpc,
    OUT PULONG                  TargetProcessors
    );

typedef
VOID
(*MINIPORT_MSI_INTERRUPT_DPC_HANDLER)(
    IN  NDIS_HANDLE             MiniportInterruptContext,
    IN  ULONG                   MessageId,
    IN  PVOID                   MiniportDpcContext,
    IN  PULONG                  NdisReserved1,
    IN  PULONG                  NdisReserved2
    );


typedef
VOID
(*MINIPORT_DISABLE_MSI_INTERRUPT_HANDLER)(
    IN  NDIS_HANDLE             MiniportInterruptContext,
    IN  ULONG                   MessageId
    );

typedef
VOID
(*MINIPORT_ENABLE_MSI_INTERRUPT_HANDLER)(
    IN  NDIS_HANDLE             MiniportInterruptContext,
    IN  ULONG                   MessageId
    );

typedef enum _NDIS_INTERRUPT_TYPE
{
    NDIS_CONNECT_LINE_BASED = 1,
    NDIS_CONNECT_MESSAGE_BASED
} NDIS_INTERRUPT_TYPE, *PNDIS_INTERRUPT_TYPE;
    
#define NDIS_MINIPORT_INTERRUPT_REVISION_1          1

typedef struct _NDIS_MINIPORT_INTERRUPT_CHARACTERISTICS
{
    IN NDIS_OBJECT_HEADER                      Header;
    IN MINIPORT_ISR_HANDLER                    InterruptHandler;
    IN MINIPORT_INTERRUPT_DPC_HANDLER          InterruptDpcHandler;
    IN MINIPORT_DISABLE_INTERRUPT_HANDLER      DisableInterruptHandler;
    IN MINIPORT_ENABLE_INTERRUPT_HANDLER       EnableInterruptHandler;
    IN BOOLEAN                                 MsiSupported;
    IN BOOLEAN                                 MsiSyncWithAllMessages;
    IN MINIPORT_MSI_ISR_HANDLER                MessageInterruptHandler;
    IN MINIPORT_MSI_INTERRUPT_DPC_HANDLER      MessageInterruptDpcHandler;
    IN MINIPORT_DISABLE_MSI_INTERRUPT_HANDLER  DisableMessageInterruptHandler;
    IN MINIPORT_ENABLE_MSI_INTERRUPT_HANDLER   EnableMessageInterruptHandler;
    OUT NDIS_INTERRUPT_TYPE                    InterruptType;
    OUT PIO_INTERRUPT_MESSAGE_INFO             MessageInfoTable;
} NDIS_MINIPORT_INTERRUPT_CHARACTERISTICS, *PNDIS_MINIPORT_INTERRUPT_CHARACTERISTICS;

#define NDIS_SIZEOF_MINIPORT_INTERRUPT_CHARACTERISTICS_REVISION_1     \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_MINIPORT_INTERRUPT_CHARACTERISTICS, MessageInfoTable)

typedef
NDIS_STATUS
(*MINIPORT_ADD_DEVICE_HANDLER)(
    IN NDIS_HANDLE              NdisMiniportHandle,
    IN NDIS_HANDLE              MiniportDriverContext
    );

typedef
VOID
(*MINIPORT_REMOVE_DEVICE_HANDLER)(
    IN NDIS_HANDLE              MiniportAddDeviceContext
    );

typedef
NDIS_STATUS
(*MINIPORT_PNP_IRP_HANDLER)(
    IN NDIS_HANDLE              MiniportAddDeviceContext,
    IN PIRP                     Irp
    );


#define NDIS_MINIPORT_PNP_CHARACTERISTICS_REVISION_1       1

typedef struct _NDIS_MINIPORT_PNP_CHARACTERISTICS
{
     NDIS_OBJECT_HEADER                       Header;
     MINIPORT_ADD_DEVICE_HANDLER              MiniportAddDeviceHandler;
     MINIPORT_REMOVE_DEVICE_HANDLER           MiniportRemoveDeviceHandler;
     MINIPORT_PNP_IRP_HANDLER                 MiniportFilterResourceRequirementsHandler;
     MINIPORT_PNP_IRP_HANDLER                 MiniportStartDeviceHandler;
     ULONG                                    Flags;
     
} NDIS_MINIPORT_PNP_CHARACTERISTICS, *PNDIS_MINIPORT_PNP_CHARACTERISTICS;

#define NDIS_SIZEOF_MINIPORT_PNP_CHARACTERISTICS_REVISION_1      \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_MINIPORT_PNP_CHARACTERISTICS, Flags)

EXPORT
NDIS_STATUS
NdisMRegisterInterruptEx(
    IN  NDIS_HANDLE                                 MiniportAdapterHandle,
    IN  NDIS_HANDLE                                 MiniportInterruptContext,
    IN  PNDIS_MINIPORT_INTERRUPT_CHARACTERISTICS    MiniportInterruptCharacteristics,
    OUT PNDIS_HANDLE                                NdisInterruptHandle
    );

EXPORT
VOID
NdisMDeregisterInterruptEx(
    IN  NDIS_HANDLE                     NdisInterruptHandle
    );

EXPORT
BOOLEAN
NdisMSynchronizeWithInterruptEx(
    IN NDIS_HANDLE                      NdisInterruptHandle,
    IN ULONG                            MessageId,
    IN PVOID                            SynchronizeFunction,
    IN PVOID                            SynchronizeContext
    );

EXPORT
ULONG
NdisMQueueDpc(
    IN  NDIS_HANDLE     NdisInterruptHandle,
    IN  ULONG           MessageId,
    IN  ULONG           TargetProcessors,
    IN  PVOID           MiniportDpcContext
    );

#endif //  NDIS_SUPPORT_NDIS6

#if NDIS_LEGACY_MINIPORT
#if (((NDIS_MINIPORT_MAJOR_VERSION == 5) &&  (NDIS_MINIPORT_MINOR_VERSION == 1)) || NDIS_WRAPPER)
typedef struct _NDIS51_MINIPORT_CHARACTERISTICS
{
#ifdef __cplusplus
    NDIS50_MINIPORT_CHARACTERISTICS Ndis50Chars;
#else
    NDIS50_MINIPORT_CHARACTERISTICS;
#endif
    //
    // Extensions for NDIS 5.1
    //
    W_CANCEL_SEND_PACKETS_HANDLER   CancelSendPacketsHandler;
    W_PNP_EVENT_NOTIFY_HANDLER      PnPEventNotifyHandler;
    W_MINIPORT_SHUTDOWN_HANDLER     AdapterShutdownHandler;
    PVOID                           Reserved1;
    PVOID                           Reserved2;
    PVOID                           Reserved3;
    PVOID                           Reserved4;
} NDIS51_MINIPORT_CHARACTERISTICS;
#endif // (((NDIS_MINIPORT_MAJOR_VERSION == 5) &&  (NDIS_MINIPORT_MINOR_VERSION == 1)) || NDIS_WRAPPER)

typedef struct _NDIS_MINIPORT_INTERRUPT
{
    PKINTERRUPT                 InterruptObject;
    KSPIN_LOCK                  DpcCountLock;
    PVOID                       Reserved;
    W_ISR_HANDLER               MiniportIsr;
    W_HANDLE_INTERRUPT_HANDLER  MiniportDpc;
    KDPC                        InterruptDpc;
    PNDIS_MINIPORT_BLOCK        Miniport;

    UCHAR                       DpcCount;
    BOOLEAN                     Filler1;

    //
    // This is used to tell when all the Dpcs for the adapter are completed.
    //

    KEVENT                      DpcsCompletedEvent;

    BOOLEAN                     SharedInterrupt;
    BOOLEAN                     IsrRequested;

} NDIS_MINIPORT_INTERRUPT, *PNDIS_MINIPORT_INTERRUPT;
#endif // NDIS_LEGACY_MINIPORT

typedef struct _NDIS_MINIPORT_TIMER
{
    KTIMER                      Timer;
    KDPC                        Dpc;
    PNDIS_TIMER_FUNCTION        MiniportTimerFunction;
    PVOID                       MiniportTimerContext;
    PNDIS_MINIPORT_BLOCK        Miniport;
    struct _NDIS_MINIPORT_TIMER *NextTimer;
} NDIS_MINIPORT_TIMER, *PNDIS_MINIPORT_TIMER;

#if NDIS_LEGACY_MINIPORT
typedef
VOID
(*ETH_RCV_INDICATE_HANDLER)(
    IN  PETH_FILTER             Filter,
    IN  NDIS_HANDLE             MacReceiveContext,
    IN  PCHAR                   Address,
    IN  PVOID                   HeaderBuffer,
    IN  UINT                    HeaderBufferSize,
    IN  PVOID                   LookaheadBuffer,
    IN  UINT                    LookaheadBufferSize,
    IN  UINT                    PacketSize
    );

typedef
VOID
(*ETH_RCV_COMPLETE_HANDLER)(
    IN  PETH_FILTER             Filter
    );

typedef
VOID
(*TR_RCV_INDICATE_HANDLER)(
    IN  PTR_FILTER              Filter,
    IN  NDIS_HANDLE             MacReceiveContext,
    IN  PVOID                   HeaderBuffer,
    IN  UINT                    HeaderBufferSize,
    IN  PVOID                   LookaheadBuffer,
    IN  UINT                    LookaheadBufferSize,
    IN  UINT                    PacketSize
    );

typedef
VOID
(*TR_RCV_COMPLETE_HANDLER)(
    IN  PTR_FILTER              Filter
    );

typedef
VOID
(*WAN_RCV_HANDLER)(
    OUT PNDIS_STATUS            Status,
    IN NDIS_HANDLE              MiniportAdapterHandle,
    IN NDIS_HANDLE              NdisLinkContext,
    IN PUCHAR                   Packet,
    IN ULONG                    PacketSize
    );

typedef
VOID
(*WAN_RCV_COMPLETE_HANDLER)(
    IN NDIS_HANDLE              MiniportAdapterHandle,
    IN NDIS_HANDLE              NdisLinkContext
    );

typedef
VOID
(*NDIS_M_SEND_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  PNDIS_PACKET            Packet,
    IN  NDIS_STATUS             Status
    );

typedef
VOID
(*NDIS_WM_SEND_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  PVOID                   Packet,
    IN  NDIS_STATUS             Status
    );

typedef
VOID
(*NDIS_M_TD_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  PNDIS_PACKET            Packet,
    IN  NDIS_STATUS             Status,
    IN  UINT                    BytesTransferred
    );

typedef
VOID
(*NDIS_M_SEND_RESOURCES_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterHandle
    );

typedef
VOID
(*NDIS_M_STATUS_HANDLER)(
    IN  NDIS_HANDLE             MiniportHandle,
    IN  NDIS_STATUS             GeneralStatus,
    IN  PVOID                   StatusBuffer,
    IN  UINT                    StatusBufferSize
    );

typedef
VOID
(*NDIS_M_STS_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterHandle
    );

typedef
VOID
(*NDIS_M_REQ_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  NDIS_STATUS             Status
    );

typedef
VOID
(*NDIS_M_RESET_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  NDIS_STATUS             Status,
    IN  BOOLEAN                 AddressingReset
    );

typedef
BOOLEAN
(FASTCALL *NDIS_M_START_SENDS)(
    IN  PNDIS_MINIPORT_BLOCK    Miniport
    );

//
// Wrapper initialization and termination.
//

EXPORT
VOID
NdisInitializeWrapper(
    OUT PNDIS_HANDLE            NdisWrapperHandle,
    IN  PVOID                   SystemSpecific1,
    IN  PVOID                   SystemSpecific2,
    IN  PVOID                   SystemSpecific3
    );

EXPORT
VOID
NdisTerminateWrapper(
    IN  NDIS_HANDLE             NdisWrapperHandle,
    IN  PVOID                   SystemSpecific
    );

#endif // NDIS_LEGACY_MINIPORT

#if NDIS_SUPPORT_NDIS6

typedef
VOID
(*MINIPORT_PROCESS_SG_LIST_HANDLER)(
    IN  PDEVICE_OBJECT          pDO,
    IN  PVOID                   Reserved,
    IN  PSCATTER_GATHER_LIST    pSGL,
    IN  PVOID                   Context
    );

//
// NDIS DMA description structure
//

typedef
VOID
(*MINIPORT_ALLOCATE_SHARED_MEM_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  PVOID                   VirtualAddress,
    IN  PNDIS_PHYSICAL_ADDRESS  PhysicalAddress,
    IN  ULONG                   Length,
    IN  PVOID                   Context
    );

/*
NDIS_STATUS
NdisMAllocateSharedMemoryAsyncEx(
    IN  NDIS_HANDLE             MiniportDmaHandle,
    IN  ULONG                   Length,
    IN  BOOLEAN                 Cached,
    IN  PVOID                   Context
    );
*/
#define NdisMAllocateSharedMemoryAsyncEx(_H, _L, _C, _X) NdisMAllocateSharedMemoryAsync(_H, _L, _C, _X)
#endif // NDIS_SUPPORT_NDIS6



EXPORT
NDIS_STATUS
NdisMAllocateSharedMemoryAsync(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  ULONG                   Length,
    IN  BOOLEAN                 Cached,
    IN  PVOID                   Context
    );

//
// defintions for slave DMA
//

//
// DMA Channel information
//
typedef struct _NDIS_DMA_DESCRIPTION
{
    BOOLEAN     DemandMode;
    BOOLEAN     AutoInitialize;
    BOOLEAN     DmaChannelSpecified;
    DMA_WIDTH   DmaWidth;
    DMA_SPEED   DmaSpeed;
    ULONG       DmaPort;
    ULONG       DmaChannel;
} NDIS_DMA_DESCRIPTION, *PNDIS_DMA_DESCRIPTION;

//
// Internal structure representing an NDIS DMA channel
//
typedef struct _NDIS_DMA_BLOCK
{
    PVOID                   MapRegisterBase;
    KEVENT                  AllocationEvent;
    PVOID                   SystemAdapterObject;
    PVOID                   Miniport;
    BOOLEAN                 InProgress;
} NDIS_DMA_BLOCK, *PNDIS_DMA_BLOCK;


EXPORT
VOID
NdisSetupDmaTransfer(
    OUT PNDIS_STATUS            Status,
    IN  NDIS_HANDLE             NdisDmaHandle,
    IN  PNDIS_BUFFER            Buffer,
    IN  ULONG                   Offset,
    IN  ULONG                   Length,
    IN  BOOLEAN                 WriteToDevice
    );

EXPORT
VOID
NdisCompleteDmaTransfer(
    OUT PNDIS_STATUS            Status,
    IN  NDIS_HANDLE             NdisDmaHandle,
    IN  PNDIS_BUFFER            Buffer,
    IN  ULONG                   Offset,
    IN  ULONG                   Length,
    IN  BOOLEAN                 WriteToDevice
    );

EXPORT
NDIS_STATUS
NdisMRegisterDmaChannel(
    OUT PNDIS_HANDLE            MiniportDmaHandle,
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  UINT                    DmaChannel,
    IN  BOOLEAN                 Dma32BitAddresses,
    IN  PNDIS_DMA_DESCRIPTION   DmaDescription,
    IN  ULONG                   MaximumLength
    );


EXPORT
VOID
NdisMDeregisterDmaChannel(
    IN  NDIS_HANDLE             MiniportDmaHandle
    );

/*++
VOID
NdisMSetupDmaTransfer(
    OUT PNDIS_STATUS            Status,
    IN  NDIS_HANDLE             MiniportDmaHandle,
    IN  PNDIS_BUFFER            Buffer,
    IN  ULONG                   Offset,
    IN  ULONG                   Length,
    IN  BOOLEAN                 WriteToDevice
    )
--*/
#define NdisMSetupDmaTransfer(_S, _H, _B, _O, _L, _M_) \
        NdisSetupDmaTransfer(_S, _H, _B, _O, _L, _M_)

/*++
VOID
NdisMCompleteDmaTransfer(
    OUT PNDIS_STATUS            Status,
    IN  NDIS_HANDLE             MiniportDmaHandle,
    IN  PNDIS_BUFFER            Buffer,
    IN  ULONG                   Offset,
    IN  ULONG                   Length,
    IN  BOOLEAN                 WriteToDevice
    )
--*/
#define NdisMCompleteDmaTransfer(_S, _H, _B, _O, _L, _M_) \
        NdisCompleteDmaTransfer(_S, _H, _B, _O, _L, _M_)

EXPORT
ULONG
NdisMReadDmaCounter(
    IN  NDIS_HANDLE             MiniportDmaHandle
    );


//
// This API has been deprecated
//
DECLSPEC_DEPRECATED_DDK
EXPORT
VOID
NdisUpdateSharedMemory(
    IN  NDIS_HANDLE                     NdisAdapterHandle,
    IN  ULONG                           Length,
    IN  PVOID                           VirtualAddress,
    IN  NDIS_PHYSICAL_ADDRESS           PhysicalAddress
    );


#if NDIS_SUPPORT_NDIS6


//
// Flags used in NDIS_SG_DMA_DESCRIPTION
//

#define NDIS_SG_DMA_64_BIT_ADDRESS      0x00000001

//
// supported revision
//
#define NDIS_SG_DMA_DESCRIPTION_REVISION_1      1

typedef struct _NDIS_SG_DMA_DESCRIPTION
{
    NDIS_OBJECT_HEADER                              Header;
    ULONG                                           Flags;
    ULONG                                           MaximumPhysicalMapping;
    MINIPORT_PROCESS_SG_LIST_HANDLER                ProcessSGListHandler;
    MINIPORT_ALLOCATE_SHARED_MEM_COMPLETE_HANDLER   SharedMemAllocateCompleteHandler;
    ULONG                                           ScatterGatherListSize;
} NDIS_SG_DMA_DESCRIPTION, *PNDIS_SG_DMA_DESCRIPTION;

#define NDIS_SIZEOF_SG_DMA_DESCRIPTION_REVISION_1    \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_SG_DMA_DESCRIPTION, ScatterGatherListSize)

#define NDIS_MINIPORT_INIT_PARAMETERS_REVISION_1     1

typedef struct _NDIS_MINIPORT_INIT_PARAMETERS
{
    IN  NDIS_OBJECT_HEADER              Header;
    IN  ULONG                           Flags;
    IN  PNDIS_RESOURCE_LIST             AllocatedResources;
    IN  NDIS_HANDLE                     IMDeviceInstanceContext;
    IN  NDIS_HANDLE                     MiniportAddDeviceContext;
    IN  NET_IFINDEX                     IfIndex;
    IN  NET_LUID                        NetLuid;
    IN  PNDIS_PORT_AUTHENTICATION_PARAMETERS   DefaultPortAuthStates;
    IN  PNDIS_PCI_DEVICE_CUSTOM_PROPERTIES PciDeviceCustomProperties;
} NDIS_MINIPORT_INIT_PARAMETERS, *PNDIS_MINIPORT_INIT_PARAMETERS;

#define NDIS_SIZEOF_MINIPORT_INIT_PARAMETER_REVISION_1      \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_MINIPORT_INIT_PARAMETERS, PciDeviceCustomProperties)

#define NDIS_MINIPORT_RESTART_PARAMETERS_REVISION_1     1

typedef struct _NDIS_MINIPORT_RESTART_PARAMETERS
{
    NDIS_OBJECT_HEADER          Header;
    PNDIS_RESTART_ATTRIBUTES    RestartAttributes;
    ULONG                       Flags;
} NDIS_MINIPORT_RESTART_PARAMETERS, *PNDIS_MINIPORT_RESTART_PARAMETERS;

#define NDIS_SIZEOF_MINIPORT_RESTART_PARAMETERS_REVISION_1      \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_MINIPORT_RESTART_PARAMETERS, Flags)

#define NDIS_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES_REVISION_1     1

typedef struct _NDIS_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES
{
    NDIS_OBJECT_HEADER              Header;
    NDIS_HANDLE                     MiniportAdapterContext;
    ULONG                           AttributeFlags;
    UINT                            CheckForHangTimeInSeconds;
    NDIS_INTERFACE_TYPE             InterfaceType;
} NDIS_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES, *PNDIS_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES;

#define NDIS_SIZEOF_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES_REVISION_1    \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES, InterfaceType)

//
// flags used in NDIS_MINIPORT_ADAPTER_GENERAL_ATTRIBUTES->SupportedStatistics
//

#define NDIS_STATISTICS_XMIT_OK_SUPPORTED                       0x00000001
#define NDIS_STATISTICS_RCV_OK_SUPPORTED                        0x00000002 
#define NDIS_STATISTICS_XMIT_ERROR_SUPPORTED                    0x00000004 
#define NDIS_STATISTICS_RCV_ERROR_SUPPORTED                     0x00000008 
#define NDIS_STATISTICS_RCV_NO_BUFFER_SUPPORTED                 0x00000010 
#define NDIS_STATISTICS_DIRECTED_BYTES_XMIT_SUPPORTED           0x00000020 
#define NDIS_STATISTICS_DIRECTED_FRAMES_XMIT_SUPPORTED          0x00000040 
#define NDIS_STATISTICS_MULTICAST_BYTES_XMIT_SUPPORTED          0x00000080 
#define NDIS_STATISTICS_MULTICAST_FRAMES_XMIT_SUPPORTED         0x00000100 
#define NDIS_STATISTICS_BROADCAST_BYTES_XMIT_SUPPORTED          0x00000200 
#define NDIS_STATISTICS_BROADCAST_FRAMES_XMIT_SUPPORTED         0x00000400 
#define NDIS_STATISTICS_DIRECTED_BYTES_RCV_SUPPORTED            0x00000800 
#define NDIS_STATISTICS_DIRECTED_FRAMES_RCV_SUPPORTED           0x00001000 
#define NDIS_STATISTICS_MULTICAST_BYTES_RCV_SUPPORTED           0x00002000 
#define NDIS_STATISTICS_MULTICAST_FRAMES_RCV_SUPPORTED          0x00004000 
#define NDIS_STATISTICS_BROADCAST_BYTES_RCV_SUPPORTED           0x00008000 
#define NDIS_STATISTICS_BROADCAST_FRAMES_RCV_SUPPORTED          0x00010000 
#define NDIS_STATISTICS_RCV_CRC_ERROR_SUPPORTED                 0x00020000 
#define NDIS_STATISTICS_TRANSMIT_QUEUE_LENGTH_SUPPORTED         0x00040000 
#define NDIS_STATISTICS_BYTES_RCV_SUPPORTED                     0x00080000 
#define NDIS_STATISTICS_BYTES_XMIT_SUPPORTED                    0x00100000 
#define NDIS_STATISTICS_RCV_DISCARDS_SUPPORTED                  0x00200000 
#define NDIS_STATISTICS_GEN_STATISTICS_SUPPORTED                0x00400000
#define NDIS_STATISTICS_XMIT_DISCARDS_SUPPORTED                 0x08000000


#define NDIS_MINIPORT_ADD_DEVICE_REGISTRATION_ATTRIBUTES_REVISION_1     1

typedef struct _NDIS_MINIPORT_ADD_DEVICE_REGISTRATION_ATTRIBUTES
{
    NDIS_OBJECT_HEADER              Header;
    NDIS_HANDLE                     MiniportAddDeviceContext;
    ULONG                           Flags;
   
} NDIS_MINIPORT_ADD_DEVICE_REGISTRATION_ATTRIBUTES, 
  *PNDIS_MINIPORT_ADD_DEVICE_REGISTRATION_ATTRIBUTES;

#define NDIS_SIZEOF_MINIPORT_ADD_DEVICE_REGISTRATION_ATTRIBUTES_REVISION_1       \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_MINIPORT_ADD_DEVICE_REGISTRATION_ATTRIBUTES, Flags)

#define NDIS_MINIPORT_ADAPTER_GENERAL_ATTRIBUTES_REVISION_1     1

typedef struct _NDIS_MINIPORT_ADAPTER_GENERAL_ATTRIBUTES
{
    NDIS_OBJECT_HEADER              Header;
    ULONG                           Flags;
    NDIS_MEDIUM                     MediaType;
    NDIS_PHYSICAL_MEDIUM            PhysicalMediumType;
    ULONG                           MtuSize;
    ULONG64                         MaxXmitLinkSpeed;
    ULONG64                         XmitLinkSpeed;
    ULONG64                         MaxRcvLinkSpeed;
    ULONG64                         RcvLinkSpeed;
    NDIS_MEDIA_CONNECT_STATE        MediaConnectState;
    NDIS_MEDIA_DUPLEX_STATE         MediaDuplexState;
    ULONG                           LookaheadSize;
    PNDIS_PNP_CAPABILITIES          PowerManagementCapabilities;
    ULONG                           MacOptions;
    ULONG                           SupportedPacketFilters;
    ULONG                           MaxMulticastListSize;
    USHORT                          MacAddressLength;
    UCHAR                           PermanentMacAddress[NDIS_MAX_PHYS_ADDRESS_LENGTH];
    UCHAR                           CurrentMacAddress[NDIS_MAX_PHYS_ADDRESS_LENGTH];
    PNDIS_RECEIVE_SCALE_CAPABILITIES RecvScaleCapabilities;
    NET_IF_ACCESS_TYPE              AccessType; // NET_IF_ACCESS_BROADCAST for a typical ethernet adapter
    NET_IF_DIRECTION_TYPE           DirectionType; // NET_IF_DIRECTION_SENDRECEIVE for a typical ethernet adapter
    NET_IF_CONNECTION_TYPE          ConnectionType; // IF_CONNECTION_DEDICATED for a typical ethernet adapter
    NET_IFTYPE                      IfType; // IF_TYPE_ETHERNET_CSMACD for a typical ethernet adapter (regardless of speed)
    BOOLEAN                         IfConnectorPresent; // RFC 2665 TRUE if physical adapter
    ULONG                           SupportedStatistics; // use NDIS_STATISTICS_XXXX_SUPPORTED    
    ULONG                           SupportedPauseFunctions; // IEEE 802.3 37.2.1
    ULONG                           DataBackFillSize;
    ULONG                           ContextBackFillSize;
    PNDIS_OID                       SupportedOidList;
    ULONG                           SupportedOidListLength;
    ULONG                           AutoNegotiationFlags;
} NDIS_MINIPORT_ADAPTER_GENERAL_ATTRIBUTES, *PNDIS_MINIPORT_ADAPTER_GENERAL_ATTRIBUTES;

#define NDIS_SIZEOF_MINIPORT_ADAPTER_GENERAL_ATTRIBUTES_REVISION_1    \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_MINIPORT_ADAPTER_GENERAL_ATTRIBUTES, AutoNegotiationFlags)

//
// The miniport registers its offload capabilities through this
// structure
//
#define NDIS_MINIPORT_ADAPTER_OFFLOAD_ATTRIBUTES_REVISION_1     1
typedef struct _NDIS_MINIPORT_ADAPTER_OFFLOAD_ATTRIBUTES
{
    //
    // Header.Type = NDIS_OBJECT_TYPE_MINIPORT_ADAPTER_OFFLOAD_ATTRIBUTES;         
    // Header.Revision = NDIS_MINIPORT_ADAPTER_OFFLOAD_ATTRIBUTES_REVISION_1;
    // Header.Size = sizeof(NDIS_MINIPORT_ADAPTER_OFFLOAD_ATTRIBUTES);
    //
    
    NDIS_OBJECT_HEADER              Header;
    PNDIS_OFFLOAD                   DefaultOffloadConfiguration;
    PNDIS_OFFLOAD                   HardwareOffloadCapabilities;
    PNDIS_TCP_CONNECTION_OFFLOAD    DefaultTcpConnectionOffloadConfiguration;
    PNDIS_TCP_CONNECTION_OFFLOAD    TcpConnectionOffloadHardwareCapabilities;
    
} NDIS_MINIPORT_ADAPTER_OFFLOAD_ATTRIBUTES, *PNDIS_MINIPORT_ADAPTER_OFFLOAD_ATTRIBUTES;

#define NDIS_SIZEOF_MINIPORT_ADAPTER_OFFLOAD_ATTRIBUTES_REVISION_1     \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_MINIPORT_ADAPTER_OFFLOAD_ATTRIBUTES, TcpConnectionOffloadHardwareCapabilities)

#include <windot11.h>

#define NDIS_MINIPORT_ADAPTER_802_11_ATTRIBUTES_REVISION_1     1

typedef  struct _NDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES 
{
    NDIS_OBJECT_HEADER Header;
    
    ULONG                    OpModeCapability;
    ULONG                    NumOfTXBuffers;
    ULONG                    NumOfRXBuffers;
    BOOLEAN                  MultiDomainCapabilityImplemented;
    ULONG                    NumSupportedPhys;
#ifdef __midl
    [size_is(NumSupportedPhys)]
#endif
    PDOT11_PHY_ATTRIBUTES    SupportedPhyAttributes;
    
    // Attributes specific to the operation modes
    PDOT11_EXTSTA_ATTRIBUTES ExtSTAAttributes;

}NDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES,
  *PNDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES;

#define NDIS_SIZEOF_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES_REVISION_1     \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES, ExtSTAAttributes)

typedef union _NDIS_MINIPORT_ADAPTER_ATTRIBUTES
{
    NDIS_MINIPORT_ADD_DEVICE_REGISTRATION_ATTRIBUTES  AddDeviceRegistrationAttributes;
    NDIS_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES     RegistrationAttributes;
    NDIS_MINIPORT_ADAPTER_GENERAL_ATTRIBUTES          GeneralAttributes;
    NDIS_MINIPORT_ADAPTER_OFFLOAD_ATTRIBUTES          OffloadAttributes;
    NDIS_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES    Native_802_11_Attributes;

} NDIS_MINIPORT_ADAPTER_ATTRIBUTES, *PNDIS_MINIPORT_ADAPTER_ATTRIBUTES;


//
// flags used in NDIS_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES
//

#define NDIS_MINIPORT_ATTRIBUTES_HARDWARE_DEVICE            0x00000001
#define NDIS_MINIPORT_ATTRIBUTES_NDIS_WDM                   0x00000002
#define NDIS_MINIPORT_ATTRIBUTES_SURPRISE_REMOVE_OK         0x00000004
#define NDIS_MINIPORT_ATTRIBUTES_NOT_CO_NDIS                0x00000008
#define NDIS_MINIPORT_ATTRIBUTES_DO_NOT_BIND_TO_ALL_CO      0x00000010
#define NDIS_MINIPORT_ATTRIBUTES_NO_HALT_ON_SUSPEND         0x00000020
#define NDIS_MINIPORT_ATTRIBUTES_BUS_MASTER                 0x00000040
#define NDIS_MINIPORT_ATTRIBUTES_CONTROLS_DEFAULT_PORT      0x00000080

//
// NDIS 6.0 miniport's entry points
//

EXPORT
NDIS_STATUS
NdisMSetMiniportAttributes(
    IN  NDIS_HANDLE                             NdisMiniportHandle,
    IN  PNDIS_MINIPORT_ADAPTER_ATTRIBUTES       MiniportAttributes
    );

typedef
NDIS_STATUS
(*MINIPORT_INITIALIZE_HANDLER)(
    IN  NDIS_HANDLE                         NdisMiniportHandle,
    IN  NDIS_HANDLE                         MiniportDriverContext,
    IN  PNDIS_MINIPORT_INIT_PARAMETERS      MiniportInitParameters
    );

typedef enum _NDIS_HALT_ACTION
{
    NdisHaltDeviceDisabled,
    NdisHaltDeviceInstanceDeInitialized,
    NdisHaltDevicePoweredDown,
    NdisHaltDeviceSurpriseRemoved,
    NdisHaltDeviceFailed,
    NdisHaltDeviceInitializationFailed,
    NdisHaltDeviceStopped
} NDIS_HALT_ACTION, *PNDIS_HALT_ACTION;

typedef
VOID
(*MINIPORT_HALT_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  NDIS_HALT_ACTION        HaltAction
    );


#define NDIS_MINIPORT_PAUSE_PARAMETERS_REVISION_1     1

typedef struct _NDIS_MINIPORT_PAUSE_PARAMETERS
{
    NDIS_OBJECT_HEADER          Header;
    ULONG                       Flags;
    ULONG                       PauseReason;
} NDIS_MINIPORT_PAUSE_PARAMETERS, *PNDIS_MINIPORT_PAUSE_PARAMETERS;

#define NDIS_SIZEOF_MINIPORT_PAUSE_PARAMETERS_REVISION_1     \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_MINIPORT_PAUSE_PARAMETERS, Flags)

typedef
NDIS_STATUS
(*MINIPORT_PAUSE_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  PNDIS_MINIPORT_PAUSE_PARAMETERS   PauseParameters
    );

typedef
NDIS_STATUS
(*MINIPORT_RESTART_HANDLER)(
    IN  NDIS_HANDLE                             MiniportAdapterContext,
    IN  PNDIS_MINIPORT_RESTART_PARAMETERS       RestartParameters
    );

typedef
NDIS_STATUS
(*MINIPORT_OID_REQUEST_HANDLER) (
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  PNDIS_OID_REQUEST       OidRequest
    );

typedef
VOID
(*MINIPORT_DRIVER_UNLOAD) (
    IN PDRIVER_OBJECT           DriverObject
    );

//
// reasons for calling shutdown handler
//
typedef enum _NDIS_SHUTDOWN_ACTION {
    NdisShutdownPowerOff,
    NdisShutdownBugCheck
} NDIS_SHUTDOWN_ACTION, PNDIS_SHUTDOWN_ACTION;

typedef
VOID
(*MINIPORT_SHUTDOWN_HANDLER) (
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  NDIS_SHUTDOWN_ACTION    ShutdownAction
    );


#define NET_DEVICE_PNP_EVENT_REVISION_1         1

typedef struct _NET_DEVICE_PNP_EVENT
{
    NDIS_OBJECT_HEADER         Header;
    NDIS_PORT_NUMBER           PortNumber;
    NDIS_DEVICE_PNP_EVENT      DevicePnPEvent;
    PVOID                      InformationBuffer;
    ULONG                      InformationBufferLength;
    UCHAR                      NdisReserved[2 * sizeof(PVOID)];
    
} NET_DEVICE_PNP_EVENT, *PNET_DEVICE_PNP_EVENT;

#define NDIS_SIZEOF_NET_DEVICE_PNP_EVENT_REVISION_1     \
        RTL_SIZEOF_THROUGH_FIELD(NET_DEVICE_PNP_EVENT, NdisReserved)
    
typedef
VOID
(*MINIPORT_DEVICE_PNP_EVENT_NOTIFY_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  PNET_DEVICE_PNP_EVENT   NetDevicePnPEvent
    );

typedef
VOID
(*MINIPORT_CANCEL_SEND_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  PVOID                   CancelId
    );


typedef
BOOLEAN
(*MINIPORT_CHECK_FOR_HANG_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext
    );

typedef
NDIS_STATUS
(*MINIPORT_RESET_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    OUT PBOOLEAN                AddressingReset
    );

typedef
VOID
(*MINIPORT_CANCEL_OID_REQUEST_HANDLER)(
    IN NDIS_HANDLE              MiniportAdapterContext,
    IN PVOID                    RequestId
    );

typedef
VOID
(*MINIPORT_SEND_NET_BUFFER_LISTS_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  PNET_BUFFER_LIST        NetBufferList,
    IN  NDIS_PORT_NUMBER        PortNumber,
    IN  ULONG                   SendFlags
    );

typedef
VOID
(*MINIPORT_RETURN_NET_BUFFER_LISTS_HANDLER)(
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  PNET_BUFFER_LIST        NetBufferLists,
    IN  ULONG                   ReturnFlags
    );


//
// flags used in Flags field of NDIS60_MINIPORT_CHARACTERISTICS
//
#define NDIS_INTERMEDIATE_DRIVER                0x00000001
#define NDIS_WDM_DRIVER                         0x00000002


#define NDIS_MINIPORT_DRIVER_CHARACTERISTICS_REVISION_1     1

typedef struct _NDIS_MINIPORT_DRIVER_CHARACTERISTICS
{
    NDIS_OBJECT_HEADER                          Header;
    UCHAR                                       MajorNdisVersion;
    UCHAR                                       MinorNdisVersion;
    UCHAR                                       MajorDriverVersion;
    UCHAR                                       MinorDriverVersion;
    ULONG                                       Flags;
    SET_OPTIONS_HANDLER                         SetOptionsHandler;
    MINIPORT_INITIALIZE_HANDLER                 InitializeHandlerEx;
    MINIPORT_HALT_HANDLER                       HaltHandlerEx;
    MINIPORT_DRIVER_UNLOAD                      UnloadHandler;
    MINIPORT_PAUSE_HANDLER                      PauseHandler;
    MINIPORT_RESTART_HANDLER                    RestartHandler;
    MINIPORT_OID_REQUEST_HANDLER                OidRequestHandler;
    MINIPORT_SEND_NET_BUFFER_LISTS_HANDLER      SendNetBufferListsHandler;
    MINIPORT_RETURN_NET_BUFFER_LISTS_HANDLER    ReturnNetBufferListsHandler;
    MINIPORT_CANCEL_SEND_HANDLER                CancelSendHandler;
    MINIPORT_CHECK_FOR_HANG_HANDLER             CheckForHangHandlerEx;
    MINIPORT_RESET_HANDLER                      ResetHandlerEx;
    MINIPORT_DEVICE_PNP_EVENT_NOTIFY_HANDLER    DevicePnPEventNotifyHandler;
    MINIPORT_SHUTDOWN_HANDLER                   ShutdownHandlerEx;
    MINIPORT_CANCEL_OID_REQUEST_HANDLER         CancelOidRequestHandler;
} NDIS_MINIPORT_DRIVER_CHARACTERISTICS, *PNDIS_MINIPORT_DRIVER_CHARACTERISTICS;

#define NDIS_SIZEOF_MINIPORT_DRIVER_CHARACTERISTICS_REVISION_1      \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_MINIPORT_DRIVER_CHARACTERISTICS, CancelOidRequestHandler)

//
// CO NDIS 6.0 handlers
//

#define NDIS_MINIPORT_CO_CHARACTERISTICS_REVISION_1      1

typedef struct _NDIS_MINIPORT_CO_CHARACTERISTICS
{
    NDIS_OBJECT_HEADER                          Header;     // Header.Type = NDIS_OBJECT_TYPE_PROTOCOL_CO_CHARACTERISTICS
    ULONG                                       Flags;
    W_CO_CREATE_VC_HANDLER                      CoCreateVcHandler;
    W_CO_DELETE_VC_HANDLER                      CoDeleteVcHandler;
    W_CO_ACTIVATE_VC_HANDLER                    CoActivateVcHandler;
    W_CO_DEACTIVATE_VC_HANDLER                  CoDeactivateVcHandler;
    W_CO_SEND_NET_BUFFER_LISTS_HANDLER          CoSendNetBufferListsHandler;
    W_CO_OID_REQUEST_HANDLER                    CoOidRequestHandler;
    
} NDIS_MINIPORT_CO_CHARACTERISTICS, *PNDIS_MINIPORT_CO_CHARACTERISTICS;

#define NDIS_SIZEOF_MINIPORT_CO_CHARACTERISTICS_REVISION_1      \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_MINIPORT_CO_CHARACTERISTICS, CoOidRequestHandler)

EXPORT
VOID
NdisMIndicateStatusEx(
    IN NDIS_HANDLE              MiniportAdapterHandle,
    IN PNDIS_STATUS_INDICATION  StatusIndication
    );

EXPORT
VOID
NdisMCoOidRequestComplete(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  NDIS_HANDLE             NdisMiniportVcHandle,
    IN  PNDIS_OID_REQUEST       Request,
    IN  NDIS_STATUS             Status
    );


EXPORT
NDIS_STATUS
NdisMCmOidRequest(
    IN  NDIS_HANDLE             NdisAfHandle,
    IN  NDIS_HANDLE             NdisVcHandle    OPTIONAL,
    IN  NDIS_HANDLE             NdisPartyHandle OPTIONAL,
    IN OUT PNDIS_OID_REQUEST    NdisRequest
    );


EXPORT
VOID
NdisMCoIndicateStatusEx(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  NDIS_HANDLE             NdisVcHandle    OPTIONAL,
    IN  PNDIS_STATUS_INDICATION StatusIndication
    );

#endif // NDIS_SUPPORT_NDIS6


//
// Do not change the structure below !!!
//
typedef struct
{
    union
    {
        PETH_FILTER             EthDB;
        PNULL_FILTER            NullDB;             // Default Filter
    };
    PTR_FILTER                  TrDB;

    PVOID                       YYYDB;

    PVOID                       XXXDB;
} FILTERDBS, *PFILTERDBS;

#if NDIS_LEGACY_MINIPORT
typedef
VOID
(*FILTER_PACKET_INDICATION_HANDLER)(
    IN  NDIS_HANDLE             Miniport,
    IN  PPNDIS_PACKET           PacketArray,
    IN  UINT                    NumberOfPackets
    );

#endif // NDIS_LEGACY_MINIPORT

#if !NDIS_WRAPPER
//
// one of these per mini-port registered on a Driver
//

#if NDIS_LEGACY_MINIPORT

struct _NDIS_MINIPORT_BLOCK
{
    NDIS_OBJECT_HEADER          Header;
    PNDIS_MINIPORT_BLOCK        NextMiniport;       // used by driver's MiniportQueue
    PNDIS_MINIPORT_BLOCK        BaseMiniport;
    NDIS_HANDLE                 MiniportAdapterContext; // context when calling mini-port functions
    UNICODE_STRING              Reserved4;
    PVOID                       Reserved10;
    NDIS_HANDLE                 OpenQueue;          // queue of opens for this mini-port
    REFERENCE                   ShortRef;           // contains spinlock for OpenQueue

    NDIS_HANDLE                 Reserved12;

    UCHAR                       Padding1;           // DO NOT REMOVE OR NDIS WILL BREAK!!!

    //
    // Synchronization stuff.
    //
    // The boolean is used to lock out several DPCs from running at the same time.
    //
    UCHAR                       LockAcquired;       // EXPOSED via macros. Do not move

    UCHAR                       PmodeOpens;         // Count of opens which turned on pmode/all_local

    //
    //  This is the processor number that the miniport's
    //  interrupt DPC and timers are running on.
    //
    UCHAR                       AssignedProcessor;

    KSPIN_LOCK                  Lock;

    PNDIS_REQUEST               MediaRequest;

    PVOID                       Interrupt;
    
    ULONG                       Flags;              // Flags to keep track of the miniport's state.
    ULONG                       PnPFlags;

    //
    // Send information
    //
    LIST_ENTRY                  PacketList;
    PNDIS_PACKET                FirstPendingPacket; // This is head of the queue of packets
                                                    // waiting to be sent to miniport.
    PNDIS_PACKET                ReturnPacketsQueue;

    //
    // Space used for temp. use during request processing
    //
    ULONG                       RequestBuffer;
    PVOID                       SetMCastBuffer;

    PNDIS_MINIPORT_BLOCK        PrimaryMiniport;
    PVOID                       Reserved11;

    //
    // context to pass to bus driver when reading or writing config space
    //
    PVOID                       BusDataContext;
    ULONG                       Reserved3;

    //
    // Resource information
    //
    PCM_RESOURCE_LIST           Resources;

    //
    // Watch-dog timer
    //
    NDIS_TIMER                  WakeUpDpcTimer;

    //
    // Needed for PnP. Upcased version. The buffer is allocated as part of the
    // NDIS_MINIPORT_BLOCK itself.
    //
    // Note:
    // the following two fields should be explicitly UNICODE_STRING because
    // under Win9x the NDIS_STRING is an ANSI_STRING
    //
    UNICODE_STRING              Reserved20;
    UNICODE_STRING              SymbolicLinkName;

    //
    // Check for hang stuff
    //
    ULONG                       CheckForHangSeconds;
    USHORT                      CFHangTicks;
    USHORT                      CFHangCurrentTick;

    //
    // Reset information
    //
    NDIS_STATUS                 ResetStatus;
    NDIS_HANDLE                 ResetOpen;

    //
    // Holds media specific information.
    //
#ifdef __cplusplus
    FILTERDBS                   FilterDbs;          // EXPOSED via macros. Do not move
#else
    FILTERDBS;                                      // EXPOSED via macros. Do not move
#endif

    FILTER_PACKET_INDICATION_HANDLER PacketIndicateHandler;
    NDIS_M_SEND_COMPLETE_HANDLER    SendCompleteHandler;
    NDIS_M_SEND_RESOURCES_HANDLER   SendResourcesHandler;
    NDIS_M_RESET_COMPLETE_HANDLER   ResetCompleteHandler;

    NDIS_MEDIUM                 MediaType;

    //
    // contains mini-port information
    //
    ULONG                       BusNumber;
    NDIS_INTERFACE_TYPE         BusType;
    NDIS_INTERFACE_TYPE         AdapterType;

    PDEVICE_OBJECT              Reserved6;
    PDEVICE_OBJECT              Reserved7;
    PDEVICE_OBJECT              Reserved8;


    PVOID                       MiniportSGDmaBlock;

    //
    // List of registered address families. Valid for the call-manager, Null for the client
    //
    PNDIS_AF_LIST               CallMgrAfList;

    PVOID                       MiniportThread;
    PVOID                       SetInfoBuf;
    USHORT                      SetInfoBufLen;
    USHORT                      MaxSendPackets;

    //
    //  Status code that is returned from the fake handlers.
    //
    NDIS_STATUS                 FakeStatus;

    PVOID                       LockHandler;        // For the filter lock

    PUNICODE_STRING             Reserved9;

    PVOID                       Reserved21;

    UINT                        MacOptions;

    //
    // RequestInformation
    //
    PNDIS_REQUEST               PendingRequest;
    UINT                        MaximumLongAddresses;
    UINT                        MaximumShortAddresses;
    UINT                        CurrentLookahead;
    UINT                        MaximumLookahead;

    //
    //  For efficiency
    //
    ULONG_PTR                   Reserved1;
    W_DISABLE_INTERRUPT_HANDLER DisableInterruptHandler;
    W_ENABLE_INTERRUPT_HANDLER  EnableInterruptHandler;
    W_SEND_PACKETS_HANDLER      SendPacketsHandler;
    NDIS_M_START_SENDS          DeferredSendHandler;

    //
    // The following cannot be unionized.
    //
    ETH_RCV_INDICATE_HANDLER    EthRxIndicateHandler;   // EXPOSED via macros. Do not move
    TR_RCV_INDICATE_HANDLER     TrRxIndicateHandler;    // EXPOSED via macros. Do not move
    PVOID                       Reserved2;

    ETH_RCV_COMPLETE_HANDLER    EthRxCompleteHandler;   // EXPOSED via macros. Do not move
    TR_RCV_COMPLETE_HANDLER     TrRxCompleteHandler;    // EXPOSED via macros. Do not move
    PVOID                       Reserved22;

    NDIS_M_STATUS_HANDLER       StatusHandler;          // EXPOSED via macros. Do not move
    NDIS_M_STS_COMPLETE_HANDLER StatusCompleteHandler;  // EXPOSED via macros. Do not move
    NDIS_M_TD_COMPLETE_HANDLER  TDCompleteHandler;      // EXPOSED via macros. Do not move
    NDIS_M_REQ_COMPLETE_HANDLER QueryCompleteHandler;   // EXPOSED via macros. Do not move
    NDIS_M_REQ_COMPLETE_HANDLER SetCompleteHandler;     // EXPOSED via macros. Do not move

    NDIS_WM_SEND_COMPLETE_HANDLER WanSendCompleteHandler;// EXPOSED via macros. Do not move
    WAN_RCV_HANDLER             WanRcvHandler;          // EXPOSED via macros. Do not move
    WAN_RCV_COMPLETE_HANDLER    WanRcvCompleteHandler;  // EXPOSED via macros. Do not move

    /********************************************************************************************/
    /****************                                                                  **********/
    /**************** STUFF ABOVE IS POTENTIALLY ACCESSED BY MACROS. ADD STUFF BELOW   **********/
    /**************** SEVERE POSSIBILITY OF BREAKING SOMETHING IF STUFF ABOVE IS MOVED **********/
    /****************                                                                  **********/
    /********************************************************************************************/

};
#endif // NDIS_LEGACY_MINIPORT

#endif // NDIS_WRAPPER not defined


#if NDIS_LEGACY_MINIPORT

#ifdef NDIS51_MINIPORT
typedef struct _NDIS51_MINIPORT_CHARACTERISTICS NDIS_MINIPORT_CHARACTERISTICS;
#else
#ifdef NDIS50_MINIPORT
typedef struct _NDIS50_MINIPORT_CHARACTERISTICS NDIS_MINIPORT_CHARACTERISTICS;
#else
#ifdef NDIS40_MINIPORT
typedef struct _NDIS40_MINIPORT_CHARACTERISTICS NDIS_MINIPORT_CHARACTERISTICS;
#else
typedef struct _NDIS30_MINIPORT_CHARACTERISTICS NDIS_MINIPORT_CHARACTERISTICS;
#endif
#endif
#endif

typedef NDIS_MINIPORT_CHARACTERISTICS * PNDIS_MINIPORT_CHARACTERISTICS;
typedef NDIS_MINIPORT_CHARACTERISTICS   NDIS_WAN_MINIPORT_CHARACTERISTICS;
typedef NDIS_WAN_MINIPORT_CHARACTERISTICS * PNDIS_MINIPORT_CHARACTERISTICS;


//
// Routines for intermediate miniport drivers. NDIS 6 IM drivers
// use the same registration deregistration APIs as regular miniports
//

EXPORT
NDIS_STATUS
NdisIMRegisterLayeredMiniport(
    IN  NDIS_HANDLE                     NdisWrapperHandle,
    IN  PNDIS_MINIPORT_CHARACTERISTICS  MiniportCharacteristics,
    IN  UINT                            CharacteristicsLength,
    OUT PNDIS_HANDLE                    DriverHandle
    );

EXPORT
VOID
NdisIMDeregisterLayeredMiniport(
    IN  NDIS_HANDLE         DriverHandle
    );

EXPORT
NDIS_STATUS
NdisMRegisterDevice(
    IN  NDIS_HANDLE             NdisWrapperHandle,
    IN  PNDIS_STRING            DeviceName,
    IN  PNDIS_STRING            SymbolicName,
    IN  PDRIVER_DISPATCH        MajorFunctions[],
    OUT PDEVICE_OBJECT      *   pDeviceObject,
    OUT NDIS_HANDLE         *   NdisDeviceHandle
    );

EXPORT
NDIS_STATUS
NdisMDeregisterDevice(
    IN  NDIS_HANDLE             NdisDeviceHandle
    );

EXPORT
VOID
NdisMRegisterUnloadHandler(
    IN  NDIS_HANDLE             NdisWrapperHandle,
    IN  PDRIVER_UNLOAD          UnloadHandler
    );

#endif // NDIS_LEGACY_MINIPORT


EXPORT
VOID
NdisIMAssociateMiniport(
    IN  NDIS_HANDLE         DriverHandle,
    IN  NDIS_HANDLE         ProtocolHandle
    );

//
// Operating System Requests
//
typedef UCHAR   NDIS_DMA_SIZE;

#define NDIS_DMA_24BITS             ((NDIS_DMA_SIZE)0)
#define NDIS_DMA_32BITS             ((NDIS_DMA_SIZE)1)
#define NDIS_DMA_64BITS             ((NDIS_DMA_SIZE)2)

#if NDIS_LEGACY_MINIPORT
EXPORT
NDIS_STATUS
NdisMAllocateMapRegisters(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  UINT                    DmaChannel,
    IN  NDIS_DMA_SIZE           DmaSize,
    IN  ULONG                   BaseMapRegistersNeeded,
    IN  ULONG                   MaximumPhysicalMapping
    );

EXPORT
VOID
NdisMFreeMapRegisters(
    IN  NDIS_HANDLE             MiniportAdapterHandle
    );

EXPORT
NDIS_STATUS
NdisMInitializeScatterGatherDma(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  BOOLEAN                 Dma64BitAddresses,
    IN  ULONG                   MaximumPhysicalMapping
    );

#endif // NDIS_LEGACY_MINIPORT

EXPORT
NDIS_STATUS
NdisMRegisterIoPortRange(
    OUT PVOID *                 PortOffset,
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  UINT                    InitialPort,
    IN  UINT                    NumberOfPorts
    );

EXPORT
VOID
NdisMDeregisterIoPortRange(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  UINT                    InitialPort,
    IN  UINT                    NumberOfPorts,
    IN  PVOID                   PortOffset
    );

EXPORT
NDIS_STATUS
NdisMMapIoSpace(
    OUT PVOID *                 VirtualAddress,
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  NDIS_PHYSICAL_ADDRESS   PhysicalAddress,
    IN  UINT                    Length
    );

EXPORT
VOID
NdisMUnmapIoSpace(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  PVOID                   VirtualAddress,
    IN  UINT                    Length
    );

#if NDIS_LEGACY_MINIPORT

EXPORT
NDIS_STATUS
NdisMRegisterInterrupt(
    OUT PNDIS_MINIPORT_INTERRUPT Interrupt,
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  UINT                    InterruptVector,
    IN  UINT                    InterruptLevel,
    IN  BOOLEAN                 RequestIsr,
    IN  BOOLEAN                 SharedInterrupt,
    IN  NDIS_INTERRUPT_MODE     InterruptMode
    );

EXPORT
VOID
NdisMDeregisterInterrupt(
    IN  PNDIS_MINIPORT_INTERRUPT Interrupt
    );

EXPORT
BOOLEAN
NdisMSynchronizeWithInterrupt(
    IN  PNDIS_MINIPORT_INTERRUPT Interrupt,
    IN  PVOID                   SynchronizeFunction,
    IN  PVOID                   SynchronizeContext
    );

EXPORT
VOID
NdisMQueryAdapterResources(
    OUT PNDIS_STATUS            Status,
    IN  NDIS_HANDLE             WrapperConfigurationContext,
    OUT PNDIS_RESOURCE_LIST     ResourceList,
    IN  OUT PUINT               BufferSize
    );

//
// Physical Mapping
//
EXPORT
VOID
NdisMStartBufferPhysicalMapping(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  PNDIS_BUFFER            Buffer,
    IN  ULONG                   PhysicalMapRegister,
    IN  BOOLEAN                 WriteToDevice,
    OUT PNDIS_PHYSICAL_ADDRESS_UNIT PhysicalAddressArray,
    OUT PUINT                   ArraySize
    );

EXPORT
VOID
NdisMCompleteBufferPhysicalMapping(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  PNDIS_BUFFER            Buffer,
    IN  ULONG                   PhysicalMapRegister
    );

/*++
//
// This API has been deprecated
//
VOID
NdisMUpdateSharedMemory(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  ULONG                   Length,
    IN  PVOID                   VirtualAddress,
    IN  NDIS_PHYSICAL_ADDRESS   PhysicalAddress
    );
*/
#define NdisMUpdateSharedMemory(_H, _L, _V, _P)
#pragma deprecated(NdisMUpdateSharedMemory)


#endif // NDIS_LEGACY_MINIPORT

//
// Timers
//
// VOID
// NdisMSetTimer(
//  IN  PNDIS_MINIPORT_TIMER    Timer,
//  IN  UINT                    MillisecondsToDelay
//  );
#define NdisMSetTimer(_Timer, _Delay)   NdisSetTimer((PNDIS_TIMER)_Timer, _Delay)

EXPORT
VOID
NdisMSetPeriodicTimer(
    IN  PNDIS_MINIPORT_TIMER     Timer,
    IN  UINT                     MillisecondPeriod
    );

EXPORT
VOID
NdisMInitializeTimer(
    IN  OUT PNDIS_MINIPORT_TIMER Timer,
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  PNDIS_TIMER_FUNCTION    TimerFunction,
    IN  PVOID                   FunctionContext
    );

EXPORT
VOID
NdisMCancelTimer(
    IN  PNDIS_MINIPORT_TIMER    Timer,
    OUT PBOOLEAN                TimerCancelled
    );

EXPORT
VOID
NdisMSleep(
    IN  ULONG                   MicrosecondsToSleep
    );

EXPORT
ULONG
NdisMGetDmaAlignment(
    IN  NDIS_HANDLE MiniportAdapterHandle
    );

//
// Shared memory
//
EXPORT
VOID
NdisMAllocateSharedMemory(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  ULONG                   Length,
    IN  BOOLEAN                 Cached,
    OUT PVOID *                 VirtualAddress,
    OUT PNDIS_PHYSICAL_ADDRESS  PhysicalAddress
    );



EXPORT
VOID
NdisMFreeSharedMemory(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  ULONG                   Length,
    IN  BOOLEAN                 Cached,
    IN  PVOID                   VirtualAddress,
    IN  NDIS_PHYSICAL_ADDRESS   PhysicalAddress
    );

#if NDIS_LEGACY_MINIPORT

//
// Requests Used by Miniport Drivers
//
#define NdisMInitializeWrapper(_a,_b,_c,_d) NdisInitializeWrapper((_a),(_b),(_c),(_d))


EXPORT
NDIS_STATUS
NdisMRegisterMiniport(
    IN  NDIS_HANDLE                     NdisWrapperHandle,
    IN  PNDIS_MINIPORT_CHARACTERISTICS  MiniportCharacteristics,
    IN  UINT                            CharacteristicsLength
    );


// EXPORT
// NDIS_STATUS
// NdisIMInitializeDeviceInstance(
//  IN  NDIS_HANDLE             DriverHandle,
//  IN  PNDIS_STRING            DriverInstance
//  );
#define NdisIMInitializeDeviceInstance(_H_, _I_)    \
                                NdisIMInitializeDeviceInstanceEx(_H_, _I_, NULL)
#endif // NDIS_LEGACY_MINIPORT

EXPORT
NDIS_STATUS
NdisIMInitializeDeviceInstanceEx(
    IN  NDIS_HANDLE             DriverHandle,
    IN  PNDIS_STRING            DriverInstance,
    IN  NDIS_HANDLE             DeviceContext   OPTIONAL
    );

EXPORT
NDIS_STATUS
NdisIMCancelInitializeDeviceInstance(
    IN  NDIS_HANDLE             DriverHandle,
    IN  PNDIS_STRING            DeviceInstance
    );

#if NDIS_LEGACY_MINIPORT
EXPORT
NDIS_HANDLE
NdisIMGetDeviceContext(
    IN  NDIS_HANDLE             MiniportAdapterHandle
    );

#endif // NDIS_LEGACY_MINIPORT

EXPORT
NDIS_HANDLE
NdisIMGetBindingContext(
    IN  NDIS_HANDLE             NdisBindingHandle
    );

EXPORT
NDIS_STATUS
NdisIMDeInitializeDeviceInstance(
    IN  NDIS_HANDLE             NdisMiniportHandle
    );


#if NDIS_LEGACY_MINIPORT

EXPORT
VOID
NdisIMCopySendPerPacketInfo(
    OUT PNDIS_PACKET            DstPacket,
    IN  PNDIS_PACKET            SrcPacket
    );

EXPORT
VOID
NdisIMCopySendCompletePerPacketInfo(
    OUT PNDIS_PACKET            DstPacket,
    IN  PNDIS_PACKET            SrcPacket
    );

// EXPORT
// VOID
// NdisMSetAttributes(
//  IN  NDIS_HANDLE             MiniportAdapterHandle,
//  IN  NDIS_HANDLE             MiniportAdapterContext,
//  IN  BOOLEAN                 BusMaster,
//  IN  NDIS_INTERFACE_TYPE     AdapterType
//  );
#define NdisMSetAttributes(_H_, _C_, _M_, _T_)                                      \
                        NdisMSetAttributesEx(_H_,                                   \
                                             _C_,                                   \
                                             0,                                     \
                                             (_M_) ? NDIS_ATTRIBUTE_BUS_MASTER : 0, \
                                             _T_)                                   \


EXPORT
VOID
NdisMSetAttributesEx(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  NDIS_HANDLE             MiniportAdapterContext,
    IN  UINT                    CheckForHangTimeInSeconds OPTIONAL,
    IN  ULONG                   AttributeFlags,
    IN  NDIS_INTERFACE_TYPE     AdapterType OPTIONAL
    );

#define NDIS_ATTRIBUTE_IGNORE_PACKET_TIMEOUT        0x00000001
#define NDIS_ATTRIBUTE_IGNORE_REQUEST_TIMEOUT       0x00000002
#define NDIS_ATTRIBUTE_IGNORE_TOKEN_RING_ERRORS     0x00000004
#define NDIS_ATTRIBUTE_BUS_MASTER                   0x00000008
#define NDIS_ATTRIBUTE_INTERMEDIATE_DRIVER          0x00000010
#define NDIS_ATTRIBUTE_DESERIALIZE                  0x00000020
#define NDIS_ATTRIBUTE_NO_HALT_ON_SUSPEND           0x00000040
#define NDIS_ATTRIBUTE_SURPRISE_REMOVE_OK           0x00000080
#define NDIS_ATTRIBUTE_NOT_CO_NDIS                  0x00000100
#define NDIS_ATTRIBUTE_USES_SAFE_BUFFER_APIS        0x00000200
#define NDIS_ATTRIBUTE_DO_NOT_BIND_TO_ALL_CO        0x00000400
#define NDIS_ATTRIBUTE_MINIPORT_PADS_SHORT_PACKETS  0x00000800

EXPORT
NDIS_STATUS
NdisMSetMiniportSecondary(
    IN  NDIS_HANDLE             MiniportHandle,
    IN  NDIS_HANDLE             PrimaryMiniportHandle
    );

EXPORT
NDIS_STATUS
NdisMPromoteMiniport(
    IN  NDIS_HANDLE             MiniportHandle
    );

#endif // NDIS_LEGACY_MINIPORT

EXPORT
NDIS_STATUS
NdisMRemoveMiniport(
    IN  NDIS_HANDLE             MiniportHandle
    );


#if NDIS_LEGACY_MINIPORT

#define NdisMSendComplete(_M, _P, _S)   (*((PNDIS_MINIPORT_BLOCK)(_M))->SendCompleteHandler)(_M, _P, _S)

#define NdisMSendResourcesAvailable(_M) (*((PNDIS_MINIPORT_BLOCK)(_M))->SendResourcesHandler)(_M)

#if (!NDIS_WRAPPER)
#if (!NDIS_SUPPORT_NDIS6)

#define NdisMResetComplete(_M, _S, _A)  (*((PNDIS_MINIPORT_BLOCK)(_M))->ResetCompleteHandler)(_M, _S, _A)
#endif // NDIS_SUPPORT_NDIS6
#endif // !NDIS_WRAPPER

#define NdisMTransferDataComplete(_M, _P, _S, _B)   \
                                        (*((PNDIS_MINIPORT_BLOCK)(_M))->TDCompleteHandler)(_M, _P, _S, _B)

/*++

VOID
NdisMWanSendComplete(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  PVOID                   Packet,
    IN  NDIS_STATUS             Status
    );

--*/

#define NdisMWanSendComplete(_M_, _P_, _S_)                                             \
                (*((PNDIS_MINIPORT_BLOCK)(_M_))->WanSendCompleteHandler)(_M_, _P_, _S_)

#define NdisMQueryInformationComplete(_M, _S)   \
                                        (*((PNDIS_MINIPORT_BLOCK)(_M))->QueryCompleteHandler)(_M, _S)

#define NdisMSetInformationComplete(_M, _S) \
                                        (*((PNDIS_MINIPORT_BLOCK)(_M))->SetCompleteHandler)(_M, _S)

/*++

VOID
NdisMIndicateReceivePacket(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  PPNDIS_PACKET           ReceivedPackets,
    IN  UINT                    NumberOfPackets
    );

--*/
#define NdisMIndicateReceivePacket(_H, _P, _N)                                  \
{                                                                               \
    (*((PNDIS_MINIPORT_BLOCK)(_H))->PacketIndicateHandler)(                     \
                        _H,                                                     \
                        _P,                                                     \
                        _N);                                                    \
}

/*++

VOID
NdisMWanIndicateReceive(
    OUT PNDIS_STATUS            Status,
    IN NDIS_HANDLE              MiniportAdapterHandle,
    IN NDIS_HANDLE              NdisLinkContext,
    IN PUCHAR                   Packet,
    IN ULONG                    PacketSize
    );

--*/

#define NdisMWanIndicateReceive(_S_, _M_, _C_, _P_, _Z_)                        \
                (*((PNDIS_MINIPORT_BLOCK)(_M_))->WanRcvHandler)(_S_, _M_, _C_, _P_, _Z_)

/*++

VOID
NdisMWanIndicateReceiveComplete(
    IN NDIS_HANDLE              MiniportAdapterHandle,
    IN NDIS_HANDLE              NdisLinkContext
    );

--*/

#define NdisMWanIndicateReceiveComplete(_M_, _C_)                                   \
                (*((PNDIS_MINIPORT_BLOCK)(_M_))->WanRcvCompleteHandler)(_M_, _C_)

/*++

VOID
NdisMEthIndicateReceive(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  NDIS_HANDLE             MiniportReceiveContext,
    IN  PVOID                   HeaderBuffer,
    IN  UINT                    HeaderBufferSize,
    IN  PVOID                   LookaheadBuffer,
    IN  UINT                    LookaheadBufferSize,
    IN  UINT                    PacketSize
    )

--*/
#ifdef __cplusplus

#define NdisMEthIndicateReceive( _H, _C, _B, _SZ, _L, _LSZ, _PSZ)               \
{                                                                               \
    (*((PNDIS_MINIPORT_BLOCK)(_H))->EthRxIndicateHandler)(                      \
        ((PNDIS_MINIPORT_BLOCK)(_H))->FilterDbs.EthDB,                          \
        _C,                                                                     \
        _B,                                                                     \
        _B,                                                                     \
        _SZ,                                                                    \
        _L,                                                                     \
        _LSZ,                                                                   \
        _PSZ                                                                    \
        );                                                                      \
}

#else

#define NdisMEthIndicateReceive( _H, _C, _B, _SZ, _L, _LSZ, _PSZ)               \
{                                                                               \
    (*((PNDIS_MINIPORT_BLOCK)(_H))->EthRxIndicateHandler)(                      \
        ((PNDIS_MINIPORT_BLOCK)(_H))->EthDB,                                    \
        _C,                                                                     \
        _B,                                                                     \
        _B,                                                                     \
        _SZ,                                                                    \
        _L,                                                                     \
        _LSZ,                                                                   \
        _PSZ                                                                    \
        );                                                                      \
}

#endif

/*++

VOID
NdisMTrIndicateReceive(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  NDIS_HANDLE             MiniportReceiveContext,
    IN  PVOID                   HeaderBuffer,
    IN  UINT                    HeaderBufferSize,
    IN  PVOID                   LookaheadBuffer,
    IN  UINT                    LookaheadBufferSize,
    IN  UINT                    PacketSize
    )

--*/
#ifdef __cplusplus
#define NdisMTrIndicateReceive( _H, _C, _B, _SZ, _L, _LSZ, _PSZ)                \
{                                                                               \
    (*((PNDIS_MINIPORT_BLOCK)(_H))->TrRxIndicateHandler)(                       \
        ((PNDIS_MINIPORT_BLOCK)(_H))->FilterDbs.TrDB,                                     \
        _C,                                                                     \
        _B,                                                                     \
        _SZ,                                                                    \
        _L,                                                                     \
        _LSZ,                                                                   \
        _PSZ                                                                    \
        );                                                                      \
}

#else

#define NdisMTrIndicateReceive( _H, _C, _B, _SZ, _L, _LSZ, _PSZ)                \
{                                                                               \
    (*((PNDIS_MINIPORT_BLOCK)(_H))->TrRxIndicateHandler)(                       \
        ((PNDIS_MINIPORT_BLOCK)(_H))->TrDB,                                     \
        _C,                                                                     \
        _B,                                                                     \
        _SZ,                                                                    \
        _L,                                                                     \
        _LSZ,                                                                   \
        _PSZ                                                                    \
        );                                                                      \
}
#endif

/*++

VOID
NdisMEthIndicateReceiveComplete(
    IN  NDIS_HANDLE             MiniportHandle
    );

--*/
#ifdef __cplusplus

#define NdisMEthIndicateReceiveComplete( _H )                                   \
{                                                                               \
    (*((PNDIS_MINIPORT_BLOCK)(_H))->EthRxCompleteHandler)(                      \
                                        ((PNDIS_MINIPORT_BLOCK)_H)->FilterDbs.EthDB);     \
}

#else

#define NdisMEthIndicateReceiveComplete( _H )                                   \
{                                                                               \
    (*((PNDIS_MINIPORT_BLOCK)(_H))->EthRxCompleteHandler)(                      \
                                        ((PNDIS_MINIPORT_BLOCK)_H)->EthDB);               \
}

#endif


/*++

VOID
NdisMTrIndicateReceiveComplete(
    IN  NDIS_HANDLE             MiniportHandle
    );

--*/
#ifdef __cplusplus
#define NdisMTrIndicateReceiveComplete( _H )                                    \
{                                                                               \
    (*((PNDIS_MINIPORT_BLOCK)(_H))->TrRxCompleteHandler)(                       \
                                        ((PNDIS_MINIPORT_BLOCK)_H)->FilterDbs.TrDB);      \
}
#else
#define NdisMTrIndicateReceiveComplete( _H )                                    \
{                                                                               \
    (*((PNDIS_MINIPORT_BLOCK)(_H))->TrRxCompleteHandler)(                       \
                                        ((PNDIS_MINIPORT_BLOCK)_H)->TrDB);      \
}
#endif


EXPORT
VOID
NdisMIndicateStatus(
    IN  NDIS_HANDLE             MiniportHandle,
    IN  NDIS_STATUS             GeneralStatus,
    IN  PVOID                   StatusBuffer,
    IN  UINT                    StatusBufferSize
    );

EXPORT
VOID
NdisMIndicateStatusComplete(
    IN  NDIS_HANDLE             MiniportHandle
    );

EXPORT
VOID
NdisMRegisterAdapterShutdownHandler(
    IN  NDIS_HANDLE             MiniportHandle,
    IN  PVOID                   ShutdownContext,
    IN  ADAPTER_SHUTDOWN_HANDLER ShutdownHandler
    );

EXPORT
VOID
NdisMDeregisterAdapterShutdownHandler(
    IN  NDIS_HANDLE             MiniportHandle
    );

EXPORT
NDIS_STATUS
NdisMPciAssignResources(
    IN  NDIS_HANDLE             MiniportHandle,
    IN  ULONG                   SlotNumber,
    IN  PNDIS_RESOURCE_LIST *   AssignedResources
    );

EXPORT
NDIS_STATUS
NdisIMNotifyPnPEvent(
    IN  NDIS_HANDLE             MiniportHandle,
    IN  PNET_PNP_EVENT          NetPnPEvent
    );

#endif // NDIS_LEGACY_MINIPORT


//
// Logging support for miniports
//

EXPORT
NDIS_STATUS
NdisMCreateLog(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  UINT                    Size,
    OUT PNDIS_HANDLE            LogHandle
    );

EXPORT
VOID
NdisMCloseLog(
    IN  NDIS_HANDLE             LogHandle
    );

EXPORT
NDIS_STATUS
NdisMWriteLogData(
    IN  NDIS_HANDLE             LogHandle,
    IN  PVOID                   LogBuffer,
    IN  UINT                    LogBufferSize
    );

EXPORT
VOID
NdisMFlushLog(
    IN  NDIS_HANDLE             LogHandle
    );

EXPORT
VOID
NdisMGetDeviceProperty(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN OUT PDEVICE_OBJECT *     PhysicalDeviceObject        OPTIONAL,
    IN OUT PDEVICE_OBJECT *     FunctionalDeviceObject      OPTIONAL,
    IN OUT PDEVICE_OBJECT *     NextDeviceObject            OPTIONAL,
    IN OUT PCM_RESOURCE_LIST *  AllocatedResources          OPTIONAL,
    IN OUT PCM_RESOURCE_LIST *  AllocatedResourcesTranslated OPTIONAL
    );

//
//  Get a pointer to the adapter's localized instance name.
//
EXPORT
NDIS_STATUS
NdisMQueryAdapterInstanceName(
    OUT PNDIS_STRING            pAdapterInstanceName,
    IN  NDIS_HANDLE             MiniportHandle
    );


#if NDIS_LEGACY_MINIPORT

//
// NDIS 5.0 extensions for miniports
//

EXPORT
VOID
NdisMCoIndicateReceivePacket(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PPNDIS_PACKET           PacketArray,
    IN  UINT                    NumberOfPackets
    );

EXPORT
VOID
NdisMCoIndicateStatus(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  NDIS_HANDLE             NdisVcHandle    OPTIONAL,
    IN  NDIS_STATUS             GeneralStatus,
    IN  PVOID                   StatusBuffer    OPTIONAL,
    IN  ULONG                   StatusBufferSize
    );

EXPORT
VOID
NdisMCoReceiveComplete(
    IN  NDIS_HANDLE             MiniportAdapterHandle
    );

EXPORT
VOID
NdisMCoSendComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PNDIS_PACKET            Packet
    );


EXPORT
VOID
NdisMCoRequestComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  PNDIS_REQUEST           Request
    );

EXPORT
NDIS_STATUS
NdisMCmRequest(
    IN  NDIS_HANDLE             NdisAfHandle,
    IN  NDIS_HANDLE             NdisVcHandle    OPTIONAL,
    IN  NDIS_HANDLE             NdisPartyHandle OPTIONAL,
    IN OUT PNDIS_REQUEST        NdisRequest
    );

#endif // NDIS_LEGACY_MINIPORT

EXPORT
VOID
NdisMCoActivateVcComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PCO_CALL_PARAMETERS     CallParameters
    );

EXPORT
VOID
NdisMCoDeactivateVcComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisVcHandle
    );

EXPORT
NDIS_STATUS
NdisMCmRegisterAddressFamily(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  PCO_ADDRESS_FAMILY      AddressFamily,
    IN  PNDIS_CALL_MANAGER_CHARACTERISTICS CmCharacteristics,
    IN  UINT                    SizeOfCmCharacteristics
    );

EXPORT
NDIS_STATUS
NdisMCmCreateVc(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  NDIS_HANDLE             NdisAfHandle,
    IN  NDIS_HANDLE             MiniportVcContext,
    OUT PNDIS_HANDLE            NdisVcHandle
    );

EXPORT
NDIS_STATUS
NdisMCmDeleteVc(
    IN  NDIS_HANDLE             NdisVcHandle
    );


EXPORT
NDIS_STATUS
NdisMCmActivateVc(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PCO_CALL_PARAMETERS     CallParameters
    );

EXPORT
NDIS_STATUS
NdisMCmDeactivateVc(
    IN  NDIS_HANDLE             NdisVcHandle
    );


#if NDIS_LEGACY_MINIPORT

// EXPORT
// VOID
// NdisMCmRequestComplete(
//  IN  NDIS_STATUS             Status,
//  IN  NDIS_HANDLE             NdisAfHandle,
//  IN  NDIS_HANDLE             NdisVcHandle    OPTIONAL,
//  IN  NDIS_HANDLE             NdisPartyHandle OPTIONAL,
//  IN  PNDIS_REQUEST           NdisRequest
//  );
#define NdisMCmRequestComplete(_S_, _AH_, _VH_, _PH_, _R_) \
                                        NdisCoRequestComplete(_S_, _AH_, _VH_, _PH_, _R_)
#endif // NDIS_LEGACY_MINIPORT

#if (NDIS_SUPPORT_NDIS6 || NDIS60)

// EXPORT
// VOID
// NdisMCmOidRequestComplete(
//  IN  NDIS_HANDLE             NdisAfHandle,
//  IN  NDIS_HANDLE             NdisVcHandle    OPTIONAL,
//  IN  NDIS_HANDLE             NdisPartyHandle OPTIONAL,
//  IN  PNDIS_OID_REQUEST       OidRequest,
//  IN  NDIS_STATUS             Status
//  );
#define NdisMCmOidRequestComplete(_AH_, _VH_, _PH_, _R_, _S_) \
                                        NdisCoOidRequestComplete(_AH_, _VH_, _PH_, _R_, _S_)
#endif // NDIS_SUPPORT_NDIS6 || NDIS60


// EXPORT
// VOID
// NdisMCmOpenAddressFamilyComplete(
//  IN  NDIS_STATUS             Status,
//  IN  NDIS_HANDLE             NdisAfHandle,
//  IN  NDIS_HANDLE             CallMgrAfContext
//  );

#define NdisMCmOpenAddressFamilyComplete(_S_, _H_, _C_) \
                                        NdisCmOpenAddressFamilyComplete(_S_, _H_, _C_)
// EXPORT
// NDIS_STATUS
// NdisMCmNotifyCloseAddressFamily(
//   IN  NDIS_HANDLE              NdisAfHandle
// );

#define NdisMCmNotifyCloseAddressFamily(_AH_) \
                                         NdisCmNotifyCloseAddressFamily(_AH_)

// EXPORT
// VOID
// NdisMCmCloseAddressFamilyComplete(
//  IN  NDIS_STATUS             Status,
//  IN  NDIS_HANDLE             NdisAfHandle
//  );

#define NdisMCmCloseAddressFamilyComplete(_S_, _H_)     \
                                        NdisCmCloseAddressFamilyComplete(_S_, _H_)



// EXPORT
// VOID
// NdisMCmRegisterSapComplete(
//  IN  NDIS_STATUS             Status,
//  IN  NDIS_HANDLE             NdisSapHandle,
//  IN  NDIS_HANDLE             CallMgrSapContext
//  );

#define NdisMCmRegisterSapComplete(_S_, _H_, _C_)       \
                                        NdisCmRegisterSapComplete(_S_, _H_, _C_)


// EXPORT
// VOID
// NdisMCmDeregisterSapComplete(
//  IN  NDIS_STATUS             Status,
//  IN  NDIS_HANDLE             NdisSapHandle
//  );

#define NdisMCmDeregisterSapComplete(_S_, _H_)          \
                                        NdisCmDeregisterSapComplete(_S_, _H_)


// EXPORT
// VOID
// NdisMCmMakeCallComplete(
//  IN  NDIS_STATUS             Status,
//  IN  NDIS_HANDLE             NdisVcHandle,
//  IN  NDIS_HANDLE             NdisPartyHandle     OPTIONAL,
//  IN  NDIS_HANDLE             CallMgrPartyContext OPTIONAL,
//  IN  PCO_CALL_PARAMETERS     CallParameters
//  );

#define NdisMCmMakeCallComplete(_S_, _VH_, _PH_, _CC_, _CP_)    \
                                        NdisCmMakeCallComplete(_S_, _VH_, _PH_, _CC_, _CP_)


// EXPORT
// VOID
// NdisMCmCloseCallComplete(
//  IN  NDIS_STATUS             Status,
//  IN  NDIS_HANDLE             NdisVcHandle,
//  IN  NDIS_HANDLE             NdisPartyHandle OPTIONAL
//  );

#define NdisMCmCloseCallComplete(_S_, _VH_, _PH_)       \
                                        NdisCmCloseCallComplete(_S_, _VH_, _PH_)


// EXPORT
// VOID
// NdisMCmAddPartyComplete(
//  IN  NDIS_STATUS             Status,
//  IN  NDIS_HANDLE             NdisPartyHandle,
//  IN  NDIS_HANDLE             CallMgrPartyContext OPTIONAL,
//  IN  PCO_CALL_PARAMETERS     CallParameters
//  );

#define NdisMCmAddPartyComplete(_S_, _H_, _C_, _P_)     \
                                        NdisCmAddPartyComplete(_S_, _H_, _C_, _P_)


// EXPORT
// VOID
// NdisMCmDropPartyComplete(
//  IN  NDIS_STATUS             Status,
//  IN  NDIS_HANDLE             NdisPartyHandle
//  );

#define NdisMCmDropPartyComplete(_S_, _H_)              \
                                        NdisCmDropPartyComplete(_S_, _H_)


// EXPORT
// NDIS_STATUS
// NdisMCmDispatchIncomingCall(
//  IN  NDIS_HANDLE             NdisSapHandle,
//  IN  NDIS_HANDLE             NdisVcHandle,
//  IN  PCO_CALL_PARAMETERS     CallParameters
//  );

#define NdisMCmDispatchIncomingCall(_SH_, _VH_, _CP_)   \
                                        NdisCmDispatchIncomingCall(_SH_, _VH_, _CP_)


// EXPORT
// VOID
// NdisMCmDispatchCallConnected(
//  IN  NDIS_HANDLE             NdisVcHandle
//  );

#define NdisMCmDispatchCallConnected(_H_)               \
                                        NdisCmDispatchCallConnected(_H_)


// EXPORT
// NdisMCmModifyCallQoSComplete(
//  IN  NDIS_STATUS             Status,
//  IN  NDIS_HANDLE             NdisVcHandle,
//  IN  PCO_CALL_PARAMETERS     CallParameters
//  );

#define NdisMCmModifyCallQoSComplete(_S_, _H_, _P_)     \
                                        NdisCmModifyCallQoSComplete(_S_, _H_, _P_)


// EXPORT
// VOID
// VOID
// NdisMCmDispatchIncomingCallQoSChange(
//  IN  NDIS_HANDLE             NdisVcHandle,
//  IN  PCO_CALL_PARAMETERS     CallParameters
//  );

#define NdisMCmDispatchIncomingCallQoSChange(_H_, _P_)  \
                                        NdisCmDispatchIncomingCallQoSChange(_H_, _P_)


// EXPORT
// VOID
// NdisMCmDispatchIncomingCloseCall(
//   IN  NDIS_STATUS             CloseStatus,
//   IN  NDIS_HANDLE             NdisVcHandle,
//   IN  PVOID                   Buffer         OPTIONAL,
//   IN  UINT                    Size
//   );

#define NdisMCmDispatchIncomingCloseCall(_S_, _H_, _B_, _Z_)    \
                                        NdisCmDispatchIncomingCloseCall(_S_, _H_, _B_, _Z_)


//  EXPORT
//  VOID
//  NdisMCmDispatchIncomingDropParty(
//      IN  NDIS_STATUS         DropStatus,
//      IN  NDIS_HANDLE         NdisPartyHandle,
//      IN  PVOID               Buffer      OPTIONAL,
//      IN  UINT                Size
//      );
#define NdisMCmDispatchIncomingDropParty(_S_, _H_, _B_, _Z_)    \
                                        NdisCmDispatchIncomingDropParty(_S_, _H_, _B_, _Z_)


#if NDIS_SUPPORT_NDIS6

EXPORT
NDIS_STATUS
NdisMRegisterScatterGatherDma(
    IN  NDIS_HANDLE                     MiniportAdapterHandle,
    IN  PNDIS_SG_DMA_DESCRIPTION        DmaDescription,
    OUT PNDIS_HANDLE                    NdisMiniportDmaHandle
    );

EXPORT
VOID
NdisMDeregisterScatterGatherDma(
    IN  NDIS_HANDLE                     NdisMiniportDmaHandle
    );

//
// flags used in the call to NdisMAllocateNetBufferSGList
//
#define NDIS_SG_LIST_WRITE_TO_DEVICE    0x000000001

EXPORT
NDIS_STATUS
NdisMAllocateNetBufferSGList(
    IN  NDIS_HANDLE             NdisMiniportDmaHandle,
    IN  PNET_BUFFER             NetBuffer,
    IN  PVOID                   Context,
    IN  ULONG                   Flags,
    IN  PVOID                   ScatterGatherListBuffer OPTIONAL,
    IN  ULONG                   ScatterGatherListBufferSize OPTIONAL
    );

EXPORT
VOID
NdisMFreeNetBufferSGList(
    IN  NDIS_HANDLE             NdisMiniportDmaHandle,
    IN  PSCATTER_GATHER_LIST    pSGL,
    IN  PNET_BUFFER             NetBuffer
    );

EXPORT
NDIS_STATUS
NdisMRegisterMiniportDriver(
    IN  PDRIVER_OBJECT                              DriverObject,
    IN  PUNICODE_STRING                             RegistryPath,
    IN  NDIS_HANDLE                                 MiniportDriverContext,
    IN  PNDIS_MINIPORT_DRIVER_CHARACTERISTICS       MiniportDriverCharacteristics,
    OUT PNDIS_HANDLE                                NdisMiniportDriverHandle
    );

EXPORT
VOID
NdisMDeregisterMiniportDriver(
    IN NDIS_HANDLE              NdisMiniportDriverHandle
    );

EXPORT
VOID
NdisMPauseComplete(
    IN  NDIS_HANDLE             MiniportAdapterHandle
    );

EXPORT
VOID
NdisMRestartComplete(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  NDIS_STATUS             Status
    );


EXPORT
VOID
NdisMOidRequestComplete(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  PNDIS_OID_REQUEST       OidRequest,
    IN  NDIS_STATUS             Status
    );
    

EXPORT
VOID
NdisMSendNetBufferListsComplete(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  PNET_BUFFER_LIST        NetBufferList,
    IN  ULONG                   SendCompleteFlags
    );

EXPORT
VOID
NdisMIndicateReceiveNetBufferLists(
    IN  NDIS_HANDLE             MiniportAdapterHandle, 
    IN  PNET_BUFFER_LIST        NetBufferList,
    IN  NDIS_PORT_NUMBER        PortNumber,
    IN  ULONG                   NumberOfNetBufferLists,
    IN  ULONG                   ReceiveFlags
    );


//
// IO workitem routines
//

EXPORT
NDIS_HANDLE
NdisAllocateIoWorkItem(
    IN  NDIS_HANDLE                 NdisObjectHandle
    );

typedef
VOID
(*NDIS_IO_WORKITEM_ROUTINE)(
    IN PVOID                        WorkItemContext,
    IN NDIS_HANDLE                  NdisIoWorkItemHandle
    );

EXPORT
VOID
NdisQueueIoWorkItem(
    IN NDIS_HANDLE                  NdisIoWorkItemHandle,
    IN NDIS_IO_WORKITEM_ROUTINE     Routine,
    IN PVOID                        WorkItemContext
    );

EXPORT
VOID
NdisFreeIoWorkItem(
    IN NDIS_HANDLE                  NdisIoWorkItemHandle
    );

//
// CONDIS 6.0 extensions
//

EXPORT
VOID
NdisMCoSendNetBufferListsComplete(
    IN NDIS_HANDLE          NdisVcHandle,
    IN PNET_BUFFER_LIST     NetBufferLists,
    IN ULONG                SendCompleteFlags
    );

EXPORT
VOID
NdisMCoIndicateReceiveNetBufferLists(
    IN NDIS_HANDLE          NdisVcHandle,
    IN PNET_BUFFER_LIST     NetBufferLists,
    IN ULONG                NumberOfNetBufferLists,
    IN ULONG                CoReceiveFlags
    );

EXPORT
NDIS_STATUS
NdisMNetPnPEvent(
    IN  NDIS_HANDLE                 MiniportAdapterHandle,
    IN  PNET_PNP_EVENT_NOTIFICATION NetPnPEventNotification
    );

EXPORT
VOID
NdisMResetComplete(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  NDIS_STATUS             Status,
    IN  BOOLEAN                 AddressingReset
    );

EXPORT
ULONG
NdisMGetBusData(
    IN NDIS_HANDLE              NdisMiniportHandle,
    IN ULONG                    WhichSpace,
    IN ULONG                    Offset,
    __out_bcount_full(Length) IN PVOID  Buffer,
    IN ULONG                    Length
    );

EXPORT
ULONG
NdisMSetBusData(
    IN NDIS_HANDLE              NdisMiniportHandle,
    IN ULONG                    WhichSpace,
    IN ULONG                    Offset,
    IN PVOID                    Buffer,
    IN ULONG                    Length
    );

//
// NDIS port APIs
//

EXPORT
NDIS_STATUS
NdisMAllocatePort(
    IN  NDIS_HANDLE                     NdisMiniportHandle,
    IN  OUT PNDIS_PORT_CHARACTERISTICS  PortCharacteristics
    );

EXPORT
NDIS_STATUS
NdisMFreePort(
    IN  NDIS_HANDLE                     NdisMiniportHandle,
    IN  NDIS_PORT_NUMBER                PortNumber
    );

#endif // NDIS_SUPPORT_NDIS6



#pragma once

typedef struct _CO_CALL_PARAMETERS      CO_CALL_PARAMETERS, *PCO_CALL_PARAMETERS;
typedef struct _CO_MEDIA_PARAMETERS     CO_MEDIA_PARAMETERS, *PCO_MEDIA_PARAMETERS;

//
// CoNdis client only handler proto-types - used by clients of call managers
//
typedef
VOID
(*CL_OPEN_AF_COMPLETE_HANDLER)(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             ProtocolAfContext,
    IN  NDIS_HANDLE             NdisAfHandle
    );

typedef
VOID
(*CL_CLOSE_AF_COMPLETE_HANDLER)(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             ProtocolAfContext
    );

typedef
VOID
(*CL_REG_SAP_COMPLETE_HANDLER)(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             ProtocolSapContext,
    IN  PCO_SAP                 Sap,
    IN  NDIS_HANDLE             NdisSapHandle
    );

typedef
VOID
(*CL_DEREG_SAP_COMPLETE_HANDLER)(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             ProtocolSapContext
    );

typedef
VOID
(*CL_MAKE_CALL_COMPLETE_HANDLER)(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             ProtocolVcContext,
    IN  NDIS_HANDLE             NdisPartyHandle     OPTIONAL,
    IN  PCO_CALL_PARAMETERS     CallParameters
    );

typedef
VOID
(*CL_CLOSE_CALL_COMPLETE_HANDLER)(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             ProtocolVcContext,
    IN  NDIS_HANDLE             ProtocolPartyContext OPTIONAL
    );

typedef
VOID
(*CL_ADD_PARTY_COMPLETE_HANDLER)(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             ProtocolPartyContext,
    IN  NDIS_HANDLE             NdisPartyHandle,
    IN  PCO_CALL_PARAMETERS     CallParameters
    );

typedef
VOID
(*CL_DROP_PARTY_COMPLETE_HANDLER)(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             ProtocolPartyContext
    );

typedef
NDIS_STATUS
(*CL_INCOMING_CALL_HANDLER)(
    IN  NDIS_HANDLE             ProtocolSapContext,
    IN  NDIS_HANDLE             ProtocolVcContext,
    IN OUT PCO_CALL_PARAMETERS  CallParameters
    );

typedef
VOID
(*CL_CALL_CONNECTED_HANDLER)(
    IN  NDIS_HANDLE             ProtocolVcContext
    );

typedef
VOID
(*CL_INCOMING_CLOSE_CALL_HANDLER)(
    IN  NDIS_STATUS             CloseStatus,
    IN  NDIS_HANDLE             ProtocolVcContext,
    IN  PVOID                   CloseData   OPTIONAL,
    IN  UINT                    Size        OPTIONAL
    );

typedef
VOID
(*CL_INCOMING_DROP_PARTY_HANDLER)(
    IN  NDIS_STATUS             DropStatus,
    IN  NDIS_HANDLE             ProtocolPartyContext,
    IN  PVOID                   CloseData   OPTIONAL,
    IN  UINT                    Size        OPTIONAL
    );

typedef
VOID
(*CL_MODIFY_CALL_QOS_COMPLETE_HANDLER)(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             ProtocolVcContext,
    IN  PCO_CALL_PARAMETERS     CallParameters
    );

typedef
VOID
(*CL_INCOMING_CALL_QOS_CHANGE_HANDLER)(
    IN  NDIS_HANDLE             ProtocolVcContext,
    IN  PCO_CALL_PARAMETERS     CallParameters
    );

#if NDIS_LEGACY_PROTOCOL
typedef struct _NDIS_CLIENT_CHARACTERISTICS
{
    UCHAR                           MajorVersion;
    UCHAR                           MinorVersion;

    USHORT                          Filler;
    UINT                            Reserved;

    CO_CREATE_VC_HANDLER            ClCreateVcHandler;
    CO_DELETE_VC_HANDLER            ClDeleteVcHandler;
    CO_REQUEST_HANDLER              ClRequestHandler;
    CO_REQUEST_COMPLETE_HANDLER     ClRequestCompleteHandler;
    CL_OPEN_AF_COMPLETE_HANDLER     ClOpenAfCompleteHandler;
    CL_CLOSE_AF_COMPLETE_HANDLER    ClCloseAfCompleteHandler;
    CL_REG_SAP_COMPLETE_HANDLER     ClRegisterSapCompleteHandler;
    CL_DEREG_SAP_COMPLETE_HANDLER   ClDeregisterSapCompleteHandler;
    CL_MAKE_CALL_COMPLETE_HANDLER   ClMakeCallCompleteHandler;
    CL_MODIFY_CALL_QOS_COMPLETE_HANDLER ClModifyCallQoSCompleteHandler;
    CL_CLOSE_CALL_COMPLETE_HANDLER  ClCloseCallCompleteHandler;
    CL_ADD_PARTY_COMPLETE_HANDLER   ClAddPartyCompleteHandler;
    CL_DROP_PARTY_COMPLETE_HANDLER  ClDropPartyCompleteHandler;
    CL_INCOMING_CALL_HANDLER        ClIncomingCallHandler;
    CL_INCOMING_CALL_QOS_CHANGE_HANDLER ClIncomingCallQoSChangeHandler;
    CL_INCOMING_CLOSE_CALL_HANDLER  ClIncomingCloseCallHandler;
    CL_INCOMING_DROP_PARTY_HANDLER  ClIncomingDropPartyHandler;
    CL_CALL_CONNECTED_HANDLER       ClCallConnectedHandler;

} NDIS_CLIENT_CHARACTERISTICS, *PNDIS_CLIENT_CHARACTERISTICS;
#endif // NDIS_LEGACY_PROTOCOL

//
// CoNdis call-manager only handler proto-types - used by call managers only
//
typedef
NDIS_STATUS
(*CM_OPEN_AF_HANDLER)(
    IN  NDIS_HANDLE             CallMgrBindingContext,
    IN  PCO_ADDRESS_FAMILY      AddressFamily,
    IN  NDIS_HANDLE             NdisAfHandle,
    OUT PNDIS_HANDLE            CallMgrAfContext
    );

typedef
NDIS_STATUS
(*CM_CLOSE_AF_HANDLER)(
    IN  NDIS_HANDLE             CallMgrAfContext
    );

typedef
NDIS_STATUS
(*CM_REG_SAP_HANDLER)(
    IN  NDIS_HANDLE             CallMgrAfContext,
    IN  PCO_SAP                 Sap,
    IN  NDIS_HANDLE             NdisSapHandle,
    OUT PNDIS_HANDLE            CallMgrSapContext
    );

typedef
NDIS_STATUS
(*CM_DEREG_SAP_HANDLER)(
    IN  NDIS_HANDLE             CallMgrSapContext
    );

typedef
NDIS_STATUS
(*CM_MAKE_CALL_HANDLER)(
    IN  NDIS_HANDLE             CallMgrVcContext,
    IN OUT PCO_CALL_PARAMETERS  CallParameters,
    IN  NDIS_HANDLE             NdisPartyHandle     OPTIONAL,
    OUT PNDIS_HANDLE            CallMgrPartyContext OPTIONAL
    );

typedef
NDIS_STATUS
(*CM_CLOSE_CALL_HANDLER)(
    IN  NDIS_HANDLE             CallMgrVcContext,
    IN  NDIS_HANDLE             CallMgrPartyContext OPTIONAL,
    IN  PVOID                   CloseData           OPTIONAL,
    IN  UINT                    Size                OPTIONAL
    );

typedef
NDIS_STATUS
(*CM_MODIFY_CALL_QOS_HANDLER)(
    IN  NDIS_HANDLE             CallMgrVcContext,
    IN  PCO_CALL_PARAMETERS     CallParameters
    );

typedef
VOID
(*CM_INCOMING_CALL_COMPLETE_HANDLER)(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             CallMgrVcContext,
    IN  PCO_CALL_PARAMETERS     CallParameters
    );

typedef
VOID
(*CM_ACTIVATE_VC_COMPLETE_HANDLER)(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             CallMgrVcContext,
    IN  PCO_CALL_PARAMETERS     CallParameters
    );

typedef
VOID
(*CM_DEACTIVATE_VC_COMPLETE_HANDLER)(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             CallMgrVcContext
    );

typedef
NDIS_STATUS
(*CM_ADD_PARTY_HANDLER)(
    IN  NDIS_HANDLE             CallMgrVcContext,
    IN OUT PCO_CALL_PARAMETERS  CallParameters,
    IN  NDIS_HANDLE             NdisPartyHandle,
    OUT PNDIS_HANDLE            CallMgrPartyContext
    );

typedef
NDIS_STATUS
(*CM_DROP_PARTY_HANDLER)(
    IN  NDIS_HANDLE             CallMgrPartyContext,
    IN  PVOID                   CloseData   OPTIONAL,
    IN  UINT                    Size        OPTIONAL
    );

#if NDIS_LEGACY_DRIVER

typedef struct _NDIS_CALL_MANAGER_CHARACTERISTICS
{
    UCHAR                           MajorVersion;
    UCHAR                           MinorVersion;
    USHORT                          Filler;
    UINT                            Reserved;

    CO_CREATE_VC_HANDLER            CmCreateVcHandler;
    CO_DELETE_VC_HANDLER            CmDeleteVcHandler;
    CM_OPEN_AF_HANDLER              CmOpenAfHandler;
    CM_CLOSE_AF_HANDLER             CmCloseAfHandler;
    CM_REG_SAP_HANDLER              CmRegisterSapHandler;
    CM_DEREG_SAP_HANDLER            CmDeregisterSapHandler;
    CM_MAKE_CALL_HANDLER            CmMakeCallHandler;
    CM_CLOSE_CALL_HANDLER           CmCloseCallHandler;
    CM_INCOMING_CALL_COMPLETE_HANDLER CmIncomingCallCompleteHandler;
    CM_ADD_PARTY_HANDLER            CmAddPartyHandler;
    CM_DROP_PARTY_HANDLER           CmDropPartyHandler;
    CM_ACTIVATE_VC_COMPLETE_HANDLER CmActivateVcCompleteHandler;
    CM_DEACTIVATE_VC_COMPLETE_HANDLER CmDeactivateVcCompleteHandler;
    CM_MODIFY_CALL_QOS_HANDLER      CmModifyCallQoSHandler;
    CO_REQUEST_HANDLER              CmRequestHandler;
    CO_REQUEST_COMPLETE_HANDLER     CmRequestCompleteHandler;
    
} NDIS_CALL_MANAGER_CHARACTERISTICS, *PNDIS_CALL_MANAGER_CHARACTERISTICS;

#endif // NDIS_LEGACY_DRIVER

//
// this send flag is used on ATM net cards to set ( turn on ) the CLP bit
// (Cell Loss Priority) bit
//
#define CO_SEND_FLAG_SET_DISCARD_ELIBILITY  0x00000001

//
// the Address structure used on NDIS_CO_ADD_ADDRESS or NDIS_CO_DELETE_ADDRESS
//
typedef struct _CO_ADDRESS
{
    ULONG                       AddressSize;
    UCHAR                       Address[1];
} CO_ADDRESS, *PCO_ADDRESS;

//
// the list of addresses returned from the CallMgr on a NDIS_CO_GET_ADDRESSES
//
typedef struct _CO_ADDRESS_LIST
{
    ULONG                       NumberOfAddressesAvailable;
    ULONG                       NumberOfAddresses;
    CO_ADDRESS                  AddressList;
} CO_ADDRESS_LIST, *PCO_ADDRESS_LIST;

#ifndef FAR
#define FAR
#endif
#include <qos.h>

typedef struct _CO_SPECIFIC_PARAMETERS
{
    ULONG                       ParamType;
    ULONG                       Length;
    UCHAR                       Parameters[1];
} CO_SPECIFIC_PARAMETERS, *PCO_SPECIFIC_PARAMETERS;

typedef struct _CO_CALL_MANAGER_PARAMETERS
{
    FLOWSPEC                    Transmit;
    FLOWSPEC                    Receive;
    CO_SPECIFIC_PARAMETERS      CallMgrSpecific;
} CO_CALL_MANAGER_PARAMETERS, *PCO_CALL_MANAGER_PARAMETERS;


//
// this is the generic portion of the media parameters, including the media
// specific component too.
//
typedef struct _CO_MEDIA_PARAMETERS
{
    ULONG                       Flags;
    ULONG                       ReceivePriority;
    ULONG                       ReceiveSizeHint;
    CO_SPECIFIC_PARAMETERS POINTER_ALIGNMENT      MediaSpecific;
} CO_MEDIA_PARAMETERS, *PCO_MEDIA_PARAMETERS;


//
// definitions for the flags in CO_MEDIA_PARAMETERS
//
#define RECEIVE_TIME_INDICATION 0x00000001
#define USE_TIME_STAMPS         0x00000002
#define TRANSMIT_VC             0x00000004
#define RECEIVE_VC              0x00000008
#define INDICATE_ERRED_PACKETS  0x00000010
#define INDICATE_END_OF_TX      0x00000020
#define RESERVE_RESOURCES_VC    0x00000040
#define ROUND_DOWN_FLOW         0x00000080
#define ROUND_UP_FLOW           0x00000100
//
// define a flag to set in the flags of an Ndis packet when the miniport
// indicates a receive with an error in it
//
#define ERRED_PACKET_INDICATION 0x00000001

//
// this is the structure passed during call-setup
//
typedef struct _CO_CALL_PARAMETERS
{
    ULONG                       Flags;
    PCO_CALL_MANAGER_PARAMETERS CallMgrParameters;
    PCO_MEDIA_PARAMETERS        MediaParameters;
} CO_CALL_PARAMETERS, *PCO_CALL_PARAMETERS;

//
// Definitions for the Flags in CO_CALL_PARAMETERS
//
#define PERMANENT_VC            0x00000001
#define CALL_PARAMETERS_CHANGED 0x00000002
#define QUERY_CALL_PARAMETERS   0x00000004
#define BROADCAST_VC            0x00000008
#define MULTIPOINT_VC           0x00000010

//
// The format of the Request for adding/deleting a PVC
//
typedef struct _CO_PVC
{
    NDIS_HANDLE                 NdisAfHandle;
    CO_SPECIFIC_PARAMETERS      PvcParameters;
} CO_PVC,*PCO_PVC;



//
// NDIS 5.0 Extensions for protocols
//

EXPORT
NDIS_STATUS
NdisCoAssignInstanceName(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PNDIS_STRING            BaseInstanceName,
    OUT PNDIS_STRING            VcInstanceName
    );

#if NDIS_LEGACY_PROTOCOL
EXPORT
VOID
NdisCoSendPackets(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PPNDIS_PACKET           PacketArray,
    IN  UINT                    NumberOfPackets
    );

#endif // NDIS_LEGACY_PROTOCOL

EXPORT
NDIS_STATUS
NdisCoCreateVc(
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  NDIS_HANDLE             NdisAfHandle        OPTIONAL,   // For CM signalling VCs
    IN  NDIS_HANDLE             ProtocolVcContext,
    IN OUT PNDIS_HANDLE         NdisVcHandle
    );


EXPORT
NDIS_STATUS
NdisCoDeleteVc(
    IN  NDIS_HANDLE             NdisVcHandle
    );

#if NDIS_LEGACY_PROTOCOL
EXPORT
NDIS_STATUS
NdisCoRequest(
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  NDIS_HANDLE             NdisAfHandle    OPTIONAL,
    IN  NDIS_HANDLE             NdisVcHandle    OPTIONAL,
    IN  NDIS_HANDLE             NdisPartyHandle OPTIONAL,
    IN OUT PNDIS_REQUEST        NdisRequest
    );


EXPORT
VOID
NdisCoRequestComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisAfHandle,
    IN  NDIS_HANDLE             NdisVcHandle    OPTIONAL,
    IN  NDIS_HANDLE             NdisPartyHandle OPTIONAL,
    IN  PNDIS_REQUEST           NdisRequest
    );

#endif // NDIS_LEGACY_PROTOCOL

#ifndef __NDISTAPI_VAR_STRING_DECLARED
#define __NDISTAPI_VAR_STRING_DECLARED

typedef struct _VAR_STRING
{
    ULONG   ulTotalSize;
    ULONG   ulNeededSize;
    ULONG   ulUsedSize;

    ULONG   ulStringFormat;
    ULONG   ulStringSize;
    ULONG   ulStringOffset;

} VAR_STRING, *PVAR_STRING;

#endif // __NDISTAPI_VAR_STRING_DECLARED


#ifndef __NDISTAPI_STRINGFORMATS_DEFINED
#define __NDISTAPI_STRINGFORMATS_DEFINED

#define STRINGFORMAT_ASCII                          0x00000001
#define STRINGFORMAT_DBCS                           0x00000002
#define STRINGFORMAT_UNICODE                        0x00000003
#define STRINGFORMAT_BINARY                         0x00000004

#endif // __NDISTAPI_STRINGFORMATS_DEFINED

EXPORT
NDIS_STATUS
NdisCoGetTapiCallId(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  OUT PVAR_STRING         TapiCallId
    );

#if NDIS_LEGACY_PROTOCOL
//
// Client Apis
//
EXPORT
NDIS_STATUS
NdisClOpenAddressFamily(
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  PCO_ADDRESS_FAMILY      AddressFamily,
    IN  NDIS_HANDLE             ProtocolAfContext,
    IN  PNDIS_CLIENT_CHARACTERISTICS ClCharacteristics,
    IN  UINT                    SizeOfClCharacteristics,
    OUT PNDIS_HANDLE            NdisAfHandle
    );

#endif // NDIS_LEGACY_PROTOCOL

EXPORT
NDIS_STATUS
NdisClCloseAddressFamily(
    IN  NDIS_HANDLE             NdisAfHandle
    );


EXPORT
NDIS_STATUS
NdisClRegisterSap(
    IN  NDIS_HANDLE             NdisAfHandle,
    IN  NDIS_HANDLE             ProtocolSapContext,
    IN  PCO_SAP                 Sap,
    OUT PNDIS_HANDLE            NdisSapHandle
    );


EXPORT
NDIS_STATUS
NdisClDeregisterSap(
    IN  NDIS_HANDLE             NdisSapHandle
    );


EXPORT
NDIS_STATUS
NdisClMakeCall(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN OUT PCO_CALL_PARAMETERS  CallParameters,
    IN  NDIS_HANDLE             ProtocolPartyContext    OPTIONAL,
    OUT PNDIS_HANDLE            NdisPartyHandle         OPTIONAL
    );


EXPORT
NDIS_STATUS
NdisClCloseCall(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  NDIS_HANDLE             NdisPartyHandle         OPTIONAL,
    IN  PVOID                   Buffer                  OPTIONAL,
    IN  UINT                    Size                    OPTIONAL
    );


EXPORT
NDIS_STATUS
NdisClModifyCallQoS(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PCO_CALL_PARAMETERS     CallParameters
    );


EXPORT
VOID
NdisClIncomingCallComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PCO_CALL_PARAMETERS     CallParameters
    );


EXPORT
NDIS_STATUS
NdisClAddParty(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  NDIS_HANDLE             ProtocolPartyContext,
    IN OUT PCO_CALL_PARAMETERS  CallParameters,
    OUT PNDIS_HANDLE            NdisPartyHandle
    );


EXPORT
NDIS_STATUS
NdisClDropParty(
    IN  NDIS_HANDLE             NdisPartyHandle,
    IN  PVOID                   Buffer      OPTIONAL,
    IN  UINT                    Size        OPTIONAL
    );


EXPORT
NDIS_STATUS
NdisClGetProtocolVcContextFromTapiCallId(
    IN  UNICODE_STRING          TapiCallId,
    OUT PNDIS_HANDLE            ProtocolVcContext
    );

//
// Call Manager Apis
//

#if NDIS_LEGACY_DRIVER
EXPORT
NDIS_STATUS
NdisCmRegisterAddressFamily(
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  PCO_ADDRESS_FAMILY      AddressFamily,
    IN  PNDIS_CALL_MANAGER_CHARACTERISTICS CmCharacteristics,
    IN  UINT                    SizeOfCmCharacteristics
    );

#endif

EXPORT
VOID
NdisCmOpenAddressFamilyComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisAfHandle,
    IN  NDIS_HANDLE             CallMgrAfContext
    );


EXPORT
VOID
NdisCmCloseAddressFamilyComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisAfHandle
    );


EXPORT
VOID
NdisCmRegisterSapComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisSapHandle,
    IN  NDIS_HANDLE             CallMgrSapContext
    );


EXPORT
VOID
NdisCmDeregisterSapComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisSapHandle
    );


EXPORT
NDIS_STATUS
NdisCmActivateVc(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN OUT PCO_CALL_PARAMETERS  CallParameters
    );


EXPORT
NDIS_STATUS
NdisCmDeactivateVc(
    IN  NDIS_HANDLE             NdisVcHandle
    );


EXPORT
VOID
NdisCmMakeCallComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  NDIS_HANDLE             NdisPartyHandle     OPTIONAL,
    IN  NDIS_HANDLE             CallMgrPartyContext OPTIONAL,
    IN  PCO_CALL_PARAMETERS     CallParameters
    );


EXPORT
VOID
NdisCmCloseCallComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  NDIS_HANDLE             NdisPartyHandle OPTIONAL
    );


EXPORT
VOID
NdisCmAddPartyComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisPartyHandle,
    IN  NDIS_HANDLE             CallMgrPartyContext OPTIONAL,
    IN  PCO_CALL_PARAMETERS     CallParameters
    );


EXPORT
VOID
NdisCmDropPartyComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisPartyHandle
    );


EXPORT
NDIS_STATUS
NdisCmDispatchIncomingCall(
    IN  NDIS_HANDLE             NdisSapHandle,
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PCO_CALL_PARAMETERS     CallParameters
    );


EXPORT
VOID
NdisCmDispatchCallConnected(
    IN  NDIS_HANDLE             NdisVcHandle
    );


EXPORT
VOID
NdisCmModifyCallQoSComplete(
    IN  NDIS_STATUS             Status,
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PCO_CALL_PARAMETERS     CallParameters
    );


EXPORT
VOID
NdisCmDispatchIncomingCallQoSChange(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PCO_CALL_PARAMETERS     CallParameters
    );


EXPORT
VOID
NdisCmDispatchIncomingCloseCall(
    IN  NDIS_STATUS             CloseStatus,
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PVOID                   Buffer      OPTIONAL,
    IN  UINT                    Size        OPTIONAL
    );


EXPORT
VOID
NdisCmDispatchIncomingDropParty(
    IN  NDIS_STATUS             DropStatus,
    IN  NDIS_HANDLE             NdisPartyHandle,
    IN  PVOID                   Buffer      OPTIONAL,
    IN  UINT                    Size        OPTIONAL
    );

#if (NDIS_SUPPORT_NDIS6)

//
// CONDIS 6.0 extensions
//
#define NDIS_CO_CLIENT_OPTIONAL_HANDLERS_REVISION_1     1

typedef struct _NDIS_CO_CLIENT_OPTIONAL_HANDLERS
{
    NDIS_OBJECT_HEADER                      Header;
    ULONG                                   Reserved;
    
    CO_CREATE_VC_HANDLER                ClCreateVcHandler;
    CO_DELETE_VC_HANDLER                ClDeleteVcHandler;
    CO_OID_REQUEST_HANDLER              ClOidRequestHandler;
    CO_OID_REQUEST_COMPLETE_HANDLER     ClOidRequestCompleteHandler;
    CL_OPEN_AF_COMPLETE_HANDLER_EX      ClOpenAfCompleteHandlerEx;
    CL_CLOSE_AF_COMPLETE_HANDLER        ClCloseAfCompleteHandler;
    CL_REG_SAP_COMPLETE_HANDLER         ClRegisterSapCompleteHandler;
    CL_DEREG_SAP_COMPLETE_HANDLER       ClDeregisterSapCompleteHandler;
    CL_MAKE_CALL_COMPLETE_HANDLER       ClMakeCallCompleteHandler;
    CL_MODIFY_CALL_QOS_COMPLETE_HANDLER ClModifyCallQoSCompleteHandler;
    CL_CLOSE_CALL_COMPLETE_HANDLER      ClCloseCallCompleteHandler;
    CL_ADD_PARTY_COMPLETE_HANDLER       ClAddPartyCompleteHandler;
    CL_DROP_PARTY_COMPLETE_HANDLER      ClDropPartyCompleteHandler;
    CL_INCOMING_CALL_HANDLER            ClIncomingCallHandler;
    CL_INCOMING_CALL_QOS_CHANGE_HANDLER ClIncomingCallQoSChangeHandler;
    CL_INCOMING_CLOSE_CALL_HANDLER      ClIncomingCloseCallHandler;
    CL_INCOMING_DROP_PARTY_HANDLER      ClIncomingDropPartyHandler;
    CL_CALL_CONNECTED_HANDLER           ClCallConnectedHandler;
    CL_NOTIFY_CLOSE_AF_HANDLER          ClNotifyCloseAfHandler;
}NDIS_CO_CLIENT_OPTIONAL_HANDLERS, *PNDIS_CO_CLIENT_OPTIONAL_HANDLERS; 

#define NDIS_SIZEOF_CO_CLIENT_OPTIONAL_HANDLERS_REVISION_1  \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_CO_CLIENT_OPTIONAL_HANDLERS, ClNotifyCloseAfHandler)

#define  NDIS_CO_CALL_MANAGER_OPTIONAL_HANDLERS_REVISION_1     1  

typedef struct _NDIS_CO_CALL_MANAGER_OPTIONAL_HANDLERS
{
    NDIS_OBJECT_HEADER              Header;
    ULONG                           Reserved;
    CO_CREATE_VC_HANDLER            CmCreateVcHandler;
    CO_DELETE_VC_HANDLER            CmDeleteVcHandler;
    CM_OPEN_AF_HANDLER              CmOpenAfHandler;
    CM_CLOSE_AF_HANDLER             CmCloseAfHandler;
    CM_REG_SAP_HANDLER              CmRegisterSapHandler;
    CM_DEREG_SAP_HANDLER            CmDeregisterSapHandler;
    CM_MAKE_CALL_HANDLER            CmMakeCallHandler;
    CM_CLOSE_CALL_HANDLER           CmCloseCallHandler;
    CM_INCOMING_CALL_COMPLETE_HANDLER CmIncomingCallCompleteHandler;
    CM_ADD_PARTY_HANDLER            CmAddPartyHandler;
    CM_DROP_PARTY_HANDLER           CmDropPartyHandler;
    CM_ACTIVATE_VC_COMPLETE_HANDLER CmActivateVcCompleteHandler;
    CM_DEACTIVATE_VC_COMPLETE_HANDLER CmDeactivateVcCompleteHandler;
    CM_MODIFY_CALL_QOS_HANDLER      CmModifyCallQoSHandler;
    CO_OID_REQUEST_HANDLER          CmOidRequestHandler;
    CO_OID_REQUEST_COMPLETE_HANDLER CmOidRequestCompleteHandler;
    CM_NOTIFY_CLOSE_AF_COMPLETE_HANDLER           CmNotifyCloseAfCompleteHandler;

} NDIS_CO_CALL_MANAGER_OPTIONAL_HANDLERS, *PNDIS_CO_CALL_MANAGER_OPTIONAL_HANDLERS;

#define NDIS_SIZEOF_CO_CALL_MANAGER_OPTIONAL_HANDLERS_REVISION_1  \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_CO_CALL_MANAGER_OPTIONAL_HANDLERS, CmOidRequestCompleteHandler)


EXPORT
VOID
NdisCoSendNetBufferLists(
    IN  NDIS_HANDLE             NdisVcHandle,
    IN  PNET_BUFFER_LIST        NetBufferLists,
    IN  ULONG                   SendFlags
    );

EXPORT
NDIS_STATUS
NdisCoOidRequest(
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  NDIS_HANDLE             NdisAfHandle    OPTIONAL,
    IN  NDIS_HANDLE             NdisVcHandle    OPTIONAL,
    IN  NDIS_HANDLE             NdisPartyHandle OPTIONAL,
    IN OUT PNDIS_OID_REQUEST    OidRequest
    );


EXPORT
VOID
NdisCoOidRequestComplete(
    IN  NDIS_HANDLE             NdisAfHandle,
    IN  NDIS_HANDLE             NdisVcHandle    OPTIONAL,
    IN  NDIS_HANDLE             NdisPartyHandle OPTIONAL,
    IN  PNDIS_OID_REQUEST       OidRequest,
    IN  NDIS_STATUS             Status
    );

EXPORT
NDIS_STATUS
NdisClOpenAddressFamilyEx (
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  PCO_ADDRESS_FAMILY      AddressFamily,
    IN  NDIS_HANDLE             ClientAfContext,
    OUT PNDIS_HANDLE            NdisAfHandle
    );

EXPORT
NDIS_STATUS
NdisCmRegisterAddressFamilyEx(
    IN  NDIS_HANDLE             NdisBindingHandle,
    IN  PCO_ADDRESS_FAMILY      AddressFamily
    );

EXPORT
NDIS_STATUS
NdisMCmRegisterAddressFamilyEx(
    IN  NDIS_HANDLE             MiniportAdapterHandle,
    IN  PCO_ADDRESS_FAMILY      AddressFamily
    );

EXPORT
NDIS_STATUS
NdisCmNotifyCloseAddressFamily (
    IN  NDIS_HANDLE             NdisAfHandle 
    );

EXPORT
VOID
NdisClNotifyCloseAddressFamilyComplete (
    IN  NDIS_HANDLE             NdisAfHandle,
    IN  NDIS_STATUS              Status
    );


#endif // NDIS_SUPPORT_NDIS6




#pragma once

#if NDIS_SUPPORT_NDIS6
//
// Ndis Light Weight filters
// 

    
//
// init / de-init
//

//
// Flags used in NDIS_FILTER_ATTACH_PARAMETERS
//

#define NDIS_FILTER_ATTACH_FLAGS_IGNORE_MANDATORY          0x00000001   // If a mandatory filter fails to attach, it can set the flag
                                                                        // to ask NDIS to ignore it is a mandatory filter


#define NDIS_FILTER_ATTACH_PARAMETERS_REVISION_1     1

typedef struct _NDIS_FILTER_ATTACH_PARAMETERS
{
    IN NDIS_OBJECT_HEADER          Header;
    IN NET_IFINDEX                 IfIndex;
    IN NET_LUID                    NetLuid;
    IN PNDIS_STRING                FilterModuleGuidName;
    IN NET_IFINDEX                 BaseMiniportIfIndex;
    IN PNDIS_STRING                BaseMiniportInstanceName;
    IN PNDIS_STRING                BaseMiniportName;
    IN NDIS_MEDIA_CONNECT_STATE    MediaConnectState;
    IN NET_IF_MEDIA_DUPLEX_STATE   MediaDuplexState;
    IN ULONG64                     XmitLinkSpeed;
    IN ULONG64                     RcvLinkSpeed;
    IN OUT NDIS_MEDIUM             MiniportMediaType;
    IN OUT NDIS_PHYSICAL_MEDIUM    MiniportPhysicalMediaType;
    IN NDIS_HANDLE                 MiniportMediaSpecificAttributes;
    IN PNDIS_OFFLOAD               DefaultOffloadConfiguration;
    IN USHORT                      MacAddressLength;
    IN UCHAR                       CurrentMacAddress[NDIS_MAX_PHYS_ADDRESS_LENGTH];
    IN NET_LUID                    BaseMiniportNetLuid;
    IN NET_IFINDEX                 LowerIfIndex;
    IN NET_LUID                    LowerIfNetLuid;
    IN OUT ULONG                   Flags;
} NDIS_FILTER_ATTACH_PARAMETERS, *PNDIS_FILTER_ATTACH_PARAMETERS;
#define NDIS_SIZEOF_FILTER_ATTACH_PARAMETERS_REVISION_1  \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_FILTER_ATTACH_PARAMETERS, Flags)

typedef
NDIS_STATUS
(*FILTER_ATTACH_HANDLER)(
    IN  NDIS_HANDLE                     NdisFilterHandle,
    IN  NDIS_HANDLE                     FilterDriverContext,
    IN  PNDIS_FILTER_ATTACH_PARAMETERS  AttachParameters
    );

typedef
VOID
(*FILTER_DETACH_HANDLER)(
    IN  NDIS_HANDLE             FilterModuleContext
    );

typedef
NDIS_STATUS
(*FILTER_SET_FILTER_MODULE_OPTIONS_HANDLER)(
    IN  NDIS_HANDLE             FilterModuleContext
    );


#define NDIS_FILTER_RESTART_PARAMETERS_REVISION_1     1


typedef struct _NDIS_FILTER_RESTART_PARAMETERS
{
    NDIS_OBJECT_HEADER          Header;
    NDIS_MEDIUM                 MiniportMediaType;
    NDIS_PHYSICAL_MEDIUM        MiniportPhysicalMediaType;
    PNDIS_RESTART_ATTRIBUTES    RestartAttributes;
    NET_IFINDEX                 LowerIfIndex;
    NET_LUID                    LowerIfNetLuid;    
    ULONG                       Flags;    
} NDIS_FILTER_RESTART_PARAMETERS, *PNDIS_FILTER_RESTART_PARAMETERS;

#define NDIS_SIZEOF__FILTER_RESTART_PARAMETERS_REVISION_1  \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_FILTER_RESTART_PARAMETERS, Flags)

typedef
NDIS_STATUS
(*FILTER_RESTART_HANDLER)(
    IN  NDIS_HANDLE                     FilterModuleContext,
    IN  PNDIS_FILTER_RESTART_PARAMETERS RestartParameters
    );

#define NDIS_FILTER_PAUSE_PARAMETERS_REVISION_1     1

typedef struct _NDIS_FILTER_PAUSE_PARAMETERS
{
    NDIS_OBJECT_HEADER          Header;
    ULONG                       Flags;
    ULONG                       PauseReason;
} NDIS_FILTER_PAUSE_PARAMETERS, *PNDIS_FILTER_PAUSE_PARAMETERS;

#define NDIS_SIZEOF_FILTER_PAUSE_PARAMETERS_REVISION_1   \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_FILTER_PAUSE_PARAMETERS, Flags)

typedef
NDIS_STATUS
(*FILTER_PAUSE_HANDLER)(
    IN  NDIS_HANDLE                     FilterModuleContext,
    IN  PNDIS_FILTER_PAUSE_PARAMETERS   PauseParameters
    );

//
// inbound requests/data
//

typedef
NDIS_STATUS
(*FILTER_OID_REQUEST_HANDLER)(
    IN  NDIS_HANDLE             FilterModuleContext,
    IN  PNDIS_OID_REQUEST       OidRequest
    );    

typedef
VOID
(*FILTER_CANCEL_OID_REQUEST_HANDLER)(
    IN  NDIS_HANDLE             FilterModuleContext,
    IN  PVOID                   RequestId
    );


typedef
VOID
(*FILTER_SEND_NET_BUFFER_LISTS_HANDLER)(
    IN  NDIS_HANDLE             FilterModuleContext,
    IN  PNET_BUFFER_LIST        NetBufferList,
    IN  NDIS_PORT_NUMBER        PortNumber,
    IN  ULONG                   SendFlags
    );

typedef
VOID
(*FILTER_CANCEL_SEND_HANDLER)(
    IN  NDIS_HANDLE             FilterModuleContext,
    IN  PVOID                   CancelId
    );

typedef
VOID
(*FILTER_RETURN_NET_BUFFER_LISTS_HANDLER)(
    IN  NDIS_HANDLE             FilterModuleContext,
    IN  PNET_BUFFER_LIST        NetBufferLists,
    IN  ULONG                   ReturnFlags
    );

typedef
VOID
(*FILTER_SEND_NET_BUFFER_LISTS_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             FilterModuleContext,
    IN  PNET_BUFFER_LIST        NetBufferList,
    IN  ULONG                   SendCompleteFlags
    );

typedef
VOID
(*FILTER_RECEIVE_NET_BUFFER_LISTS_HANDLER)(
    IN  NDIS_HANDLE             FilterModuleContext,
    IN  PNET_BUFFER_LIST        NetBufferLists,
    IN  NDIS_PORT_NUMBER        PortNumber,
    IN  ULONG                   NumberOfNetBufferLists,
    IN  ULONG                   ReceiveFlags
    );

typedef 
VOID
(*FILTER_DEVICE_PNP_EVENT_NOTIFY_HANDLER)(
    IN  NDIS_HANDLE             FilterModuleContext,
    IN  PNET_DEVICE_PNP_EVENT   NetDevicePnPEvent
    );


typedef
NDIS_STATUS
(*FILTER_NET_PNP_EVENT_HANDLER)(
    IN  NDIS_HANDLE                         FilterModuleContext,
    IN  PNET_PNP_EVENT_NOTIFICATION         NetPnPEventNotification
    );


typedef
VOID
(*FILTER_STATUS_HANDLER)(
    IN  NDIS_HANDLE             FilterModuleContext,
    IN  PNDIS_STATUS_INDICATION StatusIndication
    );


typedef
VOID
(*FILTER_OID_REQUEST_COMPLETE_HANDLER)(
    IN  NDIS_HANDLE             FilterModuleContext,
    IN  PNDIS_OID_REQUEST       OidRequest,
    IN  NDIS_STATUS             Status
    );

#define NDIS_FILTER_PARTIAL_CHARACTERISTICS_REVISION_1      1

typedef struct _NDIS_FILTER_PARTIAL_CHARACTERISTICS
{
    NDIS_OBJECT_HEADER                              Header; // Header.Type = NDIS_OBJECT_TYPE_FILTER_PARTIAL_CHARACTERISTICS
    ULONG                                           Flags;
    FILTER_SEND_NET_BUFFER_LISTS_HANDLER            SendNetBufferListsHandler;
    FILTER_SEND_NET_BUFFER_LISTS_COMPLETE_HANDLER   SendNetBufferListsCompleteHandler;
    FILTER_CANCEL_SEND_HANDLER                      CancelSendNetBufferListsHandler;
    FILTER_RECEIVE_NET_BUFFER_LISTS_HANDLER         ReceiveNetBufferListsHandler;
    FILTER_RETURN_NET_BUFFER_LISTS_HANDLER          ReturnNetBufferListsHandler;
} NDIS_FILTER_PARTIAL_CHARACTERISTICS, *PNDIS_FILTER_PARTIAL_CHARACTERISTICS;

#define NDIS_SIZEOF_FILTER_PARTIAL_CHARACTERISTICS_REVISION_1   \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_FILTER_PARTIAL_CHARACTERISTICS, ReturnNetBufferListsHandler)

//
// Filter driver flags
//
#define NDIS_FILTER_DRIVER_MANDATORY                0x00000001

#define NDIS_FILTER_CHARACTERISTICS_REVISION_1      1

typedef struct _NDIS_FILTER_DRIVER_CHARACTERISTICS
{
    NDIS_OBJECT_HEADER                              Header;
    UCHAR                                           MajorNdisVersion;
    UCHAR                                           MinorNdisVersion;
    UCHAR                                           MajorDriverVersion;
    UCHAR                                           MinorDriverVersion;
    ULONG                                           Flags;
    NDIS_STRING                                     FriendlyName;
    NDIS_STRING                                     UniqueName;
    NDIS_STRING                                     ServiceName;
    SET_OPTIONS_HANDLER                             SetOptionsHandler;
    FILTER_SET_FILTER_MODULE_OPTIONS_HANDLER        SetFilterModuleOptionsHandler;
    FILTER_ATTACH_HANDLER                           AttachHandler;
    FILTER_DETACH_HANDLER                           DetachHandler;
    FILTER_RESTART_HANDLER                          RestartHandler;
    FILTER_PAUSE_HANDLER                            PauseHandler;
    FILTER_SEND_NET_BUFFER_LISTS_HANDLER            SendNetBufferListsHandler;
    FILTER_SEND_NET_BUFFER_LISTS_COMPLETE_HANDLER   SendNetBufferListsCompleteHandler;
    FILTER_CANCEL_SEND_HANDLER                      CancelSendNetBufferListsHandler;
    FILTER_RECEIVE_NET_BUFFER_LISTS_HANDLER         ReceiveNetBufferListsHandler;
    FILTER_RETURN_NET_BUFFER_LISTS_HANDLER          ReturnNetBufferListsHandler;
    FILTER_OID_REQUEST_HANDLER                      OidRequestHandler;
    FILTER_OID_REQUEST_COMPLETE_HANDLER             OidRequestCompleteHandler;
    FILTER_CANCEL_OID_REQUEST_HANDLER               CancelOidRequestHandler;
    FILTER_DEVICE_PNP_EVENT_NOTIFY_HANDLER          DevicePnPEventNotifyHandler;
    FILTER_NET_PNP_EVENT_HANDLER                    NetPnPEventHandler;  
    FILTER_STATUS_HANDLER                           StatusHandler;
  
} NDIS_FILTER_DRIVER_CHARACTERISTICS, *PNDIS_FILTER_DRIVER_CHARACTERISTICS;
#define NDIS_SIZEOF_FILTER_DRIVER_CHARACTERISTICS_REVISION_1   \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_FILTER_DRIVER_CHARACTERISTICS, StatusHandler)

#define NDIS_FILTER_ATTRIBUTES_REVISION_1       1

typedef struct _NDIS_FILTER_ATTRIBUTES
{
    NDIS_OBJECT_HEADER                      Header;
    ULONG                                   Flags;
} NDIS_FILTER_ATTRIBUTES, *PNDIS_FILTER_ATTRIBUTES;

#define NDIS_SIZEOF_FILTER_ATTRIBUTES_REVISION_1    \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_FILTER_ATTRIBUTES, Flags)

EXPORT
NDIS_STATUS
NdisFRegisterFilterDriver(
    IN  PDRIVER_OBJECT                          DriverObject,
    IN  NDIS_HANDLE                             FilterDriverContext,
    IN  PNDIS_FILTER_DRIVER_CHARACTERISTICS     FilterDriverCharacteristics,
    OUT PNDIS_HANDLE                            NdisFilterDriverHandle
    );


EXPORT
VOID
NdisFDeregisterFilterDriver(
    IN NDIS_HANDLE                      NdisFilterDriverHandle
    );
    
EXPORT
NDIS_STATUS
NdisFSetAttributes(
    IN  NDIS_HANDLE                     NdisFilterHandle,
    IN  NDIS_HANDLE                     FilterModuleContext,
    IN  PNDIS_FILTER_ATTRIBUTES         FilterAttributes
    );    

EXPORT
NDIS_STATUS
NdisFRestartFilter(
    IN  NDIS_HANDLE             NdisFilterHandle
    );

EXPORT
VOID
NdisFSendNetBufferLists(
    IN  NDIS_HANDLE             NdisFilterHandle,
    IN  PNET_BUFFER_LIST        NetBufferList,
    IN  NDIS_PORT_NUMBER        PortNumber,
    IN  ULONG                   SendFlags
    );

EXPORT
VOID
NdisFReturnNetBufferLists(
    IN  NDIS_HANDLE             NdisFilterHandle,
    IN  PNET_BUFFER_LIST        NetBufferLists,
    IN  ULONG                   ReturnFlags
    );

EXPORT
VOID
NdisFSendNetBufferListsComplete(
    IN  NDIS_HANDLE             NdisFilterHandle,
    IN  PNET_BUFFER_LIST        NetBufferList,
    IN  ULONG                   SendCompleteFlags
    );

EXPORT
VOID
NdisFIndicateReceiveNetBufferLists(
    IN  NDIS_HANDLE             NdisFilterHandle,
    IN  PNET_BUFFER_LIST        NetBufferLists,
    IN  NDIS_PORT_NUMBER        PortNumber,
    IN  ULONG                   NumberOfNetBufferLists,
    IN  ULONG                   ReceiveFlags
    );

EXPORT
NDIS_STATUS
NdisFOidRequest(
    IN  NDIS_HANDLE             NdisFilterHandle,
    IN  PNDIS_OID_REQUEST       OidRequest
    );

EXPORT
VOID
NdisFOidRequestComplete(
    IN  NDIS_HANDLE             NdisFilterHandle,
    IN  PNDIS_OID_REQUEST       OidRequest,
    IN  NDIS_STATUS             Status
    );

EXPORT
VOID
NdisFIndicateStatus(
    IN  NDIS_HANDLE             NdisFilterHandle,
    IN  PNDIS_STATUS_INDICATION StatusIndication
    );


EXPORT
VOID
NdisFRestartComplete(
    IN  NDIS_HANDLE             NdisFilterHandle,
    IN  NDIS_STATUS             Status
    );

EXPORT
VOID
NdisFPauseComplete(
    IN  NDIS_HANDLE             NdisFilterHandle
    );


EXPORT
VOID
NdisFDevicePnPEventNotify(
    IN  NDIS_HANDLE             NdisFilterHandle,
    IN  PNET_DEVICE_PNP_EVENT   NetDevicePnPEvent
    );

EXPORT
NDIS_STATUS
NdisFNetPnPEvent(
    IN  NDIS_HANDLE                 NdisFilterHandle,
    IN  PNET_PNP_EVENT_NOTIFICATION NetPnPEventNotification
    );


EXPORT
VOID
NdisFCancelSendNetBufferLists(
    IN  NDIS_HANDLE     NdisFilterHandle,
    IN  PVOID           CancelId
    );
    
EXPORT
VOID
NdisFCancelOidRequest(
    IN  NDIS_HANDLE             NdisFilterHandle,
    IN  PVOID                   RequestId
    );

#endif // NDIS_SUPPORT_NDIS6

#pragma once

#if (NDIS_SUPPORT_NDIS6 || defined(NDIS60))

//
// NDIS IF data structures, function prototypes and macros
//

typedef
NDIS_STATUS
(*IFP_QUERY_OBJECT)(
    IN NDIS_HANDLE          ProviderIfContext,
    IN NET_IF_OBJECT_ID     ObjectId,
    IN OUT PULONG           pOutputBufferLength,
    OUT PVOID               pOutputBuffer
    );

typedef
NDIS_STATUS
(*IFP_SET_OBJECT)(
    IN NDIS_HANDLE          ProviderIfContext,
    IN NET_IF_OBJECT_ID     ObjectId,
    IN ULONG                InputBufferLength,
    IN PVOID                pInputBuffer
    );


typedef struct _NDIS_IF_PROVIDER_CHARACTERISTICS
{
    NDIS_OBJECT_HEADER      Header;

    //
    // Generic query and set handlers:
    //
    IFP_QUERY_OBJECT        QueryObjectHandler;
    IFP_SET_OBJECT          SetObjectHandler;

    PVOID                   Reserved1;
    PVOID                   Reserved2;

} NDIS_IF_PROVIDER_CHARACTERISTICS, *PNDIS_IF_PROVIDER_CHARACTERISTICS;

#define NDIS_SIZEOF_IF_PROVIDER_CHARACTERISTICS_REVISION_1   \
        RTL_SIZEOF_THROUGH_FIELD(NDIS_IF_PROVIDER_CHARACTERISTICS, Reserved2)

//
// NET_IF_INFORMATION is passed to NdisIfRegisterInterface
//
typedef struct _NET_IF_INFORMATION
{
    NDIS_OBJECT_HEADER      Header;
    ULONG                   Flags;              // Misc. information
    NET_PHYSICAL_LOCATION   PhysicalLocation;   // physical location on machine
    ULONG                   WanTunnelType;      // tunnelIfEncapsMethod (RFC 2667)
    ULONG                   PortNumber;         // for WAN devices.
    NET_IF_ACCESS_TYPE      AccessType;
    NET_IF_DIRECTION_TYPE   DirectionType;
    NET_IF_CONNECTION_TYPE  ConnectionType;
    BOOLEAN                 ifConnectorPresent;
    USHORT                  PhysAddressLength;  // in bytes (ifPhysAddress). this is -current- mac address
    USHORT                  PhysAddressOffset;  // from beginning of this struct
    USHORT                  PermanentPhysAddressOffset;  // from beginning of this struct
    //
    //  The "friendly name" represents ifDescr:
    //
    USHORT                  FriendlyNameLength; // in bytes
    USHORT                  FriendlyNameOffset; // from beginning of this struct
    GUID                    InterfaceGuid;
    NET_IF_NETWORK_GUID     NetworkGuid;
    ULONG                   SupportedStatistics;
    NDIS_MEDIUM             MediaType;
    NDIS_PHYSICAL_MEDIUM    PhysicalMediumType;    
} NET_IF_INFORMATION, *PNET_IF_INFORMATION;


#define NDIS_SIZEOF_NET_IF_INFORMATION_REVISION_1   \
        RTL_SIZEOF_THROUGH_FIELD(NET_IF_INFORMATION, PhysicalMediumType)

EXPORT
NDIS_STATUS
NdisIfRegisterProvider(
    IN  PNDIS_IF_PROVIDER_CHARACTERISTICS   ProviderCharacteristics,
    IN  NDIS_HANDLE                         IfProviderContext,
    OUT PNDIS_HANDLE                        pNdisIfProviderHandle
    );

EXPORT
VOID
NdisIfDeregisterProvider(
    IN NDIS_HANDLE          NdisProviderHandle
    );

EXPORT
NDIS_STATUS
NdisIfAllocateNetLuidIndex(
    IN  NET_IFTYPE          ifType,
    OUT PUINT32             pNetLuidIndex
    );

EXPORT
NDIS_STATUS
NdisIfFreeNetLuidIndex(
    IN NET_IFTYPE           ifType,
    IN UINT32               NetLuidIndex
    );

EXPORT
NDIS_STATUS
NdisIfRegisterInterface(
    IN NDIS_HANDLE          NdisProviderHandle,
    IN NET_LUID             NetLuid,
    IN NDIS_HANDLE          ProviderIfContext,
    IN PNET_IF_INFORMATION  pIfInfo,
    OUT PNET_IFINDEX        pfIndex
    );

EXPORT
VOID
NdisIfDeregisterInterface(
    IN NET_IFINDEX          ifIndex
    );

EXPORT
NDIS_STATUS
NdisIfGetInterfaceIndexFromNetLuid(
    IN  NET_LUID            NetLuid,
    OUT PNET_IFINDEX        pIfIndex
    );

EXPORT
NDIS_STATUS
NdisIfGetNetLuidFromInterfaceIndex(
    IN  NET_IFINDEX         ifIndex,
    OUT PNET_LUID           pNetLuid
    );

EXPORT
NDIS_STATUS
NdisIfQueryBindingIfIndex(
    IN  NDIS_HANDLE         NdisBindingHandle,
    OUT PNET_IFINDEX        pBoundIfIndex,
    OUT PNET_LUID           pBoundIfNetLuid,
    OUT PNET_IFINDEX        pLowestIfIndex,
    OUT PNET_LUID           pLowestIfNetLuid
    );

EXPORT
NDIS_STATUS
NdisIfAddIfStackEntry(
    IN NET_IFINDEX          HigherLayerIfIndex,
    IN NET_IFINDEX          LowerLayerIfIndex
    );

EXPORT
VOID
NdisIfDeleteIfStackEntry(
    IN NET_IFINDEX          HigherLayerIfIndex,
    IN NET_IFINDEX          LowerLayerIfIndex
    );

#endif // (NDIS_SUPPORT_NDIS6 || defined(NDIS60))

#endif // _NDIS_

