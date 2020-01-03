#pragma once

#include "resource.h"
#include <bkres/bkres.h>
#include <atlfile.h>
#include <bkwin/bkatldefine.h>
//#include "_idl_gen/bksafesvc.h"
#include <miniutil/bkcmdline.h>
//#include <bksafe/bksafenavigatestring.h>
//#include "bksafeexamcall.h"


#define BKSAFE_INSTANCE_OBJECT_NAME                L"{3BE816FD-E326-4f9d-A5E5-4F2A5201EAC6}"
//{E855FBFA-BE48-4487-8354-14E99D8754C7}

// 直接调用某功能的命令行参数
// #define CMDPARAM_NAME_NAVIGATE                     L"do"
// #define CMDPARAM_NAME_SETUCACHE					   L"setupcache"
// #define CMDPARAM_NAME_INSTALL_LOCAL_ENGINE         L"installlocalengine"
// #define CMDPARAM_NAME_UNINSTALL					   L"uninstall"	
// #define CMDPARAM_NAME_SAFEMONITOR				   L"safemonitor"
// #define CMDPARAM_NAME_RESTART					   L"restart"

class CBkNavigator
{
public:
	CBkNavigator()
	{
		m_strNavigate.Empty();
	}
	CString		GetNavigateStr(){return m_strNavigate;}
	

    BOOL Navigate(LPCWSTR lpszNavigateString)
    {
		m_strNavigate = lpszNavigateString;
        CString strNavigate = lpszNavigateString;

        return Navigate(strNavigate);
    }

    BOOL Navigate(CString &strNavigateString)
    {
        CString strChild;
        int nSpliter = strNavigateString.Find(L'_');

        if (-1 == nSpliter)
        { 
            strChild = strNavigateString;
            strNavigateString.Empty();
        }
        else
        {
            strChild = strNavigateString.Left(nSpliter);
            strNavigateString = strNavigateString.Mid(nSpliter + 1);
        }

        CBkNavigator *pChild = OnNavigate(strChild);

        if (pChild && !strNavigateString.IsEmpty())
        {
            return pChild->Navigate(strNavigateString);
        }

        return FALSE;
    }

    virtual CBkNavigator* OnNavigate(CString &strChildName)
    {
        return NULL;
    }
private:
	CString		m_strNavigate;
};

class CBeikeSafeUrlNavigator
    : public CBkNavigator
{
public:
    CBkNavigator* OnNavigate(CString &strChildName)
    {
        ::ShellExecute(NULL, L"open", strChildName, NULL, NULL, SW_SHOWNORMAL);

        return NULL;
    }
protected:
private:
};

class CBeikeSafeModule
    : public CAppModule
    , public CBkNavigator
{
public:
    CBeikeSafeModule()
        : m_pNavigator(NULL)
        , m_bShowMainUI(TRUE)
		, m_hModRichEdit2(NULL)
    {
    }

    HRESULT Init(HINSTANCE hInstance);
    HRESULT ParseCommandLine(LPCWSTR lpszCmdLine);
    UINT_PTR Main();
    BOOL CheckInstance();
    void SetActiveWindow(HWND hWndActive);
	VOID InitRichEdit2();

    BOOL CheckIntegrityLevel();

    //CBkNavigator* OnNavigate(CString &strChildName);

// 	LPCTSTR	GetAppFilePath();
 	LPCTSTR GetAppDirPath();
//     LPCTSTR GetProductVersion();

    //CBkSafeExamCaller Exam;

protected:

    CBkNavigator *m_pNavigator;

    CString m_strAppFileName;
    CString m_strAppPath;
    CString m_strAppVersion;
    LONG    m_lAppVersionNumber;

    CBkCmdLine _CmdLine;
	HMODULE	m_hModRichEdit2;

    BOOL m_bShowMainUI;

    CBeikeSafeUrlNavigator m_UrlNavigator;

    BkWinManager m_BkWndMagnager;

    struct BkSafe_Instance_Data
    {
        HWND hActiveWnd;
    };

    CAtlFileMapping<BkSafe_Instance_Data> m_mapping;

    void _GetFileInfo(BYTE *pbyInfo, LPCWSTR lpszFileInfoKey, LPWSTR lpszFileInfoRet);
    BOOL _GetPEProductVersion(LPCWSTR lpszFileName, CString &strProductVersion);
    void _InitUIResource();
	void _RestartSelf();
};

extern CBeikeSafeModule _Module;
