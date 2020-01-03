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

#ifndef	 _WORDWINCLEAN_H_
#define  _WORDWINCLEAN_H_

#include "CleanProc.h"
#include "i_clean.h"

#include "winosver.h"
#include "fileopt.h"
#include "regopt.h"
#include "apphistory.h"
#include <vector>

/************************************************************************/
//功能:世界之窗清理
//
/************************************************************************/

extern ScanFileCallBack g_fnScanFile;
extern ScanRegCallBack  g_fnScanReg;
extern void*			g_pMain;


class CWordWinClean
{
public:
	CWordWinClean();
	~CWordWinClean();
	void	Start()
	{
		m_appHistory.Start();
		m_regOpt.UserBreadFlag(FALSE);
		m_bScan =TRUE;
	};
	void	StopScan()
	{
		m_appHistory.StopScan();
		m_regOpt.UserBreadFlag(TRUE);
		m_bScan =FALSE;
	};
public:
	BOOL	ScanWordWinClean();
	BOOL	ScanWordWinUser();
	BOOL    ScanWorldIni(CString strPath, CString strSection);
	BOOL    CleanWorldIni(CString strFullInfo);
	BOOL    CleanWordWinUser(CString str);
private:
	CAppHistory m_appHistory;
	CRegOpt		m_regOpt;
	BOOL		m_bScan;
};


#endif