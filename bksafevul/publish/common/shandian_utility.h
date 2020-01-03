/**
* @file    shandian_utility.h
* @brief   ...
* @author  bbcallen
* @date    2010-04-19 19:58
*/

#ifndef SHANDIAN_UTILITY_H
#define SHANDIAN_UTILITY_H

#include <atlbase.h>
#include <atlsecurity.h>
#include <shlobj.h>
#include "winmod\winpath.h"
#include <bksafe\bksafedefines.h>

#define SHANDIAN_INSTALL_KEY    L"SOFTWARE\\Knsd"
#define SHANDIAN_APP_PATH       L"AppPath"
#define SHANDIAN_DATA_PATH      L"DataPath"
#define SHANDIAN_QUARA_PATH     L"QuarantinePath"


#define SHANDIAN_DATA_DIR       L"Lib"
#define SHANDIAN_CFG_DIR        L"Config"
#define SHANDIAN_QURAR_DIR      L"KnsdQuarantine"
#define SHANDIAN_CACHE_DIR      L"Cache"
#define SHANDIAN_LOG_DIR        L"ProgramData"
#define SHANDIAN_TMP_DIR        L"Temp"
#define SHANDIAN_AVP_DIR        L"Ave"
	#define SHANDIAN_AVP_CACHE_DIR        L"AveCache"
#define SHANDIAN_HOTFIX_DIR	    L"Hotfix"
#define SHANDIAN_DRIVERINSTALL_DIR        L"install"

#define SHANDIAN_APPDATA_DIR    L"Knsd"
#define SHANDIAN_RECYCLE_DIR    L"KnsdRecycle"

class CShandianPath
{
public:
    static CShandianPath& Instance()
    {
        static  CShandianPath _ins;
        return  _ins;
    }

public:	
    // return S_FALSE, 目录已存在
    // return S_OK, 目录被新创建
    // return E_XX, 失败
    HRESULT CreateFullPath(
        LPCWSTR lpszFileName, 
        DWORD dwFileAttribute = FILE_ATTRIBUTE_NORMAL
        )
    {
        HRESULT hr = S_FALSE;
        CString strPath = lpszFileName;

        if (!strPath.IsEmpty() && L'\\' != strPath[strPath.GetLength() - 1])
        {
            strPath.AppendChar(L'\\');
        }


        int nPos = strPath.Find(L'\\');
        while (-1 != nPos)
        {
            CString strParent    = strPath.Left(nPos);
            DWORD   dwFileAttrib = ::GetFileAttributes(strParent);
            if (INVALID_FILE_ATTRIBUTES == dwFileAttrib)
            {
                BOOL bRet = ::CreateDirectory(strParent, NULL);
                if (!bRet)
                    return GetLastError() ? AtlHresultFromLastError() : E_FAIL;

                ::SetFileAttributes( strParent, dwFileAttribute );
                hr = S_OK;
            }

            nPos = strPath.Find(_T('\\'), nPos + 1);
        }

        return hr;
    }
	
	LPCTSTR GetProductVersion();
	BOOL _GetFileVersion( LPCTSTR szPath, LARGE_INTEGER &lgVersion );

public:
    HRESULT GetProductAppdataFolderPath( CString& strPath )
    {
        WinMod::CWinPath path;
        WCHAR   wszBuffer[ MAX_PATH + 1 ] = { 0 };

        HRESULT hr = ::SHGetFolderPath(
            NULL,
            CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE,
            NULL,
            SHGFP_TYPE_CURRENT,
            wszBuffer
            );
        if ( FAILED( hr ) )
        {
            goto Exit0;
        }

        path.m_strPath = wszBuffer;
        path.Append( SHANDIAN_APPDATA_DIR );

        strPath = path.m_strPath;
Exit0:
        return hr;
    }

    HRESULT GetProductAppPath( CString& strPath, BOOL bSelfModePath = TRUE )
    {
        if ( bSelfModePath )
        {
            return GetPath( strPath, SHANDIAN_APP_PATH, NULL, ( HMODULE )&__ImageBase );
        }
        else
        {
            return GetPath( strPath, SHANDIAN_APP_PATH, NULL, NULL );
        }
    }

	HRESULT GetProductHotFixPath(CString& strPath,BOOL bCreate=TRUE, BOOL bSelfModePath = TRUE)
	{
		HRESULT hr = S_OK;
		WinMod::CWinPath path;

		hr = GetProductAppPath( path.m_strPath, bSelfModePath );
		if ( SUCCEEDED( hr ) )
		{
			path.Append( SHANDIAN_HOTFIX_DIR );
			strPath = path.m_strPath;

			if ( bCreate )
			{
				hr = CreateFullPath( strPath );
				if ( S_OK == hr )
				{
					hr = FixFilePrivilege( strPath );
					if ( FAILED( hr ) )
					{
						hr = S_FALSE;
					}
				}
			}
		}

		return hr;
	}

