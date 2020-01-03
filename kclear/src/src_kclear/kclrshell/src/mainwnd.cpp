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

MainWnd::MainWnd()
    : m_hKClear(NULL)
    , m_piKClear(NULL)
{

}

MainWnd::~MainWnd()
{

}

//////////////////////////////////////////////////////////////////////////

int MainWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    SetWindowPos(NULL, 0, 0, 800, 570, SWP_NOREPOSITION);
    CenterWindow();

    InitKClear();

    return 0;
}

void MainWnd::OnDestroy()
{
    UnInitKClear();
    PostQuitMessage(0);
}

void MainWnd::OnClose()
{
    DestroyWindow();
}

void MainWnd::OnPaint(CDCHandle dc)
{

}

BOOL MainWnd::OnEraseBkgnd(CDCHandle dc)
{
    return TRUE;
}

void MainWnd::OnSize(UINT nType, CSize size)
{
    if (m_wndKClear.m_hWnd)
    {
        m_wndKClear.SetWindowPos(NULL, 0, 0, size.cx, size.cy, 0);
    }
}

//////////////////////////////////////////////////////////////////////////

BOOL MainWnd::InitKClear()
{
    BOOL retval = FALSE;
    CreateKClearObject_t CreateKClearObject = NULL;
    TCHAR szKClearPath[MAX_PATH] = { 0 };
    HRESULT hr;
    HMODULE hKClear = NULL;
    IKClear* piKClear = NULL;
    HWND hWnd = NULL;

    if (!m_hKClear)
    {
        ::GetModuleFileName(NULL, szKClearPath, MAX_PATH);
        PathRemoveFileSpec(szKClearPath);
        PathAppend(szKClearPath, _T("kclear.dll"));
        hKClear = LoadLibrary(szKClearPath);
        if (!hKClear)
            goto clean0;

        m_hKClear = hKClear;
    }

    if (!m_piKClear)
    {
        CreateKClearObject = (CreateKClearObject_t)GetProcAddress(m_hKClear, "CreateKClearObject");
        if (!CreateKClearObject)
            goto clean0;

        hr = CreateKClearObject(__uuidof(IKClear), (void**)&piKClear);
        if (FAILED(hr))
            goto clean0;

        m_piKClear = piKClear;
    }

    if (!m_wndKClear.m_hWnd)
    {
        hWnd = m_piKClear->CreateKClear(m_hWnd);
        m_wndKClear.m_hWnd = hWnd;
        m_wndKClear.ShowWindow(SW_SHOW);
    }

    retval = TRUE;

clean0:
    if (!retval)
    {
        UnInitKClear();
    }

    return retval;
}

void MainWnd::UnInitKClear()
{
    if (m_wndKClear.m_hWnd)
    {
        m_wndKClear.DestroyWindow();
    }

//     if (m_piKClear)
//     {
//         m_piKClear->Release();
//         m_piKClear = NULL;
//     }
// 
//     if (m_hKClear)
//     {
//         FreeLibrary(m_hKClear);
//         m_hKClear = NULL;
//     }
}

//////////////////////////////////////////////////////////////////////////
