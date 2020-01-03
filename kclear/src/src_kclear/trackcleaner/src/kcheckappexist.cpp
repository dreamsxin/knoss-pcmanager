// Copyright (c) 2010 Kingsoft Corporation. All rights reserved.
// Copyright (c) 2010 The KSafe Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "stdafx.h"
#include <atlrx.h>
#include "kcheckappexist.h"
#define MAX_KEY_LENGTH 256;
BOOL IsFileExists( LPCTSTR pszFile )
{
	return ::PathFileExists( pszFile ) && !PathIsDirectory( pszFile );
}

BOOL IsAlpha( TCHAR c )
{
	return ( ( c >= 'a' ) && ( c <= 'z' ) ) || 
		( ( c >= 'A' ) && ( c <= 'Z' ) ) ;
}

BOOL IsNumber( TCHAR c )
{
	return ( c >= '0' ) && ( c <= '9' );
}

BOOL IsPossibleSeperator( TCHAR c )
{
	//return ( c == ' ' ) || ( c == ',' ) || ( c == ';' ) || ( c == '-' ) ;
	return ( c < 128 ) && !IsNumber( c ) && !IsAlpha( c );
}

BOOL SearchFirstPath( LPCTSTR pszCmdLine, LPCTSTR* ppszStart, LPCTSTR* ppszEnd )
{
	BOOL bRet = FALSE;

	try
	{
		LPCTSTR pszFilePathReg = TEXT( "([a-zA-Z]:(\\\\[^\\\\/:*?\"<>|]+)+)" );


		CAtlRegExp<> re;
		REParseError status = re.Parse( pszFilePathReg );

		if( REPARSE_ERROR_OK != status )
			throw 0;


		CAtlREMatchContext<> mc;
		if( !re.Match( pszCmdLine, &mc ) )
			throw 0;


		ptrdiff_t nLength = mc.m_Match.szEnd - mc.m_Match.szStart;
		if( nLength <= 0 )
			throw 0;

		*ppszStart = mc.m_Match.szStart;
		*ppszEnd   = mc.m_Match.szEnd;

		bRet = TRUE;
	}

	catch( int )
	{
		bRet = FALSE;
	}

	return bRet;
}

BOOL GetPathFromDisplayIcon( 
					   LPCTSTR pszDisplayIcon, 
					   std::wstring& strPath 
					   )
{
	if( pszDisplayIcon == NULL )
		return FALSE;

	TCHAR szBuffer[MAX_PATH] = {0};
	if( !GetFirstPath( pszDisplayIcon, szBuffer, MAX_PATH ) )
		return FALSE;

	if( !::PathRemoveFileSpec( szBuffer ) )
		return FALSE;

	strPath = szBuffer;
	return TRUE;
}

BOOL GetPathFromUninstallString( 
								LPCTSTR pszUninstallString, 
								std::wstring& strPath 
								)
{
	if( pszUninstallString == NULL )
		return FALSE;

	TCHAR szBuffer[MAX_PATH] = {0};
	if( !GetFirstPath( pszUninstallString, szBuffer, MAX_PATH ) )
		return FALSE;

	if( !::PathRemoveFileSpec( szBuffer ) )
		return FALSE;

	strPath = szBuffer;
	return TRUE;
}

LPCTSTR HasRundll( LPCTSTR pszCmdLine )
{
	if (pszCmdLine == NULL || _tcslen(pszCmdLine) == 0)
		return NULL;

	TCHAR* pszBuffer = new TCHAR[::_tcslen( pszCmdLine ) + 1];
	if( pszBuffer == NULL )
		return NULL;

	::_tcscpy( pszBuffer, pszCmdLine );
	::_tcslwr( pszBuffer );

	LPCTSTR pszRunDllText = _T( "rundll" );
	LPCTSTR pszRunDll = ::_tcsstr( pszBuffer, pszRunDllText );

	if( pszRunDll != NULL )
	{
		pszRunDll = pszCmdLine + ( pszRunDll - pszBuffer ) + ::_tcslen( pszRunDllText );
	}

	delete [] pszBuffer;
	return pszRunDll; // 返回"rundll"字符后的位置
}

