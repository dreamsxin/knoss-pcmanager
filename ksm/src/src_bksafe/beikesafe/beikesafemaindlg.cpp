#include "stdafx.h"
#include "beikesafe.h"
#include "beikesafemaindlg.h"

#include "Vulfix\BeikeVulfixEngine.h"

#include "beikesafeexamuihandler.h"
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")

extern CVulEngine *theEngine;
BOOL	g_bkMsgBox = FALSE;

#define TIMER_ID_MAIN_DLG_ONCE 0x10000

#define MSG_BEGIN						WM_APP+0x100
#define MSG_APP_DELAY_EXAM				MSG_BEGIN+1
#define MSG_APP_DELAY_NAVIGATE_IE		MSG_BEGIN+2

//////////////////////////////////////////////////////////////////////////
/// 2010-10-9 修改消息宏的定义到CPP，以免编译的时候太慢
BEGIN_MSG_MAP_EX_IMP(CBeikeSafeMainDlg)
	MSG_BK_NOTIFY(IDC_RICHVIEW_WIN)
	CHAIN_MSG_MAP(CBkDialogImpl<CBeikeSafeMainDlg>)
	CHAIN_MSG_MAP(CWHRoundRectFrameHelper<CBeikeSafeMainDlg>)

	MSG_WM_TIMER(OnTimer)
	CHAIN_MSG_MAP_MEMBER((*m_ExamHandler))
	CHAIN_MSG_MAP_MEMBER(m_viewVulfix.m_viewSoftVul)

	MSG_WM_INITDIALOG(OnInitDialog)
	MSG_WM_SYSCOMMAND(OnSysCommand)
	MSG_WM_DESTROY(OnDestroy)
	MESSAGE_HANDLER_EX(MSG_APP_DELAY_NAVIGATE_IE, OnAppDelayNavigateIE)

	REFLECT_NOTIFICATIONS_EX()
END_MSG_MAP_IMP()
//////////////////////////////////////////////////////////////////////////

CBeikeSafeMainDlg::CBeikeSafeMainDlg()
	: CBkDialogImpl<CBeikeSafeMainDlg>(IDR_BK_MAIN_DIALOG)
	, m_bPage0NeverShowed(TRUE)
	, m_bPage1NeverShowed(TRUE)
	, m_bPage5NeverShowed(TRUE)
	, m_bVulfixInited(FALSE)
	, m_bVulfixRescanRequired(FALSE)
	, m_bFirstPageChange(TRUE)
	, m_bIeFixInit(FALSE)
	, m_bKClearInit(FALSE)
{
	m_ExamHandler			= new CBeikeSafeExamUIHandler(this);

	AddUIHandler(&m_viewVulfix.m_viewSoftVul);
 	AddUIHandler(m_ExamHandler);
}

CBeikeSafeMainDlg::~CBeikeSafeMainDlg()
{
	delete m_ExamHandler;
}

LRESULT CBeikeSafeMainDlg::OnTimer( UINT_PTR uIdEvent )
{

	SetMsgHandled( FALSE );
	return TRUE;
}

void CBeikeSafeMainDlg::SetDefaultNavigate(LPCWSTR lpszNavigate)
{
	//m_strDefaultNavigate = lpszNavigate;
}

LRESULT CBeikeSafeMainDlg::OnAppDefaultNavigate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
// 	_Module.Navigate(m_strDefaultNavigate);
// 
// 	m_strDefaultNavigate.Empty();

	return 0;
}



LRESULT CBeikeSafeMainDlg::OnAppEchoFinish(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	SetMsgHandled(FALSE);


	return 0;
}

void CBeikeSafeMainDlg::InitVulFix()
{
	m_viewVulfix.Create(GetViewHWND(), NULL, WS_CHILD|WS_CLIPCHILDREN, 0, 3000);
	ATLVERIFY( m_viewVulfix.Load( IDR_BK_VULDLG_MAIN ) );
	m_viewVulfix.m_viewSoftVul.SetMainDlg( this );
	m_viewVulfix.Init(m_hWnd);
}

BOOL CBeikeSafeMainDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{

	SetIcon(::LoadIcon((HMODULE)&__ImageBase, MAKEINTRESOURCE(IDI_BEIKESAFE)));
	SetIcon(::LoadIcon((HMODULE)&__ImageBase, MAKEINTRESOURCE(IDI_SMALL)), FALSE);

	_Module.SetActiveWindow(m_hWnd);

	InitVulFix();


	//(*m_ExamHandler).InitCtrl();


	SetTabCurSel(IDC_TAB_MAIN, 0);

	m_wndHtmlOnlineInfo.Create(GetViewHWND(), IDC_IE_ONLINE_INFO, FALSE, RGB(0xFB, 0xFC, 0xFD));


	//延迟加载
	SetTimer( TIMER_ID_MAIN_DLG_ONCE, 10, NULL );

	return TRUE;
}

void CBeikeSafeMainDlg::OnBkBtnClose()
{
	EndDialog(IDCANCEL);
}


LRESULT CBeikeSafeMainDlg::OnDestroy()
{
	
	SetMsgHandled(FALSE);

	return 0;
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

void CBeikeSafeMainDlg::TabMainSelChange( int nTabOldItem, int nTabNewItem )
{
	SetTabCurSel(IDC_TAB_MAIN, nTabNewItem);
	OnBkTabMainSelChange( nTabOldItem, nTabNewItem );
}

BOOL CBeikeSafeMainDlg::OnBkTabMainSelChange(int nTabItemIDOld, int nTabItemIDNew)
{

	DEBUG_TRACE(L"Tab Change %d, %d\r\n", nTabItemIDOld, nTabItemIDNew);

	BOOL bRet = FALSE;

	if (m_bFirstPageChange)
	{
		if (0 != nTabItemIDNew)
		{
			PostMessage(MSG_APP_DELAY_EXAM, FALSE);
		}

		m_bFirstPageChange = FALSE;
	}



	bRet = TRUE;
	switch (nTabItemIDNew)
	{
	case 0:
		if (m_bPage0NeverShowed)
		{
			PostMessage(MSG_APP_DELAY_NAVIGATE_IE);
			PostMessage(MSG_APP_DELAY_EXAM, TRUE);

			m_bPage0NeverShowed = FALSE;
		}

		break;

	case 1:
		break;

	case 2:		
		break;
	case 3:
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
		break;
	case  4://系统优化
	case  5://清理
	case  6://网盾
	case  8:// 打开软件管理
	default:
		break;
	}

	return TRUE;
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

LRESULT CBeikeSafeMainDlg::OnAppDelayNavigateIE(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	CString	strURL;
// 	if ( SUCCEEDED(CAppPath::Instance().GetLeidianDataPath(strURL)) )
// 		strURL.Append(_T("\\html\\error.html"));

	m_wndHtmlOnlineInfo.Show2(L"http://www.ijinshan.com/safe/embed2.htm",strURL);

	return 0;
}

