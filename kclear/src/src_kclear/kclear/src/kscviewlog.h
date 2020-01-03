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
#include <wtlhelper/whwindow.h>
#include "kuires.h"

class CKscViewLog
	: public CKuiDialogImpl<CKscViewLog>
	, public CWHRoundRectFrameHelper<CKscViewLog>
{
public:
	CKscViewLog(LPCTSTR strName, LPCTSTR strTitle, LPCTSTR strText)
		: CKuiDialogImpl<CKscViewLog>("IDR_DLG_VIEWLOG")
	{
		m_strName	= strName;
		m_strTitle  = strTitle;
		m_strText   = strText;
	}

	~CKscViewLog(void){}

protected:
	CRichEditCtrl			m_editView;
	CFont					m_fntText;
	CString					m_strText;
	CString					m_strName;
	CString                 m_strTitle;

public:

	void CopyDataToClipboard(LPCTSTR lpData )
	{
		HGLOBAL hglbCopy; 
		LPTSTR  lptstrCopy; 

		if( OpenClipboard() )
		{
			EmptyClipboard();

			hglbCopy = GlobalAlloc( GMEM_MOVEABLE, (_tcslen(lpData)+1)*sizeof(TCHAR)); 
			if( hglbCopy ) 
			{ 
				lptstrCopy = (LPTSTR) GlobalLock(hglbCopy); 
				_tcscpy( lptstrCopy, lpData );
				GlobalUnlock(hglbCopy); 
#ifdef _UNICODE
				SetClipboardData(CF_UNICODETEXT, hglbCopy); 
#else
				SetClipboardData(CF_TEXT, hglbCopy); 	
#endif
			} 
			CloseClipboard(); 
		}
	}

	void OnBkBtnClose()
	{
		EndDialog(0);
	}
	void OnBkBtnCopy()
	{
		CopyDataToClipboard(m_strText);

		CString strText;
		/*strText.Format(BkString::Get(IDS_PLUGIN_2048));
		CBkSafeMsgBox::Show( strText, NULL, MB_ICONINFORMATION|MB_OK);*/
	}
	void OnBkBtnClear()
	{
		return;
	}

	BOOL OnInitDialog(HWND wParam, LPARAM lParam)
	{
 		m_editView.Create( GetViewHWND(), NULL, NULL, 
 			WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL|ES_AUTOHSCROLL|ES_MULTILINE|ES_WANTRETURN|WS_TABSTOP|ES_READONLY, 
 			0, IDC_ONEKEY_DETAIL_DLG_EDIT);

		m_editView.SetWindowText(m_strText);
		SetItemText(IDC_BTN_ONEKEY_LOG_TITEL, m_strName);
		
		SetItemText(IDC_TXT_ONEKEY_LOG_DETAIL, m_strTitle);

		m_fntText.CreateFont(13,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY ,DEFAULT_PITCH, _T("ËÎÌו"));
		m_editView.SetFont(m_fntText);

		CString		strX;
		/*strX.Format(BkString::Get(IDS_PLUGIN_2049),m_strName);*/
//		SetItemText(IDC_TXT_PLUG_DETAIL_NAME,strX);

		return TRUE;
	}

public:
	KUI_NOTIFY_MAP(IDC_RICHVIEW_WIN)
		KUI_NOTIFY_ID_COMMAND(IDC_BTN_ONEKEY_LOG_CLOSE, OnBkBtnClose)
		KUI_NOTIFY_ID_COMMAND(IDC_BTN_SYS_CLOSE, OnBkBtnClose)
		KUI_NOTIFY_ID_COMMAND(IDC_TXT_ONEKEY_LOG_COPY, OnBkBtnCopy)
	KUI_NOTIFY_MAP_END()

	BEGIN_MSG_MAP_EX(CKscViewLog)
		MSG_KUI_NOTIFY(IDC_RICHVIEW_WIN)
		CHAIN_MSG_MAP(CKuiDialogImpl<CKscViewLog>)
		CHAIN_MSG_MAP(CWHRoundRectFrameHelper<CKscViewLog>)
		MSG_WM_INITDIALOG(OnInitDialog)
	END_MSG_MAP()
};
