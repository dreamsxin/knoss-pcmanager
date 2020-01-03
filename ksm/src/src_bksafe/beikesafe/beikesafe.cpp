// beikesafe.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "beikesafe.h"
#include "beikesafemaindlg.h"
CBeikeSafeModule _Module;

DWORD g_dwMainThreadId = 0;
#define BKSAFE_DDE_WND_CLASS			L"{CB84AA20-E189-408a-B647-5D406934E1E0}"


typedef struct _MY_TOKEN_MANDATORY_LABEL {
	SID_AND_ATTRIBUTES Label;
} MY_TOKEN_MANDATORY_LABEL, *PMY_TOKEN_MANDATORY_LABEL;


typedef struct _MY_TOKEN_LINKED_TOKEN {
	HANDLE LinkedToken;
} MY_TOKEN_LINKED_TOKEN, *PMY_TOKEN_LINKED_TOKEN;


typedef enum _MY_TOKEN_INFORMATION_CLASS {
	/*
	TokenUser = 1,
	TokenGroups,
	TokenPrivileges,
	TokenOwner,
	TokenPrimaryGroup,
	TokenDefaultDacl,
	TokenSource,
	TokenType,
	TokenImpersonationLevel,
	TokenStatistics,

	TokenRestrictedSids,
	TokenSessionId,
	TokenGroupsAndPrivileges,
	TokenSessionReference,
	TokenSandBoxInert,
	TokenAuditPolicy,
	TokenOrigin,*/
	MYTokenElevationType  = 18, // MaxTokenInfoClass
	MYTokenLinkedToken,
	MYTokenElevation,

	MYTokenHasRestrictions,
	MYTokenAccessInformation,
	MYTokenVirtualizationAllowed,
	MYTokenVirtualizationEnabled,
	MYTokenIntegrityLevel,
	MYTokenUIAccess,
	MYTokenMandatoryPolicy,
	MYTokenLogonSid,
	//MaxTokenInfoClass  // MaxTokenInfoClass should always be the last enum
} MY_TOKEN_INFORMATION_CLASS, *PMY_TOKEN_INFORMATION_CLASS;

#ifndef SECURITY_MANDATORY_HIGH_RID
#define SECURITY_MANDATORY_HIGH_RID                 (0x00003000L)
#endif 

#ifndef SE_GROUP_INTEGRITY
#define SE_GROUP_INTEGRITY                 (0x00000020L)
#endif

BOOL NeedServiceCall()
{
    BOOL bRet = FALSE, bNeedSvcCall = FALSE;
    HANDLE hToken = NULL;
    DWORD dwBytesNeeded = 0;
    MY_TOKEN_MANDATORY_LABEL *pTIL = NULL;

    if (!RunTimeHelper::IsVista())
        goto Exit0;

    bRet = ::OpenProcessToken(::GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);
    if (NULL == hToken)
        return FALSE;

    bRet = ::GetTokenInformation(
        hToken,
        (TOKEN_INFORMATION_CLASS)MYTokenIntegrityLevel,
        NULL, 0, &dwBytesNeeded);

    pTIL = (MY_TOKEN_MANDATORY_LABEL *)new BYTE[dwBytesNeeded];
    if (!pTIL)
        goto Exit0;

    bRet = ::GetTokenInformation(
        hToken,
        (TOKEN_INFORMATION_CLASS)MYTokenIntegrityLevel,
        pTIL, dwBytesNeeded, &dwBytesNeeded);
    if (!bRet || !pTIL)
        goto Exit0;

    SID* pSid = static_cast<SID*>(pTIL->Label.Sid);
    if (!pSid)
        goto Exit0;

    if (SECURITY_MANDATORY_HIGH_RID > pSid->SubAuthority[0])
        bNeedSvcCall = TRUE;

Exit0:

    if (NULL != pTIL)
    {
        delete[] (LPBYTE)pTIL;
        pTIL = NULL;
    }

    if (hToken)
    {
        ::CloseHandle(hToken);
        hToken = NULL;
    }

    return bNeedSvcCall;
}

