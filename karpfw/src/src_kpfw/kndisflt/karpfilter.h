#pragma once

class KArpFilter
{
public:
	BOOLEAN Init( PDRIVER_OBJECT DriverObject = NULL, PUNICODE_STRING RegistryPath = NULL );
	BOOLEAN Uninit( PDRIVER_OBJECT DriverObject = NULL );
	NTSTATUS BindAddr( PVOID pListBuffer, ULONG uBufferLength );
	NTSTATUS ClearBindAddr();
	IPRESULT Filter( PKANTIARP_ADAPTER adapter, PVOID pBuffer, ULONG uBufferLength, PACKET_DIR enumDir );
	IPRESULT FilterSend( PKANTIARP_ADAPTER adapter, PARPPACKET pArpPack, PacketAttackType *br );
	IPRESULT FilterRecv( PKANTIARP_ADAPTER adapter, PARPPACKET pArpPack, PacketAttackType *br );
	NTSTATUS SetBlockAttack( PVOID pInBuffer, ULONG uBufferLength  );
	ULONG GetBlockFlag();
	BOOLEAN GetProtectMac( ULONG uIP, UCHAR* pMac );
	BOOLEAN SetNotifyTimer( LONG uTimer );
private:
	IPRESULT IsBindAddr( ULONG uIP, UCHAR* pMac );
	BOOLEAN IsGratuitousArp( PARPPACKET pArpPack, UCHAR *pMac );
	BOOLEAN IsRequestGatewayMac( PKANTIARP_ADAPTER adapter, PARPPACKET pArpPack );
	static VOID ThreadSendArp( PVOID StartContext );
private:
	KWDMMap< __int64, TRUE, max_filter_count > m_ProtectList;
	ULONG m_uBlockFlag;
	volatile LONG m_uSendArpTimer;
	KWdmThread m_threadSendArp;
	KEVENT  m_eventUnload;
};
