#pragma once

#include "resource.h"
#include <bkres/bkres.h>
#include <atlfile.h>
#include <bkwin/bkatldefine.h>
#include "common/bkcmdline.h"




class CBKSafeVulModule
    : public CAppModule
{
public:
    CBKSafeVulModule()
		: m_hModRichEdit2(NULL)
    {
    }

    HRESULT				Init(HINSTANCE hInstance);
    UINT_PTR			RunMain();
    BOOL				CheckInstance();
    void				SetActiveWindow(HWND hWndActive);
	VOID				InitRichEdit2();

	LPCTSTR				GetAppFilePath();
	LPCTSTR				GetAppDirPath();
	BOOL				CheckIntegrityLevel();

protected:
	struct BkSafe_Instance_Data
	{
		HWND hActiveWnd;
	};	
    CString				m_strAppFileName;
    CString				m_strAppPath;
	HMODULE				m_hModRichEdit2;
	BkWinManager		m_BkWndMagnager;
	CAtlFileMapping<BkSafe_Instance_Data> m_mapping;
	CBkCmdLine _CmdLine;

	void				_InitUIResource();
	BOOL				_IsNeedAdministratorCall();
};

extern CBKSafeVulModule _Module;
