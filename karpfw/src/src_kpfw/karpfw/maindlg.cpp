#include "stdafx.h"
#include "arpfw.h"
#include "maindlg.h"

//////////////////////////////////////////////////////////////////////////

CMainDlg::CMainDlg()
{
    m_hExitSignal = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hWorkThread = NULL;

    m_bmpCloseBtn.LoadBitmap(IDB_CLOSE_BTN);
    m_bmpEnableBtn.LoadBitmap(IDB_ENABLE_BTN);
	m_bmpDisableBtn.LoadBitmap(IDB_DISABLE_BTN);
    m_bmpSkin.LoadBitmap(IDB_SKIN);
    m_bkBrush.CreateSolidBrush(RGB(0xff,0xff,0xff));
    m_bmpYes.LoadBitmap(IDB_YES);
    m_bmpNo.LoadBitmap(IDB_NO);

    m_piArpSetting = CArpSetting::Instance().GetPtr();

    m_bFwEnable = TRUE;
}

CMainDlg::~CMainDlg()
{
    ::CloseHandle(m_hExitSignal);
}

//////////////////////////////////////////////////////////////////////////

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg) 
{
    return TRUE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
        IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
    SetIcon(hIcon, TRUE);
    HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
        IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
    SetIcon(hIconSmall, FALSE);

    // 创建大字体
    CFontHandle hFont = GetFont();
    LOGFONT logFont = { 0 };
    hFont.GetLogFont(&logFont);
    logFont.lfHeight = -14;
    logFont.lfWeight = FW_BOLD;
    m_fontBig.CreateFontIndirect(&logFont);
    DoDataExchange();

    CString strTitle = _T("金山卫士ARP防火墙");
    SetBkSkin(m_bmpSkin);
    InitSkinDialog(m_bmpCloseBtn, NULL, strTitle);
    SetWindowPos(NULL, 0, 0, 496, 184, SWP_NOMOVE);
    CenterWindow();

    m_btnEnable.InitButton(m_bmpEnableBtn);
    m_btnEnable.SetWindowText(_T(""));
	m_btnEnable.SetWindowPos(NULL, 412, 53, 0, 0, SWP_NOSIZE);
    m_btnDisable.InitButton(m_bmpDisableBtn);
    m_btnDisable.SetWindowText(_T(""));
	m_btnDisable.SetWindowPos(NULL, 412, 53, 0, 0, SWP_NOSIZE);
	m_lnkSetting.SetWindowPos(NULL, 424, 100, 0, 0, SWP_NOSIZE);
	//GetDlgItem(IDC_LOG_GROUP).SetWindowPos(NULL, 11, 120, 476, 104, 0);
	GetDlgItem(IDC_TXT_LOG).SetWindowPos(NULL, 20, 136, 0, 0, SWP_NOSIZE);
	m_lnkVersion.SetWindowPos(NULL, 20, 160, 0, 0, SWP_NOSIZE);
	m_lnkBBS.SetWindowPos(NULL, 120, 160, 0, 0, SWP_NOSIZE);
	m_lnkViewLog.SetWindowPos(NULL, 460, 136, 0, 0, SWP_NOSIZE);
	GetDlgItem(IDC_LINE).SetWindowPos(NULL, 20, 127, 466, 1, 0);

    CString strVersion = _T("版本:"); 
    strVersion += _Module.GetProductVersion();
    m_lnkVersion.SetWindowText(strVersion);

    if (m_piArpSetting)
    {
        m_piArpSetting->IsArpFwEnable(&m_bFwEnable);
    }
    UpdateUI();
    StartWork();

    return TRUE;
}

void CMainDlg::OnDestroy() 
{
    StopWork();
}

void CMainDlg::OnClose() 
{
    DestroyWindow();
    PostQuitMessage(0);
}

