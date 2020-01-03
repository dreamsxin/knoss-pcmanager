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

// DirectoryListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "trashcleaner.h"
#include "DirectoryListCtrl.h"
#include "trashdefine.h"
#include "trashpublicfunc.h"
#include "wtlctrls/kdraw.h"


CDirectoryListCtrl::CDirectoryListCtrl()
{
    m_clrSelect       = RGB(228, 245, 255);
    m_clrSelectBorder = RGB(0 , 0, 128);
    m_clrTextColor    = RGB(0, 0, 0);
    m_clrSelectTextColor = RGB(0, 0, 0);
    m_clrBorder       = RGB(165, 165, 165);
    m_nScaningIndex = -1;
    m_nCurrentPage  = 0;
    m_hImageList = NULL;
    m_bIsInit = TRUE;
}

CDirectoryListCtrl::~CDirectoryListCtrl()
{
}



/////////////////////////////////////////////////////////////////////////////
// CDirectoryListCtrl message handlers

void CDirectoryListCtrl::InitListCtrl()
{
    CHeaderCtrl pHeader = GetHeader();
    m_wndHeaderCtrl.SubclassWindow(pHeader.m_hWnd);

    ModifyStyle(0, 
        WS_BORDER | LVS_REPORT | LVS_SINGLESEL | LVS_OWNERDATA|LVS_OWNERDRAWFIXED);

    ModifyStyleEx(WS_EX_CLIENTEDGE, 0);

    SetExtendedListViewStyle( 
        LVS_EX_FULLROWSELECT| LVS_EX_INFOTIP );
   

    m_imagelist.Create(28, 20, ILC_COLOR8 | ILC_MASK, 2, 0);
    if (m_imagelist)
    {
        CBitmap bmp;
        bmp.LoadBitmap(IDB_CHECKBOX_BMP);

        m_imagelist.Add((HBITMAP)bmp, RGB(255, 0, 255));

        SetImageList((HIMAGELIST)m_imagelist, LVSIL_SMALL );
    }


   
    m_displist.Create(16, 16, ILC_COLOR8 | ILC_MASK, 4, 0);
    if (m_imagelist)
    {
        
        CBitmap bmpCache;
        CBitmap bmpDownload;
        CBitmap bmpExp;
        CBitmap bmpIE;
        CBitmap bmpPerson;
        CBitmap bmpRead;
        CBitmap bmpSys;
        CBitmap bmpUpdate;
        CBitmap bmpUTEMP;
		

        bmpCache.LoadBitmap(IDB_BMP_CACHE);
        bmpDownload.LoadBitmap(IDB_BMP_DOWNLOAD);
        bmpExp.LoadBitmap(IDB_BMP_EXP);
        bmpIE.LoadBitmap(IDB_BMP_IE);
        bmpRead.LoadBitmap(IDB_BMP_READ);
        bmpSys.LoadBitmap(IDB_BMP_SYS);
        bmpUpdate.LoadBitmap(IDB_BMP_UPDATE);
        bmpUTEMP.LoadBitmap(IDB_BMP_UTEMP);
        bmpPerson.LoadBitmap(IDB_BMP_PERSON);
		

       /* m_scanlist.Add(&bmp0, RGB(255, 255, 255));
        m_scanlist.Add(&bmp1, RGB(255, 255, 255));
        m_scanlist.Add(&bmp2, RGB(255, 255, 255));
        m_scanlist.Add(&bmp3, RGB(255, 255, 255));*/

        m_displist.Add((HBITMAP)bmpCache, RGB(255, 255, 255));
        m_displist.Add((HBITMAP)bmpDownload, RGB(255, 255, 255));
        m_displist.Add((HBITMAP)bmpExp, RGB(255, 255, 255));
        m_displist.Add((HBITMAP)bmpIE, RGB(255, 255, 255));
        m_displist.Add((HBITMAP)bmpRead, RGB(255, 255, 255));
        m_displist.Add((HBITMAP)bmpSys, RGB(255, 255, 255));
        m_displist.Add((HBITMAP)bmpUpdate, RGB(255, 255, 255));
        m_displist.Add((HBITMAP)bmpUTEMP, RGB(255, 255, 255));
    }

    SetWindowPos(NULL, 0, 0, 0, 0,
        SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

}

void CDirectoryListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CClientDC dc(m_hWnd);

    int nSaveDC = dc.SaveDC();

    SelectObject((HDC)dc,GetFont());

    RECT rcItem     = lpDrawItemStruct->rcItem;
    int  nitemIndex = lpDrawItemStruct->itemID;
    bool bSelected = (lpDrawItemStruct->itemState & ODS_SELECTED) != 0;
    bool bFocus    = (lpDrawItemStruct->itemState & ODS_FOCUS) != 0;

    CRect rcClient;
    GetClientRect(&rcClient);

    rcItem.right = rcClient.right;

    DIR_LIST_ITEM& item = m_itemArray[nitemIndex];

    CRect rcSubItem;

    GetSubItemRect(nitemIndex, 0, LVIR_LABEL, rcSubItem);

    
    RECT rcBkGnd = rcSubItem;
    rcBkGnd.left  = rcSubItem.left + 15;
    COLORREF clrBack = RGB(185,219,255);
    
    SetBkMode((HDC)dc,TRANSPARENT);
    dc.FillSolidRect(&rcBkGnd, 
        bSelected && (item.nStatus == em_ds_uncheck || item.nStatus == em_ds_check) ? 
clrBack:GetSysColor(COLOR_WINDOW) );  
              
   

    POINT pt = { rcItem.left , rcItem.top };

    if (item.nStatus == em_ds_uncheck)
    {
        m_imagelist.Draw((HDC)dc, 0, pt, ILD_TRANSPARENT);
    }
    else //if (item.nStatus == em_ds_check)
    {        
        m_imagelist.Draw((HDC)dc, 1, pt, ILD_TRANSPARENT);
    }

   // if (m_hImageList != NULL)
    {
        if (_tcsicmp(item.strCaption, _T("Office 安装缓存目录")) == 0)
        {
            POINT pt = {rcSubItem.left-6 , rcSubItem.top+2};
            m_displist.Draw((HDC)dc, 0,  pt, ILD_TRANSPARENT);
        }
        else if (_tcsicmp(item.strCaption, _T("已下载程序文件")) == 0)
        {
            POINT pt = {rcSubItem.left - 6, rcSubItem.top + 2};
            m_displist.Draw((HDC)dc, 1,  pt, ILD_TRANSPARENT);
        }

        else if(_tcsicmp(item.strCaption, _T("系统盘垃圾文件深度搜索")) == 0)
        {
            POINT pt = {rcSubItem.left - 6, rcSubItem.top + 2};
            m_displist.Draw((HDC)dc, 2,  pt, ILD_TRANSPARENT);
        }

        else if (_tcsicmp(item.strCaption, _T("网页历史文件夹")) == 0)
        {
            POINT pt = {rcSubItem.left - 6, rcSubItem.top + 2};
            m_displist.Draw((HDC)dc, 3,  pt, ILD_TRANSPARENT);
        }
        else if(_tcsicmp(item.strCaption, _T("windows 预读文件")) == 0)
        {
            POINT pt = {rcSubItem.left - 6, rcSubItem.top + 2};
            m_displist.Draw((HDC)dc, 4,  pt, ILD_TRANSPARENT);
        }

        else if(_tcsicmp(item.strCaption, _T("Windows 临时目录")) == 0
            || _tcsicmp(item.strCaption, _T("Windows 日志文件")) == 0)
        {
            POINT pt = {rcSubItem.left - 6, rcSubItem.top + 2};
            m_displist.Draw((HDC)dc, 5,  pt, ILD_TRANSPARENT);
        }

        else if ( _tcsicmp(item.strCaption, _T("Windows 更新补丁")) == 0)
        {
            POINT pt = {rcSubItem.left - 6, rcSubItem.top + 2};
            m_displist.Draw((HDC)dc, 6,  pt, ILD_TRANSPARENT);

        }
        else if(_tcsicmp(item.strCaption, _T("用户临时文件夹")) == 0)
        {
            POINT pt = {rcSubItem.left - 6, rcSubItem.top + 2};
            m_displist.Draw((HDC)dc, 7,  pt, ILD_TRANSPARENT);
        }

    }

    rcSubItem.left += 16;


    dc.DrawText(item.strCaption, item.strCaption.GetLength(),
        &rcSubItem, 
        DT_LEFT | DT_VCENTER | DT_SINGLELINE );

    if (!m_bIsInit && item.nStatus != em_ds_scaning)
    {
        CString strFileSize = GetFileSizeText(item.ulSize);;

        SIZE sizeCaption;
        SIZE sizeFileSize;
        SIZE sizeFlag;

        dc.GetTextExtent(item.strCaption,-1,&sizeCaption);
        dc.GetTextExtent(strFileSize,-1,&sizeFileSize);
        dc.GetTextExtent(_T("("),-1,&sizeFlag);
        rcSubItem.left += sizeCaption.cx;

        dc.DrawText(_T("("), 1, 
            &rcSubItem, 
            DT_LEFT | DT_VCENTER | DT_SINGLELINE );

        COLORREF clrOld = dc.SetTextColor(RGB(255, 0, 0));

        rcSubItem.left += sizeFlag.cx;
        dc.DrawText(strFileSize, strFileSize.GetLength(),
            &rcSubItem, 
            DT_LEFT | DT_VCENTER | DT_SINGLELINE );

        dc.SetTextColor(clrOld);

        rcSubItem.left += sizeFileSize.cx;
        dc.DrawText(_T(")"), 1, 
            &rcSubItem, 
            DT_LEFT | DT_VCENTER | DT_SINGLELINE );
    }else if (!m_bIsInit && item.nStatus == em_ds_scaning)
    {
        CString strFileSize = L"正在扫描...";

        SIZE sizeCaption;
        SIZE sizeFileSize;
        SIZE sizeFlag;

        dc.GetTextExtent(item.strCaption,-1,&sizeCaption);
        dc.GetTextExtent(strFileSize,-1,&sizeFileSize);
        dc.GetTextExtent(_T("("),-1,&sizeFlag);
        rcSubItem.left += sizeCaption.cx;

        dc.DrawText(_T("("), 1, 
            &rcSubItem, 
            DT_LEFT | DT_VCENTER | DT_SINGLELINE );

        COLORREF clrOld = dc.SetTextColor(RGB(255, 0, 0));

        rcSubItem.left += sizeFlag.cx;
        dc.DrawText(strFileSize, strFileSize.GetLength(),
            &rcSubItem, 
            DT_LEFT | DT_VCENTER | DT_SINGLELINE );

        dc.SetTextColor(clrOld);

        rcSubItem.left += sizeFileSize.cx;
        dc.DrawText(_T(")"), 1, 
            &rcSubItem, 
            DT_LEFT | DT_VCENTER | DT_SINGLELINE );
    }

   


    dc.RestoreDC(nSaveDC);

}


