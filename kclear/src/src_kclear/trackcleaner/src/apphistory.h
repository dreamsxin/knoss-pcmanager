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

#ifndef _APPHISTORY_H_
#define _APPHISTORY_H_

#include "CleanProc.h"
#include "i_clean.h"

#include "winosver.h"
#include "fileopt.h"
#include "regopt.h"

/************************************************************************/
//功能:应用软件清理
//
/************************************************************************/

extern ScanFileCallBack g_fnScanFile;
extern ScanRegCallBack  g_fnScanReg;
extern void*			g_pMain;

class CAppHistory
{
public:
	CAppHistory();
	~CAppHistory();
public:
	void	Start()
	{
		m_bScan=TRUE;
		m_fileOpt.UserBreadFlag(FALSE);
		m_regOpt.UserBreadFlag(FALSE);

	};
	void	StopScan()
	{ 
		m_bScan=FALSE;
		m_fileOpt.UserBreadFlag(TRUE);
		m_regOpt.UserBreadFlag(TRUE);
	};
public:
	BOOL CommfunReg(int iType,LPCTSTR lpcszRegPath,CSimpleArray<CString>& vec_valueName);
	BOOL CommfunFile(int iType,LPCTSTR lpcszFilePath,CSimpleArray<CString>& vec_fileName,BOOL bRecursion=TRUE,BOOL bEnumFiles=TRUE);
	
	BOOL CommfunRegProc(int iType,HKEY hRootKey,LPCTSTR lpSubKey,LPCTSTR lpValueName,LPCTSTR lpValueData );
	BOOL CommfunFileProc(int iType,LPCTSTR lpFileOrPath,LARGE_INTEGER filesize);
	BOOL ConvetPath(CString& strSourcPath);
	CString GetFolderPath(LPCTSTR pszName);
private:
	CFileOpt				m_fileOpt;
	CRegOpt					m_regOpt;
	BOOL					m_bScan;
	TCHAR					m_szLoginName[MAX_PATH];
public:
	static BOOL WINAPI myEnumerateFile(LPCTSTR lpFileOrPath, void* pUserData,LARGE_INTEGER filesize);
	static BOOL WINAPI myEnumerateReg(HKEY rootKey,LPCTSTR lpItemPath,LPCTSTR lpName,DWORD dwValueNameSize,LPBYTE lpValue ,DWORD dwValueSize,DWORD dwType,void* pUserData);
};

#endif