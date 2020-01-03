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

// FileListHeader.cpp : implementation file
//

#include "stdafx.h"
#include "trashcleaner.h"
#include "FileListHeader.h"
#include "trashdefine.h"
#include "trashpublicfunc.h"

/////////////////////////////////////////////////////////////////////////////
// CFileListHeader
CFileListHeader::CFileListHeader() :
m_clrBkGnd(RGB(10, 16, 116)),
m_clrTextColor(RGB(9, 9, 9)),
m_iSortColumn(-1)
{
	m_dwFileCount = 0;
	m_ulFileSize  = 0;
	m_nFoucusIdx = 1;
	m_pFileList = NULL;

	for (int i = 0; i < MAX_COLUN; i++)
	{
		m_pbSortAscending[i] = FALSE;
	}

    m_strFileName.LoadString(IDS_FILECOUNT);
    m_strUsedSize.LoadString(IDS_FILESIZE);
    m_strClearFlag.LoadString(IDS_CLEARFLAG);
}

CFileListHeader::~CFileListHeader()
{
}




/////////////////////////////////////////////////////////////////////////////
// CFileListHeader message handlers

extern void DrawHeaderBg(CDC *pDC, CRect rcRect, LONG lFirstItemSize);


BOOL CFileListHeader::DrawRectIcon(CPaintDC &dc, RECT rcSubItem, const int iOffset, BOOL bAscending)		//共宽2*offset - 2
{	

 	CPen penShadow;
    penShadow.CreatePen( PS_SOLID, 1, GetSysColor( COLOR_3DSHADOW ) );

    #define UP_PADING 2
	const int nUP = iOffset - UP_PADING + 1;
	const int nDown = rcSubItem.bottom - 2 - iOffset - UP_PADING - 1;
	const int nLeft = rcSubItem.right - 3 * iOffset;
	const int nRight = rcSubItem.right - iOffset;
	const int nMid = (nLeft + nRight) / 2;
	
	//CBrush *pBrush = NULL;
	//CBrush brushShadow;

	//brushShadow.CreateSolidBrush(RGB(172, 168, 153));
//	pBrush = (CBrush*)dc.SelectObject(&brushShadow);
	HGDIOBJ hOld = (HGDIOBJ)SelectObject((HDC)dc,&penShadow );
	POINT pt1, pt2;// pt3;

	if(bAscending)
	{
		// draw the arrow pointing upwards.
/*
		pt1.x = nLeft - 1;	pt1.y = nDown;
		pt2.x = nRight + 1; pt2.y = nDown;
		pt3.x = nMid;	pt3.y = nUP;*/
		int nBottom = ((rcSubItem.bottom + rcSubItem.top) / 2);
		pt1.x = nLeft;		pt1.y = nBottom;
		pt2.x = nLeft + 7;	pt2.y = nBottom;
		dc.MoveTo(pt1);		dc.LineTo(pt2);
		pt1.x = nLeft + 1;	pt1.y = nBottom - 1;
		pt2.x = nLeft + 6;	pt2.y = nBottom - 1;
		dc.MoveTo(pt1);		dc.LineTo(pt2);
		pt1.x = nLeft + 2;	pt1.y = nBottom - 2;
		pt2.x = nLeft + 5;	pt2.y = nBottom - 2;
		dc.MoveTo(pt1);		dc.LineTo(pt2);
		pt1.x = nLeft + 3;	pt1.y = nBottom - 3;
		pt2.x = nLeft + 4;	pt2.y = nBottom - 3;
		dc.MoveTo(pt1);		dc.LineTo(pt2);
		
/*
		dc.MoveTo(nMid, nUP);
		dc.LineTo( nRight, nDown);
		dc.LineTo( nLeft, nDown);
		(void)dc.SelectObject(&penShadow );
		dc.MoveTo(nLeft, nDown);
		dc.LineTo(nMid, nUP);	*/
	}
	else
	{
/*
		pt1.x = nLeft;	pt1.y = nUP;
		pt2.x = nRight; pt2.y = nUP;
		pt3.x = nMid;	pt3.y = nDown;*/

		// draw the arrow pointing downwards.
		int nR = ((rcSubItem.bottom + rcSubItem.top) / 2) - 3;
		pt1.x = nLeft;		pt1.y = nR;
		pt2.x = nLeft + 7;	pt2.y = nR;
		dc.MoveTo(pt1);		dc.LineTo(pt2);
		pt1.x = nLeft + 1;	pt1.y = nR + 1;
		pt2.x = nLeft + 6;	pt2.y = nR + 1;
		dc.MoveTo(pt1);		dc.LineTo(pt2);
		pt1.x = nLeft + 2;	pt1.y = nR + 2;
		pt2.x = nLeft + 5;	pt2.y = nR + 2;
		dc.MoveTo(pt1);		dc.LineTo(pt2);
		pt1.x = nLeft + 3;	pt1.y = nR + 3;
		pt2.x = nLeft + 4;	pt2.y = nR + 3;
		dc.MoveTo(pt1);		dc.LineTo(pt2);
	
	}

	SelectObject((HDC)dc,hOld);
	return TRUE;
}

