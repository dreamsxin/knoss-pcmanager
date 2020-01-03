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

#pragma once
#include "kscviewlog.h"
//////////////////////////////////////////////////////////////////////////

class CBigFileDelLeftDlg 
    : public CKuiDialogImpl<CBigFileDelLeftDlg>
    , public CWHRoundRectFrameHelper<CBigFileDelLeftDlg>
{
public:
    CBigFileDelLeftDlg(LPCTSTR strName, LPCTSTR strTitle, LPCTSTR strText, int nFlag)
        : CKuiDialogImpl<CBigFileDelLeftDlg>("")
    {
		if (nFlag == 0)
		{
			SetResId("IDR_DLG_MOVE_LEFT");
		}
		else
		{
			SetResId("IDR_DLG_DEL_LEFT");
		}
		m_strName	= strName;
		m_strTitle  = strTitle;
		m_strText   = strText;
    }

    ~CBigFileDelLeftDlg()
    {
    }

    BOOL OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
    {
		SetItemVisible(101, TRUE);

        return FALSE;
    }

    void OnSysBtnClose()
    {
        EndDialog(0);
    }

	void OnShowDetail()
	{
		CKscViewLog dlg(m_strText,m_strName, m_strTitle);
		dlg.DoModal();
		return;
	}

    KUI_NOTIFY_MAP(IDC_RICHVIEW_WIN)
        KUI_NOTIFY_ID_COMMAND(IDC_BTN_SYS_CLOSE, OnSysBtnClose)
        KUI_NOTIFY_ID_COMMAND(IDOK, OnSysBtnClose)
		KUI_NOTIFY_ID_COMMAND(101, OnShowDetail)
    KUI_NOTIFY_MAP_END()

    BEGIN_MSG_MAP_EX(CBigFileDelLeftDlg)
        MSG_KUI_NOTIFY(IDC_RICHVIEW_WIN)
        CHAIN_MSG_MAP(CKuiDialogImpl<CBigFileDelLeftDlg>)
        CHAIN_MSG_MAP(CWHRoundRectFrameHelper<CBigFileDelLeftDlg>)
        MSG_WM_INITDIALOG(OnInitDialog)
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()


protected:
	CString					m_strText;
	CString					m_strName;
	CString                 m_strTitle;

};

//////////////////////////////////////////////////////////////////////////
