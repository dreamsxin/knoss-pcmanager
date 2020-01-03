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
#include "kclearimpl.h"

//////////////////////////////////////////////////////////////////////////

CKClearImpl::CKClearImpl() : m_pDlgMain(NULL)
{

}

CKClearImpl::~CKClearImpl()
{

}

//////////////////////////////////////////////////////////////////////////

void CKClearImpl::Release()
{
    delete this;
}

HWND CKClearImpl::CreateKClear(HWND hParent)
{
    HWND retval = NULL;

    if (m_pDlgMain)
    {
        retval = m_pDlgMain->GetParent();
        goto clean0;
    }

    m_pDlgMain = new CKscMainDlg();
    if (!m_pDlgMain)
        goto clean0;

    retval = m_pDlgMain->Create(hParent);

clean0:
    return retval;
}

//////////////////////////////////////////////////////////////////////////

BOOL CKClearImpl::Navigate(const wchar_t* szNavigate)
{
    BOOL retval = FALSE;

    if (!szNavigate)
        goto clean0;

    if (!m_pDlgMain)
        goto clean0;

    return m_pDlgMain->OnNavigate(szNavigate);

clean0:
    return retval;
}

//////////////////////////////////////////////////////////////////////////
/*
void CKClearImpl::OnSize(const RECT* lprcWnd)
{
    if (!lprcWnd)
        goto clean0;

    if (!m_pDlgMain)
        goto clean0;

    m_pDlgMain->SetWindowPos(NULL, lprcWnd, 0);

clean0:
    return;
}
*/
//////////////////////////////////////////////////////////////////////////
