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

// ContainerPage.cpp : implementation file
//

#include "stdafx.h"
#include "trashcleaner.h"
#include "dlgcontainer.h"
#include "wtlctrls/kdraw.h"

CContainerPage::CContainerPage()

{
    m_nScanStatus = 0;
    m_nScanStop = false;
    m_nPos = 10;
    m_nClearStop = false;
    m_bIsWindow = FALSE;
    m_nInit= false;
    m_last_str = _T("");
	m_bShowTips = FALSE;
	m_bTipsIn = FALSE;
}

CContainerPage::~CContainerPage()
{
}


LRESULT	CContainerPage::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    
    m_bIsWindow = TRUE;
    CRect rctmpClient;
    CBitmap bmp0,bmp1,bmp2,bmp3,bmp4,bmp5,bmp6,bmp7;
    GetClientRect(rctmpClient);//0,395,0,785

    RECT rcClient = { rctmpClient.left+1, rctmpClient.top+32, rctmpClient.right, rctmpClient.bottom};
    RECT rcPro = { rctmpClient.left+1, rctmpClient.top+8,rctmpClient.right-185, rctmpClient.top+8+20 };
   
    m_bkBrush.CreateSolidBrush(RGB(255,255,255));
    if (!m_TrashFileCleanerPage.Create(m_hWnd))
    {
     
        goto Exit0;
    }

    m_TrashFileCleanerPage.ShowWindow(SW_HIDE);
    m_TrashFileCleanerPage.MoveWindow(&rcClient);
    m_TrashFileCleanerPage.ShowWindow(SW_SHOW);

    m_scanlist.Create(16, 16, ILC_COLOR8 | ILC_MASK, 8, 0);

    bmp0.LoadBitmap(IDB_BMP_LOAD1);
    bmp1.LoadBitmap(IDB_BMP_LOAD2);
    bmp2.LoadBitmap(IDB_BMP_LOAD3);
    bmp3.LoadBitmap(IDB_BMP_LOAD4);
    bmp4.LoadBitmap(IDB_BMP_LOAD5);
    bmp5.LoadBitmap(IDB_BMP_LOAD6);
    bmp6.LoadBitmap(IDB_BMP_LOAD7);
    bmp7.LoadBitmap(IDB_BMP_LOAD8);

    m_scanlist.Add((HBITMAP)bmp0, RGB(255, 255, 255));
    m_scanlist.Add((HBITMAP)bmp1, RGB(255, 255, 255));
    m_scanlist.Add((HBITMAP)bmp2, RGB(255, 255, 255));
    m_scanlist.Add((HBITMAP)bmp3, RGB(255, 255, 255));
    m_scanlist.Add((HBITMAP)bmp4, RGB(255, 255, 255));
    m_scanlist.Add((HBITMAP)bmp5, RGB(255, 255, 255));
    m_scanlist.Add((HBITMAP)bmp6, RGB(255, 255, 255));
    m_scanlist.Add((HBITMAP)bmp7, RGB(255, 255, 255));
    
	m_bmpTipsClose.LoadBitmap(IDB_BMP_TIPS_CLOSE);
Exit0:
    return S_OK;
}




// CContainerPage message handlers

void CContainerPage::OnClose()
{
    if (m_TrashFileCleanerPage.m_hWnd)
        m_TrashFileCleanerPage.DestroyWindow();
}

void CContainerPage::OnDestroy()
{
    if (m_TrashFileCleanerPage)
        m_TrashFileCleanerPage.DestroyWindow();
}
 
 HBRUSH CContainerPage::OnCtlColor(HDC hDC, HWND hWnd)
 {
       return (HBRUSH)m_bkBrush;
 }

