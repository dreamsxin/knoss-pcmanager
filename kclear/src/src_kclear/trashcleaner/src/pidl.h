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

/********************************************************************
* CreatedOn: 2007-1-12   17:59
* FileName: Pidl.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/
#ifndef _PIDL_H_
#define _PIDL_H_

#include <shlobj.h>


BOOL GetName (LPSHELLFOLDER lpsf, LPITEMIDLIST lpi, DWORD dwFlags, LPTSTR lpFriendlyName);

int GetIconIndex(LPCTSTR lpi, UINT uFlags);
int GetNormalAndSelectedIcons(LPCTSTR lpifq, LPTVITEM lptvitem);
int GetNormalAndSelectedIcons(LPITEMIDLIST lpifq, LPTVITEM lptvitem);

LPITEMIDLIST GetFullyQualPidl(LPSHELLFOLDER lpDesktopFolder, LPSHELLFOLDER lpsf, LPITEMIDLIST lpi);

bool PidlIsEqual(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
LPITEMIDLIST PidlConcatPidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
LPITEMIDLIST PidlNext(LPCITEMIDLIST pidl);
UINT PidlGetSize(LPCITEMIDLIST pidl);
LPITEMIDLIST PidlClone(LPCITEMIDLIST pidl, int cb = -1);
LPITEMIDLIST GetItemIDFromPath(LPCTSTR pszPath);
LPITEMIDLIST PidlRemoveLastID(LPITEMIDLIST pidl);
LPITEMIDLIST PidlFindLastID(LPITEMIDLIST pidl);

class CPidl
{
protected:
    LPITEMIDLIST m_pidl;
public:
    CPidl(): m_pidl(NULL) {}
    CPidl(LPITEMIDLIST other): m_pidl(other) {}
    CPidl(const CPidl& other): m_pidl(::PidlClone(other.m_pidl)) {}
    CPidl(LPCTSTR pszPath): m_pidl(GetItemIDFromPath(pszPath)) { }
    CPidl(int nSpecialFolder)
    {
        ::SHGetSpecialFolderLocation(AfxGetMainWnd()->GetSafeHwnd(),
            nSpecialFolder, &m_pidl);
    }
    virtual ~CPidl()
    { Free(); }

    LPITEMIDLIST Detach()
    { LPITEMIDLIST pidl = m_pidl; m_pidl=NULL; return pidl; }
    LPITEMIDLIST Copy()
    { return PidlClone(m_pidl); }
    void Free() 
    { if( m_pidl ) { FreeBuffer(m_pidl); m_pidl=NULL; } }
    operator bool()
    { return m_pidl != NULL; }
    operator LPITEMIDLIST()
    { return m_pidl; }
    operator LPITEMIDLIST*()       
    { return &m_pidl; }  
    operator LPCITEMIDLIST*()
    { return (LPCITEMIDLIST*)&m_pidl; }
    operator LPCTSTR()
    { return (LPCTSTR)m_pidl; }
    operator LPTSTR()
    { return (LPTSTR)m_pidl;}
    LPITEMIDLIST* operator&()
    { return &m_pidl; }
    CPidl& operator=(LPITEMIDLIST other)
    { Free(); m_pidl=other; return *this; }
    CPidl& operator=(const CPidl& other)
    { Free(); m_pidl=::PidlClone(other.m_pidl); return *this; }

    BOOL IsEmpty()
    { return m_pidl == NULL; }

public:
    static IMalloc* gs_piMalloc;

    static BYTE* AllocBuffer(int  nSize);
    static void  FreeBuffer(void* pBuf);

    static IMalloc* GetMalloc();
    static void DelMalloc();
};


#endif //_PIDL_H_