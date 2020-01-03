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

#include "StdAfx.h"
#include "filtertool.h"

CFilterTool::CFilterTool(void)
{
    m_bInitFlag = FALSE;
    m_FilterFuncMap.clear();
}

CFilterTool::~CFilterTool(void)
{
    Uninit();
    m_bInitFlag = TRUE;
}

CFilterTool& CFilterTool::Instance()
{
    static CFilterTool ToolInst;
    ToolInst.Init();
    return ToolInst;

}

BOOL CFilterTool::Init()
{
    BOOL bRet = FALSE;
    if (m_bInitFlag)
    {
        goto Exit0;
    }
   
    _DoMakeFuncMap(L"%windir%",&CFilterTool::CheckWindowsPath);
    _DoMakeFuncMap(L"%driver%",&CFilterTool::CheckDrivePath);
    _DoMakeFuncMap(L"%program%",&CFilterTool::CheckProgramPath);
    _DoMakeFuncMap(L"%downloadlist%",&CFilterTool::CheckDownloadProPath);
    _DoMakeFuncMap(L"%specfiles%",&CFilterTool::CheckSpecFile);

    

    m_bInitFlag = TRUE;

Exit0:
    bRet = TRUE;
    return bRet;
}

BOOL CFilterTool::Uninit()
{
    _DoClearAllFuncMap();
    return TRUE;
}


BOOL CFilterTool::_DoMakeFuncMap(CString strFuncName,FilterOper pHandle)
{
    BOOL bRet = FALSE;

    if (pHandle)
    {
        assert(pHandle);

        m_FilterFuncMap[strFuncName] = pHandle;

        bRet = TRUE;
    }

    return bRet;

}


BOOL CFilterTool::_DoFindFuncMap(CString& strFuncName,CString strFileName)
{
    BOOL bRet = FALSE;

    std::map<CString, FilterOper>::iterator iter;
    
    FilterOper FOper;
    iter = m_FilterFuncMap.find(strFuncName);
    if( iter != m_FilterFuncMap.end())
    { 
        FOper = iter->second;
        if((this->*FOper)(strFileName))
            bRet = TRUE;
        
    }



    return bRet;

}


BOOL CFilterTool::_DoClearAllFuncMap()
{
    m_FilterFuncMap.clear();
    return TRUE;
}

BOOL CFilterTool::CheckWindowsPath(CString strFilePath)
{
    BOOL bRet = FALSE;
    CString strWindir = L"";

    GetWindowsDir(strWindir);
    strWindir.MakeLower();
    strFilePath.MakeLower();
   

    int nPos = strFilePath.Find(strWindir);

    if (nPos!=-1)
    {
       bRet = TRUE;   
    }

    return bRet;
}

BOOL CFilterTool::CheckDrivePath(CString strFilePath)
{
    BOOL bRet = FALSE;
    strFilePath.MakeLower();

    ::PathRemoveFileSpec(strFilePath.GetBuffer());
    
    if (strFilePath.GetLength()<=3)
    {
       bRet = TRUE;
    }

    return bRet;
}

BOOL CFilterTool::CheckProgramPath(CString strFilePath)
{
    BOOL bRet = FALSE;
    CString strSysdrive = L"";

    strFilePath.MakeLower();

    GetSystemDrive(strSysdrive);
    strSysdrive.MakeLower();
    strSysdrive += L"\\program files";

    if (strFilePath.Find(strSysdrive)!=-1)
    {
        bRet = TRUE;
    }

    return bRet;
}

BOOL CFilterTool::CheckDownloadProPath(CString strFilePath)
{
    BOOL bRet = FALSE;

    strFilePath.MakeLower();
    CString strFileExt = GetFileExtName(strFilePath);
    CString strFileName = strFilePath.Mid(strFilePath.ReverseFind(L'\\')+1);


    if(Filter(strFileName,g_pszDownloadFiles,8))
    {
        bRet = TRUE;
    }

    return bRet;
}

BOOL CFilterTool::CheckSpecFile(CString strFilePath)
{
    BOOL bRet = FALSE;

    strFilePath.MakeLower();

    CString strFileExt = GetFileExtName(strFilePath);
    CString strFileName = strFilePath.Mid(strFilePath.ReverseFind(L'\\')+1);

    if(Filter(strFileName,g_pszSpecFiles,9))
    {
        bRet = TRUE;
    }

    return bRet;
}