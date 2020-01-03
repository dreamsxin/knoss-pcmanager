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

/************************************************************************
* @file      : kppuserlog.h
* @author    : WangBin <WangBin5@kingsoft.com>
* @date      : 6/9/2009 PM 3:32:01
* @brief     : 
*
* $Id: $
/************************************************************************/

#ifndef __KPPUSERLOG_H__
#define __KPPUSERLOG_H__

#include <string>
#include <vector>
#include <map>
#include <atlstr.h>

#define TIME_MAX_LEN 30
#define TIME_LEN 19

class KppUserLog
{

public:

	KppUserLog();

public:

	~KppUserLog();

	STDMETHODIMP Init(const WCHAR* wszPathName);

	STDMETHODIMP Uninit(void);

	STDMETHODIMP WriteSingleLog(const std::wstring& wstLogString);

    STDMETHODIMP ReadOrgLog(std::vector<std::wstring>& vLogCache);

    STDMETHODIMP ReadLog(std::map<std::wstring, std::wstring>& mapLine);

    STDMETHODIMP ClearLog();
	
public:

	static KppUserLog& KppUserLog::UserLogInstance();

protected:

	BOOL ParseTime(std::wstring& sTime, SYSTEMTIME* pTime);

	HRESULT AddPathName(WCHAR* wszMainPathName, const WCHAR* wszPathName);
    
    DWORD _DoGetLineByBuf(
        WCHAR* const pszBuf, 
        DWORD dwlen, 
        std::map<std::wstring, std::wstring>& mapStrline);

    DWORD _DoGetFileSizeByFileName(const WCHAR* pFileName);

    DWORD _DoGetLineByBuf(
        WCHAR* const pszBuf, 
        DWORD dwlen, 
        std::vector<std::wstring>& vcStrline);

    DWORD _DoGetLineDescByBuf(
        WCHAR* const pszBuf, 
        DWORD dwlen, 
        std::map<std::wstring, std::wstring>& mapStrline);

protected:

	void _LockWork();

	void _UnlockWork();

protected:

	CRITICAL_SECTION			m_LogLock;
	FILE*						m_pFile;
	std::wstring				m_strPathName;
	std::locale					m_Loc;
	BOOL						m_bInitFlag;
	
};

#endif /* __KPPUSERLOG_H__ */
