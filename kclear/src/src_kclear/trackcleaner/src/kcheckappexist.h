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

#ifndef _KCHECKAPPEXIST_H_
#define _KCHECKAPPEXIST_H_

#include <string>
class KCheckApp
{
public:
	std::wstring strSubKey;
	std::wstring strKeyName;
	HKEY hKey;
	BOOL bIsFile;
	std::wstring strFileName;
};

class KCkeckByUnistall
{
public:
	std::wstring strDisplayName;
	std::wstring strUninstallString;
	std::wstring strInstallLocation;
	std::wstring strDisplayIcon;
};
BOOL QueryUninstallData(const std::wstring pszDisplayName, 
						const std::wstring pszFileName);
BOOL UninstallInfoCheck(const KCkeckByUnistall appUninatall, 
						const std::wstring pszDisplayName, 
						const std::wstring pszFileName);
BOOL IsFileExists( LPCTSTR pszFile );
BOOL IsAlpha( TCHAR c );
BOOL IsNumber( TCHAR c );
BOOL IsPossibleSeperator( TCHAR c );
BOOL GetPathFromDisplayIcon(LPCTSTR pszDisplayIcon, std::wstring& strPath);
BOOL GetPathFromUninstallString(LPCTSTR pszUninstallString, std::wstring& strPath);
LPCTSTR HasRundll( LPCTSTR pszCmdLine );
BOOL _KillParamEx( LPCTSTR pszCmdLine, LPTSTR pszFileName, int nSize );
BOOL SearchFirstPath( LPCTSTR pszCmdLine, LPCTSTR* ppszStart, LPCTSTR* ppszEnd );
BOOL GetFirstPath( LPCTSTR pszCmdLine, LPTSTR pszFileName, int nSize );
BOOL CheckFileIsExist(const std::wstring strFilePath);
BOOL GetAppPathByReg(const KCheckApp theAppInfo);
BOOL _Check360Exist();
BOOL _CheckTheworldExist();
BOOL _CheckFireFoxExist();
BOOL _CheckMaxthonExist();
BOOL _CheckTTExist();
BOOL _CheckChromeExist();
BOOL _CheckSogouExist();

#endif