/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    passthru.h

Abstract:

    Ndis Intermediate Miniport driver sample. This is a passthru driver.

Author:

Environment:


Revision History:

 
--*/

#ifdef NDIS51_MINIPORT
#define PASSTHRU_MAJOR_NDIS_VERSION            5
#define PASSTHRU_MINOR_NDIS_VERSION            1
#else
#define PASSTHRU_MAJOR_NDIS_VERSION            4
#define PASSTHRU_MINOR_NDIS_VERSION            0
#endif

#ifdef NDIS51
#define PASSTHRU_PROT_MAJOR_NDIS_VERSION    5
#define PASSTHRU_PROT_MINOR_NDIS_VERSION    0
#else
#define PASSTHRU_PROT_MAJOR_NDIS_VERSION    4
#define PASSTHRU_PROT_MINOR_NDIS_VERSION    0
#endif

#define MAX_BUNDLEID_LENGTH 50

#define TAG 'ImPa'
#define WAIT_INFINITE 0



//advance declaration
typedef struct _ADAPT KANTIARP_ADAPTER, *PKANTIARP_ADAPTER;

extern "C"
NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT            DriverObject,
    IN PUNICODE_STRING           RegistryPath
    );

#pragma NDIS_INIT_FUNCTION(DriverEntry)

DRIVER_DISPATCH PtDispatch;
NTSTATUS
PtDispatch(
    IN PDEVICE_OBJECT            DeviceObject,
    IN PIRP                      Irp
    );

NDIS_STATUS
PtRegisterDevice(
    VOID
    );

NDIS_STATUS
PtDeregisterDevice(
    VOID
   );

DRIVER_UNLOAD PtUnload;
VOID
PtUnloadProtocol(
    VOID
    );

//
// Protocol proto-types
//
extern
VOID
PtOpenAdapterComplete(
    IN NDIS_HANDLE                ProtocolBindingContext,
    IN NDIS_STATUS                Status,
    IN NDIS_STATUS                OpenErrorStatus
    );

extern
VOID
PtCloseAdapterComplete(
    IN NDIS_HANDLE                ProtocolBindingContext,
    IN NDIS_STATUS                Status
    );

extern
VOID
PtResetComplete(
    IN NDIS_HANDLE                ProtocolBindingContext,
    IN NDIS_STATUS                Status
    );

extern
VOID
PtRequestComplete(
    IN NDIS_HANDLE                ProtocolBindingContext,
    IN PNDIS_REQUEST              NdisRequest,
    IN NDIS_STATUS                Status
    );

extern
VOID
PtStatus(
    IN NDIS_HANDLE                ProtocolBindingContext,
    IN NDIS_STATUS                GeneralStatus,
    IN PVOID                      StatusBuffer,
    IN UINT                       StatusBufferSize
    );

extern
VOID
PtStatusComplete(
    IN NDIS_HANDLE                ProtocolBindingContext
    );

extern
VOID
PtSendComplete(
    IN NDIS_HANDLE                ProtocolBindingContext,
    IN PNDIS_PACKET               Packet,
    IN NDIS_STATUS                Status
    );

extern
VOID
PtTransferDataComplete(
    IN NDIS_HANDLE                ProtocolBindingContext,
    IN PNDIS_PACKET               Packet,
    IN NDIS_STATUS                Status,
    IN UINT                       BytesTransferred
    );

extern
NDIS_STATUS
PtReceive(
    IN NDIS_HANDLE                ProtocolBindingContext,
    IN NDIS_HANDLE                MacReceiveContext,
    IN PVOID                      HeaderBuffer,
    IN UINT                       HeaderBufferSize,
    IN PVOID                      LookAheadBuffer,
    IN UINT                       LookaheadBufferSize,
    IN UINT                       PacketSize
    );

extern
VOID
PtReceiveComplete(
    IN NDIS_HANDLE                ProtocolBindingContext
    );

extern
INT
PtReceivePacket(
    IN NDIS_HANDLE                ProtocolBindingContext,
    IN PNDIS_PACKET               Packet
    );

extern
VOID
PtBindAdapter(
    OUT PNDIS_STATUS              Status,
    IN  NDIS_HANDLE               BindContext,
    IN  PNDIS_STRING              DeviceName,
    IN  PVOID                     SystemSpecific1,
    IN  PVOID                     SystemSpecific2
    );

