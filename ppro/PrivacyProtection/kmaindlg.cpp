#include "stdafx.h"
#include "PrivacyProtectionGUI.h"
#include "kmaindlg.h"
#include <Psapi.h>
#include "KLogic.h"
#include <atlpath.h>
#include "KClipBoardDlg.h"
#include <GdiplusGpStubs.h>
#include <GdiplusFlat.h>
#include "GdiplusImaging.h"
#include "beikesafemsgbox.h"
#include "AskExitDlg.h"

#include "KWarningDlg.h"
using namespace Gdiplus;
using namespace DllExports;
#include "kutfconv.h"
#include "./Carmera/serachcamera.h"
#include "KAddSoftwareDlg.h"
#include "KXmlWriter.h"
#include "KXmlReader.h"
#include "MinAskDlg.h"
#include "KSignCache.h"


#define MONITOR_TIMER				500
#define TRAY_ICON_ID				101
#define TIP_TIMER					501

#define CLSID_BMP     _T("{557CF400-1A04-11D3-9A73-0000F81EF32E}")
#define CLSID_JPEG    _T("{557CF401-1A04-11D3-9A73-0000F81EF32E}")
#define CLSID_GIF     _T("{557CF402-1A04-11D3-9A73-0000F81EF32E}")
#define CLSID_TIFF    _T("{557CF405-1A04-11D3-9A73-0000F81EF32E}")
#define CLSID_PNG     _T("{557CF406-1A04-11D3-9A73-0000F81EF32E}")


#define GUID_Compress		_T("{E09D739D-CCD4-44EE-8EBA-3FBF8BE4FC58}")
#define GUID_ColorDepth		_T("{66087055-AD66-4C7C-9A18-38A2310B8337}")
#define GUID_Quality		_T("{1D5BE4B5-FA4A-452D-9CDD-5DB35105E7EB}")
#define GUID_Transform		_T("{8D0EB2D1-A58E-4EA8-AA14-108074B7B6F9}")
#define GUID_SaveFlag		_T("{292266FC-AC40-47BF-8CFC-A85B89A655DE}")
//#define MAX_UPLOAD_SIZE				1024 * 1024 * 2
//#define SAVE_SIZE					5000

//////////////////////////////////////////////////////////////////////////


bool KCreateLongDir(
					LPCWSTR lpPathName, 
					LPSECURITY_ATTRIBUTES lpSecurityAttributes
					)
{
	if (lpPathName == NULL)
	{
		return false;
	}
	if (::PathIsDirectoryW(lpPathName))
	{
		return true;
	}
	else 
	{
		WCHAR szFatherPath[MAX_PATH];
		::lstrcpynW(szFatherPath, lpPathName, MAX_PATH);
		LPWSTR lpSlash = ::wcsrchr(szFatherPath, TEXT('\\'));
		if (lpSlash != NULL)
		{
			*lpSlash = 0;
		}
		// 递归，先创建基层目录，再创建子目录。
		if (lpSlash == NULL || KCreateLongDir(szFatherPath, lpSecurityAttributes))
		{
			if (::CreateDirectoryW(lpPathName, lpSecurityAttributes))
			{
				return true;
			}
		}
		return false;
	}
}

BOOL KDelAndUninstallSvr(LPWSTR szSvrName)
{
    SC_HANDLE hServiceMgr = NULL, hServiceTwdm = NULL;
    SERVICE_STATUS SvrSta;
    hServiceMgr = OpenSCManagerW( NULL, NULL, SC_MANAGER_ALL_ACCESS );
    if( hServiceMgr == NULL ) 
    {return FALSE;}
    hServiceTwdm = OpenServiceW( hServiceMgr, szSvrName, SERVICE_ALL_ACCESS );

    if( hServiceTwdm == NULL ) {
        CloseServiceHandle( hServiceMgr );
        return FALSE;
    }

    //动态卸载驱动程序。
    DeleteService( hServiceTwdm );
    if (hServiceTwdm) {
        CloseServiceHandle( hServiceTwdm );
        hServiceTwdm = NULL;
    }

    if (hServiceMgr) {
        CloseServiceHandle( hServiceMgr );
        hServiceMgr = NULL;
    }

    WCHAR   wszFolderPath[MAX_PATH + 1]; 
    if (!::SHGetSpecialFolderPathW(NULL, wszFolderPath, CSIDL_APPDATA, 0)) {return FALSE;}

    WCHAR   wszPath[MAX_PATH + 1]; 
    ::GetModuleFileNameW(NULL, wszPath, MAX_PATH);
    ::PathStripPathW(wszPath);
    ::PathAppendW(wszFolderPath, wszPath);

    ::PathAppendW(wszFolderPath, L"ksafefilemon.sys");
    ::DeleteFile(wszFolderPath);

    return TRUE;
}

BOOL KIsInstalled(LPWSTR szServiceName)
{
	BOOL bResult = FALSE;

	//打开服务控制管理器
	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hSCM != NULL)
	{
		//打开服务
		SC_HANDLE hService = ::OpenServiceW(hSCM, szServiceName, SERVICE_QUERY_CONFIG);
		if (hService != NULL)
		{
			bResult = TRUE;
			::CloseServiceHandle(hService);
		}
		::CloseServiceHandle(hSCM);
	}
	return bResult;
}

BOOL KInstallSys(LPWSTR szServiceName, LPWSTR szFilePath)
{
    WCHAR wszMes[255] = {0};
    BOOL nRet = FALSE;

	SC_HANDLE hSCM = NULL;
	SC_HANDLE hService = NULL;

	if (KIsInstalled(szServiceName)) {
        OutputDebugString(L"KIsInstalled");
        nRet = TRUE;
        goto _Exit;
    }

	hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
        wsprintf(wszMes, L"OpenSCManager Fail %x\n", GetLastError ());
        OutputDebugString(wszMes);
        nRet = FALSE;
        goto _Exit;
	}

	hService = ::CreateServiceW(
		hSCM, szServiceName, szServiceName,
		SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
		szFilePath, NULL, NULL, L"", NULL, NULL);

	if (hService == NULL)
	{
        wsprintf(wszMes, L"CreateServiceW Fail %x\n", GetLastError ());
        OutputDebugString(wszMes);
		::CloseServiceHandle(hSCM);
        if (GetLastError() == 0x00000431) {
            nRet = TRUE;
        } else {
            nRet = FALSE;
        }
        goto _Exit;
	}

    OutputDebugString(L"CreateServiceW Suc");
    nRet = TRUE;

_Exit:
    if (hService) {
        ::CloseServiceHandle(hService);
        hService = NULL;
    }

    if (hSCM) {
        ::CloseServiceHandle(hSCM);
        hSCM = NULL;
    }
	
	return nRet;
}

HRESULT KStartServices( LPCWSTR lpwszServiceName )
{
	HRESULT hresult = S_OK;
	SC_HANDLE	hScManager  = NULL;
	SC_HANDLE	hService = NULL;
	SERVICE_STATUS ServiceStatus;

    WCHAR wszMes[255] = {0};

	hScManager = ::OpenSCManager( NULL, NULL, GENERIC_READ | GENERIC_EXECUTE );
	if ( NULL == hScManager )
	{
		OutputDebugStringW(L"Couldn't Open SCManager");
		hresult = E_FAIL;
		goto Exit0;
	}

	hService = ::OpenServiceW( hScManager, lpwszServiceName, GENERIC_READ | GENERIC_EXECUTE );
	if ( NULL == hService )
	{
		OutputDebugStringW(L"Couldn't Open Service");
		hresult = E_FAIL;
		goto Exit0;
	}

	if ( ::QueryServiceStatus (hService, &ServiceStatus ) )
	{
		if ( ( ServiceStatus.dwCurrentState != SERVICE_STOPPED ) )
		{
			OutputDebugStringW(L"ServiceStatus.dwCurrentState != SERVICE_STOPPED");
			hresult = S_OK;
			goto Exit0;
		}
	}

	if (!::StartService( hService, 0, NULL ) )
	{
		DWORD dwLastError = GetLastError ();
		if ( ERROR_SERVICE_ALREADY_RUNNING == dwLastError )
		{
			OutputDebugStringW(L"ERROR_SERVICE_ALREADY_RUNNING");
			hresult = S_OK;
			goto Exit0;
		}
        wsprintf(wszMes, L"StartService~ %x\n", dwLastError);
        OutputDebugString(wszMes);
		int nCount = 5;
		while(nCount--> 0)
		{
			::Sleep(1000);
			if ( ::QueryServiceStatus (hService, &ServiceStatus ) )
			{
				if ( ( ServiceStatus.dwCurrentState == SERVICE_RUNNING ) )
				{
					OutputDebugStringW(L"dwCurrentState != SERVICE_RUNNING");
					hresult = S_OK;
					goto Exit0;
				}
                wsprintf(wszMes, L"QueryServiceStatus %x\n", ServiceStatus.dwCurrentState);
                OutputDebugString(wszMes);
			}
            wsprintf(wszMes, L"QueryServiceStatus Fail %x\n", GetLastError());
            OutputDebugString(wszMes);
		}

		hresult = E_FAIL;
	}
    OutputDebugStringW(L"Ok!");

Exit0:

	if ( hService )
	{
		::CloseServiceHandle ( hService );
		hService = NULL;
	}

	if ( hScManager )
	{
		::CloseServiceHandle ( hScManager );
		hScManager = NULL;
	}

    wsprintf(wszMes, L"KStartServices end %x\n", hresult);
    OutputDebugString(wszMes);
	
	return hresult;
}


