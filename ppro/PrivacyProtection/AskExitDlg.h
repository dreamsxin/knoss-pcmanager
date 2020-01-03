#pragma once

#include "wtlhelper/whwindow.h"
#include "bkres/bkres.h"

class KAskExitDlg
	: public CBkDialogImpl<KAskExitDlg>
{
public:
	KAskExitDlg()
		: CBkDialogImpl<KAskExitDlg>(IDR_ASK_EXIT_DLG)
	{
	}
	KAskExitDlg(int nCloseChoice)
		: CBkDialogImpl<KAskExitDlg>(IDR_ASK_EXIT_DLG)
	{
		m_nCloseChoice = nCloseChoice;
	}
private:
	int m_nCloseChoice;
protected:
	void OnButtonQuit()
	{
		__super::EndDialog(IDABORT);
	}
	void Close()
	{
		__super::EndDialog(IDCLOSE);
	}
	void OnButtonYes()
	{
		__super::EndDialog(IDYES);
	}

	void OnButtonNo()
	{
		__super::EndDialog(IDNO);
	}
	BOOL OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
	{
		SetRadioCheck(m_nCloseChoice);
		return TRUE;
	}
public:
	BOOL GetChecked()
	{
		return GetItemCheck(100);
	}

	void SetCheck(BOOL bChecked)
	{
		SetItemCheck(100, bChecked);
	}

	void SetRadioCheck(int nIndex)
	{
		if (nIndex == 1)
			SetItemCheck(1001, TRUE);
		else if (nIndex == 2)
			SetItemCheck(1002, TRUE);
	}

	int GetRadioCheck()
	{
		if (GetItemCheck(1001))
			return 1;
		else if (GetItemCheck(1002))
			return 2;
		else
			return -1;
	}
public:

	BK_NOTIFY_MAP(IDC_RICHVIEW_WIN)
		BK_NOTIFY_ID_COMMAND(60001, OnButtonQuit)
		BK_NOTIFY_ID_COMMAND(101, OnButtonYes)
		BK_NOTIFY_ID_COMMAND(102, OnButtonNo)
	BK_NOTIFY_MAP_END()
	BEGIN_MSG_MAP_EX(KAskExitDlg)
		MSG_BK_NOTIFY(IDC_RICHVIEW_WIN)
		CHAIN_MSG_MAP(CBkDialogImpl<KAskExitDlg>)
		MSG_WM_INITDIALOG(OnInitDialog)
	END_MSG_MAP()
};