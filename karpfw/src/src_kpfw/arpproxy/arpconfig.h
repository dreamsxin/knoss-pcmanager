#pragma once
#include "arpcommondef.h"

class ArpConfig
{
public:
	ArpConfig(void);
	~ArpConfig(void);

	BOOL GetArpEnable();
	BOOL SetArpEnable(BOOL bValue);

	BOOL GetArpSafe();
	BOOL SetArpSafe(BOOL bSafe);

	BOOL GetArpSetting(BOOL& bBlockRecvAttack, BOOL& bBlockRecvIPAttack, BOOL& bBlockSendAttack);
	BOOL SetArpSetting(BOOL bBlockRecvAttack, BOOL bBlockRecvIPAttack, BOOL bBlockSendAttack);

	BOOL GetManualGateAddr(mac_ip*& pAddr, INT& nCnt);
	BOOL SetManualGateAddr(mac_ip* pAddr, INT nCnt);

	BOOL GetNotifyGateway(BOOL& bNotify, INT& nTimes );
	BOOL SetNotifyGateway(BOOL bNotify, INT nTimes);

	BOOL GetUseAutoGateAddr();
	BOOL SetUseAutoGateAddr(BOOL bEnable);

	BOOL ClearArpProtectList();

	BOOL Load();
	BOOL Save();

	BOOL m_nVer;

    // 是否启用
	BOOL m_bEnable;

    // 安全模式
	BOOL m_bSafeMode;

    // 阻止对内的ARP攻击
    BOOL m_bBlockRecvAttack;
    // 阻止对内的IP冲突
    BOOL m_bBlockRecvIPAttack;
    // 阻止对外的ARP攻击
    BOOL m_bBlockSendAttack;

    // 通知网关
	BOOL m_bNotifyGw;
	BOOL m_bAutoGate;
	INT m_nTimes;
	list<mac_ip> m_pMacIpList;
};
