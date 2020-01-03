#pragma once

//////////////////////////////////////////////////////////////////////////

#include "resource.h"
#include "skingdi.h"
#include "skinlink.h"
#include "skinbutton.h"
#include "skindlg.h"
#include "arpsetting.h"

//////////////////////////////////////////////////////////////////////////

#define WM_ARPFW_STATE          (WM_USER+100)
#define WM_ARPFW_ENABLE         (WM_USER+101)
#define WM_ARPFW_LASTLOG        (WM_USER+102)

//////////////////////////////////////////////////////////////////////////

class CMainDlg 
    : public CDialogImpl<CMainDlg>
    , public CWinDataExchange<CMainDlg>
    , public CSkinDialog<CMainDlg>
    , public CMessageFilter
{
public:
    enum { IDD = IDD_MAIN_DLG };

    CMainDlg();
    ~CMainDlg();

    BEGIN_MSG_MAP_EX(CMainDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_COMMAND(OnCommand)
        MSG_WM_CLOSE(OnClose)
        MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColor)
        MESSAGE_HANDLER(WM_CTLCOLOREDIT, OnCtlColor)
        MESSAGE_HANDLER(WM_CTLCOLORBTN, OnCtlColor)
        MESSAGE_HANDLER(WM_CTLCOLORDLG, OnCtlColor)
        MESSAGE_HANDLER(WM_ARPFW_STATE, OnArpFwState)
        MESSAGE_HANDLER(WM_ARPFW_ENABLE, OnArpFwEnable)
        MESSAGE_HANDLER(WM_ARPFW_LASTLOG, OnArpFwLastLog)
        REFLECT_NOTIFICATIONS()
        CHAIN_MSG_MAP(CSkinDialog<CMainDlg>)
    END_MSG_MAP()

    BEGIN_DDX_MAP(CMainDlg)
        DDX_CONTROL(IDC_LNK_BBS, m_lnkBBS)
        DDX_CONTROL_HANDLE(IDC_LNK_VERSION, m_lnkVersion)
        DDX_CONTROL(IDC_BTN_ENABLE, m_btnEnable)
        DDX_CONTROL(IDC_BTN_DISABLE, m_btnDisable)
        DDX_CONTROL(IDC_LNK_VIEWLOG, m_lnkViewLog)
        DDX_CONTROL(IDC_LNK_SETTING, m_lnkSetting)
    END_DDX_MAP()

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl);
    BOOL PreTranslateMessage(MSG* pMsg);
    void OnDestroy();
    void OnClose();
    void OnPaint(CPaintDC& dc);
    HRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    HRESULT OnArpFwState(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    HRESULT OnArpFwEnable(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    HRESULT OnArpFwLastLog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    void InitUserInterface();
    void UpdateUI();
    void EnableArpFw(BOOL bEnable);
    void ShowSetting();
    void GotoBBS();
    void ViewLog();
	void UpdateLogUI();

    static UINT WINAPI WorkThreadProc(LPVOID pSetting);
    BOOL StartWork();
    void StopWork();

    CSkinLink m_lnkBBS;
    CSkinLink m_lnkSetting;
    CSkinLink m_lnkViewLog;
    CStatic m_lnkVersion;
    CSkinButton m_btnEnable;
    CSkinButton m_btnDisable;

    CBrush m_bkBrush;
    CBitmap m_bmpCloseBtn;
    CBitmap m_bmpDisableBtn;
	CBitmap m_bmpEnableBtn;
    CBitmap m_bmpSkin;
    CBitmap m_bmpYes;
    CBitmap m_bmpNo;
    CFont m_fontBig;

    BOOL m_bFwEnable;
    IArpFwSetting* m_piArpSetting;
    HANDLE m_hExitSignal;
    HANDLE m_hWorkThread;
    CString m_strLastLog;
};

//////////////////////////////////////////////////////////////////////////
