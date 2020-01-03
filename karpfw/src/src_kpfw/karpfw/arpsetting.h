#pragma once

//////////////////////////////////////////////////////////////////////////

#include "kcominterfaceptr.h"
#include "../arpsetting/arpsetting_public.h"
#include "antiarp/AntiArpInterface.h"

//////////////////////////////////////////////////////////////////////////

class CArpSetting
{
public:
    static CArpSetting& Instance();
    BOOL Install();
    BOOL UnInstall();
    BOOL StartSP();
    BOOL StopSP();
    IArpFwSetting* GetPtr();

protected:
    int KxeBaseInit(void);
    int KxeBaseUnInit(void);

    BOOL m_bKxeBaseInited;

private:
    CArpSetting();
    ~CArpSetting();

    KComInterfacePtr<IArpFwSetting> m_spIArpFwSetting;
    KComInterfacePtr<IKArpSp> m_spIArpFwSp;
};

//////////////////////////////////////////////////////////////////////////
