// beikesafe.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "beikesafe.h"
#include "beikesafemaindlg.h"
#include <runoptimize/interface.h>

CBeikeSafeModule _Module;

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

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
#   ifdef _DEBUG

	//bkconsole theConsole;

#   endif

	HRESULT hRet = E_FAIL;

	hRet = _Module.Init(hInstance);
	if (FAILED(hRet))
		return -1;

	if (FALSE == _Module.CheckIntegrityLevel())
		return -2;


	::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	_Module.InitRichEdit2();
	_Module.RunMain();

	::CoUninitialize();

	return 0;
}

HRESULT CBeikeSafeModule::Init(HINSTANCE hInstance)
{
	HRESULT hRet = __super::Init(NULL, hInstance);
	if (FAILED(hRet))
		return hRet;

	//get app file fullpath
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

	//register install path
	CRegKey reg;
	reg.Create(HKEY_LOCAL_MACHINE, L"SOFTWARE\\KSafe");
	reg.Create(HKEY_LOCAL_MACHINE, L"SOFTWARE\\KSafe\\ksafevul");
	WCHAR szTemp[2];

	dwRet = 1;
	LONG lRet = reg.QueryStringValue(L"Install Path", szTemp, &dwRet);
	if (ERROR_FILE_NOT_FOUND == lRet)
	{
		reg.SetStringValue(L"Install Path", m_strAppPath);
	}

	return hRet;
}

UINT_PTR CBeikeSafeModule::RunMain()
{
	if ( TRUE )
	{
		CString strPath;

		GetModuleFileName((HMODULE)&__ImageBase, strPath.GetBuffer(MAX_PATH + 10), MAX_PATH);
		strPath.ReleaseBuffer();
		strPath.Truncate(strPath.ReverseFind(L'\\') + 1);
		strPath += L"res\\ksafe";

		if ( PathFileExists(strPath) )
			BkResManager::SetResourcePath(strPath);
	}

	//only one
	if (!CheckInstance())
	{
		return 0;
	}

	UINT_PTR uRet = 0;
	_InitUIResource();
	CBeikeSafeMainDlg dlgMain;
	uRet = dlgMain.DoModal(NULL);

	return uRet;
}

BOOL CBeikeSafeModule::_IsNeedAdministratorCall()
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

#define BKSAFE_SYSOPT_DDE_WND_CLASS L"{10FAEEA8-67C9-4f46-823A-AF3C72F53DB5}"

BOOL CBeikeSafeModule::CheckInstance()
{
	BOOL bRet = FALSE;
	HRESULT hRet = E_FAIL;
	BOOL bAlreadyExisted = FALSE;

	if (NULL != m_mapping.GetHandle())
		return TRUE;

	hRet = m_mapping.MapSharedMem(sizeof(BkSafe_Instance_Data), BKSAFE_SYSOPT_DDE_WND_CLASS, &bAlreadyExisted);
	if (FAILED(hRet))
		return bRet;

	if (bAlreadyExisted)
	{
		HWND hWndActive = ((BkSafe_Instance_Data *)m_mapping)->hActiveWnd;

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

LPCTSTR CBeikeSafeModule::GetAppFilePath()
{
	return m_strAppFileName;
}

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
	BOOL bRet = TRUE;
	if (FALSE == _IsNeedAdministratorCall())
		return bRet;

	bRet = _CmdLine.Execute(m_strAppFileName, TRUE, FALSE);

	if (!bRet)
	{
		::MessageBox(0, L"请使用管理员账户运行本软件", 0, 0);
	}

	return FALSE;
}

void CBeikeSafeModule::_InitUIResource()
{
    BkString::Load(IDR_BK_STRING_DEF);
    BkFontPool::SetDefaultFont(BkString::Get(IDS_APP_FONT), -12);
    BkSkin::LoadSkins(IDR_BK_SKIN_DEF);
    BkStyle::LoadStyles(IDR_BK_STYLE_DEF);
}