#include "stdafx.h"
#include "arpsetting.h"
#include "kxebase/kxebase.h"

//////////////////////////////////////////////////////////////////////////

CArpSetting::CArpSetting()
{
    m_bKxeBaseInited = FALSE;

    m_spIArpFwSetting.LoadInterface(_T("arpsetting.dll"));
    m_spIArpFwSp.LoadInterface(_T("arpproxy.dll"));
}

CArpSetting::~CArpSetting()
{

}

//////////////////////////////////////////////////////////////////////////

CArpSetting& CArpSetting::Instance()
{
    static CArpSetting _singleton;
    return _singleton;
}

IArpFwSetting* CArpSetting::GetPtr()
{
    return m_spIArpFwSetting.m_pT;
}

BOOL CArpSetting::Install()
{
    BOOL retval = FALSE;
    BOOL bRetCode;

    KAV_PROCESS_ERROR(m_spIArpFwSetting);

    bRetCode = m_spIArpFwSetting->InstallArpFw();
    KAV_PROCESS_ERROR(bRetCode);

    retval = TRUE;

Exit0:
    return retval;
}

BOOL CArpSetting::UnInstall()
{
    BOOL retval = FALSE;
    BOOL bRetCode;

    KAV_PROCESS_ERROR(m_spIArpFwSetting);

    bRetCode = m_spIArpFwSetting->UnInstallArpFw();
    KAV_PROCESS_ERROR(bRetCode);

    retval = TRUE;

Exit0:
    return retval;
}

BOOL CArpSetting::StartSP()
{
    BOOL retval = FALSE;
    BOOL bRetCode;

    KAV_PROCESS_ERROR(m_spIArpFwSp);

    KxeBaseInit();

    bRetCode = m_spIArpFwSp->Start();
    KAV_PROCESS_ERROR(bRetCode);

    retval = TRUE;

Exit0:
    return retval;
}

BOOL CArpSetting::StopSP()
{
    BOOL retval = FALSE;
    BOOL bRetCode;

    KAV_PROCESS_ERROR(m_spIArpFwSp);

    bRetCode = m_spIArpFwSp->Stop();
    KAV_PROCESS_ERROR(bRetCode);

    KxeBaseUnInit();

    retval = TRUE;

Exit0:
    return retval;
}

int CArpSetting::KxeBaseInit(void)
{
    int nRet = kxe_base_init_client();
    if (nRet != 0)
    {
        return nRet;
    }

    nRet = kxe_base_start();
    if (nRet != 0)
    {
        kxe_base_fini();
        return nRet;
    }

    m_bKxeBaseInited = TRUE;
    return 0;
}

int CArpSetting::KxeBaseUnInit(void)
{
    if (!m_bKxeBaseInited)
        return 0;

    kxe_base_stop_ipc();
    kxe_base_stop();
    kxe_base_fini();

    m_bKxeBaseInited = FALSE;

    return 0;

}

//////////////////////////////////////////////////////////////////////////
