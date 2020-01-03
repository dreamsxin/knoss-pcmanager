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

#ifndef KCLRSHELL_MAINWND_H_
#define KCLRSHELL_MAINWND_H_

//////////////////////////////////////////////////////////////////////////

#include "kclear/ikclear.h"

//////////////////////////////////////////////////////////////////////////

class MainWnd: public CWindowImpl<MainWnd, CWindow, CFrameWinTraits>
{
public:
    DECLARE_WND_CLASS(_T("KClrShell"))

    MainWnd();
    ~MainWnd();

    BEGIN_MSG_MAP_EX(MainWnd)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_CLOSE(OnClose)
        //MSG_WM_PAINT(OnPaint)
        //MSG_WM_ERASEBKGND(OnEraseBkgnd)
        MSG_WM_SIZE(OnSize)
    END_MSG_MAP()

    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnDestroy();
    void OnClose();
    void OnPaint(CDCHandle dc);
    BOOL OnEraseBkgnd(CDCHandle dc);
    void OnSize(UINT nType, CSize size);

protected:
    BOOL InitKClear();
    void UnInitKClear();

private:
    HMODULE     m_hKClear;
    IKClear*    m_piKClear;
    CWindow     m_wndKClear;
};

//////////////////////////////////////////////////////////////////////////

#endif // 