void CDirectoryListCtrl::AutoColumnWidth()
{
    CClientDC dc(m_hWnd);

    HGDIOBJ pFont = SelectObject((HDC)dc,GetFont());

    CSize sizeMax;

    sizeMax.cx = 0;

    for (int i = 0; i < m_itemArray.size(); i++)
    {
        const CString& strCaption = m_itemArray[i].strCaption;

        CString strDisplayText;

        if (m_itemArray[i].nStatus == em_ds_check ||
            m_itemArray[i].nStatus == em_ds_uncheck)
        {
            strDisplayText.Format(TEXT("%s ( %s)"), 
                strCaption, 
                GetFileSizeText(m_itemArray[i].ulSize));
        }
        else
        {
            strDisplayText = strCaption;
        }

        CSize  size;
        dc.GetTextExtent(strDisplayText,-1,&size); 

        if (sizeMax.cx < size.cx)
            sizeMax = size;
    }


    SelectObject((HDC)dc,pFont);

    CRect rcClient;
    GetClientRect(&rcClient);

    sizeMax.cx += 36 + 20;

    if (sizeMax.cx < rcClient.right)
        SetColumnWidth(0, rcClient.right);
    else    
        SetColumnWidth(0, sizeMax.cx);
    Invalidate();

}

void CDirectoryListCtrl::AddItem(LPCTSTR pszName, LPCTSTR pszPath,int nStatus /* = em_ds_none */)
{

    DIR_LIST_ITEM item;

    item.nStatus    = nStatus;
    item.strCaption = pszName;
    item.ulSize     = 0;
    m_itemArray.push_back(item);
    SetItemCountEx(m_itemArray.size(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);

}

int  CDirectoryListCtrl::SetDirItemStatus(int nIndex, int nStatus /* = em_ds_none */)
{

    int nResult = m_itemArray[nIndex].nStatus;

    m_itemArray[nIndex].nStatus = nStatus;

    RedrawItems(nIndex, nIndex);

    return nResult;
}

int  CDirectoryListCtrl::GetDirItemStatus(int nIndex)
{
    

    return m_itemArray[nIndex].nStatus;
}

void CDirectoryListCtrl::SetScanning(int nIndex)
{

    int nTmp = m_nScaningIndex;

    m_nScaningIndex = nIndex;

    m_nCurrentPage = 0;

    m_itemArray[nIndex].nStatus = em_ds_scaning;
   
    RedrawItems(nIndex, nIndex);


    

    if (nTmp < 0)
    {
        
    }
    else
    {
        RedrawItems(nTmp, nTmp);
    }

}

void CDirectoryListCtrl::StopScanning()
{
    if (m_nScaningIndex >= 0)
    {
    
        RedrawItems(m_nScaningIndex, m_nScaningIndex);

        m_nCurrentPage = 0;
    }

    m_nScaningIndex = -1;
}

 void CDirectoryListCtrl::OnNcPaint(HRGN rgn) 
 {
     CWindowDC dc(m_hWnd);
     CRect rcWindow;
 
     GetWindowRect(&rcWindow);
 
     rcWindow.right  = rcWindow.Width();
     rcWindow.bottom = rcWindow.Height();
     rcWindow.left   = 0;
     rcWindow.top    = 0;
 
     int nSaveDC = dc.SaveDC();

    

    // SelectObject((HDC)dc,&pen);
    // SelectObject((HDC)dc,GetStockObject(WHITE_BRUSH));

     HBRUSH hBrush = ::CreateSolidBrush(RGB(165,165,165));

     ::FrameRect((HDC)dc, &rcWindow, hBrush );

     ::DeleteObject(hBrush);

 
     dc.RestoreDC(nSaveDC);

 }



void CDirectoryListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
    CRect rcHeadClient;
    m_wndHeaderCtrl.GetClientRect(rcHeadClient);

    if (point.y < rcHeadClient.bottom)
        return;

    UINT uFlag  = 0;

    LVHITTESTINFO pHitTestInfo = {0};
    pHitTestInfo.pt = point;
    pHitTestInfo.flags = 0;
    
    int  nIndex = HitTest(&pHitTestInfo);
    if (nIndex < 0)
    {
        SetMsgHandled(FALSE);
        return;
    }


    DIR_LIST_ITEM& item = m_itemArray[nIndex];

    if ( nIndex >= 0 && pHitTestInfo.flags == LVHT_ONITEMSTATEICON)
    {
        int uStatus = item.nStatus;

        HWND pParent = GetParent();

        if (uStatus == em_ds_uncheck)
        {
            m_itemArray[nIndex].nStatus = em_ds_check;

            RedrawItems(nIndex, nIndex);

            if (pParent != NULL)
                 SendMessage(pParent, WM_CHECK_CHANGE, em_ds_check, nIndex);

        }
        else if (uStatus == em_ds_check)
        {
            m_itemArray[nIndex].nStatus = em_ds_uncheck;

            RedrawItems(nIndex, nIndex);

            if (pParent != NULL)
                SendMessage(pParent, WM_CHECK_CHANGE, em_ds_uncheck, nIndex);
        }

        return;
    }

    if (item.nStatus == em_ds_check || item.nStatus == em_ds_uncheck)
        SetMsgHandled(FALSE);
//         SetItemState(nIndex,LVIS_SELECTED | LVIS_FOCUSED, 
//         LVIS_SELECTED | LVIS_FOCUSED);

        
}

LRESULT CDirectoryListCtrl::OnHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    LRESULT lResult = -1;
    bHandled = TRUE;
    LPLVHITTESTINFO pHitTestInfo = (LPLVHITTESTINFO)lParam;

    lResult = DefWindowProc(LVM_HITTEST, wParam, lParam);

    if ( lResult >= 0 )
    {

        CRect rcCheckBox;
        GetSubItemRect(lResult, 0, LVIR_BOUNDS, rcCheckBox);

        rcCheckBox.left  = rcCheckBox.left + 9;
        rcCheckBox.right = rcCheckBox.left + 13;

        rcCheckBox.top  = rcCheckBox.top + 4;
        rcCheckBox.bottom = rcCheckBox.top + 13;

        if (::PtInRect(&rcCheckBox, pHitTestInfo->pt))
            pHitTestInfo->flags = LVHT_ONITEMSTATEICON;
    }

    return lResult;
}





void CDirectoryListCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
    CRect rcHeadClient;
    m_wndHeaderCtrl.GetClientRect(rcHeadClient);

    if (point.y < rcHeadClient.bottom)
        return;

    UINT uFlag  = 0;

    int  nIndex = HitTest(point, &uFlag);
    if (nIndex < 0)
    {
        
        return;
    }



}