void CContainerPage::OnMouseLeave()
{
	SetMsgHandled(FALSE);
	if (m_bShowTips)
	{
		m_bTipsIn = FALSE;
	}
}
void CContainerPage::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);
	if (m_bShowTips)
	{
		if (m_rcTipsClose.PtInRect(point))
		{
			SendMessage(WM_TRASH_TIPS_CLOSE_STATUS, NULL, NULL);
		}
	}
}
void CContainerPage::OnMouseMove(UINT nFlags, CPoint point)
{	
	if (m_bShowTips)
	{
		if (m_rcTipsClose.PtInRect(point))
		{
			m_bTipsIn = TRUE;
			InvalidateRect(&m_rcTipsClose);
		}
		else
		{
			if (m_bTipsIn)
			{
				m_bTipsIn = FALSE;
				InvalidateRect(&m_rcTipsClose);
			}
		}
	}
	SetMsgHandled(FALSE);
}

LRESULT CContainerPage::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
   
    CPaintDC	hdc(m_hWnd);
    CRect rcClient;
    CBitmap bmpIcon;
    static int m_nCurrentPage = 0;
    bHandled  = TRUE;
   

    GetClientRect(rcClient);

    HDC dcMem;
    dcMem = ::CreateCompatibleDC(hdc); //创建与视图的设备相兼容的内存设备
    CBitmap bmp;
    bmp.CreateCompatibleBitmap(hdc,rcClient.right,rcClient.bottom); //创建一个与视图兼容的位图
    SelectObject(dcMem, bmp);
    FillRect(dcMem,rcClient,m_bkBrush);
    SetBkMode(dcMem,TRANSPARENT);  
	POINT pt = {10,8}; // Icon 左上角的坐标
	
	if (m_bShowTips)
	{
		pt.y += m_nTipsHeight;
	}

    if(m_nScanStatus == CLEAR_HIDE||m_nScanStatus == UN_SIZE)
    {
        bmpIcon.LoadBitmap(IDB_BITMAP_TIP_PROMPT);
        KDraw::DrawBitmap(dcMem, bmpIcon, pt.x, pt.y);
        
    }
    else if(m_nScanStatus == UN_CHECK||m_nScanStatus == DLG_INIT)
    {
        bmpIcon.LoadBitmap(IDB_BMP_BLUE);
        KDraw::DrawBitmap(dcMem, bmpIcon, pt.x, pt.y);
    }
    else if(m_nScanStatus == 2)
    {
        bmpIcon.LoadBitmap(IDB_BMP_RED);
        KDraw::DrawBitmap(dcMem, bmpIcon, pt.x, pt.y);
    }
    else
    {
//         POINT pt = {10,8};
       
        m_scanlist.Draw(dcMem, m_nCurrentPage, pt, ILD_TRANSPARENT);
        m_nCurrentPage++;
        if(m_nCurrentPage==8)
            m_nCurrentPage=0;

    }

	if (m_bShowTips)
	{
		CRect rcDraw;
		GetClientRect(&rcDraw);
		rcDraw.DeflateRect(0,0,0,rcDraw.bottom - m_nTipsHeight);
		_DrawTrashTips(dcMem, rcDraw);
	}
    _DrawPromptText(dcMem);
    BitBlt(hdc, 0, 0, rcClient.Width(), rcClient.Height(), dcMem, 0, 0, SRCCOPY);
    DeleteDC(dcMem);
    ::DeleteObject(bmp);
    ::ReleaseDC(this->m_hWnd, hdc);

    return 0;
   


}

