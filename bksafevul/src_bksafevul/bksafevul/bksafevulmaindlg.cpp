#include "stdafx.h"
#include "bksafevulmodule.h"
#include "bksafevulmaindlg.h"
#include "Vulfix\BeikeVulfixEngine.h"

BOOL	g_bkMsgBox = FALSE;

//////////////////////////////////////////////////////////////////////////
/// 修改消息宏的定义到CPP，以免编译的时候太慢
BEGIN_MSG_MAP_EX_IMP(CBeikeSafeMainDlg)
MSG_BK_NOTIFY(IDC_RICHVIEW_WIN)
CHAIN_MSG_MAP(CBkDialogImpl<CBeikeSafeMainDlg>)
CHAIN_MSG_MAP(CWHRoundRectFrameHelper<CBeikeSafeMainDlg>)


CHAIN_MSG_MAP_MEMBER(m_viewVulfix.m_viewSoftVul)

MSG_WM_INITDIALOG(OnInitDialog)
MSG_WM_SYSCOMMAND(OnSysCommand)
//	MSG_WM_DESTROY(OnDestroy)
REFLECT_NOTIFICATIONS_EX()
END_MSG_MAP_IMP()
//////////////////////////////////////////////////////////////////////////

CBeikeSafeMainDlg::CBeikeSafeMainDlg()
: CBkDialogImpl<CBeikeSafeMainDlg>(IDR_BK_MAIN_DIALOG)
, m_bVulfixInited(FALSE)
, m_bVulfixRescanRequired(FALSE)
{
}

CBeikeSafeMainDlg::~CBeikeSafeMainDlg()
{

}

BOOL CBeikeSafeMainDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{
	SetIcon(::LoadIcon((HMODULE)&__ImageBase, MAKEINTRESOURCE(IDI_BEIKESAFE)));
	SetIcon(::LoadIcon((HMODULE)&__ImageBase, MAKEINTRESOURCE(IDI_SMALL)), FALSE);

	_Module.SetActiveWindow(m_hWnd);
	m_viewVulfix.Create(GetViewHWND(), NULL, WS_CHILD|WS_CLIPCHILDREN, 0, 3000);
	ATLVERIFY( m_viewVulfix.Load( IDR_BK_VULDLG_MAIN ) );
	m_viewVulfix.m_viewSoftVul.SetMainDlg( this );
	m_viewVulfix.Init(m_hWnd);

	StartScanLeak();

	return TRUE;
}

void CBeikeSafeMainDlg::OnBkBtnClose()
{
	EndDialog(IDCANCEL);
}


void CBeikeSafeMainDlg::OnBkBtnMax()
{
	if (WS_MAXIMIZE == (GetStyle() & WS_MAXIMIZE))
	{
		SendMessage(WM_SYSCOMMAND, SC_RESTORE | HTCAPTION, 0);
	}
	else
	{
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE | HTCAPTION, 0);
	}
}

void CBeikeSafeMainDlg::OnBkBtnMin()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE | HTCAPTION, 0);
}

void CBeikeSafeMainDlg::EndDialog(UINT uRetCode)
{
	__super::EndDialog(uRetCode);	
}

void CBeikeSafeMainDlg::OnSysCommand(UINT nID, CPoint point)
{
	SetMsgHandled(FALSE);

	switch (nID & 0xFFF0)
	{
	case SC_CLOSE:
		SetMsgHandled(TRUE);
		if( !g_bkMsgBox )		//已经弹出msgbox，不关闭窗口
			EndDialog(IDCANCEL);
		break;
	case SC_RESTORE:
		{
			DWORD dwStyle = GetStyle();
			//dwStyle = dwStyle & (WS_MAXIMIZE | WS_MINIMIZE);
			if (WS_MINIMIZE == (dwStyle & WS_MINIMIZE))
				break;

			if (WS_MAXIMIZE == (dwStyle & WS_MAXIMIZE))
			{
				SetItemAttribute(IDC_BTN_SYS_MAX, "skin", "maxbtn");
				break;
			}
		}
	case SC_MAXIMIZE:
		SetItemAttribute(IDC_BTN_SYS_MAX, "skin", "restorebtn");
		break;
	}
}

void CBeikeSafeMainDlg::StartScanLeak()
{
	m_viewVulfix.ShowWindow(SW_SHOW);
	if(!m_bVulfixInited || m_bVulfixRescanRequired)
	{
		BOOL toRescan = TRUE;
		if(m_bVulfixRescanRequired && theEngine && theEngine->m_isRepairing)
		{
			toRescan = FALSE;
		}			
		m_bVulfixInited = TRUE;
		m_bVulfixRescanRequired = FALSE;
		if(toRescan)
			m_viewVulfix.m_viewSoftVul.InitEnv();
	}
}