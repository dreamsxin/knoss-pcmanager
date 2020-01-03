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
#include "kclrshell.h"
#include "mainwnd.h"

//////////////////////////////////////////////////////////////////////////

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
    HRESULT hRet = E_FAIL;

    hRet = _Module.Init(hInstance);
    if (FAILED(hRet))
        return -1;

    ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    _Module.Main();

    ::CoUninitialize();

    return 0;
}

//////////////////////////////////////////////////////////////////////////

KClrShellModule _Module;

//////////////////////////////////////////////////////////////////////////

KClrShellModule::KClrShellModule() 
{

}

KClrShellModule::~KClrShellModule() 
{

}

//////////////////////////////////////////////////////////////////////////

HRESULT KClrShellModule::Init(HINSTANCE hInstance)
{
    HRESULT hRet = __super::Init(NULL, hInstance);
    if (FAILED(hRet))
        return hRet;

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiToken;

    GdiplusStartup(&gdiToken, &gdiplusStartupInput, NULL);

    return S_OK;
}

UINT_PTR KClrShellModule::Main()
{
    CMessageLoop theLoop;
    MainWnd mainwnd;

    _Module.AddMessageLoop(&theLoop);

    if (mainwnd.Create(NULL, CWindow::rcDefault, _T("KClear")) == NULL) 
    {
        return 0;
    }

    mainwnd.ShowWindow(SW_SHOW);

    int nRet = theLoop.Run();

    _Module.RemoveMessageLoop();

    return nRet;
}

//////////////////////////////////////////////////////////////////////////
