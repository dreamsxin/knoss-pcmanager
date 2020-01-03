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
#include "koperatefile.h"


DWORD _DoGetLineByBuf(WCHAR* const pszBuf, DWORD dwlen,
					std::vector<std::wstring>& vcStrline)
{
	if (!pszBuf)
	{
		return 0;
	}

	WCHAR* pszBeginPoint = pszBuf;
	WCHAR* pszCurrentPoint = pszBuf;
	WCHAR* pszTempBuf = NULL;
	DWORD dwlenSize = 0;
	DWORD dwLineCount = 0;

	while (*pszCurrentPoint != 0 && (unsigned)(pszCurrentPoint - pszBuf) <= dwlen)
	{
		if (*pszCurrentPoint == '\n')
		{
			dwlenSize = (DWORD)(pszCurrentPoint - pszBeginPoint);
			pszTempBuf = new WCHAR[dwlenSize + 1];
			if (!pszTempBuf)
			{
				break;
			}
			::ZeroMemory(pszTempBuf, (dwlenSize + 1)* 2);
			::memcpy(pszTempBuf, pszBeginPoint, dwlenSize * 2);

			pszBeginPoint = pszCurrentPoint + 1;

			vcStrline.push_back(pszTempBuf);
			dwLineCount++ ;

			delete []pszTempBuf;
			pszTempBuf = NULL;
		}

		pszCurrentPoint ++;
	}

	return dwLineCount;
}

DWORD _DoGetFileSizeByFileName(const WCHAR* pFileName)
{
	if (!pFileName)
	{
		return 0;
	}

	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD dwFileSize = 0;

	hFile = ::CreateFile(
		pFileName, 
		GENERIC_READ,
		0, 
		NULL,
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL
		);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		dwFileSize = ::GetFileSize(hFile, NULL);
		::CloseHandle(hFile);
	}

	return dwFileSize;
}