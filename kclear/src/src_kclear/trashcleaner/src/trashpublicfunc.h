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

#ifndef __PUBLICFUNC_H__
#define __PUBLICFUNC_H__

#include <vector>
#include "trashdefine.h"
#include "traversefile.h"
#include "ionekeyclean.h"
#include "filestruct.h"
#include "filelistdata.h"
#include "deletefiletraverse.h"
#include "trashcleaner.h"
#include <TlHelp32.h>


void SplitString(CString str, WCHAR split, std::vector <CString>& vcResult);

int KAEGetSystemVersion(/* [Out] */ SYSTEM_VERSION *pnSystemVersion);

CString KasLangFormatFileName( LPCTSTR pszName );

CString KasLangGetTrashConfigFilePath();

LPCTSTR GetFileSizeText(__int64 fNum);

CString GetExtName(CString fileName);

BOOL FindFileInDirectory(LPCTSTR pszFullPath,
                         const std::vector<CString>& extArray, 
                         int nRecursive, 
                         int nStopIndex,
                         ITraverseFile *pTF,
                         CFileListData *pFilelist,
                         int nCurrentIndex,
                         std::vector<CString> &vFilt,
						 CString);

BOOL FileFilter(LPCTSTR pszFileName);

BOOL DirFilter(LPCTSTR pszDirName);

BOOL FindFileWithExtInDirectory(LPCTSTR pszFullPath, 
                                LPCTSTR pszExt, 
                                int nStopIndex, 
                                ITraverseFile *pTF,
                                CFileListData *pFilelist,
                                int nCurrentIndex,
                                std::vector<CString> &vFilt,
								CString);

BOOL DeleteFileWithExtInDirectory(LPCTSTR pszFullPath, 
                                  LPCTSTR pszExt,
                                  int nStopIndex,
                                  CFileListData *pFilelist,
                                  int nCurIndex,
                                  ICleanCallback *pCcb,
                                  CFileDelete *pDelFile,
                                  std::wstring strDesc,
                                  std::vector<CString> &vFilt);

BOOL DeleteFileInDirectory(LPCTSTR pszFullPath,
                           const std::vector<CString>& extArray, 
                           int nRecursive,
                           int nStopIndex,
                           CFileListData *pFilelist,
                           int nCurIndex,
                           ICleanCallback *pCcb,
                           CFileDelete *pDelFile,
                           std::wstring strDesc,
                           std::vector<CString> &vFilt);

BOOL IsFileTimeInner(FILETIME tCurFile);

 DWORD GetProcessIdFromName(LPCTSTR szProcessName);

 BOOL IsExtCompare(std::wstring strFileName, std::wstring strExt);

//  HRESULT FindLinkArgument(const WCHAR *pLnkFilePath, 
// 	 BOOL &bHave);
// 
//  HRESULT FindLinkUrl(const WCHAR *pLnkFilePath, 
// 	 BOOL &bHave);

#endif 