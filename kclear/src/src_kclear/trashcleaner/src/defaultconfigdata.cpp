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

// DefaultConfigData.cpp: implementation of the CDefaultConfigData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "trashcleaner.h"
#include "DefaultConfigData.h"
#include <Shlwapi.h>
#include "trashpublicfunc.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CDefaultConfigData::CDefaultConfigData()
{
   // m_strConfigFile = GetDefaultConfigFile();
}

CDefaultConfigData::~CDefaultConfigData()
{

}

int CDefaultConfigData::ReadConfig()
{
    return _ReadConfigFile();
}


int CDefaultConfigData::WriteConfig()
{
    return _WriteConfigFile();
}

CString CDefaultConfigData::GetDefaultConfigFile()
{
	/*CString strRet = KasLangGetTrashConfigFilePath();
	if( strRet.GetLength() != 0 && ::PathFileExists(strRet) )
		goto Exit0;

    TCHAR szPath[MAX_PATH] = {0};

    if (!::GetModuleFileName(AfxGetModuleState()->m_hCurrentInstanceHandle, szPath, MAX_PATH))
        goto Exit0;
    
    int nPathSize = _tcslen(szPath);
    int nLastPos  = -1;

    for (int i = 0; i < nPathSize; i++)
    {
        if (szPath[i] == TEXT('\\'))
            nLastPos = i;
    }

    if (nLastPos != -1)
        szPath[nLastPos] = 0;
    _tcscat(szPath, TEXT("\\"));
    _tcscat(szPath, FILENAME_TRASHCONFIG);
    strRet = szPath;

Exit0:*/
    return L"";
}


//////////////////////////////////////////////////////////////////////////
//
// 
//
//////////////////////////////////////////////////////////////////////////