BOOL CFileListHeader::DrawItemHeader(CPaintDC &dc, int nItmIdx, CString strTitle)
{
	if (nItmIdx < 0 || nItmIdx >= 2)
    {
		return FALSE;
	}
	RECT	rt;
	GetItemRect(nItmIdx, &rt);
	const int iOffset = ( rt.bottom - rt.top ) / 5;
	if (nItmIdx == m_nFoucusIdx)
	{
		if (2 * iOffset - 1 < rt.right - rt.left)
			DrawRectIcon(dc, rt, iOffset, m_pbSortAscending[nItmIdx]);
	}
	
	RECT rtText = rt;
	rtText.left += 5;
	rtText.right -= 5;
	if (rtText.right < rtText.left)
		rtText.right = rtText.left;
	DrawText(dc, strTitle, -1, &rtText, 
			DT_WORD_ELLIPSIS | DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	if (1 == nItmIdx)		//仅仅在绘制中间一格时绘制分隔线
	{
		RECT rcSpl = rt;
		rcSpl.top += 1;
		rcSpl.bottom -= 4;
		rcSpl.right = rcSpl.left;

		dc.Draw3dRect(&rcSpl, 
			RGB(255, 255, 255),
			RGB(200, 200, 200));

		rcSpl.left  = rt.right;
		rcSpl.right = rcSpl.left;
		dc.Draw3dRect(&rcSpl, 
			RGB(255, 255, 255),
			RGB(200, 200, 200));
	}
	
	return TRUE;
}

void CFileListHeader::OnPaint(HDC pDC) 
{
	CPaintDC dc(m_hWnd); 

	CRect rcClient;

	GetClientRect(&rcClient);


	CBrush brush;
	brush.CreateSolidBrush(m_clrBkGnd);

	int nItems = GetItemCount();
	DrawHeaderBg(&dc, rcClient, 0);

	HGDIOBJ pOldFont = SelectObject((HDC)dc,GetFont());
	dc.SetTextColor(m_clrTextColor);
	dc.SetBkMode(TRANSPARENT);

	CString strColumnName0;
	CString strColumnName1;
	CString strColumnName2;

	if (m_pFileList != NULL)
	{
		m_dwFileCount = m_pFileList->itemArray.size();
		m_ulFileSize  = m_pFileList->ulAllFileSize;
	}

	strColumnName0.Format(m_strFileName);
	strColumnName1.Format(m_strUsedSize, GetFileSizeText(m_ulFileSize));
	strColumnName2 = m_strClearFlag;


	//下面的过程开始绘制
	DrawItemHeader(dc, 0, strColumnName0);
	DrawItemHeader(dc, 1, strColumnName1);
    //DrawItemHeader(dc, 2, strColumnName2);

	SelectObject((HDC)dc,pOldFont);
}


int CFileListHeader::GetFocusIdx() const
{
	return m_nFoucusIdx;
}

BOOL CFileListHeader::SetFocusIdx(int nColumn)
{
	if (nColumn < 0 || nColumn >= GetItemCount())
	{
		return FALSE;
	}
	m_nFoucusIdx = nColumn;
	return TRUE;
}

BOOL CFileListHeader::SwitchArrow(int nColumn, BOOL bSortAscending)
{
	if (nColumn < 0 || nColumn >= GetItemCount())
		return FALSE;
	m_pbSortAscending[nColumn] = bSortAscending;
	SetFocusIdx(nColumn);

	HD_ITEM hditem;
	hditem.mask = HDI_FORMAT;
	if (m_pbSortAscending[nColumn])
	{
		hditem.fmt |= HDF_SORTUP;
	}
	else
	{
		hditem.fmt |= HDF_SORTDOWN;
	}
	return TRUE;
}

BOOL CFileListHeader::_HeaderYHitTest(POINT &pt)
{
	RECT rt;
	GetClientRect(&rt);
	if (pt.y >= rt.top && pt.y <= rt.bottom)
	{
		return TRUE;
	}
	return FALSE;
}