void CContainerPage::_DrawPromptText(HDC pDC)
{
    if (!pDC)
        return;

    CString strFormat;
    CString strPrompt;
    CRect rcDraw;
    CRect rcPro;
    CString str;

    CFont font;
    font.CreatePointFont(90, L"宋体");

    HFONT hftOld = (HFONT)SelectObject(pDC,font);// (HFONT)pDC->SelectObject(KDraw::GetBigFont());

    switch (m_nScanStatus)
    {
    case 0:         // initializing status
        strPrompt.LoadString(IDS_TEXT_PROMPT_SCANING);
        m_strJunkFileSize = "";
        m_nScanStop = false;
       
        m_nPos = 10;
       
        break;

    case 1:
        strPrompt = L"正在扫描 ";
        strPrompt += m_pass->filepath;//.LoadString(IDS_TEXT_PROMPT_SCANING_STATUS);
		if (strPrompt.GetLength() > 110)
		{
			strPrompt = strPrompt.Left(109);
			strPrompt += L"...";
		}
        m_nScanStop = false;
       
        m_nPos = 10;
      
        // strPrompt.Format(strFormat, m_strJunkFileSize);
        break;

    case 2:
        strPrompt.LoadString(IDS_TEXT_PROMPT_SCANING_DONE);
       
        m_nPos = 10;
        
        
        break;
    case CLEAR_STATUS:
   
        m_nClearStop = false;
        strPrompt = L" 正在清理......";
        //pDC->DrawText(str, &rcPro, DT_SINGLELINE);
        //mScanProgress.ShowWindow(SW_SHOW);
        /*if(m_nPos+15 < 99)
            m_nPos += 15;
        mScanProgress.SetPos(m_nPos);*/
        // strPrompt.Format(strFormat, m_strJunkFileSize);
        break;
    case CLEAR_END:
       /* strPrompt.Empty();
        m_strJunkFileSize.Empty();*/
        m_nPos = 99;
        strPrompt = L" 正在清理......";
        //mScanProgress.SetPos(99);
        m_nClearStop = false;
        // strPrompt.Format(strFormat, m_strJunkFileSize);
        break;
    case CLEAR_HIDE:
        m_nClearStop = true;
        strPrompt = L"垃圾文件清理成功，共释放磁盘空间 ";
       
        break;
    case UN_CHECK:
        m_nClearStop = true;
        m_nScanStop = true;
        strPrompt = L"您尚未选择需要扫描的项目，请先选择， 然后开始扫描";
        break;
    case UN_SIZE:
        strPrompt = L"在您指定的清理选项中未发现垃圾文件";
        break;
    case DLG_INIT:
        m_nInit = true;
        strPrompt = L"垃圾文件过多会拖慢系统速度，建议定期清理！";
        break;
    default:
        break;

    }

    if(m_nScanStatus == 10||m_nScanStatus == 9 ||m_nScanStatus == 8 || m_nScanStatus == 7 || m_nScanStatus == 1 || m_nScanStatus == 2 || m_nScanStatus == 0 || m_nScanStatus == 4 || m_nScanStatus == 5 || m_nScanStatus == 6)
    {
        GetClientRect(&rcDraw);
		if (m_bShowTips)
		{
			rcDraw.DeflateRect(30, 10+m_nTipsHeight, 0, 300);
		}
		else
		{
			rcDraw.DeflateRect(30, 10, 0, 300);
		}
		DrawText(pDC,strPrompt, strPrompt.GetLength(), rcDraw, 
            DT_SINGLELINE | DT_END_ELLIPSIS);
        
        //pDC->SelectObject(KDraw::GetBoldFont());
        SetTextColor(pDC,RGB(255, 0, 0));

        SIZE sizeCaption ={0} ;
        ::GetTextExtentPoint32(pDC,strPrompt,strPrompt.GetLength(),&sizeCaption);

        rcDraw.left = rcDraw.left + sizeCaption.cx;
            
        if(m_nScanStatus != 8 && m_nScanStatus != 9&& m_nScanStatus != 10
            &&m_nScanStatus != 1&&m_nScanStatus != 0&&m_nScanStatus != 5&&m_nScanStatus != 6)
        {
            DrawText(pDC,m_strJunkFileSize, m_strJunkFileSize.GetLength(), rcDraw, 
                DT_SINGLELINE);
            ::GetTextExtentPoint32(pDC,m_strJunkFileSize,m_strJunkFileSize.GetLength(),&sizeCaption);
            rcDraw.left = rcDraw.left + sizeCaption.cx;
        }

        if (2 == m_nScanStatus || 4 == m_nScanStatus)
        {
            SetTextColor(pDC,RGB(0, 0, 0));
            strPrompt.LoadString(IDS_TEXT_PROMPT_ACTION);
           // pDC->DrawText(strPrompt, rcDraw, DT_SINGLELINE);
            DrawText(pDC,strPrompt, strPrompt.GetLength(), rcDraw, 
                DT_SINGLELINE);
           
        }
        else if(m_nScanStatus == CLEAR_HIDE)
        {
            SetTextColor(pDC,RGB(0, 0, 0));
            strPrompt.LoadString(IDS_TEXT_PROMPT_ACTION1);
            DrawText(pDC,strPrompt,strPrompt.GetLength(), rcDraw, DT_SINGLELINE);
        }

        SelectObject(pDC,hftOld);
    }
}

