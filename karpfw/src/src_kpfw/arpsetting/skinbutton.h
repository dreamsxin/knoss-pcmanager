#ifndef SKINBUTTON_INC_
#define SKINBUTTON_INC_


//////////////////////////////////////////////////////////////////////////

class CSkinButton;
typedef CWindowImpl<CSkinButton, CWindow, 
CWinTraits<WS_CHILD | WS_VISIBLE | BS_OWNERDRAW> > CSkinButtonBase;

class CSkinButton : public CSkinButtonBase
{
public:
    DECLARE_WND_SUPERCLASS(NULL, _T("BUTTON"))

    CSkinButton() : m_hCursor(FALSE), m_fBold(FALSE), m_nWidth(0), m_nHeight(0)
        , m_colorText(RGB(0, 0, 0))
    {
        m_bButtonDown = m_bTracking = FALSE;
    }

    ~CSkinButton()
    {
    }

    typedef enum _BUTTON_STATUS {
        enumBS_Normal = 0,
        enumBS_Hover = 1,	
        enumBS_Down = 2,	
    } BUTTON_STATUS;

    BEGIN_MSG_MAP_EX(CSkinButton)
        MESSAGE_HANDLER(OCM_DRAWITEM, OnDrawItem)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUP)
        MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
        MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
        MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    void InitButton(HBITMAP hBitmap, int nPosX = 0, int nPosY = 0)
    {
        m_hBitmap = hBitmap;
        GetButtonSize();
        ResizeForButton(nPosX, nPosY);
    }

    void InitCloseButton(HBITMAP hBitmap)
    {
        m_hBitmap = hBitmap;
        GetButtonSize();
        ResizeForCloseButton();
    }

    void ResizeForCloseButton()
    {
        RECT rcClient;
        CWindow wndParent = GetParent();
        wndParent.GetClientRect(&rcClient);
        int nPosX = rcClient.right - m_nWidth;
        int nPosY = 0;

        SetWindowPos(NULL, nPosX, nPosY, m_nWidth, m_nHeight, SWP_SHOWWINDOW);
    }

    void ResizeForButton(int nPosX, int nPosY)
    {

        if (nPosX < 0)
        {
            RECT rcClient;
            CWindow wndParent = GetParent();
            wndParent.GetClientRect(&rcClient);
            nPosX = rcClient.right + nPosX - m_nWidth;
        }

        if (nPosY < 0)
        {
            RECT rcClient;
            CWindow wndParent = GetParent();
            wndParent.GetClientRect(&rcClient);
            nPosY = rcClient.bottom + nPosY - m_nHeight;
        }

        if (!nPosX && !nPosY) {
            SetWindowPos(NULL, nPosX, nPosY, m_nWidth, m_nHeight, SWP_SHOWWINDOW|SWP_NOMOVE);
        } else {
            SetWindowPos(NULL, nPosX, nPosY, m_nWidth, m_nHeight, SWP_SHOWWINDOW);
        }
    }

    LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        m_bTracking = FALSE;
        Invalidate();
        return 0;
    }

    LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
        HDC hdcMem = ::CreateCompatibleDC(lpdis->hDC);
        HBITMAP hbmMem = ::CreateCompatibleBitmap(lpdis->hDC,
            lpdis->rcItem.right - lpdis->rcItem.left,
            lpdis->rcItem.bottom - lpdis->rcItem.top);

        HGDIOBJ hbmOld = ::SelectObject(hdcMem, (HGDIOBJ)hbmMem);
        HBRUSH transparent = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
        RECT rect={lpdis->rcItem.left, lpdis->rcItem.top, lpdis->rcItem .right, lpdis->rcItem.bottom };
        int cx = rect.right - rect.left;
        int cy = rect.bottom -rect.top;

        BOOL bEnable = IsWindowEnabled();
        RECT rcButton = { 0, 0, m_nWidth, m_nHeight };

        FillRect(hdcMem , &rect, transparent);


        POINT mouse_position;
        if ((m_bButtonDown) &&(::GetCapture() == m_hWnd) &&(::GetCursorPos(&mouse_position)))
        {
            if (::WindowFromPoint(mouse_position) == m_hWnd)
            {
                if ((GetState() & BST_PUSHED) != BST_PUSHED)
                {
                    SetState(TRUE);
                    return -1;
                }
            }
            else 
            {
                if ((GetState() & BST_PUSHED) == BST_PUSHED)
                {
                    SetState(FALSE);
                    return -1;
                }
            }
        }

        if (bEnable)
        {
            if (lpdis->itemState & ODS_SELECTED)
            {
                DrawBitmap(lpdis->hDC, enumBS_Down, rect); 
            }
            else
            {
                if (m_bTracking)
                {
                    DrawBitmap(lpdis->hDC, enumBS_Hover, rect); 
                }
                else
                {
                    DrawBitmap(lpdis->hDC, enumBS_Normal, rect); 
                }
            }
        }
        else
        {
            CPen border;
            CBrush bkground;
            border.CreatePen(PS_SOLID, 1, RGB(125,158,188));
            bkground.CreateSolidBrush(RGB(251,252,253));
            CDCHandle dc = lpdis->hDC;
            dc.SelectPen(border);
            dc.SelectBrush(bkground);
            dc.Rectangle(&rcButton);
        }

        WCHAR szTitle[64] = { 0 };
        HGDIOBJ hOld = NULL;

        GetWindowTextW(szTitle, 64);
        SetBkMode(lpdis->hDC, TRANSPARENT);
        if (IsWindowEnabled()) {
            ::SetTextColor(lpdis->hDC, m_colorText);
        } else {
            ::SetTextColor(lpdis->hDC, RGB(204, 204, 204));
        }

        if (m_fBold)
        {
            hOld = SelectObject(lpdis->hDC, m_fontBold);
            ::DrawTextW(lpdis->hDC, szTitle, (int)wcslen(szTitle), &rcButton, 
                DT_CENTER|DT_VCENTER|DT_SINGLELINE);
            ::SelectObject(lpdis->hDC, hOld);
        }
        else
        {
            ::DrawTextW(lpdis->hDC, szTitle, (int)wcslen(szTitle), &rcButton, 
                DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        }

        ::SelectObject(hdcMem, hbmOld);
        ::DeleteObject((HGDIOBJ)hbmMem);
        ::DeleteDC(hdcMem);

        return 0;
    }

