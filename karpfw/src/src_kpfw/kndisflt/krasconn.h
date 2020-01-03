///////////////////////////////////////////////////////////////	
//	
// FileName  :  krasconn.h
// Creator   :  yangzhenhui@kingsoft.com
// Date    	 :  2009-09-21  15:07
// Comment   :  
//
///////////////////////////////////////////////////////////////	
#pragma once


class KRasConnection
{
public:
	BOOLEAN Init( PDRIVER_OBJECT DriverObject = NULL, PUNICODE_STRING RegistryPath = NULL );
	BOOLEAN Uninit( PDRIVER_OBJECT DriverObject = NULL );
	VOID LineUp( PNDIS_WAN_LINE_UP pNdisWanLineUp );
	VOID LineDown( PNDIS_WAN_LINE_DOWN pNdisWanLineDown );
	VOID PassThru( PKANTIARP_ADAPTER adapter, PVOID pBuffer, ULONG uBufferLength, PACKET_DIR enumDir );
	NTSTATUS EnumRasConnection( PVOID pBuffer, ULONG uBufferLength, ULONG *puResultLen );
private:
	VOID AddInTraffic( UCHAR* uRemoteAddress, ULONG uBytes );
	VOID AddOutTraffic( UCHAR* uRemoteAddress, ULONG uBytes );

	KDList <__int64, PRAS_CONNECTION> m_rasConnList;
};