BOOL _KillParamEx( LPCTSTR pszCmdLine, LPTSTR pszFileName, int nSize )
{
	BOOL   bRet       = FALSE;
	TCHAR* pszTemp    = NULL;

	try
	{
		if( pszCmdLine == NULL || pszFileName == NULL || nSize <= 0 )
			throw 0;

		int nTempSize = nSize + 128;

		pszTemp = new TCHAR[nTempSize];
		if( pszTemp == NULL )
			throw 0;

		//-----------------------------------------
		// 替换环境变量
		DWORD dwRet = ::ExpandEnvironmentStrings( pszCmdLine, pszTemp, nTempSize);
		if( (int)dwRet > nTempSize )
			throw 0;

		//-----------------------------------------
		// 找出命令行中的第一个路径字符串
		LPCTSTR pszStart = NULL;
		LPCTSTR pszEnd   = NULL;
		bRet = SearchFirstPath( pszTemp, &pszStart, &pszEnd );

		if( !bRet )
			throw 0;

		ptrdiff_t nLength = pszEnd - pszStart;
		if( nLength <= 0 || nLength > nSize - 5 )
			throw 0;

		::_tcsncpy( pszFileName, pszStart, nLength );
		pszFileName[nLength] = 0;


		//-----------------------------------------
		// 查找文件
		// <1> 首先查找该文件名, 若不存在则附加上可能的扩展名进行查找
		// <2> 如果还不存在, 则把文件截断至上一个空格, 并以<1>查找
		// <3> 直到文件存在或已经截断至'\\'.
		LPTSTR pszIter      = pszFileName + nLength;
		LPTSTR pszLastSlash = ::_tcsrchr( pszFileName, '\\' );

		if( pszLastSlash == NULL )
			throw 0;

		while( pszIter > pszLastSlash )
		{
			pszIter[0] = 0;
			if( IsFileExists( pszFileName ) )
				break;

			::_tcsncpy( pszIter, _T( ".exe" ), 5 );
			if( IsFileExists( pszFileName ) )
				break;

			::_tcsncpy( pszIter, _T( ".bat" ), 5 );
			if( IsFileExists( pszFileName ) )
				break;

			::_tcsncpy( pszIter, _T( ".com" ), 5 );
			if( IsFileExists( pszFileName ) )
				break;

			//pszIter = ::_tcsrchr( pszFileName, ' ' );

			do 
			{
				pszIter--;
			} 
			while( pszIter > pszLastSlash && !IsPossibleSeperator( *pszIter ) );
		}


		if( pszIter <= pszLastSlash )
			throw 0;


		bRet = TRUE;
	}

	catch( int )
	{
		if( pszFileName != NULL && nSize > 0 )
		{
			pszFileName[0] = 0;
		}

		bRet = FALSE;
	}


	delete [] pszTemp;
	return bRet;
}

