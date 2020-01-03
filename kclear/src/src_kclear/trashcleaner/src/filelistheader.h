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


#include "FileListData.h"
#define MAX_COLUN 10


class CFileListHeader : public CWindowImpl<CFileListHeader,CHeaderCtrl>
{
// Construction
public:
	CFileListHeader();
    virtual ~CFileListHeader();

// Attributes
public:
    COLORREF m_clrBkGnd ;
    COLORREF m_clrTextColor;
    DWORD m_dwFileCount;
    ULONGLONG m_ulFileSize;
    CString m_strFileName;
    CString m_strUsedSize;
    CString m_strClearFlag;
    TRASHFILELIST* m_pFileList;
	int		m_nFoucusIdx;

// Operations
	BOOL	SwitchArrow(int nColumn, BOOL bSortAscending);
	BOOL	SetFocusIdx(int nColumn);
	int		GetFocusIdx() const;
    BOOL	_HeaderYHitTest(POINT &pt);			//仅测试y值是否在header上

protected:
   
	int			m_iSortColumn;
	BOOL		m_pbSortAscending[MAX_COLUN];

	BOOL	DrawItemHeader(CPaintDC &dc, int nItmIdx, CString strTitle);
	BOOL	DrawRectIcon(CPaintDC &dc, RECT rcSubItem, const int iOffset, BOOL bAscending);


public:
    BEGIN_MSG_MAP(CDirectoryListCtrl)
        MSG_WM_PAINT(OnPaint)
        MESSAGE_HANDLER(HDM_LAYOUT, OnLayOut)
    END_MSG_MAP()

protected:
	void OnPaint(HDC pDC);
    LRESULT OnLayOut(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LRESULT lRes = 0L;
        bHandled = TRUE;
        HD_LAYOUT *playout	=(HD_LAYOUT*)lParam;
        RECT&      rect = *(playout->prc);
        WINDOWPOS& pos	= *(playout->pwpos);

        lRes = DefWindowProc(HDM_LAYOUT, wParam, lParam);

        pos.cy   = 18;
        rect.top = 18;

        return lRes;

    }

};


