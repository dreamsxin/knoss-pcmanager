///////////////////////////////////////////////////////////////	
//	
// FileName  :  krasconn.cpp
// Creator   :  yangzhenhui@kingsoft.com
// Date    	 :  2009-09-21  15:07
// Comment   :  
//
///////////////////////////////////////////////////////////////	
#include "pch.h"
#include "krasconn.h"
#include "kndisfirewall.h"


BOOLEAN KRasConnection::Init( PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath )
{
	m_rasConnList._Init();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KRasConnection::Uninit( PDRIVER_OBJECT DriverObject )
{
	KIRQL pOldIrq;
	PRAS_CONNECTION pRasConnection = NULL;
	KDList <PVOID, PRAS_CONNECTION>::iterator item;

	m_rasConnList.lock( &pOldIrq );
	for( item = m_rasConnList.begin(); item != m_rasConnList.end(); item = m_rasConnList.next(item) )
	{
		pRasConnection = m_rasConnList.get_data( item );
		if( pRasConnection != NULL )
		{
			non_paged_pool::__DeallocateSystemMemory( pRasConnection );
			pRasConnection = NULL;
		}
	}
	m_rasConnList.clear();
	m_rasConnList.unlock( pOldIrq );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS KRasConnection::EnumRasConnection( PVOID pBuffer, ULONG uBufferLength, ULONG *puResultLen )
{
	NTSTATUS status = STATUS_SUCCESS;
	int i = 0;
	int	connCount = uBufferLength / sizeof(RAS_CONNECTION);
	KIRQL pOldIrq;
	PRAS_CONNECTION pRasConnection = NULL;
	PRAS_CONNECTION pOutRasConnection = (PRAS_CONNECTION)pBuffer;
	KDList <PVOID, PRAS_CONNECTION>::iterator item;

	if( pBuffer == NULL || uBufferLength == 0 || puResultLen == NULL != 0 )
		return STATUS_INVALID_PARAMETER;
	
	if( m_rasConnList.size() > connCount )
		return STATUS_INFO_LENGTH_MISMATCH;

	*puResultLen = 0;
	m_rasConnList.lock( &pOldIrq );
	for( item = m_rasConnList.begin(); item != m_rasConnList.end(); item = m_rasConnList.next(item) )
	{
		if( i >= connCount )
		{
			status = STATUS_INFO_LENGTH_MISMATCH;
			break; 
		}

		pRasConnection = m_rasConnList.get_data( item );
		if( pRasConnection != NULL )
		{
			RtlCopyMemory( &pOutRasConnection[i], pRasConnection, sizeof(RAS_CONNECTION) );
			pRasConnection->uInTraffic.QuadPart = 0;
			pRasConnection->uOutTraffic.QuadPart = 0;
			pRasConnection->uTimeMiss.QuadPart = 0;
			KeQuerySystemTime( &pRasConnection->uTimeLastGet );
		}

		i++;
	}
	m_rasConnList.unlock( pOldIrq );
	
	if( status == STATUS_SUCCESS )
		*puResultLen = i * sizeof(RAS_CONNECTION);

	return status;
}

//////////////////////////////////////////////////////////////////////////

VOID KRasConnection::LineUp( PNDIS_WAN_LINE_UP pNdisWanLineUp )
{
	KIRQL pOldIrq;
	PRAS_CONNECTION pRasConnection = NULL;
	KDList <PVOID, PRAS_CONNECTION>::iterator item;

	do 
	{	
		m_rasConnList.lock( &pOldIrq );
		
		for( item = m_rasConnList.begin(); item != m_rasConnList.end(); item = m_rasConnList.next(item) )
		{
			pRasConnection = m_rasConnList.get_data( item );
			if( RtlCompareMemory( (char*)&pRasConnection->id,  pNdisWanLineUp->DeviceName.Buffer, sizeof(GUID) ) == sizeof(GUID) )
			{
				RtlCopyMemory( (char*)pRasConnection->LocalAddress, pNdisWanLineUp->LocalAddress, 6 );
				RtlCopyMemory( (char*)pRasConnection->RemoteAddress, pNdisWanLineUp->RemoteAddress, 6 );
				pRasConnection->uStatus = (!pRasConnection->uStatus);	//断开连接时，也会发送 LINE_UP 状态
				break;
			}
			pRasConnection = NULL;
		}
		
		m_rasConnList.unlock( pOldIrq );

		if( pRasConnection == NULL )
		{
			pRasConnection = (PRAS_CONNECTION)non_paged_pool::__AllocateSystemMemory( sizeof(RAS_CONNECTION) );
			if( pRasConnection != NULL )
			{
				RtlZeroMemory( (char*)pRasConnection, sizeof(RAS_CONNECTION) );
				pRasConnection->nEventType = enumRasEvent;
				RtlCopyMemory( (char*)&pRasConnection->id, pNdisWanLineUp->DeviceName.Buffer, sizeof(GUID) );
				RtlCopyMemory( (char*)pRasConnection->LocalAddress, pNdisWanLineUp->LocalAddress, 6 );
				RtlCopyMemory( (char*)pRasConnection->RemoteAddress, pNdisWanLineUp->RemoteAddress, 6 );
				pRasConnection->uStatus = TRUE;
				m_rasConnList.push_back( pRasConnection );
			}
		}
		
		if( pRasConnection != NULL && pRasConnection->uStatus )
		{
			KeQuerySystemTime( &pRasConnection->uTimeLineUp );
			pRasConnection->uTimeLastGet.QuadPart = pRasConnection->uTimeLineUp.QuadPart;
			g_ndisFirewall->NotifyRasEvent( pRasConnection );
		}

	} while (FALSE);
}

//////////////////////////////////////////////////////////////////////////

VOID KRasConnection::LineDown( PNDIS_WAN_LINE_DOWN pNdisWanLineDown )
{
	KIRQL pOldIrq;
	PRAS_CONNECTION pRasConnection = NULL;
	KDList <PVOID, PRAS_CONNECTION>::iterator item;
	
	m_rasConnList.lock( &pOldIrq );

	for( item = m_rasConnList.begin(); item != m_rasConnList.end(); item = m_rasConnList.next(item) )
	{
		pRasConnection = m_rasConnList.get_data( item );
		if( RtlCompareMemory( pRasConnection->RemoteAddress , pNdisWanLineDown->RemoteAddress, 6 ) == 6 )
		{
			pRasConnection->uStatus = FALSE;
		}
	}

	m_rasConnList.unlock( pOldIrq );

	if( pRasConnection != NULL )
	{
		KeQuerySystemTime( &pRasConnection->uTimeLineDown );
		pRasConnection->uTimeMiss.QuadPart += 
				(pRasConnection->uTimeLineDown.QuadPart - pRasConnection->uTimeLastGet.QuadPart);
		g_ndisFirewall->NotifyRasEvent( pRasConnection );
	}
}

//////////////////////////////////////////////////////////////////////////

VOID KRasConnection::AddInTraffic( UCHAR* uRemoteAddress, ULONG uBytes )
{
	PRAS_CONNECTION pRasConnection = NULL;
	KDList <PVOID, PRAS_CONNECTION>::iterator item;
	for( item = m_rasConnList.begin(); item != m_rasConnList.end(); item = m_rasConnList.next(item) )
	{
		pRasConnection = m_rasConnList.get_data( item );
		if( RtlCompareMemory( pRasConnection->RemoteAddress , uRemoteAddress, 6 ) == 6 )
		{
			ExInterlockedAddLargeStatistic( &pRasConnection->uInTraffic, uBytes );
		}
	}
}

//////////////////////////////////////////////////////////////////////////

VOID KRasConnection::AddOutTraffic( UCHAR* uRemoteAddress, ULONG uBytes )
{
	PRAS_CONNECTION pRasConnection = NULL;
	KDList <PVOID, PRAS_CONNECTION>::iterator item;
	for( item = m_rasConnList.begin(); item != m_rasConnList.end(); item = m_rasConnList.next(item) )
	{
		pRasConnection = m_rasConnList.get_data( item );
		if( RtlCompareMemory( pRasConnection->RemoteAddress , uRemoteAddress, 6 ) == 6 )
		{
			ExInterlockedAddLargeStatistic( &pRasConnection->uOutTraffic, uBytes );
		}
	}
}

//////////////////////////////////////////////////////////////////////////

VOID KRasConnection::PassThru( PKANTIARP_ADAPTER adapter, PVOID pBuffer, ULONG uBufferLength, PACKET_DIR enumDir )
{
	PEHHDR pEhHdr = (PEHHDR)pBuffer;

	if( adapter->Medium != NdisMediumWan || uBufferLength < sizeof(EHHDR) )
		return;
		
	if( enumDir == enumPD_Send )
		AddOutTraffic( pEhHdr->eh_dst, uBufferLength );
	else
		AddInTraffic( pEhHdr->eh_src, uBufferLength );
}