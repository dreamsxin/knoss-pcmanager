#pragma once

#include "antiarp/arpcommondef.h"
#include "keventsender.h"
#include "karpfilter.h"
#include "kdhcpfilter.h"
#include "kadvipfilter.h"
#include "kipfilter.h"
#include "krasconn.h"

#define MAX_CACHE_LOGS	3
#define IOCTL_TRANSFER_TYPE(ioctl)	((ioctl) & 3)

//struct WorkItemContext
//{
//	NDIS_WORK_ITEM	workItem;
//	PVOID pThis;
//	PVOID pAdapter;
//};

class KNdisFirewall
{
public:
	BOOLEAN Init( PDRIVER_OBJECT DriverObject = NULL, PUNICODE_STRING RegistryPath = NULL );
	BOOLEAN Uninit( PDRIVER_OBJECT DriverObject = NULL);
	
	static NTSTATUS DeviceDispatch ( __in PDEVICE_OBJECT DeviceObject, __in PIRP irp );

	IPRESULT FilterPackets( PKANTIARP_ADAPTER adapter, PNDIS_PACKET localPacket, PACKET_DIR enumDir );
	IPRESULT Filter( PKANTIARP_ADAPTER adapter, PVOID pBuffer, ULONG uBufferLength, PACKET_DIR enumDir );	

	static BOOLEAN GetAdapterMac( PKANTIARP_ADAPTER pAdapter );
	static BOOLEAN GetAdapterPhysicalMedium( PKANTIARP_ADAPTER pAdapter );
	static BOOLEAN GetAdapterStatus( PKANTIARP_ADAPTER pAdapter );

	BOOLEAN IncRecvArp( PKANTIARP_ADAPTER adapter, ULONG uIP,  UCHAR* pMac );
	VOID DecRecvArp( PKANTIARP_ADAPTER adapter );
	IPRESULT IsGateway( PKANTIARP_ADAPTER adapter, ULONG uIP,  UCHAR* pMac );

	BOOLEAN CompMacIp( PKANTIARP_ADAPTER adapter, ULONG uIP, UCHAR *pMac );	
	BOOLEAN CompIpMac( PKANTIARP_ADAPTER adapter, ULONG uIP, UCHAR *pMac );

	BOOLEAN SendArpLog( PKANTIARP_ADAPTER adapter, PARPPACKET pArpPack, PacketAttackType blockReason, PACKET_DIR enumDir );
	BOOLEAN SendIpLog( LOG_IP_RULE *ipLog );
	BOOLEAN SendAdvIpLog( LOG_IP_RULE *ipLog );

	BOOLEAN IsModuleEnable( ULONG uModuleCode );
	NTSTATUS LoadConfig( PUNICODE_STRING pRegisterPath );

	BOOLEAN BindMacForGateway();
	BOOLEAN UnBindMacForGateway();
	NDIS_STATUS SendUserPacket( PKANTIARP_ADAPTER adapter, PVOID pBuffer, ULONG uBufferLength );
	ULONG GetArpBlockFlag();
	
	VOID SetAddrEvent();
	VOID WanLineUp( PNDIS_WAN_LINE_UP pNdisWanLineUp );
	VOID WanLineDown( PNDIS_WAN_LINE_DOWN pNdisWanLineDown );
	VOID NotifyRasEvent( PRAS_CONNECTION pRasConn );

private:
	template<class T>
	BOOLEAN CreateAndInit(T* &pInitT,  PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath );

	template<class T>
	BOOLEAN UninitAndDel(T* &pInitT,  PDRIVER_OBJECT DriverObject );
	
	BOOLEAN PrivateInit( PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath );
	BOOLEAN PrivateUninit( PDRIVER_OBJECT DriverObject );
	
	NTSTATUS HandleIRP( PIRP pIrp );
	NTSTATUS ProcessRequest ( PIRP irp, ULONG uIoCtlCode, UCHAR *pOutBuffer, ULONG uInBufferLen, UCHAR *pInBuffer, ULONG uOutBufferLen, ULONG *uResultLen );
	NTSTATUS GetNdisState( UCHAR *uOutBuffer, ULONG uBufferLen, ULONG *puResultLen );
	VOID EnableNdisKfw( BOOLEAN bEnable );
	ULONG IsKfwEnable();
	ULONG ReadPacket( PNDIS_PACKET pPacket, PVOID pBuffer, ULONG size );

	NTSTATUS EnumAdapter( PVOID pBuffer, ULONG uBufferLength, ULONG *puResultLen );
	NTSTATUS GetRegistryKeyValue(
		HANDLE Handle,
		const PWCHAR KeyNameString,
		ULONG KeyNameStringLength,
		PVOID Data,
		ULONG DataLength,
		PULONG Type,
		PULONG RetrunLength);
	
	NTSTATUS GetIpFromReg( PUNICODE_STRING pDeviceName, ULONG *pLocalIp, ULONG *pGatewayIP );
	BOOLEAN SetAdapterIPFromReg( PKANTIARP_ADAPTER adapter );
	VOID SetAdapterIp( PKANTIARP_ADAPTER adapter, ULONG *uLocalIP, ULONG uGatewayIP );
	BOOLEAN WaitForGetAddrEvent();
	BOOLEAN IsEqualLog( LOG_IP_RULE* ipLog1, LOG_IP_RULE* ipLog2 );
	VOID CacheLogs( LOG_IP_RULE* ipLog );
	static VOID ThreadGetAddr( PVOID StartContext );
	static VOID ThreadSendLogs( PVOID StartContext );
	NTSTATUS CleanGatewayMac();
	NTSTATUS SelectGatewayMac();
private:

	KArpFilter *m_arpFilter;
	KEventSender *m_eventSender;
	KAdvIpFilter *m_advIpFilter;
	KIpFilter *m_ipFilter;
	KRasConnection *m_rasConnections; 

    volatile LONG m_uEnableNetwork;
	ULONG m_uLoadFlag;
	volatile LONG m_uEnableFlag;
	volatile LONG m_uRequestFlag;
	volatile LONG m_uArpBlockFlag;
	volatile LONG m_uAcquireList;
	volatile LONG m_bInit;

	PDEVICE_OBJECT  m_devControl;
	NDIS_HANDLE m_NdisDeviceHandle;

	KWdmThread m_threadGetAddr;
	KWdmThread m_threadSendLogs;
	HANDLE m_kfwServiceID;

	KSPIN_LOCK m_cacheLogsLock;
	LOG_IP_RULE m_cacheLogs[MAX_CACHE_LOGS];
	volatile LONG m_cacheLogIdx;
};


extern KNdisFirewall *g_ndisFirewall;