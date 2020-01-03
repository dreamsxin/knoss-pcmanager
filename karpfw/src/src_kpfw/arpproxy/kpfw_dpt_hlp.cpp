////////////////////////////////////////////////////////////////////////////////
//      
//      common include File for kpfw
//      
//      File      : kpfw_dpt_hlp.cpp
//      Version   : 1.0
//      Comment   : 定义网镖服务和界面之间的转换辅助类
//      
//      Create at : 2009-07-15
//      Create by : chenguicheng
//      
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "kpfw_dpt_hlp.h"

namespace Dpt_hlp
{
	void ToMd5( const Kxe_MD5_Param* md5Param, BYTE md5[16] )
	{
		if( md5Param && md5 )
			memcpy( md5, md5Param, sizeof(BYTE)*16 );
	}

	void FromMd5( Kxe_MD5_Param* md5Param, const BYTE md5[16] )
	{
		if ( md5Param && md5 )
			memcpy( md5Param, md5, sizeof(BYTE)*16 );
	}
	
	void ToMAC( const Kxe_MAC_Param* macParam, BYTE mac[6] )
	{
		if( macParam && mac)
			memcpy( mac, macParam, sizeof(BYTE)*6  );
	}

	void FromMAC( Kxe_MAC_Param* macParam, const BYTE mac[6] )
	{
		if( macParam && mac )
			memcpy( macParam, mac, sizeof(BYTE)* 6 );
	}

	void ToGUID( const Kxe_GUID_Param* pGuidParam, GUID* pGuid )
	{
		if( pGuidParam && pGuid)
			memcpy( pGuid, pGuidParam, sizeof(GUID) );
	}

	void FromGUID( Kxe_GUID_Param* pGuidParam, const GUID* pGuid )
	{
		if( pGuidParam && pGuid)
			memcpy( pGuidParam, pGuid, sizeof(GUID) );
	}

	void ToIpFilterAddr( const Kxe_FilterIPAddr_Param& filterParam, FILTER_IP_ADDR& filter )
	{
		filter.eMode = (IP_ADDR_MODE)filterParam.eMode;
		filter.ipAddr = filterParam.IpAddr.ipAddr;
		filter.ipRange = filterParam.IpAddr.ipRange;
	}

	void FromIpFilterAddr( Kxe_FilterIPAddr_Param& filterParam, const FILTER_IP_ADDR& filter )
	{
		filterParam.eMode = (int)(filter.eMode);
		filterParam.IpAddr.ipAddr = filter.ipAddr;
		filterParam.IpAddr.ipRange = filter.ipRange;
	}

	void ToFilterPort( const Kxe_FilterPort_Param& filerParam, FILTER_PORT& filter )
	{
		filter.eMode = (PORT_MODE)filerParam.eMode;
		filter.port = filerParam.port.port;
		filter.portRange = filerParam.port.portRange;
	}
	void FromFilterPort( Kxe_FilterPort_Param& filerParam, const FILTER_PORT& filter )
	{
		filerParam.eMode = (int)(filter.eMode);
		filerParam.port.port = filter.port;
		filerParam.port.portRange = filter.portRange;
	}

	void ToTCP_DATA( const Kxe_TCPData_Param& tcpParam, TCP_EXTRA_DATA& tcp)
	{
		ToFilterPort( tcpParam.LocalPort, tcp.LocalPort );
		ToFilterPort( tcpParam.RemotePort, tcp.RemotePort );
		tcp.byTcpFlags = tcpParam.byTcpFlags;
	}

	void FromTCP_DATA( Kxe_TCPData_Param& tcpParam, const TCP_EXTRA_DATA& tcp)
	{
		FromFilterPort( tcpParam.LocalPort, tcp.LocalPort );
		FromFilterPort( tcpParam.RemotePort, tcp.RemotePort );
		tcpParam.byTcpFlags = tcp.byTcpFlags;
	}

	void ToUDP_DATA( const Kxe_UDPData_Param& udpParam, UDP_EXTRA_DATA& udp )
	{
		ToFilterPort( udpParam.LocalPort, udp.LocalPort );
		ToFilterPort( udpParam.RemotePort, udp.RemotePort );
	}

	void FromUDP_DATA( Kxe_UDPData_Param& udpParam, const UDP_EXTRA_DATA& udp )
	{
		FromFilterPort( udpParam.LocalPort, udp.LocalPort );
		FromFilterPort( udpParam.RemotePort, udp.RemotePort );
	}

	void ToICMP_DATA( const Kxe_ICMPData_Param& icmpParam, ICMP_EXTRA_DATA& icmp )
	{
		icmp.byType = icmpParam.byType;
		icmp.byCode = icmpParam.byCode;
	}