DWORD WINAPI KMainDlg::KpReleaseAndSatrtSys(LPVOID lpThreadParameter)
{
	HINSTANCE hInstance = ::GetModuleHandle(NULL);//当前exe文件的句柄 

	HRSRC hResInfo = ::FindResourceW(hInstance, MAKEINTRESOURCE(IDR_SYS), L"SYS"); 
	if (!hResInfo) {return FALSE;}

	HGLOBAL hgRes = ::LoadResource(hInstance, hResInfo);
	if (!hgRes) {return FALSE;}

	void* pvRes = ::LockResource(hgRes);
	if (!pvRes) {return FALSE;}

	DWORD cbRes = ::SizeofResource(hInstance, hResInfo); 
	if (10 > cbRes) {return FALSE;}

	WCHAR   wszFolderPath[MAX_PATH + 1]; 
	if (!::SHGetSpecialFolderPathW(NULL, wszFolderPath, CSIDL_APPDATA, 0)) {return FALSE;}

	WCHAR   wszPath[MAX_PATH + 1]; 
	::GetModuleFileNameW(NULL, wszPath, MAX_PATH);
	::PathStripPathW(wszPath);
	//::PathAppendW(wszFolderPath, wszPath);
    ::PathAppendW(wszFolderPath, L"kprivacy.exe");

	KCreateLongDir(wszFolderPath, NULL);
	::PathAppendW(wszFolderPath, L"ksafefilemon.sys");

	HANDLE hFile = CreateFileW(wszFolderPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 0);
	if (NULL == hFile || INVALID_HANDLE_VALUE == hFile) {return FALSE;}

	DWORD cbWritten = 0;
	::WriteFile(hFile, pvRes, cbRes, &cbWritten, 0);
	if (hFile) {
		::CloseHandle(hFile);
		hFile = NULL;
	}

	int nCount = 5;
	while(nCount--> 0)
	{
		if (KInstallSys(L"ksafefilemon", wszFolderPath) &&
			S_OK == KStartServices(L"ksafefilemon")) {
			break;
		}

		::Sleep(1000);
	}

	if (lpThreadParameter)
	{
		KMainDlg* pThis = (KMainDlg*)lpThreadParameter;
		pThis->m_bInit = true;
	}

	return TRUE;
}

BOOL KMainDlg::KReleaseAndSatrtSys()
{
	DWORD dwThreadId = 0;
	HANDLE hThread = ::CreateThread(NULL, 0, KpReleaseAndSatrtSys, this, 0, &dwThreadId);
	return NULL != hThread ? TRUE : FALSE;
}
//////////////////////////////////////////////////////////////////////////

KMainDlg::~KMainDlg()
{
	m_ActorInit.Stop();
}

void KMainDlg::SetDefaultNavigate(LPCWSTR lpszNavigate)
{
    m_strDefaultNavigate = lpszNavigate;
}

LRESULT KMainDlg::OnAppDefaultNavigate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
    return 0;
}

BOOL KMainDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{
	OutputDebugString(_T("KMainDlg::OnInitDialog Start."));

    SetIcon(::LoadIcon((HMODULE)&__ImageBase, MAKEINTRESOURCE(IDI_KAV_MAIN)));
    SetIcon(::LoadIcon((HMODULE)&__ImageBase, MAKEINTRESOURCE(IDI_KAV_MAIN)), FALSE);

    _Module.SetActiveWindow(m_hWnd);

	SetItemText(201, BkString::Get(133));


	m_MonitorItemlist.SetClickLinkCallback(static_cast<IClickLinkCallback*>(this) );
	m_MonitorItemlist.Create( 
		GetViewHWND(), NULL, NULL, 
		WS_CHILDWINDOW|WS_VISIBLE|WS_TABSTOP|LBS_NOTIFY|LBS_OWNERDRAWVARIABLE|LBS_NOINTEGRALHEIGHT|WS_VSCROLL, 
		0, 2000, NULL);

	MONITOR_ITEM Item;

	Item.nType = enumMonitorItemTypeNONE;

	Item.strName = BkString::Get(102);
	Item.bCheck = true;
	Item.bDel = false;
	Item.bDisable = true;
	Item.nImg = IDB_IE;
	Item.bNew = false;
	m_MonitorItemlist.Add(Item);

	//Item.strName = BkString::Get(122);
	//Item.bCheck = true;
	//Item.bDel = false;
	//Item.bDisable = true;
	//Item.nImg = IDB_VIDEO;
	//m_MonitorItemlist.Add(Item);


	Item.strName = BkString::Get(103);
	Item.bCheck = true;
	Item.bDel = false;
	Item.bDisable = true;
	Item.nImg = IDB_QQ;
	Item.bNew = false;
	m_MonitorItemlist.Add(Item);

	Item.strName = BkString::Get(104);
	Item.bCheck = true;
	Item.bDel = false;
	Item.bDisable = true;
	Item.nImg = IDB_MSN;
	Item.bNew = false;
	m_MonitorItemlist.Add(Item);

	Item.strName = BkString::Get(110);
	Item.bCheck = true;
	Item.bDel = false;
	Item.bDisable = true;
	Item.nImg = IDB_ALIWW;
	Item.bNew = false;
	m_MonitorItemlist.Add(Item);

	Item.strName = BkString::Get(105);
	Item.bCheck = true;
	Item.bDel = false;
	Item.bDisable = true;
	Item.nImg = IDB_MY_DOC;
	Item.bNew = false;
	m_MonitorItemlist.Add(Item);

	Item.strName = BkString::Get(123);
	Item.bCheck = true;
	Item.bDel = false;
	Item.bDisable = true;
	Item.nImg = IDB_MY_EMAIL;
	Item.bNew = false;
	m_MonitorItemlist.Add(Item);

	Item.nType = enumMonitorItemTypeMyCamera;
	Item.strName = BkString::Get(122);
	Item.bCheck = true;
	Item.bDel = false;
	Item.bDisable = false;
	Item.nImg = IDB_MY_CAMERA;
	Item.bNew = true;
	m_MonitorItemlist.Add(Item);
	m_bCamera = true;


	Item.nType = enumMonitorItemTypeMyComputer;
	Item.strName = BkString::Get(121);
	Item.bCheck = false;
	Item.bDel = false;
	Item.bDisable = false;
	Item.nImg = IDB_MY_COMPUTER;
	Item.bNew = false;
	m_MonitorItemlist.Add(Item);


	if (!m_RuleImpl.Init())
		return FALSE;

	AddMonitorList();

	m_ResultList.Create( 
		GetViewHWND(), NULL, NULL, 
		WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL , 
		0, 2001, NULL);
	{
		INT nRow = 0;
		m_ResultList.InsertColumn(nRow++, BkString::Get(106), LVCFMT_CENTER, 136);
		m_ResultList.InsertColumn(nRow++, BkString::Get(107), LVCFMT_CENTER, 175);
		m_ResultList.InsertColumn(nRow++, BkString::Get(108), LVCFMT_CENTER, 120);
		m_ResultList.InsertColumn(nRow++, BkString::Get(109), LVCFMT_CENTER, 80);
	}
	m_ResultList.SetObserverWindow( m_hWnd );


	// software
	TCHAR szFolderPath[MAX_PATH + 1] = _T(""); 
	if (::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_APPDATA, 0)) 
	{
		TCHAR szPath[MAX_PATH + 1] = _T(""); 
		::GetModuleFileName(NULL, szPath, MAX_PATH);
		::PathStripPath(szPath);
		::PathAppend(szFolderPath, _T("Kingsoft"));
		if (!::PathFileExists(szFolderPath))
		{
			CreateDirectory(szFolderPath, NULL);
		}

		::PathAppend(szFolderPath, _T("金山隐私保护"));
		if (!::PathFileExists(szFolderPath))
		{
			CreateDirectory(szFolderPath, NULL);
		}

		m_SaveResultFilePath = szFolderPath;
	}

	m_MonitorSoftwarelist.SetClickLinkCallback(static_cast<IClickLinkCallback*>(this) );
	m_MonitorSoftwarelist.Create( 
		GetViewHWND(), NULL, NULL, 
		WS_CHILDWINDOW|WS_VISIBLE|WS_TABSTOP|LBS_NOTIFY|LBS_OWNERDRAWVARIABLE|LBS_NOINTEGRALHEIGHT|WS_VSCROLL, 
		0, 2002, NULL);

	AddMonitorSoftwareList();

	m_SoftwareResultList.Create( 
		GetViewHWND(), NULL, NULL, 
		WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL , 
		0, 2003, NULL);
	{
		INT nRow = 0;
		m_SoftwareResultList.InsertColumn(nRow++, BkString::Get(106), LVCFMT_CENTER, 136);
		m_SoftwareResultList.InsertColumn(nRow++, BkString::Get(107), LVCFMT_CENTER, 175);
		m_SoftwareResultList.InsertColumn(nRow++, BkString::Get(108), LVCFMT_CENTER, 120);
		m_SoftwareResultList.InsertColumn(nRow++, BkString::Get(109), LVCFMT_CENTER, 80);
	}
	m_SoftwareResultList.SetObserverWindow( m_hWnd );


	m_ActorInit.Startup(this);
	m_ActorCleaner.Startup(this);


	OutputDebugString(_T("KMainDlg::OnInitDialog End."));
    return TRUE;
}

void KMainDlg::OnBtnMax()
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

static void TrayIconAdd(HWND hWnd, HICON hIcon)
{
	NOTIFYICONDATA nid = { 0 };

	nid.cbSize = sizeof (nid);
	nid.hIcon = hIcon;
	nid.hWnd = hWnd;
	nid.uID = TRAY_ICON_ID;
	nid.uFlags = NIF_ICON | NIF_MESSAGE;
	nid.uCallbackMessage = MSG_TRAYICON;

	Shell_NotifyIcon(NIM_ADD, &nid);
}

