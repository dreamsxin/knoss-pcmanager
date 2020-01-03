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


// CContainerPage dialog
#include "dlgmain.h"

class CContainerPage : public CDialogImpl<CContainerPage>
{

public:
	CContainerPage();   // standard constructor
	virtual ~CContainerPage();

// Dialog Data
	enum { IDD = IDD_DIALOG_CONTAINER };


    BEGIN_MSG_MAP(CContainerPage)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_TRASH_SCAN_STATUS_CHANGED, OnScanStatusChanged)
		MESSAGE_HANDLER(WM_TRASH_TIPS_SHOW_STATUS, OnShowTrashTips)
		MESSAGE_HANDLER(WM_TRASH_TIPS_CLOSE_STATUS, OnCloseTrashTips)
        MSG_WM_SIZE(OnSize)
        MSG_WM_CLOSE(OnClose)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
        //MSG_WM_PAINT(OnPaint)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MSG_WM_ERASEBKGND(OnEraseBkgnd)
        MSG_WM_CTLCOLORSTATIC(OnCtlColor)
    END_MSG_MAP()

protected:
     LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
     void OnClose();
     void OnDestroy();
     LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
     LRESULT OnScanStatusChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	 LRESULT OnShowTrashTips(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	 LRESULT OnCloseTrashTips(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
     void OnSize(UINT wParam, const _WTYPES_NS::CSize& size);
	 void OnTimer(UINT_PTR nEventID);
     BOOL OnEraseBkgnd(HDC pDC) ;
     HBRUSH OnCtlColor(HDC hDC, HWND hWnd);

	 void OnMouseLeave();
	 void OnLButtonDown(UINT nFlags, CPoint point);
	 void OnMouseMove(UINT nFlags, CPoint point);


protected:
    CMainDialog m_TrashFileCleanerPage;
    CBrush      m_bkBrush;
    CBitmap     m_bmpCaptionIcon;
    int         m_nScanStatus;
    bool        m_nScanStop;
    bool        m_nClearStop;
    bool        m_nInit;
    CString     m_strJunkFileSize;
    CString     m_last_str;
    int         m_nPos;
    BOOL        m_bIsWindow;
    CImageList m_scanlist;
    MessageP    *m_pass;

	BOOL		m_bShowTips;
	BOOL		m_bTipsIn;
	int			m_nTipsHeight;
	int			m_nTipsCount;
	CRect		m_rcTipsClose;
	CBitmap		m_bmpTipsClose;
protected:
    void        _DrawPromptText(HDC pDC);
    void        __InvalidCount();
	void		_DrawTrashTips(HDC pDC, CRect& rcDraw);
};
