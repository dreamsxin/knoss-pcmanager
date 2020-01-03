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

// dummyz@126.com

#ifndef _CMDLINE_PARSER_INC_
#define _CMDLINE_PARSER_INC_

#include <list>
#include <atlcoll.h>
#include <atlstr.h>
#include <ShlObj.h>

class CCmdLineParser  
{
public:
	enum {
		DEF_CUR_DIR		= 0x0001, // 在当前目录查找
		DEF_ENV_VAR		= 0x0002, // 在默认的环境变量查找
		DEF_EXT_NAME	= 0x0004, // 使用默认可执行的扩展名
		NEED_PARAM		= 0x0008,  // 解析参数
		DEF_FLAG		= DEF_CUR_DIR | DEF_ENV_VAR | DEF_EXT_NAME | NEED_PARAM
	};

public:
	CCmdLineParser(DWORD dwFlag = DEF_FLAG);
	CCmdLineParser(LPCTSTR lpCmdLine /*dwFlag = DEF_CUR_DIR | DEF_ENV_VAR | DEF_EXT_NAME | NEED_PARAM*/);
	virtual ~CCmdLineParser();
	
	BOOL	DoParser(LPCTSTR lpCmdLine, 
		LPCTSTR lpCurDir = NULL, 
		LPCTSTR lpEnvVars = NULL, 
		LPCTSTR lpExtNames = NULL,
		LPCTSTR lpParentPath = NULL
		);

	void	SetFlag(DWORD dwFlag) {
		m_dwFlag = dwFlag;
	}

	LPCTSTR GetCmd() const {
		return (m_szCmd[0] != 0) ? m_szCmd : NULL;
	}

	LPCTSTR GetParam() const {
		return m_lpParam;
	}

private:
	BOOL	ExpandExtNames(LPTSTR lpFilePath, LPCTSTR lpExtNames);
	BOOL	ExpandEnvVars(LPTSTR lpFilePath, LPCTSTR lpEnvVars, LPCTSTR lpExtNames);

	LPTSTR	ExpandMacro(LPCTSTR lpCmdBegin);
	LPTSTR	ExpandMacro2(LPCTSTR lpCmdBegin);
	LPTSTR	ExpandRelative(LPCTSTR lpCmdBegin, LPCTSTR lpCurDir, BOOL bQuotation);
	BOOL	ExpandAbsolute(LPCTSTR lpCmdBegin, LPCTSTR lpExtNames, BOOL bQuotation);

	void	ExpandParam(LPCTSTR lpCmdEnd);

private:
	TCHAR	m_szCmd[MAX_PATH + 10]; // 多几个字节防止溢出
	LPTSTR	m_lpParam;
	DWORD	m_dwFlag;
};

DWORD GetFileListFromCmd(CCmdLineParser& cmdParser, CAtlArray<CString>& fileArray, LPCTSTR lpCurDir);

#endif /* _CMDLINE_PARSER_INC_ */