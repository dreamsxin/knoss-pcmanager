// arpsetting.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "resource.h"
#include "arpsetting.h"
#include "arpsettingimpl.h"

KAS_BEGIN_OBJECT_MAP()
KAS_OBJECT_ENTRY( IArpFwSetting, CArpSettingImpl )
KAS_END_OBJECT_MAP()

KDllModule _Module;

STDAPI  DllCanUnloadNow(void)
{
    return _Module.DllCanUnloadNow();
}

STDAPI  DllGetClassObject(IN REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv)
{
    return _Module.DllGetClassObject( rclsid, riid, ppv );
}


BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) 
        _Module.Init( hModule );
    return TRUE;
}