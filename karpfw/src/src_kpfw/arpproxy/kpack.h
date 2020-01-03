#pragma once
#include "netwatch.h"

//#define USE_XML

#ifdef USE_XML
#include "../../../../../3rdparty/XmlLib/src/Common/tinyxml.h"
#include <map>
#endif

using namespace std;

#define OFFSET_ETHER_TYPE	12
#define OFFSET_IP_TYPE		23
#define OFFSET_IP_ADDR		26
#define OFFSET_SRC_PORT		34
#define OFFSET_TCP_FLAG	    47
#define OFFSET_ICMP_TYPE	34
#define OFFSET_ICMP_CODE	35

#define FLAG_IP_UDP			0x11
#define FLAG_IP_TCP			0x06
#define FLAG_IP_ICMP		0x01


class KPack
{
public:
	KPack(void);
	~KPack(void);
	#ifdef USE_XML
	KPack( const wchar_t* szXmlFile );
	KPack( const char* szXml );
	bool Construct( TiXmlDocument &xmlDoc );
	#endif
	KPack( KIP_FILTER_INFO* pIpRule );
	KPack( PVOID pTemplateBuffer, ULONG uBufferLength );
	BOOL MatchPackage( const char* pRawPackage, DWORD dwRawPackLength );
	
	PBYTE GetTemplateBuffer()
	{
		return (PBYTE)m_packTemplate;
	}

	DWORD GetBufferSize()
	{
		return m_nTemplateSize;
	}
	
	const char* GetDesc()
	{
		if( m_packTemplate != NULL )
			return m_packTemplate->szDesc;
		return NULL;
	}
private:

#ifdef USE_XML
	bool GetNodeType( TiXmlElement* pNodeElement, CompType &cmpType );
	bool GetNodeOffset( TiXmlElement* pNodeElement, const int nMaxOffset, int &nOffset );
	bool GetNodeDataCount( TiXmlElement* pNodeElement, const CompType cType, int &nCount );
	bool GetNodeData( TiXmlElement* pNodeElement, const CompType cType, const int nIdx, KPackNode* pPackNode );
#endif

	int GetProtocolSize( PROTO_TYPE ProtocolType );
	int GetProtoTypeNode( PROTO_TYPE ProtocolType, list< pair< KPackNode*, int > > &packList );
	int GetIPAddrNode( PROTO_TYPE ProtocolType, const FILTER_IP_ADDR* ipAddr, list< pair< KPackNode*, int > > &packList, bool bRemote );
	int GetPortNode( PROTO_TYPE ProtocolType, const FILTER_PORT* port, list< pair< KPackNode*, int > > &packList, bool bRemote );
	int GetExtraNode( PROTO_TYPE ProtocolType, const FILTERINFO_EXTRADATA* extraData, list< pair< KPackNode*, int > > &packList );
	bool AllocPackTemplate( list< pair< KPackNode*, int > > &packList, int nAllNodeSize );
	
	template <typename T >
	int GetSingleNode( CompType cType, short sOffset, T tData, list< pair< KPackNode*, int > > &packList );
	template <typename T >
	int GetRangeNode( CompType cType, short sOffset, T tStart, T tEnd, list< pair< KPackNode*, int > > &packList );

private:
	int m_nTemplateSize;
	KPackTemplate *m_packTemplate;
};
