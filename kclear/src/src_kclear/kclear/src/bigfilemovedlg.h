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


class CBigFileMoveDlg 
    : public CKuiDialogImpl<CBigFileMoveDlg>
    , public CWHRoundRectFrameHelper<CBigFileMoveDlg>
{
public:
    CBigFileMoveDlg(
        const CString& strSize, 
        const CString& strCount, 
        const CString& strDestDir,
        const CString& strFileName = _T("")
        )
        : CKuiDialogImpl<CBigFileMoveDlg>("IDR_DLG_MOVE")
        , m_strSize(strSize)
        , m_strCount(strCount)
        , m_strDestDir(strDestDir)
        , m_strFileName(strFileName)
    {
        
    }

    ~CBigFileMoveDlg()
    {
    }

    BOOL OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
    {
        // 创建Edit控件
        DWORD dwEditFlags = WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY;
        m_ctlDestDir.Create(GetViewHWND(), NULL, NULL, dwEditFlags, WS_EX_NOPARENTNOTIFY, 102, NULL);
        m_ctlDestDir.SetFont(KuiFontPool::GetFont(FALSE, FALSE, FALSE));
        m_ctlDestDir.SetWindowText(m_strDestDir);

        if (m_strFileName.GetLength())
        {
            m_ctlFileName.Create(GetViewHWND(), NULL, NULL, dwEditFlags, WS_EX_NOPARENTNOTIFY, 202, NULL);
            m_ctlFileName.SetFont(KuiFontPool::GetFont(FALSE, FALSE, FALSE));
            m_ctlFileName.SetWindowText(m_strFileName);

            SetItemText(203, m_strSize);
            SetItemVisible(200, FALSE);
            SetItemVisible(201, TRUE);
        }
        else
        {
            SetItemText(100, m_strSize);
            SetItemText(101, m_strCount);
            SetItemVisible(200, TRUE);
            SetItemVisible(201, FALSE);
        }

        if (IsGoodDestDir(m_strDestDir))
        {
            EnableItem(IDOK, TRUE);
            SetItemVisible(104, FALSE);
        }
        else
        {
            EnableItem(IDOK, FALSE);
            SetItemVisible(104, TRUE);
        }

        return FALSE;
    }

    void OnCancel()
    {
        EndDialog(IDCANCEL);
    }

    void OnOK()
    {
        EndDialog(IDOK);
    }

    void OnEditCtlReSize(CRect rcWnd)
    {
        if (m_ctlDestDir.m_hWnd)
            m_ctlDestDir.SetWindowPos(NULL, rcWnd, 0);
    }

    void OnFileNameCtlReSize(CRect rcWnd)
    {
        if (m_ctlFileName.m_hWnd)
            m_ctlFileName.SetWindowPos(NULL, rcWnd, 0);
    }

    static BOOL _SelectFolder(CString &strPath, HWND hWnd)
    {
        KAppRes& appRes = KAppRes::Instance();
        BROWSEINFO bInfo = {0};
        bInfo.hwndOwner = hWnd;
        bInfo.lpszTitle = appRes.GetString("IDS_SET_DEST_DIR_TITLE");
        bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_STATUSTEXT | BIF_DONTGOBELOWDOMAIN;

        LPITEMIDLIST lpDlist;
        lpDlist = SHBrowseForFolder(&bInfo);
        if(lpDlist==NULL)
            return FALSE;

        TCHAR szPath[MAX_PATH] = {0};
        SHGetPathFromIDList(lpDlist, szPath );
        strPath = szPath;
        return TRUE;
    }

    BOOL IsGoodDestDir(const CString& strDir)
    {
        BOOL retval = FALSE;
        TCHAR szSysDir[MAX_PATH] = { 0 };
        CString strSysDir;
        CString _strDir = strDir;
        CString _drv;
        UINT uType;

        GetSystemDirectory(szSysDir, MAX_PATH);
        strSysDir = szSysDir;
        strSysDir.MakeLower();
        _strDir.MakeLower();

        if (_strDir[0] == _T('a') ||
            _strDir[0] == _T('b'))
        {
            goto clean0;
        }

        _drv = _strDir.Left(3);

        uType = GetDriveType(_drv);
        if (uType != DRIVE_FIXED &&
            uType != DRIVE_REMOVABLE)
        {
            goto clean0;
        }

        if (_strDir[0] == strSysDir[0])
            goto clean0;

        retval = TRUE;

clean0:
        return retval;
    }

    void OnChangeDestDir()
    {
        CString strDestDir;
        if (!_SelectFolder(strDestDir, GetRichWnd()))
            return;

        if (strDestDir.GetLength() < 3)
            return;

        if (IsGoodDestDir(strDestDir))
        {
            EnableItem(IDOK, TRUE);
            SetItemVisible(104, FALSE);
        }
        else
        {
            EnableItem(IDOK, FALSE);
            SetItemVisible(104, TRUE);
        }

        m_ctlDestDir.SetWindowText(strDestDir);
		m_strDestDir = strDestDir;
    }

    CString GetDestDir()
    {
        return m_strDestDir;
    }

    KUI_NOTIFY_MAP(IDC_RICHVIEW_WIN)
        KUI_NOTIFY_ID_COMMAND(IDC_BTN_SYS_CLOSE, OnCancel)
        KUI_NOTIFY_ID_COMMAND(IDCANCEL, OnCancel)
        KUI_NOTIFY_ID_COMMAND(IDOK, OnOK)
        KUI_NOTIFY_ID_COMMAND(103, OnChangeDestDir)
        KUI_NOTIFY_REALWND_RESIZED(102, OnEditCtlReSize)
        KUI_NOTIFY_REALWND_RESIZED(202, OnFileNameCtlReSize)
    KUI_NOTIFY_MAP_END()

    BEGIN_MSG_MAP_EX(CBigFileMoveDlg)
        MSG_KUI_NOTIFY(IDC_RICHVIEW_WIN)
        CHAIN_MSG_MAP(CKuiDialogImpl<CBigFileMoveDlg>)
        CHAIN_MSG_MAP(CWHRoundRectFrameHelper<CBigFileMoveDlg>)
        MSG_WM_INITDIALOG(OnInitDialog)
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()

protected:
    CString m_strSize;
    CString m_strCount;
    CString m_strDestDir;
    CString m_strFileName;  // 单文件的文件名
    CEdit m_ctlDestDir;
    CEdit m_ctlFileName;
};

//////////////////////////////////////////////////////////////////////////
