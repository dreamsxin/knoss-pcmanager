#include "stdafx.h"
#include "arpsettingimpl.h"
#include "dlgsetting.h"
#include "common/utility.h"
#include "kpfw_dpt_def.h"
#include "dlgeditgateway.h"

//////////////////////////////////////////////////////////////////////////

CArpSettingImpl::CArpSettingImpl() : m_dwRefCount(0)
{   
    LoadArpInstaller();
}

CArpSettingImpl::~CArpSettingImpl()
{
}

//////////////////////////////////////////////////////////////////////////
/*
STDMETHODIMP CArpSettingImpl::QueryInterface(const IID &riid, void **ppvObject)
{
    if (!ppvObject)
        return E_NOINTERFACE;

    if (__uuidof(IUnknown) == riid)
    {
        *ppvObject = static_cast<IArpFwSetting*>(this);
    }
    else if (__uuidof(IArpFwSetting) == riid)
    {
        *ppvObject = static_cast<IArpFwSetting*>(this);
    }
    else
    {
        return E_NOINTERFACE;
    }

    reinterpret_cast<IUnknown*>(*ppvObject)->AddRef();
    return S_OK;
}

STDMETHODIMP_(ULONG) CArpSettingImpl::AddRef()
{
    return ++m_dwRefCount;
}

STDMETHODIMP_(ULONG) CArpSettingImpl::Release()
{
    ULONG dwRetCode = --m_dwRefCount;

    if (!dwRetCode)
        delete this;

    return dwRetCode;
}
*/
//////////////////////////////////////////////////////////////////////////

STDMETHODIMP CArpSettingImpl::HasInstalled(BOOL* pbInstalled)
{
    HRESULT hr = E_FAIL;

    if (NULL == m_spIArpInstaller)
        goto clean0;

    if (!pbInstalled)
    {
        hr = E_INVALIDARG;
        goto clean0;
    }

    hr = m_spIArpInstaller->IsRuning(pbInstalled);

clean0:
    return hr;
}

STDMETHODIMP CArpSettingImpl::InstallArpFw()
{
    HRESULT hr = E_FAIL;
    HRESULT hRetCode;
    BOOL bRunning = FALSE;

    if (NULL == m_spIArpInstaller)
        goto clean0;

    hRetCode = m_spIArpInstaller->IsRuning(&bRunning);

    if (bRunning)
    {
        hr = S_OK;
        goto clean0;
    }

    hr = m_spIArpInstaller->Install();

clean0:
    return hr;
}

STDMETHODIMP CArpSettingImpl::UnInstallArpFw()
{
    HRESULT hr = E_FAIL;
    HRESULT hRetCode;
    BOOL bRunning = FALSE;

    if (NULL == m_spIArpInstaller)
        goto clean0;

    hRetCode = m_spIArpInstaller->IsRuning(&bRunning);

    if (!bRunning)
    {
        hr = S_OK;
        goto clean0;
    }

    hr = m_spIArpInstaller->UnInstall();

clean0:
    return hr;
}

STDMETHODIMP CArpSettingImpl::EnableArpFw(BOOL bEnable)
{
    KxEPipeClientT<IArpInfoDpt> spArpFw;
    Kxe_BOOL_Param enable;
    Kxe_VOID_Param nil;
    Kxe_HRESULT_Ret ret;
    int nRetCode;

    enable.bParam = bEnable;
    ret.hr = E_FAIL;
    nRetCode = spArpFw.SetEnable(enable, ret);
    if (nRetCode)
        goto clean0;

    nRetCode = spArpFw.ApplySetting(nil, ret);
    if (nRetCode)
        goto clean0;

clean0:
    return ret.hr;
}

STDMETHODIMP CArpSettingImpl::IsArpFwEnable(BOOL* pbEnable)
{
    HRESULT hr;
    KxEPipeClientT<IArpInfoDpt> spArpFw;
    Kxe_VOID_Param nil;
    Kxe_INTHResult_Ret ret;

    if (!pbEnable)
    {
        hr = E_INVALIDARG;
        goto clean0;
    }

    ret.nRet = FALSE;
    spArpFw.GetEnable(nil, ret);

    *pbEnable = ret.nRet ? TRUE : FALSE;

    hr = S_OK;

clean0:
    return hr;
}

STDMETHODIMP CArpSettingImpl::GetLastLog(int nLog, wchar_t* pLog)
{
    HRESULT hr = E_FAIL;
    KxEPipeClientT<IArpInfoDpt> spArpFw;
    Kxe_VOID_Param nil;
    Kxe_StringHResult_Ret ret;
    int nRetCode;

    if (!nLog || !pLog)
        goto clean0;

    nRetCode = spArpFw.GetLastLog(nil, ret);
    if (nRetCode)
        goto clean0;

    if (nLog < (int)ret.str.length() + 1)
        goto clean0;

    wcscpy(pLog, ret.str.c_str());

    hr = S_OK;

clean0:
    return hr;
}

