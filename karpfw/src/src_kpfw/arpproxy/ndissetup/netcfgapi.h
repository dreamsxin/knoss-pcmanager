/*++

Copyright (C) Kingsoft Corporation 2009

Creator:

Chen Liang <chenliang@kingsoft.com>

Description:

Implement of net setup

取代 snetcfg.exe 来进行NDIS驱动的安装

--*/

#ifndef _NETCFGAPI_H_
#define _NETCFGAPI_H_

#include "pch.h"
#pragma hdrstop


//////////////////////////////////////////////////////////////////////////

#define LOCK_TIME_OUT     5000

//////////////////////////////////////////////////////////////////////////

HRESULT 
HrGetINetCfg(
			 IN BOOL fGetWriteLock,
			 IN LPCTSTR lpszAppName,
			 OUT INetCfg** ppnc,
			 OUT LPTSTR *lpszLockedBy
			 );

HRESULT 
HrReleaseINetCfg(
				 INetCfg* pnc,
				 BOOL fHasWriteLock
				 );

HRESULT 
HrInstallNetComponent(
					  IN INetCfg *pnc,
					  IN LPCTSTR szComponentId,
					  IN const GUID    *pguildClass,
					  IN LPCTSTR lpszInfFullPath
					  );

HRESULT 
HrInstallComponent(
				   IN INetCfg* pnc,
				   IN LPCTSTR szComponentId,
				   IN const GUID* pguidClass
				   );

HRESULT 
HrUninstallNetComponent(
						IN INetCfg* pnc,
						IN LPCTSTR szComponentId
						);

VOID 
ReleaseRef(
		   IUnknown* punk
		   );


#endif