    HRESULT GetProductDataPath( CString& strPath, BOOL bCreate = FALSE, BOOL bSelfModePath = TRUE )
    {
        HRESULT hr = S_OK;

        if ( bSelfModePath )
        {
            hr = GetPath( strPath, SHANDIAN_DATA_PATH, SHANDIAN_DATA_DIR, ( HMODULE )&__ImageBase );
        }
        else
        {
            hr = GetPath( strPath, SHANDIAN_DATA_PATH, SHANDIAN_DATA_DIR, NULL );
        }

        if ( SUCCEEDED( hr ) && bCreate )
        {
            hr = CreateFullPath( strPath );
            if ( S_OK == hr )
            {
                hr = FixFilePrivilege( strPath );
                if ( FAILED( hr ) )
                {
                    hr = S_FALSE;
                }
            }
        }

        return hr;
    }

    HRESULT GetProductQuarantinePath( CString& strPath )
    {
        WinMod::CWinPath path;
        HRESULT hr = S_OK;

        hr = GetProductAppdataFolderPath( path.m_strPath );
        if ( FAILED( hr ) )
        {
            goto Exit0;
        }

        path.RemoveFileSpec();
        path.Append( SHANDIAN_QURAR_DIR );

        strPath = path.m_strPath;
Exit0:
        return hr;
    }

    // 创建隔离目录并且返回隔离目录
    HRESULT CreateProductQuarantinePath( CString& strPath )
    {
        HRESULT hr = S_OK;

        hr = GetProductQuarantinePath( strPath );
        if ( FAILED( hr ) )
        {
            return hr;
        }

		if ( !WinMod::CWinPathApi::IsFileExisting(strPath) )
		{
			hr = CreateFullPath( strPath, FILE_ATTRIBUTE_HIDDEN );
			if ( FAILED( hr ) )
			{
				return hr;
			}

			if ( S_OK == hr )
			{
				hr = FixFilePrivilege( strPath, FILE_GENERIC_READ, FILE_EXECUTE );
				if ( FAILED( hr ) )
				{
					hr = S_FALSE;
				}
			}
		}

        return hr;
    }

    HRESULT GetProductCfgPath( CString& strPath, BOOL bCreate = FALSE, BOOL bSelfModePath = TRUE )
    {
        HRESULT hr = S_OK;
        WinMod::CWinPath path;

        hr = GetProductAppPath( path.m_strPath, bSelfModePath );
        if ( SUCCEEDED( hr ) )
        {
            path.Append( SHANDIAN_CFG_DIR );
            strPath = path.m_strPath;

            if ( bCreate )
            {
                hr = CreateFullPath( strPath );
                if ( S_OK == hr )
                {
                    hr = FixFilePrivilege( strPath );
                    if ( FAILED( hr ) )
                    {
                        hr = S_FALSE;
                    }
                }
            }
        }

        return hr;
    }

    HRESULT GetProductCachePath( CString& strPath, BOOL bCreate = FALSE )
    {
        HRESULT hr = S_OK;
        WinMod::CWinPath path;

        hr = GetProductAppdataFolderPath( path.m_strPath );
        if ( SUCCEEDED( hr ) )
        {
            path.Append( SHANDIAN_CACHE_DIR );
            strPath = path.m_strPath;
            if ( bCreate )
            {
                hr = CreateFullPath( strPath );
                if ( S_OK == hr )
                {
                    hr = FixFilePrivilege( strPath );
                    if ( FAILED( hr ) )
                    {
                        hr = S_FALSE;
                    }
                }
            }
        }

        return hr;
    }

    HRESULT GetProductLogPath( CString& strPath, BOOL bCreate = TRUE, BOOL bSelfModePath = TRUE )
    {
        HRESULT hr = S_OK;
        WinMod::CWinPath path;

        hr = GetProductAppPath( path.m_strPath, bSelfModePath );
        if ( SUCCEEDED( hr ) )
        {
            path.Append( SHANDIAN_LOG_DIR );
            strPath = path.m_strPath;

            if ( bCreate )
            {
                hr = CreateFullPath( strPath );
                if ( S_OK == hr )
                {
                    hr = FixFilePrivilege( strPath );
                    if ( FAILED( hr ) )
                    {
                        hr = S_FALSE;
                    }
                }
            }
        }

        return hr;
    }