void KMainDlg::SetTipInfo(LPCTSTR lpText, LPCTSTR lpTittle, int nType)
{
	NOTIFYICONDATA nid = { 0 };

	nid.cbSize = sizeof (nid);
	nid.hWnd = m_hWnd;
	nid.uID = TRAY_ICON_ID;

	nid.uFlags = NIF_TIP;
	_tcsncpy(nid.szTip, BkString::Get(1), 128);

	nid.szTip[128 - 1] = 0;
	if (nType == 1)
	{
		nid.szInfo[0] = 0;
		nid.szInfoTitle[0] = 0;
	}
	else if (nType == 2)
	{
		nid.dwInfoFlags = NIIF_INFO;
		_tcsncpy(nid.szInfo, lpText, 255);
		nid.szInfo[255 - 1] = 0;
		_tcsncpy(nid.szInfoTitle, lpTittle, 64);
		nid.szInfoTitle[64 - 1] = 0;
		nid.uTimeout = 1000;	// 被自动设为10秒，故无效，用定时器实现timeout
		nid.uFlags |= NIF_INFO;
	}	
	//SetTimer(TIP_TIMER, 1000);
	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

typedef BOOL (WINAPI *PFN_ChangeWindowMessageFilter)(UINT uMessage, int nFlag);
BOOL WINAPI	Vista_ChangeWindowMessageFilter(UINT uMessage, int nFlag)
{
	HMODULE hUserMod;
	PFN_ChangeWindowMessageFilter pfnChangeWindowMessageFilter;

	hUserMod = GetModuleHandle(_T("user32.dll"));
	pfnChangeWindowMessageFilter = (PFN_ChangeWindowMessageFilter)GetProcAddress(hUserMod, "ChangeWindowMessageFilter");

	return (pfnChangeWindowMessageFilter != NULL) && pfnChangeWindowMessageFilter(uMessage, nFlag);
}

#define WM_SHELLAPPBAR			(WM_USER + 105)
#define MSGFLT_ADD		1

static void	ShellAppBarMsgAdd(HWND hWnd)
{
	APPBARDATA abd = { 0 };

	abd.cbSize = sizeof(APPBARDATA);  
	abd.hWnd = hWnd;
	abd.uCallbackMessage = WM_SHELLAPPBAR;

	Vista_ChangeWindowMessageFilter(WM_SHELLAPPBAR, MSGFLT_ADD);
	SHAppBarMessage(ABM_NEW, &abd);
}

void KMainDlg::CloseByType(int nType)
{
	if (nType == 1)
	{// 最小化托盘，继续监控
		ShowTray();
		SetTipInfo(BkString::Get(144), BkString::Get(1), 2);
	}
	else if (nType == 2)
	{// 直接关闭
		EndDialog(IDCANCEL);	
	}
	else
	{// 没有选择
		NULL;
	}
}

void KMainDlg::OnBtnClose()
{
	if (!m_bStart)
	{
		EndDialog(IDCANCEL);
		return;
	}
	int nCloseChoice = 0;
	BOOL bValueExist = FALSE;
	bool bRet = FALSE;
	KXmlReader xmlReader;
	if (xmlReader.LoadFile(CStringA(m_SaveResultFilePath) + "\\cfg.dat"))
	{
		
		xmlReader.Open("root");
		bValueExist = xmlReader.Read("closeButtonChoice", nCloseChoice);
	}

	if (!bValueExist)
	{// 如果没有记录
		nCloseChoice = 1;
	}
	else
	{
		CloseByType(nCloseChoice);
		return;
	}

	KAskExitDlg dlg(nCloseChoice);
	UINT_PTR nResult = dlg.DoModal();
	int nRadioChoice = -1;

	if (nResult == IDABORT)
		return;
	else if (nResult == IDYES)
	{
		nRadioChoice = dlg.GetRadioCheck();
		CloseByType(nRadioChoice);
	}
	else if (nResult == IDNO)
	{
		//NULL;
	}

	if (dlg.GetChecked() && nRadioChoice != -1)
	{
		KXmlWriter xmlWriter;
		xmlWriter.Open("root");
		xmlWriter.Write("closeButtonChoice", nRadioChoice);
		xmlWriter.WriteFile(CStringA(m_SaveResultFilePath) + "\\cfg.dat");
	}
}

void KMainDlg::ShowTray()
{
	MENUITEMINFO mii = { 0 };
	mii.cbSize = sizeof (mii);
	mii.fMask = MIIM_STATE;
	mii.fState = MFS_DEFAULT;

	m_hTrayMenu = GetSubMenu(AtlLoadMenu(MAKEINTRESOURCE(IDR_MENU_TRAY)), 0);
	SetMenuItemInfo(m_hTrayMenu, ID_MAIN_APP, MF_BYCOMMAND, &mii);

	m_hIcons = AtlLoadIcon(IDI_KAV_MAIN);

	TrayIconAdd(m_hWnd, m_hIcons);

	ShellAppBarMsgAdd(m_hWnd);
	Vista_ChangeWindowMessageFilter(m_uTaskbarCreatedMsg, MSGFLT_ADD);


	ShowWindow(SW_HIDE);
}

void KMainDlg::OnBtnMin()
{
	if (m_bStart)
	{
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE | HTCAPTION, 0);
		return;
	}
	KMinAskDlg dlg;
	UINT_PTR nResult = dlg.DoModal();

	if (nResult == IDABORT)
	{// 直接关闭
		return;
	}
	else if (nResult == IDYES)
	{// 选择仅最小化
		NULL;
	}
	else if (nResult == IDNO)
	{// 最小化且开监控
		//ShowTray();
		SendMessage(MSG_START);
	}
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE | HTCAPTION, 0);
	//EmptyWorkingSet(GetCurrentProcess());
}
LRESULT KMainDlg::OnTaskbarCreated(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TrayIconAdd(m_hWnd, m_hIcons);
	return 0;
}

void KMainDlg::OnBtnToFriend()
{
	//MessageBox(L"此功能还未开放！", L"提示！", MB_OK);
	BOOL nRetCode = FALSE;
	KClipBoardDlg dlg;
	nRetCode = dlg.DoModal();
}

void KMainDlg::OnBtnPic()
{
	SaveCapture();
}

CString KMainDlg::GetGUID(int nIndex)
{
	switch (nIndex)
	{
	case 1:
		return CLSID_PNG;
	case 2:
		return CLSID_JPEG;
	case 3:
		return CLSID_BMP;
	case 4:
		return CLSID_GIF;
	case 5:
		return CLSID_TIFF;
	default:
		return _T("");
	}
}


BOOL KMainDlg::GetSaveRect( OUT CRect &cRect )
{
	HDC screenDC;

	int nWidth;
	int nHeight;

	screenDC = CreateDC(_T("display"),NULL, NULL, NULL);
	if (NULL == screenDC)
		return FALSE;

	if (!GetWindowRect(cRect))
		return FALSE;
	
	int xRes = GetDeviceCaps(screenDC, HORZRES);
	int yRes = GetDeviceCaps(screenDC, VERTRES);

	nWidth	= (cRect.right < xRes ? cRect.right : xRes)	- (cRect.left > 0 ? cRect.left : 0);
	nHeight	= (cRect.bottom < yRes ? cRect.bottom : yRes) - (cRect.top > 0 ? cRect.top : 0);

	if (cRect.left > 0)
		cRect.left = 0;

	if (cRect.top > 0)
		cRect.top = 0;

	cRect.right = cRect.left + nWidth;
	cRect.bottom = cRect.top + nHeight;

	DeleteDC(screenDC);

	return TRUE;
}

BOOL KMainDlg::SaveCapture()
{
	// 获取图像
	CRect cRect;
	GetSaveRect(cRect);

	CDCHandle dc = GetWindowDC();
	CDC	hDCMem;
	hDCMem.CreateCompatibleDC(dc);

	HBITMAP hBitmap = CreateCompatibleBitmap(dc, cRect.Width(), cRect.Height());
	HGDIOBJ hOldBitmap = SelectObject(hDCMem, hBitmap);
	hDCMem.BitBlt(
		cRect.left > 0 ? 0 : cRect.left, 
		cRect.top  > 0 ? 0 : cRect.top,
		cRect.Width() + (cRect.left > 0 ? 0 : -cRect.left), 
		cRect.Height() + (cRect.top > 0 ? 0 : -cRect.top), 
		dc, 0, 0, SRCCOPY);

	GpBitmap* pBitmap = NULL;
	GpStatus retStatust = GdipCreateBitmapFromHBITMAP(hBitmap, (HPALETTE)CF_BITMAP, &pBitmap);

	CLSID lcEncode;
	ZeroMemory(&lcEncode, sizeof(CLSID));
	
	// 保存图像
	CString strFilePath;
	int nRet = GetSaveAsPath(strFilePath);
	if (nRet == -1 || strFilePath.IsEmpty())
		return FALSE;

	GUID lcGuid;
	ZeroMemory(&lcGuid, sizeof(GUID));
	CLSIDFromString((LPOLESTR)GUID_Quality, &lcGuid);

	EncoderParameters encoderParameters;
	ULONG quality = 100;
	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = lcGuid;
	encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;
	encoderParameters.Parameter[0].Value = &quality;

	HRESULT hResult = CLSIDFromString((LPOLESTR)(LPCTSTR)GetGUID(nRet), &lcEncode);
	retStatust = GdipSaveImageToFile(pBitmap, strFilePath, &lcEncode, NULL);
	
	SelectObject(hDCMem, hOldBitmap);
	DeleteObject(hBitmap);
	ReleaseDC(dc);

	// 结果提示
	KWarningDlg kWarn;
	if (Ok == retStatust)
		kWarn.SetTextAndIcon(1);
	else
		kWarn.SetTextAndIcon(2);
	kWarn.DoModal();
	return TRUE;
}

int KMainDlg::GetSaveAsPath(CString &strPath) 
{
	// 过滤器
	TCHAR szFilter[] = _T(	"PNG 图像文件 (*.png)\0*.png\0\
							JPEG 图像文件 (*.jpg)\0*.jpg\0\
							BMP 图像文件 (*.bmp)\0*.bmp\0\0"	);

	CFileDialog FileDlg(FALSE, 0, 0, 
		OFN_HIDEREADONLY | OFN_EXPLORER | OFN_OVERWRITEPROMPT, 
		szFilter, this->m_hWnd); 
	
	// 初始化 后缀，文件名，保存路径
	TCHAR szDefExt[MAX_PATH];
	_tcsncpy(szDefExt, _T("gif"), sizeof(_T("gif")) );
	TCHAR szDefFileName[MAX_PATH];
	_tcsncpy(szDefFileName, _T("金山隐私保护截图"), sizeof(_T("金山隐私保护截图")) );
	TCHAR szDefFilePath[MAX_PATH];
	if(!SHGetSpecialFolderPath(NULL, szDefFilePath, CSIDL_DESKTOPDIRECTORY, FALSE))
		return -1;
	
	// 初始化
	FileDlg.m_ofn.lpstrInitialDir = szDefFilePath;
	FileDlg.m_ofn.lpstrDefExt = szDefExt;
	FileDlg.m_ofn.lpstrFile = szDefFileName;
	FileDlg.m_ofn.hwndOwner = this->m_hWnd;
	FileDlg.m_ofn.nFilterIndex = 1;		// 用来设置默认类型，现为gif
	
	// open file diadlg 
	if(FileDlg.DoModal() == IDOK) 
	{ 
		// get save file name 
		strPath.Format(_T("%s"), FileDlg.m_ofn.lpstrFile);
	}
	if (strPath.IsEmpty())
		return -1;
	else
		return FileDlg.m_ofn.nFilterIndex;	// 返回用户选择的类型索引
}

void KMainDlg::OnSelectAll()
{
	SetItemCheck(105, !GetItemCheck(105));
}

