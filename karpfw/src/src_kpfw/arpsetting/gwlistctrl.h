#pragma once

//////////////////////////////////////////////////////////////////////////

#include "skingdi.h"

//////////////////////////////////////////////////////////////////////////

class CGwListHdrCtrl;
typedef CWindowImpl<CGwListHdrCtrl, CHeaderCtrl> CGwListHdrCtrlBase;

//////////////////////////////////////////////////////////////////////////

class CGwListHdrCtrl 
    : public CGwListHdrCtrlBase
    , public CDoubleBufferImpl<CGwListHdrCtrl>
{
public:
    BEGIN_MSG_MAP(CGwListHdrCtrl)
        MSG_WM_SIZE(OnSize)
        MESSAGE_HANDLER(HDM_LAYOUT, OnHdmLayout)
        CHAIN_MSG_MAP(CDoubleBufferImpl<CGwListHdrCtrl>)
    END_MSG_MAP()

    void Init()
    {
        m_hFont = GetParent().GetParent().GetFont();
        GetClientRect(&m_rcClient);
    }

    BOOL SubclassWindow(HWND hWnd)
    {
        BOOL retval = FALSE;
        retval = CGwListHdrCtrlBase::SubclassWindow(hWnd);

        Init();
        return retval;
    }

    LRESULT OnHdmLayout(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LRESULT retval;
        HDLAYOUT* pHdLayout = (HDLAYOUT*)lParam;

        retval = DefWindowProc(uMsg, wParam, lParam);
        if (pHdLayout)
        {
            pHdLayout->pwpos->cy = 24;
            pHdLayout->prc->top = 24;
            pHdLayout->prc->bottom -= 4;
        }

        return 0;
    }

    void DoPaint(CDCHandle dc)
    {
        //CPaintDC dc(m_hWnd);
        CRect rcTemp;
        CFontHandle font = m_hFont;
        CRect rcSplit, rcHalf;
        int nItemCount = GetItemCount();

        GetClientRect(&m_rcClient);
        rcTemp = m_rcClient;
        rcTemp.bottom -= 1;
        rcHalf = rcTemp;
        rcHalf.bottom = rcTemp.Height() / 2;
        DrawGradualColorRect(dc, rcHalf, RGB(255,255,255), RGB(248,253,255));
        rcHalf.top = rcHalf.bottom;
        rcHalf.bottom = m_rcClient.bottom - 1;
        DrawGradualColorRect(dc, rcHalf, RGB(229,246,254), RGB(238,249,255));
        rcTemp.top = m_rcClient.bottom - 1;
        rcTemp.bottom = m_rcClient.bottom;
        dc.FillSolidRect(rcTemp, RGB(150,174,202));

        dc.SetBkMode(TRANSPARENT);
        dc.SetTextColor(RGB(0,0,0));
        dc.SelectFont(font);
        for (int i = 0; i < nItemCount; ++i)
        {
            HDITEM hdi;
            CString strText;

            hdi.mask = HDI_TEXT;
            hdi.pszText = strText.GetBuffer(MAX_PATH);
            hdi.cchTextMax = MAX_PATH;

            GetItem(i, &hdi);
            strText.ReleaseBuffer();
            GetItemRect(i, rcTemp);

            dc.DrawText(strText, strText.GetLength(), rcTemp, 
                DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

            rcSplit.left = rcTemp.right - 2;
            rcSplit.right = rcTemp.right - 1;
            rcSplit.top = rcTemp.top + 4;
            rcSplit.bottom = rcTemp.bottom - 3;
            dc.FillSolidRect(rcSplit, RGB(162,186,212));
            rcSplit.left = rcTemp.right - 1;
            rcSplit.right = rcTemp.right - 0;
            dc.FillSolidRect(rcSplit, RGB(255,255,255));
        }
    }

    void OnSize(UINT nType, CSize size)
    {
        GetClientRect(&m_rcClient);
    }

private:
    RECT m_rcClient;
    CFontHandle m_hFont;
};

//////////////////////////////////////////////////////////////////////////

class CGwListCtrl;
typedef CWindowImpl<CGwListCtrl, CListViewCtrl> CGwListCtrlBase;

class CGwListCtrl 
    : public CGwListCtrlBase
    , public COwnerDraw<CGwListCtrl>
{
public:
    DECLARE_WND_CLASS(_T("GatewayListView"))

    CGwListCtrl() 
    {
        m_nHeaderHeight = 24;
        m_nSelected = -1;
    }

    BEGIN_MSG_MAP_EX(CGwListCtrl)
        MSG_WM_SIZE(OnSize)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
        CHAIN_MSG_MAP_ALT(COwnerDraw<CGwListCtrl>, 1)
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()

    void Init()
    {
        m_hFont = GetParent().GetFont();
        ModifyStyle(0, LVS_SINGLESEL);
        m_toolTip.Create(m_hWnd);
        m_toolTip.SetMaxTipWidth(500);
        m_toolTip.AddTool(m_hWnd, L"Test");
        m_toolTip.Activate(TRUE);
    }

    BOOL SubclassWindow(HWND hWnd)
    {
        BOOL retval = FALSE;
        retval = CGwListCtrlBase::SubclassWindow(hWnd);
        m_HdrCtrl.SubclassWindow(GetHeader().m_hWnd);

        Init();
        return retval;
    }

	BOOL OnEraseBkgnd(CDCHandle dc)
	{
		CRect rcClient;
		GetClientRect(rcClient);
		dc.FillSolidRect(rcClient, RGB(0xff,0xff,0xff));
		return TRUE;
	}

    void OnSize(UINT nType, CSize size)
    {
        CHeaderCtrl hdr = this->GetHeader();
        CRect rcHeader;
        hdr.GetClientRect(rcHeader);
        m_nHeaderHeight = rcHeader.Height();
    }

    void DeleteItem(LPDELETEITEMSTRUCT lParam)
    {

    }

    void DrawItem(PDRAWITEMSTRUCT lpDrawItem)
    {
        BOOL bSelect = lpDrawItem->itemState & ODS_SELECTED;
        CRect rcItem = lpDrawItem->rcItem;
        CDCHandle dc = lpDrawItem->hDC;
        int nItem = lpDrawItem->itemID;
        int nColumnCount = 2;
        int i;
        int nLeft;

        if (bSelect)
        {
            dc.FillSolidRect(rcItem, RGB(0xaa,0xaa,0xff));
            m_nSelected = nItem;
        }
        else
        {
            dc.FillSolidRect(rcItem, RGB(0xff,0xff,0xff));
        }

        dc.SetBkMode(TRANSPARENT);

        nLeft = rcItem.left;
        for (i = 0; i < nColumnCount; ++i)
        {
            CRect rcDraw = rcItem;
            CString strText;

            rcDraw.left = nLeft;
            rcDraw.right = nLeft + GetColumnWidth(i);
            nLeft = rcDraw.right;
            GetItemText(nItem, i, strText);
            dc.DrawText(strText, strText.GetLength(), rcDraw, DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);
        }
    }

    LRESULT CompareItem(LPCOMPAREITEMSTRUCT lParam)
    {
        return 0;
    }

private:
    CFontHandle m_hFont;
    int m_nHeaderHeight;
    CGwListHdrCtrl m_HdrCtrl;
    CToolTipCtrl m_toolTip;
    int m_nSelected;
};

//////////////////////////////////////////////////////////////////////////
