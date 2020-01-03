#pragma once




class KAdvIpFilter
{
public:
	BOOLEAN Init( PDRIVER_OBJECT DriverObject = NULL, PUNICODE_STRING RegistryPath = NULL );
	BOOLEAN Uninit( PDRIVER_OBJECT DriverObject = NULL );
	IPRESULT Filter( PKANTIARP_ADAPTER adapter, PVOID pBuffer, ULONG uBufferLength, PACKET_DIR enumDir );
	NTSTATUS EnableIpRule( PVOID pGuidBuffer, ULONG uBufferLength, BOOLEAN bEnable );
	NTSTATUS AddIpRule( PVOID pTemplateBuffer, ULONG uBufferLength );
	NTSTATUS RemoveIpRule( PVOID pGuidBuffer, ULONG uBufferLength );
	NTSTATUS ClearIpRule();
private:

	BOOLEAN GetPackData( IPHEADER *pIpHdr, ULONG uIpPackLen, PVOID *pProtoHdr, char** pPackData, ULONG *uDataLen );
	IPRESULT MatchIpRuleList( PVOID pProtoHdr, char* pData, ULONG uDataLen, KDList< GUID, KPackTemplate* > *ipRules, /*out*/GUID *ruleID );
	BOOLEAN MatchIpRule( char* pPackBuffer, ULONG uPackLength, KPackTemplate* packTemplate );
	BOOLEAN MatchHdr( PVOID pProtoHdr, KPackTemplate* packTemplate );
	
	BOOLEAN AddIpRuleToList( KDList< GUID, KPackTemplate* > *ruleList, PVOID pTemplateBuffer, ULONG uBufferLength );
	VOID ClearList( KDList< GUID, KPackTemplate* > *pList );
	VOID RemoveIpRule( KDList< GUID, KPackTemplate* >* ruleList, GUID ruleId, NTSTATUS *status );
	KDList< GUID, KPackTemplate* >* GetRuleListFromType( PROTO_TYPE protoType, PACKET_DIR enumDir );

private:
	
	KDList< GUID, KPackTemplate* >  m_udpSendList;
	KDList< GUID, KPackTemplate* >  m_udpRecvList;
	KDList< GUID, KPackTemplate* >  m_tcpSendList;
	KDList< GUID, KPackTemplate* >  m_tcpRecvList;

};