void KMainDlg::OnAddFile()
{
	wchar_t wcsFilter[128] = {0};
	wcsncpy(wcsFilter,L"All Files (*.*)\0*.*\0\0",21);
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLESIZING, 
		wcsFilter,m_hWnd);

	INT_PTR nRet = dlg.DoModal(m_hWnd);
	if (IDOK == nRet)
	{
		CString strFile = dlg.m_ofn.lpstrFile;
		if( !m_MonitorItemlist.IsItemInList( strFile.MakeLower() ) )
		{
			MONITOR_ITEM MonitorItem;
			MonitorItem.bCheck = true;
			MonitorItem.bDel = true;
			MonitorItem.bDisable = false;
			MonitorItem.nImg = IDB_CUSTOM;
			MonitorItem.bNew = false;


			HICON hIcon = NULL;
			if (m_Logic.GetFileIcon(dlg.m_ofn.lpstrFile, hIcon))
				MonitorItem.nImg = (int)hIcon;
			MonitorItem.nType = enumMonitorItemTypeFile;
			MonitorItem.strMonitorItem = strFile;
			MonitorItem.strName = ATLPath::FindFileName(dlg.m_ofn.lpstrFile);
			MonitorItem.strTip = dlg.m_ofn.lpstrFile;
			m_MonitorItemlist.Add( MonitorItem );

			m_RuleImpl.AddPrivacyFile(MonitorItem.strMonitorItem);
			m_RuleImpl.ReloadRule();
		}
		else
		{	
			//MessageBox(L"您选择的文件已经存在，不用再次添加。", L"提示！", MB_OK);
			KWarningDlg kWarn(2, L"您选择的文件已经存在，不用再次添加。", -34);
			kWarn.DoModal();
		}
	}
}

void KMainDlg::OnAddDir()
{
	CFolderDialog Dlg;
	Dlg.DoModal();
	
	CString strDir = Dlg.GetFolderPath();
	if (strDir.IsEmpty())
		return;
		
	if(!m_MonitorItemlist.IsItemInList( CString(strDir).MakeLower() ) )
	{
		if (strDir.Mid(0, 2) == _T("\\\\"))
		{
			KWarningDlg kWarn(2, L"无法添加您选择的文件夹，请另选文件夹。", -34);
			kWarn.DoModal();
			//MessageBox(L"无法添加您选择的文件夹，请另选文件夹。", L"提示！", MB_OK);
		}
		else
		{
			TCHAR szFolderPath[MAX_PATH + 1] = _T(""); 
			if (::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_MYDOCUMENTS, 0) && strDir == szFolderPath) 
			{
				KWarningDlg kWarn(2, L"您选择的文件夹已经存在，不用再次添加。", -34);
				kWarn.DoModal();
				return;
			}


			m_RuleImpl.AddPrivacyDir(CString(strDir).MakeLower());
			m_RuleImpl.ReloadRule();

			MONITOR_ITEM MonitorItem;
			MonitorItem.bCheck = true;
			MonitorItem.bDel = true;
			MonitorItem.bDisable = false;
			MonitorItem.nImg = IDB_CUSTOM;
			MonitorItem.bNew = false;
			HICON hIcon = NULL;
			ExtractIconEx(_T("SHELL32.DLL") , 4, NULL, &hIcon, 1); 
			if (hIcon)
				MonitorItem.nImg = (int)hIcon;
			MonitorItem.nType = enumMonitorItemTypeDir;
			MonitorItem.strName = ATLPath::FindFileName(strDir);
			MonitorItem.strTip = strDir;
			MonitorItem.strMonitorItem = CString(strDir).MakeLower();
			m_MonitorItemlist.Add( MonitorItem );
		}
	}
	else
	{
		KWarningDlg kWarn(2, L"您选择的文件夹已经存在，不用再次添加。", -34);
		kWarn.DoModal();
		//MessageBox(L"您选择的文件夹已经存在，不用再次添加。", L"提示！", MB_OK);
	}
}

void KMainDlg::OnRiskResultCheck()
{
	m_ResultList.SetOnlyShowRisk(GetItemCheck(109));
	m_SoftwareResultList.SetOnlyShowRisk(GetItemCheck(109));
}

void KMainDlg::OnRiskResult()
{
	SetItemCheck(109, !GetItemCheck(109));

	m_ResultList.SetOnlyShowRisk(GetItemCheck(109));
	m_SoftwareResultList.SetOnlyShowRisk(GetItemCheck(109));
}

void KMainDlg::OnClearResult()
{
	if (0 == GetTabCurSel(136))
	{
		if (m_ResultList.GetItemCount() <= 0)
			return;

		m_ResultList.DeleteAllItems();

	}
	else
	{
		if (m_SoftwareResultList.GetItemCount() <= 0)
			return;

		m_SoftwareResultList.DeleteAllItems();
	}

	m_strResult.Format(BkString::Get(119), 0, 0, 0);
	SetRichText(203, m_strResult);
}

void KMainDlg::OnExportResult()
{
	// 过滤器
	TCHAR szFilter[] = _T(	"文本文档(*.txt)\0*.txt\0\0"	);

	CFileDialog FileDlg(FALSE, 0, 0, 
		OFN_HIDEREADONLY | OFN_EXPLORER | OFN_OVERWRITEPROMPT, 
		szFilter, m_hWnd); 

	// 初始化文件名，保存路径
	TCHAR szDefFile[MAX_PATH] = _T("");
	_tcscpy(szDefFile, BkString::Get(140));
	TCHAR szDefFilePath[MAX_PATH] = _T("");
	if(SHGetSpecialFolderPath(NULL, szDefFilePath, CSIDL_DESKTOPDIRECTORY, FALSE))
		FileDlg.m_ofn.lpstrInitialDir = szDefFilePath;
	TCHAR szDefExt[MAX_PATH] = _T("txt");

	FileDlg.m_ofn.lpstrFile = szDefFile;
	FileDlg.m_ofn.lpstrDefExt = szDefExt;
	FileDlg.m_ofn.hwndOwner = m_hWnd;
	FileDlg.m_ofn.nFilterIndex = 1;


	INT_PTR nRet = FileDlg.DoModal(m_hWnd);
	if (IDOK != nRet)
		return;
	
	CString strFile = FileDlg.m_ofn.lpstrFile;

	KXmlWriter XmlWriter;
	CString strNode;
	strNode.Format(_T("%s\\%s"), BkString::Get(140), BkString::Get(142)); 
	XmlWriter.Open(CStringA(strNode));

	int nCount = GetItemCheck(109) ? m_ResultList.GetAllRiskCount() : m_ResultList.GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		CListViewCtrlEx::TListItem* pItem = m_ResultList._GetItemData(i);
		if (!pItem)
			continue;

		XmlWriter.AddNewElement(CStringA(BkString::Get(141)));
		XmlWriter.Write(CStringA(BkString::Get(106)), CStringA(pItem->subItems[0].str));
		XmlWriter.Write(CStringA(BkString::Get(107)), CStringA(pItem->subItems[1].str));
		XmlWriter.Write(CStringA(BkString::Get(108)), CStringA(pItem->subItems[2].str));
		XmlWriter.Write(CStringA(BkString::Get(109)), CStringA(pItem->subItems[3].str));
	}

	strNode.Format(_T("%s\\%s"), BkString::Get(140), BkString::Get(143)); 
	XmlWriter.Open(CStringA(strNode));

	nCount = GetItemCheck(109) ? m_SoftwareResultList.GetAllRiskCount() : m_SoftwareResultList.GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		CListViewCtrlEx::TListItem* pItem = m_SoftwareResultList._GetItemData(i);
		if (!pItem)
			continue;

		XmlWriter.AddNewElement(CStringA(BkString::Get(141)));
		XmlWriter.Write(CStringA(BkString::Get(106)), CStringA(pItem->subItems[0].str));
		XmlWriter.Write(CStringA(BkString::Get(107)), CStringA(pItem->subItems[1].str));
		XmlWriter.Write(CStringA(BkString::Get(108)), CStringA(pItem->subItems[2].str));
		XmlWriter.Write(CStringA(BkString::Get(109)), CStringA(pItem->subItems[3].str));
	}


	XmlWriter.WriteFile(CStringA(FileDlg.m_ofn.lpstrFile));

	KWarningDlg kWarn;
	kWarn.SetTextAndIcon(1);
}

