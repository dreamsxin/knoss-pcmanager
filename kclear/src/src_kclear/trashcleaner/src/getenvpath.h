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

// GetEnvPath.h: interface for the CGetEnvPath class.
//
//////////////////////////////////////////////////////////////////////


#pragma once


class CGetEnvPath  
{
public:
	CGetEnvPath();
	virtual ~CGetEnvPath();
    
public:    
    CString GetRealPath(LPCTSTR pszEnvPath);


private:

    CString CombinationPath(const CString& strLeft, 
        const CString& strMiddle, 
        const CString& strRight);

    BOOL FindEnvironmentPos(LPCTSTR pszEnvPath, int& nFirstPos, int& nSecondPos);
    CString GetEnvVariable(LPCTSTR pszName);
    CString GetFolderPath(LPCTSTR pszName);
    CString GetDrive(CString suffix);

private:

    typedef DWORD (WINAPI* pfnGetLongPathNameW)(LPWSTR, LPWSTR, DWORD);

    HMODULE m_hModule;
    pfnGetLongPathNameW m_pfnGetLongPathName;
};

