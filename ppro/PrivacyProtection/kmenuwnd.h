#ifndef _K_MENU_WND_H_
#define _K_MENU_WND_H_
#include <Psapi.h>


class KMenuWnd
	: public CBkDialogImpl<KMenuWnd>
{
public:
	KMenuWnd()
		: CBkDialogImpl<KMenuWnd>(IDR_MENU_WND)
	{
	}

	~KMenuWnd(void)
	{
	}

	BOOL PopUp(CPoint& point)
	{
		SetWindowPos(HWND_TOP, point.x, point.y, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
		ShowWindow(SW_SHOWNORMAL);

		return TRUE;
	}

public:
	void OnBtnAbout()
	{
		Hide();
		//EmptyWorkingSet(GetCurrentProcess());
	}

	BOOL OnInitDialog(HWND wParam, LPARAM lParam)
	{
		return TRUE;
	}

	LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		Hide();
		return TRUE;
	}

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		Hide();
		return TRUE;
	}


	void Hide()
	{
		ShowWindow(SW_HIDE);
	}


public:
	BK_NOTIFY_MAP(IDC_RICHVIEW_WIN)
		BK_NOTIFY_ID_COMMAND(100, OnBtnAbout)
	BK_NOTIFY_MAP_END()

	BEGIN_MSG_MAP_EX(KMenuWnd)
		MSG_BK_NOTIFY(IDC_RICHVIEW_WIN)
		CHAIN_MSG_MAP(CBkDialogImpl<KMenuWnd>)
		MSG_WM_INITDIALOG(OnInitDialog)
		MESSAGE_HANDLER( WM_KILLFOCUS,		OnKillFocus)
		MESSAGE_HANDLER( WM_LBUTTONDOWN,	OnLButtonDown )
	END_MSG_MAP()
};

#endif	// _K_MENU_WND_H_
