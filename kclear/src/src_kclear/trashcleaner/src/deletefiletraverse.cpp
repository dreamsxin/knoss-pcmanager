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

// DeleteFileTraverse.cpp: implementation of the CDeleteFileTraverse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "trashcleaner.h"
#include "deletefiletraverse.h"
#include "trashdefine.h"
#include "filestruct.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileDelete::CFileDelete()
{
    m_hModule = NULL;

    if (m_hModule == NULL)
        m_pfnMoveFileExA = NULL;
    else
    {
        m_pfnMoveFileExA = 
            (PFN_MoveFileExA)GetProcAddress(m_hModule, "MoveFileExA");
    }
}

CFileDelete::~CFileDelete()
{
    m_pfnMoveFileExA = NULL;
    
    if (m_hModule != NULL)
        FreeLibrary(m_hModule);
}


int CFileDelete::TrashFileDelete(LPCTSTR pszFullName)
{
    int nResult = -1;
    DWORD dwNewAttributes = 0;

    if (!pszFullName)
    {
        nResult = Delete_File_Failed;
        goto Exit0 ;
    }

    dwNewAttributes = ::GetFileAttributes(pszFullName);
    if (INVALID_FILE_ATTRIBUTES == dwNewAttributes)
    {
        nResult = Delete_File_Failed;
        goto Exit0;
    }

    if (dwNewAttributes & FILE_ATTRIBUTE_READONLY)
    {
        dwNewAttributes &= ~FILE_ATTRIBUTE_READONLY;
    }

    ::SetFileAttributes(pszFullName, dwNewAttributes);
    
    nResult = ::DeleteFile(pszFullName);

    if (nResult)
        nResult = Delete_File_Success;
	else
		nResult = Delete_File_Failed;

    if ( ERROR_FILE_NOT_FOUND == GetLastError() )
        nResult = Delete_File_Success;

Exit0:

    return nResult;
}

BOOL CFileDelete::MoveFileEx(
    LPCSTR lpExistingFileName,
    LPCSTR lpNewFileName,
    DWORD dwFlags)
{
    if (m_pfnMoveFileExA != NULL)
        return m_pfnMoveFileExA(lpExistingFileName, lpNewFileName, dwFlags);

    return FALSE;
}


