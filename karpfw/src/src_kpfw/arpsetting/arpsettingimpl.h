#pragma once

//////////////////////////////////////////////////////////////////////////

#include "arpsetting.h"
#include "arpsetting_public.h"
#include "antiarp/antiarpinterface.h"
#include "kasesc/KComInterfacePtr.h"

//////////////////////////////////////////////////////////////////////////

class CArpSettingImpl : public IArpFwSetting
{
public:
    CArpSettingImpl();
    virtual ~CArpSettingImpl();

// public: // IUnknown
//     STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject);
//     STDMETHODIMP_(ULONG) AddRef(void);
//     STDMETHODIMP_(ULONG) Release(void);

public: // IArpFwSetting
    STDMETHODIMP HasInstalled(BOOL* pbInstalled);
    STDMETHODIMP InstallArpFw();
    STDMETHODIMP UnInstallArpFw();
    STDMETHODIMP EnableArpFw(BOOL bEnable);
    STDMETHODIMP IsArpFwEnable(BOOL* pbEnable);
    STDMETHODIMP ShowSetting(HWND hParent);
    STDMETHODIMP GetLastLog(int nLog, wchar_t* pLog);

    KAS_BEGIN_COM_MAP( CArpSettingImpl )
        KAS_COM_INTERFACE_ENTRY( IArpFwSetting )
    KAS_END_COM_MAP()

protected:
    BOOL LoadArpInstaller();
    void FreeArpInstaller();

private:
    ULONG m_dwRefCount;
    KComInterfacePtr<IArpInstaller> m_spIArpInstaller;
};

//////////////////////////////////////////////////////////////////////////