void KMainDlg::OnAddSoftware()
{
	KAddSoftwareDlg Dlg;
	UINT_PTR nResult = Dlg.DoModal();
	if (nResult == IDABORT)
	{
		// 过滤器
		TCHAR szFilter[] = _T(	"程序\0*.exe;*.com;*.bat\0\0"	);

		CFileDialog FileDlg(TRUE, 0, 0, 
			OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ENABLESIZING, 
			szFilter, m_hWnd); 

		INT_PTR nRet = FileDlg.DoModal(m_hWnd);
		if (IDOK == nRet)
		{
			CString strFile = FileDlg.m_ofn.lpstrFile;
			if( !m_MonitorSoftwarelist.IsItemInList( strFile.MakeLower() ) )
			{
				MONITOR_ITEM MonitorItem;
				MonitorItem.bCheck = true;
				MonitorItem.bDel = true;
				MonitorItem.bDisable = false;
				MonitorItem.nImg = IDB_CUSTOM;
				MonitorItem.bNew = false;

				HICON hIcon = NULL;
				if (m_Logic.GetFileIcon(FileDlg.m_ofn.lpstrFile, hIcon))
					MonitorItem.nImg = (int)hIcon;
				MonitorItem.nType = enumMonitorItemTypeProcess;
				MonitorItem.strMonitorItem = strFile;
				MonitorItem.strName = ATLPath::FindFileName(FileDlg.m_ofn.lpstrFile);
				MonitorItem.strTip = FileDlg.m_ofn.lpstrFile;
				m_MonitorSoftwarelist.Add( MonitorItem );

				m_RuleImpl.AddMonitorProcess(MonitorItem.strMonitorItem);
				m_RuleImpl.ReloadRule();

				RESULT_INFO	Info;
				int nIndex = m_ResultInfoMap.FindKey(MonitorItem.strMonitorItem.MakeLower());
				if (-1 == nIndex)
				{	
					Info.strName	= MonitorItem.strName;
					Info.hIcon		= hIcon;
					Info.bMonitor	= TRUE;
					m_ResultInfoMap.Add(MonitorItem.strMonitorItem, Info);
				}
				else
				{
					m_ResultInfoMap.GetValueAt(nIndex).bMonitor = TRUE;
				}

			}
			else
			{	
				KWarningDlg kWarn(2, L"您选择的软件已经存在，不用再次添加。", -34);
				kWarn.DoModal();
			}
		}
	}
	else if (nResult == IDOK)
	{
		bool bWarning = false;
		for (int i = 0; i < Dlg.m_IndexArray.GetSize(); i++)
		{
			KAddSoftwareDlg::SOFTWARE_INFO& Info = Dlg.m_SoftwareMap.GetValueAt(Dlg.m_IndexArray[i]);

			MONITOR_ITEM MonitorItem;
			MonitorItem.bCheck = true;
			MonitorItem.bDel = true;
			MonitorItem.bDisable = false;
			MonitorItem.nImg = IDB_CUSTOM;
			MonitorItem.bNew = false;

			if (Info.hIcon)
				MonitorItem.nImg = (int)Info.hIcon;
			MonitorItem.nType = enumMonitorItemTypeProcess;
			MonitorItem.strMonitorItem = Info.strProcessFile;
			MonitorItem.strMonitorItem.MakeLower();
			MonitorItem.strName = Info.strName;
			MonitorItem.strTip = Info.strProcessFile;

			if (!m_MonitorSoftwarelist.IsItemInList( MonitorItem.strMonitorItem ))
			{
				RESULT_INFO	ResultInfo;
				int nIndex = m_ResultInfoMap.FindKey(MonitorItem.strMonitorItem);
				if (-1 == nIndex)
				{	
					ResultInfo.strName	= MonitorItem.strName;
					ResultInfo.hIcon	= Info.hIcon;
					ResultInfo.bMonitor	= TRUE;
					m_ResultInfoMap.Add(MonitorItem.strMonitorItem, ResultInfo);

					m_RuleImpl.AddMonitorProcess(MonitorItem.strMonitorItem);
					m_RuleImpl.ReloadRule();
					m_MonitorSoftwarelist.Add( MonitorItem );
				}
				else if (!m_ResultInfoMap.GetValueAt(nIndex).bMonitor)
				{
					m_RuleImpl.AddMonitorProcess(MonitorItem.strMonitorItem);
					m_RuleImpl.ReloadRule();
					m_MonitorSoftwarelist.Add( MonitorItem );
					m_ResultInfoMap.GetValueAt(nIndex).bMonitor = TRUE;
				}
			}
			else
			{
				bWarning = true;
			}
		}

		if (bWarning)
		{
			KWarningDlg kWarn(2, L"已经存在的软件，不用再次添加", -34);
			kWarn.DoModal();
		}
	}

	if (m_MonitorSoftwarelist.GetItemCount() > 0)
	{
		SetItemVisible(502, FALSE);
		SetItemVisible(501, TRUE);

		SetItemVisible(203, TRUE);
		m_strResult.Format(
			BkString::Get(119), 
			m_SoftwareResultList.GetAllSafeCount(), 
			m_SoftwareResultList.GetAllRiskCount(), 
			m_SoftwareResultList.GetAllUnknownCount());
		SetRichText(203, m_strResult);

		SetItemVisible(109, TRUE);
		SetItemVisible(110, TRUE);
		SetItemVisible(111, TRUE);
		SetItemVisible(113, TRUE);
		SetItemVisible(114, TRUE);
	}
}

LRESULT KMainDlg::OnUpdateResult(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	Filter((PVOID)wParam, lParam);
	return 0;
}

LRESULT KMainDlg::OnUpdateCameraInfo(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	if (wParam == 0)
		return FALSE;

	CString strProcessFile = ((LPTSTR)wParam);
	RESULT_INFO	Info;
	TCHAR* pszFile = ATLPath::FindFileName(strProcessFile);
	if (pszFile == NULL)
		Info.strName	= strProcessFile;
	else
		Info.strName	= pszFile;

	int nIndex = m_ResultInfoMap.FindKey(strProcessFile.MakeLower());
	if (-1 == nIndex)
	{	
		Info.bMonitor	= FALSE;

		if (!m_Logic.GetFileIcon(strProcessFile, Info.hIcon ) )
			Info.hIcon = NULL;

		m_ResultInfoMap.Add(strProcessFile, Info);
	}
	else
	{
		Info.strName = m_ResultInfoMap.GetValueAt(nIndex).strName;
		Info.hIcon = m_ResultInfoMap.GetValueAt(nIndex).hIcon;
		Info.bMonitor = m_ResultInfoMap.GetValueAt(nIndex).bMonitor;
	}

	int nLevel = CListViewCtrlEx::enumLevelUnknown;
	CString strLevel = BkString::Get(139);

	LPCSTR pSign = g_signCache.GetFileSign(strProcessFile, strProcessFile.GetLength());
	CStringA strSign;
	if (pSign)
		strSign = pSign;

	nIndex = m_WhiteSignDataMap.FindKey(CString(strSign).MakeLower());
	if (-1 != nIndex)
	{
		nLevel = CListViewCtrlEx::enumLevelSafe;
		strLevel = BkString::Get(114);
	}


	if (m_bCamera)
	{
		int nItem = m_ResultList.Append(Info.strName, Info.hIcon, (CListViewCtrlEx::LEVEL)nLevel);
		m_ResultList.AppendSubItem(nItem, _T("我的摄像头"), IDB_MY_CAMERA, SUBITEM_PNG);

		CTime tNow = CTime::GetCurrentTime();
		CString StrTime;
		StrTime.Format(_T("%4d-%02d-%02d %02d:%02d:%02d"), tNow.GetYear(), tNow.GetMonth(), tNow.GetDay(), tNow.GetHour(), tNow.GetMinute(), tNow.GetSecond());
		m_ResultList.AppendSubItem(nItem, StrTime);
		m_ResultList.AppendSubItem(nItem, strLevel);
	}


	if (Info.bMonitor)
	{
		int nItem = m_SoftwareResultList.Append(Info.strName, Info.hIcon, (CListViewCtrlEx::LEVEL)nLevel);
		m_SoftwareResultList.AppendSubItem(nItem, _T("我的摄像头"), IDB_MY_CAMERA, SUBITEM_PNG);

		CTime tNow = CTime::GetCurrentTime();
		CString StrTime;
		StrTime.Format(_T("%4d-%02d-%02d %02d:%02d:%02d"), tNow.GetYear(), tNow.GetMonth(), tNow.GetDay(), tNow.GetHour(), tNow.GetMinute(), tNow.GetSecond());
		m_SoftwareResultList.AppendSubItem(nItem, StrTime);
		m_SoftwareResultList.AppendSubItem(nItem, strLevel);
	}


	if (0 == GetTabCurSel(136))
	{
		m_strResult.Format(
			BkString::Get(119), 
			m_ResultList.GetAllSafeCount(), 
			m_ResultList.GetAllRiskCount(), 
			m_ResultList.GetAllUnknownCount());

		SetRichText(203, m_strResult);
	}
	else if (1 == GetTabCurSel(136))
	{
		m_strResult.Format(
			BkString::Get(119), 
			m_SoftwareResultList.GetAllSafeCount(), 
			m_SoftwareResultList.GetAllRiskCount(), 
			m_SoftwareResultList.GetAllUnknownCount());

		SetRichText(203, m_strResult);
	}


	return 0;
}

BOOL KMainDlg::Filter(PVOID pvData, int nLevel)
{
	KsafeFileMonStr* pFileNameStr = (KsafeFileMonStr*)pvData;
	if (pFileNameStr == NULL)
		return FALSE;

	RESULT_INFO	Info;
	int nIndex = m_ResultInfoMap.FindKey(CString(pFileNameStr->sProName).MakeLower());
	if (-1 == nIndex)
	{	
		TCHAR* pszFile = ATLPath::FindFileName(pFileNameStr->sProName);
		if (pszFile == NULL)
			Info.strName = pFileNameStr->sProName;
		else
			Info.strName = pszFile;
		Info.bMonitor	= FALSE;
		
		if (!m_Logic.GetFileIcon( pFileNameStr->sProName, Info.hIcon ) )
			Info.hIcon = NULL;

		m_ResultInfoMap.Add(CString(pFileNameStr->sProName).MakeLower(), Info);
	}
	else
	{
		Info.strName = m_ResultInfoMap.GetValueAt(nIndex).strName;
		Info.hIcon = m_ResultInfoMap.GetValueAt(nIndex).hIcon;
		Info.bMonitor = m_ResultInfoMap.GetValueAt(nIndex).bMonitor;
	}


	CString strExt;
	RESULT_INFO	InfoExt;
	TCHAR* pszFileExt = ATLPath::FindExtension(pFileNameStr->sOpenName);
	if (pszFileExt)
		strExt = pszFileExt;
	else
		strExt = pFileNameStr->sOpenName;

	strExt.MakeLower();
	if (strExt == _T(".exe") 
		|| strExt == _T(".lnk"))
		strExt = pFileNameStr->sOpenName;


	int nIndexExt = m_ResultInfoMap.FindKey(strExt.MakeLower());
	if (-1 == nIndexExt)
	{	
		TCHAR* pszFile = ATLPath::FindFileName(pFileNameStr->sOpenName);
		if (pszFile == NULL)
			InfoExt.strName = pFileNameStr->sOpenName;
		else
			InfoExt.strName = pszFile;
		InfoExt.bMonitor	= FALSE;

		if (!m_Logic.GetFileIcon(pFileNameStr->sOpenName, InfoExt.hIcon) )
			InfoExt.hIcon = NULL;

		m_ResultInfoMap.Add(strExt, InfoExt);
	}
	else
	{
		InfoExt.strName = m_ResultInfoMap.GetValueAt(nIndexExt).strName;
		InfoExt.hIcon = m_ResultInfoMap.GetValueAt(nIndexExt).hIcon;
		InfoExt.bMonitor = m_ResultInfoMap.GetValueAt(nIndexExt).bMonitor;
	}


	int nItem = m_ResultList.Append(Info.strName, Info.hIcon, (CListViewCtrlEx::LEVEL)nLevel);
	m_ResultList.AppendSubItem(nItem, pFileNameStr->sOpenName, (DWORD)InfoExt.hIcon, SUBITEM_ICON);

	CTime tNow = CTime::GetCurrentTime();
	CString StrTime;
	StrTime.Format(_T("%4d-%02d-%02d %02d:%02d:%02d"), tNow.GetYear(), tNow.GetMonth(), tNow.GetDay(), tNow.GetHour(), tNow.GetMinute(), tNow.GetSecond());
	m_ResultList.AppendSubItem(nItem, StrTime);

	CString strLevel;
	switch (nLevel)
	{
	case CListViewCtrlEx::enumLevelSafe:
		strLevel = BkString::Get(114);
		break;
	case CListViewCtrlEx::enumLevelRisk:
		strLevel = BkString::Get(115);
		break;
	case CListViewCtrlEx::enumLevelUnknown:
	default:
		strLevel = BkString::Get(139);
		break;
	}
	m_ResultList.AppendSubItem(nItem, strLevel);


	if (Info.bMonitor)
	{
		int nItem = m_SoftwareResultList.Append(Info.strName, Info.hIcon, (CListViewCtrlEx::LEVEL)nLevel);
		m_SoftwareResultList.AppendSubItem(nItem, pFileNameStr->sOpenName, (DWORD)InfoExt.hIcon, SUBITEM_ICON);

		CTime tNow = CTime::GetCurrentTime();
		CString StrTime;
		StrTime.Format(_T("%4d-%02d-%02d %02d:%02d:%02d"), tNow.GetYear(), tNow.GetMonth(), tNow.GetDay(), tNow.GetHour(), tNow.GetMinute(), tNow.GetSecond());
		m_SoftwareResultList.AppendSubItem(nItem, StrTime);
		m_SoftwareResultList.AppendSubItem(nItem, strLevel);
	}


	if (0 == GetTabCurSel(136))
	{
		m_strResult.Format(
			BkString::Get(119), 
			m_ResultList.GetAllSafeCount(), 
			m_ResultList.GetAllRiskCount(), 
			m_ResultList.GetAllUnknownCount());

		SetRichText(203, m_strResult);
	}
	else if (1 == GetTabCurSel(136))
	{
		m_strResult.Format(
			BkString::Get(119), 
			m_SoftwareResultList.GetAllSafeCount(), 
			m_SoftwareResultList.GetAllRiskCount(), 
			m_SoftwareResultList.GetAllUnknownCount());

		SetRichText(203, m_strResult);
	}


	return TRUE;
}

