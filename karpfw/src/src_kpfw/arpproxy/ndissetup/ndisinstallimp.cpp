#include "stdafx.h"
#include "ndisinstallimp.h"
#include "netcfgapi.h"
#include "../log_file.h"
#include <Shlwapi.h>

CNdisInstallImp::CNdisInstallImp()
{

}

CNdisInstallImp::~CNdisInstallImp()
{

}

STDMETHODIMP_(BOOL) 
CNdisInstallImp::InstallNdisDriver(	
								   LPCTSTR lpcszInfPath,
								   ULONG uTimeOut, 
								   BOOL* bTimeOut, 
								   BOOL* bNeedReboot, 
								   BOOL* bCanceled
								   )
{
	if ( !bTimeOut || !bNeedReboot || !bCanceled )
		return FALSE;

	BOOL bRetCode = FALSE;
	HANDLE hInstall = NULL;
	::CNdisInstallImp::INSTALL_PARAM param;
	
	param.pCancelled = bCanceled;
	param.pInfFilePath = lpcszInfPath;
	param.pNeedReboot = bNeedReboot;
	param.pThis = this;
	param.fRetCode = FALSE;

	*bTimeOut = FALSE;

	hInstall = (HANDLE)_beginthreadex(NULL, 0, InstallRoutine, &param, 0, NULL);
	if ( !hInstall )
		return FALSE;

	DWORD dwRet = WaitForSingleObject(hInstall, uTimeOut);
	if ( dwRet != WAIT_OBJECT_0 )
	{
		*bTimeOut = TRUE;
		TerminateThread(hInstall, 1);
	}
	else
	{
		bRetCode = param.fRetCode;
	}

	if ( hInstall )
	{
		CloseHandle(hInstall);
		hInstall = NULL;
	}

	return bRetCode;
	
}

STDMETHODIMP_(BOOL)
CNdisInstallImp::UnInstallNdisDriver(
									 ULONG uTimeOut, 
									 BOOL* bTimeOut,
									 BOOL* bNeedReboot
									 )
{
	if ( !bNeedReboot || !bTimeOut )
		return FALSE;

	BOOL bRetCode = FALSE;
	HANDLE hInstall = NULL;
	::CNdisInstallImp::UNINSTALL_PARAM param;

	param.pNeedReboot = bNeedReboot;
	param.pThis = this;
	param.fRetCode = FALSE;
	
	*bTimeOut = FALSE;

	hInstall = (HANDLE)_beginthreadex(NULL, 0, UninstallRoutine, &param, 0, NULL);
	if ( !hInstall )
		return FALSE;

	DWORD dwRet = WaitForSingleObject(hInstall, uTimeOut);
	if ( dwRet != WAIT_OBJECT_0 )
	{
		*bTimeOut = TRUE;
		TerminateThread(hInstall, 1);
	}
	else
	{
		bRetCode = TRUE;
	}

	if ( hInstall )
	{
		CloseHandle(hInstall);
		hInstall = NULL;
	}

	return bRetCode;
	
}

unsigned WINAPI
CNdisInstallImp::InstallRoutine(
								void *pParam
								)
{
	PINSTALL_PARAM pInstall = (PINSTALL_PARAM)pParam;

	if ( !pInstall )
		goto Exit0;

	pInstall->fRetCode = pInstall->pThis->InstallDriver(
		pInstall->pInfFilePath,
		pInstall->pNeedReboot,
		pInstall->pCancelled
		);

Exit0:
	_endthreadex(0);
	return 0;

}