LRESULT CContainerPage::OnScanStatusChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    //DWORD dwJunkSize = (DWORD)lParam;

    m_pass = (MPass*)lParam;

    m_nScanStatus = (int)wParam;

    if (3 == m_nScanStatus || 4 == m_nScanStatus)
    {
        m_strJunkFileSize.Format(_T("%.2fGB"),  ((double)m_pass->m_ulTotalJunkFileSize/ ((1024) * 1024 * 1024)));
    }
    else
    {
        if (1024 >= m_pass->m_ulTotalJunkFileSize)
            m_strJunkFileSize = _T("0KB");
        else if (1024 * 1024 >= m_pass->m_ulTotalJunkFileSize)
        {
            m_strJunkFileSize.Format(_T("%.2fKB"), ((double)m_pass->m_ulTotalJunkFileSize / (1024)));
        }
        else if (1024 * 1024 * 1024 >= m_pass->m_ulTotalJunkFileSize)
        {
            m_strJunkFileSize.Format(_T("%.2fMB"), ((double)m_pass->m_ulTotalJunkFileSize / ((1024) * (1024))));
        }
        else 
        {
            m_strJunkFileSize.Format(_T("%.2fGB"), ((double)m_pass->m_ulTotalJunkFileSize / ((1024) * 1024 * 1024)));
        }
    }

    if(m_nScanStatus==2&&!m_nScanStop || 
        (m_nScanStatus==DLG_INIT&&!m_nInit)
        ||m_nScanStatus==CLEAR_HIDE&&!m_nClearStop)
    {
        if(m_nScanStatus==2&&!m_nScanStop)
            m_nScanStop = true;
        
        __InvalidCount();
    }
    else if(m_nScanStatus!=2&&m_nScanStatus!=DLG_INIT&&m_nScanStatus!=CLEAR_HIDE)
        __InvalidCount();

    return 0;
}

LRESULT CContainerPage::OnShowTrashTips(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	BOOL bShow = (BOOL)wParam;
	if (bShow)
	{
		m_nTipsCount = DEF_TIPS_HEIHT / DEF_TIPS_SPEED;
		m_nTipsHeight = DEF_TIPS_HEIHT;
		if (!m_bShowTips)
		{
			// 已经显示了就不移动啦
			SetTimer(WM_TRASH_TIPS_SHOW_EVENT, DEF_TIPS_SPEED_TIME);
		}
		else
		{
			CRect rcDraw;
			GetClientRect(&rcDraw);
			rcDraw.DeflateRect(0,0,0,rcDraw.bottom - m_nTipsHeight);
			InvalidateRect(rcDraw);
		}
		m_bShowTips = TRUE;
	}
	
	return 0;
}

