#pragma once

//////////////////////////////////////////////////////////////////////////

#define WM_ARPFW_SYSTRAY      (WM_USER+100)
#define WM_ARPFW_SETICON      (WM_USER+101)

//////////////////////////////////////////////////////////////////////////

class CArpTray : public CWindowImpl<CArpTray, CWindow, CFrameWinTraits> 
{
public:
    CArpTray();
    ~CArpTray();

    DECLARE_WND_CLASS(_T("KSafeArpTray"))

    BEGIN_MSG_MAP(CArpTray)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_CLOSE(OnClose)
        MSG_WM_COMMAND(OnCommand)
        MESSAGE_HANDLER(WM_ARPFW_SYSTRAY, OnSysTray)
        MESSAGE_HANDLER(WM_ARPFW_SETICON, OnSetIcon)
    END_MSG_MAP()

    LRESULT OnSysTray(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSetIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnDestroy();
    void OnClose();
    void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnShowMain();
    void OnExit();
    void OnEnable(BOOL bEnable);

    void InitTrayIconStruct();
    void ShowTrayMenu();

    NOTIFYICONDATAW m_trayIcon;
    BOOL m_bArpFwEnable;
    HICON m_hIcons[2];
};



//////////////////////////////////////////////////////////////////////////
