#include "StdAfx.h"
#include "kpack.h"

//
//ULONG __stdcall ntohl(ULONG netlong)
//{
//	ULONG ByteSwapped;
//
//	ByteSwapped = ((netlong & 0x00ff00ff) << 8) | ((netlong & 0xff00ff00) >> 8);
//
//	return (ByteSwapped << 16) | (ByteSwapped >> 16);
//}
//
//USHORT __stdcall ntohs(USHORT netshort)
//{
//	return ((netshort << 8) | (netshort >> 8));
//}

#define ntohll(x) (((unsigned __int64)(ntohl((unsigned int)((x << 32) >> 32))) << 32) | (unsigned int)ntohl(((unsigned int)(x >> 32)))) 

KPack::KPack(void)
{
	m_packTemplate = NULL;
	m_nTemplateSize = 0;
}

KPack::~KPack(void)
{
	if( m_packTemplate != NULL )
	{
		free((PVOID)m_packTemplate);
		m_packTemplate = NULL;
	}
	m_nTemplateSize = 0;
}

//////////////////////////////////////////////////////////////////////////

BOOL KPack::MatchPackage( const char* pRawPackage, DWORD dwRawPackLength )
{

	KPackNode* pNodePack = NULL;
	BOOL bResult = FALSE;
	
	if( m_packTemplate == NULL )
		return FALSE;

	if( dwRawPackLength < (DWORD)m_packTemplate->uPackSize )
		return FALSE;
	
	pNodePack = m_packTemplate->packNode;

	for( int i=0; i<m_packTemplate->sNodeCount; i++ )
	{
		switch( pNodePack->cType )
		{
		case enumChar:
			bResult = *((unsigned char*)(pRawPackage + pNodePack->sOffset)) == pNodePack->Parameters[0].c;
			break;
		case enumShort:
			bResult = *((unsigned short*)(pRawPackage + pNodePack->sOffset)) == pNodePack->Parameters[0].s;
			break;
		case enumLong:
			bResult = *((unsigned int*)(pRawPackage + pNodePack->sOffset)) == pNodePack->Parameters[0].u;
			break;
		case enumInt64:
			bResult = *((unsigned __int64*)(pRawPackage + pNodePack->sOffset)) == pNodePack->Parameters[0].i64;
			break;
		case enumCharRange:
			bResult = *((unsigned char*)(pRawPackage + pNodePack->sOffset)) >= pNodePack->Parameters[0].c 
						&& *((unsigned char*)(pRawPackage + pNodePack->sOffset)) <= pNodePack->Parameters[1].c ;
			break;
		case enumShortRange:
			bResult = ntohs( *((unsigned short*)(pRawPackage + pNodePack->sOffset) ) ) >= pNodePack->Parameters[0].s 
				&& ntohs( *((unsigned short*)(pRawPackage + pNodePack->sOffset) ) ) <= pNodePack->Parameters[1].s ;
			break;
		case enumLongRange:
			bResult = ntohl( *((unsigned int*)(pRawPackage + pNodePack->sOffset) ) ) >= pNodePack->Parameters[0].u
				&& ntohl( *((unsigned int*)(pRawPackage + pNodePack->sOffset) ) ) <= pNodePack->Parameters[1].u ;
			break;
		case enumInt64Range:
			bResult = ntohll(*((unsigned __int64*)(pRawPackage + pNodePack->sOffset))) >= pNodePack->Parameters[0].i64 
				&&  ntohll(*((unsigned __int64*)(pRawPackage + pNodePack->sOffset))) <= pNodePack->Parameters[1].i64 ;
			break;
		case enumCharAny:
		case enumShortAny:
		case enumLongAny:
		case enumInt64Any:
			bResult = true;
			break;
		}
		
		if( !bResult )
			break;

		pNodePack = (KPackNode*)((char*)pNodePack + pNodePack->sNextNode);
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////////
#ifdef USE_XML

bool KPack::GetNodeType( TiXmlElement* pNodeElement, CompType &cmpType )
{
	map <string , CompType> typeList;
	typeList["CHAR"] = enumChar;
	typeList["SHORT"] = enumShort;
	typeList["LONG"] = enumLong;
	typeList["I64"] = enumInt64;
	typeList["CRANGE"] = enumCharRange;
	typeList["SRANGE"] = enumShortRange;
	typeList["LRANGE"] = enumLongRange;
	typeList["I64RANGE"] = enumInt64Range;
	typeList["CMULT"] = enumCharMult;
	typeList["SMULT"] = enumShortMult;
	typeList["LMULT"] = enumLongMult;
	typeList["I64MULT"] = enumInt64Mult;
	typeList["CANY"] = enumCharAny;
	typeList["SANY"] = enumShortAny;
	typeList["LANY"] = enumLongAny;
	typeList["I64ANY"] = enumInt64Any;


	const char* szType = pNodeElement->Attribute( "Type" );
	if( szType == NULL )
		return false;
	
	map <string , CompType>::iterator item = typeList.find(szType);
	if( item == typeList.end() )
		return false;

	cmpType = item->second;
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool KPack::GetNodeOffset( TiXmlElement* pNodeElement, const int nMaxOffset, int &nOffset )
{
	return  (TIXML_SUCCESS == pNodeElement->QueryIntAttribute( "offset", &nOffset ))
				&& nOffset > 0
				&& nOffset < nMaxOffset
				&& nOffset < 65536;
}

//////////////////////////////////////////////////////////////////////////

bool KPack::GetNodeData( TiXmlElement* pNodeElement, const CompType cType, const int nIdx, KPackNode* pPackNode )
{
	char szDataName[16] = {0};
	
	if( cType >= enumCharAny && cType <= enumInt64Any )
		return true;

	sprintf_s( szDataName, "Data%d", nIdx );
	const char* szData = pNodeElement->Attribute( szDataName );
	if( szData == NULL )
		return false;

	switch(cType)
	{
	case enumChar:
	case enumCharRange:
	case enumCharMult:
		sscanf_s( szData, "%02X", &pPackNode->Parameters[nIdx].c );
		break;

	case enumShort:
	case enumShortRange:
	case enumShortMult:
		sscanf_s( szData, "%04X", &pPackNode->Parameters[nIdx].s );
		break;

	case enumLong:
	case enumLongRange:
	case enumLongMult:
		sscanf_s( szData, "%08X", &pPackNode->Parameters[nIdx].u );
		break;

	case enumInt64:
	case enumInt64Range:
	case enumInt64Mult:
		sscanf_s( szData, "%I64X", &pPackNode->Parameters[nIdx].i64 );
		break;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool KPack::GetNodeDataCount( TiXmlElement* pNodeElement, const CompType cType, int &nCount  )
{
	switch(cType)
	{
	case enumChar:
	case enumShort:
	case enumLong:
	case enumInt64:

	case enumCharAny:
	case enumShortAny:
	case enumLongAny:
	case enumInt64Any:
		nCount = 1;
		return true;

	case enumCharRange:
	case enumShortRange:
	case enumLongRange:
	case enumInt64Range:
		nCount = 2;
		return true;

	case enumCharMult:
	case enumShortMult:
	case enumLongMult:
	case enumInt64Mult:
		if( TIXML_SUCCESS == pNodeElement->QueryIntAttribute( "Count", &nCount ) 
				&& nCount > 0 
				&& nCount < 256 )
		{
			return true;
		}
		break;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////
//string sXml = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\
//			  <Filter Desc=\"Tcp_XXX\" Size=\"200\">\
//			  <Node offset=\"10\" Data0=\"11223344\" Type =\"ULONG\"/>\
//			  <Node offset=\"20\" Data0=\"11223344\" Data1=\"66778899\"  Type =\"URANGE\"/>\
//			  <Node offset=\"28\" Data0=\"11\" Type =\"CHAR\"/>\
//			  </Filter>";
KPack::KPack( const char *szXml )
{
	TiXmlDocument xmlDoc;

	xmlDoc.Parse( szXml );
	if( xmlDoc.Error() )
		throw;

	if( !Construct( xmlDoc ) )
		throw;

}

KPack::KPack( const wchar_t* szXmlFile )
{
	char *pBuffer = NULL;
	DWORD dwRead = 0;
	bool bResult = false;
	DWORD dwFileSize = 0;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	TiXmlDocument xmlDoc;

	do 
	{
		hFile = CreateFile( szXmlFile, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
		if( hFile == INVALID_HANDLE_VALUE )
			break;

		dwFileSize = GetFileSize( hFile , 0 );
		pBuffer = new char[dwFileSize];
		if( pBuffer == NULL )
			break;

		if( !ReadFile(hFile, pBuffer, dwFileSize, &dwRead, 0) )
			break;

		xmlDoc.Parse( pBuffer );
		if( xmlDoc.Error() )
			break;

		if( !Construct( xmlDoc ) )
			break;
		
		bResult = true;
	} while (false);


	if( hFile != INVALID_HANDLE_VALUE )
	{
		CloseHandle( hFile );
		hFile = INVALID_HANDLE_VALUE;
	}

	if( pBuffer != NULL )
	{
		delete []pBuffer;
		pBuffer = NULL;
	}
	
	if( !bResult )
		throw;
}

//////////////////////////////////////////////////////////////////////////

bool KPack::Construct( TiXmlDocument &xmlDoc )
{
	TiXmlElement *pFilterElement = NULL;
	TiXmlElement *pNodeElement = NULL;

	m_packTemplate = NULL;
	m_nTemplateSize = 0;

	int nAllNodeSize = 0;
	int nPackSize = 0;
	int nAllocSize = 0;
	list< pair< KPackNode*, int > > packNodeList;


	pFilterElement = xmlDoc.FirstChildElement( "Filter" );
	if( pFilterElement == NULL )
		throw;

	pNodeElement = pFilterElement->FirstChildElement( "Node" );
	if( pNodeElement == NULL )
		throw;

	if( TIXML_SUCCESS != pFilterElement->QueryIntAttribute( "Size", &nPackSize ) 
		|| nPackSize < 0 
		|| nPackSize > 65536 )
		return false;

	const char *pszDesc = pFilterElement->Attribute( "Desc" );
	if( pszDesc == NULL )
		return false;

	do 
	{	
		CompType cType;
		int nOffset = 0;
		int nCount = 0;
		KPackNode* packNode = NULL;

		if( !GetNodeType( pNodeElement, cType ) )
			return false;

		if( !GetNodeOffset( pNodeElement, nPackSize, nOffset ) )
			return false;

		if( !GetNodeDataCount( pNodeElement, cType, nCount ) )
			return false;

		nAllocSize = sizeof(KPackNode) + sizeof(__int64) * (nCount - 1);
		packNode = (KPackNode*)malloc( nAllocSize );
		if( packNode == NULL )
			return false;
		memset( (PVOID)packNode, 0, nAllocSize );

		packNode->bDataCount = nCount;
		packNode->cType = cType;
		packNode->sOffset = nOffset;
		packNode->sNextNode = nAllocSize;
		for( int i=0; i<nCount; i++ )
		{
			if( !GetNodeData( pNodeElement, cType, i, packNode ) )
			{
				if( packNode != NULL )
				{
					free( (PVOID)packNode );
					packNode = NULL;
				}

				for( list< pair< KPackNode*, int > >::iterator item = packNodeList.begin(); item != packNodeList.end(); item++ )
					free( (PVOID)item->first );

				return false;
			}
		}

		packNodeList.push_back( make_pair(packNode, nAllocSize) );
		nAllNodeSize += nAllocSize;
	} while ( pNodeElement = pNodeElement->NextSiblingElement() );


	if( !AllocPackTemplate( packNodeList, nAllNodeSize ) )
		return false;

	m_packTemplate->uPackSize = nPackSize;
	strcpy_s( m_packTemplate->szDesc, 32, pszDesc );
	return true;
}

#endif // USE_XML

//////////////////////////////////////////////////////////////////////////

KPack::KPack( PVOID pTemplateBuffer, ULONG uBufferLength )
{
	m_packTemplate = (KPackTemplate *)malloc( uBufferLength );
	if( m_packTemplate == NULL )
		throw;
	memcpy( m_packTemplate, pTemplateBuffer, uBufferLength );
	m_nTemplateSize = uBufferLength;
}

//////////////////////////////////////////////////////////////////////////

KPack::KPack( KIP_FILTER_INFO* pIpRule )
{
	m_packTemplate = NULL;
	m_nTemplateSize = 0;
	int nAllocSize = 0;
	if( pIpRule == NULL )
		throw;
	
	bool bResult = false;
	list< pair< KPackNode*, int > > packNodeList;
	
	do 
	{
		nAllocSize = GetProtoTypeNode( pIpRule->filter.ProtocolType, packNodeList );
		if( !nAllocSize )
			break;
		nAllocSize += GetIPAddrNode( pIpRule->filter.ProtocolType, &pIpRule->filter.LocalAddr, packNodeList, false );
		nAllocSize += GetIPAddrNode( pIpRule->filter.ProtocolType, &pIpRule->filter.RemoteAddr, packNodeList, true );
		nAllocSize += GetExtraNode( pIpRule->filter.ProtocolType, &pIpRule->filter.FilterInfo_ExtraData, packNodeList );

	} while (FALSE);

	if( !AllocPackTemplate( packNodeList, nAllocSize ) )
		throw;
	m_packTemplate->bPackDir = pIpRule->filter.Direction;
	m_packTemplate->bOperation = pIpRule->filter.Operation ? enumIP_Block : enumIP_Pass;
	m_packTemplate->uPackSize = GetProtocolSize( pIpRule->filter.ProtocolType );
	m_packTemplate->guidID = pIpRule->id;
}

//////////////////////////////////////////////////////////////////////////

bool KPack::AllocPackTemplate( list< pair< KPackNode*, int > > &packNodeList, int nAllNodeSize )
{
	bool bResult = false;
	int nAllocSize = sizeof(KPackTemplate) - sizeof(KPackNode) + nAllNodeSize;
	m_packTemplate = (KPackTemplate*)malloc( nAllocSize );
	if( m_packTemplate != NULL )
	{
		memset( (char*)m_packTemplate, 0, nAllocSize );
		m_packTemplate->cb = sizeof(KPackTemplate);
		m_packTemplate->sNodeCount = (short)packNodeList.size();

		int nMemPos = 0;
		for( list< pair< KPackNode*, int > >::iterator item = packNodeList.begin(); item != packNodeList.end(); item++ )
		{
			memcpy( (char*)m_packTemplate->packNode + nMemPos, (PVOID)item->first, item->second );
			nMemPos += item->second;
		}

		m_nTemplateSize = nAllocSize;
		bResult = true;
	}

	for( list< pair< KPackNode*, int > >::iterator item = packNodeList.begin(); item != packNodeList.end(); item++ )
		free( (PVOID)item->first );

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

int KPack::GetProtoTypeNode( PROTO_TYPE ProtocolType, list< pair< KPackNode*, int > > &packList )
{
	int nResult = 0;
	KPackNode *pPack = NULL;
	
	switch( ProtocolType )
	{
	case enumPT_UDP:
		{
			nResult += GetSingleNode( enumShort, OFFSET_ETHER_TYPE, ETHER_IP, packList );
			nResult += GetSingleNode( enumChar, OFFSET_IP_TYPE, FLAG_IP_UDP, packList );
		}
		break;
	case enumPT_TCP:
		{
			nResult += GetSingleNode( enumShort, OFFSET_ETHER_TYPE, ETHER_IP, packList );
			nResult += GetSingleNode( enumChar, OFFSET_IP_TYPE, FLAG_IP_TCP, packList );

		}
		break;
	case enumPT_ICMP:
		{
			nResult += GetSingleNode( enumShort, OFFSET_ETHER_TYPE, ETHER_IP, packList );
			nResult += GetSingleNode( enumChar, OFFSET_IP_TYPE, FLAG_IP_ICMP, packList );
		}
		break;
	case enumPT_IGMP:
		break;
	}

	return nResult;
}

//////////////////////////////////////////////////////////////////////////

int KPack::GetIPAddrNode( PROTO_TYPE ProtocolType, const FILTER_IP_ADDR* ipAddr, list< pair< KPackNode*, int > > &packList, bool bRemote )
{
	short nOffset = 0;
	int nResult = 0;
	KPackNode *pPack = NULL;
	if( ipAddr->eMode == enumPM_Any )
		return nResult;
	
	nOffset = bRemote ? OFFSET_IP_ADDR + sizeof(ULONG) : OFFSET_IP_ADDR;

	if( ipAddr->eMode == enumPM_Single )
	{

		nResult = GetSingleNode( enumLong, 
			nOffset, 
			ipAddr->ipAddr, 
			packList );
	}
	else
	{
		nResult = GetRangeNode( enumLongRange, 
			nOffset, 
			ipAddr->ipRange.ipStart, 
			ipAddr->ipRange.ipEnd,
			packList );
	}

	return nResult;
}

//////////////////////////////////////////////////////////////////////////

int KPack::GetProtocolSize( PROTO_TYPE ProtocolType )
{
	//typedef enum tagProtoType {
	//	enumPT_TCP = 0,
	//	enumPT_UDP = 1,
	//	enumPT_ICMP = 2,
	//	enumPT_IP = 3,
	//	enumPT_IGMP = 4,
	//	enumPT_RAWIP = 5,
	//	enumPT_LANMAN = 6,
	//	enumPT_HTTP = 7
	//} PROTO_TYPE;

	const unsigned short nProtocolSize[] = 
	{
		sizeof(ETHHEADER) + sizeof(IPHEADER) + sizeof(TCPHEADER),		//enumPT_TCP
		sizeof(ETHHEADER) + sizeof(IPHEADER) + sizeof(UDPHEADER),		//enumPT_UDP
		sizeof(ETHHEADER) + sizeof(IPHEADER) + sizeof(ICMPHEADER),		//enumPT_ICMP
		sizeof(ETHHEADER) + sizeof(IPHEADER),							//enumPT_IP
		sizeof(ETHHEADER) + sizeof(IPHEADER) + sizeof(IGMPHEADER)		//enumPT_IGMP
	};
	
	if( ProtocolType < enumPT_TCP || ProtocolType > enumPT_IGMP )
		return 0;

	return nProtocolSize[ProtocolType];
}

//////////////////////////////////////////////////////////////////////////

int KPack::GetPortNode( PROTO_TYPE ProtocolType, const FILTER_PORT* port, list< pair< KPackNode*, int > > &packList, bool bRemote )
{
	int nResult = 0;
	KPackNode *pPack = NULL;
	short nOffset = 0;
	if( port->eMode == enumPM_Any )
		return 0;

	nOffset = bRemote ? OFFSET_SRC_PORT + sizeof(short) : OFFSET_SRC_PORT;
	if( port->eMode == enumPM_Single )
	{
		nResult = GetSingleNode( enumShort, nOffset, port->port, packList );
	}
	else if( port->eMode == enumPM_Range )
	{
		nResult = GetRangeNode( enumShortRange, nOffset, port->portRange.nStart, port->portRange.nEnd, packList );
	}
	
	return nResult;
}

//////////////////////////////////////////////////////////////////////////

int KPack::GetExtraNode( PROTO_TYPE ProtocolType, const FILTERINFO_EXTRADATA* extraData, list< pair< KPackNode*, int > > &packList )
{
	int nResult = 0;
	KPackNode *pPack = NULL;

	switch( ProtocolType )
	{
	case enumPT_UDP:
		{
			nResult += GetPortNode( ProtocolType, &extraData->UDP_DATA.LocalPort, packList, false );
			nResult += GetPortNode( ProtocolType, &extraData->UDP_DATA.RemotePort, packList, true );
		}
		break;
	case enumPT_TCP:
		{
			if( extraData->TCP_DATA.byTcpFlags & 0x80 )
				nResult += GetSingleNode( enumChar, OFFSET_TCP_FLAG, extraData->TCP_DATA.byTcpFlags & 0x7f, packList );

			nResult += GetPortNode( ProtocolType, &extraData->TCP_DATA.LocalPort, packList, false );
			nResult += GetPortNode( ProtocolType, &extraData->TCP_DATA.RemotePort, packList, true );
		}
		break;
	case enumPT_ICMP:
		{
			if( extraData->ICMP_DATA.byCode != 0xff )
				nResult += GetSingleNode( enumChar, OFFSET_ICMP_TYPE, extraData->ICMP_DATA.byCode, packList );
			if( extraData->ICMP_DATA.byType != 0xff )
				nResult += GetSingleNode( enumChar, OFFSET_ICMP_CODE, extraData->ICMP_DATA.byType, packList );
		}
		break;
	}
	return nResult;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template <typename T >
int KPack::GetSingleNode( CompType cType, short sOffset, T tData, list< pair< KPackNode*, int > > &packList )
{
	KPackNode *pPack = (KPackNode*)malloc( sizeof( KPackNode ) );
	if( pPack == NULL )
		return 0;

	memset( (char*)pPack, 0, sizeof(KPackNode) );
	pPack->sOffset = sOffset;
	pPack->bDataCount = 1;
	*((T*)&pPack->Parameters->c) = tData;
	pPack->cType = cType;
	pPack->sNextNode = sizeof(KPackNode);
	packList.push_back( make_pair(pPack, (int)sizeof(KPackNode) ) );

	return sizeof(KPackNode);
}

//////////////////////////////////////////////////////////////////////////

template <typename T >
int KPack::GetRangeNode( CompType cType, short sOffset, T tStart, T tEnd, list< pair< KPackNode*, int > > &packList )
{
	KPackNode *pPack = NULL;
	int nAllocSize = sizeof( KPackNode ) + sizeof(__int64);
	pPack = (KPackNode*)malloc( nAllocSize );
	if( pPack == NULL )
		return 0;

	memset( (char*)pPack, 0, nAllocSize );

	pPack->cType = cType;
	pPack->bDataCount = 2;
	pPack->sOffset = sOffset;
	*((T*)&pPack->Parameters[0].c) = tStart;
	*((T*)&pPack->Parameters[1].c) = tEnd;
	pPack->sNextNode = nAllocSize;
	
	packList.push_back( make_pair(pPack, nAllocSize ) );

	return sizeof( KPackNode ) + sizeof(__int64);
}
