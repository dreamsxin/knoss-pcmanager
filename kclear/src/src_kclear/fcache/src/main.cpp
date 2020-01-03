// Copyright (c) 2010 Kingsoft Corporation. All rights reserved.
// Copyright (c) 2010 The KSafe Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "stdafx.h"
#include "fcacheimp.h"

//////////////////////////////////////////////////////////////////////////

HINSTANCE g_hInstance;

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	g_hInstance = hInstance;
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////

BOOL __cdecl GetFCache(REFIID riid, void** ppvObject, char cVol)
{
    BOOL retval = FALSE;
    CFCacheImpl* pInstance = NULL;
    HRESULT hRetCode;

    pInstance = new CFCacheImpl(cVol);
    if (!pInstance)
        goto clean0;

    hRetCode = pInstance->QueryInterface(riid, ppvObject);
    if (FAILED(hRetCode))
        goto clean0;

    retval = TRUE;

clean0:
    if (!retval)
    {
        if (pInstance)
        {
            delete pInstance;
            pInstance = NULL;
        }
    }

    return retval;
}

//////////////////////////////////////////////////////////////////////////
