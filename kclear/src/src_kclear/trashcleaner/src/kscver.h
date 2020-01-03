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

#pragma once


//////////////////////////////////////////////////////////////////////////

//#include <miniutil/bkcmdline.h>

//////////////////////////////////////////////////////////////////////////

#define KSC_DDE_WND_CLASS L"{A866E23E-2789-44a7-AA60-8E728E108B93}"

//////////////////////////////////////////////////////////////////////////

class CKscVersion
{
public:
	CKscVersion()
	{
	}

	HRESULT Init(HINSTANCE hInstance);
	VOID UnInit();
    LPCTSTR GetAppVersion();

protected:
	CString m_strAppFileName;
	CString m_strAppPath;
	CString m_strAppVersion;
	LONG    m_lAppVersionNumber;

	void _GetFileInfo(BYTE *pbyInfo, LPCWSTR lpszFileInfoKey, LPWSTR lpszFileInfoRet);
	BOOL _GetPEProductVersion(LPCWSTR lpszFileName, CString &strProductVersion);
    BOOL GetProductVersion();
};

//////////////////////////////////////////////////////////////////////////