STDMETHODIMP CArpSettingImpl::ShowSetting(HWND hParent)
{
    HRESULT hr = E_FAIL;
    CArpSettingDlg dlgSetting;
    KxEPipeClientT<IArpInfoDpt> spArpFw;
    Kxe_VOID_Param nil;
    Kxe_ArpAdv_Ret arpAdv;
    Kxe_INTHResult_Ret enable;
    Kxe_NotifyGetway_Ret notifygw;
    Kxe_Getway_Ret gateway;
    //Kxe_ArpHostInfo_Ret arphost; 
    Kxe_HRESULT_Ret ret;
    int nRetCode;
    vector<Kxe_MacIp_Param>::const_iterator it;
    //size_t i;

    enable.nRet = FALSE;
    nRetCode = spArpFw.GetEnable(nil, enable);
    if (nRetCode)
        goto clean0;

    nRetCode = spArpFw.GetAdvSetting(nil, arpAdv);
    if (nRetCode)
        goto clean0;

    nRetCode = spArpFw.GetNotifyGateway(nil, notifygw);
    if (nRetCode)
        goto clean0;

    nRetCode = spArpFw.GetAutoGateWay(nil, gateway);
    if (nRetCode)
        goto clean0;

    nRetCode = spArpFw.Refresh(nil, ret);
    if (nRetCode)
        goto clean0;

//     nRetCode = spArpFw.GetHostInfo(nil, arphost);
//     if (nRetCode)
//         goto clean0;
// 
//     for (i = 0; i < (size_t)arphost.hostInfo.nGateWayCnt; ++i)
//     {
//         MyGw gw;
//         struct in_addr ip;
//         ip.s_addr = arphost.hostInfo.vecGetWay[i].ip;
//         gw.strIpAddress = inet_ntoa(ip);
//         mac_to_str((const BYTE*)&arphost.hostInfo.vecGetWay[i].mac, gw.strMacAddress);
//         dlgSetting.m_vDefaultGws.push_back(gw);
//     }

    dlgSetting.m_bEnableFw = (BOOL)enable.nRet;
    dlgSetting.m_bSafeMode = arpAdv.arpdav.bSafeMode;
    //dlgSetting.m_bBlockRecvAttack = arpAdv.arpdav.bBlockRecvAttack;
    dlgSetting.m_bBlockRecvIPAttack = arpAdv.arpdav.bBlockRecvIPAttack;
    dlgSetting.m_bBlockSendAttack = arpAdv.arpdav.bBlockSendAttack;
    dlgSetting.m_bNoticeGw = notifygw.bNotify;
    if (notifygw.nTimes < 1)
        notifygw.nTimes = 1;
    if (notifygw.nTimes > 50)
        notifygw.nTimes = 50;
    dlgSetting.m_nNoticeTimer = notifygw.nTimes;
    dlgSetting.m_bUseMyGw = gateway.bAuto ? FALSE : TRUE;
    for (it = gateway.getway.begin(); it != gateway.getway.end(); it++)
    {
        MyGw gw;
        struct in_addr ip;
        ip.s_addr = it->ip;
        gw.strIpAddress = inet_ntoa(ip);
        mac_to_str((const BYTE*)&it->mac, gw.strMacAddress);
        dlgSetting.m_vMyGws.push_back(gw);
    }

    if (dlgSetting.DoModal(hParent) == IDOK)
    {
        Kxe_Getway_Param _gw;
        Kxe_BOOL_Param _enable;
        Kxe_HRESULT_Ret ret;

        _gw.bAuto = dlgSetting.m_bUseMyGw ? FALSE : TRUE;
        for (size_t i = 0; i < dlgSetting.m_vMyGws.size(); ++i)
        {
            Kxe_MacIp_Param macip;
            UINT uIp = inet_addr(dlgSetting.m_vMyGws[i].strIpAddress.c_str());
            macip.ip = uIp;
            str_to_mac(dlgSetting.m_vMyGws[i].strMacAddress, (BYTE*)&macip.mac);
            _gw.getway.push_back(macip);
        }
        nRetCode = spArpFw.SetAutoGateWay(_gw, ret);
        if (nRetCode)
            goto clean0;

        _enable.bParam = dlgSetting.m_bEnableFw;
        nRetCode = spArpFw.SetEnable(_enable, ret);
        if (nRetCode)
            goto clean0;

		nRetCode = spArpFw.ApplySetting(nil, ret);
		if (nRetCode)
			goto clean0;

        hr = S_OK;
    }

clean0:
    return hr;
}

//////////////////////////////////////////////////////////////////////////

BOOL CArpSettingImpl::LoadArpInstaller()
{
    BOOL retval = FALSE;
    WCHAR wszProxy[MAX_PATH + 1] = { 0 };
    HRESULT hr;
    
    if (m_spIArpInstaller != NULL)
    {
        retval = TRUE;
        goto clean0;
    }

    GetModuleFileName(NULL, wszProxy, MAX_PATH);
    *(wcsrchr(wszProxy, L'\\')) = 0L;
    wcscat_s(wszProxy, MAX_PATH, L"\\arpproxy.dll");

    hr = m_spIArpInstaller.LoadInterface(wszProxy);
    if (hr != S_OK)
        goto clean0;

    retval = TRUE;

clean0:
    return retval;
}

//////////////////////////////////////////////////////////////////////////
