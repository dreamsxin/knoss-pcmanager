#pragma once

#include "wtlhelper/whwindow.h"
#include "bkres/bkres.h"

class KWarningDlg
    : public CBkDialogImpl<KWarningDlg>
{
public:
    KWarningDlg()
        : CBkDialogImpl<KWarningDlg>(IDR_WARMING_DLG)
    {
    }
	KWarningDlg(int nType, CString strText, int nOffset = 0)
		: CBkDialogImpl<KWarningDlg>(IDR_WARMING_DLG)
	{
		m_nType		=	nType;
		m_strText	=	strText;
		m_nOffset	=	nOffset;
	}
    ~KWarningDlg();
	/* @brief	nType=1 成功，nType=2 提醒 */
	BOOL			SetTextAndIcon(int nType);
	BOOL			SetText(CString str);
	BOOL			SetTextPos(int nLeft, int nTop);
	/* @brief	nType=1 成功，nType=2 提醒 */
	BOOL			SetIcon(int nType);
protected:
	void			OnBtnClose();
	//void			OnBtnQuit();
	void			OnButtonQuit();
	
    BOOL			OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/);
	LRESULT			OnCreatePage(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	void			EndDialog(UINT uRetCode);
	int				m_nType;
	CString			m_strText;
	int				m_nOffset;
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

	BEGIN_MSG_MAP_EX(KWarningDlg)
        MSG_BK_NOTIFY(IDC_RICHVIEW_WIN)
        CHAIN_MSG_MAP(CBkDialogImpl<KWarningDlg>)
		MSG_WM_INITDIALOG(OnInitDialog)
    END_MSG_MAP()

};