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

#include <wtlhelper/kuifolderbrowsectrl.h>

class CKscFolderBrowseDlg
    : public CKuiDialogImpl<CKscFolderBrowseDlg>
    , public CWHRoundRectFrameHelper<CKscFolderBrowseDlg>
{
public:
    CKscFolderBrowseDlg()
        : CKuiDialogImpl<CKscFolderBrowseDlg>("IDR_DLG_DEST_FOLDER_BROWSE")
        , m_bNeverShowed(TRUE)
    {
    }

    CAtlList<CString>& GetSelectedPathes()
    {
        return m_lstSelectedPathes;
    }

    void EndDialog(UINT uRetCode)
    {
        if (IDOK == uRetCode)
            m_wndFolderTreeCtrl.GetSelectedPathes(m_lstSelectedPathes);

        __super::EndDialog(uRetCode);
    }

protected:

    BOOL m_bNeverShowed;

    CKuiFolderBrowseCtrl m_wndFolderTreeCtrl;

    CAtlList<CString> m_lstSelectedPathes;

    void OnBkBtnClose()
    {
        EndDialog(IDCANCEL);
    }

    void OnOK()
    {
        if (m_wndFolderTreeCtrl.HasCheckedItem())
            EndDialog(IDOK);
    }

    BOOL OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
    {
        m_wndFolderTreeCtrl.Create(GetViewHWND(), IDC_TRV_FOLDER_BROWSE, WS_CHILD | WS_VISIBLE);

        m_wndFolderTreeCtrl.SetFocus();

        EnableItem(IDOK, FALSE);

        return TRUE;
    }

    void OnShowWindow(BOOL bShow, UINT nStatus)
    {
        if (bShow && m_bNeverShowed)
        {
            m_bNeverShowed = FALSE;
        }
    }

    LRESULT OnTreeFolderBrowseItemCheckChange(LPNMHDR pnmh)
    {
        EnableItem(IDOK, m_wndFolderTreeCtrl.HasCheckedItem());

        return 0;
    }

public:

    KUI_NOTIFY_MAP(IDC_RICHVIEW_WIN)
        KUI_NOTIFY_ID_COMMAND(IDC_BTN_SYS_CLOSE, OnBkBtnClose)
        KUI_NOTIFY_ID_COMMAND(IDOK, OnOK)
        KUI_NOTIFY_ID_COMMAND(IDCANCEL, OnBkBtnClose)
    KUI_NOTIFY_MAP_END()

	BEGIN_MSG_MAP_EX(CKscFolderBrowseDlg)
        MSG_KUI_NOTIFY(IDC_RICHVIEW_WIN)
        CHAIN_MSG_MAP(CKuiDialogImpl<CKscFolderBrowseDlg>)
        CHAIN_MSG_MAP(CWHRoundRectFrameHelper<CKscFolderBrowseDlg>)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_SHOWWINDOW(OnShowWindow)
        NOTIFY_HANDLER_EX(IDC_TRV_FOLDER_BROWSE, WH3STVN_ITEMCHECKCHANGE, OnTreeFolderBrowseItemCheckChange)
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()
};