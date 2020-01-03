#ifndef SKINDLG_INC_
#define SKINDLG_INC_

//////////////////////////////////////////////////////////////////////////

#include "skingdi.h"
#include "skinbutton.h"

//////////////////////////////////////////////////////////////////////////

template <class T>
class CSkinDialog {
public:
    CSkinDialog() : m_fInited(false) {

    }

    BEGIN_MSG_MAP_EX(CSkinDialog)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_PAINT(OnPaint)
        MSG_WM_SIZE(OnSize)
    END_MSG_MAP()

    void InitSkinDialog(HBITMAP hClose, HBITMAP hMin, const CString& strTitle= L"") {
        T* pT = static_cast<T*>(this);
        RECT rcClient = { 0 };
        CRgn myRgn;

        pT->ModifyStyle(WS_CAPTION, 0);
        pT->ModifyStyle(DS_MODALFRAME, 0);
        pT->ModifyStyleEx(WS_EX_DLGMODALFRAME, 0);
        pT->ModifyStyleEx(WS_EX_WINDOWEDGE, 0);
        GetDialogSize();

        if (hClose) {
            m_btnClose.Create(pT->m_hWnd);
            m_btnClose.InitCloseButton(hClose);
            m_btnClose.SetWindowLong(GWL_ID, SC_CLOSE);
        }

        if (hMin) {
            m_btnMin.Create(pT->m_hWnd);
            m_btnMin.InitButton(hMin, -39, 1);
            m_btnMin.SetWindowLong(GWL_ID, SC_MINIMIZE);
        }

        pT->SetWindowText(strTitle);

        pT->UpdateWindow();

        m_fInited = true;
    }

    void SetBkSkin(HBITMAP hBkSkin) {
        m_bmpSkin = hBkSkin;
    }

    void GetDialogSize() {
        T* pT = static_cast<T*>(this);
        GetClientRect(pT->m_hWnd, &m_rcClient);
    }

    LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
        return 1L;
    }

    void DrawBorder(CPaintDC& dc) {
        RECT rcLeft = { 0, 32, 2, m_rcClient.bottom };
        RECT rcRight = { m_rcClient.right - 2, 32, m_rcClient.right, m_rcClient.bottom };
        RECT rcBottom = { 0, m_rcClient.bottom - 2, m_rcClient.right, m_rcClient.bottom };
        dc.FillSolidRect(&rcLeft, RGB(1, 93, 140));
        dc.FillSolidRect(&rcRight, RGB(1, 93, 140));
        dc.FillSolidRect(&rcBottom, RGB(1, 93, 140));
    }

    void DrawTitle(CPaintDC& dc) {
        CDC memDC;
        memDC.CreateCompatibleDC(dc);
        memDC.SelectBitmap(m_bmpSkin);
        dc.BitBlt(0, 0, 4, 32, memDC, 0, 0, SRCCOPY);
        dc.BitBlt(m_rcClient.right - 4, 0, 4, 32, memDC, 7, 0, SRCCOPY);
        dc.StretchBlt(4, 0, m_rcClient.right - 8, 32, memDC, 6, 0, 1, 32, SRCCOPY);

        RECT rcTitle = { 0, 0, m_rcClient.right, 32 };
        WCHAR szTitle[64] = { 0 };
        T* pT = static_cast<T*>(this);
        ::GetWindowTextW(pT->m_hWnd, szTitle, 64);
        CFontHandle wndFont(pT->GetFont());
        CFont titleFont;
        LOGFONT logFont;
        wndFont.GetLogFont(&logFont);
        logFont.lfWeight = FW_BOLD;
        titleFont.CreateFontIndirect(&logFont);
        HGDIOBJ hOld = SelectObject(dc, titleFont);
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(255,255,255));
        rcTitle.left += 24;
        DrawTextW(dc, szTitle, (int)wcslen(szTitle), &rcTitle, 
            DT_VCENTER|DT_SINGLELINE);
        SelectObject(dc, hOld);

    }

    void DrawIcon(CPaintDC& dc) {
        T* pT = static_cast<T*>(this);
        HICON hIcon = pT->GetIcon(FALSE);
        if (hIcon) {
            dc.DrawIconEx(6, 6, hIcon, 16, 16);
        }
    }

    void DrawBackground(CPaintDC& dc) {
        dc.FillSolidRect(&m_rcClient, RGB(255, 255, 255));
    }

    void OnPaint(CDCHandle /*dc*/) {
        T* pT = static_cast<T*>(this);
        CPaintDC dc(pT->m_hWnd);
        GetDialogSize();

        DrawBackground(dc);
        DrawTitle(dc);
        DrawIcon(dc);
        DrawBorder(dc);
        pT->OnPaint(dc);
    }

    bool InTitleZone(CPoint& pt) {
        if (pt.y <= 24)
            return true;
        else
            return false;
    }

    void OnLButtonDown(UINT nFlags, CPoint point)  {
        T* pT = static_cast<T*>(this);
        if (InTitleZone(point)) {
            pT->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x,point.y));
        }
    }

    void OnSize(UINT nType, CSize size)
    {
        T *pT = static_cast<T*>(this);

        if (nType == SIZE_MINIMIZED)
            return;

        if (size == m_rcWnd)
            return;

        CRect rcWindow, rcClient;
        CRgn rgnWindow, rgnMinus, rgnAdd;

        pT->GetWindowRect(rcWindow);
        pT->GetClientRect(rcClient);
        pT->ClientToScreen(rcClient);

        rcClient.OffsetRect(- rcWindow.TopLeft());

        rgnWindow.CreateRectRgn(rcClient.left, rcClient.top + 2, rcClient.right, rcClient.bottom - 2);
        rgnAdd.CreateRectRgn(rcClient.left + 2, rcClient.top, rcClient.right - 2, rcClient.top + 1);
        rgnWindow.CombineRgn(rgnAdd, RGN_OR);
        rgnAdd.OffsetRgn(0, rcClient.Height() - 1);
        rgnWindow.CombineRgn(rgnAdd, RGN_OR);
        rgnAdd.SetRectRgn(rcClient.left + 1, rcClient.top + 1, rcClient.right - 1, rcClient.top + 2);
        rgnWindow.CombineRgn(rgnAdd, RGN_OR);
        rgnAdd.OffsetRgn(0, rcClient.Height() - 3);
        rgnWindow.CombineRgn(rgnAdd, RGN_OR);

        pT->SetWindowRgn(rgnWindow);

        m_rcWnd = size;
        pT->GetClientRect(m_rcClient);
        pT->GetDlgItem(SC_CLOSE).SetWindowPos(NULL, m_rcClient.right - 43, 0, 0, 0, SWP_NOSIZE);
    }

private:
    CSkinButton m_btnClose;
    CSkinButton m_btnMin;
    bool m_fInited;
    CRect m_rcClient;
    CBitmapHandle m_bmpSkin;
    CSize m_rcWnd;
};

//////////////////////////////////////////////////////////////////////////

#endif  // !SKINDLG_INC_