unsigned WINAPI
CNdisInstallImp::UninstallRoutine(
								  void *pParam
								  )
{
	PUNINSTALL_PARAM pUninstall = (PUNINSTALL_PARAM)pParam;

	if ( !pUninstall )
		goto Exit0;

	pUninstall->fRetCode = pUninstall->pThis->UnInstallDriver(
		pUninstall->pNeedReboot
		);

Exit0:
	_endthreadex(0);
	return 0;
}
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP 
CNdisInstallImp::InstallSpecifiedComponent(
	LPTSTR lpszInfFile,
	LPTSTR lpszPnpID,
	const GUID *pguidClass
	)
{
	INetCfg    *pnc;
	LPTSTR     lpszApp;
	HRESULT    hr;

	::SetLastError(0);
	hr = HrGetINetCfg(TRUE, APP_NAME, &pnc, &lpszApp);
	KAV_COM_PROCESS_ERROR(hr);

	hr = HrInstallNetComponent( pnc,
		lpszPnpID,
		pguidClass,
		lpszInfFile );

	if ( (hr == S_OK) || (hr == NETCFG_S_REBOOT) )
	{
		hr = pnc->Apply();
	}
	else
		goto Exit0;

	HrReleaseINetCfg(pnc, TRUE);

Exit0:
	if ((hr == NETCFG_E_NO_WRITE_LOCK) && lpszApp)
	{
		CoTaskMemFree(lpszApp);
	}
	return hr;
}
//////////////////////////////////////////////////////////////////////////
BOOL
CNdisInstallImp::InstallDriver(
							   const WCHAR cwszInfFilePath[], 
							   BOOL *pNeedReboot, 
							   BOOL *pCancelled
							   )
{
	BOOL retval = FALSE;
	HRESULT hr = E_FAIL;
	WCHAR   wszFileFullPath[MAX_PATH];
	WCHAR   wszCurrentDirectory[MAX_PATH];
	DWORD dwRetCode;

	//KAV_PROCESS_ERROR(cwszInfFilePath);
	KAV_PROCESS_ERROR(pNeedReboot);
	KAV_PROCESS_ERROR(pCancelled);

	*pNeedReboot = FALSE;
	*pCancelled = FALSE;

	RtlZeroMemory(wszCurrentDirectory, sizeof(wszCurrentDirectory));

	if (!cwszInfFilePath)
	{
		dwRetCode = GetModuleFileNameW(g_hModule, wszFileFullPath, MAX_PATH);
		KAV_PROCESS_ERROR(dwRetCode);
		KAV_PROCESS_ERROR(dwRetCode < MAX_PATH);

		*(wcsrchr(wszFileFullPath, L'\\')) = 0L;
		GetCurrentDirectoryW(MAX_PATH, wszCurrentDirectory);
		SetCurrentDirectoryW(wszFileFullPath);
		StringCchCatW(wszFileFullPath, MAX_PATH, L"\\kndisflt.inf");
	}
	else
	{
		hr = StringCchCopyW(wszFileFullPath, MAX_PATH, cwszInfFilePath);
		KAV_COM_PROCESS_ERROR(hr);
	}

	hr = InstallSpecifiedComponent( wszFileFullPath,
		KNDISFLT_SERVICE_PNP_DEVICE_ID,
		&GUID_DEVCLASS_NETSERVICE
		);

	if (hr == NETCFG_S_REBOOT)
	{
		*pNeedReboot = TRUE;
		retval = TRUE;
	}
	else if (hr == S_OK)
	{
		retval = TRUE;
	}
	else if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
	{
		*pCancelled = TRUE;
	}


Exit0:
	if (wcslen(wszCurrentDirectory))
	{
		SetCurrentDirectoryW(wszCurrentDirectory);
	}

	return retval;
}