extern
VOID
PtUnbindAdapter(
    OUT PNDIS_STATUS              Status,
    IN  NDIS_HANDLE               ProtocolBindingContext,
    IN  NDIS_HANDLE               UnbindContext
    );
    
VOID
PtUnload(
    IN PDRIVER_OBJECT             DriverObject
    );



extern 
NDIS_STATUS
PtPNPHandler(
    IN NDIS_HANDLE                ProtocolBindingContext,
    IN PNET_PNP_EVENT             pNetPnPEvent
    );




NDIS_STATUS
PtPnPNetEventReconfigure(
    IN PKANTIARP_ADAPTER            pAdapt,
    IN PNET_PNP_EVENT    pNetPnPEvent
    );    

NDIS_STATUS 
PtPnPNetEventSetPower (
    IN PKANTIARP_ADAPTER                    pAdapt,
    IN PNET_PNP_EVENT            pNetPnPEvent
    );
    

//
// Miniport proto-types
//
NDIS_STATUS
MPInitialize(
    OUT PNDIS_STATUS             OpenErrorStatus,
    OUT PUINT                    SelectedMediumIndex,
    IN PNDIS_MEDIUM              MediumArray,
    IN UINT                      MediumArraySize,
    IN NDIS_HANDLE               MiniportAdapterHandle,
    IN NDIS_HANDLE               WrapperConfigurationContext
    );

VOID
MPSendPackets(
    IN NDIS_HANDLE                MiniportAdapterContext,
    IN PPNDIS_PACKET              PacketArray,
    IN UINT                       NumberOfPackets
    );

NDIS_STATUS
MPSend(
    IN NDIS_HANDLE                MiniportAdapterContext,
    IN PNDIS_PACKET               Packet,
    IN UINT                       Flags
    );

NDIS_STATUS
MPQueryInformation(
    IN NDIS_HANDLE                MiniportAdapterContext,
    IN NDIS_OID                   Oid,
    IN PVOID                      InformationBuffer,
    IN ULONG                      InformationBufferLength,
    OUT PULONG                    BytesWritten,
    OUT PULONG                    BytesNeeded
    );

NDIS_STATUS
MPSetInformation(
    IN NDIS_HANDLE                                      MiniportAdapterContext,
    IN NDIS_OID                                         Oid,
    __in_bcount(InformationBufferLength) IN PVOID       InformationBuffer,
    IN ULONG                                            InformationBufferLength,
    OUT PULONG                                          BytesRead,
    OUT PULONG                                          BytesNeeded
    );

VOID
MPReturnPacket(
    IN NDIS_HANDLE                MiniportAdapterContext,
    IN PNDIS_PACKET               Packet
    );

NDIS_STATUS
MPTransferData(
    OUT PNDIS_PACKET              Packet,
    OUT PUINT                     BytesTransferred,
    IN NDIS_HANDLE                MiniportAdapterContext,
    IN NDIS_HANDLE                MiniportReceiveContext,
    IN UINT                       ByteOffset,
    IN UINT                       BytesToTransfer
    );

VOID
MPHalt(
    IN NDIS_HANDLE                MiniportAdapterContext
    );


VOID
MPQueryPNPCapabilities(  
    OUT PKANTIARP_ADAPTER                    MiniportProtocolContext, 
    OUT PNDIS_STATUS              Status
    );


#ifdef NDIS51_MINIPORT

VOID
MPCancelSendPackets(
    IN NDIS_HANDLE            MiniportAdapterContext,
    IN PVOID                  CancelId
    );

VOID
MPAdapterShutdown(
    IN NDIS_HANDLE                MiniportAdapterContext
    );

VOID
MPDevicePnPEvent(
    IN NDIS_HANDLE                MiniportAdapterContext,
    IN NDIS_DEVICE_PNP_EVENT      DevicePnPEvent,
    IN PVOID                      InformationBuffer,
    IN ULONG                      InformationBufferLength
    );

#endif // NDIS51_MINIPORT

VOID
MPFreeAllPacketPools(
    IN PKANTIARP_ADAPTER                    pAdapt
    );


VOID
MPProcessSetPowerOid(
    IN OUT PNDIS_STATUS                             pNdisStatus,
    IN PKANTIARP_ADAPTER                                       pAdapt,
    __in_bcount(InformationBufferLength) IN PVOID   InformationBuffer,
    IN ULONG                                        InformationBufferLength,
    OUT PULONG                                      BytesRead,
    OUT PULONG                                      BytesNeeded
    );

