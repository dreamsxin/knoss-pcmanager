#pragma once

#include "wtlhelper/whwindow.h"
#include "bkres/bkres.h"

class KMinAskDlg
	: public CBkDialogImpl<KMinAskDlg>
{
public:
	KMinAskDlg()
		: CBkDialogImpl<KMinAskDlg>(IDR_MIN_ASK_DLG)
	{
	}


protected:
	void OnButtonQuit()
	{
		__super::EndDialog(IDABORT);
	}

	void OnButtonYes()
	{
		__super::EndDialog(IDYES);
	}

	void OnButtonNo()
	{
		__super::EndDialog(IDNO);
	}

public:

	BK_NOTIFY_MAP(IDC_RICHVIEW_WIN)
		BK_NOTIFY_ID_COMMAND(60001, OnButtonQuit)
		BK_NOTIFY_ID_COMMAND(101, OnButtonYes)
		BK_NOTIFY_ID_COMMAND(102, OnButtonNo)
	BK_NOTIFY_MAP_END()
	BEGIN_MSG_MAP_EX(KMinAskDlg)
		MSG_BK_NOTIFY(IDC_RICHVIEW_WIN)
		CHAIN_MSG_MAP(CBkDialogImpl<KMinAskDlg>)
	END_MSG_MAP()
};