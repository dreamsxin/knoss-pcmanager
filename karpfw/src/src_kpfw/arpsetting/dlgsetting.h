#pragma once

//////////////////////////////////////////////////////////////////////////

#include "resource.h"
#include "skindlg.h"
#include "skinlink.h"
#include "skinbutton.h"
#include <string>
#include <vector>
#include "gwlistctrl.h"

//////////////////////////////////////////////////////////////////////////

typedef struct  tagMyGw {
    std::string strIpAddress;
    std::string strMacAddress;
} MyGw;

//////////////////////////////////////////////////////////////////////////

class CArpSettingDlg 
    : public CDialogImpl<CArpSettingDlg>
    , public CSkinDialog<CArpSettingDlg>
    , public CWinDataExchange<CArpSettingDlg>
{
public:
    enum { IDD = IDD_ARP_SETTING };

    CArpSettingDlg();
    ~CArpSettingDlg();

    BEGIN_MSG_MAP_EX(CArpSettingDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnOK)
        COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
        COMMAND_ID_HANDLER(SC_CLOSE, OnCancel)
        MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)
        MESSAGE_HANDLER(WM_CTLCOLOREDIT, OnCtlColor)
        MESSAGE_HANDLER(WM_CTLCOLORBTN, OnCtlColor)
        MESSAGE_HANDLER(WM_CTLCOLORDLG, OnCtlColor)
        MSG_WM_COMMAND(OnCommand)
        CHAIN_MSG_MAP(CSkinDialog<CArpSettingDlg>)
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    BEGIN_DDX_MAP(CArpSettingDlg)
        DDX_CONTROL(IDC_LNK_ADV_SETTING, m_lnkAdvSetting)
        DDX_CONTROL(IDC_LNK_GETGW, m_lnkGetGateway)
        DDX_CONTROL(IDOK, m_btnOK)
        DDX_CONTROL(IDCANCEL, m_btnCancel)
        DDX_CONTROL(IDC_BTN_ADD, m_btnAdd)
        DDX_CONTROL(IDC_BTN_EDIT, m_btnEdit)
        DDX_CONTROL(IDC_BTN_DEL, m_btnDel)
        DDX_CONTROL(IDC_LIST_GETWAY, m_lstGateway)
        DDX_CHECK(IDC_CHK_ENABLE_FW, m_bEnableFw)
        DDX_CHECK(IDC_CHK_USE_MYGW, m_bUseMyGw)
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

    void OnResetMyGw();
    void OnAddMyGw();
    void OnEditMyGw();
    void OnDelMyGw();
    void UpdateMyGatewayList();
    BOOL SaveAdvSetting();
    BOOL GetDefaultGetwayList();

public:
    BOOL m_bEnableFw;           // 启用ARP防火墙
    BOOL m_bUseMyGw;            // 使用自定义网关
    BOOL m_bSafeMode;           // 安全模式
    BOOL m_bBlockRecvAttack;    // 拦截外部ARP攻击
    BOOL m_bBlockRecvIPAttack;  // 拦截外部IP冲突攻击
    BOOL m_bBlockSendAttack;    // 拦截本机对外ARP攻击
    BOOL m_bNoticeGw;           // 通知网关
    int  m_nNoticeTimer;        // 通知网关的频率

    std::vector<MyGw> m_vMyGws;
    std::vector<MyGw> m_vDefaultGws;

protected:
    CBrush m_bkBrush;
    CBitmap m_bmpCloseBtn;
    CBitmap m_bmpCommonBtn;
    CBitmap m_bmpSkin;
    CSkinLink m_lnkAdvSetting;
    CSkinLink m_lnkGetGateway;
    CSkinButton m_btnOK;
    CSkinButton m_btnCancel;
    CSkinButton m_btnAdd;
    CSkinButton m_btnEdit;
    CSkinButton m_btnDel;
    CGwListCtrl m_lstGateway;
    CString m_strStatus;
};

//////////////////////////////////////////////////////////////////////////
