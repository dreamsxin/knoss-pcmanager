#pragma once

//////////////////////////////////////////////////////////////////////////

#include "resource.h"
#include "skindlg.h"
#include "skinlink.h"
#include "skinbutton.h"

//////////////////////////////////////////////////////////////////////////

class CArpAdvSettingDlg 
    : public CDialogImpl<CArpAdvSettingDlg>
    , public CSkinDialog<CArpAdvSettingDlg>
    , public CWinDataExchange<CArpAdvSettingDlg>
{
public:
    enum { IDD = IDD_ADV_SETTING };

    CArpAdvSettingDlg();
    ~CArpAdvSettingDlg();

    BEGIN_MSG_MAP_EX(CArpAdvSettingDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnOK)
        COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
        COMMAND_ID_HANDLER(SC_CLOSE, OnCancel)
        MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)
        MESSAGE_HANDLER(WM_CTLCOLOREDIT, OnCtlColor)
        MESSAGE_HANDLER(WM_CTLCOLORBTN, OnCtlColor)
        MESSAGE_HANDLER(WM_CTLCOLORDLG, OnCtlColor)
        MSG_WM_COMMAND(OnCommand)
        REFLECT_NOTIFICATIONS()
        CHAIN_MSG_MAP(CSkinDialog<CArpAdvSettingDlg>)
    END_MSG_MAP()

    BEGIN_DDX_MAP(CArpAdvSettingDlg)
        DDX_CONTROL(IDOK, m_btnOK)
        DDX_CONTROL(IDCANCEL, m_btnCancel)
        DDX_CHECK(IDC_CHK_NOTICE_GW, m_bNoticeGw)
        //DDX_CHECK(IDC_CHK_BLOCK_INATTACK, m_bBlockRecvAttack)
        DDX_CHECK(IDC_CHK_BLOCK_IPCONFLICT, m_bBlockRecvIPAttack)
        DDX_CHECK(IDC_CHK_BLOCK_OUTATTACK, m_bBlockSendAttack)
		DDX_CHECK(IDC_CHK_SAFEMODE, m_bSafeMode)
        DDX_UINT_RANGE(IDC_EDT_NOTICE_TIMER, m_nNoticeTimer, 1, 50)
    END_DDX_MAP()

    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    void OnPaint(CPaintDC& dc);
    HRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl);
    
public:
    BOOL m_bSafeMode;           // 安全模式
    BOOL m_bBlockRecvAttack;    // 拦截外部ARP攻击
    BOOL m_bBlockRecvIPAttack;  // 拦截外部IP冲突攻击
    BOOL m_bBlockSendAttack;    // 拦截本机对外ARP攻击
    BOOL m_bNoticeGw;           // 通知网关
    int  m_nNoticeTimer;        // 通知网关的频率

protected:
    CBrush m_bkBrush;
    CBitmap m_bmpCloseBtn;
    CBitmap m_bmpCommonBtn;
    CBitmap m_bmpSkin;
    CSkinButton m_btnOK;
    CSkinButton m_btnCancel;
};

//////////////////////////////////////////////////////////////////////////