	HRESULT GetProductDriverInstallPath( CString& strPath, BOOL bCreate = TRUE, BOOL bSelfModePath = TRUE )
	{
		HRESULT hr = S_OK;
		WinMod::CWinPath path;

		hr = GetProductAppPath( path.m_strPath, bSelfModePath );
		if ( SUCCEEDED( hr ) )
		{
			path.Append( SHANDIAN_DRIVERINSTALL_DIR );
			strPath = path.m_strPath;

			if ( bCreate )
			{
				hr = CreateFullPath( strPath );
				if ( S_OK == hr )
				{
					hr = FixFilePrivilege( strPath );
					if ( FAILED( hr ) )
					{
						hr = S_FALSE;
					}
				}
			}
		}

		return hr;
	}

    HRESULT GetProductTempPath( CString& strPath, BOOL bCreate = FALSE, BOOL bSelfModePath = TRUE )
    {
        HRESULT hr = S_OK;
        WinMod::CWinPath path;

        hr = GetProductAppPath( path.m_strPath, bSelfModePath );
        if ( SUCCEEDED( hr ) )
        {
            path.Append( SHANDIAN_TMP_DIR );
            strPath = path.m_strPath;

            if ( bCreate )
            {
                hr = CreateFullPath( strPath );
                if ( S_OK == hr )
                {
                    hr = FixFilePrivilege( strPath );
                    if ( FAILED( hr ) )
                    {
                        hr = S_FALSE;
                    }
                }
            }
        }

        return hr;
    }

    HRESULT GetProductScanTempPath( CString& strPath, BOOL bCreate )
    {
        return GetProductTempPath( strPath, bCreate );
    }
	
    HRESULT GetProductAvpPath( CString& strPath, BOOL bCreate = TRUE, BOOL bSelfModePath = TRUE )
    {
        HRESULT hr = S_OK;
        WinMod::CWinPath path;

        hr = GetProductAppPath( path.m_strPath, bSelfModePath );
        if ( SUCCEEDED( hr ) )
        {
            path.Append( SHANDIAN_AVP_DIR );
            strPath = path.m_strPath;

            if ( bCreate )
            {
                hr = CreateFullPath( strPath );
                if ( S_OK == hr )
                {
                    hr = FixFilePrivilege( strPath );
                    if ( FAILED( hr ) )
                    {
                        hr = S_FALSE;
                    }
                }
            }
        }

        return hr;
    }
	
	HRESULT GetProductAvpCachePath( CString& strPath, BOOL bCreate = TRUE, BOOL bSelfModePath = TRUE )
	{
        HRESULT hr = S_OK;
        WinMod::CWinPath path;

        hr = GetProductAppdataFolderPath( path.m_strPath );
        if ( SUCCEEDED( hr ) )
        {
            path.Append( SHANDIAN_AVP_CACHE_DIR );
            strPath = path.m_strPath;
            if ( bCreate )
            {
                hr = CreateFullPath( strPath );
                if ( S_OK == hr )
                {
                    hr = FixFilePrivilege( strPath );
                    if ( FAILED( hr ) )
                    {
                        hr = S_FALSE;
                    }
                }
            }
        }

        return hr;
	}

    HRESULT GetProductRecyclePath( const CString& strReference, CString& strPath )
    {
        WinMod::CWinPath path( strReference );

        path.StripToRoot();
        path.Append( SHANDIAN_RECYCLE_DIR );

        strPath = path.m_strPath;
        return S_OK;
    }

    HRESULT CreateProductRecyclePath( const CString& strReference, CString& strPath )
    {
        HRESULT hr = S_OK;

        hr = GetProductRecyclePath( strReference, strPath );
        if ( FAILED( hr ) )
        {
            return hr;
        }

        if( !WinMod::CWinPathApi::IsFileExisting( strPath ) )
        {
            hr = ::CreateDirectory( strPath, NULL );
            if ( FAILED( hr ) )
            {
                return hr;
            }

            SetFileAttributes( strPath, FILE_ATTRIBUTE_HIDDEN );

            hr = FixFilePrivilege( strPath, FILE_GENERIC_READ, FILE_EXECUTE );
            if ( FAILED( hr ) )
            {
                hr = S_FALSE;
            }
        }


        return hr;
    }