public:

    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 0;
    }

    LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 1;
    }

    LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if (m_bTracking)
        {
            TRACKMOUSEEVENT t = 
            {
                sizeof(TRACKMOUSEEVENT),
                TME_CANCEL | TME_LEAVE,
                m_hWnd,
                0
            };
            if (::_TrackMouseEvent(&t))
            {
                m_bTracking = FALSE;
            }
        }


        m_bButtonDown = TRUE;

        bHandled = FALSE;

        return 0;
    }

    LRESULT OnLButtonUP(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        m_bButtonDown = FALSE;
        bHandled = FALSE;
        return 0;
    }

    LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if ((m_bButtonDown) &&(::GetCapture() == m_hWnd))
        {
            POINT p2;
            p2.x = LOWORD(lParam);
            p2.y = HIWORD(lParam);

            ::ClientToScreen(m_hWnd, &p2);
            HWND mouse_wnd = ::WindowFromPoint(p2);

            bool pressed = ((GetState() & BST_PUSHED) == BST_PUSHED);
            bool need_pressed = (mouse_wnd == m_hWnd);
            if (pressed != need_pressed)
            {
                SetState(need_pressed ? TRUE : FALSE);
                Invalidate();
            }
        }
        else 
        {
            if (!m_bTracking)
            {
                TRACKMOUSEEVENT t = 
                {
                    sizeof(TRACKMOUSEEVENT),
                    TME_LEAVE,
                    m_hWnd,
                    0
                };
                if (::_TrackMouseEvent(&t))
                {
                    m_bTracking = TRUE;
                    Invalidate();
                }
            }
        }
        bHandled = FALSE;
        return 0;
    }

    void GetButtonSize()
    {
        BITMAP bm;
        ::GetObject(m_hBitmap, sizeof(BITMAP), (PSTR)&bm); 
        m_nWidth = bm.bmWidth / 3;
        m_nHeight = bm.bmHeight;
    }

    void DrawBitmap(HDC hdc, BUTTON_STATUS btnStatus, RECT& rc)
    {
        if (!(HBITMAP)m_hBitmap)
        {
            return;
        }

        int nBmpPosX;
        if (!IsWindowEnabled())
            nBmpPosX = m_nWidth * 2;
        else
            nBmpPosX = btnStatus * m_nWidth;

        HDC hdcMem = CreateCompatibleDC(hdc);
        SelectObject(hdcMem, m_hBitmap);

        BitBlt(hdc, rc.left, rc.top, m_nWidth, m_nHeight, 
            hdcMem, nBmpPosX, 0, SRCCOPY);

        DeleteDC(hdcMem);
    }

    // 	void LoadBitmap(UINT uResId)
    // 	{
    // 		if ((HBITMAP)m_bmpSkin)
    // 		{
    // 			m_bmpSkin.DeleteObject();
    // 		}
    // 
    // 		m_bmpSkin.LoadBitmap(uResId);
    // 	}

    UINT GetState() const
    {
        return (UINT)::SendMessage(m_hWnd, BM_GETSTATE, 0, 0L);
    }

    void SetState(BOOL bHighlight)
    {
        ::SendMessage(m_hWnd, BM_SETSTATE, bHighlight, 0L);
    }	

    int GetWidth()
    {
        return m_nWidth;
    }

    int GetHeight()
    {
        return m_nHeight;
    }

    void Init()
    {
        ModifyStyle(0, BS_OWNERDRAW);
    }

    BOOL SubclassWindow(HWND hWnd)
    {
        BOOL bRet = CSkinButtonBase::SubclassWindow(hWnd);
        if(bRet)
            Init();
        return bRet;
    }

    CSkinButton& Enable(BOOL fEnable) {
        m_fEnable = fEnable;
        InvalidateRect(NULL);
        return *this;
    }

    CSkinButton& SetBold(BOOL fBold = TRUE) {
        m_fBold = fBold;
        if (m_fBold && !(HFONT)m_fontBold) {
            CFontHandle wndFont(GetFont());
            LOGFONT logFont;
            wndFont.GetLogFont(&logFont);
            logFont.lfWeight = FW_BOLD;
            m_fontBold.CreateFontIndirect(&logFont);
        }

        return *this;
    }

    CSkinButton& SetTextColor(COLORREF dwColor) {
        m_colorText = dwColor;
        InvalidateRect(NULL);
        return *this;
    }

private:
    HBITMAP m_hBitmap;
    BOOL m_bTracking;
    BOOL m_bButtonDown;	
    int m_nWidth;
    int m_nHeight;
    BOOL m_fEnable;
    BOOL m_fBold;
    CFont m_fontBold;
    HCURSOR m_hCursor;
    COLORREF m_colorText;

    LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if (::GetCapture() == m_hWnd)
        {
            ::ReleaseCapture();
            m_bButtonDown = FALSE;
        }

        bHandled = FALSE;
        return 0;
    }
};

//////////////////////////////////////////////////////////////////////////

#endif	SKINBUTTON_INC_
