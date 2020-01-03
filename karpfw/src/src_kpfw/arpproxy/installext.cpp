// InstallExt.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include "InstallExt.h"
#include <atlpath.h>
#include <shellapi.h>
#include <atlbase.h>
#include <Winbase.h>
#include "log_file.h"

#include "closewhql/closewhql.h"


InstallExt::InstallExt()
:m_piNdisImp(NULL)
{
}


HRESULT InstallExt::DoInstall()
{
	HRESULT hrResult = E_FAIL;

	WCHAR szModulePath[MAX_PATH] = { 0 };
	WCHAR szWindowInfPath[MAX_PATH] = { 0 };
	CLSID nilClsId = { 0 };
	BOOL bTimeOut = FALSE;
	BOOL bNeedReboot = FALSE;
	BOOL bCancel = FALSE;
	BOOL bRetCode = FALSE;
	CCloseWHQL CloseWsql;

	::SetLastError(0);

	::GetModuleFileName(g_hModule, szModulePath, MAX_PATH);
	::GetSystemWindowsDirectory(szWindowInfPath, MAX_PATH);

	*(wcsrchr(szModulePath, L'\\') + 1) = '\0';
	
	if ( !m_piNdisImp )
		m_piNdisImp = new CNdisInstallImp;

	if ( m_piNdisImp )
	{
		StringCchCatW(szModulePath, MAX_PATH, _T("kndisflt_m.inf"));
		
		StringCchCatW(szWindowInfPath, MAX_PATH, _T("\\inf\\kndisflt_m.inf"));

		if ( !::CopyFile(szModulePath, szWindowInfPath, FALSE) )
		{
			goto Exit0;
		}

		*(wcsrchr(szModulePath, L'\\') + 1) = '\0';

		StringCchCatW(szModulePath, MAX_PATH, _T("kndisflt.inf"));

		//开启线程来防止出现徽标验证对话框
		CloseWsql.StartCloseThread();
		
		::SetLastError(0);
		bRetCode = m_piNdisImp->InstallNdisDriver(szModulePath, 2 * 60 * 1000, &bTimeOut, &bNeedReboot, &bCancel);
		if ( !bRetCode )
		{
			goto Exit0;
		}
	}

	hrResult = S_OK;
Exit0:

	if ( m_piNdisImp )
	{
		delete m_piNdisImp;
		m_piNdisImp = NULL;
	}

	CloseWsql.EndCloseThread();

	return hrResult;

}

HRESULT InstallExt::DoUninstall()
{
	HRESULT hrResult = E_FAIL;
	WCHAR szWindowInfPath[MAX_PATH] = { 0 };

	BOOL bTimeOut = FALSE;
	BOOL bNeedReboot = FALSE;
	BOOL bCancel = FALSE;

	::SetLastError(0);
	
	::GetSystemWindowsDirectory(szWindowInfPath, MAX_PATH);

	if ( !m_piNdisImp )
		m_piNdisImp = new CNdisInstallImp;

	if ( m_piNdisImp )
	{
		StringCchCatW(szWindowInfPath, MAX_PATH, _T("\\inf\\kndisflt_m.inf"));

		m_piNdisImp->UnInstallNdisDriver(2 * 60 * 1000, &bTimeOut, &bNeedReboot);
		if ( bTimeOut )
		{
			goto Exit0;
		}

	}
	
	DeleteFileW(szWindowInfPath);

	hrResult = S_OK;
Exit0:

	if ( m_piNdisImp )
	{
		delete m_piNdisImp;
		m_piNdisImp = NULL;
	}
	return hrResult;

}
