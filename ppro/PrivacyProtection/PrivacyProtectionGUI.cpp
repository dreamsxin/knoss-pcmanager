// PrivacyProtectionGUI.cpp : main source file for PrivacyProtectionGUI.exe
//
#include "stdafx.h"
#include "resource.h"
#include <bkres/bkres.h>
#include "PrivacyProtectionGUI.h"
#include "kmaindlg.h"
#include "KCommandLine.h"
#include "KRuleTestImpl.h"

KAppModule _Module;


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	//OutputDebugString(_T("_tWinMain Start."));

	if (FAILED(_Module.Init(hInstance)))
		return -1;


	::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	_Module.Run();
	::CoUninitialize();

	_Module.Uninit();

	//OutputDebugString(_T("_tWinMain End."));

	return 0;
}

HRESULT KAppModule::Init(HINSTANCE hInstance)
{
	KLogic Logic;
	Logic.Startup();


	HRESULT hRet = __super::Init(NULL, hInstance);
	if (FAILED(hRet))
		return hRet;


	return hRet;
}

void KAppModule::Uninit()
{
}

HRESULT KAppModule::Run()
{
	// 判断前一实例
	
	if (ProcessCommandLine())
		return 0;

	if (!CheckInstance())
		return 0;

	if (m_bQuit)
		return 0;

	_InitUIResource();
	if ( m_hModRichEdit2 == NULL )
		m_hModRichEdit2 = ::LoadLibrary(_T("RICHED20.DLL"));

	
	KMainDlg dlgMain;
	return dlgMain.DoModal(NULL);
}

BOOL KAppModule::CheckInstance()
{
	BOOL bRet = FALSE;
	HRESULT hRet = E_FAIL;
	BOOL bAlreadyExisted = FALSE;

	if (NULL != m_mapping.GetHandle())
		return TRUE;

	hRet = m_mapping.MapSharedMem(sizeof(K_Instance_Data), L"{3DBD768C-EBBC-4bbf-82B1-F8758A381B7C}", &bAlreadyExisted);
	if (FAILED(hRet))
		return bRet;

	if (bAlreadyExisted)
	{
		HWND hWndActive = ((K_Instance_Data *)m_mapping)->hActiveWnd;
		if (!::IsWindow(hWndActive))
			goto Exit1;

		if (m_bQuit)
		{
			::SendMessage(hWndActive, MSG_UNINSTALL, 0, 0);
		}
		else
		{
			if (::IsIconic(hWndActive))
				::SendMessage(hWndActive, WM_SYSCOMMAND, SC_RESTORE | HTCAPTION, 0);

			::SetWindowPos(hWndActive, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
			::SetForegroundWindow(hWndActive);
			::SetFocus(hWndActive);
		}

		goto Exit0;
	}


Exit1:
	bRet = TRUE;

Exit0:

	return bRet;
}

void KAppModule::SetActiveWindow(HWND hWndActive)
{
	K_Instance_Data *pInstance = (K_Instance_Data *)m_mapping;

	if (pInstance)
		pInstance->hActiveWnd = hWndActive;

	if (NULL == hWndActive)
		m_mapping.Unmap();
}

BOOL KAppModule::ProcessCommandLine(void)
{
	KCommandLine cmdLine;

	std::vector<CString> vecParamQuit = cmdLine.GetCommandParam(_T("Quit"));
	if (!vecParamQuit.empty()) 
	{
		m_bQuit = true;
		return FALSE;
	}

	std::vector<CString> vecParam = cmdLine.GetCommandParam(_T("debug"));
	if (vecParam.empty()) 
		return FALSE;

	DPrintA("ProcessCommandLine start\n");
	for (size_t i = 0; i < vecParam.size(); i++)
	{
		if (_taccess(vecParam[i], 0) == 0)
		{
			BOOL bRetCode = 0;
			KRuleTestImpl testImpl;

			bRetCode = testImpl.Init(vecParam[i]);
			if (!bRetCode)
			{
				DPrintA("KRuleTestImpl::Init fail, file:%ws\n", vecParam[i]);
				continue;
			}
			else
			{
				DPrintA("file:%ws, result:\n", vecParam[i]);
				testImpl.DebugMatch();
				DPrintA("\n\n");
			}
			
		}
		else
		{
			DPrintW(L"cann't access file:%s\n", vecParam[i]);
		}
	}
	DPrintA("ProcessCommandLine end\n");
	return TRUE;
}

void KAppModule::_InitUIResource()
{
	BkFontPool::SetDefaultFont(_T("宋体"), -12);

	BkSkin::LoadSkins(IDR_SKIN_DEF);
	BkStyle::LoadStyles(IDR_STYLE_DEF);
	BkString::Load(IDR_STRING_DEF);
}