	void FromICMP_DATA( Kxe_ICMPData_Param& icmpParam, const ICMP_EXTRA_DATA& icmp )
	{
		icmpParam.byType = icmp.byType;
		icmpParam.byCode = icmp.byCode;
	}

	void ToIGMP_DATA( const Kxe_IGMPData_Param& igmpParam, IGMP_EXTRA_DATA& igmp )
	{
		igmp.byType = igmpParam.byType;
		igmp.byTypeOfGroup = igmpParam.byTypeOfGroup;
		igmp.uMulticastAddr = igmpParam.uMulticastAddr;
	}

	void FromIGMP_DATA( Kxe_IGMPData_Param& igmpParam, const IGMP_EXTRA_DATA& igmp )
	{
		igmpParam.byType = igmp.byType;
		igmpParam.byTypeOfGroup = igmp.byTypeOfGroup;
		igmpParam.uMulticastAddr = igmp.uMulticastAddr;
	}

	void ToDataUnion( PROTO_TYPE Protoc, const DataUnion& dataUnion, FILTERINFO_EXTRADATA& data )
	{
		switch (Protoc)
		{
		case enumPT_TCP:
			ToTCP_DATA( dataUnion.TCP_DATA, data.TCP_DATA );
			break;
		case enumPT_UDP:
			ToUDP_DATA( dataUnion.UDP_DATA, data.UDP_DATA );
			break;
		case enumPT_ICMP:
			ToICMP_DATA( dataUnion.ICMP_DATA, data.ICMP_DATA );
			break;
		case enumPT_IGMP:
			ToIGMP_DATA( dataUnion.IGMP_DATA, data.IGMP_DATA );
			break;
		default:
			break;
		}
	}

	void FromDataUnion( PROTO_TYPE Protoc, DataUnion& dataUnion, const FILTERINFO_EXTRADATA& data )
	{
		memset(&dataUnion, 0, sizeof(DataUnion));
		switch (Protoc)
		{
		case enumPT_TCP:
			FromTCP_DATA( dataUnion.TCP_DATA, data.TCP_DATA );
			break;
		case enumPT_UDP:
			FromUDP_DATA( dataUnion.UDP_DATA, data.UDP_DATA );
			break;
		case enumPT_ICMP:
			FromICMP_DATA( dataUnion.ICMP_DATA, data.ICMP_DATA );
			break;
		case enumPT_IGMP:
			FromIGMP_DATA( dataUnion.IGMP_DATA, data.IGMP_DATA );
			break;
		default:
			break;
		}
	}

	void ToFilterInfo( const Kxe_FilterInfo_Param& filterInfoParam, FILTER_INFO& filterInfo )
	{
		filterInfo.cbSize		= filterInfoParam.cbSize;
		filterInfo.nVer			= filterInfoParam.nVer;
		filterInfo.ProtocolType = (PROTO_TYPE)filterInfoParam.ProtocolType;
		filterInfo.Direction	= (PACKET_DIR)filterInfoParam.Direction;
		filterInfo.Operation	= filterInfoParam.Operation;

		ToIpFilterAddr( filterInfoParam.LocalAddr, filterInfo.LocalAddr );
		ToIpFilterAddr( filterInfoParam.RemoteAddr, filterInfo.RemoteAddr );

		ToDataUnion( filterInfo.ProtocolType, filterInfoParam.Data, filterInfo.FilterInfo_ExtraData );
	}

	void FromFilterInfo( Kxe_FilterInfo_Param& filterInfoParam, const FILTER_INFO& filterInfo )
	{
		filterInfoParam.cbSize			= filterInfo.cbSize;
		filterInfoParam.nVer			= filterInfo.nVer;
		filterInfoParam.ProtocolType	= (int)filterInfo.ProtocolType;
		filterInfoParam.Direction		= (int)filterInfo.Direction;
		filterInfoParam.Operation		= filterInfo.Operation;

		FromIpFilterAddr( filterInfoParam.LocalAddr, filterInfo.LocalAddr );
		FromIpFilterAddr( filterInfoParam.RemoteAddr, filterInfo.RemoteAddr );

		FromDataUnion( filterInfo.ProtocolType, filterInfoParam.Data, filterInfo.FilterInfo_ExtraData );
	}

	void ToIpFilterInfo( const Kxe_IPFilterInfo_Param& infoParam, KIP_FILTER_INFO& info )
	{
		ToGUID( &(infoParam.guid), &(info.id) );
		ToFilterInfo( infoParam.filterInfo, info.filter );
	}

