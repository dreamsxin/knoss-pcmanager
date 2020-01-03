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

//
#define MAX_COLUN_CNT 10
/////////////////////////////////////////////////////////////////////////////
// CFileListCtrl window

#include "FileListHeader.h"
#include "FileListData.h"


class CFileListCtrl : public CWindowImpl<CFileListCtrl,CListViewCtrl>,
     public COwnerDraw<CFileListCtrl>
{
// Construction
public:
	CFileListCtrl();
    virtual ~CFileListCtrl();

	BOOL Sort(int nColumIdx);
	static int Qsort_compare( const void *arg1, const void *arg2 );
	static int Compare_TrashfileItem(TRASHFILEITEM *pLeft, TRASHFILEITEM *pRight, int nIdx);
    void InitListCtrl();
    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    void SetFileListData(TRASHFILELIST* pFileList);
    TRASHFILELIST* GetFileListData();
    int	_XIdxEdgeHitTest(const CPoint &point);
    int _SubItemHitTest(LPLVHITTESTINFO pInfo);
    BOOL OnSwitchTable();
  

	
public:
    COLORREF m_clrSelect;
    COLORREF m_clrSelectBorder;
    COLORREF m_clrTextColor;
    COLORREF m_clrSelectTextColor;
    COLORREF m_clrBorder;
    CFileListHeader m_wndHeaderCtrl;
    CBitmap m_right;
    int m_nHeight;
    int m_nWidth;
    CImageList m_imagelist;
    CBrush m_bkBrush;

protected:
    TRASHFILELIST* m_pFileList;
    CString m_strClearFlag[3];
	int		m_nLDown;
	static int m_snSortColumnIdx;		//用于通信给比较函数
	static BOOL m_spbSortAscending[MAX_COLUN_CNT];

public:

    BEGIN_MSG_MAP(CFileListCtrl)
        MSG_WM_PAINT(OnPaint)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
        MSG_WM_SIZE(OnSize)
        CHAIN_MSG_MAP_ALT(COwnerDraw<CFileListCtrl>, 1)
    END_MSG_MAP()

    void DeleteItem(LPDELETEITEMSTRUCT lParam)
    {

    }
    void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
    {
        lpMeasureItemStruct->itemHeight  = 18;
    }
  


protected:

	 void OnPaint(HDC pDC);
	 void OnSize(UINT nType,CSize size);
	 void OnMouseMove(UINT nFlags, CPoint point);
	 void OnLButtonDown(UINT nFlags, CPoint point);
	 void OnLButtonUp(UINT nFlags, CPoint point);
	 void OnLButtonDblClk(UINT nFlags, CPoint point);
     HBRUSH OnCtlColor(HDC hDC, HWND hWnd);
   

};