VOID
PtReferenceAdapt(
    IN PKANTIARP_ADAPTER     pAdapt
    );

BOOLEAN
PtDereferenceAdapt(
    IN PKANTIARP_ADAPTER     pAdapt
    );

//
// There should be no DbgPrint's in the Free version of the driver
//
#if DBG

#define DBGPRINT(Fmt)                                        \
    {                                                        \
        DbgPrint("KAntiarp: ");                                \
        DbgPrint Fmt;                                        \
    }

#else // if DBG

#define DBGPRINT(Fmt)                                            

#endif // if DBG 

#define    NUM_PKTS_IN_POOL    256


//
// Protocol reserved part of a sent packet that is allocated by us.
//
typedef struct _SEND_RSVD
{
    PNDIS_PACKET    OriginalPkt;
	BOOLEAN			bNeedFree;	//added by Yzh
} SEND_RSVD, *PSEND_RSVD;

//
// Miniport reserved part of a received packet that is allocated by
// us. Note that this should fit into the MiniportReserved space
// in an NDIS_PACKET.
//
typedef struct _RECV_RSVD
{
    PNDIS_PACKET    OriginalPkt;
	SHORT			uPackSize;
	SHORT			uNeedSize;
} RECV_RSVD, *PRECV_RSVD;

C_ASSERT(sizeof(RECV_RSVD) <= sizeof(((PNDIS_PACKET)0)->MiniportReserved));

//
// Event Codes related to the PassthruEvent Structure
//

typedef enum 
{
    Passthru_Invalid,
    Passthru_SetPower,
    Passthru_Unbind

} PASSSTHRU_EVENT_CODE, *PPASTHRU_EVENT_CODE; 

//
// Passthru Event with  a code to state why they have been state
//

typedef struct _PASSTHRU_EVENT
{
    NDIS_EVENT Event;
    PASSSTHRU_EVENT_CODE Code;

} PASSTHRU_EVENT, *PPASSTHRU_EVENT;

#define MAX_ARP_CACHE 128
typedef struct _ARP_CACHE
{
	UCHAR GatewayMac[6];
	volatile long nRecvCount;
}ARP_CACHE, *PARP_CACHE;

//
// Structure used by both the miniport as well as the protocol part of the intermediate driver
// to represent an adapter and its corres. lower bindings
//
typedef struct _ADAPT
{
	struct _ADAPT *                Next;

	NDIS_HANDLE                    BindingHandle;    // To the lower miniport
	NDIS_HANDLE                    MiniportHandle;    // NDIS Handle to for miniport up-calls
	NDIS_HANDLE                    SendPacketPoolHandle;
	NDIS_HANDLE                    RecvPacketPoolHandle;
	NDIS_STATUS                    Status;            // Open Status
	NDIS_EVENT                     Event;            // Used by bind/halt for Open/Close Adapter synch.
	NDIS_MEDIUM                    Medium;
	NDIS_REQUEST                   Request;        // This is used to wrap a request coming down
	// to us. This exploits the fact that requests
	// are serialized down to us.
	PULONG                         BytesNeeded;
	PULONG                         BytesReadOrWritten;
	BOOLEAN                        ReceivedIndicationFlags[32];

	BOOLEAN                        OutstandingRequests;      // TRUE iff a request is pending
	// at the miniport below
	BOOLEAN                        QueuedRequest;            // TRUE iff a request is queued at
	// this IM miniport

	BOOLEAN                        StandingBy;                // True - When the miniport or protocol is transitioning from a D0 to Standby (>D0) State
	BOOLEAN                        UnbindingInProcess;
	NDIS_SPIN_LOCK                 Lock;
	// False - At all other times, - Flag is cleared after a transition to D0
	NDIS_DEVICE_POWER_STATE		   MiniportPowerState;
	NDIS_DEVICE_POWER_STATE        PTDeviceState;            // Protocol's Device State 
	NDIS_STRING                    DeviceName;                // For initializing the miniport edge
	NDIS_EVENT                     MiniportInitEvent;        // For blocking UnbindAdapter while
	// an IM Init is in progress.
	BOOLEAN                        MiniportInitPending;    // TRUE iff IMInit in progress
	NDIS_STATUS                    LastIndicatedStatus;    // The last indicated media status
	NDIS_STATUS                    LatestUnIndicateStatus; // The latest suppressed media status
	ULONG                          OutstandingSends;
	LONG                           RefCount;
	BOOLEAN                        MiniportIsHalted;

	//add by Yzh
	NDIS_STRING                     UpperDeviceName;                 // saved upper Device name
	
	NDIS_HANDLE						hSendBufferPool;
	NDIS_HANDLE						hRecvBufferPool;			
	NDIS_EVENT						hQueryEvent;
	BOOLEAN							bGetAddr;
	ULONG							LocalIP[MAX_ADAPTER_IP_NUM];
	UCHAR							LocalMac[6];
	ULONG							GatewayIP;
	UCHAR							GatewayMac[6];
	LARGE_INTEGER					InTraffic;
	LARGE_INTEGER					OutTraffic;
	NDIS_PHYSICAL_MEDIUM			physicalMedium;
	BOOLEAN							bRequestGatewayMac;
	BOOLEAN							bManualGatewayMac;
	PARP_CACHE						arpCache;
	//end

} KANTIARP_ADAPTER, *PKANTIARP_ADAPTER;