LRESULT CContainerPage::OnCloseTrashTips(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{

	if (!m_bShowTips) 
		goto Clear0;

	m_bShowTips = FALSE;

	// Resize child dlg
	SendMessage(WM_SIZE, NULL, NULL);

Clear0:	
	return 0;
}

void CContainerPage::__InvalidCount()
{
    CRect rcClient;
	int nHeight = 30;

    GetClientRect(&rcClient);

	if (m_bShowTips)
	{
		nHeight += m_nTipsHeight;
	}

    rcClient.bottom = rcClient.top + nHeight;
    InvalidateRect(rcClient, FALSE);
}
void CContainerPage::_DrawTrashTips(HDC pDC, CRect& rcDraw)
{
	CString strTips = _T("检测到您的浏览器正在运行，扫描时已跳过，建议您关闭正在运行的浏览器后重新扫描。");
	CRect rcText(rcDraw);
	CFont font;
	int nTextLeft = 10;

	TRIVERTEX vert[2];
	GRADIENT_RECT gRect;
	COLORREF clr1 = RGB(255,242,182);
	COLORREF clr2 = RGB(250,231,147);

	vert[0].Red = GetRValue(clr1) << 8;
	vert[0].Green = GetGValue(clr1) << 8;
	vert[0].Blue = GetBValue(clr1) << 8;
	vert[1].Red = (GetRValue(clr2) << 8);
	vert[1].Green = (GetGValue(clr2) << 8);
	vert[1].Blue = (GetBValue(clr2) << 8);

	vert[0].x = rcDraw.left;
	vert[0].y = rcDraw.top;
	vert[1].x = rcDraw.right;
	vert[1].y = rcDraw.bottom;
	vert[0].Alpha = 0x0000;
	vert[1].Alpha = 0x0000;
	gRect.UpperLeft = 0;
	gRect.LowerRight = 1;

	GradientFill(pDC, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
	// draw text
	font.CreatePointFont(90, L"宋体");
	HFONT oldFont = (HFONT)SelectObject(pDC,font);
	rcText.left += nTextLeft;
	DrawText(pDC,strTips, strTips.GetLength(), rcText, 
		DT_SINGLELINE | DT_END_ELLIPSIS | DT_VCENTER);

	SelectObject(pDC, oldFont);
	
	// draw close bitmap
	BITMAP bmpInfo;
	HDC dcDraw = ::CreateCompatibleDC(pDC);
	HBITMAP hBmpOld = (HBITMAP)::SelectObject(dcDraw, m_bmpTipsClose);
	CRect rcBmp;
	::GetObject(m_bmpTipsClose, sizeof(BITMAP), &bmpInfo);
	rcBmp.left = rcDraw.right - 28;
	rcBmp.top = (rcDraw.Height() - bmpInfo.bmHeight) / 2;
	rcBmp.right = rcBmp.left + bmpInfo.bmWidth;
	rcBmp.bottom = rcBmp.top + bmpInfo.bmHeight;
	if (m_bTipsIn)
	{
		::BitBlt(pDC, rcBmp.left, rcBmp.top, bmpInfo.bmWidth/2, bmpInfo.bmHeight - 1, 
				 dcDraw, 16, 0, SRCCOPY);
	}
	else
	{
		::BitBlt(pDC, rcBmp.left, rcBmp.top, bmpInfo.bmWidth/2, bmpInfo.bmHeight - 1, 
				 dcDraw, 0, 0, SRCCOPY);
	}
	::CopyRect(m_rcTipsClose, rcBmp);
	::SelectObject(dcDraw, hBmpOld);
	::DeleteDC(dcDraw);
	
}

void CContainerPage::OnSize(UINT wParam, const _WTYPES_NS::CSize& size)
{
    int nHeight = 30;

    if(!m_bIsWindow)
        return;

    CRect rctmpClient;
    GetClientRect(rctmpClient);
	if (m_bShowTips)
	{
		nHeight += m_nTipsHeight;
	}

    RECT rcClient = { rctmpClient.left, rctmpClient.top+nHeight, rctmpClient.right, rctmpClient.bottom };

    m_TrashFileCleanerPage.MoveWindow(&rcClient);


    InvalidateRect(&rctmpClient);
}

void CContainerPage::OnTimer(UINT_PTR nEventID)
{
	if (nEventID == WM_TRASH_TIPS_SHOW_EVENT)
	{
		m_nTipsHeight = DEF_TIPS_HEIHT - (DEF_TIPS_SPEED * m_nTipsCount);
		if (m_nTipsHeight <= 0)
			m_nTipsHeight = DEF_TIPS_SPEED;

		SendMessage(WM_SIZE, NULL, NULL);		
		if (m_nTipsCount <= 0)
		{
			KillTimer(WM_TRASH_TIPS_SHOW_EVENT);
		}
		--m_nTipsCount;
	}
}

BOOL CContainerPage::OnEraseBkgnd(HDC pDC) 
{
    // Draw gray background
    return TRUE;
}