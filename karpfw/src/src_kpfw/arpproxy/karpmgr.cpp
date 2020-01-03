#include "StdAfx.h"
#include "karpmgr.h"

extern "C"
{
DWORD
WINAPI
GetIpNetTable(
	OUT    PMIB_IPNETTABLE pIpNetTable,
	IN OUT PULONG          pdwSize,
	IN     BOOL            bOrder
	);

DWORD
WINAPI
DeleteIpNetEntry(
	IN PMIB_IPNETROW    pArpEntry
	);

DWORD
WINAPI
SendARP(
	IPAddr DestIP,
	IPAddr SrcIP,
	PULONG pMacAddr,
	PULONG  PhyAddrLen
	);
}

#define		MAX_IPADDR		30
#define		KPFW_ARP_NOTIFYGATEWAY_SEC_CNT			5

KArpInfo::KArpInfo()
{
	m_arpDevc = NULL;
	m_pAdaterInfos = new ADAPTER_INFO[20];
	ZeroMemory( (char*)m_pAdaterInfos, sizeof(ADAPTER_INFO)*20 );
}

KArpInfo::~KArpInfo()
{
	delete []m_pAdaterInfos;
	m_pAdaterInfos = NULL;
}

STDMETHODIMP KArpInfo::Initialize()
{
	HRESULT hResult = E_FAIL;

	GetNetConfig()->Load();
	m_txtLog.Init();

	if (GetAntiArpDevC())
	{
		if( GetAntiArpDevC()->InitDevC() )
		{
			GetAntiArpDevC()->SetCallback( this );

			m_uAdapterCount = GetAntiArpDevC()->GetAdapter( m_pAdaterInfos, 20 );

			UpdateArpConfig();
			hResult = S_OK;
		}
	}

	return hResult;
}

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP KArpInfo::UnInitialize()
{
	if (GetAntiArpDevC())
	{
		GetAntiArpDevC()->SetModuleFlag(0);
		GetAntiArpDevC()->UninitDevC();
		delete m_arpDevc;
		m_arpDevc = NULL;
	}
	m_txtLog.Unint();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////

void KArpInfo::DeleteAllArpEntry()
{
	DWORD nSize = sizeof(MIB_IPNETTABLE );
	PMIB_IPNETTABLE  table0 = (PMIB_IPNETTABLE)new BYTE[nSize];

	if (::GetIpNetTable(table0, &nSize, FALSE) == ERROR_INSUFFICIENT_BUFFER)
	{
		delete[] table0;
		table0 = (PMIB_IPNETTABLE)new BYTE[nSize];
		if (::GetIpNetTable(table0, &nSize, FALSE) == NO_ERROR)
		{
			for (ULONG i = 0; i < table0->dwNumEntries; i++)
			{
				if (::DeleteIpNetEntry(&table0->table[i]) != NO_ERROR)
				{
					in_addr addr;
					addr.S_un.S_addr = table0->table[i].dwAddr;
				}
			}
		}
		delete [] table0;
	}
}


BOOL KArpInfo::UpdateArpConfig()
{
	// 填充各种配置
	BOOL bSafe		= FALSE;
	BOOL bBlockRecvAttack = TRUE;
    BOOL bBlockRecvIPAttack = TRUE;
    BOOL bBlockSendAttack = TRUE;
	BOOL bNotifyGateway = FALSE;
	INT nTimes = 0;

	StopArp();

	if (!GetNetConfig()->GetArpEnable())
	{
		return FALSE;
	}

	DeleteAllArpEntry();

	if ( GetNetConfig() )
	{
		bSafe = GetNetConfig()->GetArpSafe();
		GetNetConfig()->GetArpSetting(bBlockRecvAttack, bBlockRecvIPAttack, bBlockSendAttack);
		GetNetConfig()->GetNotifyGateway(bNotifyGateway, nTimes);
	}

	ULONG uArpBlockFlag = 0;
	uArpBlockFlag |= bSafe ? enumSafe_Mode : 0;
	uArpBlockFlag |= bBlockRecvAttack ? enumRecv_Attack : 0;
	uArpBlockFlag |= bBlockRecvIPAttack ? enumRecv_IP_Attack : 0;
    uArpBlockFlag |= bBlockSendAttack ? enumSend_Attack : 0;
	uArpBlockFlag |= bNotifyGateway ? enumNotify_Gateway : 0;

	if (GetAntiArpDevC())
	{
        if (bNotifyGateway)
            GetAntiArpDevC()->SetNotifyTimer(1000 / nTimes);

		GetAntiArpDevC()->SetArpBlockFlag(uArpBlockFlag);
	}

	SetAntiArpProtectList();	

	StartArp();

	return TRUE;
}

BOOL KArpInfo::SetAntiArpProtectList()
{
	// todo: 设置要保护的地址列表	

	if (!GetNetConfig()->GetUseAutoGateAddr())
	{
		// 手动设置网关地址
		mac_ip*		pAddr = NULL;
		INT			nCnt = 0;
		GetNetConfig()->GetManualGateAddr(pAddr, nCnt);

		if (nCnt <= 0)
		{
			GetAntiArpDevC()->ClearArpProtectList();
			return FALSE;
		}

		GetAntiArpDevC()->SetArpProtectList(pAddr, nCnt);
        delete[] pAddr;
        pAddr = NULL;
	}

 	else
 	{    
         mac_ip* pAddr = NULL;
         INT			nCnt = 0;
         KArpHostInfo arphost;
 
         RtlZeroMemory(&arphost, sizeof(KArpHostInfo));
 
         Refresh();   
         GetHostInfo(&arphost);      
         nCnt = arphost.m_nGateWayCnt;
         if (nCnt)
         {
             pAddr = new mac_ip[nCnt];
             if(NULL == pAddr)
                 return FALSE;
         }
         for (int i = 0; i< nCnt; ++i)
         {
             pAddr[i] = arphost.m_GateWay[i]; 
         }
                     
         GetAntiArpDevC()->SetArpProtectList(pAddr, nCnt);    
 
         delete[] pAddr;
         pAddr = NULL;
//         IAntiArpDevC* piDev = GetAntiArpDevC();
//         if (piDev)
//         {
//             ADAPTER_INFO adapterInfo[10];
//             ULONG pulMac[2];
//             int nResult;
// 
//             piDev->CleanGatewayMac();           // 清除网关MAC
//             memset( (char*)&adapterInfo, 0, sizeof(adapterInfo) );
//             nResult = piDev->GetAdapter(adapterInfo, 10);
//             for (int i = 0; i < 10; i++)
//             {
//                 DelArpCache();                  // arp -d
//                 for( int i=0; i<nResult; i++ )
//                 {
//                     // send arp
//                     ULONG ulLen = 6;
//                     SendARP(adapterInfo[i].uGatewagIP, 0, pulMac, &ulLen);			
//                 }
//                 ::Sleep(100);
//             }
//         }
// 
//             return piDev->SelectGatewayMac();
     }

	return TRUE;
}


HRESULT KArpInfo::GetMacFromIp (mac_ip* pAddr)
{
	return IpToMac(pAddr);
}

BOOL KArpInfo::IpToMac(mac_ip* pAddr, long hostip, int nTryCount)
{
	for (int i = 0 ; i < nTryCount; i++)
	{
		ULONG nLen = 6;
		HRESULT hr = ::SendARP(pAddr->ip, hostip, (PULONG)&pAddr->mac[0], &nLen);
		if (hr == NOERROR)
			return TRUE;
	}
	return FALSE;
}

// arp 高级设置
HRESULT KArpInfo::GetNotifyGateway(BOOL& bNotifyGw, INT& nTimes)
{
	if ( !GetNetConfig() )
		return E_FAIL;

	return GetNetConfig()->GetNotifyGateway(bNotifyGw, nTimes);
}

HRESULT KArpInfo::SetNotifyGateway(BOOL bNotifyGw, INT nTimes)
{
	if ( !GetNetConfig() )
		return E_FAIL;

	return GetNetConfig()->SetNotifyGateway( bNotifyGw, nTimes );
}


HRESULT KArpInfo::GetAdvSetting(BOOL& bSafeMode, BOOL& bBlockRecvAttack, BOOL& bBlockRecvIPAttack, BOOL& bBlockSendAttack)
{
	bSafeMode = GetNetConfig()->GetArpSafe();
	GetNetConfig()->GetArpSetting(bBlockRecvAttack, bBlockRecvIPAttack, bBlockSendAttack);
	return S_OK;
}
HRESULT KArpInfo::SetAdvSetting(BOOL bSafeMode, BOOL bBlockRecvAttack, BOOL bBlockRecvIPAttack, BOOL bBlockSendAttack)
{
	GetNetConfig()->SetArpSafe(bSafeMode);
	GetNetConfig()->SetArpSetting(bBlockRecvAttack, bBlockRecvIPAttack, bBlockSendAttack);
	return S_OK;
}

void KArpInfo::StopArp()
{
	ARP_STATE state = {0};
	if (GetAntiArpDevC()->GetKfwState(&state))
	{
		state.uEnableFlag &= ~ENABLE_ARP;
		state.uRequestFlag &= ~REQUEST_ARP_LOG;
		GetAntiArpDevC()->SetModuleFlag(state.uEnableFlag);
		GetAntiArpDevC()->SetLogFlag(state.uRequestFlag);
		GetAntiArpDevC()->SetArpBlockFlag(0);
	}
}

BOOL KArpInfo::StartArp()
{
	ARP_STATE state = {0};
	if ( GetNetConfig()->GetArpEnable())
	{
		if (GetAntiArpDevC()->GetKfwState(&state))
		{
			state.uEnableFlag |= ENABLE_ARP;
			state.uRequestFlag |= REQUEST_ARP_LOG;
			GetAntiArpDevC()->SetModuleFlag(state.uEnableFlag);
			GetAntiArpDevC()->SetLogFlag(state.uRequestFlag);
			return TRUE;
		}
	}
	return FALSE;
}

void KArpInfo::IpLog( PLOG_IP_RULE pIpRule )
{
	if (!pIpRule)
		return;

	if (pIpRule->id == ArpFilterGuid)
	{
		m_txtLog.AddLog( pIpRule );
	}
}

void KArpInfo::GetLastLog(std::string& strLog)
{
    m_txtLog.GetLastLog(strLog);
}

BOOL KArpInfo::ApplySetting()
{
	// 填充各种配置
	BOOL bSafeMode		= FALSE;
	BOOL bNotifyGateway = FALSE;
    BOOL bBlockRecvAttack = TRUE;
    BOOL bBlockRecvIPAttack = TRUE;
    BOOL bBlockSendAttack = TRUE;
	INT nTimes = 0;

	DeleteAllArpEntry();

	if ( GetNetConfig() )
	{
		bSafeMode = GetNetConfig()->GetArpSafe();
		GetNetConfig()->GetArpSetting(bBlockRecvAttack, bBlockRecvIPAttack, bBlockSendAttack);
		GetNetConfig()->GetNotifyGateway(bNotifyGateway, nTimes);
	}
	else
	{
		return FALSE;
	}

	ULONG uArpBlockFlag = 0;
	uArpBlockFlag |= bSafeMode ? enumSafe_Mode : 0;
	uArpBlockFlag |= bBlockRecvAttack ? enumRecv_Attack : 0;
	uArpBlockFlag |= bBlockRecvIPAttack ? enumRecv_IP_Attack : 0;
    uArpBlockFlag |= bBlockSendAttack ? enumSend_Attack : 0;
	uArpBlockFlag |= bNotifyGateway ? enumNotify_Gateway : 0;

	if (GetAntiArpDevC())
	{
        if (bNotifyGateway)
            GetAntiArpDevC()->SetNotifyTimer(1000 / nTimes);

		GetAntiArpDevC()->SetArpBlockFlag(uArpBlockFlag);
	}

	SetAntiArpProtectList();

    if (GetNetConfig()->GetArpEnable())
    {
        StartArp();
    }
    else
    {
        StopArp();
    }

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

IAntiArpDevC* KArpInfo::GetAntiArpDevC()
{
	if( m_arpDevc == NULL )
	{
		m_arpDevc = new KDllComObject<KASAntiArpDevC>();
	}

	return m_arpDevc;
}

//////////////////////////////////////////////////////////////////////////

ArpConfig* KArpInfo::GetNetConfig()
{
	return &m_arpConfig;
}

//////////////////////////////////////////////////////////////////////////

HRESULT KArpInfo::GetHostInfo (KArpHostInfo* pArpHostInfo)
{
	HRESULT hResult = E_FAIL;

    pArpHostInfo->m_nHostIpCnt = 0;
    pArpHostInfo->m_nGateWayCnt = 0;

	for( ULONG i=0; i<m_uAdapterCount; i++ )
	{
        
		if( m_pAdaterInfos[i].uLocalIP != 0 )
		{
			pArpHostInfo->m_HostIp[pArpHostInfo->m_nHostIpCnt].ip = m_pAdaterInfos[i].uLocalIP[0];
			memcpy( pArpHostInfo->m_HostIp[pArpHostInfo->m_nHostIpCnt].mac, m_pAdaterInfos[i].uLocalMac, 6 );
			pArpHostInfo->m_nHostIpCnt++;
		}

		if( m_pAdaterInfos[i].uGatewagIP != 0 )
		{
            mac_ip ip = { 0 };
			pArpHostInfo->m_GateWay[pArpHostInfo->m_nGateWayCnt].ip = m_pAdaterInfos[i].uGatewagIP;
            ip.ip = m_pAdaterInfos[i].uGatewagIP;
            GetMacFromIp(&ip);
			memcpy( pArpHostInfo->m_GateWay[pArpHostInfo->m_nGateWayCnt].mac, ip.mac, 6 );
			pArpHostInfo->m_nGateWayCnt++;
		}
	}
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////

HRESULT KArpInfo::GetEnable (BOOL* pValue)
{
	if ( GetNetConfig() && pValue )
	{
		*pValue = GetNetConfig()->GetArpEnable();
		return S_OK;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////

HRESULT KArpInfo::SetEnable (BOOL bValue)
{
	if (GetNetConfig())
	{
		GetNetConfig()->SetArpEnable(bValue);
        if (bValue)
        {
            StartArp();
        }
        else
        {
            StopArp();
        }
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////

HRESULT KArpInfo::GetAutoGateWay (BOOL* pAuto, vector<mac_ip>* pGateAddr)
{
	if (GetNetConfig() && pAuto && pGateAddr)
	{
		*pAuto = GetNetConfig()->GetUseAutoGateAddr();
		if (!(*pAuto))
		{
			mac_ip* pAddr = NULL;
			INT nCnt = 0;
			GetNetConfig()->GetManualGateAddr(pAddr, nCnt);
			pGateAddr->clear();
			for (int i = 0; i < nCnt; i++)
				pGateAddr->push_back(pAddr[i]);
		}
		return S_OK;
	}
	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////////

HRESULT KArpInfo::SetAutoGateWay (BOOL bAuto,  vector<mac_ip>* pGateAddr)
{
	if (GetNetConfig())
	{
		GetNetConfig()->SetUseAutoGateAddr(bAuto);
		if (pGateAddr && !bAuto && (pGateAddr->size() > 0))
			GetNetConfig()->SetManualGateAddr(&((*pGateAddr)[0]), (int)pGateAddr->size());
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////

HRESULT KArpInfo::Ping(ATL::CString addr)
{
	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////////

HRESULT	KArpInfo::Refresh()
{
	m_uAdapterCount = GetAntiArpDevC()->GetAdapter( m_pAdaterInfos, 20 );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////

void KArpInfo::DelArpCache()
{
	ULONG dwSize = 0;
	PMIB_IPNETTABLE pIpNetTable = NULL;
	if( ERROR_INSUFFICIENT_BUFFER == GetIpNetTable( pIpNetTable, &dwSize, TRUE ) )
	{
		pIpNetTable = (PMIB_IPNETTABLE)new char[dwSize];
		if( NO_ERROR == GetIpNetTable( pIpNetTable, &dwSize, TRUE ) )
		{
			for( DWORD i=0; i<pIpNetTable->dwNumEntries; i++ )
			{
				DeleteIpNetEntry( &pIpNetTable->table[i] );
			}
		}
	}
	if( pIpNetTable != NULL )
	{
		delete []((char*)pIpNetTable);
		pIpNetTable = NULL;
	}
}

void KArpInfo::ClearProtectList()
{
    IAntiArpDevC*  pAntiArpDevC = GetAntiArpDevC();
    if (pAntiArpDevC)
    {
        pAntiArpDevC->ClearArpProtectList();
    }
}

//////////////////////////////////////////////////////////////////////////
