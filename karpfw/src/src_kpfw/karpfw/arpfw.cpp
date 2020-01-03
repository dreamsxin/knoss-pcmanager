#include "stdafx.h"
#include "arpfw.h"
#include "arpsetting.h"
#include "maindlg.h"
#include "tray.h"
#include <GdiPlus.h>

using namespace Gdiplus;

//////////////////////////////////////////////////////////////////////////

CArpFwModule _Module;

//////////////////////////////////////////////////////////////////////////

#define CMDPARAM_NAME_INSTALL       _T("install")
#define CMDPARAM_NAME_UNINSTALL     _T("uninstall")
#define CMDPARAM_NAME_STARTUP       _T("startup")

//////////////////////////////////////////////////////////////////////////


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
    HRESULT hRet = E_FAIL;

    hRet = _Module.Init(hInstance);
    if (FAILED(hRet))
        return -1;

    hRet = _Module.ParseCommandLine(lpstrCmdLine);
    if (FAILED(hRet))
        return -2;

    ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    _Module.InitRichEdit2();
    _Module.Main();

    ::CoUninitialize();

    return 0;
}

//////////////////////////////////////////////////////////////////////////


HRESULT CArpFwModule::Init(HINSTANCE hInstance)
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

    CString strVerDllFileName = m_strAppPath + L"karpfw.exe";

    _GetPEProductVersion(strVerDllFileName, m_strAppVersion);

    m_lAppVersionNumber = ::StrToLong(m_strAppVersion.Mid(m_strAppVersion.ReverseFind(L'.') + 1));

    return hRet;
}

LPCTSTR CArpFwModule::GetProductVersion()
{
    return m_strAppVersion;
}

LPCTSTR CArpFwModule::GetAppDirPath()
{
    return m_strAppPath;
}

HRESULT CArpFwModule::ParseCommandLine(LPCWSTR lpszCmdLine)
{
    HRESULT hRet = S_OK;

    BOOL bRet = _CmdLine.Analyze(lpszCmdLine);
    if (!bRet)
        return E_FAIL;

    return hRet;
}

VOID CArpFwModule::InitRichEdit2()
{
    if (NULL == m_hModRichEdit2)
        m_hModRichEdit2 = ::LoadLibrary(_T("RICHED20.DLL"));
}

UINT_PTR CArpFwModule::Main()
{
    CArpSetting& rArpCore = CArpSetting::Instance();
    HWND hSysTray = NULL;
    int nCmdShow = SW_NORMAL;

    if (_CmdLine.HasParam(CMDPARAM_NAME_INSTALL)) 
    {
        // 安装
        int retval = rArpCore.Install() ? 0 : 1;
        return retval;

    }
    else if (_CmdLine.HasParam(CMDPARAM_NAME_UNINSTALL)) 
    {
        // 卸载
        int retval = rArpCore.UnInstall() ? 0 : 1;
        return retval;

    } 
    else if (_CmdLine.HasParam(CMDPARAM_NAME_STARTUP))
    {
        // 隐藏启动
        nCmdShow = SW_HIDE;
    }

    if (!CheckInstance())
    {
        return 0;
    }

    // 创建窗口
    _InitUIResource();

    rArpCore.StartSP();

    CMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);

    CMainDlg dlgMain;
    CArpTray wndTray;

    if (wndTray.Create(NULL) == NULL) 
    {
        return 0;
    }
    m_hTrayWnd = wndTray.m_hWnd;

    if(dlgMain.Create(NULL) == NULL) 
    {
        return 0;
    }
    m_hMainWnd = dlgMain.m_hWnd;
    ((ArpFwInstanceData *)m_mapping)->hActiveWnd = m_hMainWnd;

    dlgMain.ShowWindow(nCmdShow);

    int nRet = theLoop.Run();

    _Module.RemoveMessageLoop();

    rArpCore.StopSP();

    return nRet;
}

void CArpFwModule::ShowMain()
{
    if (!IsWindowVisible(m_hMainWnd))
        ShowWindow(m_hMainWnd, SW_SHOW);

    if (IsIconic(m_hMainWnd))
        ::SendMessage(m_hMainWnd, WM_SYSCOMMAND, SC_RESTORE | HTCAPTION, 0);

    ::BringWindowToTop(m_hMainWnd);
    ::SetForegroundWindow(m_hMainWnd);
}

void CArpFwModule::CloseMain()
{
    ::PostMessage(m_hMainWnd, WM_CLOSE, 0, 0);
}

void CArpFwModule::EnableArpFw(BOOL bEnable)
{
    ::PostMessage(m_hMainWnd, WM_ARPFW_ENABLE, bEnable, 0);
}

void CArpFwModule::SetTrayIcon(BOOL bEnable)
{
    ::PostMessage(m_hTrayWnd, WM_ARPFW_SETICON, bEnable, 0);
}

//////////////////////////////////////////////////////////////////////////

BOOL CArpFwModule::_GetPEProductVersion(LPCWSTR lpszFileName, CString &strProductVersion)
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


void CArpFwModule::_GetFileInfo(BYTE *pbyInfo, LPCWSTR lpszFileInfoKey, LPWSTR lpszFileInfoRet)
{
    BOOL bRet = FALSE;
    UINT uCount = 0;
    UINT uRetSize = MAX_PATH - 1;
    LPWSTR lpszValue = NULL;

    struct LANGANDCODEPAGE {
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

void CArpFwModule::_InitUIResource()
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiToken;

    GdiplusStartup(&gdiToken, &gdiplusStartupInput, NULL);
}

#define KARPFW_DDE_WND_CLASS    _T("{E79ACF9E-C2AE-4f3d-8629-F2AA7417E9F1}")

BOOL CArpFwModule::CheckInstance()
{
    BOOL bRet = FALSE;
    HRESULT hRet = E_FAIL;
    BOOL bAlreadyExisted = FALSE;

    if (NULL != m_mapping.GetHandle())
        return TRUE;

    hRet = m_mapping.MapSharedMem(sizeof(ArpFwInstanceData), KARPFW_DDE_WND_CLASS, &bAlreadyExisted);
    if (FAILED(hRet))
        return bRet;

    if (bAlreadyExisted)
    {
        HWND hWndActive = ((ArpFwInstanceData *)m_mapping)->hActiveWnd;

        if (!::IsWindowVisible(hWndActive))
        {
            ::ShowWindow(hWndActive, SW_SHOW);
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

//////////////////////////////////////////////////////////////////////////
