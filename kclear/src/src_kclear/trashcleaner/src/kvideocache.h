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

#ifndef _KVIDEOCACHE_H_
#define _KVIDEOCACHE_H_
#include <Shlwapi.h>
#include "kregfunction.h"
#include "IniEditor.h"
#include "traversefile.h"
#include "filestruct.h"
#include "trashdefine.h"

#pragma comment(lib, "shlwapi.lib")

CString GetDrive(CString suffix);
DWORD GetDiffDaysEx( SYSTEMTIME local,  FILETIME ftime );
DWORD FileTimeInnerEx(FILETIME tCurFile);
void FileTimeToTime_tEx( FILETIME ft, time_t *t );
BOOL ScanFileBasedPathAndName(CString strItem,ITraverseFile *pTF, const CString strPath, const CString strName,  int nStopIndex, BOOL bTime = FALSE, DWORD dwTime = 0);

BOOL _ScanTudouCache(ITraverseFile *pTF, CString& strPath, int nStopIndex);

BOOL _ScanKuaiboCache(ITraverseFile *pTF, CString& strPath, int nStopIndex);

BOOL _ScanKankanCache(ITraverseFile *pTF, CString& strPath, int nStopIndex);
BOOL ScanKankanCacheEx(ITraverseFile *pTF, const CString strPath, int nStopIndex, int nFlag);

BOOL _ScanYoukuCache(ITraverseFile *pTF, CString& strPath, int nStopIndex);

BOOL _ScanKu6Cache(ITraverseFile *pTF, CString& strPath, int nStopIndex);
BOOL ScanKu6CacheEx(ITraverseFile *pTF, const CString strPath, int nStopIndex);

BOOL _ScanPPTVCache(ITraverseFile *pTF, CString& strPath, int nStopIndex);

BOOL _ScanFengxingCache(ITraverseFile *pTF, CString& strPath, int nStopIndex);

BOOL ScanFengxingCacheEx(ITraverseFile *pTF, const CString strPath, int nStopIndex);

BOOL _ScanQQLiveCache(ITraverseFile *pTF, CString& strPath, int nStopIndex);

BOOL ScanQQLiveCacheEx(ITraverseFile *pTF, CString& strPath, int nStopIndex, BOOL bTime = FALSE, DWORD dwTime = 0);

#endif