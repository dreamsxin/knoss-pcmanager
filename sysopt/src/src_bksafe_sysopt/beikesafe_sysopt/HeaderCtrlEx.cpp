#include "stdafx.h"
#include <algorithm>
#include "beikesafe.h"
#include "HeaderCtrlEx.h"

#define COLOR_LIST_LINK				RGB(40,100,165)	
#define LEFT_MARGIN_TEXT_COLUMN		2

CHeaderCtrlEx::CHeaderCtrlEx()
{
	m_nHeight = -1;
	m_font.Attach( BkFontPool::GetFont(BKF_DEFAULTFONT) );
	m_bitmapHeader.Attach( BkBmpPool::GetBitmap(IDB_BITMAP_LISTCTRL_HEADER));
}

CHeaderCtrlEx::~CHeaderCtrlEx()
{
	m_font.Detach();
	m_bitmapHeader.Detach();
}

void CHeaderCtrlEx::SetHeight(INT nHeight)
{
	m_nHeight = nHeight;
}

void CHeaderCtrlEx::OnPaint( CDCHandle )
{	
	CPaintDC dc(m_hWnd);
	CDC dcTemp;

	dcTemp.CreateCompatibleDC( dc );
	HBITMAP hOldBitmap = dcTemp.SelectBitmap(m_bitmapHeader);

	HFONT hOldFont = dc.SelectFont( m_font );
	dc.SetBkMode( TRANSPARENT );

	RECT rc = {0};
	GetClientRect( &rc );	
	dc.StretchBlt(rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, dcTemp, 0, 0, 3, 20, SRCCOPY);
	int nItems = GetItemCount();
	for(int i=0; i<nItems; ++i)
	{
		CRect rcItem;
		GetItemRect(i, &rcItem);

		// Draw split 
		dc.StretchBlt(rcItem.right-3, rcItem.top, 3, rcItem.Height(), dcTemp, 3, 0, 3, 20, SRCCOPY);

		// Draw text
		rcItem.DeflateRect(2, 0);
		TCHAR buf[255] = {0};
		HDITEM hdItem = {0};

		hdItem.mask	 = HDI_TEXT | HDI_FORMAT | HDI_ORDER;
		hdItem.pszText = buf;
		hdItem.cchTextMax = 255;

		GetItem(i, &hdItem);
		UINT uFormat = DT_SINGLELINE | DT_END_ELLIPSIS | DT_VCENTER;
		UINT uAlign = DT_LEFT;
		if(hdItem.fmt & HDF_CENTER)
			uAlign = DT_CENTER;
		else if(hdItem.fmt & HDF_RIGHT)
			uAlign = DT_RIGHT;
		uFormat |= uAlign;

		dc.DrawText(buf, -1, rcItem, uFormat);
	}
	dc.SelectFont(hOldFont);	
	dcTemp.SelectBitmap(hOldBitmap);	
}

BOOL CHeaderCtrlEx::OnEraseBkgnd( CDCHandle dc )
{
	return TRUE;
}

LRESULT CHeaderCtrlEx::OnHDMLayout(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lResult = CWindowImpl<CHeaderCtrlEx, CHeaderCtrl>::DefWindowProc(uMsg, wParam, lParam);
	HD_LAYOUT &hdl = *( HD_LAYOUT * ) lParam;
	RECT *prc = hdl.prc;
	WINDOWPOS *pwpos = hdl.pwpos;
	int nHeight = (int)(pwpos->cy);
	if(m_nHeight>=0)
		nHeight = m_nHeight;
	pwpos->cy = nHeight;
	prc->top = nHeight;
	return lResult;
}

LRESULT CHeaderCtrlEx::OnMouseMove( UINT nFlags, CPoint point )
{
	SetMsgHandled(FALSE);
	return S_OK;
}