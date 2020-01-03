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

#ifndef _GARBAGECLEAN_H_
#define _GARBAGECLEAN_H_

#include "CleanProc.h"
#include "i_clean.h"

#include "winosver.h"
#include "fileopt.h"
#include "regopt.h"
/************************************************************************/
//功能:垃圾文件清理
//
/************************************************************************/

extern ScanFileCallBack g_fnScanFile;
extern ScanRegCallBack  g_fnScanReg;
extern void*			g_pMain;

TCHAR strcmpEx( const TCHAR *str1, const TCHAR *str2); 

class CGarbageClean :IClean
{
public:
	CGarbageClean() ;
	~CGarbageClean() ;
	//BOOL	TEST();
	BOOL	ScanGarbageFile(LPCTSTR lpScanPath,CSimpleArray<FILEEXTS_IN>& vec_fileExts_In);
	BOOL	GarbageFile(int iType,LPCTSTR lpFileOrPath,LARGE_INTEGER filesize);
	static BOOL WINAPI myEnumerateFile(LPCTSTR lpFileOrPath, void* pUserData,LARGE_INTEGER filesize);
	static BOOL WINAPI myEnumerateFileEx(int iType,LPCTSTR lpFileOrPath, void* pUserData,LARGE_INTEGER filesize);
public:
	void	StopScan()
	{ 
		m_bScan=FALSE; 
		m_fileOpt.UserBreadFlag(TRUE);
		//m_regOpt.UserBreadFlag(FALSE);
	};
	void	Start()
	{
		m_bScan=TRUE;
		m_fileOpt.UserBreadFlag(FALSE);
		//m_regOpt.UserBreadFlag(FALSE);
	
	};
private:
	BOOL		m_bScan;
	CFileOpt	m_fileOpt;
};

#endif