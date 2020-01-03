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


#ifndef __PUBLICSTRUCT_H__
#define __PUBLICSTRUCT_H__


typedef struct tagFIND_FILE_DATA
{
    LPCTSTR     pszFileNmae;    // 文件名
    LPCTSTR     pszPathName;    // 文件路径
    
    LPCTSTR     pszExtName;     // 扩展名
    ULONGLONG   uFileSize;      // 文件大小
	int         nCount;
    
}FINDFILEDATA, *LPFINDFILEDATA;

#endif //__PUBLICSTRUCT_H__