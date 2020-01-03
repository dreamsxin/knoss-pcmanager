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

// FileListData.cpp: implementation of the CFileListData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "trashcleaner.h"
#include "FileListData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileListData::CFileListData()
{
    m_nTotalsize = 0;
}

CFileListData::~CFileListData()
{

}

__int64 CFileListData::AddTrashFile(int nIndex, LPFINDFILEDATA pFileData)
{
    
    TRASHFILEITEM item;

	WCHAR szFullPathName[MAX_PATH] = {0};
	wcscpy_s(szFullPathName,MAX_PATH, pFileData->pszPathName);
	::PathAppend(szFullPathName, pFileData->pszFileNmae);
	item.strFileName = szFullPathName;
   /* item.strFileName  = pFileData->pszPathName;
    item.strFileName += pFileData->pszFileNmae;*/

    __int64 int64FileSize = pFileData->uFileSize;

    if (int64FileSize > 0 && _wcsicmp(pFileData->pszPathName, L"Ë«»÷²é¿´ÏêÇé") != 0)
    {
        TCHAR  szRoot[4] = _T("C:\\");
        static DWORD m_dwBytesSector[128] = { 0 };

        szRoot[0] = item.strFileName[0];

        if (m_dwBytesSector[szRoot[0]] == 0 && 
            szRoot[0] < 128 && 
            szRoot[0] >= 0)
        {
            DWORD dwSectorsPerCluster     = 0;
            DWORD dwBytesPerSector        = 0;
            DWORD dwNumberOfFreeClusters  = 0;
            DWORD dwTotalNumberOfClusters = 0;

            BOOL bRet = GetDiskFreeSpace( szRoot, 
                &dwSectorsPerCluster, 
                &dwBytesPerSector,
                &dwNumberOfFreeClusters,
                &dwTotalNumberOfClusters);

            m_dwBytesSector[szRoot[0]] = 
                dwSectorsPerCluster * dwBytesPerSector;
        }

        if (m_dwBytesSector[szRoot[0]] != 0)
            int64FileSize = ( ((int64FileSize - 1) / 
                m_dwBytesSector[szRoot[0]] + 1) * 
                m_dwBytesSector[szRoot[0]] );
    }
    
    item.ulFileSize = int64FileSize;
    item.bIsClear = true;
    item.hItem = NULL;
    item.bIsVedio = false;
    
	if (m_itemArray.size() > nIndex)
	{    
		m_itemArray[nIndex].ulAllFileSize += int64FileSize;

		m_itemArray[nIndex].itemArray.push_back(item);
		m_itemArray[nIndex].strfileList += item.strFileName;
		m_itemArray[nIndex].strfileList += L"\n";

		m_nTotalsize += int64FileSize;
	}

    return int64FileSize;
}
