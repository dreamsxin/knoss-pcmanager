#include "pch.h"
#include "karpfilter.h"
#include "kndisfirewall.h"
#include "karpfilter.tmh"

#pragma warning( push ) 
#pragma warning( disable : 4312 )

BOOLEAN KArpFilter::Init( PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath )
{
	m_uBlockFlag = 0;
	m_uSendArpTimer = 2000;
	m_threadSendArp.Init( (PVOID)ThreadSendArp, (PVOID)this );
	m_ProtectList._Init();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KArpFilter::Uninit( PDRIVER_OBJECT DriverObject )
{
	ClearBindAddr();
	m_threadSendArp.Uninit();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS KArpFilter::BindAddr( PVOID pListBuffer, ULONG uBufferLength )
{
	NTSTATUS ntStatus = STATUS_INVALID_PARAMETER;
	p_mac_ip pList = (p_mac_ip)pListBuffer;
	ULONG uItemCount = uBufferLength / sizeof(mac_ip);

	do 
	{
		if(  pListBuffer == NULL || uItemCount == 0  )
			break;
		
		if( (uBufferLength % sizeof(mac_ip)) != 0 )
			break;
		
		m_ProtectList.clear();
		
		for( ULONG i=0; i<uItemCount; i++ )
		{
			__int64 Mac = 0;
			RtlCopyMemory( (UCHAR*)&Mac, pList[i].mac, 6 );
			m_ProtectList.insert( (PVOID)pList[i].ip, Mac );
		}
		
		ntStatus = STATUS_SUCCESS;
	} while (FALSE);
		
	return ntStatus;
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS KArpFilter::ClearBindAddr()
{
	m_ProtectList.clear();
	return STATUS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////

IPRESULT KArpFilter::Filter( PKANTIARP_ADAPTER adapter, PVOID pBuffer, ULONG uBufferLength, PACKET_DIR enumDir )
{
	PacketAttackType blockReason = pa_none;
	IPRESULT bResult = enumIP_Unknow;
	PARPPACKET pArpPack = (PARPPACKET)pBuffer;	
	ULONG uGatewayIP = 0;
	ULONG uLocalIP = 0;

	do 
	{
		if( adapter->Medium != NdisMedium802_3 )
			break;

		if( uBufferLength < sizeof(ARPPACKET) )
			break;

		if( pArpPack->ehhdr.eh_type != ETHER_ARP )
			break;

		if( pArpPack->arphdr.arp_hln != 6			
			|| pArpPack->arphdr.arp_pln != 4 )	//ipv4
			break;
		
		//智能绑定网关逻辑
		if( !adapter->bManualGatewayMac )
		{
			if( enumDir == enumPD_Send )
			{
				if( IsRequestGatewayMac( adapter, pArpPack ) )
				{
					adapter->bRequestGatewayMac = TRUE;
					g_ndisFirewall->DecRecvArp( adapter );
				}
			}
			
			if( enumDir == enumPD_Recieve )
			{
				//从没请求过网卡
				if( adapter->bRequestGatewayMac )
				{
					g_ndisFirewall->IncRecvArp( 
						adapter, 
						(ULONG)pArpPack->arphdr.arp_spa,
						(UCHAR*)pArpPack->arphdr.arp_sha );
				}
			}	
		}


		if( !g_ndisFirewall->IsModuleEnable( ENABLE_ARP ) )
		{
			bResult = enumIP_Pass;
			break;
		}

		if( enumDir == enumPD_Send )
		{
			bResult = FilterSend( adapter, pArpPack, &blockReason );
		}
		else
		{
			bResult = FilterRecv( adapter, pArpPack, &blockReason );
		}
		
		if( (bResult == enumIP_Block) && ( blockReason != pa_none ) )
		{
			g_ndisFirewall->SendArpLog( adapter, pArpPack, blockReason, enumDir );

			KAntiarpDebugPrint( TRACE_LEVEL_INFORMATION, FLAG_NDIS_ARP, 
				"KArpFilter::Filter Block( %d.%d.%d.%d, %02X-%02X-%02X-%02X-%02X-%02X ) ( %d.%d.%d.%d, %02X-%02X-%02X-%02X-%02X-%02X )", 
				((UCHAR*)&pArpPack->arphdr.arp_spa)[0], 
				((UCHAR*)&pArpPack->arphdr.arp_spa)[1], 
				((UCHAR*)&pArpPack->arphdr.arp_spa)[2], 
				((UCHAR*)&pArpPack->arphdr.arp_spa)[3], 
				pArpPack->arphdr.arp_sha[0],
				pArpPack->arphdr.arp_sha[1],
				pArpPack->arphdr.arp_sha[2],
				pArpPack->arphdr.arp_sha[3],
				pArpPack->arphdr.arp_sha[4],
				pArpPack->arphdr.arp_sha[5],

				((UCHAR*)&pArpPack->arphdr.arp_tpa)[0], 
				((UCHAR*)&pArpPack->arphdr.arp_tpa)[1], 
				((UCHAR*)&pArpPack->arphdr.arp_tpa)[2], 
				((UCHAR*)&pArpPack->arphdr.arp_tpa)[3], 
				pArpPack->arphdr.arp_tha[0],
				pArpPack->arphdr.arp_tha[1],
				pArpPack->arphdr.arp_tha[2],
				pArpPack->arphdr.arp_tha[3],
				pArpPack->arphdr.arp_tha[4],
				pArpPack->arphdr.arp_tha[5]
				);
		}

	} while (FALSE);

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

IPRESULT KArpFilter::FilterSend( PKANTIARP_ADAPTER adapter, PARPPACKET pArpPack, PacketAttackType *blockReason )
{
	IPRESULT bResult = enumIP_Pass;

	do 
	{
		if( IsGratuitousArp( pArpPack, adapter->LocalMac ) )
		{
			if( adapter->LocalIP[0] == 0 && adapter->GatewayIP == 0 )
			{
				g_ndisFirewall->SetAddrEvent();
			}
			break;
		}


		if( !(m_uBlockFlag & enumSend_Attack ) )
			break;

		//发送攻击包: Sender_IP != 本地网卡IP  && Sender_MAC != 本地网卡MAC
		//攻击手段一: 将 Sender_IP 设为 网关IP , Sender_MAC 设为 本地网卡MAC. 目标机器收到该包后，不能连接网关
		//攻击手段二: 将 Sender_IP 设为 对方IP , Sender_MAC 设为 任意. 目标机器收到该包后，导致ＩＰ冲突		

		//通过该网卡发送的信息，其MAC 和 IP 必需相同
		if( !g_ndisFirewall->CompMacIp( adapter, (ULONG)pArpPack->arphdr.arp_spa, (UCHAR*)pArpPack->arphdr.arp_sha ) )
		{
			*blockReason = pa_arp_send_attack;
			bResult = enumIP_Block;
			break;
		}

		//发送到网关
		bResult = g_ndisFirewall->IsGateway( adapter, (ULONG)pArpPack->arphdr.arp_tpa, pArpPack->arphdr.arp_tha );
		if( bResult == enumIP_Block || bResult == enumIP_Pass )
		{
			*blockReason = pa_arp_send_fake_gate_mac;
			break;
		}

		//发送到已经绑定的地址
		//TODO:是否多个网卡共用同一个绑定列表
		bResult = IsBindAddr( (ULONG)pArpPack->arphdr.arp_tpa, pArpPack->arphdr.arp_tha );
		if( bResult == enumIP_Block || bResult == enumIP_Pass )
		{
			*blockReason = pa_arp_send_fake_bindmac;
			break;
		}
		
		//安全模式下，ARP包只允许发送给 绑定MAC的机器 和 网关
		if(  m_uBlockFlag & enumSafe_Mode )
		{
			*blockReason = pa_arp_send_unexcept_arp_on_safe_mode;
			bResult = enumIP_Block;
			break;
		}
		
		//其它情况记录在等待回复队列中	

	} while (FALSE);

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

IPRESULT KArpFilter::FilterRecv( PKANTIARP_ADAPTER adapter, PARPPACKET pArpPack, PacketAttackType *blockReason )
{
	IPRESULT bResult = enumIP_Pass;
	
	do 
	{

		//接收到攻击包
		//攻击手段一: 将 Sender_IP 设为 网关IP , Sender_MAC 设为 攻击者MAC. 收到该包后，不能连接网关
		//攻击手段二: 将 Sender_IP 为本地网卡IP , Sender_MAC 不等于本地网卡. 收到该包后，导致ＩＰ冲突
		if( m_uBlockFlag & enumRecv_IP_Attack )
		{
			//IP冲突：即收到IP地址相同，但MAC不同的ARP包
			if( !g_ndisFirewall->CompIpMac( adapter, (ULONG)pArpPack->arphdr.arp_spa, (UCHAR*)pArpPack->arphdr.arp_sha ) )
			{
				*blockReason = pa_arp_recv_ip_conflict;
				bResult = enumIP_Block;
				break;
			}
		}

		////丢弃不是请求本机IP的包, 局域网上这样的包比较多，所以不提示日志
		//if( adapter->LocalIP != (ULONG)pArpPack->arphdr.arp_tpa )
		//{
		//	bResult = enumIP_Block;
		//	*blockReason = enum_None;
		//	break;
		//}
		
		if( !(m_uBlockFlag & (enumRecv_Attack | enumSafe_Mode ) ) )
			break;

		//来自网关
		bResult = g_ndisFirewall->IsGateway( adapter, (ULONG)pArpPack->arphdr.arp_spa, (UCHAR*)pArpPack->arphdr.arp_sha );
		if( bResult == enumIP_Block || bResult == enumIP_Pass )
		{
			*blockReason = pa_arp_recv_fake_gate_mac;
			break;
		}

		//来自绑定地址
		//TODO:是否多个网卡共用同一个绑定列表
		bResult = IsBindAddr( (ULONG)pArpPack->arphdr.arp_spa, (UCHAR*)pArpPack->arphdr.arp_sha );
		if( bResult == enumIP_Block || bResult == enumIP_Pass )
		{
			*blockReason = pa_arp_recv_fake_bindmac;
			break;
		}

		if( m_uBlockFlag & enumSafe_Mode )
		{
			*blockReason = pa_arp_recv_unexcept_arp_on_safe_mode;
			bResult = enumIP_Block;
			break;
		}
		
		//查询等待回复队列

	} while (FALSE);

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

IPRESULT KArpFilter::IsBindAddr( ULONG uIP, UCHAR* pMac )
{
	__int64 mac = 0;

	if( m_ProtectList.find_lock( (PVOID)uIP, &mac ) )
	{
		//请求绑定列表中MAC
		if( RtlCompareMemory( pMac, "\0\0\0\0\0\0\0", 6 ) == 6 ) 
			return enumIP_Pass;

		if( RtlCompareMemory( (UCHAR*)&mac, pMac, 6 ) == 6 )
			return enumIP_Pass;
		
		//和列表中设定的不同
		return enumIP_Block;
	}

	return enumIP_Unknow;
}

//////////////////////////////////////////////////////////////////////////

ULONG KArpFilter::GetBlockFlag()
{
	return m_uBlockFlag;
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS KArpFilter::SetBlockAttack( PVOID pInBuffer, ULONG uBufferLength  )
{
	NTSTATUS ntStatus = STATUS_INVALID_PARAMETER;
	
	if( uBufferLength != sizeof(ULONG) )
		return ntStatus;

	m_uBlockFlag = *((ULONG*)pInBuffer);
	return STATUS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KArpFilter::IsGratuitousArp( PARPPACKET pArpPack, UCHAR *pMac )
{
	BOOLEAN bResult = FALSE;
	if( pArpPack != NULL )
	{
		if( ( pArpPack->arphdr.arp_op == ETHER_ARP_REQ )
				&& ( pArpPack->arphdr.arp_spa == pArpPack->arphdr.arp_tpa )
				&& ( RtlCompareMemory( pArpPack->arphdr.arp_sha, pMac, 6 ) == 6 ) )
		{
			bResult = TRUE;
		}
	}
	return bResult;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KArpFilter::GetProtectMac( ULONG uIP, UCHAR* pMac )
{
	BOOLEAN bResult = FALSE;
	__int64 mac = 0;

	if( pMac == NULL )
		return bResult;

	if( m_ProtectList.find_lock( (PVOID)uIP, &mac ) )
	{
		RtlCopyMemory( pMac, (PVOID)&mac, 6 );
		bResult = TRUE;
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

VOID KArpFilter::ThreadSendArp( PVOID StartContext )
{
	KArpFilter* pThis = (KArpFilter*)StartContext;
	PKANTIARP_ADAPTER pAdapter = NULL;
	int i = 0;
	ARPPACKET *arpPack = (ARPPACKET *)non_paged_pool::__AllocateSystemMemory( sizeof(ARPPACKET) );
	RtlZeroMemory( (char*)arpPack, sizeof(ARPPACKET) );
	
	arpPack->ehhdr.eh_type = ETHER_ARP;
	arpPack->arphdr.arp_hrd = ETHER_NET;
	arpPack->arphdr.arp_pro = ETHER_IP;
	arpPack->arphdr.arp_hln = 6;
	arpPack->arphdr.arp_pln = 4;
	arpPack->arphdr.arp_op = ETHER_ARP_ACK;
	

	while( TRUE )
	{
		if( g_ndisFirewall != NULL
			&& g_ndisFirewall->IsModuleEnable(ENABLE_ARP) 
			&& (pThis->m_uBlockFlag & enumNotify_Gateway) )
		{
			NdisAcquireSpinLock(&g_Data.GlobalLock);

			for( pAdapter = g_Data.AdapterList; pAdapter != NULL; pAdapter = pAdapter->Next )
			{
				PtReferenceAdapt( pAdapter );
				NdisReleaseSpinLock(&g_Data.GlobalLock);

				if( pAdapter->Medium != NdisMedium802_3 || pAdapter->LocalIP[0] == 0 || pAdapter->GatewayIP == 0 )
					goto Next0; 

				if( RtlCompareMemory( pAdapter->GatewayMac, "\0\0\0\0\0\0\0", 6 ) == 6 )
					goto Next0;

				RtlCopyMemory ( arpPack->ehhdr.eh_src, pAdapter->LocalMac, 6 );
				RtlCopyMemory ( arpPack->ehhdr.eh_dst, pAdapter->GatewayMac, 6 );
				RtlCopyMemory ( arpPack->arphdr.arp_sha, pAdapter->LocalMac, 6 );
				RtlCopyMemory ( arpPack->arphdr.arp_tha, pAdapter->GatewayMac, 6 );
				arpPack->arphdr.arp_tpa = pAdapter->GatewayIP;

				for( i=0; i<MAX_ADAPTER_IP_NUM; i++ )
				{
					if( pAdapter->LocalIP[i] == 0 )
						break;
					
					arpPack->arphdr.arp_spa = pAdapter->LocalIP[i];
					g_ndisFirewall->SendUserPacket( pAdapter, (PVOID)arpPack, sizeof(ARPPACKET) );
				}				
Next0:
				NdisAcquireSpinLock(&g_Data.GlobalLock);
				if( PtDereferenceAdapt( pAdapter ) )
					break;
			}

			NdisReleaseSpinLock(&g_Data.GlobalLock);

		}
		
		if( pThis->m_threadSendArp.Wait( max( pThis->m_uSendArpTimer, 50 ) ) )
			break;
	}
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KArpFilter::SetNotifyTimer( LONG uTimer )
{
	if( uTimer == 0 )
		return FALSE;
	
	InterlockedExchange( &m_uSendArpTimer, uTimer );
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KArpFilter::IsRequestGatewayMac( PKANTIARP_ADAPTER adapter, PARPPACKET pArpPack )
{
	const UCHAR szBroadcastAddr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	BOOLEAN bResult = FALSE;

	do 
	{
		//以太头目标MAC == ff ff ff ff ff ff
		if( RtlCompareMemory( pArpPack->ehhdr.eh_dst, szBroadcastAddr, 6 ) != 6 )
			break;

		//opcode == 0x0001
		if( pArpPack->arphdr.arp_op != ETHER_ARP_REQ )
			break;
		
		//发送者MAC==网卡MAC
		if( RtlCompareMemory( pArpPack->arphdr.arp_sha, adapter->LocalMac, 6 ) != 6 )
			break;
		
		//发送者IP==网卡IP
		if( pArpPack->arphdr.arp_spa != adapter->LocalIP[0] &&  pArpPack->arphdr.arp_spa != adapter->LocalIP[1])
			break;
		
		//目标IP == 网关IP
		if( pArpPack->arphdr.arp_tpa != adapter->GatewayIP )
			break;
		
		bResult = TRUE;
	} while (FALSE);

	KAntiarpDebugPrint( TRACE_LEVEL_INFORMATION, FLAG_NDIS_ARP, "KArpFilter::IsRequestGatewayMac:%d", bResult );
	return bResult;
}


#pragma warning( pop ) 