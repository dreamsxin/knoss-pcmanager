#include "stdafx.h"
#include "arpinstaller.h"
#include "installext.h"
#include <shlwapi.h>
#include <string>

#pragma comment (lib, "shlwapi.lib")

//////////////////////////////////////////////////////////////////////////

KArpInstaller::KArpInstaller()
{
}

KArpInstaller::~KArpInstaller()
{
	
}

//////////////////////////////////////////////////////////////////////////
 
STDMETHODIMP KArpInstaller::Install()
{
	InstallExt ext;
	return ext.DoInstall();
}

STDMETHODIMP KArpInstaller::UnInstall()
{
	InstallExt ext;
	return ext.DoUninstall();
}
 

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP KArpInstaller::SetConfig( DWORD dwEnableFlag, DWORD dwRequestFlag )
{
	HRESULT hResult = ERROR_SUCCESS;
	
	hResult = SHSetValueW( HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\"NDIS_ANTIARP_DRIVER, 
		L"Enable", REG_DWORD, (PVOID)&dwEnableFlag, sizeof(DWORD) );
	if( hResult != ERROR_SUCCESS )
		return hResult;

	hResult = SHSetValueW( HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\"NDIS_ANTIARP_DRIVER, 
		L"Request", REG_DWORD, (PVOID)&dwRequestFlag, sizeof(DWORD) );
	if( hResult != ERROR_SUCCESS )
		return hResult;

	return hResult;

}

STDMETHODIMP KArpInstaller::IsRuning( BOOL* bRuning )
{
	HRESULT hrResult = E_FAIL;
	BOOL bSvrRuning	= FALSE;

	SC_HANDLE hSvrMgr = NULL;
	SC_HANDLE hSvr = NULL;

	::SetLastError(0);

	do 
	{
		hSvrMgr = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if ( !hSvrMgr )
		{
			hrResult = HRESULT_FROM_WIN32(GetLastError());
			break;
		}

		hSvr = ::OpenService(hSvrMgr, NDIS_ANTIARP_DRIVER, SERVICE_QUERY_STATUS);
		if ( !hSvr )
		{
			hrResult = HRESULT_FROM_WIN32(GetLastError());
			break;
		}

		if ( hSvr )
		{
			SERVICE_STATUS svrstatus = { 0 };
			if ( !::QueryServiceStatus(hSvr, &svrstatus) )
			{
				hrResult = HRESULT_FROM_WIN32(GetLastError());
				break;
			}

			*bRuning = (svrstatus.dwCurrentState == SERVICE_RUNNING);
		}

		hrResult = S_OK;

	} while (FALSE);

	if ( hSvr )
	{
		::CloseServiceHandle(hSvr);
		hSvr = NULL;
	}
	if ( hSvrMgr )
	{
		::CloseServiceHandle(hSvrMgr);
		hSvrMgr = NULL;
	}

	return hrResult;
}