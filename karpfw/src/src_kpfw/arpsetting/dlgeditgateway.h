#pragma once

//////////////////////////////////////////////////////////////////////////

#include "resource.h"
#include "skindlg.h"
#include "skinlink.h"
#include "skinbutton.h"

//////////////////////////////////////////////////////////////////////////

BOOL mac_to_str(const BYTE* pMac, std::string& strMac);
BOOL str_to_mac(const std::string& strMac, BYTE* pMac);

//////////////////////////////////////////////////////////////////////////

class CEditGatewayDlg 
    : public CDialogImpl<CEditGatewayDlg>
    , public CSkinDialog<CEditGatewayDlg>
    , public CWinDataExchange<CEditGatewayDlg>
{
public:
    enum { IDD = IDD_EDIT_GATEWAY };

    CEditGatewayDlg();
    ~CEditGatewayDlg();

    BEGIN_MSG_MAP_EX(CEditGatewayDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnOK)
        COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
        COMMAND_ID_HANDLER(SC_CLOSE, OnCancel)
        MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)
        MESSAGE_HANDLER(WM_CTLCOLOREDIT, OnCtlColor)
        MESSAGE_HANDLER(WM_CTLCOLORBTN, OnCtlColor)
        MESSAGE_HANDLER(WM_CTLCOLORDLG, OnCtlColor)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_COMMAND(OnCommand)
        REFLECT_NOTIFICATIONS()
        CHAIN_MSG_MAP(CSkinDialog<CEditGatewayDlg>)
    END_MSG_MAP()

    BEGIN_DDX_MAP(CEditGatewayDlg)
        DDX_CONTROL(IDOK, m_btnOK)
        DDX_CONTROL(IDCANCEL, m_btnCancel)
        DDX_CONTROL_HANDLE(IDC_IP_ADDRESS, m_ctlIpAddress)
        DDX_CONTROL_HANDLE(IDC_MAC_ADDRESS, m_ctrlMacAddress)
        DDX_CONTROL(IDC_LNK_GETMAC, m_lnkGetMac)
    END_DDX_MAP()

    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    void OnDestroy();

    void OnPaint(CPaintDC& dc);
    HRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnGetMac();
    static unsigned __stdcall ThreadFun(void* params);

public:
    CString m_strIp;
    CString m_strMac;

protected:
    CBrush m_bkBrush;
    CBitmap m_bmpCloseBtn;
    CBitmap m_bmpCommonBtn;
    CBitmap m_bmpSkin;
    CSkinButton m_btnOK;
    CSkinButton m_btnCancel;
    CSkinLink m_lnkGetMac;

    CIPAddressCtrl m_ctlIpAddress;
    CEdit m_ctrlMacAddress;
    BOOL m_bGetMacing;
    HANDLE m_hGetMacThread;
};

//////////////////////////////////////////////////////////////////////////
