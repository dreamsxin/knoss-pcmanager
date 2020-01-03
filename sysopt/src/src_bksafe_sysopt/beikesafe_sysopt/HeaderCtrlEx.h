#pragma once

class CHeaderCtrlEx : public CWindowImpl<CHeaderCtrlEx, CHeaderCtrl>
{
public:
	CHeaderCtrlEx();
	virtual ~CHeaderCtrlEx();
	void				SetHeight(INT nHeight);

protected:
	void				OnPaint(CDCHandle dc);
	BOOL				OnEraseBkgnd(CDCHandle dc);
	LRESULT				OnHDMLayout(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT				OnMouseMove(UINT nFlags, CPoint point);

protected:
	CBitmap				m_bitmapHeader;
	CFont				m_font;
	INT					m_nHeight;

	CRBMap<int, BOOL>	m_mapHeaderDrag;

public:
	BEGIN_MSG_MAP_EX(CHeaderCtrlEx) 
		MSG_WM_PAINT(OnPaint)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MESSAGE_HANDLER(HDM_LAYOUT, OnHDMLayout)
	END_MSG_MAP()
};