BOOL
CNdisInstallImp::UnInstallDriver(BOOL* pNeedReboot )
{
	BOOL                 retval = FALSE;
	INetCfg              *pnc;
	INetCfgComponent     *pncc;
	INetCfgClass         *pncClass;
	INetCfgClassSetup    *pncClassSetup;
	LPTSTR               lpszApp;
	GUID                 guidClass;
	OBO_TOKEN            obo;
	HRESULT              hr;

	if (!pNeedReboot)
		return FALSE;

	*pNeedReboot = FALSE;

	hr = HrGetINetCfg(TRUE, APP_NAME, &pnc, &lpszApp);

	if (hr == S_OK) 
	{
		//
		// Get a reference to the network component to uninstall.
		//
		hr = pnc->FindComponent(KNDISFLT_SERVICE_PNP_DEVICE_ID, &pncc);

		if (hr == S_OK)
		{
			//
			// Get the class GUID.
			//
			hr = pncc->GetClassGuid(&guidClass);

			if (hr == S_OK)
			{
				//
				// Get a reference to component's class.
				//

				hr = pnc->QueryNetCfgClass(&guidClass, IID_INetCfgClass, (PVOID *)&pncClass);
				if (hr == S_OK)
				{
					//
					// Get the setup interface.
					//

					hr = pncClass->QueryInterface(IID_INetCfgClassSetup, (LPVOID *)&pncClassSetup);

					if (hr == S_OK)
					{
						//
						// Uninstall the component.
						//

						ZeroMemory(&obo, sizeof(OBO_TOKEN));

						obo.Type = OBO_USER;

						hr = pncClassSetup->DeInstall(pncc, &obo, NULL);
						if ((hr == S_OK) || (hr == NETCFG_S_REBOOT))
						{
							hr = pnc->Apply();

							if (hr == S_OK)
							{
								retval = TRUE;
							} 
							else if (hr == NETCFG_S_REBOOT)
							{
								retval = TRUE;
								*pNeedReboot = TRUE;
							}
						}

						ReleaseRef(pncClassSetup);
					}

					ReleaseRef(pncClass);
				}
			}

			ReleaseRef(pncc);
		}

		HrReleaseINetCfg(pnc, TRUE);
	}
	else
	{
		if ( (hr == NETCFG_E_NO_WRITE_LOCK) && lpszApp )
		{
			CoTaskMemFree( lpszApp );
		}
	}

	CleanOemInf();

	CleanDriverFile();

	CleanSvrReg();

	return retval;
}

void
CNdisInstallImp::CleanOemInf()
{
	TCHAR szInfoPath[MAX_PATH];
	TCHAR szFormat[MAX_PATH];
	HRESULT hr;
	DWORD dwRetCode;
	int i;
	char szMsg[256];
	FILE* pFile = NULL;

	dwRetCode = GetWindowsDirectory(szFormat, MAX_PATH);

	if (szFormat[wcslen(szFormat) - 1] != '\\')
	{
		hr = StringCchCatW(szFormat, MAX_PATH, _T("\\"));
	}

	hr = StringCchCatW(szFormat, MAX_PATH, _T("inf\\oem%d.inf"));

	for (i = 0; i < 50; ++i)
	{
		hr = StringCchPrintfW(szInfoPath, MAX_PATH, szFormat, i);

		RtlZeroMemory(szMsg, 256);
		pFile = _wfopen(szInfoPath, _T("r"));
		if (pFile)
		{
			fread(szMsg, 1, 256, pFile);
			fclose(pFile);
		}

		szMsg[255] = 0;
		if (strstr(szMsg, "kndisflt.INF") || strstr(szMsg, "kndisflt_m.INF"))
		{
			DeleteFileW(szInfoPath);

			if (wcsrchr(szInfoPath, _T('.')))
			{
				*wcsrchr(szInfoPath, _T('.')) = 0;
				wcscat_s(szInfoPath, _T(".PNF"));
				DeleteFileW(szInfoPath);
			}
			break;
		}
	}

	return;
}

void
CNdisInstallImp::CleanDriverFile()
{
	TCHAR szDriverFile[MAX_PATH];
	HRESULT hr;
	DWORD dwRetCode;

	dwRetCode = GetWindowsDirectory(szDriverFile, MAX_PATH);

	if (szDriverFile[wcslen(szDriverFile) - 1] != '\\')
	{
		hr = StringCchCatW(szDriverFile, MAX_PATH, _T("\\"));
	}

	hr = StringCchCatW(szDriverFile, MAX_PATH, _T("system32\\drivers\\kndisflt.sys"));

	DeleteFileW(szDriverFile);

}

void CNdisInstallImp::CleanSvrReg()
{
	::SHDeleteKey(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\services\\kndisflt"));
}