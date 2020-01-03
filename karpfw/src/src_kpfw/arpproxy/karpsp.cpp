#include "StdAfx.h"
#include "karpsp.h"
#include "KASAntiArpDevC.h"
#include "kpfw_dpt_hlp.h"
#include "kxebase_import.inl"


KArpInfo* KxeKpfwArpInfo::GetInterface()
{
	return &m_arpInfo;
}

int KxeKpfwArpInfo::Start()
{ 
	HRESULT hr = m_arpInfo.Initialize(); 

	return RegisterToServer(); 
}

int KxeKpfwArpInfo::Stop()  
{
	if( !m_arpInfo.UnInitialize() ) 
		return -1;
	
	return UnregisterServer();
}

int KxeKpfwArpInfo::GetHostInfo			(IN const Kxe_VOID_Param&,			OUT Kxe_ArpHostInfo_Ret& outParam)
{
	if (GetInterface())
	{
		KArpHostInfo info;
		ZeroMemory( (char*)&info, sizeof(KArpHostInfo) );
		outParam.hr = GetInterface()->GetHostInfo(&info);
		Dpt_hlp::FromArpHostInfo(outParam, info);
	}
	return 0;
}

int KxeKpfwArpInfo::GetHostAdapterInfo	(IN const Kxe_VOID_Param&,			OUT Kxe_KHostAdapterInfo_Ret& outParam) 
{
	//if (GetInterface())
	//{
	//	KHostAdapterInfo info;
	//	outParam.hr = GetInterface()->GetHostAdapterInfo(&info);

	//	outParam.info.reserve(info.GetCount());
	//	for (int i = 0; i < info.GetCount(); i++)
	//	{
	//		KAdapterInfo* pInfo = info.GetInfo(i);
	//		Kxe_KAdapterInfo_Param param;
	//		Dpt_hlp::FromAdapterInfo(param, *pInfo);
	//		outParam.info.push_back(param);
	//	}
	//}
	return 0;
}

int KxeKpfwArpInfo::Ping					(IN const Kxe_String_Param& inParam,		OUT Kxe_HRESULT_Ret& outParam)
{
	if (GetInterface())
	{
		ATL::CString str;
		str = inParam.str.c_str();
		outParam = GetInterface()->Ping(str);
	}
	return 0;
}

// 启用arp防火墙
int KxeKpfwArpInfo::GetEnable				(IN const Kxe_VOID_Param&,			OUT Kxe_INTHResult_Ret& outParam)
{
	if (GetInterface())
	{
		outParam.hr = GetInterface()->GetEnable(&outParam.nRet);
	}
	return 0;
}
int KxeKpfwArpInfo::SetEnable				(IN const Kxe_BOOL_Param& inParam,			OUT Kxe_HRESULT_Ret& outParam)
{
	if (GetInterface())
	{
		outParam = GetInterface()->SetEnable(inParam.bParam);
	}
	return 0;
}

// 使用手动设置的网关或者自动获取网关
int KxeKpfwArpInfo::GetAutoGateWay		(IN const Kxe_VOID_Param&,			OUT Kxe_Getway_Ret& outParam)
{
	if (GetInterface())
	{
		BOOL b = 0;
		vector<mac_ip> info;

		GetInterface()->DelArpCache();
		outParam.hr = GetInterface()->GetAutoGateWay(&b, &info);
		outParam.bAuto = b;

		outParam.getway.reserve(info.size());
		for (int i = 0; i < (int)info.size(); i++)
		{
			Kxe_MacIp_Param param;
			Dpt_hlp::FromMacIp(param, info[i]);
			outParam.getway.push_back(param);
		}
	}
	return 0;
}
int KxeKpfwArpInfo::SetAutoGateWay		(IN const Kxe_Getway_Param& inParam,		OUT Kxe_HRESULT_Ret& outParam)
{
	if (GetInterface())
	{
		vector<mac_ip> info;
		info.reserve(inParam.getway.size());
		for(int i = 0; i < (int)inParam.getway.size(); i++)
		{
			mac_ip macip = {0};
			Dpt_hlp::ToMacIp(inParam.getway[i], macip);
			info.push_back(macip);
		}
		outParam = GetInterface()->SetAutoGateWay(inParam.bAuto, &info);
	}
	return 0;
}
int KxeKpfwArpInfo::GetMacFromIp			(IN const Kxe_MacIp_Param& inParam,			OUT Kxe_MacIp_Ret& outParam)
{
	if (GetInterface())
	{
		mac_ip macip = {0};
		macip.ip = inParam.ip;
		outParam.hr = GetInterface()->GetMacFromIp(&macip);
		Dpt_hlp::FromMacIp(outParam.macIp, macip);
		;	
	}
	return 0;
}

//arp 高级设置
int KxeKpfwArpInfo::GetNotifyGateway	(IN const Kxe_VOID_Param&,			OUT Kxe_NotifyGetway_Ret& outParam)
{
	if (GetInterface())
	{
		outParam.hr = GetInterface()->GetNotifyGateway(outParam.bNotify, outParam.nTimes);
	}
	return 0;
}
int KxeKpfwArpInfo::SetNotifyGateway	(IN const Kxe_NotifyGetway_Param& inParam,	OUT Kxe_HRESULT_Ret& outParam)
{
	if (GetInterface())
	{
		outParam = GetInterface()->SetNotifyGateway(inParam.bNotify, inParam.nTimes);
	}
	return 0;
}