DWORD WINAPI KMainDlg::KucCallBack (
/* [in] */ KucIAllocator*   piAllocator,             // 返回数据使用的分配器
/* [in] */ PVOID            pvData,                  // 数据
/* [in] */ DWORD            dwDataLength,            // 数据长度
/* [in] */ PVOID            pvParam,                 // 回调的自定义参数
/* [in] */ PVOID*           ppvDataRet,              // 返回的数据, 如果不需要返回则ppvDataRet == NULL
/* [in] */ DWORD*           pdwDataLengthRet         // 返回的数据长度, 如果不需要返回则pdwDataLengthRet == NULL
)
{
	KsafeFileMonStr* pFileNameStr = (KsafeFileMonStr*)pvData;
	if (pFileNameStr == NULL)
		return FALSE;

	KMainDlg* pThis = (KMainDlg*)pvParam;
	if (!pThis)
		return FALSE;

	if (!pThis->m_bStart)
		return FALSE;


	if (pThis->m_strCameraDeviceName == pFileNameStr->sOpenName)
	{
		pThis->SendMessage(MSG_UPDATE_CAMERA_INFO, (WPARAM)pFileNameStr->sProName);
		return TRUE;
	}

	if (wcsstr(pFileNameStr->sProName, L"~")) {
		::GetLongPathNameW(pFileNameStr->sProName, pFileNameStr->sProName, MAX_PATH);
	}

	if (wcsstr(pFileNameStr->sOpenName, L"~")) {
		::GetLongPathNameW(pFileNameStr->sOpenName, pFileNameStr->sOpenName, MAX_PATH);
	}


	int nRetCode = pThis->m_RuleImpl.Match(pFileNameStr->sOpenName, pFileNameStr->sProName);
	int nLevel;
	switch (nRetCode)
	{
	case emRuleResult_Show:
	case emRuleResult_Distrustful:
		nLevel = CListViewCtrlEx::enumLevelRisk;
		break;
	case emRuleResult_Unknown:
		nLevel = CListViewCtrlEx::enumLevelUnknown;
		return 0;
		break;
	case emRuleResult_Trust:
		nLevel = CListViewCtrlEx::enumLevelSafe;
		break;
	default:
		return 0;
		break;
	}


	pThis->SendMessage(MSG_UPDATE_RESULT, (WPARAM)pvData, nLevel);
	return 0;
}

LRESULT KMainDlg::OnStart(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	SetItemVisible(1001, TRUE);
	SetItemVisible(1002, FALSE);


	if (m_ResultList.GetItemCount() <= 0)
	{
		m_ResultList.SetEmptyString(BkString::Get(137));
		m_ResultList.Invalidate();
	}

	if (m_SoftwareResultList.GetItemCount() <= 0)
	{
		m_SoftwareResultList.SetEmptyString(BkString::Get(137));
		m_SoftwareResultList.Invalidate();
	}

	SetItemVisible(202, TRUE);
	CString strTime;
	strTime.Format(BkString::Get(146), 0);
	m_strTime.Format(BkString::Get(132), strTime);
	m_strStatus.Format(
		BkString::Get(125), 
		BkString::Get(126), 
		BkString::Get(128), 
		BkString::Get(130),
		m_strTime);
	SetRichText(202, m_strStatus);

	if (2 != GetTabCurSel(136))
	{
		SetItemVisible(203, TRUE);
		m_strResult.Format(BkString::Get(119), 0, 0, 0);
		SetRichText(203, m_strResult);
	}

	SetTimer(MONITOR_TIMER, 1000);
	StartMon();

	return S_OK;
}

BOOL KMainDlg::AddMonitorList()
{
	BOOL bResult = FALSE;
	CString strPath;
	WCHAR   wszFolderPath[MAX_PATH + 1]; 
	if (!::SHGetSpecialFolderPathW(NULL, wszFolderPath, CSIDL_APPDATA, 0)) {return FALSE;}

	WCHAR   wszPath[MAX_PATH + 1]; 
	::GetModuleFileNameW(NULL, wszPath, MAX_PATH);
	::PathStripPathW(wszPath);
	::PathAppendW(wszFolderPath, wszPath);

	KCreateLongDir(wszFolderPath, NULL);
	::PathAppendW(wszFolderPath, L"kprivacy.cfg");
	FILE *fp = _wfopen(wszFolderPath, L"r");
	if (NULL == fp)
	{
		goto Exit0;
	}

	CHAR wszMonitorPath[MAX_PATH + 1] = {0};
	CHAR wszCheck[10] = {0};
	CHAR *pwsz = NULL;

	while(!feof(fp))
	{
		pwsz = fgets(wszMonitorPath, MAX_PATH, fp);
		if (pwsz)
			fgets(wszCheck, 9, fp);

		USES_CONVERSION;
		strPath = A2W_CP(wszMonitorPath, CP_UTF8);
		strPath.Trim();

		if (pwsz && strPath.GetLength() > 2)
		{
			MONITOR_ITEM MonitorItem;
			MonitorItem.bDel = true;
			MonitorItem.nImg = IDB_CUSTOM;
			MonitorItem.bNew = false;
			MonitorItem.strMonitorItem = strPath;
			MonitorItem.strName = ATLPath::FindFileName(MonitorItem.strMonitorItem);
			MonitorItem.strTip = MonitorItem.strMonitorItem;
			MonitorItem.strMonitorItem.MakeLower();
			MonitorItem.bCheck = strnicmp(wszCheck, "0", 1) != 0;
			MonitorItem.bDisable = false;
			

			if (PathIsDirectoryW(strPath))
			{
				HICON hIcon = NULL;
				ExtractIconEx(_T("SHELL32.DLL") , 4, NULL, &hIcon, 1); 
				if (hIcon)
					MonitorItem.nImg = (int)hIcon;

				MonitorItem.nType = enumMonitorItemTypeDir;
				m_MonitorItemlist.Add( MonitorItem );
				if (MonitorItem.bCheck)
				{

					m_RuleImpl.AddPrivacyDir(strPath);

				}

			}
			else
			{
				HICON hIcon = NULL;
				if (m_Logic.GetFileIcon(strPath, hIcon))
					MonitorItem.nImg = (int)hIcon;

				MonitorItem.nType = enumMonitorItemTypeFile;
				m_MonitorItemlist.Add( MonitorItem );
				if (MonitorItem.bCheck)
				{
					m_RuleImpl.AddPrivacyFile(strPath);

				}
			}
		}

	}

	m_RuleImpl.ReloadRule();

	bResult = TRUE;
Exit0:
	if (fp)
	{
		fclose(fp);
	}
	return bResult;
}

BOOL KMainDlg::SaveMonitorList()
{
	BOOL bResult = FALSE;
	WCHAR   wszFolderPath[MAX_PATH + 1]; 
	if (!::SHGetSpecialFolderPathW(NULL, wszFolderPath, CSIDL_APPDATA, 0)) {return FALSE;}

	WCHAR   wszPath[MAX_PATH + 1]; 
	::GetModuleFileNameW(NULL, wszPath, MAX_PATH);
	::PathStripPathW(wszPath);
	::PathAppendW(wszFolderPath, wszPath);

	KCreateLongDir(wszFolderPath, NULL);
	::PathAppendW(wszFolderPath, L"kprivacy.cfg");
	FILE *fp = _wfopen(wszFolderPath, L"w+");
	if (NULL == fp)
	{
		goto Exit0;
	}
	int nItemSize = m_MonitorItemlist.GetItemCount();

	for (int i = 0; i < nItemSize; i++)
	{
		MONITOR_ITEM *pItem = (MONITOR_ITEM *)m_MonitorItemlist.GetItemDataX(i);

		USES_CONVERSION;
		if (pItem && (pItem->nType == enumMonitorItemTypeDir ||  pItem->nType == enumMonitorItemTypeFile))
		{
			fprintf(fp, "%s\n%d\n", W2A_CP((LPCWSTR)pItem->strTip, CP_UTF8), pItem->bCheck);
		}
	}


	{
		KXmlWriter XmlWriter;
		XmlWriter.Open("root");
		for (int i = 0; i < m_MonitorSoftwarelist.GetItemCount(); i++)
		{
			MONITOR_ITEM* pItem = (MONITOR_ITEM *)m_MonitorSoftwarelist.GetItemDataX(i);
			if (!pItem)
				continue;

			if (pItem->nType != enumMonitorItemTypeProcess)
				continue;

			XmlWriter.AddNewElement("software");
			XmlWriter.Write("Name", pItem->strName);
			XmlWriter.Write("Process", pItem->strTip);
			XmlWriter.Write("check", (int)pItem->bCheck);
		}
		XmlWriter.WriteFile(CStringA(m_SaveResultFilePath) + "\\config.dat");
	}


	bResult = TRUE;
Exit0:
	if (fp)
	{
		fclose(fp);
	}
	return bResult;
}

