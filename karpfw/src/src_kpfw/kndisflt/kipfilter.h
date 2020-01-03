#pragma once
#include "ktdifilt_public.h"

const GUID GUID_NULL = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };

typedef struct tagPacketData
{
	PACKET_DIR      PacketDirection;
	PROTO_TYPE      ProtoType;
	ULONG           LocalAddress;
	ULONG           RemoteAddress;
	USHORT          LocalPort;
	USHORT          RemotePort;
	UCHAR           TcpFlag;
	UCHAR           IcmpType;
	UCHAR           IcmpCode;
	UCHAR           IgmpType;
	ULONG			IgmpAddr;
}PACKET_DATA, *PPACKET_DATA;


typedef struct tagTrafficCtrl
{
	ULONG uStartTime;
	ULONG uSendLimit;
	ULONG uRecvLimit;
	ULARGE_INTEGER i64SendBytes;
	ULARGE_INTEGER i64RecvBytes;
}TRAFFIC_CTRL, *PTRAFFIC_CTRL;

class KIpFilter
{
public:
	BOOLEAN Init( PDRIVER_OBJECT DriverObject = NULL, PUNICODE_STRING RegistryPath = NULL );
	BOOLEAN Uninit( PDRIVER_OBJECT DriverObject = NULL );

	IPRESULT Filter( PKANTIARP_ADAPTER adapter, PVOID pBuffer, ULONG uBufferLength, PACKET_DIR enumDir );
	NTSTATUS EnableIpRule( PVOID pGuidBuffer, ULONG uBufferLength, BOOLEAN bEnable );
	NTSTATUS AddIpRule( PVOID pRuleInfoBuffer, ULONG uBufferLength );
	NTSTATUS RemoveIpRule( PVOID pGuidBuffer, ULONG uBufferLength );
	NTSTATUS ClearIpRule();
	VOID SetPassGuid( GUID guidPass );
	static ULONG GetIpHeader( PEHHDR pEhHdr, ULONG uBufferLen, PIPHEADER *ppIpHdr );
	PVOID GetTdiInterface();
	BOOLEAN AddPortTrafficCtrl( USHORT uPort, ULONG uSendSpeed, ULONG uRecvSpeed );

	//NTSTATUS GetPortFunction( OUT UCHAR *pOutBuffer, IN ULONG uOutBufferLen, OUT ULONG *uResultLen );
	//NTSTATUS GetOpenPort( OUT UCHAR *pOutBuffer, IN ULONG uOutBufferLen, OUT ULONG *uResultLen );
	//static VOID __stdcall OpenPort( ULONG protoType, USHORT uPort );
	//static VOID __stdcall ClosePort( ULONG protoType, USHORT uPort );
private:
	typedef BOOLEAN (* PFNMatchRule)(PFILTER_INFO ipRule, PPACKET_DATA pPktData, LOG_IP_RULE* );
	
	BOOLEAN ReadPacketData(PIPHEADER pIpHdr, ULONG size, PACKET_DIR enumDir, PPACKET_DATA pPktData);
	BOOLEAN MatchIpRule(  PPACKET_DATA pPacketData,  FILTER_INFO* ruleInfo, LOG_IP_RULE *pIpLog );
	VOID SetIpRuleEnable( KDList< GUID, KIP_FILTER_INFO* >* ruleList, GUID ruleId, BOOLEAN bEnable, NTSTATUS *status );
	VOID RemoveIpRule( KDList< GUID, KIP_FILTER_INFO* >* ruleList, GUID ruleId, NTSTATUS *status );
	VOID AddIpRule( KDList< GUID, KIP_FILTER_INFO* >* ruleList, KIP_FILTER_INFO* irRule );
	VOID ClearList( KDList< GUID, KIP_FILTER_INFO* > *pList );
	KDList< GUID, KIP_FILTER_INFO* >* GetRuleListFromType( PROTO_TYPE protoType, PACKET_DIR enumDir, PFNMatchRule* pMatchFunc );
	static BOOLEAN MatchIpAddr( PFILTER_IP_ADDR ruleIp, ULONG ip );
	static BOOLEAN MatchPort( PFILTER_PORT rulePort, USHORT port );
	static BOOLEAN MatchTcpFlag( UCHAR ruleFlag, unsigned char flag );
	static BOOLEAN MatchIpRule( PFILTER_INFO ipRule, PPACKET_DATA pPktData, LOG_IP_RULE* pLog );
	static BOOLEAN MatchTcpRule( PFILTER_INFO tcpRule, PPACKET_DATA pPktData, LOG_IP_RULE* pLog );
	static BOOLEAN MatchUdpRule( PFILTER_INFO udpRule, PPACKET_DATA pPktData, LOG_IP_RULE* pLog );
	static BOOLEAN MatchIcmpRule( PFILTER_INFO icmpRule, PPACKET_DATA pPktData, LOG_IP_RULE* pLog );
	static BOOLEAN MatchIgmpRule( PFILTER_INFO icmpRule, PPACKET_DATA pPktData, LOG_IP_RULE* pLog );
	
	BOOLEAN TrafficCtrl( PPACKET_DATA pPacketData, ULONG uBufferLen );
	BOOLEAN MatchPortOpen( PPACKET_DATA pPacketData, LOG_IP_RULE* pLog );
	BOOLEAN StartGetFuncThread();
	static VOID GetFunction( PVOID StartContext );
	
private:
	KDList< GUID, KIP_FILTER_INFO* >  m_udpSendList;
	KDList< GUID, KIP_FILTER_INFO* >  m_udpRecvList;
	KDList< GUID, KIP_FILTER_INFO* >  m_tcpSendList;
	KDList< GUID, KIP_FILTER_INFO* >  m_tcpRecvList;
	KDList< GUID, KIP_FILTER_INFO* >  m_icmpSendList;
	KDList< GUID, KIP_FILTER_INFO* >  m_icmpRecvList;
	KDList< GUID, KIP_FILTER_INFO* >  m_igmpSendList;
	KDList< GUID, KIP_FILTER_INFO* >  m_igmpRecvList;
	KDList< GUID, KIP_FILTER_INFO* >  m_ipSendList;
	KDList< GUID, KIP_FILTER_INFO* >  m_ipRecvList;
	GUID m_guidPass;
	NDIS_PORT_FUNC m_portFunc;
};
