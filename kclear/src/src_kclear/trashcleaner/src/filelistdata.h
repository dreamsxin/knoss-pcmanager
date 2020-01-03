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

// FileListData.h: interface for the CFileListData class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


#include "DeleteFileTraverse.h"
#include "filestruct.h"
#include "trashdefine.h"

typedef struct tag_TRASHFILEITEM
{
    CString   strFileName;
    ULONGLONG ulFileSize;
    bool      bIsClear;
    bool      bIsVedio;//是否完整视频
    HTREEITEM hItem;
    int ParentDirId;

    tag_TRASHFILEITEM() : ulFileSize(0)
    {}
    
}TRASHFILEITEM;

typedef struct tag_TRASHFILELIST {
    
    std::vector<TRASHFILEITEM> itemArray;

    ULONGLONG ulAllFileSize;

	CString strfileList;

    tag_TRASHFILELIST() : ulAllFileSize(0)
    {
		strfileList = L"";
	}

    tag_TRASHFILELIST(const tag_TRASHFILELIST& cpy)
    {
       
        std::vector<tag_TRASHFILEITEM>::const_iterator iter;
        for (iter=cpy.itemArray.begin();iter!=cpy.itemArray.end();iter++)
            itemArray.push_back(*iter);
        ulAllFileSize = cpy.ulAllFileSize;
		strfileList = cpy.strfileList;
    }

    const tag_TRASHFILELIST& operator = (const tag_TRASHFILELIST& cpy)
    {
       
        std::vector<tag_TRASHFILEITEM>::const_iterator iter;
        for (iter=cpy.itemArray.begin();iter!=cpy.itemArray.end();iter++)
            itemArray.push_back(*iter);
        ulAllFileSize = cpy.ulAllFileSize;
		strfileList = cpy.strfileList;
        return *this;
    }

}TRASHFILELIST;

class CFileListData  
{
public:

	CFileListData();

	virtual ~CFileListData();

    __int64 AddTrashFile(int nIndex, LPFINDFILEDATA pFileData);
    
public:
    std::vector<TRASHFILELIST> m_itemArray;
    __int64 m_nTotalsize;
};

