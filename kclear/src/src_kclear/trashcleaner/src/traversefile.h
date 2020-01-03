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
* CreatedOn: 2007-1-16   18:31
* FileName: TraverseFile.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#ifndef __TRAVERSEFILE_H__
#define __TRAVERSEFILE_H__

typedef struct tagFIND_FILE_DATA   FINDFILEDATA, *LPFINDFILEDATA;


class ITraverseFile
{
public:
    virtual void TraverseFile(LPFINDFILEDATA pFileData) = 0;

    virtual void TraverseProcess(unsigned long uCurPos) = 0;

    virtual void TraverseProcessEnd(unsigned long uCurPos,CString videoPath) = 0;

    virtual void TraverseFinished() = 0;
};


#endif //__TRAVERSEFILE_H__