void KMainDlg::AddMonitorSoftwareList()
{
	KXmlReader XmlReader;
	if (!XmlReader.LoadFile(CStringA(m_SaveResultFilePath) + "\\config.dat"))
		return;

	BOOL bRet = XmlReader.Open("root\\software");
	if (!bRet)
		return;

	do 
	{
		MONITOR_ITEM MonitorItem;
		MonitorItem.bCheck = true;
		MonitorItem.bDel = true;
		MonitorItem.bDisable = false;
		MonitorItem.nImg = IDB_CUSTOM;
		MonitorItem.bNew = false;


		XmlReader.Read("Name", MonitorItem.strName);
		XmlReader.Read("Process", MonitorItem.strMonitorItem);
		XmlReader.Read("check", MonitorItem.bCheck);

		HICON hIcon = NULL;
		if (m_Logic.GetFileIcon(MonitorItem.strMonitorItem, hIcon))
			MonitorItem.nImg = (int)hIcon;
		MonitorItem.nType = enumMonitorItemTypeProcess;
		MonitorItem.strTip = MonitorItem.strMonitorItem;
		MonitorItem.strMonitorItem.MakeLower();
		m_MonitorSoftwarelist.Add( MonitorItem );

		if (MonitorItem.bCheck)
			m_RuleImpl.AddMonitorProcess(MonitorItem.strMonitorItem);
		else
			m_RuleImpl.RemoveMonitorProcess(MonitorItem.strMonitorItem);


		RESULT_INFO	Info;
		int nIndex = m_ResultInfoMap.FindKey(MonitorItem.strMonitorItem.MakeLower());
		if (-1 == nIndex)
		{	
			Info.strName	= MonitorItem.strName;
			Info.hIcon		= hIcon;
			Info.bMonitor	= MonitorItem.bCheck ? TRUE : FALSE;
			m_ResultInfoMap.Add(MonitorItem.strMonitorItem, Info);
		}
		else
		{
			m_ResultInfoMap.GetValueAt(nIndex).bMonitor = MonitorItem.bCheck ? TRUE : FALSE;
		}



		bRet = XmlReader.NextSibling();
		if (!bRet) 
			break;

	} while (true);

	m_RuleImpl.ReloadRule();
}

void KMainDlg::InitWhiteSignData()
{
	TCHAR szPath[MAX_PATH] = {0};
	::GetModuleFileName(NULL, szPath, MAX_PATH);
	::PathRemoveFileSpec(szPath);

	CPath strFile = szPath;
	strFile.AddBackslash();
	strFile += _T("rule.dat");


	KXmlReader XmlReader;
	if (!XmlReader.LoadFile(CStringA(strFile)))
		return;

	BOOL bRet = XmlReader.Open("KRuleConfig\\KRules\\KSignDataRule\\SignItem");
	if (!bRet)
		return;

	CString strSign;
	do 
	{
		XmlReader.Read("sign", strSign);

		int nIndex = m_WhiteSignDataMap.FindKey(strSign.MakeLower());
		if (-1 == nIndex)
			m_WhiteSignDataMap.Add(strSign, true);


		bRet = XmlReader.NextSibling();
		if (!bRet) 
			break;

	} while (true);
}

void KMainDlg::OnStop()
{
	SetItemVisible(1001, FALSE);
	SetItemVisible(1002, TRUE);

	if (m_ResultList.GetItemCount() <= 0)
	{
		m_ResultList.SetEmptyString(BkString::Get(138));
		m_ResultList.Invalidate();
	}

	if (m_SoftwareResultList.GetItemCount() <= 0)
	{
		m_SoftwareResultList.SetEmptyString(BkString::Get(138));
		m_SoftwareResultList.Invalidate();
	}

	KillTimer(MONITOR_TIMER);

	m_strStatus.Format(
		BkString::Get(125), 
		BkString::Get(127), 
		BkString::Get(129), 
		BkString::Get(131),
		m_strTime);
	SetRichText(202, m_strStatus);

	//m_KpsRule.StopMon();
	m_ActorMonitorWebCam.Stop();

	m_bStart = false;
}

void KMainDlg::OnContinue()
{
	SetItemVisible(1001, TRUE);
	SetItemVisible(1002, FALSE);


	if (m_ResultList.GetItemCount() <= 0)
	{
		m_ResultList.SetEmptyString(BkString::Get(137));
		m_ResultList.Invalidate();
	}

	if (m_SoftwareResultList.GetItemCount() <= 0)
	{
		m_SoftwareResultList.SetEmptyString(BkString::Get(137));
		m_SoftwareResultList.Invalidate();
	}

	m_strStatus.Format(
		BkString::Get(125), 
		BkString::Get(126), 
		BkString::Get(128), 
		BkString::Get(130),
		m_strTime);
	SetRichText(202, m_strStatus);

	SetTimer(MONITOR_TIMER, 1000);
	StartMon();
}

void KMainDlg::StartMon()
{
	if (!m_bInit)
	{
		KucInit(NULL);
		KReleaseAndSatrtSys();
		int nRet = KucRegisterCallback(
			enumKxERuleMgr              // 通信客户端ID
			, KucCallBack				// 回调函数指针
			, this                      // 给回调函数指定一个自定义参数
			);

		InitWhiteSignData();
	}

	m_ActorMonitorWebCam.Startup(this);

	m_bStart = true;
}

static void TrayIconDelete(HWND hWnd)
{
	NOTIFYICONDATA nid = { 0 };

	nid.cbSize = sizeof (nid);
	nid.hWnd = hWnd;
	nid.uID = TRAY_ICON_ID;

	Shell_NotifyIcon(NIM_DELETE, &nid);
}

static void ShellAppBarMsgDelete(HWND hWnd)
{
	APPBARDATA abd = { 0 };

	abd.cbSize = sizeof(APPBARDATA);  
	abd.hWnd = hWnd;
	abd.uCallbackMessage = WM_SHELLAPPBAR;

	SHAppBarMessage(ABM_REMOVE, &abd);
}

void KMainDlg::EndDialog(UINT uRetCode)
{
	if (m_KClearLoader.GetPlugin())
	{
		int nCanQuit = TRUE;
		m_KClearLoader.GetPlugin()->CanQuitNow(&nCanQuit);
		if (nCanQuit != TRUE)
			return;
	}

	if (m_hTrayMenu)
	{
		TrayIconDelete(m_hWnd);
		ShellAppBarMsgDelete(m_hWnd);
	}

	OnStop();
	SaveMonitorList();

	KucUnInit();
	KucUnregisterCallback(enumKxERuleMgr);
	KDelAndUninstallSvr(L"ksafefilemon");

	_Module.SetActiveWindow(NULL);
	__super::EndDialog(uRetCode);
}

BOOL KMainDlg::OnBkTabMainSelChange(int nTabItemIDOld, int nTabItemIDNew)
{
	BOOL bRet = FALSE;


    switch (nTabItemIDNew)
    {
    case 0:
		if (m_KClearLoader.GetPlugin())
			m_KClearLoader.GetPlugin()->Show(FALSE);

		SetItemVisible(500, TRUE);
		SetItemVisible(501, FALSE);
		SetItemVisible(502, FALSE);

		SetItemText(201, BkString::Get(133));

		SetItemVisible(203, TRUE);
		m_strResult.Format(
			BkString::Get(119), 
			m_ResultList.GetAllSafeCount(), 
			m_ResultList.GetAllRiskCount(), 
			m_ResultList.GetAllUnknownCount());

		SetRichText(203, m_strResult);


		SetItemVisible(107, TRUE);
		SetItemVisible(108, TRUE);

		SetItemVisible(109, TRUE);
		SetItemVisible(110, TRUE);
		SetItemVisible(111, TRUE);
		SetItemVisible(113, FALSE);
		SetItemVisible(114, TRUE);

    	break;

    case 1:
		if (m_KClearLoader.GetPlugin())
			m_KClearLoader.GetPlugin()->Show(FALSE);

		SetItemVisible(500, FALSE);
		SetItemText(201, BkString::Get(134));

		SetItemVisible(107, FALSE);
		SetItemVisible(108, FALSE);


		if (m_MonitorSoftwarelist.GetItemCount() <= 0)
		{
			SetItemVisible(501, FALSE);
			SetItemVisible(502, TRUE);

			SetItemVisible(203, FALSE);

			SetItemVisible(109, FALSE);
			SetItemVisible(110, FALSE);
			SetItemVisible(111, FALSE);
			SetItemVisible(113, FALSE);
			SetItemVisible(114, FALSE);
		}
		else
		{
			SetItemVisible(501, TRUE);
			SetItemVisible(502, FALSE);

			SetItemVisible(203, TRUE);
			m_strResult.Format(
				BkString::Get(119), 
				m_SoftwareResultList.GetAllSafeCount(), 
				m_SoftwareResultList.GetAllRiskCount(), 
				m_SoftwareResultList.GetAllUnknownCount());
			SetRichText(203, m_strResult);

			SetItemVisible(109, TRUE);
			SetItemVisible(110, TRUE);
			SetItemVisible(111, TRUE);
			SetItemVisible(113, TRUE);
			SetItemVisible(114, TRUE);
		}


    	break;

	case 2:
		SetItemVisible(107, FALSE);
		SetItemVisible(108, FALSE);

		SetItemVisible(109, FALSE);
		SetItemVisible(110, FALSE);
		SetItemVisible(111, FALSE);
		SetItemVisible(113, FALSE);
		SetItemVisible(114, FALSE);

		SetItemText(201, BkString::Get(145));
		SetItemVisible(203, FALSE);

		if (m_KClearLoader.GetPlugin())
			m_KClearLoader.GetPlugin()->Show(TRUE);

		break;

	default:
		break;
    }

	
	bRet = TRUE;

Exit0:
	//EmptyWorkingSet(GetCurrentProcess());

    return bRet;
}

