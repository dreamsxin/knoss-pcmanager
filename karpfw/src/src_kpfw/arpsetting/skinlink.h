/********************************************************************
created:	2010/03/15
created:	15:3:2010   15:09
filename: 	kppskin\skinlink.h
author:		Jiang Fengbing

purpose:	超链接
*********************************************************************/

#ifndef SKINLINK_INC_
#define SKINLINK_INC_

//////////////////////////////////////////////////////////////////////////

class CSkinLink;
typedef CWindowImpl<CSkinLink, CWindow, CWinTraits<WS_CHILD | WS_VISIBLE | BS_OWNERDRAW> > CSkinLinkBase;

//////////////////////////////////////////////////////////////////////////

class CSkinLink : public CSkinLinkBase
{
public:
    DECLARE_WND_SUPERCLASS(NULL, _T("STATIC"))

    CSkinLink() : m_fBold(FALSE), m_colorLink(RGB(60, 114, 168))
        , m_fUnderline(TRUE), m_fEnable(TRUE)
    {
        m_hCursor = ::LoadCursor(NULL, IDC_HAND);
    }

    ~CSkinLink()
    {
        // 不需要是否m_hCursor在Win2K以后
    }

    BEGIN_MSG_MAP_EX(CSkinLink)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
        MSG_WM_PAINT(OnPaint)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_MOUSELEAVE(OnMouseLeave)
        MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
        REFLECT_NOTIFICATIONS()
        END_MSG_MAP()

        BOOL SetWindowText(LPCTSTR lpszString) throw() {
            ::SetWindowText(m_hWnd, lpszString);
            ShowWindow(SW_HIDE);
            ShowWindow(SW_SHOWNORMAL);
            return TRUE;
    }

    void Init()
    {
        ModifyStyle(0, SS_NOTIFY);
    }

    BOOL SubclassWindow(HWND hWnd)
    {
        BOOL bRet = CSkinLinkBase::SubclassWindow(hWnd);
        if(bRet)
            Init();
        return bRet;
    }

    LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 1;
    }

    void DrawText(HDC hDC)
    {
        WCHAR szText[512] = { 0 };
        HGDIOBJ hOld = NULL;
        CFont fontBold;
        CFontHandle wndFont(GetFont());
        COLORREF colorOld;
        BOOL bEnable;

        bEnable = IsWindowEnabled();

        if (m_fBold || m_fUnderline)
        {
            LOGFONT logFont;
            wndFont.GetLogFont(&logFont);
            if (m_fBold)
                logFont.lfWeight = FW_BOLD;
            if (m_fUnderline)
                logFont.lfUnderline = TRUE;
            fontBold.CreateFontIndirect(&logFont);
            hOld = SelectObject(hDC, fontBold);
        }
        else
        {
            hOld = SelectObject(hDC, wndFont);
        }
        
        GetWindowText(szText, 512);
        SetBkMode(hDC, TRANSPARENT);
        colorOld = SetTextColor(hDC, bEnable ? m_colorLink : RGB(0xCC,0xCC,0xCC));
        TextOutW(hDC, 0, 0, szText, (int)wcslen(szText));
        SetTextColor(hDC, colorOld);
        SelectObject(hDC, hOld);
    }

    void OnPaint(CDCHandle /*dc*/)
    {
        CPaintDC dc(m_hWnd);
        CRect rcClient;

        GetClientRect(rcClient);
        dc.FillSolidRect(rcClient, RGB(0xff,0xff,0xff));
        DrawText(dc);
    }

    void OnMouseMove(UINT nFlags, CPoint point)
    {
        //POINT pt = { point.x, point.y };
        RECT rcLink;
        GetClientRect(&rcLink);

        if (::PtInRect(&rcLink, point))
        {
            if (m_fEnable)
                ::SetCursor(m_hCursor);
        }
    }

    void OnMouseLeave()
    {

    }

    LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        POINT pt = { 0, 0 };
        RECT rcLink;

        GetCursorPos(&pt);
        GetClientRect(&rcLink);
        ScreenToClient(&pt);

        if (::PtInRect(&rcLink, pt) && m_fEnable)
        {
            return TRUE;
        }

        bHandled = FALSE;
        return FALSE;
    }

//     void Enable(BOOL fEnable = TRUE)
//     {
//         m_fEnable = fEnable;
//         if (FALSE == m_fEnable)
//         {
//             m_fBold = FALSE;
//             m_fUnderline = FALSE;
//             m_colorLink = RGB(0, 0, 0);
//         }
//         else
//         {
//             m_fBold = FALSE;
//             m_fUnderline = TRUE;
//             m_colorLink = RGB(60, 114, 168);
//         }
//         InvalidateRect(NULL);
//     }

private:
    BOOL m_fBold;
    BOOL m_fUnderline;
    BOOL m_fTracking;
    COLORREF m_colorLink;
    HCURSOR m_hCursor;
    BOOL m_fEnable;
};

//////////////////////////////////////////////////////////////////////////

#endif	// SKINLINK_INC_