// global (per driver) data block
typedef struct _KANTIARP_DATA
{
	PKANTIARP_ADAPTER   AdapterList;

	NDIS_HANDLE         DriverHandle;
	NDIS_HANDLE         ProtocolHandle;

	NDIS_HANDLE         NdisWrapperHandle;
	
	NDIS_SPIN_LOCK		GlobalLock;
	LONG				MiniportCount;
	
	PDRIVER_OBJECT		pDriverObject;
	UNICODE_STRING		RegistryPath;	//add by Yzh
} KANTIARP_DATA, *PKANTIARP_DATA;

extern KANTIARP_DATA g_Data;


extern    NDIS_MEDIUM                        MediumArray[4];



#define ADAPT_MINIPORT_HANDLE(_pAdapt)    ((_pAdapt)->MiniportHandle)
#define ADAPT_DECR_PENDING_SENDS(_pAdapt)     \
    {                                         \
        NdisAcquireSpinLock(&(_pAdapt)->Lock);   \
        (_pAdapt)->OutstandingSends--;           \
        NdisReleaseSpinLock(&(_pAdapt)->Lock);   \
    }

//
// Custom Macros to be used by the passthru driver 
//
/*
BOOLEAN
IsIMDeviceStateOn(
   PADAPT 
   )

*/
#define IsIMDeviceStateOn(_pP)        ((_pP)->MiniportPowerState == NdisDeviceStateD0 && (_pP)->PTDeviceState == NdisDeviceStateD0 ) 

//////////////////////////////////////////////////////////////////////////

#define DBG_NONE            0
#define DBG_ERR             1
#define DBG_WARN            2
#define DBG_TRACE           3
#define DBG_INFO            4
#define DBG_VERB            5

#ifdef KANTIARP_WMI_TRACE

/*
tracepdb -f objchk_wxp_x86\i386\KAntiarp.pdb -p C:\KAntiarp
SET TRACE_FORMAT_SEARCH_PATH=C:\KAntiarp

tracelog -start KAntiarp -guid KAntiarp.ctl -f KAntiarp.log -flags 0x7FFFFFFF -level 5
tracelog -stop KAntiarp

tracefmt -o KAntiarp.txt -f KAntiarp.log
*/

#define WPP_AREA_LEVEL_LOGGER(Area,Lvl)           WPP_LEVEL_LOGGER(Area)
#define WPP_AREA_LEVEL_ENABLED(Area,Lvl)          (WPP_LEVEL_ENABLED(Area) && WPP_CONTROL(WPP_BIT_##Area).Level >= Lvl)

