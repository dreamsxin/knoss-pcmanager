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

#include <atlcoll.h>
#include <wtlhelper/whwindow.h>
#include "kuires.h"
#include "trashpublicfunc.h"
#include "kscbase/ksclock.h"

//////////////////////////////////////////////////////////////////////////


class CTrashDoingDlg 
	: public CKuiDialogImpl<CTrashDoingDlg>
	, public CWHRoundRectFrameHelper<CTrashDoingDlg>
{
public:
	CTrashDoingDlg()
		: CKuiDialogImpl<CTrashDoingDlg>("IDR_DLG_TRASH_DOING")
	{
		m_nPos = 1;
		m_strFileDel = L"";
		m_strFileRemaining = L"";
		m_strFilePos = L"";
		m_bDlgBegin = FALSE;
	}

	~CTrashDoingDlg()
	{
		
	}

	BOOL OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
	{
		CString strTitle;
		LPCTSTR strSize;
		strSize = GetFileSizeText(m_llTotalSize);
		strTitle.Format(_T("共计%d个垃圾文件（%s）"), m_nFileCount, strSize);
		SetItemText(101, strTitle);
		::SetTimer(this->m_hWnd, 0, 10, NULL);
		m_bStop = FALSE;

		m_bDlgBegin = TRUE;
		return FALSE;
	}
	void OnDestroy()
	{
		::KillTimer(this->m_hWnd, 0);
	}

	void Init(int nCount, ULONGLONG llSize)
	{
		m_nFileCount = nCount;
		m_llTotalSize = llSize;
	}
	BOOL IsStop()
	{
		return m_bStop;
	}

	void OnSysBtnClose()
	{
		OnCancel();
		EndDialog(0);
	}

	void SetFilePos(const CString strFilePos)
	{
		KAutoLock lock(m_lock);
		m_strFilePos = strFilePos;
	}

	void SetFileDel(const CString strFileDel)
	{
		KAutoLock lock(m_lock);
		m_strFileDel = strFileDel;
	}

	void SetProcessPos(const int nPos)
	{
		KAutoLock lock(m_lock);
		m_nPos = nPos;
	}

	void SetFileRemaining(const CString strFileRemaining)
	{
		KAutoLock lock(m_lock);
		m_strFileRemaining = strFileRemaining;
	}

	void OnTimer(UINT_PTR nIDEvent)
	{
		CString strFilePos;
		CString strFileDel;
		CString strFileRemaining;

		strFilePos = _T("文件位置：");
		strFilePos += m_strFilePos;
		strFileDel = _T("正在清理：");
		strFileDel += m_strFileDel;
		strFileRemaining = _T("剩余项：");
		strFileRemaining += m_strFileRemaining;

		if (strFileDel.GetLength() > 40)
		{
			strFileDel = strFileDel.Left(37);
			strFileDel += L"...";
		}

		SetItemText(102, strFilePos);
		SetItemText(103, strFileDel);
		SetItemText(105, strFileRemaining);
		SetItemIntAttribute(104, "value", m_nPos);
		if( m_nPos >= 100 || m_bStop)
		{
			OnSysBtnClose();
		}
	}
	void OnCancel()
	{
		m_bStop = TRUE;
	}

	BOOL IsWindowExist()
	{
		if (m_hWnd != NULL && m_bDlgBegin)
		{
			return TRUE;
		}
		return FALSE;
	}

	KUI_NOTIFY_MAP(IDC_RICHVIEW_WIN)
		KUI_NOTIFY_ID_COMMAND(IDC_BTN_SYS_CLOSE, OnSysBtnClose)
		KUI_NOTIFY_ID_COMMAND(106, OnCancel)
		KUI_NOTIFY_MAP_END()

		BEGIN_MSG_MAP_EX(CTrashDoingDlg)
		MSG_KUI_NOTIFY(IDC_RICHVIEW_WIN)
		CHAIN_MSG_MAP(CKuiDialogImpl<CTrashDoingDlg>)
		CHAIN_MSG_MAP(CWHRoundRectFrameHelper<CTrashDoingDlg>)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_DESTROY(OnDestroy)
		REFLECT_NOTIFICATIONS_EX()
		END_MSG_MAP()

public:

protected:
	int m_nFileCount;
	ULONGLONG m_llTotalSize;	
	CString m_strFilePos;
	CString m_strFileDel;
	CString m_strFileRemaining;
	int m_nPos;
	BOOL m_bStop;
	KLock m_lock;
	BOOL m_bDlgBegin;
};

//////////////////////////////////////////////////////////////////////////