void AllowDropFile()
{
    typedef BOOL (WINAPI *FN_ChangeWindowMessageFilter)(UINT, DWORD);

    HMODULE hModUser = ::GetModuleHandle(L"user32.dll");

    FN_ChangeWindowMessageFilter pfnChangeWindowMessageFilter = (FN_ChangeWindowMessageFilter)::GetProcAddress(hModUser, "ChangeWindowMessageFilter");

    if (pfnChangeWindowMessageFilter)
    {
        pfnChangeWindowMessageFilter(0x0049/*WM_COPYGLOBALDATA*/, 1/*MSGFLT_ADD*/);
        pfnChangeWindowMessageFilter(WM_DROPFILES, 1);
    }
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
#   ifdef _DEBUG

    //bkconsole theConsole;

#   endif

    HRESULT hRet = E_FAIL;

	g_dwMainThreadId = ::GetCurrentThreadId();

    
    hRet = _Module.Init(hInstance);
    if (FAILED(hRet))
        return -1;

    hRet = _Module.ParseCommandLine(lpstrCmdLine);
    if (FAILED(hRet))
        return -2;

    if (!_Module.CheckIntegrityLevel())
        return -3;

    if (hRet == S_FALSE)
        return 0;

	AtlInitCommonControls( ICC_STANDARD_CLASSES | ICC_BAR_CLASSES | ICC_WIN95_CLASSES );
    ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	_Module.InitRichEdit2();
    _Module.Main();

    ::CoUninitialize();



    return 0;
}

HRESULT CBeikeSafeModule::Init(HINSTANCE hInstance)
{
    HRESULT hRet = __super::Init(NULL, hInstance);
    if (FAILED(hRet))
        return hRet;

    DWORD dwRet = ::GetModuleFileName(NULL, m_strAppFileName.GetBuffer(MAX_PATH + 1), MAX_PATH);
    if (0 == dwRet)
    {
        m_strAppFileName.ReleaseBuffer(0);

        hRet = E_FAIL;
    }
    else
    {
        m_strAppFileName.ReleaseBuffer();

        m_strAppPath = m_strAppFileName;
        m_strAppPath.Truncate(m_strAppPath.ReverseFind(L'\\') + 1);
    }

    CString strVerDllFileName = m_strAppPath + L"ksafever.dll";

    _GetPEProductVersion(strVerDllFileName, m_strAppVersion);

   return hRet;
}

HRESULT CBeikeSafeModule::ParseCommandLine(LPCWSTR lpszCmdLine)
{
    HRESULT hRet = S_OK;
 
     BOOL bRet = _CmdLine.Analyze(lpszCmdLine);
     if (!bRet)
         return E_FAIL;

    return hRet;
}

UINT_PTR CBeikeSafeModule::Main()
{
    BOOL bNavigate = FALSE;
    CString strNavigate;

	/*主程序初始化最开始注册Dump*/
	//KDump::Instance().Install();

    if ( TRUE /*_CmdLine.HasParam(L"{69DD4969-E6C4-42d9-A508-105DDA13CE40}")*/)
    {
        CString strPath;

        GetModuleFileName((HMODULE)&__ImageBase, strPath.GetBuffer(MAX_PATH + 10), MAX_PATH);
        strPath.ReleaseBuffer();
        strPath.Truncate(strPath.ReverseFind(L'\\') + 1);
        strPath += L"res\\ksafe";

		if ( PathFileExists(strPath) )
			BkResManager::SetResourcePath(strPath);
    }

	

    // 判断前一实例
    if (!CheckInstance())
    {
        return 0;
    }

    UINT_PTR uRet = 0;

    if (m_bShowMainUI)
    {
        _InitUIResource();

        AllowDropFile();
// 
 		CBeikeSafeMainDlg dlgMain;
		uRet = dlgMain.DoModal(NULL);

        m_pNavigator = NULL;
    }

    return uRet;
}



