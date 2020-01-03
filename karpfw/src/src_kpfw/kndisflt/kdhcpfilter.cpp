#include "pch.h"
#include "kdhcpfilter.h"
#include "kndisfirewall.h"

BOOLEAN KDhcpFilter::Init( PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath )
{
	//初始化设备
	BOOLEAN bResult = TRUE;

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KDhcpFilter::Uninit( PDRIVER_OBJECT DriverObject )
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

IPRESULT KDhcpFilter::Filter( PKANTIARP_ADAPTER adapter, PVOID pBuffer, ULONG uBufferLength, PACKET_DIR enumDir )
{
	IPRESULT nResult = enumIP_Unknow;
	if( enumDir == enumPD_Recieve )
	{
		nResult = FilterRecv( adapter, pBuffer, uBufferLength );
	}
	else
	{
		nResult = FilterSend( adapter, pBuffer, uBufferLength );
	}
	
	return nResult;
}

//////////////////////////////////////////////////////////////////////////

IPRESULT KDhcpFilter::FilterRecv( PKANTIARP_ADAPTER adapter, PVOID pBuffer, ULONG uBufferLength )
{
	IPRESULT nResult = enumIP_Unknow;
	DHCP_PACKET *pDhcpPacket = (DHCP_PACKET *)pBuffer;
	ULONG uLocalIP[MAX_ADAPTER_IP_NUM] = {0};

	do 
	{
		if( uBufferLength < sizeof( DHCP_PACKET ) )
			break;

		if( pDhcpPacket->eth_h.eh_type != ETHER_IP )
			break;

		if( pDhcpPacket->ip_h.byProtocolType != IPHEADER_UDP )
			break;

		if( pDhcpPacket->udp_h.uDestinationPort != htons(68) )
			break;

		if( pDhcpPacket->dhcp_h.magic != DHCP_MAGIC )
			break;

		if( pDhcpPacket->dhcp_h.flags != DHCP_FLAGS_UNICAST )
			break;

		if( pDhcpPacket->dhcp_h.op != DHCP_BOOTREPLY )
			break;
		
		if( pDhcpPacket->dhcp_h.yiaddr == 0 && pDhcpPacket->dhcp_h.giaddr == 0 )
			break;

		if( RtlCompareMemory( pDhcpPacket->dhcp_h.chaddr, adapter->LocalMac, 6 ) != 6 )
			break;
		
		KAntiarpDebugPrint(DBG_KFW, DBG_INFO, __FUNCTION__" DHCP LocalIP:%08X", pDhcpPacket->dhcp_h.yiaddr );
		KAntiarpDebugPrint(DBG_KFW, DBG_INFO, __FUNCTION__" DHCP GatewayIP:%08X", pDhcpPacket->dhcp_h.giaddr );
		KAntiarpDebugPrint(DBG_KFW, DBG_INFO, __FUNCTION__" DHCP LocalMac:%02X-%02X-%02X-%02X-%02X-%02X", 
			pDhcpPacket->dhcp_h.chaddr[0], 
			pDhcpPacket->dhcp_h.chaddr[1],
			pDhcpPacket->dhcp_h.chaddr[2],
			pDhcpPacket->dhcp_h.chaddr[3],
			pDhcpPacket->dhcp_h.chaddr[4],
			pDhcpPacket->dhcp_h.chaddr[5] );
		
		uLocalIP[0] = pDhcpPacket->dhcp_h.yiaddr;

		//g_ndisFirewall->SetAdapterAddr(
		//	adapter,
		//	uLocalIP,  
		//	pDhcpPacket->dhcp_h.giaddr );

		nResult = enumIP_Pass;

	} while (FALSE);

	return nResult;
}

//////////////////////////////////////////////////////////////////////////

IPRESULT KDhcpFilter::FilterSend( PKANTIARP_ADAPTER adapter, PVOID pBuffer, ULONG uBufferLength )
{
	IPRESULT nResult = enumIP_Unknow;
	//DHCP_PACKET *pDhcpPacket = (DHCP_PACKET *)pBuffer;

	do 
	{
		//if( uBufferLength < sizeof( DHCP_PACKET ) )
		//	break;

		//if( pDhcpPacket->eth_h.eh_type != ETHER_IP )
		//	break;

		//if( pDhcpPacket->ip_h.byProtocolType != IPHEADER_UDP )
		//	break;

		//if( pDhcpPacket->udp_h.uSourcePort != htons(68) )
		//	break;

		//if( pDhcpPacket->dhcp_h.magic != DHCP_MAGIC )
		//	break;

		//if( pDhcpPacket->dhcp_h.flags != DHCP_FLAGS_BCAST )
		//	break;

		//if( pDhcpPacket->dhcp_h.op != DHCP_BOOTREQUEST )
		//	break;

		//if( pDhcpPacket->dhcp_h.options[0] != DHCP_TAG_TYPE )
		//	break;

		//if( pDhcpPacket->dhcp_h.options[2] != DHCP_RELEASE )
		//	break;

		//KAntiarpDebugPrint(DBG_KFW, DBG_INFO, __FUNCTION__" DHCP LocalIP:%08X", pDhcpPacket->dhcp_h.ciaddr );
		//KAntiarpDebugPrint(DBG_KFW, DBG_INFO, __FUNCTION__" DHCP LocalMac:%02X-%02X-%02X-%02X-%02X-%02X", 
		//	pDhcpPacket->dhcp_h.chaddr[0], 
		//	pDhcpPacket->dhcp_h.chaddr[1],
		//	pDhcpPacket->dhcp_h.chaddr[2],
		//	pDhcpPacket->dhcp_h.chaddr[3],
		//	pDhcpPacket->dhcp_h.chaddr[4],
		//	pDhcpPacket->dhcp_h.chaddr[5] );

		//g_ndisFirewall->SetAdapterAddr( adapter, 0, 0 );

		//nResult = enumIP_Pass;

	} while (FALSE);

	return nResult;
}

//////////////////////////////////////////////////////////////////////////