#define WPP_CONTROL_GUIDS \
	WPP_DEFINE_CONTROL_GUID(KAntiarp,(1A1C68BB,536A,4D51,B7A6,D1730BD7731E), \
	WPP_DEFINE_BIT(DBG_GENERAL)                 /* bit  0 = 0x00000001 */ \
	WPP_DEFINE_BIT(DBG_PNP)                     /* bit  1 = 0x00000002 */ \
	WPP_DEFINE_BIT(DBG_POWER)                   /* bit  2 = 0x00000004 */ \
	WPP_DEFINE_BIT(DBG_COUNT)                   /* bit  3 = 0x00000008 */ \
	WPP_DEFINE_BIT(DBG_CREATECLOSE)             /* bit  4 = 0x00000010 */ \
	WPP_DEFINE_BIT(DBG_WMI)                     /* bit  5 = 0x00000020 */ \
	WPP_DEFINE_BIT(DBG_UNLOAD)                  /* bit  6 = 0x00000040 */ \
	WPP_DEFINE_BIT(DBG_IO)                      /* bit  7 = 0x00000080 */ \
	WPP_DEFINE_BIT(DBG_INIT)                    /* bit  8 = 0x00000100 */ \
	WPP_DEFINE_BIT(DBG_09)                      /* bit  9 = 0x00000200 */ \
	WPP_DEFINE_BIT(DBG_10)                      /* bit 10 = 0x00000400 */ \
	WPP_DEFINE_BIT(DBG_11)                      /* bit 11 = 0x00000800 */ \
	WPP_DEFINE_BIT(DBG_12)                      /* bit 12 = 0x00001000 */ \
	WPP_DEFINE_BIT(DBG_13)                      /* bit 13 = 0x00002000 */ \
	WPP_DEFINE_BIT(DBG_14)                      /* bit 14 = 0x00004000 */ \
	WPP_DEFINE_BIT(DBG_15)                      /* bit 15 = 0x00008000 */ \
	WPP_DEFINE_BIT(DBG_16)                      /* bit 16 = 0x00010000 */ \
	WPP_DEFINE_BIT(DBG_17)                      /* bit 17 = 0x00020000 */ \
	WPP_DEFINE_BIT(DBG_18)                      /* bit 18 = 0x00040000 */ \
	WPP_DEFINE_BIT(DBG_19)                      /* bit 19 = 0x00080000 */ \
	WPP_DEFINE_BIT(DBG_20)                      /* bit 20 = 0x00100000 */ \
	WPP_DEFINE_BIT(DBG_21)                      /* bit 21 = 0x00200000 */ \
	WPP_DEFINE_BIT(DBG_22)                      /* bit 22 = 0x00400000 */ \
	WPP_DEFINE_BIT(DBG_23)                      /* bit 23 = 0x00800000 */ \
	WPP_DEFINE_BIT(DBG_24)                      /* bit 24 = 0x01000000 */ \
	WPP_DEFINE_BIT(DBG_25)                      /* bit 25 = 0x02000000 */ \
	WPP_DEFINE_BIT(DBG_26)                      /* bit 26 = 0x04000000 */ \
	WPP_DEFINE_BIT(DBG_27)                      /* bit 27 = 0x08000000 */ \
	WPP_DEFINE_BIT(DBG_28)                      /* bit 28 = 0x10000000 */ \
	WPP_DEFINE_BIT(DBG_29)                      /* bit 29 = 0x20000000 */ \
	WPP_DEFINE_BIT(DBG_30)                      /* bit 30 = 0x40000000 */ \
	WPP_DEFINE_BIT(DBG_31)                      /* bit 31 = 0x80000000 */ \
	)

#else

// definition of debug areas

#define DBG_GENERAL         (1 << 0)
#define DBG_PNP             (1 << 1)
#define DBG_POWER           (1 << 2)
#define DBG_COUNT           (1 << 3)
#define DBG_CREATECLOSE     (1 << 4)
#define DBG_WMI             (1 << 5)
#define DBG_UNLOAD          (1 << 6)
#define DBG_IO              (1 << 7)
#define DBG_INIT            (1 << 8)
#define DBG_KFW             (1 << 9)

#define DBG_ALL             0xFFFFFFFF

#if DBG

//VOID KAntiarpDebugPrint(
//						IN ULONG    Area,
//						IN ULONG    Level,
//						IN PCCHAR   Format,
//						IN          ...
//						);

#else	// !DBG

//__inline VOID KAntiarpDebugPrint(
//								 IN ULONG    Area,
//								 IN ULONG    Level,
//								 IN PCCHAR   Format,
//								 IN          ...
//								 )
//{
//}

#define KAntiarpDumpIrp(Irp)
#define IrpMajorFunctionString(MajorFunction)
#define PnPMinorFunctionString(MinorFunction)
#define PowerMinorFunctionString(MinorFunction)
#define SystemPowerStateString(SystemState) 0
#define DevicePowerStateString(DeviceState) 0
#define WMIMinorFunctionString(MinorFunction)
#define KAntiarpTdiDumpAddress(Address)

#endif	// DBG
#endif	// KANTIARP_WMI_TRACE

ULONG htonl(ULONG hostlong);
ULONG ntohl(ULONG netlong);
USHORT htons(USHORT hostshort);
USHORT ntohs(USHORT netshort);
ULONG inet_aton(const WCHAR *cp, ULONG *addr);