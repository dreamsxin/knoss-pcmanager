#include "pch.h"
#include "kadvipfilter.h"
#include "kipfilter.h"
#include "kndisfirewall.h"
#include "kadvipfilter.tmh"

#define ntohll(x) (((unsigned __int64)(ntohl((unsigned int)((x << 32) >> 32))) << 32) | (unsigned int)ntohl(((unsigned int)(x >> 32)))) 

BOOLEAN KAdvIpFilter::Init( PDRIVER_OBJECT /*DriverObject*/, PUNICODE_STRING /*RegistryPath*/ )
{
	m_udpSendList._Init();
	m_udpRecvList._Init();
	m_tcpSendList._Init();
	m_tcpRecvList._Init();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KAdvIpFilter::Uninit( PDRIVER_OBJECT /*DriverObject */)
{
	ClearIpRule();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

IPRESULT KAdvIpFilter::Filter( PKANTIARP_ADAPTER adapter, PVOID pBuffer, ULONG uBufferLength, PACKET_DIR enumDir )
{
	char *pData = NULL;
	ULONG uDataLen = 0;
	IPHEADER *pIpHdr = NULL;
	ULONG uIpHdrOffset = 0;
	IPRESULT bResult = enumIP_Unknow;
	KDList< GUID, KPackTemplate* > *ruleList = NULL;
	GUID MatchID;
	PVOID pProtoHdr = NULL;
	LOG_IP_RULE ipLog;
	
	if( !g_ndisFirewall->IsModuleEnable( ENABLE_ADV_IP_FILTER ) )
		return bResult;
		
	uIpHdrOffset = KIpFilter::GetIpHeader( (PEHHDR)pBuffer, uBufferLength, &pIpHdr );
	if( uIpHdrOffset == 0 )
		return bResult;
	
	switch( pIpHdr->byProtocolType )
	{
	case IPHEADER_TCP:
		ruleList = GetRuleListFromType( enumPT_TCP, enumDir );
		break;

	case IPHEADER_UDP:
		ruleList = GetRuleListFromType( enumPT_UDP, enumDir );
		break;
	}
	
	if( ruleList == NULL )
		return bResult;
	
	if( !GetPackData( pIpHdr, uBufferLength - uIpHdrOffset, &pProtoHdr, &pData, &uDataLen ) )
		return bResult;

	bResult = MatchIpRuleList( pProtoHdr, pData, uDataLen, ruleList, &MatchID );
	if( bResult == enumIP_Block )
	{
		RtlZeroMemory( (char*)&ipLog, sizeof(LOG_IP_RULE) );
		ipLog.id = MatchID;
		ipLog.nAttackType = pa_arp_adv_ip_filter;
		ipLog.byDirection = enumDir;
		ipLog.nOperation = enumMO_Deny;

		if( enumDir == enumPD_Send )
		{
			ipLog.LocalAddr = htonl(pIpHdr->uSourceAddress);
			ipLog.RemoteAddr = htonl(pIpHdr->uDestinationAddress);
			ipLog.ExtraInfo.UDP_DATA.LocalPort = htons(((UDPHEADER*)pProtoHdr)->uSourcePort);
			ipLog.ExtraInfo.UDP_DATA.RemotePort = htons(((UDPHEADER*)pProtoHdr)->uDestinationPort);
		}
		else
		{
			ipLog.LocalAddr = htonl(pIpHdr->uDestinationAddress);
			ipLog.RemoteAddr = htonl(pIpHdr->uSourceAddress);
			ipLog.ExtraInfo.UDP_DATA.LocalPort = htons(((UDPHEADER*)pProtoHdr)->uDestinationPort);
			ipLog.ExtraInfo.UDP_DATA.RemotePort = htons(((UDPHEADER*)pProtoHdr)->uSourcePort);
		}

		g_ndisFirewall->SendAdvIpLog( &ipLog );
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

IPRESULT KAdvIpFilter::MatchIpRuleList( 
	PVOID pProtoHdr,
	char* pPackBuffer, 
	ULONG uPackLength, 
	KDList<GUID,KPackTemplate*> *ipRules, 
	/*out*/GUID *ruleID )
{
	IPRESULT bResult = enumIP_Unknow;
	KIRQL oldIrq;
	KPackTemplate *ipRule = NULL;
	KDList<GUID,KPackTemplate>::iterator ruleItem;

	if( pPackBuffer == NULL || uPackLength == 0 || ipRules == NULL || ruleID == NULL )
		return bResult;
	
	ipRules->lock( &oldIrq );
	for( ruleItem = ipRules->begin(); ruleItem != ipRules->end(); ruleItem = ipRules->next(ruleItem) )
	{
		ipRule = ipRules->get_data( ruleItem );
		
		if( !MatchHdr( pProtoHdr, ipRule ) )
			continue;

		if( MatchIpRule( pPackBuffer, uPackLength, ipRule ) )
		{
			*ruleID = ipRule->guidID;
			bResult = enumIP_Block;
			//bResult = ((KPackTemplate*)ipRule->pTemplateBuffer)->nOperation & ;
			break;
		}
	}	
	ipRules->unlock( oldIrq );

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KAdvIpFilter::MatchIpRule( char* pData, ULONG uDataLen, KPackTemplate* packTemplate )
{
	KPackNode* pNodePack = NULL;
	BOOL bResult = FALSE;

	if( pData == NULL || packTemplate == NULL )
		return FALSE;

	if( uDataLen < (DWORD)packTemplate->uPackSize )
		return FALSE;

	pNodePack = packTemplate->packNode;

	for( int i=0; i<packTemplate->sNodeCount; i++ )
	{
		switch( pNodePack->cType )
		{
		case enumChar:
			bResult = *((char*)(pData + pNodePack->sOffset)) == pNodePack->Parameters[0].c;
			break;
		case enumShort:
			bResult = *((short*)(pData + pNodePack->sOffset)) == pNodePack->Parameters[0].s;
			break;
		case enumLong:
			bResult = *((int*)(pData + pNodePack->sOffset)) == pNodePack->Parameters[0].u;
			break;
		case enumInt64:
			bResult = *((__int64*)(pData + pNodePack->sOffset)) == pNodePack->Parameters[0].i64;
			break;
		case enumCharRange:
			bResult = *((char*)(pData + pNodePack->sOffset)) >= pNodePack->Parameters[0].c 
				&& *((char*)(pData + pNodePack->sOffset)) <= pNodePack->Parameters[1].c ;
			break;
		case enumShortRange:
			bResult =  *((short*)(pData + pNodePack->sOffset) ) >= pNodePack->Parameters[0].s 
				&&  *((short*)(pData + pNodePack->sOffset) ) <= pNodePack->Parameters[1].s ;
			break;
		case enumLongRange:
			bResult = *((ULONG*)(pData + pNodePack->sOffset) ) >= pNodePack->Parameters[0].u
				&& *((ULONG*)(pData + pNodePack->sOffset) ) <= pNodePack->Parameters[1].u ;
			break;
		case enumInt64Range:
			bResult = *((unsigned __int64*)(pData + pNodePack->sOffset)) >= pNodePack->Parameters[0].i64 
				&&  *((unsigned __int64*)(pData + pNodePack->sOffset)) <= pNodePack->Parameters[1].i64 ;
			break;
		case enumMem:
			bResult = ( RtlCompareMemory( pData + pNodePack->sOffset, 
				&pNodePack->Parameters[0].m.p,
				pNodePack->Parameters[0].m.l ) == pNodePack->Parameters[0].m.l );
			break;
		}

		if( !bResult )
			break;

		pNodePack = (KPackNode*)((char*)pNodePack + pNodePack->sNextNode);
	}

	return bResult;
}


//////////////////////////////////////////////////////////////////////////

BOOLEAN KAdvIpFilter::MatchHdr( PVOID pProtoHdr, KPackTemplate* packTemplate )
{
	PUDPHEADER pUdpHdr = (PUDPHEADER)pProtoHdr;		//TCP头和UDP头的前两个字都是端口

	if( packTemplate->usDesPort != 0 && packTemplate->usDesPort != ntohs(pUdpHdr->uDestinationPort) )
		return FALSE;
	if( packTemplate->usSrcPort != 0 && packTemplate->usSrcPort != ntohs(pUdpHdr->uSourcePort) )
		return FALSE;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS KAdvIpFilter::EnableIpRule( PVOID pGuidBuffer, ULONG uBufferLength, BOOLEAN bEnable )
{
	NTSTATUS bResult = STATUS_INVALID_PARAMETER;

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KAdvIpFilter::AddIpRuleToList( KDList< GUID, KPackTemplate* > *ruleList, PVOID pTemplateBuffer, ULONG uBufferLength )
{
	KPackTemplate *ipRule = (KPackTemplate *)non_paged_pool::__AllocateSystemMemory( uBufferLength );
	if( ipRule == NULL )
		return FALSE;

	RtlCopyMemory( (PVOID)ipRule, pTemplateBuffer, uBufferLength );
	ruleList->insert( ipRule->guidID, ipRule );
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS KAdvIpFilter::AddIpRule( PVOID pTemplateBuffer, ULONG uBufferLength )
{
	NTSTATUS bResult = STATUS_INVALID_PARAMETER;
	
	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_ADV_IP, "KAdvIpFilter::AddIpRule++");

	KPackTemplate *ipRule = (KPackTemplate *)pTemplateBuffer;
	KDList< GUID, KPackTemplate* > *ruleList = NULL;

	do 
	{
		if( ipRule == NULL )
			break;
		
		if( ipRule->bPackDir == enumPD_Send || ipRule->bPackDir == enumPD_Both )
		{
			ruleList = GetRuleListFromType( ipRule->protoType, enumPD_Send );
			if( ruleList == NULL )
				break;
			if( !AddIpRuleToList( ruleList, pTemplateBuffer, uBufferLength ) )
				break;
		}
		
		if( ipRule->bPackDir == enumPD_Recieve || ipRule->bPackDir == enumPD_Both )
		{
			ruleList = GetRuleListFromType( ipRule->protoType, enumPD_Recieve );
			if( ruleList == NULL )
				break;
			if( !AddIpRuleToList( ruleList, pTemplateBuffer, uBufferLength ) )
				break;
		}

		bResult = STATUS_SUCCESS;
	} while (FALSE);
	
	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_ADV_IP, "KAdvIpFilter::AddIpRule--");
	return bResult;
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS KAdvIpFilter::RemoveIpRule( PVOID pGuidBuffer, ULONG uBufferLength )
{
	KPackTemplate *ipRule = NULL;
	NTSTATUS bResult = STATUS_INVALID_PARAMETER;
	
	if( uBufferLength < sizeof(GUID) || pGuidBuffer == NULL )
		return bResult;

	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_ADV_IP, "KAdvIpFilter::RemoveIpRule++");

	RemoveIpRule( &m_udpSendList, *((GUID*)pGuidBuffer), &bResult );
	RemoveIpRule( &m_udpRecvList, *((GUID*)pGuidBuffer), &bResult );
	RemoveIpRule( &m_tcpSendList, *((GUID*)pGuidBuffer), &bResult );
	RemoveIpRule( &m_tcpRecvList, *((GUID*)pGuidBuffer), &bResult );

	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_ADV_IP, "KAdvIpFilter::RemoveIpRule--");
	return bResult;
}

//////////////////////////////////////////////////////////////////////////

VOID KAdvIpFilter::RemoveIpRule( KDList< GUID, KPackTemplate* >* ruleList, GUID ruleId, NTSTATUS *status )
{
	KPackTemplate *ipRule = NULL;
	if( ruleList->erase( ruleId, &ipRule ) )
	{
		non_paged_pool::__DeallocateSystemMemory( (PVOID)ipRule );
		*status = STATUS_SUCCESS;
	}
}


//////////////////////////////////////////////////////////////////////////

NTSTATUS KAdvIpFilter::ClearIpRule()
{
	ClearList( &m_udpSendList );
	ClearList( &m_udpRecvList );
	ClearList( &m_tcpSendList );
	ClearList( &m_tcpRecvList );
	return STATUS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////

VOID KAdvIpFilter::ClearList( KDList< GUID, KPackTemplate* > *pList )
{
	KIRQL oldirq;
	KDList< GUID, KPackTemplate* >::iterator item;
	KPackTemplate* ipRule = NULL;
	pList->lock( &oldirq );
	for ( item = pList->begin(); item != pList->end(); item = pList->next(item) )
	{
		ipRule = pList->get_data( item );
		non_paged_pool::__DeallocateSystemMemory( (PVOID)ipRule );
	}
	pList->clear();
	pList->unlock( oldirq );

}

//////////////////////////////////////////////////////////////////////////

KDList< GUID, KPackTemplate* >* KAdvIpFilter::GetRuleListFromType( PROTO_TYPE protoType, PACKET_DIR enumDir )
{
	KDList<GUID,KPackTemplate*> *ipRules = NULL;

	switch( protoType )
	{
	case enumPT_TCP:
		if( enumDir == enumPD_Send )
			ipRules = &m_tcpSendList;
		else
			ipRules = &m_tcpRecvList;

		break;
	case enumPT_UDP:
		if( enumDir == enumPD_Send )
			ipRules = &m_udpSendList;
		else
			ipRules = &m_udpRecvList;
		break;
	}

	return ipRules;
}


//////////////////////////////////////////////////////////////////////////

BOOLEAN KAdvIpFilter::GetPackData( IPHEADER *pIpHdr, ULONG uIpPackLen, PVOID *pProtoHdr, char** pPackData, ULONG *uDataLen )
{
	ULONG dwLeastSize = 0;

	if( pIpHdr == NULL || uIpPackLen == 0 || pProtoHdr == NULL || pPackData == NULL || uDataLen == NULL )
		return FALSE;
	
	switch( pIpHdr->byProtocolType )
	{
	case IPHEADER_UDP:
		{
			if ( uIpPackLen <= sizeof(IPHEADER) + sizeof(UDPHEADER) )
				break;

			dwLeastSize = sizeof(IPHEADER) + sizeof(UDPHEADER);
		}
		break;
	case IPHEADER_TCP:
		{
			if ( uIpPackLen <= sizeof(IPHEADER) + sizeof(TCPHEADER) )
				break;

			dwLeastSize = sizeof(IPHEADER) + sizeof(TCPHEADER);
		}
		break;
	}
	
	if( dwLeastSize )
	{
		*pProtoHdr = (char*)pIpHdr + sizeof(IPHEADER);
		*pPackData = (char*)pIpHdr + dwLeastSize;
		*uDataLen = uIpPackLen - dwLeastSize;
	}

	return dwLeastSize != 0;
}

//////////////////////////////////////////////////////////////////////////
