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

//////////////////////////////////////////////////////////////////////////

class CBigFileJobLeftDlg 
    : public CKuiDialogImpl<CBigFileJobLeftDlg>
    , public CWHRoundRectFrameHelper<CBigFileJobLeftDlg>
{
public:
    CBigFileJobLeftDlg(BOOL fMove)
        : CKuiDialogImpl<CBigFileJobLeftDlg>("")
    {
        if (fMove)
        {
            SetResId("IDR_DLG_MOVE_LEFT");
        }
        else
        {
            SetResId("IDR_DLG_DEL_LEFT");
        }
    }

    ~CBigFileJobLeftDlg()
    {
    }

    BOOL OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
    {
        return FALSE;
    }

    void OnSysBtnClose()
    {
        EndDialog(0);
    }

    KUI_NOTIFY_MAP(IDC_RICHVIEW_WIN)
        KUI_NOTIFY_ID_COMMAND(IDC_BTN_SYS_CLOSE, OnSysBtnClose)
        KUI_NOTIFY_ID_COMMAND(IDOK, OnSysBtnClose)
    KUI_NOTIFY_MAP_END()

    BEGIN_MSG_MAP_EX(CBigFileJobLeftDlg)
        MSG_KUI_NOTIFY(IDC_RICHVIEW_WIN)
        CHAIN_MSG_MAP(CKuiDialogImpl<CBigFileJobLeftDlg>)
        CHAIN_MSG_MAP(CWHRoundRectFrameHelper<CBigFileJobLeftDlg>)
        MSG_WM_INITDIALOG(OnInitDialog)
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()
};

//////////////////////////////////////////////////////////////////////////