void CMainDlg::OnPaint(CPaintDC& dc)
{
    CDC memdc;

    memdc.CreateCompatibleDC(dc);
    ::SelectObject(memdc, m_bFwEnable ? m_bmpYes : m_bmpNo);

    dc.BitBlt(30, 50, 48, 48, memdc, 0, 0, SRCCOPY);
    ::SelectObject(dc, m_fontBig);
    dc.SetBkMode(TRANSPARENT);
    CString strState = m_bFwEnable ? _T("已经开启防护") : _T("尚未开启防护");
    COLORREF dwColor = m_bFwEnable ? RGB(0x00, 0xcc, 0x00) : RGB(0xcc, 0x00, 0x00);
    dc.SetTextColor(dwColor);
    dc.TextOut(120, 54, strState);

	::SelectObject(dc, GetFont());
	dc.SetTextColor(RGB(0x00,0x00,0x00));
	dc.TextOut(120, 82, _T("ARP防火墙可以保护您免受局域网中ARP欺骗的攻击。"));

    CString strLastLog = m_strLastLog.IsEmpty() ? _T("暂无记录") : m_strLastLog;
    RECT rcLog = { 80, 136, 80 + 350, 136 + 38 };
    ::SelectObject(dc, GetFont());
	if (m_strLastLog.IsEmpty())
		dc.SetTextColor(RGB(0x00,0x00,0x00));
	else
		dc.SetTextColor(RGB(0xee,0x00,0x00));
    dc.DrawText(strLastLog, strLastLog.GetLength(), &rcLog, DT_LEFT|DT_TOP|DT_SINGLELINE|DT_END_ELLIPSIS);
}

void CMainDlg::UpdateLogUI()
{
	RECT rcLog = { 80, 136, 80 + 350, 136 + 38 };
	InvalidateRect(&rcLog);
}

HRESULT CMainDlg::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CDCHandle dc = GetDC();
    dc.SelectBrush(m_bkBrush);
    return (HRESULT)(ULONG_PTR)(HBRUSH)m_bkBrush;
}

HRESULT CMainDlg::OnArpFwState(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (m_bFwEnable != (BOOL)wParam)
    {
        m_bFwEnable = (BOOL)wParam;
        UpdateUI();
    }

    return TRUE;
}

HRESULT CMainDlg::OnArpFwEnable(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    EnableArpFw((BOOL)wParam);
    return TRUE;
}

HRESULT CMainDlg::OnArpFwLastLog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CString strLog;

    KAV_PROCESS_ERROR(wParam);

    strLog = (const wchar_t*)wParam;

    strLog.Replace(_T("\t"), _T(","));
    strLog.Replace(_T("\r\n"), _T(""));

    if (m_strLastLog != strLog)
    {
        m_strLastLog = strLog;
        UpdateLogUI();
    }

Exit0:
    return TRUE;
}

void CMainDlg::OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    if (SC_CLOSE == nID)
    {
        ShowWindow(SW_HIDE);
    }

    if (IDC_BTN_ENABLE == nID)
    {
        EnableArpFw(TRUE);
    }

    if (IDC_BTN_DISABLE == nID)
    {
        EnableArpFw(FALSE);
    }

    if (IDC_LNK_SETTING == nID)
    {
        ShowSetting();
    }

    if (IDC_LNK_BBS == nID)
    {
        GotoBBS();
    }

    if (IDC_LNK_VIEWLOG == nID)
    {
        ViewLog();
    }
}

//////////////////////////////////////////////////////////////////////////

void CMainDlg::GotoBBS()
{
    ::ShellExecuteA(NULL, "open", "http://bbs.ijinshan.com/forum-18-1.html", NULL, NULL, SW_SHOWNORMAL);
}

void CMainDlg::ViewLog()
{
    CString strLogPath = _Module.GetAppDirPath();
    if (strLogPath[strLogPath.GetLength() - 1] != _T('\\'))
        strLogPath += _T("\\");
    strLogPath += _T("AppData\\antiarp.log");
    ::ShellExecute(NULL, _T("open"), strLogPath, NULL, NULL, SW_SHOWNORMAL);
}

