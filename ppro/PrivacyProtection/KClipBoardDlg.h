#pragma once

#include "wtlhelper/whwindow.h"
#include "bkres/bkres.h"

class KClipBoardDlg
    : public CBkDialogImpl<KClipBoardDlg>
{
public:
    KClipBoardDlg()
        : CBkDialogImpl<KClipBoardDlg>(IDR_CLIPBOARD_DLG)
    {
    }

    ~KClipBoardDlg();

protected:
	void			OnBtnClose();
	//void			OnBtnQuit();
	void			OnButtonQuit();
	
    BOOL			OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/);
	LRESULT			OnCreatePage(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	void			EndDialog(UINT uRetCode);

	BOOL			CopyToClipboard_(const WCHAR* pszData, const int nDataLen);
	BOOL			CopyToClipboard();


public:

    BK_NOTIFY_MAP(IDC_RICHVIEW_WIN)
		BK_NOTIFY_ID_COMMAND(100, OnBtnClose)
		BK_NOTIFY_ID_COMMAND(101, OnButtonQuit)
		//BK_NOTIFY_ID_COMMAND(102, OnBtnQuit)
		/*BK_NOTIFY_ID_COMMAND(101, OnBtnMin)
        BK_NOTIFY_ID_COMMAND(102, OnBtnClose)
		BK_NOTIFY_ID_COMMAND(103, OnBtnHome)
		BK_NOTIFY_ID_COMMAND(206, OnLog)
        BK_NOTIFY_ID_COMMAND(207, OnSetting)
		BK_NOTIFY_ID_COMMAND(208, OnUpdate)
		BK_NOTIFY_ID_COMMAND(209, OnCharge)
		BK_NOTIFY_ID_COMMAND(210, OnApplyPassport)
		BK_NOTIFY_ID_COMMAND(211, OnChangePassport)
		BK_NOTIFY_ID_COMMAND(212, OnPassportSet)
        BK_NOTIFY_TAB_SELCHANGE(136, OnBkTabMainSelChange)*/
		//BK_NOTIFY_ID_COMMAND(156, OnLinkPassport)
    BK_NOTIFY_MAP_END()

	BEGIN_MSG_MAP_EX(KClipBoardDlg)
        MSG_BK_NOTIFY(IDC_RICHVIEW_WIN)
        CHAIN_MSG_MAP(CBkDialogImpl<KClipBoardDlg>)
		MSG_WM_INITDIALOG(OnInitDialog)
    END_MSG_MAP()

};