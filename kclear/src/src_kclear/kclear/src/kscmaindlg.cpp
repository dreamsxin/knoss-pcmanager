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
#include "kscmaindlg.h"
#include "kscbase/kscres.h"
#include "kscbase/kscfilepath.h"
#include "kscbase/kscconf.h"
#include "kuiwin/kuimsgbox.h"
#include "kclearnavigate.h"
#include "../../publish/bksafe/bksafenavigatestring.h"

//////////////////////////////////////////////////////////////////////////

CKscMainDlg::CKscMainDlg() 
    : CKuiDialogImpl<CKscMainDlg>("IDR_DLG_CLEAR")
    , m_handlerOpt(this)
    , m_handlerOnekey(this)
    , m_handlerBigFile(this)
{
}

CKscMainDlg::~CKscMainDlg()
{
}

//////////////////////////////////////////////////////////////////////////

typedef void (__cdecl *SetCareWindow_t)(HWND hWnd);

//////////////////////////////////////////////////////////////////////////

BOOL CKscMainDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{
	CString	str;
	KAppRes& appRes = KAppRes::Instance();

    SetIcon(appRes.GetIcon("IDI_CLEAR_ICON_BIG"), TRUE);
    SetIcon(appRes.GetIcon("IDI_CLEAR_ICON_SMALL"), FALSE);

	RECT rcTest = { 1, 1, 20, 20 };
	m_wndToolVector.Create(GetRichWnd(), &rcTest, NULL, 0, 0, 3888);

	m_wndToolVector.InitTools();
	SetTabCurSel(IDC_TAB_MAIN, 0);

    m_handlerOpt.Init();
    m_handlerOnekey.Init();
    m_handlerBigFile.Init();

	SetItemText(IDC_TXT_PRODUCT_VERSION, _Module.GetAppFileVersion());

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

BOOL CKscMainDlg::OnTabMainSelChange(int nTabItemIDOld, int nTabItemIDNew)
{
    if (4 == nTabItemIDNew)
    {
        m_handlerBigFile.Show();
    }

	return m_wndToolVector.SwitchTool(nTabItemIDNew);
}

void CKscMainDlg::OnSize(UINT nType, CSize size)
{
	UpdateToolSize();
}

//////////////////////////////////////////////////////////////////////////

BOOL CKscMainDlg::UpdateToolSize()
{
	BOOL retval = FALSE;
	CRect rcTool;

	if (!m_wndToolVector.m_hWnd)
		goto clean0;

	GetClientRect(rcTool);
	//rcTool.left += 7;
	rcTool.top += 28;
	//rcTool.right -= 7;
	rcTool.bottom -= 0;
	m_wndToolVector.SetWindowPos(HWND_TOP, &rcTool, 0);

	retval = TRUE;

clean0:
	return retval;
}

//////////////////////////////////////////////////////////////////////////

BOOL CKscMainDlg::OnNavigate(const CString& strNavigate)
{
    BOOL retval = FALSE;
    int nTabIndex = -1;

    if (!strNavigate.CompareNoCase(KCLEARNS_ONEKEY) ||
        !strNavigate.CompareNoCase(BKSFNS_SYSOPT_CLR_ONEKEY))
    {
        nTabIndex = 0;
    }
    else if (!strNavigate.CompareNoCase(KCLEARNS_TRASHCLEANER) ||
             !strNavigate.CompareNoCase(BKSFNS_SYSOPT_CLR_RUBBISH))
    {
        nTabIndex = 1;
    }
    else if (!strNavigate.CompareNoCase(KCLEARNS_TRACKCLEANER) ||
             !strNavigate.CompareNoCase(BKSFNS_SYSOPT_CLR_HENJI))
    {
        nTabIndex = 2;
    }
    else if (!strNavigate.CompareNoCase(KCLEARNS_REGCLEANER) ||
             !strNavigate.CompareNoCase(BKSFNS_SYSOPT_CLR_REG))
    {
        nTabIndex = 3;
    }
    else if (!strNavigate.CompareNoCase(KCLEARNS_BIGCLEANER) ||
             !strNavigate.CompareNoCase(BKSFNS_SYSOPT_CLR_SHOUSHEN))
    {
        nTabIndex = 4;
    }

    if (-1 == nTabIndex)
        goto clean0;

    retval = SetTabCurSel(IDC_TAB_MAIN, nTabIndex);

clean0:
    return retval;
}

void CKscMainDlg::OnDestroy()
{
    m_handlerOnekey.UnInit();
    m_handlerBigFile.UnInit();
	m_handlerOpt.UnInit();
}

//////////////////////////////////////////////////////////////////////////