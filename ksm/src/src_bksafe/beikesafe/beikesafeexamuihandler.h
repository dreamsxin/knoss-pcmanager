#pragma once


class CBeikeSafeMainDlg;


class CBeikeSafeExamUIHandler
	: public CBkNavigator
	, public CBkSafeUIHandlerBase
{
public:


public:
	CBeikeSafeExamUIHandler(CBeikeSafeMainDlg *pDialog)
		: m_pDlg(pDialog)
	{
	}

	~CBeikeSafeExamUIHandler();

	STATE_CLOSE_CHECK	CloseCheck(){ return CLOSE_CHECK_CONTINUE;}
public:
	BK_NOTIFY_MAP(IDC_RICHVIEW_WIN)
		BK_NOTIFY_MAP_END()

		BEGIN_MSG_MAP_EX(CBeikeSafeExamUIHandler)
		MSG_BK_NOTIFY(IDC_RICHVIEW_WIN)
		END_MSG_MAP()

private:
 	CBeikeSafeMainDlg*						 m_pDlg;				//主程序窗口指针

};


