// kantiarpdevc.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "KASAntiArpDevC.h"
#include "arpinstaller.h"

KAS_BEGIN_OBJECT_MAP()
KAS_OBJECT_ENTRY( IAntiArpDevC, KASAntiArpDevC )
KAS_OBJECT_ENTRY( IArpInstaller, KArpInstaller )
KAS_END_OBJECT_MAP()

USE_DEFAULT_COM_PROC

HINSTANCE g_hModule = NULL;
//////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain(
	__in HANDLE     hInstance, 
	__in DWORD      dwReason, 
	__in_opt LPVOID lpReserved
	)
{
	UNREFERENCED_PARAMETER(lpReserved);

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls((HMODULE)hInstance);
		g_hModule = (HINSTANCE)hInstance;
		break;
	case DLL_PROCESS_DETACH:
		break;
	default:
		break;
	}

	return TRUE;
}

