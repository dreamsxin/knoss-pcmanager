////////////////////////////////////////////////////////////////////////////////
//      
//      common include File for kpfw
//      
//      File      : kpfw_dpt_hlp.h
//      Version   : 1.0
//      Comment   : 定义网镖服务和界面之间的转换辅助类
//      
//      Create at : 2009-07-15
//      Create by : chenguicheng
//      
////////////////////////////////////////////////////////////////////////////////

#include "kpfw/kpfw_dpt_def.h"
#include "arpcommondef.h"

namespace Dpt_hlp
{

	void ToGUID( const Kxe_GUID_Param* guidParam, GUID* guid );
	void FromGUID( Kxe_GUID_Param* pGuiParam, const GUID* pGuid );

	void ToIpFilterAddr( const Kxe_FilterIPAddr_Param& filterParam, FILTER_IP_ADDR& filter );
	void FromFilterAddr( Kxe_FilterIPAddr_Param& filterParam, const FILTER_IP_ADDR& filter );

	void ToFilterPort( const Kxe_FilterPort_Param& filerParam, FILTER_PORT& filter );
	void FromFilterPort( Kxe_FilterPort_Param& filerParam, const FILTER_PORT& filter );

	void ToTCP_DATA( const Kxe_TCPData_Param& tcpParam, TCP_EXTRA_DATA& tcp);
	void FromTCP_DATA( Kxe_TCPData_Param& tcpParam, const TCP_EXTRA_DATA& tcp);

	void ToUDP_DATA( const Kxe_UDPData_Param& udpParam, UDP_EXTRA_DATA& udp );
	void FromUDP_DATA( Kxe_UDPData_Param& udpParam, const UDP_EXTRA_DATA& udp );

	void ToICMP_DATA( const Kxe_ICMPData_Param& icmpParam, ICMP_EXTRA_DATA& icmp );
	void FromICMP_DATA( Kxe_ICMPData_Param& icmpParam, const ICMP_EXTRA_DATA& icmp );

	void ToIGMP_DATA( const Kxe_IGMPData_Param& igmpParam, IGMP_EXTRA_DATA& igmp );
	void FromIGMP_DATA( Kxe_IGMPData_Param& igmpParam, const IGMP_EXTRA_DATA& igmp );

	void ToDataUnion( const DataUnion& dataUnion, FILTERINFO_EXTRADATA& data );
	void FromDataUnion( PROTO_TYPE Protoc, DataUnion& dataUnion, const FILTERINFO_EXTRADATA& data );

	void ToFilterInfo( const Kxe_FilterInfo_Param& filterInfoParam, FILTER_INFO& filterInfo );
	void FromFilterInfo( Kxe_FilterInfo_Param& filterInfoParam, const FILTER_INFO& filterInfo );

	void ToIpFilterInfo( const Kxe_IPFilterInfo_Param& infoParam, KIP_FILTER_INFO& info );
	void FromIpFilterInfo( Kxe_IPFilterInfo_Param& infoParam, const KIP_FILTER_INFO& info );

	void ToMAC( const Kxe_MAC_Param* macParam, BYTE mac[6] );
	void FromMAC( Kxe_MAC_Param* macParam, const BYTE mac[6] );

	void ToAttackInfo( const Kxe_AttackInfo_Param& attackInfoParam, AttackInfo& attackInfo );
	void FromAttackInfo( Kxe_AttackInfo_Param& attackInfoParam, const AttackInfo& attackInfo );

	void FromAttackList( Kxe_AttackList_Ret& attackListParam, const vector<AttackInfo>& vecAttack );
	void ToAttackList( const Kxe_AttackList_Ret& attackListParam, vector<AttackInfo>& vecAttack );

	void FromMd5( Kxe_MD5_Param* md5Param, const BYTE md5[16] );
	void ToMd5( const Kxe_MD5_Param* md5Param, BYTE md5[16] );

	void ToMacIp( const Kxe_MacIp_Param& macipParam, mac_ip& macip );
	void FromMacIp( Kxe_MacIp_Param& macipParam, const mac_ip& macip );

	void ToMacIp( const Kxe_MacIp_Ret& macipParam, mac_ip& macip );
	void FromMacIp( Kxe_MacIp_Ret& macipParam, const mac_ip& macip );

	void FromArpHostInfo( Kxe_ArpHostInfo_Ret& infoParam, const KArpHostInfo& info );
	void ToArpHostInfo( const Kxe_ArpHostInfo_Ret& infoParam, KArpHostInfo& info );

}// Dpt_hlp