	void FromIpFilterInfo( Kxe_IPFilterInfo_Param& infoParam, const KIP_FILTER_INFO& info )
	{
		FromGUID( &(infoParam.guid), &(info.id) );
		FromFilterInfo( infoParam.filterInfo, info.filter );
	}


	void ToAttackInfo( const Kxe_AttackInfo_Param& attackInfoParam, AttackInfo& attackInfo )
	{
		attackInfo.attackAddr = attackInfoParam.attackAddr;
		ToGUID( &(attackInfoParam.nReason), &(attackInfo.nReason) );
		attackInfo.nAttackTimes = attackInfoParam.nAttackTimes;
		attackInfo.nTime = attackInfoParam.nTime;
	}

	void FromAttackInfo( Kxe_AttackInfo_Param& attackInfoParam, const AttackInfo& attackInfo )
	{
		attackInfoParam.attackAddr		= attackInfo.attackAddr;
		FromGUID( &(attackInfoParam.nReason), &(attackInfo.nReason) );
		attackInfoParam.nAttackTimes	= attackInfo.nAttackTimes;
		attackInfoParam.nTime			= attackInfo.nTime;
	}

	void ToAttackList( const Kxe_AttackList_Ret& attackListParam, vector<AttackInfo>& vecAttack )
	{
		vecAttack.clear();

		for ( int i = 0; i < (int)attackListParam.attackList.size(); i ++ )
		{
			AttackInfo attackInfo;
			ToAttackInfo( attackListParam.attackList[i], attackInfo );
			vecAttack.push_back( attackInfo );
		}
	}

	void FromAttackList( Kxe_AttackList_Ret& attackListParam, const vector<AttackInfo>& vecAttack )
	{
		attackListParam.attackList.clear();

		for ( int i = 0; i < (int)vecAttack.size(); i++ )
		{
			Kxe_AttackInfo_Param attackInfo;
			FromAttackInfo( attackInfo, vecAttack[i] );
			attackListParam.attackList.push_back( attackInfo );
		}
	}

	void ToMacIp( const Kxe_MacIp_Param& macipParam, mac_ip& macip )
	{
		ToMAC( &(macipParam.mac), macip.mac );
		macip.ip = macipParam.ip;
	}
	
	void FromMacIp( Kxe_MacIp_Param& macipParam, const mac_ip& macip )
	{
		FromMAC( &(macipParam.mac), macip.mac );
		macipParam.ip = macip.ip;
	}


	void ToMacIp( const Kxe_MacIp_Ret& macipParam, mac_ip& macip )
	{
		ToMAC( &(macipParam.macIp.mac), macip.mac );
		macip.ip = macipParam.macIp.ip;
	}

	void FromMacIp( Kxe_MacIp_Ret& macipParam, const mac_ip& macip )
	{
		FromMAC( &(macipParam.macIp.mac), macip.mac );
		macipParam.macIp.ip = macip.ip;
	}


	void ToArpHostInfo( const Kxe_ArpHostInfo_Ret& infoParam, KArpHostInfo& info )
	{
		info.m_nHostIpCnt = infoParam.hostInfo.nHostIpCnt;

		for( int i = 0; i < (int)infoParam.hostInfo.vecHostMacIp.size() && i < max_hostip_count; i++ )
		{
			ToMacIp( infoParam.hostInfo.vecHostMacIp[i], info.m_HostIp[i] );
		}

		info.m_nGateWayCnt = infoParam.hostInfo.nGateWayCnt;

		for( int i = 0; i < (int)infoParam.hostInfo.vecGetWay.size() && i < max_hostip_count; i++ )
		{
			ToMacIp( infoParam.hostInfo.vecGetWay[i], info.m_GateWay[i] );
		}
	}

	void FromArpHostInfo( Kxe_ArpHostInfo_Ret& infoParam, const KArpHostInfo& info )
	{
		infoParam.hostInfo.nHostIpCnt = info.m_nHostIpCnt;

		for( int i = 0; i < infoParam.hostInfo.nHostIpCnt && i < max_hostip_count; i++ )
		{
			Kxe_MacIp_Param macIp;
			FromMacIp( macIp, info.m_HostIp[i] );
			infoParam.hostInfo.vecHostMacIp.push_back( macIp );
		}

		infoParam.hostInfo.nGateWayCnt = info.m_nGateWayCnt;

		for( int i = 0; i < infoParam.hostInfo.nGateWayCnt && i < max_hostip_count; i++ )
		{
			Kxe_MacIp_Param macIp;
			FromMacIp( macIp, info.m_GateWay[i] );
			infoParam.hostInfo.vecGetWay.push_back( macIp );
		}
	}
}// Dpt_hlp