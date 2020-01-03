// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:

//#include "buildcfg/buildcfg.h"
#include <Winsock2.h>
#include <windows.h>
#include <shlwapi.h>
#include <tchar.h>
#include <stdio.h>
#include <mmsystem.h>
#include <ntddndis.h>
#include <strsafe.h>
#include <WinIoCtl.h>
#include <atlbase.h>
#include <atlstr.h>
#include <list>
#include <vector>
using namespace std;

#include "kascom.h"
#include <Iprtrmib.h>

extern HINSTANCE g_hModule;
// TODO: reference additional headers your program requires here
//////////////////////////////////////////////////////////////////////////
#define  KAV_PROCESS_ERROR( x )		if ( !( x ) ) goto Exit0;
#define  KAV_COM_PROCESS_ERROR( x )  if ( FAILED( x ) ) goto Exit0;
//////////////////////////////////////////////////////////////////////////
#define APP_NAME                            L"KNdisInstall"
#define KNDISFLT_SERVICE_PNP_DEVICE_ID      L"ms_kndisflt"
//////////////////////////////////////////////////////////////////////////


// TODO: reference additional headers your program requires here
