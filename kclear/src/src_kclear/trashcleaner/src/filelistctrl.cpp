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

// FileListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "trashcleaner.h"
#include "filelistctrl.h"
#include <stdlib.h>
#include <string>
#include "trashdefine.h"
#include "trashpublicfunc.h"
#include <iostream>

/////////////////////////////////////////////////////////////////////////////
// CFileListCtrl

int			CFileListCtrl::m_snSortColumnIdx = 0; 		//用于通信给比较函数
BOOL		CFileListCtrl::m_spbSortAscending[MAX_COLUN_CNT] = {0} ;


int CFileListCtrl::Compare_TrashfileItem(TRASHFILEITEM *pLeft, TRASHFILEITEM *pRight, int nIdx)
{
    int nRet = 0;
	switch (nIdx)
	{
	case 0:
		nRet = pLeft->strFileName.Compare(pRight->strFileName);
        break;
	case 1:
		if (pLeft->ulFileSize > pRight->ulFileSize)
			nRet = 1;
		else if (pLeft->ulFileSize == pRight->ulFileSize)
			nRet = 0;
		else
			nRet = -1;
        break;
	case 2:
		
        break;
	default:
		break;
	}
	return nRet;
}

int CFileListCtrl::Qsort_compare( const void *arg1, const void *arg2 )
{
	TRASHFILEITEM *pItemLeft = (TRASHFILEITEM*)arg1;
	TRASHFILEITEM *pItemRight = (TRASHFILEITEM*)arg2;

	if (CFileListCtrl::m_snSortColumnIdx >= 0 && CFileListCtrl::m_snSortColumnIdx <= 2)
	{
		int nMul = 0;
		if (CFileListCtrl::m_spbSortAscending[CFileListCtrl::m_snSortColumnIdx])
		{
			nMul = 1;
		}
		else
		{
			nMul = -1;
		}
		return  nMul * Compare_TrashfileItem(pItemLeft, pItemRight, CFileListCtrl::m_snSortColumnIdx);
	}

	return 0;
}




CFileListCtrl::CFileListCtrl() :
    m_pFileList(NULL)
{
    m_clrSelect       = RGB(228, 245, 255);
    m_clrSelectBorder = RGB(0 , 0, 128);
    m_clrTextColor    = RGB(0, 0, 0);
    m_clrSelectTextColor = RGB(0,0,0);
    m_clrBorder       = RGB(165, 165, 165);
	m_nLDown = -1;
	for (int n = 0; n < MAX_COLUN_CNT; n++)
	{
		m_spbSortAscending[n] = FALSE;
	}
    m_bkBrush.CreateSolidBrush(RGB(255,255,255));

  
}

CFileListCtrl::~CFileListCtrl()
{
    ::DeleteObject(m_bkBrush);
}


/////////////////////////////////////////////////////////////////////////////
// CFileListCtrl message handler
void CFileListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    SetMsgHandled(FALSE);
    LVHITTESTINFO subItmInfo;
    subItmInfo.pt = point;
    _SubItemHitTest(&subItmInfo);

    int nItmIdx = subItmInfo.iSubItem;
    int nItm = subItmInfo.iItem;
    
    if (nItmIdx == 0 && nItm >=0 && m_pFileList->itemArray.size() > 0)
    {
        CString filename = m_pFileList->itemArray.at(nItm).strFileName;
        int nPos = filename.ReverseFind('\\');
        filename = filename.Mid(0, nPos + 1);
        ShellExecute( m_hWnd, 
            _T("open"), 
            filename, 
            NULL, NULL, SW_SHOWNORMAL );
    }

    if(m_pFileList->ulAllFileSize==0)
        DrawItem(NULL);
}

void CFileListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
    SetMsgHandled(FALSE);
	BOOL bReserve = FALSE;
	BOOL bOnEdge  = FALSE;
	LVHITTESTINFO subItmInfo;
	subItmInfo.pt = point;
	_SubItemHitTest(&subItmInfo);

	if (subItmInfo.flags == LVHT_ONITEMICON && subItmInfo.iItem == -2)
	{
		bOnEdge = TRUE;
	}

	if (-1 != m_nLDown || bOnEdge)
	{
		SetCursor(LoadCursor(_Module.m_hInst,MAKEINTRESOURCE(IDC_SELFCURSOR)));
		
		if (-1 != m_nLDown && nFlags == MK_LBUTTON)
		{
            if(m_pFileList->ulAllFileSize==0)
               return;
			HDITEM hdItm;		
			RECT   rt;
			m_wndHeaderCtrl.GetItemRect(m_nLDown, &rt);
			
			int nXScrollPos = GetScrollPos(SB_HORZ);			//取滚动条的位置
            std::cout<<nXScrollPos<<" ";
			int nPtXPos = point.x + nXScrollPos;
			if (nPtXPos <= rt.left)
				hdItm.cxy = 0;
			else
			{
				hdItm.cxy = nPtXPos - rt.left;
			}

			hdItm.mask = HDI_WIDTH;
			m_wndHeaderCtrl.SetItem(m_nLDown, &hdItm);
			m_wndHeaderCtrl.Invalidate(FALSE);
			bReserve = TRUE;
		}	
	}
	else
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}

	if (!bReserve)
	{
		m_nLDown = -1;
	}
}

//该函数仅测试水平方向的的subitemidx
int CFileListCtrl::_XIdxEdgeHitTest(const CPoint &point)
{
	int nRet = -1;
	int nXScroll = GetScrollPos(SB_HORZ);
	int nXPos = point.x;
	nXPos += nXScroll;

	HDITEM hdItm0;
	hdItm0.mask = HDI_WIDTH;
	if (!m_wndHeaderCtrl.GetItem(0, &hdItm0))
		goto Exit0;
	int nWidth0 = hdItm0.cxy;

	HDITEM hdItm1;
	hdItm1.mask = HDI_WIDTH;
	if (!m_wndHeaderCtrl.GetItem(1, &hdItm1))
		goto Exit0;
	int nWidth1 = hdItm1.cxy;

	if (nXPos > nWidth0 - EDGE_PADING && nXPos <= nWidth0 + EDGE_PADING)
	{
		nRet = 1;
	}

	//没有用else是为了解决重合的情况
	else if (nXPos > nWidth0 + nWidth1 - EDGE_PADING && nXPos <= nWidth1 + nWidth0 + EDGE_PADING)
	{
		nRet = 2;
	}
	
Exit0:
	return nRet;
}

int CFileListCtrl::_SubItemHitTest(LPLVHITTESTINFO pInfo)
{
	int nRet = SubItemHitTest(pInfo);

	//去掉垂直方向上的滚动条的影响
 	if (m_wndHeaderCtrl._HeaderYHitTest(pInfo->pt))
 	{
 		pInfo->iItem = -2;		//表示列表头	
 		int nId = _XIdxEdgeHitTest(pInfo->pt);
 		if (-1 != nId)
 		{
 			pInfo->iSubItem = nId;
 			pInfo->flags = LVHT_ONITEMICON;
 		}
 	}

	return pInfo->iItem;
}


BOOL CFileListCtrl::OnSwitchTable()
{
	int nIdx = m_wndHeaderCtrl.GetFocusIdx();
	Sort(nIdx);
	return TRUE;
}

 void CFileListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
 {
     
     if (nFlags == 9 || nFlags == 5)
         return;
     SetMsgHandled(FALSE);
 	LVHITTESTINFO subItmInfo;
 	subItmInfo.pt = point;
     int m = _SubItemHitTest(&subItmInfo);
 
 	if (LVHT_ONITEMICON == subItmInfo.flags && -2 == subItmInfo.iItem)		//仅此位有效时
 	{
 		m_nLDown = subItmInfo.iSubItem - 1;
         
 		SetCapture();
 	
 	}
 	else
 	{
 		if (-2 != subItmInfo.iItem || -1 == subItmInfo.iSubItem)
 		{
 
             
 			return;
 		}
         m = -1;
 		m_spbSortAscending[subItmInfo.iSubItem] ^= 0x01;
 		m_wndHeaderCtrl.SwitchArrow(subItmInfo.iSubItem, m_spbSortAscending[subItmInfo.iSubItem]);
 		Sort(subItmInfo.iSubItem);
 		this->Invalidate();
 		
 	}
     if(m_pFileList->ulAllFileSize==0)
         DrawItem(NULL);
     
    
 }

void CFileListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
    SetMsgHandled(FALSE);
	m_nLDown = -1;
	ReleaseCapture();
    if(m_pFileList->ulAllFileSize==0)
        DrawItem(NULL);
    
}



void CFileListCtrl::InitListCtrl()
{
    CHeaderCtrl pHeader = GetHeader();
  
    m_wndHeaderCtrl.SubclassWindow(pHeader.m_hWnd);

    m_wndHeaderCtrl.EnableWindow(FALSE);

    ModifyStyle(0, 
        WS_BORDER | LVS_REPORT | LVS_OWNERDATA | LVS_OWNERDRAWFIXED|WS_HSCROLL | WS_VSCROLL);
    
    ModifyStyleEx(WS_EX_CLIENTEDGE, 0);

//     SetExtendedListViewStyle( 
//         LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);    

    SetWindowPos(NULL, 0, 0, 0, 0,
        SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

    m_imagelist.Create(1, 20, ILC_COLOR, 1, 0);
    SetImageList((HIMAGELIST)m_imagelist, LVSIL_SMALL );

    m_right.LoadBitmap(IDB_BMP_OK);
    BITMAP bm;
    ::GetObject(m_right, sizeof(BITMAP), (PSTR)&bm); 
    m_nWidth = bm.bmWidth;
    m_nHeight = bm.bmHeight;

    //  
}

 void CFileListCtrl::OnPaint(HDC pDC) 
 {
      SetMsgHandled(FALSE);
      CWindowDC dc(m_hWnd);
      CRect rcWindow;
  
      GetWindowRect(&rcWindow);
  
      rcWindow.right  = rcWindow.Width();
      rcWindow.bottom = rcWindow.Height();
      rcWindow.left   = 0;
      rcWindow.top    = 0;
  
      int nSaveDC = dc.SaveDC();
  
      HBRUSH hBrush = ::CreateSolidBrush(RGB(165,165,165));
  
      ::FrameRect((HDC)dc, &rcWindow, hBrush );
  
      ::DeleteObject(hBrush);
     
  
      dc.RestoreDC(nSaveDC);
 
}



void CFileListCtrl::SetFileListData(TRASHFILELIST* pFileList)
{
    
    m_pFileList = pFileList;
    m_wndHeaderCtrl.m_pFileList = pFileList;
    
    SetItemCountEx(
        m_pFileList->itemArray.size(), 
        LVSICF_NOINVALIDATEALL|	LVSICF_NOSCROLL );
    
    this->Invalidate();
}

TRASHFILELIST* CFileListCtrl::GetFileListData()
{
    return m_pFileList;
}


void CFileListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    if (m_pFileList == NULL)
        return;

    if(lpDrawItemStruct == NULL || m_pFileList->ulAllFileSize == 0)
    {
        CWindowDC dc(m_hWnd);
        CRect rcWindow;

        GetWindowRect(&rcWindow);

        rcWindow.right  = rcWindow.Width();
        rcWindow.bottom = rcWindow.Height();
        rcWindow.left   = 0;
        rcWindow.top    = 0;

        int nSaveDC = dc.SaveDC();

        HBRUSH hBrush = ::CreateSolidBrush(RGB(165,165,165));

        ::FrameRect((HDC)dc, &rcWindow, hBrush );

        ::DeleteObject(hBrush);


        dc.RestoreDC(nSaveDC);

        CPaintDC hdc(m_hWnd);    

        CRect rcDraw;

        rcDraw.left = 150;
        rcDraw.top = 160;
        rcDraw.right = rcDraw.left + 200;
        rcDraw.bottom = rcDraw.top + 16;

    
        FillRect((HDC)hdc,rcDraw,m_bkBrush);
        SetBkMode((HDC)hdc,TRANSPARENT); 
      
        ::SetTextColor((HDC)hdc,RGB(0,0,0));
      
        CFont font;
        font.CreatePointFont(90, TEXT("宋体"));
       
        HFONT oldobj = (HFONT)hdc.SelectFont((HFONT)font);

        CString strTmp = _T("未在此项中发现垃圾文件");
        DrawText((HDC)hdc,strTmp,strTmp.GetLength(),rcDraw, DT_SINGLELINE);
        SelectObject(hdc,oldobj);

       

        return;
    }

    CDC dc;
    
    dc.Attach(lpDrawItemStruct->hDC);
    
    int nSaveDC = dc.SaveDC();

    RECT rcItem     = lpDrawItemStruct->rcItem;
    int  nitemIndex = lpDrawItemStruct->itemID;
    
    bool bSelected = (lpDrawItemStruct->itemState & ODS_SELECTED) != 0;
    bool bFocus    = (lpDrawItemStruct->itemState & ODS_FOCUS) != 0;
    
    if (nitemIndex >= m_pFileList->itemArray.size())
        return;

    dc.SetTextColor( bSelected ? 
       RGB(0,0,0) : 
        GetSysColor(COLOR_WINDOWTEXT));
    
    COLORREF clrBkGnd = 0;

    if (bSelected)
    {
         clrBkGnd = RGB(185,219,255);
    }
    else
    {
        clrBkGnd = GetSysColor(COLOR_WINDOW);
    }
    
    RECT rcFrame = rcItem;

    rcFrame.left = rcFrame.left+5;
    rcFrame.right = rcFrame.right - 5;
   
    dc.FillSolidRect(&rcFrame, clrBkGnd);
   
    CRect rcSubItem0;
    CRect rcSubItem1;
  

    GetSubItemRect(nitemIndex, 0, LVIR_LABEL, rcSubItem0);
    GetSubItemRect(nitemIndex, 1, LVIR_LABEL, rcSubItem1);


    rcSubItem0.left += 5;

    dc.DrawText(m_pFileList->itemArray[nitemIndex].strFileName,
        m_pFileList->itemArray[nitemIndex].strFileName.GetLength(),
        &rcSubItem0, DT_LEFT | DT_VCENTER | DT_PATH_ELLIPSIS | DT_SINGLELINE);
    
    rcSubItem1.right -= 5;
    dc.DrawText( GetFileSizeText(m_pFileList->itemArray[nitemIndex].ulFileSize), -1,
        &rcSubItem1, DT_VCENTER | DT_SINGLELINE | DT_RIGHT );

    dc.RestoreDC(nSaveDC);
    dc.Detach();
   
}



BOOL CFileListCtrl::Sort(int nColumIdx)
{
	if (nColumIdx >= m_wndHeaderCtrl.GetItemCount() || nColumIdx < 0)
	{
		return FALSE;
	}
	m_snSortColumnIdx = nColumIdx;
	if (1)
	{		
		if (m_pFileList->itemArray.size())								//qsort不能保证表为空时正确排序
		{
			qsort((void*)&m_pFileList->itemArray[0], m_pFileList->itemArray.size(), 
				sizeof(m_pFileList->itemArray[0]), CFileListCtrl::Qsort_compare);
		}

	}
	
	return TRUE;
}


void CFileListCtrl::OnSize(UINT nType, CSize size) 
{
    SetMsgHandled(FALSE);
    RedrawWindow();
}



