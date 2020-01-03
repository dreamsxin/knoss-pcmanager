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
#include "pidl.h"
#include <shlwapi.h>

IMalloc* CPidl::gs_piMalloc = NULL;

BYTE* CPidl::AllocBuffer(int  nSize)
{
    if (gs_piMalloc == NULL)
        GetMalloc();
    
    if (gs_piMalloc == NULL)
        return NULL;

    return (BYTE*)gs_piMalloc->Alloc(nSize);
}
void  CPidl::FreeBuffer(void* pBuf)
{
    if (gs_piMalloc == NULL)
        GetMalloc();

    if (gs_piMalloc != NULL)
        gs_piMalloc->Free(pBuf);

    DelMalloc();
}

IMalloc* CPidl::GetMalloc()
{
    if (gs_piMalloc == NULL)
    {
        ::SHGetMalloc(&gs_piMalloc);
    }

    if (gs_piMalloc != NULL)
        gs_piMalloc->AddRef();

    return gs_piMalloc ;
}

void CPidl::DelMalloc()
{
    if (gs_piMalloc == NULL)
        return;

    if (gs_piMalloc->Release() == 0)
        gs_piMalloc = NULL;
}



BOOL GetName(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi, DWORD dwFlags, LPTSTR lpFriendlyName)
{
    BOOL bSuccess = TRUE;
//    STRRET str = { STRRET_CSTR };
//    
//    if (lpsf->GetDisplayNameOf(lpi, dwFlags, &str) == NOERROR)
//    {
//        switch (str.uType)
//        {
//        case STRRET_WSTR:
//            lstrcpy(lpFriendlyName, W2CT(str.pOleStr));
//            ::CoTaskMemFree(str.pOleStr);
//            break;
//        case STRRET_OFFSET:
//            {
//                if (StrRetToBuf(&str, lpi, lpFriendlyName, 255) != S_OK)
//                    bSuccess = FALSE;
//            }
//            break;
//        case STRRET_CSTR:
//            {
//                if (StrRetToBuf(&str, lpi, lpFriendlyName, 255) != S_OK)
//                    bSuccess = FALSE;
//            }
//            break;
//        default:
//            bSuccess = FALSE;
//            break;
//        }
//    }
//    else
//    {
//        bSuccess = FALSE;
//    }

    return bSuccess;
}

int GetIconIndex(LPCTSTR lpi, UINT uFlags)
{
    SHFILEINFO sfi = { 0 };
    DWORD dwRet = ::SHGetFileInfo((LPCTSTR)lpi, 0, &sfi, sizeof(SHFILEINFO), uFlags);
    return (dwRet != 0) ? sfi.iIcon : -1;
}