void CMainDlg::UpdateUI()
{
    if (m_bFwEnable)
    {
        m_btnEnable.ShowWindow(SW_HIDE);
        m_btnDisable.ShowWindow(SW_SHOWNORMAL);
    }
    else
    {
        m_btnEnable.ShowWindow(SW_SHOWNORMAL);
        m_btnDisable.ShowWindow(SW_HIDE);
    }

    _Module.SetTrayIcon(m_bFwEnable);
    InvalidateRect(NULL);
}

void CMainDlg::ShowSetting()
{
    BOOL retval = FALSE;
    HRESULT hRetCode;

    KAV_PROCESS_ERROR(m_piArpSetting);

    hRetCode = m_piArpSetting->ShowSetting(m_hWnd);
    //KAV_COM_PROCESS_ERROR(hRetCode);

    retval = TRUE;

Exit0:
    if (!retval)
    {
        CString strTitle = _T("金山卫士ARP防火墙");
        CString strText = _T("调用设置模块失败！");
        MessageBox(strText, strTitle, MB_ICONWARNING);
    }
    return;
}

void CMainDlg::EnableArpFw(BOOL bEnable)
{
    BOOL retval = FALSE;
    HRESULT hRetCode;

    KAV_PROCESS_ERROR(m_piArpSetting);

    hRetCode = m_piArpSetting->EnableArpFw(bEnable);
    KAV_COM_PROCESS_ERROR(hRetCode);

    m_bFwEnable = bEnable;
    UpdateUI();

    retval = TRUE;

Exit0:
    if (!retval)
    {
        CString strTitle = _T("金山卫士ARP防火墙");
        CString strText = _T("启用或关闭ARP防火墙失败！");
        MessageBox(strText, strTitle, MB_ICONWARNING);
    }
    return;
}

//////////////////////////////////////////////////////////////////////////

UINT CMainDlg::WorkThreadProc(LPVOID pSetting)
{
    CMainDlg* pThis = (CMainDlg*)pSetting;
    IArpFwSetting* piArpSetting = NULL;
    HANDLE hExitSignal = NULL;
    HRESULT hRetCode;
    wchar_t szLastLog[512] = { 0 };

    KAV_PROCESS_ERROR(pThis);

    piArpSetting = pThis->m_piArpSetting;
    KAV_PROCESS_ERROR(piArpSetting);

    hExitSignal = pThis->m_hExitSignal;
    KAV_PROCESS_ERROR(hExitSignal);

    for (;;)
    {
        BOOL bEnable = FALSE;

        if (::WaitForSingleObject(hExitSignal, 1000) == WAIT_OBJECT_0)
            break;

        hRetCode = piArpSetting->IsArpFwEnable(&bEnable);
        if (FAILED(hRetCode))
            continue;

        pThis->PostMessage(WM_ARPFW_STATE, bEnable, 0);

        hRetCode = piArpSetting->GetLastLog(512, szLastLog);
        if (FAILED(hRetCode))
            continue;

        pThis->SendMessage(WM_ARPFW_LASTLOG, (WPARAM)szLastLog);
    }

Exit0:
    _endthreadex(0);
    return 0;
}

BOOL CMainDlg::StartWork()
{
    BOOL retval = FALSE;

    if (m_hWorkThread)
    {
        retval = TRUE;
        goto Exit0;
    }

    m_hWorkThread = (HANDLE)::_beginthreadex(
        NULL, 0, WorkThreadProc, this, 0, NULL);
    KAV_PROCESS_ERROR(m_hWorkThread);

    retval = TRUE;

Exit0:
    return retval;
}

void CMainDlg::StopWork()
{
    if (m_hWorkThread)
    {
        ::SetEvent(m_hExitSignal);
        ::WaitForSingleObject(m_hExitSignal, 1000);
        ::CloseHandle(m_hWorkThread);
        m_hWorkThread = NULL;
    }
}

//////////////////////////////////////////////////////////////////////////