void KMainDlg::OnResultListReSize( CRect rcWnd )
{
	int nWidth = rcWnd.Width() 
		- m_ResultList.GetColumnWidth(0) 
		- m_ResultList.GetColumnWidth(2)
		- m_ResultList.GetColumnWidth(3) - 18;
	m_ResultList.SetColumnWidth(1, nWidth);

	nWidth = rcWnd.Width() 
		- m_SoftwareResultList.GetColumnWidth(0) 
		- m_SoftwareResultList.GetColumnWidth(2)
		- m_SoftwareResultList.GetColumnWidth(3) - 18;
	m_SoftwareResultList.SetColumnWidth(1, nWidth);
}

void KMainDlg::OnKClearReSize( CRect rcWnd )
{
	if (m_KClearLoader.GetPlugin())
		m_KClearLoader.GetPlugin()->Move(rcWnd);
}

void KMainDlg::OnSysCommand(UINT nID, CPoint point)
{
	SetMsgHandled(FALSE);

	switch (nID & 0xFFF0)
	{
	case SC_CLOSE:
		SetMsgHandled(TRUE);
		OnBtnClose();
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

//设置
void KMainDlg::OnSetting()
{
}

//立即升级
void KMainDlg::OnUpdate()
{
}

void KMainDlg::OnActivate(KActor* pActor)
{
	if (m_ActorInit.IsActive())
	{
		SendMessage(MSG_START);
		EmptyWorkingSet(GetCurrentProcess());
	}
	else if (m_ActorCleaner.IsActive())
	{
		if (m_ActorCleaner.WaitForStop(180000))
			return;

		EmptyWorkingSet(GetCurrentProcess());
	}
	else if (m_ActorMonitorWebCam.IsActive())
	{
		std::wstring strDeviceName = _T("");
		std::wstring strDeviceDesc = _T("");


		int nErrCount = 0;
		while (TRUE)
		{
			if (m_ActorMonitorWebCam.WaitForStop(1))
				return;

			if (!m_bCamera || !m_bInit)
				goto Continue;


			if (!GetPhysicalDeviceObjectNameBySearchCarmera(strDeviceName, strDeviceDesc, m_ActorMonitorWebCam))
			{
				m_strCameraDeviceName = _T("");
				goto Continue;
			}

			if (strDeviceName.empty())
				goto Continue;

			if (m_strCameraDeviceName != strDeviceName.c_str())
			{
				m_strCameraDeviceName = strDeviceName.c_str();
			}
			else
				goto Continue;


			if (!KucSetVideoDevName(m_strCameraDeviceName))
			{
				m_strCameraDeviceName = _T("");
				nErrCount++;
				if (nErrCount >= 3)
				{
					SendMessage(MSG_CAMERA_REBOOT);
					return;
				}
			}

		Continue:
			if (m_ActorMonitorWebCam.WaitForStop(3000))
				return;
		}

	}
}

void KMainDlg::OnCheck(DWORD nId, bool bCheck)
{
	if (0 == GetTabCurSel(136))
	{
		MONITOR_ITEM* pItem = m_MonitorItemlist.GetItemDataX(nId);
		if (!pItem)
			return;

		switch (pItem->nType )
		{
		case enumMonitorItemTypeFile:
			if (bCheck)
				m_RuleImpl.AddPrivacyFile(pItem->strMonitorItem);
			else
				m_RuleImpl.RemovePrivacyFile(pItem->strMonitorItem);
			m_RuleImpl.ReloadRule();
			break;
		case enumMonitorItemTypeDir:
			if (bCheck)
				m_RuleImpl.AddPrivacyDir(pItem->strMonitorItem);
			else
				m_RuleImpl.RemovePrivacyDir(pItem->strMonitorItem);
			m_RuleImpl.ReloadRule();
			break;
		case enumMonitorItemTypeMyComputer:
			{
				UINT	uRet		= 0;
				TCHAR	szPath[]	= _T("A:");


				for (TCHAR driver = _T('A'); driver <= _T('Z'); driver++)
				{
					szPath[0] = driver;
					uRet = GetDriveType(szPath);

					if (DRIVE_UNKNOWN == uRet
						|| DRIVE_NO_ROOT_DIR == uRet
						|| DRIVE_REMOTE == uRet
						|| DRIVE_CDROM == uRet)
					{
						continue;
					}

					if (bCheck)
						m_RuleImpl.AddPrivacyDir(szPath);
					else
						m_RuleImpl.RemovePrivacyDir(szPath);
				}

				m_RuleImpl.ReloadRule();
			}
			break;
		case enumMonitorItemTypeMyCamera:
			{
				m_bCamera = bCheck;
			}
			break;
		}
	}
	else
	{
		MONITOR_ITEM* pItem = m_MonitorSoftwarelist.GetItemDataX(nId);
		if (!pItem)
			return;

		if (pItem->nType != enumMonitorItemTypeProcess)
			return;
		
		RESULT_INFO	ResultInfo;
		int nIndex = m_ResultInfoMap.FindKey(CString(pItem->strMonitorItem).MakeLower());
		if (-1 != nIndex)
		{	
			m_ResultInfoMap.GetValueAt(nIndex).bMonitor = bCheck ? TRUE : FALSE;
		}

		if (bCheck)
			m_RuleImpl.AddMonitorProcess(pItem->strMonitorItem);
		else
			m_RuleImpl.RemoveMonitorProcess(pItem->strMonitorItem);
		m_RuleImpl.ReloadRule();
	}

}

void KMainDlg::OnDelete(DWORD nId)
{
	if (0 == GetTabCurSel(136))
	{
		MONITOR_ITEM* pItem = m_MonitorItemlist.GetItemDataX(nId);
		if (!pItem)
			return;

		switch (pItem->nType )
		{
		case enumMonitorItemTypeFile:
			m_RuleImpl.RemovePrivacyFile(pItem->strMonitorItem);
			m_RuleImpl.ReloadRule();
			break;
		case enumMonitorItemTypeDir:
			m_RuleImpl.RemovePrivacyDir(pItem->strMonitorItem);
			m_RuleImpl.ReloadRule();
			break;
		}

		m_MonitorItemlist.DeleteItem(nId);
	}
	else
	{
		MONITOR_ITEM* pItem = m_MonitorSoftwarelist.GetItemDataX(nId);
		if (!pItem)
			return;

		switch (pItem->nType )
		{
		case enumMonitorItemTypeProcess:
			RESULT_INFO	ResultInfo;
			int nIndex = m_ResultInfoMap.FindKey(CString(pItem->strMonitorItem).MakeLower());
			if (-1 != nIndex && m_ResultInfoMap.GetValueAt(nIndex).bMonitor)
			{	
				m_ResultInfoMap.GetValueAt(nIndex).bMonitor = FALSE;
			}

			m_RuleImpl.RemoveMonitorProcess(pItem->strMonitorItem);
			m_RuleImpl.ReloadRule();
			break;
		}

		m_MonitorSoftwarelist.DeleteItem(nId);


		if (m_MonitorSoftwarelist.GetItemCount() <= 0)
		{
			SetItemVisible(501, FALSE);
			SetItemVisible(502, TRUE);

			SetItemVisible(203, FALSE);

			SetItemVisible(109, FALSE);
			SetItemVisible(110, FALSE);
			SetItemVisible(111, FALSE);
			SetItemVisible(113, FALSE);
			SetItemVisible(114, FALSE);
		}
	}

}

void KMainDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == MONITOR_TIMER)
	{
		m_dwTime++;
		CString strTime;
		if (m_dwTime <= 60)
		{
			strTime.Format(BkString::Get(146), m_dwTime);
		}
		else if (m_dwTime < 3600)
		{
			strTime.Format(BkString::Get(147), m_dwTime / 60, m_dwTime % 60);
		}
		else if (m_dwTime < 3600 * 24)
		{
			strTime.Format(BkString::Get(148), m_dwTime / 3600, m_dwTime % 3600 / 60);
		}

		m_strTime.Format(BkString::Get(132), strTime);
		m_strStatus.Format(
			BkString::Get(125), 
			m_bStart ? BkString::Get(126) : BkString::Get(127), 
			m_bStart ? BkString::Get(128) : BkString::Get(129), 
			m_bStart ? BkString::Get(130) : BkString::Get(131),
			m_strTime);

		SetRichText(202, m_strStatus);
	}
	else if (nIDEvent == TIP_TIMER)
	{
		// 控制Balloon tip持续时间，暂无法实现
		//SetTipInfo(BkString::Get(144), BkString::Get(1), 2);
		//KillTimer(TIP_TIMER);
	}
}

LRESULT KMainDlg::OnTrayIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	long nNotifyEvent = LOWORD(lParam);
	long nTrayId = HIWORD(lParam);

	if ( nNotifyEvent == WM_RBUTTONUP )
	{
		POINT pt;
		GetCursorPos(&pt);

		SetForegroundWindow(m_hWnd);
		TrackPopupMenu(m_hTrayMenu,
			TPM_RIGHTBUTTON,
			pt.x,
			pt.y,
			0,
			m_hWnd,
			NULL
			);
		PostMessage(WM_NULL, 0, 0);
	}
	else if ( nNotifyEvent == WM_LBUTTONUP )
	{
		Showup();
	}
	else if ( nNotifyEvent == WM_LBUTTONDBLCLK )
	{
	}

	bHandled = TRUE;
	return 0;
}

void KMainDlg::Showup()
{
	if (IsIconic())
		SendMessage(WM_SYSCOMMAND, SC_RESTORE | HTCAPTION, 0);

	SetWindowPos(HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
	::SetForegroundWindow(m_hWnd);
	SetFocus();
}

LRESULT KMainDlg::OnMainApp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	Showup();
	return 0;
}

LRESULT KMainDlg::OnSOS(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ShellExecute(NULL, _T("open"), BkString::Get(120), NULL, NULL, SW_SHOW);
	return 0;
}

LRESULT KMainDlg::OnExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static BOOL bOn = FALSE;

	if ( !bOn )
	{
		bOn = TRUE;

		EndDialog(IDCANCEL);
	}

	return 0;
}

LRESULT KMainDlg::OnUninstall(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	EndDialog(IDCANCEL);

	return S_OK;
}

LRESULT KMainDlg::OnCameraReboot(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	KWarningDlg kWarn(2, L"对摄像头的监控将在重启系统后生效！", -34);
	kWarn.DoModal();

	return S_OK;
}