BOOL CBeikeSafeModule::CheckInstance()
{
    BOOL bRet = FALSE;
    HRESULT hRet = E_FAIL;
    BOOL bAlreadyExisted = FALSE;

    if (NULL != m_mapping.GetHandle())
        return TRUE;

    hRet = m_mapping.MapSharedMem(sizeof(BkSafe_Instance_Data), BKSAFE_DDE_WND_CLASS, &bAlreadyExisted);
    if (FAILED(hRet))
        return bRet;

    if (bAlreadyExisted)
    {
        HWND hWndActive = ((BkSafe_Instance_Data *)m_mapping)->hActiveWnd;
		
		if( IsWindowRunBackGround(hWndActive) )
		{
			SetWindowRunBackGround(hWndActive, FALSE);
		}

        if (::IsIconic(hWndActive))
            ::SendMessage(hWndActive, WM_SYSCOMMAND, SC_RESTORE | HTCAPTION, 0);
				
        ::SetWindowPos(hWndActive, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
        ::SetForegroundWindow(hWndActive);
        ::SetFocus(hWndActive);

        goto Exit0;
    }

    bRet = TRUE;

Exit0:

    return bRet;
}

void CBeikeSafeModule::SetActiveWindow(HWND hWndActive)
{
    BkSafe_Instance_Data *pInstance = (BkSafe_Instance_Data *)m_mapping;

    if (pInstance)
        pInstance->hActiveWnd = hWndActive;
}

void CBeikeSafeModule::_GetFileInfo(BYTE *pbyInfo, LPCWSTR lpszFileInfoKey, LPWSTR lpszFileInfoRet)
{
    BOOL bRet = FALSE;
    UINT uCount = 0;
    UINT uRetSize = MAX_PATH - 1;
    LPWSTR lpszValue = NULL;

    struct LANGANDCODEPAGE
    {
        WORD wLanguage;
        WORD wCodePage;
    } *lpTranslate;

    bRet = ::VerQueryValue(pbyInfo, L"\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &uCount);
    if (!bRet || uCount == 0)
        return;

    CString strSubBlock;

    strSubBlock.Format(
        L"\\StringFileInfo\\%04x%04x\\%s",
        lpTranslate[0].wLanguage,
        lpTranslate[0].wCodePage, 
        lpszFileInfoKey
        );
    bRet = ::VerQueryValue(
        pbyInfo, (LPWSTR)(LPCWSTR)strSubBlock, 
        (LPVOID *)&lpszValue, &uRetSize
        );
    if (!bRet)
        return;

    wcsncpy(lpszFileInfoRet, lpszValue, wcslen(lpszValue) + 1);
}

BOOL CBeikeSafeModule::_GetPEProductVersion(LPCWSTR lpszFileName, CString &strProductVersion)
{
    BOOL bResult = FALSE;
    BOOL bRet = FALSE;
    DWORD dwHandle          = 0;
    DWORD dwFileInfoSize    = 0;
    VS_FIXEDFILEINFO *pFixFileInfo = NULL;
    BYTE *pbyInfo           = NULL;

    dwFileInfoSize = ::GetFileVersionInfoSize(lpszFileName, &dwHandle);
    if (0 == dwFileInfoSize)
        goto Exit0;

    pbyInfo = new BYTE[dwFileInfoSize];
    if (!pbyInfo)
        goto Exit0;

    bRet = ::GetFileVersionInfo(lpszFileName, dwHandle, dwFileInfoSize, pbyInfo);
    if (!bRet)
        goto Exit0;

    _GetFileInfo(pbyInfo, L"ProductVersion", strProductVersion.GetBuffer(MAX_PATH + 1));
    strProductVersion.ReleaseBuffer();

    bResult = TRUE;

Exit0:

    if (pbyInfo)
    {
        delete[] pbyInfo;
        pbyInfo = NULL;
    }

    return bResult;
}


// 
LPCTSTR CBeikeSafeModule::GetAppDirPath()
{
	return m_strAppPath;
}


VOID CBeikeSafeModule::InitRichEdit2()
{
	if ( m_hModRichEdit2 == NULL )
		m_hModRichEdit2 = ::LoadLibrary(_T("RICHED20.DLL"));
}

BOOL CBeikeSafeModule::CheckIntegrityLevel()
{
    BOOL bNeedSvcCall = NeedServiceCall();
    CString strCmdLine;

    _CmdLine.GetCmdLine(strCmdLine);

    if (!bNeedSvcCall)
        return TRUE;


    BOOL bRet = _CmdLine.Execute(m_strAppFileName, TRUE, FALSE);

    if (!bRet)
        ::MessageBox(0, L"请使用管理员账户运行本软件", 0, 0);

    return FALSE;
}

void CBeikeSafeModule::_InitUIResource()
{


    BkString::Load(IDR_BK_STRING_DEF);

    BkFontPool::SetDefaultFont(BkString::Get(IDS_APP_FONT), -12);

    BkSkin::LoadSkins(IDR_BK_SKIN_DEF);
    BkStyle::LoadStyles(IDR_BK_STYLE_DEF);
}



