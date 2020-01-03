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

// FileListTipHelp.cpp : implementation file
//

#include "stdafx.h"
#include "trashcleaner.h"
#include "filelisttiphelp.h"

/////////////////////////////////////////////////////////////////////////////
// CFileListTipHelp



CFileListTipHelp::CFileListTipHelp()
{
   
}

CFileListTipHelp::~CFileListTipHelp()
{
}


// CFileListTipHelp message handlers
void CFileListTipHelp::InitToolButton()
{
    CRect rect(0, 0, 20, 20);

    m_button.Create(m_hWnd,rect,NULL,BS_OWNERDRAW | BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE);
    m_button.SetBitmapButtonExtendedStyle(BMPBTN_HOVER);
    m_button.SetBtnImages(IDB_BUTTON, IDB_BUTTONSEL,IDB_BUTTONSEL ,TRUE);
    m_button.SetImages(0, 1, 2);

    SizeButton();	
}

BOOL CFileListTipHelp::SaveIsShowHelp(BOOL bShow)
{
    BOOL bResult = FALSE;
    HKEY hRegKey = NULL;
    
    do {

        LONG  lRet = 0;
        DWORD dwWrite = bShow;
        
        lRet = RegOpenKeyEx(HKEY_CURRENT_USER, 
            IDS_SUB_KEY,
            0,
            KEY_WRITE,
            &hRegKey);

        if (lRet != ERROR_SUCCESS)
            break;

        lRet = RegSetValueEx(hRegKey, 
            IDS_VALUENAME,
            0,
            REG_DWORD,
            (LPBYTE)&dwWrite,
            sizeof(dwWrite) );

        if (lRet != ERROR_SUCCESS)
            break;

        bResult = TRUE;

    } while(0);

    if (hRegKey != NULL)
        RegCloseKey(hRegKey);

    return bResult;
}

BOOL CFileListTipHelp::IsShowHelp()
{
    BOOL bResult = TRUE;
    HKEY hRegKey = NULL;
    
    do {

        LONG  lRet       = 0;
        DWORD dwType     = REG_DWORD;

        DWORD dwRet  = 0;
        DWORD dwSize = sizeof(dwRet);


        lRet = RegOpenKeyEx(HKEY_CURRENT_USER, 
            IDS_SUB_KEY,
            0,
            KEY_READ,
            &hRegKey);

        if (lRet != ERROR_SUCCESS)
            break;

        lRet = RegQueryValueEx(hRegKey, 
            IDS_VALUENAME,
            0,
            &dwType,
            (LPBYTE)&dwRet,
            &dwSize);

        if (lRet != ERROR_SUCCESS)
            break;

        bResult = dwRet;

    } while(0);

    if (hRegKey != NULL)
        RegCloseKey(hRegKey);

    return bResult;
}



void CFileListTipHelp::OnPaint(HDC pdc) 
{
	CPaintDC dc(m_hWnd);
	
    COLORREF clrBorder = RGB(165, 165, 165);
    COLORREF clrBkGnd  = RGB(255, 255, 200);
    COLORREF clrText   = clrBorder; //RGB(165, 165, 165);

    CRect   rcClient;
    CString strCaption;

    GetClientRect(&rcClient);

    dc.FillSolidRect(&rcClient, clrBkGnd);
    dc.Draw3dRect(&rcClient, clrBorder, clrBorder);
    
    GetWindowText(strCaption);
    
    int nSaveID = dc.SaveDC();
    
    dc.SetBkMode(TRANSPARENT);
    SelectObject((HDC)dc,GetFont());
    dc.SetTextColor(clrText);

    CRect rcText = rcClient;

    rcText.left  += 5;
    rcText.right -= 15;

    dc.DrawText(strCaption,strCaption.GetLength(), &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    dc.RestoreDC(nSaveID);
}


void CFileListTipHelp::OnSize(UINT nType, CSize size) 
{
    SizeButton();	
}

void CFileListTipHelp::SizeButton()
{
    if ( m_button.m_hWnd)
    {
        CRect rcClient;

        GetClientRect(rcClient);

        m_button.SetWindowPos( NULL, rcClient.right - 15, rcClient.top + 6, 0, 0, 
            SWP_NOSIZE | SWP_NOACTIVATE);
    }
}

LRESULT CFileListTipHelp::OnCloseWindow(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
    SaveIsShowHelp(FALSE);

    HWND pParent = GetParent();

	DestroyWindow();    

    if (pParent)
        SendMessage(pParent,WM_SIZE,0,0);

    return S_OK;
}
