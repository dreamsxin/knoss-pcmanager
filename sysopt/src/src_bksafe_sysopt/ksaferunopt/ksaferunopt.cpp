// ksaferunopt.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <runoptimize/interface.h>
#include "RunManEngine.h"
#include <markupupdate/DataUpdate.h>
#include "LibLoader.h"
#include "CRunDatQueryEng.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

extern "C"
{

IRunOptimizeEng* WINAPI pxGetRunOptEng(DWORD nVer)
{
	if (nVer!=VERSION_KSAFE_RUNOPT_INTERFACE)
		return NULL;

	CRunManEngine* eng = new CRunManEngine;
	return (IRunOptimizeEng*)eng;
}

VOID WINAPI pxFreeRunOptEng(IRunOptimizeEng* pEng)
{
	CRunManEngine* eng = (CRunManEngine*)pEng;
	
	if (eng!=NULL)
	{
		delete eng;
	}
}

IRunDatQueryEng* WINAPI pxGetQueryEng(DWORD nVer)
{
	if (nVer!=VERSION_KSAFE_RUNOPT_INTERFACE)
		return NULL;

	CRunDatQueryEng* eng = new CRunDatQueryEng;
	return (IRunDatQueryEng*)eng;
}

VOID WINAPI pxFreeQueryEng(IRunDatQueryEng* pEng)
{
	CRunDatQueryEng* eng = (CRunDatQueryEng*)pEng;

	if (eng!=NULL)
	{
		delete eng;
	}
}



HRESULT WINAPI CreateUpdateHelper( REFIID riid, void** ppvObj )
{
	if (__uuidof(IUpdateHelper) == riid || IID_IUnknown == riid)
	{
		IUpdateHelper *piUpdateer = new KComObject<CDataUpdate<CLibLoader>>;
		if (NULL == piUpdateer)
			return E_OUTOFMEMORY;

		piUpdateer->AddRef();

		*ppvObj = piUpdateer;

		return S_OK;
	}

	return S_FALSE;
}

};

#ifdef _MANAGED
#pragma managed(pop)
#endif