int GetNormalAndSelectedIcons(LPCTSTR lpifq, LPTVITEM lptvitem)
{
    int nRet = lptvitem->iImage = GetIconIndex(lpifq, SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
    if (nRet < 0)
        return -1;

    nRet = lptvitem->iSelectedImage = GetIconIndex(lpifq, SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_OPENICON);
    if (nRet < 0)
        return -1;

    return 0;

}


int GetNormalAndSelectedIcons(LPITEMIDLIST lpifq, LPTVITEM lptvitem)
{
    int nRet = lptvitem->iImage = GetIconIndex((LPCTSTR)lpifq, SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
    if (nRet < 0)
        return -1;
    nRet = lptvitem->iSelectedImage = GetIconIndex((LPCTSTR)lpifq, SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_OPENICON);
    if (nRet < 0)
        return -1;

    return 0;
}

LPITEMIDLIST GetFullyQualPidl(LPSHELLFOLDER lpDesktopFolder, LPSHELLFOLDER lpsf, LPITEMIDLIST lpi)
{
    TCHAR szBuff[MAX_PATH] = { 0 };

    if (!GetName(lpsf, lpi, SHGDN_FORPARSING, szBuff))
        return NULL;

    ULONG ulEaten = 0;
    LPITEMIDLIST lpifq = NULL;
//    ULONG ulAttribs = 0;
//    USES_CONVERSION;
//
//    HRESULT hr = lpDesktopFolder->ParseDisplayName(NULL, NULL, T2W(szBuff), &ulEaten, &lpifq, &ulAttribs);
//
//    if (FAILED(hr))
//        return NULL;

    return lpifq;
}

bool PidlIsEqual(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
    bool bResult = false;

    if (pidl1 == NULL || pidl2 == NULL)
        return bResult;
    
    UINT uSize1 = PidlGetSize(pidl1);
    UINT uSize2 = PidlGetSize(pidl2);

    if (uSize1 != uSize2)
        return bResult;

    int nRet = memcmp(pidl1, pidl2, uSize1);

    return nRet == 0;
}

LPITEMIDLIST PidlConcatPidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2)
{
    UINT cb1 = 0;
    if (pidl1 != NULL)   // May be NULL
        cb1 = PidlGetSize(pidl1) - sizeof(pidl1->mkid.cb);

    UINT cb2 = PidlGetSize(pidl2);

    LPITEMIDLIST pidlNew = (LPITEMIDLIST)CPidl::AllocBuffer(cb1 + cb2);
    if (pidlNew != NULL)
    {
        if (pidl1 != NULL)
            memcpy(pidlNew, pidl1, cb1);

        memcpy(((LPSTR)pidlNew) + cb1, pidl2, cb2);
    }

    return pidlNew;
}

LPITEMIDLIST PidlNext(LPCITEMIDLIST pidl)
{
    // Check for valid pidl.
    if(pidl == NULL)
        return NULL;

    // Get the size of the specified item identifier. 
    int cb = pidl->mkid.cb; 

    // If the size is zero, it is the end of the list. 
    if (cb == 0) 
        return NULL; 

    // Add cb to pidl (casting to increment by bytes). 
    pidl = (LPITEMIDLIST) (((LPBYTE) pidl) + cb); 

    // Return NULL if it is null-terminating, or a pidl otherwise. 
    return (LPITEMIDLIST) pidl; 
}

UINT PidlGetSize(LPCITEMIDLIST pidl)
{
    UINT cbTotal = 0;
    if (pidl != NULL)
    {
        cbTotal += sizeof(pidl->mkid.cb);   // Null terminator
        while (pidl->mkid.cb != NULL)
        {
            cbTotal += pidl->mkid.cb;
            pidl = PidlNext(pidl);
        }
    }

    return cbTotal;
}

LPITEMIDLIST PidlClone(LPCITEMIDLIST pidl, int cb /*=-1*/) 
{ 
    if( cb == -1 )
        cb = PidlGetSize(pidl); 

    //	Allocate a new item identifier list. 
    LPITEMIDLIST pidlNew = (LPITEMIDLIST) CPidl::AllocBuffer(cb + sizeof(USHORT)); 
    if(pidlNew == NULL) return NULL; 

    //	Copy the specified item identifier. 
    CopyMemory(pidlNew, pidl, cb); 

    //	Append a terminating zero. 
    *((USHORT *)(((LPBYTE) pidlNew) + cb)) = 0; 

    return pidlNew; 
}

LPITEMIDLIST GetItemIDFromPath(LPCTSTR pszPath)
{
    HRESULT       hr;
    LPITEMIDLIST  pidl;
    ULONG         chEaten;
    ULONG         dwAttributes;
    OLECHAR		  olePath[_MAX_PATH];
    
    IShellFolder* piDesktopFolder = NULL;
    ::SHGetDesktopFolder(&piDesktopFolder);
    if (piDesktopFolder == NULL)
        return NULL;

    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,
        CT2A(pszPath), -1, olePath, sizeof(olePath));

    hr = piDesktopFolder->ParseDisplayName(
        NULL,NULL, olePath, &chEaten, &pidl, &dwAttributes);

    piDesktopFolder->Release();

    if( FAILED(hr) ) 
        return NULL;

    return pidl;
}

LPITEMIDLIST PidlRemoveLastID(LPITEMIDLIST pidl)
{
    LPITEMIDLIST plast = PidlFindLastID(pidl);

    if (plast == NULL)
        return pidl;
    
    *((USHORT*)plast) = 0;

    return pidl;
}

LPITEMIDLIST PidlFindLastID(LPITEMIDLIST pidl)
{
    if (pidl == NULL)
        return NULL;

    LPITEMIDLIST plast = pidl;
    LPITEMIDLIST pnext = PidlNext(plast);

    while (pnext != NULL && pnext->mkid.cb != 0)
    {
        plast = pnext;
        pnext = PidlNext(pnext);
    }

    return plast;
}
