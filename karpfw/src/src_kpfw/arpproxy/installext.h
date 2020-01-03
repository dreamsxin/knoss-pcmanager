/********************************************************************
* CreatedOn: 2007-10-22   15:25
* FileName: InstallExt.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include "./ndissetup/ndisinstallimp.h"
enum{
    em_none,
    em_install,
    em_uninstall
};

class InstallExt
{
public:
    
    InstallExt();

    HRESULT RunInsatll(LPTSTR pszCmdLine, LPTSTR pszCurPath);
    HRESULT DoInstall();
    HRESULT DoUninstall();
    void WaitEXEExit();

private:
    CNdisInstallImp* m_piNdisImp;
};