BOOL GetFirstPath( LPCTSTR pszCmdLine, LPTSTR pszFileName, int nSize )
{
	BOOL bRet = FALSE;

	LPCTSTR pszAfterRundll = HasRundll( pszCmdLine );

	if( pszAfterRundll != NULL && pszAfterRundll[0] != 0 )
	{
		bRet = _KillParamEx( pszAfterRundll, pszFileName, nSize );
	}

	if( !bRet )
	{
		bRet = _KillParamEx( pszCmdLine, pszFileName, nSize );
	}

	return bRet;
}
BOOL QueryUninstallData(const std::wstring pszDisplayName, 
						const std::wstring pszFileName)
{
	BOOL     bRet        = FALSE;
	DWORD    dwRet       = ERROR_SUCCESS;
	HKEY     hRootKey    = HKEY_LOCAL_MACHINE;
	HKEY     hKey        = NULL; 
	HKEY     hSubKey     = NULL;
	DWORD    dwSize      = MAX_PATH;
	FILETIME ftLastRec                = {0};
	std::wstring strRegKeyUninstall = TEXT( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall" );
	std::wstring strTemp;
	WCHAR szValueName[MAX_PATH] = {0};  
	LONG lRet = -1;
	int nIndex = 0;

	CRegKey reg;
	lRet = reg.Open(hRootKey, strRegKeyUninstall.c_str(), KEY_READ);
	if (lRet != ERROR_SUCCESS)
	{
		bRet = FALSE;
		goto clean0;
	}
	while(ERROR_SUCCESS == reg.EnumKey(nIndex, szValueName, &dwSize))
	{
		CRegKey regEx;
		strTemp = strRegKeyUninstall;
		strTemp += L"\\";
		strTemp += szValueName;
		WCHAR szBuffer[MAX_PATH] = {0};
		DWORD dwRetSize = MAX_PATH;
		std::wstring strTempDis;
		KCkeckByUnistall appUnintall;
		if (ERROR_SUCCESS != regEx.Open(hRootKey, strTemp.c_str(), KEY_READ))
		{
			nIndex++;
			continue;;
		}
		if (ERROR_SUCCESS == regEx.QueryStringValue(L"DisplayName", szBuffer, &dwRetSize))
		{
			appUnintall.strDisplayName = szBuffer;
		}
		else
		{
			ZeroMemory(szBuffer, MAX_PATH);
			dwSize = MAX_PATH;
			nIndex++;
			continue;
		}
		ZeroMemory(szBuffer, MAX_PATH);
		dwRetSize = MAX_PATH;
		if (ERROR_SUCCESS == regEx.QueryStringValue(L"DisplayIcon", szBuffer, &dwRetSize))
		{
			appUnintall.strDisplayIcon = szBuffer;
		}
		ZeroMemory(szBuffer, MAX_PATH);
		dwRetSize = MAX_PATH;

		if (ERROR_SUCCESS == regEx.QueryStringValue(L"InstallLocation", szBuffer, &dwRetSize))
		{
			appUnintall.strInstallLocation = szBuffer;
		}
		ZeroMemory(szBuffer, MAX_PATH);
		dwRetSize = MAX_PATH;

		if (ERROR_SUCCESS == regEx.QueryStringValue(L"UninstallString", szBuffer, &dwRetSize))
		{
			appUnintall.strUninstallString = szBuffer;
		}

		regEx.Close();

		if (UninstallInfoCheck(appUnintall, pszDisplayName, pszFileName))
		{
			bRet = TRUE;
			goto clean0;
		}
		ZeroMemory(szBuffer, MAX_PATH);
		dwSize = MAX_PATH;
		nIndex++;
	}


clean0:
	reg.Close();
	return bRet;
}

BOOL UninstallInfoCheck(const KCkeckByUnistall appUninatall, 
						const std::wstring pszDisplayName, 
						const std::wstring pszFileName)
{
	BOOL bRet = FALSE;
	std::wstring strDisplayName;
	std::wstring strFileName;
	strDisplayName = pszDisplayName;
	strFileName = pszFileName;
	BOOL bFound = FALSE;
	std::wstring strStartPath;

	if(appUninatall.strDisplayName.find(strDisplayName.c_str()) == -1)
	{
		goto clean0;
	}

	bRet = GetPathFromDisplayIcon( appUninatall.strDisplayIcon.c_str(), strStartPath);
	if( bRet )
	{
		strStartPath += L"\\";
		strStartPath += strFileName;
		if (CheckFileIsExist(strStartPath ))
		{
			bRet = TRUE;
			goto clean0;
		}
	}

	bRet = GetPathFromUninstallString( appUninatall.strUninstallString.c_str(), strStartPath );
	if( bRet )
	{
		strStartPath += L"\\";
		strStartPath += strFileName;
		if (CheckFileIsExist(strStartPath ))
		{
			bRet = TRUE;
			goto clean0;
		}
	}
	bRet = FALSE;
clean0:

	return bRet;
}


BOOL CheckFileIsExist(const std::wstring strFilePath)
{
	BOOL bRet = FALSE;
	DWORD dwFileAtt;
	dwFileAtt = GetFileAttributes(strFilePath.c_str());
	if (dwFileAtt == INVALID_FILE_ATTRIBUTES)
	{
		bRet = FALSE;
		goto clean0;
	}
	bRet = TRUE;
clean0:
	return bRet;
}

BOOL GetAppPathByReg(const KCheckApp theAppInfo)
{
	HKEY hKey = NULL;
	BOOL bRet = FALSE;
	LONG ulRet = -1;
	CRegKey reg;
	WCHAR szPath[MAX_PATH] = {0};
	std::wstring strSubKey;
	std::wstring strKeyName;
	hKey = theAppInfo.hKey;
	strKeyName = theAppInfo.strKeyName;
	strSubKey = theAppInfo.strSubKey;
	ULONG ulSize = MAX_PATH;
	ulRet = reg.Open(hKey, strSubKey.c_str(), KEY_READ);
	if (ulRet != ERROR_SUCCESS)
	{
		bRet = FALSE;
		goto clean0;
	}
	ulRet = reg.QueryStringValue(strKeyName.c_str(), szPath, &ulSize);
	if (ulRet != ERROR_SUCCESS)
	{
		bRet = FALSE;
		goto clean0;
	}

	if (theAppInfo.bIsFile)
	{
		bRet = CheckFileIsExist(szPath);
	}
	else
	{
		std::wstring strFileFolder;
		strFileFolder = szPath;
		strFileFolder += L"\\";
		strFileFolder += theAppInfo.strFileName;
		bRet = CheckFileIsExist(strFileFolder.c_str());
	}
clean0:
	reg.Close();
	return bRet;
}
BOOL _Check360Exist()
{
	BOOL bRet = FALSE;
	KCheckApp theInfo;
	theInfo.hKey = HKEY_LOCAL_MACHINE;
	theInfo.strSubKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\360se 3.exe";
	theInfo.strKeyName = L"Path";
	theInfo.strFileName = L"360se.exe";
	theInfo.bIsFile = FALSE;
	bRet = GetAppPathByReg(theInfo);
	if (!bRet)
	{
		bRet = QueryUninstallData(L"360安全浏览器", L"360SE.exe");
	}
	return bRet;
}

BOOL _CheckTheworldExist()
{
	BOOL bRet = FALSE;
	KCheckApp theInfo;
	theInfo.hKey = HKEY_LOCAL_MACHINE;
	theInfo.strSubKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\TheWorld.exe";
	theInfo.strKeyName = L"Path";
	theInfo.strFileName = L"TheWorld.exe";
	theInfo.bIsFile = FALSE;
	bRet = GetAppPathByReg(theInfo);
	if (!bRet)
	{
		bRet = QueryUninstallData(L"世界之窗浏览器", L"TheWorld.exe");
	}
	return bRet;
}

BOOL _CheckSogouExist()
{
	BOOL bRet = FALSE;
	HKEY hKey = NULL;
	LONG ulRet = -1;
	CRegKey reg;
	WCHAR szPath[MAX_PATH] = {0};

	
	ULONG ulSize = MAX_PATH;
	ulRet = reg.Open(HKEY_LOCAL_MACHINE, L"SOFTWARE\\SogouExplorer", KEY_READ);
	if (ulRet != ERROR_SUCCESS)
	{
		bRet = FALSE;
		goto clean0;
	}
	ulRet = reg.QueryStringValue(NULL, szPath, &ulSize);
	if (ulRet != ERROR_SUCCESS)
	{
		bRet = FALSE;
		goto clean0;
	}

	if (::PathIsDirectory(szPath))
	{
		bRet = TRUE;
	}
	
clean0:
	reg.Close();
	return bRet;
}

BOOL _CheckFireFoxExist()
{
	BOOL bRet = FALSE;
	KCheckApp theInfo;
	CRegKey reg;
	LONG lRet = -1;
	DWORD nIndex = 0;
	DWORD dwSize = MAX_PATH;
	WCHAR szSubName[MAX_PATH] = {0};
	std::wstring strSubKey;
	strSubKey = L"SOFTWARE\\Mozilla";
	lRet = reg.Open(HKEY_LOCAL_MACHINE, strSubKey.c_str(), KEY_READ);
	if (lRet != ERROR_SUCCESS)
	{
		bRet = FALSE; 
		goto clean1;
	}
	while (ERROR_SUCCESS == reg.EnumKey(nIndex, szSubName, &dwSize))
	{
		theInfo.hKey = HKEY_LOCAL_MACHINE;
		theInfo.strSubKey = L"SOFTWARE\\Mozilla\\";
		theInfo.strSubKey += szSubName;
		theInfo.strSubKey += L"\\bin";
		theInfo.strKeyName = L"PathToExe";
		theInfo.strFileName = L"firefox.exe";
		theInfo.bIsFile = TRUE;
		bRet = GetAppPathByReg(theInfo);
		nIndex++;
		if (bRet)
		{
			goto clean0;
		}
	}
clean1:
	if (!bRet)
	{
		bRet = QueryUninstallData(L"Mozilla Firefox", L"firefox.exe");
	}
clean0:
	reg.Close();
	return bRet;
}

BOOL _CheckMaxthonExist()
{
	BOOL bRet = FALSE;
	KCheckApp theInfo;
	theInfo.hKey = HKEY_CURRENT_USER;
	theInfo.strSubKey = L"Software\\Maxthon3";
	theInfo.strKeyName = L"Folder";
	theInfo.strFileName = L"Maxthon.exe";
	theInfo.bIsFile = FALSE;
	bRet = GetAppPathByReg(theInfo);

	if (!bRet)
	{
		theInfo.hKey = HKEY_CURRENT_USER;
		theInfo.strSubKey = L"Software\\Maxthon2";
		theInfo.strKeyName = L"Folder";
		theInfo.strFileName = L"Maxthon.exe";
		theInfo.bIsFile = FALSE;
		bRet = GetAppPathByReg(theInfo);
	}
	if (!bRet)
	{
		bRet = QueryUninstallData(L"傲游", L"Maxthon.exe");
	}
	return bRet;
}

BOOL _CheckTTExist()
{
	BOOL bRet = FALSE;
	KCheckApp theInfo;
	theInfo.hKey = HKEY_LOCAL_MACHINE;
	theInfo.strSubKey = L"SOFTWARE\\Tencent\\TTraveler";
	theInfo.strKeyName = L"Install";
	theInfo.strFileName = L"TTraveler.exe";
	theInfo.bIsFile = FALSE;
	bRet = GetAppPathByReg(theInfo);
	if (!bRet)
	{
		bRet = QueryUninstallData(L"Tencent Traveler", L"TTraveler.exe");
	}
	return bRet;
}

BOOL _CheckChromeExist()
{
	BOOL bRet = FALSE;
	KCheckApp theInfo;
	theInfo.hKey = HKEY_LOCAL_MACHINE;
	theInfo.strSubKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\chrome.exe";
	theInfo.strKeyName = L"Path";
	theInfo.strFileName = L"chrome.exe";
	theInfo.bIsFile = FALSE;
	bRet = GetAppPathByReg(theInfo);
	return bRet;
}
