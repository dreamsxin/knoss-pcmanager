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

#ifndef	 _360CLEAN_H_
#define  _360CLEAN_H_

#include "CleanProc.h"
#include "i_clean.h"

#include "winosver.h"
#include "fileopt.h"
#include "regopt.h"
#include "apphistory.h"

/************************************************************************/
//¹¦ÄÜ:360
//
/************************************************************************/

extern ScanFileCallBack g_fnScanFile;
extern ScanRegCallBack  g_fnScanReg;
extern void*			g_pMain;


class C360Clean
{
public:
	C360Clean();
	~C360Clean();
	void Start()
	{
		m_appHistory.Start();
		m_bScan =TRUE;
	}
	void	StopScan(){m_appHistory.StopScan(); m_bScan =FALSE;};
public:
	BOOL	Scan360Clean();
	BOOL	Scan360User(int nType);
	BOOL    Clean360User(CString str);
private:
	CAppHistory m_appHistory;
	BOOL		m_bScan;
	BOOL        m_bFlag[10000];
};

#endif