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

// DeleteFileTraverse.h: interface for the CDeleteFileTraverse class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


#include "TraverseFile.h"


class CFileDelete
{
    typedef BOOL (WINAPI * PFN_MoveFileExA)(LPCSTR, LPCSTR, DWORD);

public:
    
    CFileDelete();
    ~CFileDelete();

    int TrashFileDelete(LPCTSTR pszFullName);


    BOOL MoveFileEx(
        LPCSTR lpExistingFileName,
        LPCSTR lpNewFileName,
        DWORD dwFlags);

private:
    HMODULE m_hModule;
    PFN_MoveFileExA m_pfnMoveFileExA;
};


typedef void (__stdcall * TraverseProcessCallback)(unsigned long, unsigned long);


