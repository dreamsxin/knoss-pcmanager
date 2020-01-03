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

/////////////////////////////////////////////////////////////////////////////
// CDirectoryListCtrl window
#include "wtlctrls/listheader.h"
#include "trashdefine.h"

typedef struct  {

    int     nStatus;
    CString strCaption;
    ULONGLONG ulSize;
    int nImage;
    int nSelImage;
    int id;
    int parent;

} DIR_LIST_ITEM;

class CDirectoryListCtrl : public CWindowImpl<CDirectoryListCtrl,CListViewCtrl>,
    public COwnerDraw<CDirectoryListCtrl>
{
// Construction
public:
	CDirectoryListCtrl();
    virtual ~CDirectoryListCtrl();

    typedef std::vector<DIR_LIST_ITEM> CDirItemArray;
// Attributes

public:

    COLORREF m_clrSelect;
    COLORREF m_clrSelectBorder;
    COLORREF m_clrTextColor;
    COLORREF m_clrSelectTextColor;
    COLORREF m_clrBorder;
    CImageList m_imagelist;
	CImageList m_displist;
    int m_nScaningIndex;
    int m_nCurrentPage;
    CDirItemArray m_itemArray;
    BOOL m_bIsInit;

    void AutoColumnWidth();
    void AddItem(LPCTSTR pszName, LPCTSTR pszPath, int nStatus = em_ds_none);
    void SetScanning(int nIndex);
    void StopScanning();
    int  SetDirItemStatus(int nIndex, int nStatus = em_ds_none);
    int  GetDirItemStatus(int nIndex);
    void InitListCtrl();
    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

    void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
    {
        lpMeasureItemStruct->itemHeight  = 18;
    }
    void DeleteItem(LPDELETEITEMSTRUCT lParam)
    {

    }

    BEGIN_MSG_MAP(CDirectoryListCtrl)
        MSG_WM_NCPAINT(OnNcPaint)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_RBUTTONDOWN(OnRButtonDown)
        MESSAGE_HANDLER(LVM_HITTEST, OnHitTest)
        CHAIN_MSG_MAP_ALT(COwnerDraw<CDirectoryListCtrl>, 1)
    END_MSG_MAP()



private:

    CListHeader m_wndHeaderCtrl;
    HIMAGELIST  m_hImageList;

protected:
	
     LRESULT OnHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
	 void OnNcPaint(HRGN rgn);
	 void OnTimer(UINT nIDEvent);
	 void OnLButtonDown(UINT nFlags, CPoint point);
	 void OnDestroy();
	 void OnRButtonDown(UINT nFlags, CPoint point);


};


