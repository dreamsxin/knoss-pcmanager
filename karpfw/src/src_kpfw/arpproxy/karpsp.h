#pragma once
#include "antiarp/AntiArpInterface.h"
#include "kpfw_dpt_def.h"
#include "idl_common.h"
#include "karpmgr.h"

class KxeKpfwArpInfo:public KxEIPCServerT<IArpInfoDpt>,
	public IKArpSp
{
public:
	STDPROC_(int) Start();
	STDPROC_(int) Stop();

	STDPROC_(int) InitDriver			(IN const Kxe_VOID_Param&,			OUT Kxe_HRESULT_Ret& );
	STDPROC_(int) GetHostInfo			(IN const Kxe_VOID_Param&,			OUT Kxe_ArpHostInfo_Ret& );
	STDPROC_(int) GetHostAdapterInfo	(IN const Kxe_VOID_Param&,			OUT Kxe_KHostAdapterInfo_Ret&) ;
	STDPROC_(int) Ping					(IN const Kxe_String_Param&,		OUT Kxe_HRESULT_Ret& );

	// 启用arp防火墙
	STDPROC_(int) GetEnable				(IN const Kxe_VOID_Param&,			OUT Kxe_INTHResult_Ret& );
	STDPROC_(int) SetEnable				(IN const Kxe_BOOL_Param&,			OUT Kxe_HRESULT_Ret&);

	// 使用手动设置的网关或者自动获取网关
	STDPROC_(int) GetAutoGateWay		(IN const Kxe_VOID_Param&,			OUT Kxe_Getway_Ret&);
	STDPROC_(int) SetAutoGateWay		(IN const Kxe_Getway_Param&,		OUT Kxe_HRESULT_Ret&);
	STDPROC_(int) GetMacFromIp			(IN const Kxe_MacIp_Param&,			OUT Kxe_MacIp_Ret&);

	//arp 高级设置
	STDPROC_(int)	GetNotifyGateway	(IN const Kxe_VOID_Param&,			OUT Kxe_NotifyGetway_Ret&);
	STDPROC_(int)	SetNotifyGateway	(IN const Kxe_NotifyGetway_Param&,	OUT Kxe_HRESULT_Ret&);

	STDPROC_(int)	GetAdvSetting		(IN const Kxe_VOID_Param&,		OUT Kxe_ArpAdv_Ret& outParam);
	STDPROC_(int)	SetAdvSetting		(IN const Kxe_ArpAdv_Param& ,		OUT Kxe_HRESULT_Ret& outParam);

	STDPROC_(int)	Refresh				(IN const Kxe_VOID_Param&,			OUT Kxe_HRESULT_Ret&);

    STDPROC_(int)   GetLastLog          (IN const Kxe_VOID_Param&,          OUT Kxe_StringHResult_Ret&);
	STDPROC_(int)	ApplySetting		(IN const Kxe_VOID_Param&,			OUT Kxe_HRESULT_Ret&);
    STDPROC_(int)	ClearProtectList	(IN const Kxe_VOID_Param&,			OUT Kxe_HRESULT_Ret&);

	//SCOM
	KAS_BEGIN_COM_MAP( KxeKpfwArpInfo )
	KAS_COM_INTERFACE_ENTRY( IKArpSp )
	KAS_END_COM_MAP()

private:
	KArpInfo* GetInterface();
	KArpInfo m_arpInfo;
};
