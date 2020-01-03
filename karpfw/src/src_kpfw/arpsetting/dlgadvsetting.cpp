#include "stdafx.h"
#include "arpsetting.h"
#include "dlgadvsetting.h"

//////////////////////////////////////////////////////////////////////////

CArpAdvSettingDlg::CArpAdvSettingDlg()
{
    m_bBlockRecvAttack = TRUE;
    m_bkBrush.CreateSolidBrush(RGB(0xff, 0xff, 0xff));
    m_bmpCloseBtn.LoadBitmap(IDB_CLOSE_BTN);
    m_bmpCommonBtn.LoadBitmap(IDB_COMMON_BTN);
    m_bmpSkin.LoadBitmap(IDB_SKIN);
}

CArpAdvSettingDlg::~CArpAdvSettingDlg()
{

}

//////////////////////////////////////////////////////////////////////////

LRESULT CArpAdvSettingDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    // center the dialog on the screen
    // set icons
    HICON hIcon = (HICON)::LoadImage(_Module.GetInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
        IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
    SetIcon(hIcon, TRUE);
    HICON hIconSmall = (HICON)::LoadImage(_Module.GetInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
        IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
    SetIcon(hIconSmall, FALSE);

    DoDataExchange();

    CString strTitle;
    GetWindowText(strTitle);
    SetBkSkin(m_bmpSkin);
    InitSkinDialog(m_bmpCloseBtn, NULL, strTitle);
    SetWindowPos(NULL, 0, 0, 382, 370, SWP_NOMOVE);
    CenterWindow();

    m_btnOK.InitButton(m_bmpCommonBtn, 0, 0);
    m_btnCancel.InitButton(m_bmpCommonBtn, 0, 0);

    if (m_bNoticeGw)
    {
        GetDlgItem(IDC_EDT_NOTICE_TIMER).EnableWindow(TRUE);
    }
    else
    {
        GetDlgItem(IDC_EDT_NOTICE_TIMER).EnableWindow(FALSE);
    }

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////

void CArpAdvSettingDlg::OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    if (nID == IDC_CHK_NOTICE_GW)
    {
        CButton btn = GetDlgItem(nID);
        if (btn.GetCheck())
        {
            GetDlgItem(IDC_EDT_NOTICE_TIMER).EnableWindow(TRUE);
        }
        else
        {
            GetDlgItem(IDC_EDT_NOTICE_TIMER).EnableWindow(FALSE);
        }
    }
}

LRESULT CArpAdvSettingDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if (DoDataExchange(TRUE))
    {
        EndDialog(wID);
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////

LRESULT CArpAdvSettingDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    EndDialog(wID);
    return 0;
}

//////////////////////////////////////////////////////////////////////////

void CArpAdvSettingDlg::OnPaint(CPaintDC& dc)
{

}

HRESULT CArpAdvSettingDlg::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CDCHandle dc = GetDC();
    dc.SelectBrush(m_bkBrush);
    return (HRESULT)(ULONG_PTR)(HBRUSH)m_bkBrush;
}

//////////////////////////////////////////////////////////////////////////