    HRESULT FixFilePrivilege(
        LPCWSTR lpszFileName, 
        DWORD dwAllowACE = FILE_ALL_ACCESS, 
        DWORD dwDeniedACE = 0 
        )
    {
        HRESULT hr = S_FALSE;

        if ( IsVistaOrLater() )
        {
            return S_OK;
        }

        try
        {
            CDacl dacl;

            BOOL bRet = ATL::AtlGetDacl(lpszFileName, SE_FILE_OBJECT, &dacl);
            if (!bRet)
                return GetLastError() ? AtlHresultFromLastError() : E_FAIL;

            dacl.RemoveAllAces();

            // 允许users用户
            bRet = dacl.AddAllowedAce( Sids::Users(), dwAllowACE, CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE );
            if (!bRet)
                return GetLastError() ? AtlHresultFromLastError() : E_FAIL;
            // 拒绝everyone
            bRet = dacl.AddDeniedAce( Sids::World(), dwDeniedACE, CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE );
            if (!bRet)
                return GetLastError() ? AtlHresultFromLastError() : E_FAIL;

            bRet = ATL::AtlSetDacl(lpszFileName, SE_FILE_OBJECT, dacl);
            if (!bRet)
                return GetLastError() ? AtlHresultFromLastError() : E_FAIL;


            return S_OK;
        }
        catch (...)
        {
            hr = S_FALSE;
        }

        return hr;
    }
	
	HRESULT GetProductAVPBasesPath( CString& strPath, BOOL bCreate = TRUE, BOOL bSelfModePath = TRUE )
	{
		HRESULT hr = S_OK;
		WinMod::CWinPath path;
		
		hr = GetProductAvpPath( path.m_strPath, bSelfModePath );
		if ( SUCCEEDED( hr ) )
		{
			path.Append( _T("bases") );
			strPath = path.m_strPath;
			
			if ( bCreate )
			{
				hr = CreateFullPath( strPath );
				if ( S_OK == hr )
				{
					hr = FixFilePrivilege( strPath );
					if ( FAILED( hr ) )
					{
						hr = S_FALSE;
					}
				}
			}
		}
		return hr;
	}
	
	HRESULT GetProductAVPTempPath( CString& strPath, BOOL bCreate = TRUE, BOOL bSelfModePath = TRUE )
	{
		HRESULT hr = S_OK;
		WinMod::CWinPath path;
		
		hr = GetProductAvpPath( path.m_strPath, bSelfModePath );
		if ( SUCCEEDED( hr ) )
		{
			path.Append( _T("AVPTemp") );
			strPath = path.m_strPath;

			if ( bCreate )
			{
				hr = CreateFullPath( strPath );
				if ( S_OK == hr )
				{
					hr = FixFilePrivilege( strPath );
					if ( FAILED( hr ) )
					{
						hr = S_FALSE;
					}
				}
			}
		}
		return hr;
	}

private:
    HRESULT GetPath( 
        CString& strPath, 
        LPCWSTR lpwszValueName, 
        LPCTSTR lpwszDefaultDirName,
        HMODULE hBaseModule // 指定一个产品的句柄，来获取产品安装路径
        )
    {
        LONG    lret = 0;
        DWORD   dwRet = 0;
        CRegKey reg;
        HRESULT hr = S_OK;
        WinMod::CWinPath path;
        WCHAR   wszBuffer[ MAX_PATH + 1 ] = { 0 };

        if ( NULL == lpwszValueName )
        {
            hr = E_INVALIDARG;
            goto Exit0;
        }

        lret = reg.Open( HKEY_LOCAL_MACHINE, SHANDIAN_INSTALL_KEY, KEY_READ );
        if ( ERROR_SUCCESS == lret )
        {
            dwRet = MAX_PATH;
            lret = reg.QueryStringValue( 
                lpwszValueName,
                wszBuffer,
                &dwRet
                );
            if ( ERROR_SUCCESS == lret )
            {
                wszBuffer[ MAX_PATH ] = L'\0';
                path.m_strPath = wszBuffer;
                goto Exit0;
            }
        }

        // local path
        dwRet = ::GetModuleFileName( 
            hBaseModule, 
            wszBuffer, 
            MAX_PATH
            );
        if ( 0 == dwRet )
        {
            hr = HRESULT_FROM_WIN32( GetLastError() );
            goto Exit0;
        }

        wszBuffer[ MAX_PATH ] = L'\0';
        path.m_strPath = wszBuffer;
        path.RemoveFileSpec();
        if ( lpwszDefaultDirName )
        {
            path.Append( lpwszDefaultDirName );
        }
Exit0:
        if ( SUCCEEDED( hr ) )
        {
            strPath = path.m_strPath;
        }
        return hr;
    }

    inline BOOL IsVistaOrLater()
    {
        OSVERSIONINFO osInfo;
        osInfo.dwOSVersionInfoSize = sizeof(osInfo);
        ::GetVersionEx(&osInfo);

        return ( 0 <= ( ( int )( osInfo.dwMajorVersion - 6 ) ) );
    }

};


#endif//SHANDIAN_UTILITY_H