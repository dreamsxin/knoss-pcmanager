#include "stdafx.h"
#include "arpfw.h"
#include "tray.h"
#include "resource.h"

//////////////////////////////////////////////////////////////////////////

CArpTray::CArpTray()
{
    m_hIcons[0] = LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_ARP_ENABLE));
    m_hIcons[1] = LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_ARP_DISABLE));
}

CArpTray::~CArpTray()
{

}

//////////////////////////////////////////////////////////////////////////


void CArpTray::InitTrayIconStruct() 
{
    USES_CONVERSION;
    CString strArpFw;
    HICON show_icon = NULL;

    RtlZeroMemory(&m_trayIcon, sizeof(m_trayIcon));
    m_trayIcon.cbSize = NOTIFYICONDATA_V1_SIZE;
    m_trayIcon.hWnd = m_hWnd;
    m_trayIcon.uID = 1;
    m_trayIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

    if (m_bArpFwEnable) 
    {
        m_trayIcon.hIcon = m_hIcons[0];
        strArpFw = _T("金山卫士ARP防火墙 - 已开启");
    } 
    else
    {
        m_trayIcon.hIcon = m_hIcons[1];
        strArpFw = _T("金山卫士ARP防火墙 - 未开启");
    }

    m_trayIcon.uCallbackMessage = WM_ARPFW_SYSTRAY;
    StringCchCopyW(m_trayIcon.szTip, ARRAYSIZE(m_trayIcon.szTip), strArpFw);
}

LRESULT CArpTray::OnSetIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
{
    m_bArpFwEnable = (BOOL)wParam;
    CString strArpFw;

    if (m_bArpFwEnable)
    {
        m_trayIcon.hIcon = m_hIcons[0];
        strArpFw = _T("金山卫士ARP防火墙 - 已开启");
    } 
    else
    {
        m_trayIcon.hIcon = m_hIcons[1];
        strArpFw = _T("金山卫士ARP防火墙 - 未开启");
    }

    StringCchCopyW(m_trayIcon.szTip, ARRAYSIZE(m_trayIcon.szTip), strArpFw);
    Shell_NotifyIcon(NIM_MODIFY, &m_trayIcon);

    return 0L;
}

int CArpTray::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    RECT rc = {0, 0, 0, 0};

    SetWindowPos(NULL, &rc, SWP_HIDEWINDOW);
    SetWindowText(_T(""));

    InitTrayIconStruct();

    Shell_NotifyIconW(NIM_ADD, &m_trayIcon);

    return 0;
}


void CArpTray::OnDestroy() 
{
    Shell_NotifyIcon(NIM_DELETE, &m_trayIcon);
    _Module.CloseMain();
}


void CArpTray::OnClose() 
{
    DestroyWindow();
}


LRESULT CArpTray::OnSysTray(UINT uMsg, WPARAM wParam, LPARAM lParam, 
                           BOOL& bHandled) 
{
    switch (lParam) 
    {
    case WM_RBUTTONUP:
        ShowTrayMenu();
        break;

    case WM_LBUTTONUP:
        _Module.ShowMain();
        break;
    }
    return 0;
}

void CArpTray::OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl) 
{
    switch (nID) 
    {
    case ID_ARPFW_SHOWMAIN:
        OnShowMain();
        break;

    case ID_ARPFW_ENABLE:
        OnEnable(TRUE);
        break;

    case ID_ARPFW_DISABLE:
        OnEnable(FALSE);
        break;

    case ID_ARPFW_EXIT:
        OnExit();
        break;
    }
}

void CArpTray::OnEnable(BOOL bEnable)
{
    _Module.EnableArpFw(bEnable);
}

void CArpTray::OnShowMain()
{
    _Module.ShowMain();
}

void CArpTray::OnExit()
{
    CString strText;
    CString strTitle;
    int nRetCode;

    strText = _T("您确认要退出金山卫士ARP防火墙吗？");
    strTitle = _T("金山卫士ARP防火墙");

    nRetCode = MessageBox(strText, strTitle, MB_ICONQUESTION|MB_OKCANCEL);
    if (IDOK == nRetCode)
    {
        PostMessage(WM_CLOSE, 0, 0);
    }
}

void CArpTray::ShowTrayMenu() 
{
    POINT pos;
    CMenu menu;
    HMENU hMenu = NULL;

    menu.LoadMenu(IDR_TRAY_MENU);
    hMenu = menu.GetSubMenu(0);
	RemoveMenu(hMenu, m_bArpFwEnable ? ID_ARPFW_ENABLE : ID_ARPFW_DISABLE, MF_BYCOMMAND);
    GetCursorPos(&pos);
    TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, 0, m_hWnd, NULL);
}

//////////////////////////////////////////////////////////////////////////
