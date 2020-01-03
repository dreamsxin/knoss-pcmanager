#pragma once

//////////////////////////////////////////////////////////////////////////

#include "miniutil/bkcmdline.h"

//////////////////////////////////////////////////////////////////////////

class CArpFwModule : public CAppModule
{
public:
    CArpFwModule()
        : m_bShowMainUI(TRUE)
        , m_hModRichEdit2(NULL)
        , m_hMainWnd(NULL)
        , m_hTrayWnd(NULL)
    {
    }

    HRESULT Init(HINSTANCE hInstance);
    HRESULT ParseCommandLine(LPCWSTR lpszCmdLine);
    UINT_PTR Main();
    BOOL CheckInstance();
    void SetActiveWindow(HWND hWndActive);
    VOID InitRichEdit2();

    LPCTSTR	GetAppFilePath();
    LPCTSTR GetAppDirPath();
    LPCTSTR GetProductVersion();

    void ShowMain();
    void CloseMain();
    void EnableArpFw(BOOL bEnable);
    void SetTrayIcon(BOOL bEnable);

protected:
    CString m_strAppFileName;
    CString m_strAppPath;
    CString m_strAppVersion;
    LONG    m_lAppVersionNumber;

    HMODULE	m_hModRichEdit2;
    HWND m_hMainWnd;
    HWND m_hTrayWnd;

    BOOL m_bShowMainUI;

    typedef struct tagArpFwInstanceData {
        HWND hActiveWnd;
    } ArpFwInstanceData;

    CAtlFileMapping<ArpFwInstanceData> m_mapping;

    void _GetFileInfo(BYTE *pbyInfo, LPCWSTR lpszFileInfoKey, LPWSTR lpszFileInfoRet);
    BOOL _GetPEProductVersion(LPCWSTR lpszFileName, CString &strProductVersion);
    void _InitUIResource();
    HANDLE _GetWindowProcess( HWND hWnd );

    CBkCmdLine _CmdLine;
};

//////////////////////////////////////////////////////////////////////////

extern CArpFwModule _Module;

//////////////////////////////////////////////////////////////////////////