int KxeKpfwArpInfo::GetAdvSetting		(IN const Kxe_VOID_Param&,		OUT Kxe_ArpAdv_Ret& outParam)
{
	if (GetInterface())
	{
		outParam.hr = GetInterface()->GetAdvSetting(
            outParam.arpdav.bSafeMode, 
            outParam.arpdav.bBlockRecvAttack, 
			outParam.arpdav.bBlockRecvIPAttack, 
            outParam.arpdav.bBlockSendAttack
            );
	}
	return 0;
}
int KxeKpfwArpInfo::SetAdvSetting		(IN const Kxe_ArpAdv_Param& inParam,			OUT Kxe_HRESULT_Ret& outParam)
{
	if (GetInterface())
	{
		outParam = GetInterface()->SetAdvSetting(
            inParam.bSafeMode, 
            inParam.bBlockRecvAttack, 
            inParam.bBlockRecvIPAttack, 
            inParam.bBlockSendAttack
            );
	}
	return 0;
}

int KxeKpfwArpInfo::Refresh				(IN const Kxe_VOID_Param&,			OUT Kxe_HRESULT_Ret& outParam)
{
	if (GetInterface())
	{
		outParam = GetInterface()->Refresh();
	}
	return 0;
}


int KxeKpfwArpInfo::InitDriver			(IN const Kxe_VOID_Param&,			OUT Kxe_HRESULT_Ret& outParam)
{
	return	m_arpInfo.Initialize();
}

int KxeKpfwArpInfo::GetLastLog          (IN const Kxe_VOID_Param&,          OUT Kxe_StringHResult_Ret& outParam)
{
    std::string strLastLog = "";

    if (GetInterface())
    {
        USES_CONVERSION;
        GetInterface()->GetLastLog(strLastLog);
        std::wstring strLog = A2W(strLastLog.c_str());
        outParam.str = strLog;
        outParam.hr = S_OK;
    }
    return 0;
}

int KxeKpfwArpInfo::ApplySetting		(IN const Kxe_VOID_Param&,			OUT Kxe_HRESULT_Ret& outParam)
{
	outParam.hr = E_FAIL;

	if (GetInterface())
	{
		GetInterface()->ApplySetting();
		outParam.hr = S_OK;
	}
	return 0;
}

int KxeKpfwArpInfo::ClearProtectList	(IN const Kxe_VOID_Param&,			OUT Kxe_HRESULT_Ret& outParam)
{
    outParam.hr = E_FAIL;

    if (GetInterface())
    {
        GetInterface()->ClearProtectList();
        outParam.hr = S_OK;
    }

    return 0;
}

KXE_REGISTER_COMMAND_PROC(KxeKpfwArpInfo, IArpInfoDpt, InitDriver);
KXE_REGISTER_COMMAND_PROC(KxeKpfwArpInfo, IArpInfoDpt, GetHostInfo);
KXE_REGISTER_COMMAND_PROC(KxeKpfwArpInfo, IArpInfoDpt, GetHostAdapterInfo);
KXE_REGISTER_COMMAND_PROC(KxeKpfwArpInfo, IArpInfoDpt, Ping);
KXE_REGISTER_COMMAND_PROC(KxeKpfwArpInfo, IArpInfoDpt, GetEnable);
KXE_REGISTER_COMMAND_PROC(KxeKpfwArpInfo, IArpInfoDpt, SetEnable);
KXE_REGISTER_COMMAND_PROC(KxeKpfwArpInfo, IArpInfoDpt, GetAutoGateWay);
KXE_REGISTER_COMMAND_PROC(KxeKpfwArpInfo, IArpInfoDpt, SetAutoGateWay);
KXE_REGISTER_COMMAND_PROC(KxeKpfwArpInfo, IArpInfoDpt, GetMacFromIp);
KXE_REGISTER_COMMAND_PROC(KxeKpfwArpInfo, IArpInfoDpt, GetNotifyGateway);
KXE_REGISTER_COMMAND_PROC(KxeKpfwArpInfo, IArpInfoDpt, SetNotifyGateway);
KXE_REGISTER_COMMAND_PROC(KxeKpfwArpInfo, IArpInfoDpt, GetAdvSetting);
KXE_REGISTER_COMMAND_PROC(KxeKpfwArpInfo, IArpInfoDpt, SetAdvSetting);
KXE_REGISTER_COMMAND_PROC(KxeKpfwArpInfo, IArpInfoDpt, Refresh);
KXE_REGISTER_COMMAND_PROC(KxeKpfwArpInfo, IArpInfoDpt, GetLastLog);
KXE_REGISTER_COMMAND_PROC(KxeKpfwArpInfo, IArpInfoDpt, ApplySetting);
KXE_REGISTER_COMMAND_PROC(KxeKpfwArpInfo, IArpInfoDpt, ClearProtectList);