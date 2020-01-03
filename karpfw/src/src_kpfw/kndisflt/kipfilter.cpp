#include "pch.h"
#include "kipfilter.h"
#include "kndisfirewall.h"
#include "kipfilter.tmh"

//////////////////////////////////////////////////////////////////////////


BOOLEAN KIpFilter::Init( PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath )
{
	m_ipSendList._Init();
	m_ipRecvList._Init();
	m_udpSendList._Init();
	m_udpRecvList._Init();
	m_tcpSendList._Init();
	m_tcpRecvList._Init();
	m_icmpSendList._Init();
	m_icmpRecvList._Init();
	m_igmpSendList._Init();
	m_igmpRecvList._Init();
	m_guidPass = GUID_NULL;
	m_portFunc.IsOpenPort = NULL;
	m_portFunc.IsTrafficCtrl = NULL;
	//m_portFunc.pRefCount = NULL;
	StartGetFuncThread();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KIpFilter::Uninit( PDRIVER_OBJECT DriverObject )
{
	ClearIpRule();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KIpFilter::ReadPacketData( PIPHEADER pIpHdr, ULONG size, PACKET_DIR enumDir, PPACKET_DATA pPktData )
{
	BOOL bReturn = FALSE;
	ULONG nIpHdrLen = 0;
	PUDPHEADER pUdpHdr = NULL;
	PTCPHEADER pTcpHdr = NULL;
	PICMPHEADER pIcmpHdr = NULL;
	PIGMPHEADER pIgmpHdr = NULL;

	nIpHdrLen = pIpHdr->byHeaderLen * 4;
	if( nIpHdrLen != sizeof(IPHEADER) )
		return bReturn;
	
	pPktData->PacketDirection = enumDir;
	pPktData->LocalAddress = (enumDir == enumPD_Send) ? ntohl(pIpHdr->uSourceAddress) : ntohl(pIpHdr->uDestinationAddress);
	pPktData->RemoteAddress = (enumDir == enumPD_Send) ? ntohl(pIpHdr->uDestinationAddress) : ntohl(pIpHdr->uSourceAddress);

	switch( pIpHdr->byProtocolType )
	{
	case IPHEADER_UDP:
		{
			if ( size < ( sizeof(IPHEADER) + sizeof(UDPHEADER) ) )
				break;

			pUdpHdr = (PUDPHEADER)((char*)pIpHdr + nIpHdrLen);
			pPktData->ProtoType = enumPT_UDP;
			pPktData->LocalPort = (enumDir == enumPD_Send) ? ntohs(pUdpHdr->uSourcePort) : ntohs(pUdpHdr->uDestinationPort);
			pPktData->RemotePort = (enumDir == enumPD_Send) ? ntohs(pUdpHdr->uDestinationPort) : ntohs(pUdpHdr->uSourcePort);
			bReturn = TRUE;
		}
		break;
	case IPHEADER_TCP:
		{
			if ( size < ( sizeof(IPHEADER) + sizeof(TCPHEADER) ) )
				break;

			pTcpHdr = (PTCPHEADER)((char*)pIpHdr + nIpHdrLen);
			if ( (pTcpHdr->HeaderLen_And_Flags.Details.byHeaderLen * 4 + nIpHdrLen) > size )
				break;

			pPktData->ProtoType = enumPT_TCP;
			pPktData->TcpFlag = pTcpHdr->HeaderLen_And_Flags.byByte2 & 0x3F;
			pPktData->LocalPort = (enumDir == enumPD_Send) ? ntohs(pTcpHdr->uSourcePort) : ntohs(pTcpHdr->uDestinationPort);
			pPktData->RemotePort = (enumDir == enumPD_Send) ? ntohs(pTcpHdr->uDestinationPort) : ntohs(pTcpHdr->uSourcePort);
			bReturn = TRUE;
		}
		break;
	case IPHEADER_ICMP:
		{
			if ( size < ( sizeof(IPHEADER) + sizeof(ICMPHEADER) ) )
				break;

			pPktData->ProtoType = enumPT_ICMP;
			pIcmpHdr = (PICMPHEADER)((char*)pIpHdr + nIpHdrLen);
			pPktData->IcmpType = pIcmpHdr->byType;
			pPktData->IcmpCode = pIcmpHdr->byCode;

			if ( size >= ( sizeof(IPHEADER) + sizeof(ICMPHEADER) + sizeof(GUID) ) )
			{
				if( m_guidPass != GUID_NULL && m_guidPass == *((GUID*)( pIcmpHdr + 1 )) )
				{
					KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_IP, "KIpFilter::ReadPacketData Matched Ping");
					break;
				}
			}

			bReturn = TRUE;
		}
		break;
	case IPHEADER_IGMP:
		{
			if ( size < ( sizeof(IPHEADER) + sizeof(IGMPHEADER) ) )
				break;

			pPktData->ProtoType = enumPT_IGMP;
			pIgmpHdr = (PIGMPHEADER)((char*)pIpHdr + nIpHdrLen);
			pPktData->IgmpType = pIgmpHdr->byType;
			pPktData->IgmpAddr = ntohl(pIgmpHdr->uAddress);
			bReturn = TRUE;
		}
		break;
	default:
		{
			pPktData->ProtoType = enumPT_IP;
			bReturn = TRUE;
		}
	}
	return bReturn;
}

//////////////////////////////////////////////////////////////////////////

IPRESULT KIpFilter::Filter( PKANTIARP_ADAPTER adapter, PVOID pBuffer, ULONG uBufferLength, PACKET_DIR enumDir )
{
	IPRESULT bResult = enumIP_Unknow;
	BOOLEAN bMatchResult = FALSE;
	GUID MatchID;
	int nCount = 0;
	KIRQL oldIrq;
	KIP_FILTER_INFO *ipRule = NULL;
	KDList<GUID,KIP_FILTER_INFO*>::iterator ruleItem;
	KDList<GUID,KIP_FILTER_INFO*> *ipRules = NULL;
	BOOLEAN bNeedLog = FALSE;
	PACKET_DATA packetData;
	LOG_IP_RULE ipLog;
	IPHEADER *pIPHeader = NULL;
	ULONG uIPHeaderOffset = 0;
	PFNMatchRule MatchRuleFunc = NULL;

	do 
	{
		if( !g_ndisFirewall->IsModuleEnable( ENABLE_IP_FILTER ) )
			break;
		
		uIPHeaderOffset = GetIpHeader( (PEHHDR)pBuffer, uBufferLength, &pIPHeader );
		if( uIPHeaderOffset == 0 )
			break;

		if( !ReadPacketData( pIPHeader, uBufferLength - uIPHeaderOffset, enumDir, &packetData ) )
			break;
		
		RtlZeroMemory( (char*)&ipLog, sizeof(LOG_IP_RULE) );

		if( g_ndisFirewall->IsModuleEnable( ENABLE_ANTI_SCAN ) )
		{
			if( !MatchPortOpen( &packetData, &ipLog ) )
			{
				MatchID = AntiScanGuid;
				bResult = enumIP_Block;
				bMatchResult = TRUE;
				bNeedLog = TRUE;
				break;
			}
		}

		if( g_ndisFirewall->IsModuleEnable( ENABLE_TRAFFIC_CONTRL ) )
		{
			if( !TrafficCtrl( &packetData, uBufferLength ) )
			{
				bResult = enumIP_Block;
				break;
			}
		}

		ipRules = GetRuleListFromType( packetData.ProtoType, enumDir, &MatchRuleFunc );
		if ( ipRules == NULL )
			break;
		
		ipRules->lock( &oldIrq );
		for( ruleItem = ipRules->begin(); ruleItem != ipRules->end(); ruleItem = ipRules->next(ruleItem) )
		{
			ipRule = ipRules->get_data( ruleItem );
			if( ipRule == NULL )
				continue;
			
			if( ipRule->filter.ProtocolType == enumPT_IP )
			{
				bMatchResult = MatchIpRule( &ipRule->filter, &packetData, &ipLog );
			}
			else
			{
				bMatchResult = MatchRuleFunc( &ipRule->filter, &packetData, &ipLog );
			}
			
			if( bMatchResult )
			{
				MatchID = ipRule->id;
				bResult = ( ( ipRule->filter.Operation & enumMO_Deny ) ? enumIP_Block : enumIP_Pass );		//0 放行 1 拦截
				bNeedLog = ( ipRule->filter.Operation & ~enumMO_Deny );
				break;
			}
		}
		ipRules->unlock( oldIrq );
				
	} while (FALSE);

	if( bMatchResult && bNeedLog )
	{
		ipLog.id = MatchID;
		g_ndisFirewall->SendIpLog( &ipLog );
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

VOID KIpFilter::SetIpRuleEnable( KDList< GUID, KIP_FILTER_INFO* >* ruleList, GUID ruleId, BOOLEAN bEnable, NTSTATUS *status )
{
	KIP_FILTER_INFO* ipRule = NULL;
	if( ruleList->find( ruleId, &ipRule ) )
	{
		//ipRule->filter.bEnable = bEnable;
		*status = STATUS_SUCCESS;
	}
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS KIpFilter::EnableIpRule( PVOID pGuidBuffer, ULONG uBufferLength, BOOLEAN bEnable )
{
	NTSTATUS bResult = STATUS_INVALID_PARAMETER;

	if( uBufferLength < sizeof(GUID) || pGuidBuffer == NULL )
		return bResult;

	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_IP, "KIpFilter::EnableIpRule++");
	
	SetIpRuleEnable( &m_udpSendList, *((GUID*)pGuidBuffer), bEnable, &bResult );
	SetIpRuleEnable( &m_udpRecvList, *((GUID*)pGuidBuffer), bEnable, &bResult );
	SetIpRuleEnable( &m_tcpSendList, *((GUID*)pGuidBuffer), bEnable, &bResult );
	SetIpRuleEnable( &m_tcpRecvList, *((GUID*)pGuidBuffer), bEnable, &bResult );
	SetIpRuleEnable( &m_icmpSendList, *((GUID*)pGuidBuffer), bEnable, &bResult );
	SetIpRuleEnable( &m_icmpRecvList, *((GUID*)pGuidBuffer), bEnable, &bResult );
	SetIpRuleEnable( &m_igmpSendList, *((GUID*)pGuidBuffer), bEnable, &bResult );
	SetIpRuleEnable( &m_igmpRecvList, *((GUID*)pGuidBuffer), bEnable, &bResult );
	SetIpRuleEnable( &m_ipSendList, *((GUID*)pGuidBuffer), bEnable, &bResult );
	SetIpRuleEnable( &m_ipSendList, *((GUID*)pGuidBuffer), bEnable, &bResult );
	
	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_IP, "KIpFilter::EnableIpRule--");

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

VOID KIpFilter::AddIpRule( KDList< GUID, KIP_FILTER_INFO* >* ruleList, KIP_FILTER_INFO* irRule )
{
	if ( ruleList != NULL )
	{
		KIP_FILTER_INFO* newIpRule = (KIP_FILTER_INFO *)non_paged_pool::__AllocateSystemMemory( sizeof(KIP_FILTER_INFO) );
		if( newIpRule != NULL )
		{
			RtlCopyMemory( (char*)newIpRule, (char*)irRule, sizeof(KIP_FILTER_INFO) );
			ruleList->insert( newIpRule->id, newIpRule );
		}
	}
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS KIpFilter::AddIpRule( PVOID pRuleInfoBuffer, ULONG uBufferLength )
{
	NTSTATUS bResult = STATUS_INVALID_PARAMETER;
	KIP_FILTER_INFO* ipRule = NULL;
	KDList< GUID, KIP_FILTER_INFO* > *ruleList = NULL;
	PFNMatchRule MatchRuleFunc = NULL;
	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_IP, "KIpFilter::AddIpRule++");

	do 
	{
		if( uBufferLength != sizeof(KIP_FILTER_INFO) || pRuleInfoBuffer == NULL )
			break;

		ipRule = (KIP_FILTER_INFO *)pRuleInfoBuffer;

		if( ipRule->filter.Direction == enumPD_Send || ipRule->filter.Direction == enumPD_Both )
		{
			ruleList = GetRuleListFromType( ipRule->filter.ProtocolType, enumPD_Send, &MatchRuleFunc );
			AddIpRule( ruleList, ipRule );
			if( ipRule->filter.ProtocolType == enumPT_IP )
			{
				AddIpRule( &m_icmpSendList, ipRule );
				AddIpRule( &m_igmpSendList, ipRule );
				AddIpRule( &m_tcpSendList, ipRule );
				AddIpRule( &m_udpSendList, ipRule );
			}
		}

		if( ipRule->filter.Direction == enumPD_Recieve || ipRule->filter.Direction == enumPD_Both )
		{
			ruleList = GetRuleListFromType( ipRule->filter.ProtocolType, enumPD_Recieve, &MatchRuleFunc );
			AddIpRule( ruleList, ipRule );
			if( ipRule->filter.ProtocolType == enumPT_IP )
			{
				AddIpRule( &m_icmpRecvList, ipRule );
				AddIpRule( &m_igmpRecvList, ipRule );
				AddIpRule( &m_tcpRecvList, ipRule );
				AddIpRule( &m_udpRecvList, ipRule );
			}
		}

		bResult = STATUS_SUCCESS;
	} while (FALSE);

	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_IP, "KIpFilter::AddIpRule--");
	return bResult;
}

//////////////////////////////////////////////////////////////////////////

VOID KIpFilter::RemoveIpRule( KDList< GUID, KIP_FILTER_INFO* >* ruleList, GUID ruleId, NTSTATUS *status )
{
	KIP_FILTER_INFO *ipRule = NULL;
	if( m_ipSendList.erase( ruleId, &ipRule ) )
	{
		non_paged_pool::__DeallocateSystemMemory( (PVOID)ipRule );
		*status = STATUS_SUCCESS;
	}
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS KIpFilter::RemoveIpRule( PVOID pGuidBuffer, ULONG uBufferLength )
{
	NTSTATUS bResult = STATUS_INVALID_PARAMETER;
	if( uBufferLength < sizeof(GUID) || pGuidBuffer == NULL )
		return bResult;

	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_IP, "KIpFilter::AddIpRule++");
	
	RemoveIpRule( &m_udpSendList, *((GUID*)pGuidBuffer), &bResult );
	RemoveIpRule( &m_udpRecvList, *((GUID*)pGuidBuffer), &bResult );
	RemoveIpRule( &m_tcpSendList, *((GUID*)pGuidBuffer), &bResult );
	RemoveIpRule( &m_tcpRecvList, *((GUID*)pGuidBuffer), &bResult );
	RemoveIpRule( &m_icmpSendList, *((GUID*)pGuidBuffer), &bResult );
	RemoveIpRule( &m_icmpRecvList, *((GUID*)pGuidBuffer), &bResult );
	RemoveIpRule( &m_igmpSendList, *((GUID*)pGuidBuffer), &bResult );
	RemoveIpRule( &m_igmpRecvList, *((GUID*)pGuidBuffer), &bResult );
	RemoveIpRule( &m_ipSendList, *((GUID*)pGuidBuffer), &bResult );
	RemoveIpRule( &m_ipSendList, *((GUID*)pGuidBuffer), &bResult );

	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_IP, "KIpFilter::AddIpRule--");
	return bResult;
}

//////////////////////////////////////////////////////////////////////////

VOID KIpFilter::ClearList( KDList< GUID, KIP_FILTER_INFO* > *pList )
{
	KIRQL oldirq;
	KDList< GUID, KIP_FILTER_INFO* >::iterator item;
	KIP_FILTER_INFO* ipRule = NULL;
	pList->lock( &oldirq );
	for ( item = pList->begin(); item != pList->end(); item = pList->next(item) )
	{
		ipRule = pList->get_data( item );
		non_paged_pool::__DeallocateSystemMemory( (PVOID)ipRule );
		ipRule = NULL;
	}
	pList->clear();
	pList->unlock( oldirq );
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS KIpFilter::ClearIpRule()
{
	ClearList( &m_udpSendList );
	ClearList( &m_udpRecvList );

	ClearList( &m_ipSendList );
	ClearList( &m_ipRecvList );

	ClearList( &m_tcpSendList );
	ClearList( &m_tcpRecvList );

	ClearList( &m_icmpSendList );
	ClearList( &m_icmpRecvList );

	ClearList( &m_igmpSendList );
	ClearList( &m_igmpRecvList );

	return STATUS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KIpFilter::MatchIpAddr( PFILTER_IP_ADDR ruleIp, ULONG ip )
{
	BOOLEAN bResult = FALSE;

	switch( ruleIp->eMode )
	{
	case enumAM_Any:
		bResult = TRUE;
		break;
	case enumAM_Single:
		bResult = ( ruleIp->ipAddr == ip );
		break;
	case enumAM_Range:
		bResult = ( ip >= ruleIp->ipRange.ipStart && ip <= ruleIp->ipRange.ipEnd );
		break;
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KIpFilter::MatchPort( PFILTER_PORT rulePort, USHORT port )
{
	BOOLEAN bResult = FALSE;
	switch(rulePort->eMode)
	{
	case enumPM_Any:
		bResult = TRUE;
		break;
	case enumPM_Single:
		bResult = ( rulePort->port == port );
		break;
	case enumPM_Range:
		bResult = ( port >= rulePort->portRange.nStart && port <= rulePort->portRange.nEnd );
		break;
	}
	return bResult;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KIpFilter::MatchTcpFlag( UCHAR ruleFlag, unsigned char flag )
{
	if (!(ruleFlag & 0x80))
		return TRUE;
	
	if (flag == (ruleFlag & 0x3F))
		return TRUE;

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KIpFilter::MatchIpRule( PFILTER_INFO ipRule, PPACKET_DATA pPktData, LOG_IP_RULE* pLog )
{
	if ( MatchIpAddr( &ipRule->LocalAddr, pPktData->LocalAddress ) 
			&& MatchIpAddr( &ipRule->RemoteAddr, pPktData->RemoteAddress ) )
	{
		pLog->byProtocolType = enumPT_IP;
		pLog->nOperation = ipRule->Operation;
		pLog->byDirection = pPktData->PacketDirection;
		pLog->LocalAddr = pPktData->LocalAddress;
		pLog->RemoteAddr = pPktData->RemoteAddress;
		return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KIpFilter::MatchTcpRule( PFILTER_INFO tcpRule, PPACKET_DATA pPktData, LOG_IP_RULE* pLog )
{
	if ( MatchIpAddr( &tcpRule->LocalAddr, pPktData->LocalAddress ) 
			&& MatchIpAddr( &tcpRule->RemoteAddr, pPktData->RemoteAddress ) 
			&& MatchPort( &tcpRule->FilterInfo_ExtraData.TCP_DATA.LocalPort, pPktData->LocalPort )
			&& MatchPort( &tcpRule->FilterInfo_ExtraData.TCP_DATA.RemotePort, pPktData->RemotePort )
			&& MatchTcpFlag( tcpRule->FilterInfo_ExtraData.TCP_DATA.byTcpFlags, pPktData->TcpFlag ) )
	{
		pLog->byProtocolType = enumPT_TCP;
		pLog->nOperation = tcpRule->Operation;
		pLog->byDirection = pPktData->PacketDirection;
		pLog->LocalAddr = pPktData->LocalAddress;
		pLog->RemoteAddr = pPktData->RemoteAddress;
		pLog->ExtraInfo.TCP_DATA.LocalPort = pPktData->LocalPort;
		pLog->ExtraInfo.TCP_DATA.RemotePort = pPktData->RemotePort;
		pLog->ExtraInfo.TCP_DATA.byTcpFlags = pPktData->TcpFlag;
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KIpFilter::MatchUdpRule( PFILTER_INFO udpRule, PPACKET_DATA pPktData, LOG_IP_RULE* pLog )
{
	if ( MatchIpAddr( &udpRule->LocalAddr, pPktData->LocalAddress ) 
			&& MatchIpAddr( &udpRule->RemoteAddr, pPktData->RemoteAddress ) 
			&& MatchPort( &udpRule->FilterInfo_ExtraData.UDP_DATA.LocalPort, pPktData->LocalPort ) 
			&& MatchPort( &udpRule->FilterInfo_ExtraData.UDP_DATA.RemotePort, pPktData->RemotePort ) )
	{
		pLog->byProtocolType = enumPT_UDP;
		pLog->nOperation = udpRule->Operation;
		pLog->byDirection = pPktData->PacketDirection;
		pLog->LocalAddr = pPktData->LocalAddress;
		pLog->RemoteAddr = pPktData->RemoteAddress;
		pLog->ExtraInfo.UDP_DATA.LocalPort = pPktData->LocalPort;
		pLog->ExtraInfo.UDP_DATA.RemotePort = pPktData->RemotePort;
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KIpFilter::MatchIcmpRule( PFILTER_INFO icmpRule, PPACKET_DATA pPktData, LOG_IP_RULE* pLog )
{
	if ( MatchIpAddr( &icmpRule->LocalAddr, pPktData->LocalAddress ) 
			&& MatchIpAddr( &icmpRule->RemoteAddr, pPktData->RemoteAddress ) )
	{
		if ( ( icmpRule->FilterInfo_ExtraData.ICMP_DATA.byCode == 0xff || icmpRule->FilterInfo_ExtraData.ICMP_DATA.byCode == pPktData->IcmpCode ) 
				&& ( icmpRule->FilterInfo_ExtraData.ICMP_DATA.byType == 0xff || icmpRule->FilterInfo_ExtraData.ICMP_DATA.byType == pPktData->IcmpType ) )
		{
			pLog->byProtocolType = enumPT_ICMP;
			pLog->nOperation = icmpRule->Operation;
			pLog->byDirection = pPktData->PacketDirection;
			pLog->LocalAddr = pPktData->LocalAddress;
			pLog->RemoteAddr = pPktData->RemoteAddress;
			pLog->ExtraInfo.ICMP_DATA.byCode = pPktData->IcmpCode;
			pLog->ExtraInfo.ICMP_DATA.byType = pPktData->IcmpType;
			return TRUE;
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KIpFilter::MatchIgmpRule( PFILTER_INFO igmpRule, PPACKET_DATA pPktData, LOG_IP_RULE* pLog )
{
	if ( MatchIpAddr( &igmpRule->LocalAddr, pPktData->LocalAddress ) 
		&& MatchIpAddr( &igmpRule->RemoteAddr, pPktData->RemoteAddress ) )
	{
		pLog->byProtocolType = enumPT_IGMP;
		pLog->nOperation = igmpRule->Operation;
		pLog->byDirection = pPktData->PacketDirection;
		pLog->LocalAddr = pPktData->LocalAddress;
		pLog->RemoteAddr = pPktData->RemoteAddress;
		return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

KDList< GUID, KIP_FILTER_INFO* >* KIpFilter::GetRuleListFromType( PROTO_TYPE protoType, PACKET_DIR enumDir, PFNMatchRule *MatchRuleFunc )
{
	KDList<GUID,KIP_FILTER_INFO*> *ipRules = NULL;
	
	switch( protoType )
	{
	case enumPT_TCP:
		if( enumDir == enumPD_Send )
			ipRules = &m_tcpSendList;
		else
			ipRules = &m_tcpRecvList;

		*MatchRuleFunc = MatchTcpRule;
		break;
	case enumPT_UDP:
		if( enumDir == enumPD_Send )
			ipRules = &m_udpSendList;
		else
			ipRules = &m_udpRecvList;

		*MatchRuleFunc = MatchUdpRule;
		break;
	case enumPT_ICMP:
		if( enumDir == enumPD_Send )
			ipRules = &m_icmpSendList;
		else
			ipRules = &m_icmpRecvList;

		*MatchRuleFunc = MatchIcmpRule;
		break;
	case enumPT_IP:
		if( enumDir == enumPD_Send )
			ipRules = &m_ipSendList;
		else
			ipRules = &m_ipRecvList;

		*MatchRuleFunc = MatchIpRule;
		break;
	case enumPT_IGMP:
		if( enumDir == enumPD_Send )
			ipRules = &m_igmpSendList;
		else
			ipRules = &m_igmpRecvList;
		*MatchRuleFunc = MatchIgmpRule;
		break;
	}

	return ipRules;
}

//////////////////////////////////////////////////////////////////////////

ULONG KIpFilter::GetIpHeader( PEHHDR pEhHdr, ULONG uBufferLen, PIPHEADER *ppIpHdr )
{
	ULONG uIPHeaderOffset = 0;
	PHdrPpp pHdrPpp = NULL;
	do 
	{
		if( pEhHdr == NULL || ppIpHdr == NULL )
			break;

		if( uBufferLen < sizeof(EHHDR) )
			break;

		switch( pEhHdr->eh_type )
		{
		case ETHER_IP:
			{
				if( uBufferLen < sizeof(EHHDR) + sizeof(IPHEADER) )
					break;
				uIPHeaderOffset = sizeof(EHHDR);
			}
			break;
		case ETHER_PPPOE_SESSION:
			{
				if( uBufferLen < sizeof(EHHDR) + sizeof(HdrPppoe) + sizeof(HdrPpp) + sizeof(IPHEADER) )
					break;

				pHdrPpp = (PHdrPpp)((char*)pEhHdr + sizeof(EHHDR) + sizeof(HdrPppoe) );
				if( pHdrPpp->proto != enumPppIp )
					break;
				uIPHeaderOffset = sizeof(EHHDR) + sizeof(HdrPppoe) + sizeof(HdrPpp);
			}
			break;
		}

		if( uIPHeaderOffset != 0 )
		{
			*ppIpHdr = (PIPHEADER)( (char*)pEhHdr + uIPHeaderOffset );
		}

	} while (FALSE);

	return uIPHeaderOffset;
}

//////////////////////////////////////////////////////////////////////////

VOID KIpFilter::SetPassGuid( GUID guidPass )
{
	m_guidPass = guidPass;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KIpFilter::MatchPortOpen( PPACKET_DATA pPacketData, LOG_IP_RULE* pLog )
{
	BOOLEAN bResult = TRUE;

	if( (pPacketData->ProtoType != enumPT_TCP)
		&& (pPacketData->ProtoType != enumPT_UDP) )
		return bResult;

	if( m_portFunc.IsOpenPort != NULL /*&& m_portFunc.pRefCount*/ )
	{
		//if( *m_portFunc.pRefCount > 0 )
		{
		//	InterlockedIncrement( m_portFunc.pRefCount );
			bResult = m_portFunc.IsOpenPort( pPacketData->ProtoType, pPacketData->LocalPort );
		//	InterlockedDecrement( m_portFunc.pRefCount );
		}
	}

	if( !bResult )
	{
		pLog->byDirection = pPacketData->PacketDirection;
		pLog->byProtocolType = pPacketData->ProtoType;
		pLog->ExtraInfo.UDP_DATA.LocalPort = pPacketData->LocalPort;
		pLog->ExtraInfo.UDP_DATA.RemotePort = pPacketData->RemotePort;
		pLog->LocalAddr = pPacketData->LocalAddress;
		pLog->RemoteAddr = pPacketData->RemoteAddress;
		pLog->nAttackType = pa_arp_anti_scan;
		pLog->nOperation = enumMO_Deny;
	}
	return bResult;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KIpFilter::TrafficCtrl( PPACKET_DATA pPacketData, ULONG uBufferLen )
{
	BOOLEAN bResult = TRUE;

	if( (pPacketData->ProtoType != enumPT_TCP)
		&& (pPacketData->ProtoType != enumPT_UDP) )
		return bResult;

	if( m_portFunc.IsTrafficCtrl != NULL /*&& m_portFunc.pRefCount */)
	{
		//if( *m_portFunc.pRefCount > 0 )
		{
			//InterlockedIncrement( m_portFunc.pRefCount );
			bResult = m_portFunc.IsTrafficCtrl( pPacketData->ProtoType,
				pPacketData->LocalPort,
				(char)pPacketData->PacketDirection,
				uBufferLen );	
			//InterlockedDecrement( m_portFunc.pRefCount );
		}
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KIpFilter::StartGetFuncThread()
{
	HANDLE hThread = NULL;
	OBJECT_ATTRIBUTES oa; 

	InitializeObjectAttributes( &oa, 0, OBJ_CASE_INSENSITIVE, 0, 0 );
	if( STATUS_SUCCESS == PsCreateSystemThread( &hThread, THREAD_ALL_ACCESS, 
		&oa, 0, 0, (PKSTART_ROUTINE)GetFunction, (PVOID)this ) )
	{
		return TRUE;
	}
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////

VOID KIpFilter::GetFunction( PVOID StartContext )
{
	KIpFilter *pThis = (KIpFilter *)StartContext;
	UNICODE_STRING strNdisName;
	PFILE_OBJECT hFile = NULL; 
	NTSTATUS Status = STATUS_INSUFFICIENT_RESOURCES; 
	PIRP pIRP = NULL; 
	IO_STATUS_BLOCK IO_Block = {0,0};
	PDEVICE_OBJECT pTdiDevice = NULL;
	int tryCount = 0;
	LARGE_INTEGER waitTime;
	waitTime.QuadPart = -1l * 1000 *(1000 / 100);
	waitTime.QuadPart *= 300;

	for( tryCount=0; tryCount<400; tryCount++ ) //两分钟
	{
		if( hFile == NULL )
		{
			RtlInitUnicodeString( &strNdisName, KTDIFLT_NT_NAME );
			Status = IoGetDeviceObjectPointer( &strNdisName, FILE_ALL_ACCESS, &hFile, &pTdiDevice );
			if ( !NT_SUCCESS(Status) ) 
			{
				KeDelayExecutionThread( KernelMode, FALSE, &waitTime );
				continue;
			}
		}

		pIRP = IoBuildDeviceIoControlRequest( 
			(ULONG)IOCTL_KTDIFLT_GET_PORT_FUNCTION, pTdiDevice, 
			(PVOID)&pThis->m_portFunc, sizeof(NDIS_PORT_FUNC),
			(PVOID)&pThis->m_portFunc, sizeof(NDIS_PORT_FUNC),
			TRUE, 
			NULL,
			&IO_Block
			);
		if( pIRP == NULL )
		{
			KeDelayExecutionThread( KernelMode, FALSE, &waitTime );
			continue;
		}

		Status = IoCallDriver( pTdiDevice, pIRP );
		if( !NT_SUCCESS(Status) )
		{
			KeDelayExecutionThread( KernelMode, FALSE, &waitTime );
			continue;
		}
		
		if( (PVOID)(pThis->m_portFunc.IsOpenPort) == NULL )
			continue;

		break;
	} 

	if( hFile != NULL )
	{
		ObDereferenceObject( hFile );
	}
}

//////////////////////////////////////////////////////////////////////////

//ULONG KIpFilter::GetTickCount_()
//{ 
//	LARGE_INTEGER tick_count; 
//	ULONG myinc = KeQueryTimeIncrement(); 
//	KeQueryTickCount(&tick_count); 
//	tick_count.QuadPart *= myinc; 
//	tick_count.QuadPart /=  10000; 
//	return tick_count.LowPart; 
//} 


//////////////////////////////////////////////////////////////////////////


//
////////////////////////////////////////////////////////////////////////////
//
//BOOLEAN KIpFilter::AddPortTrafficCtrl( USHORT uPort, ULONG uSendSpeed,  ULONG uRecvSpeed )
//{
//	PTRAFFIC_CTRL pCtrlItem = (PTRAFFIC_CTRL)non_paged_pool::__AllocateSystemMemory( sizeof(TRAFFIC_CTRL) );
//	if( pCtrlItem == NULL )
//		return FALSE;
//	RtlZeroMemory( (char*)pCtrlItem, sizeof(TRAFFIC_CTRL) );
//	pCtrlItem->uStartTime = GetTickCount_();
//	pCtrlItem->uRecvLimit = uRecvSpeed;
//	pCtrlItem->uSendLimit = uSendSpeed;
//	
//	m_traffciCtrlList.insert( (PVOID)uPort, pCtrlItem );
//	return TRUE;
//}

//////////////////////////////////////////////////////////////////////////

