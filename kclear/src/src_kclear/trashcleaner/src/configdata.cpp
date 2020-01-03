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

// ConfigData.cpp: implementation of the CConfigData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "trashcleaner.h"
#include "ConfigData.h"
#include "3rdparty/hash/crc32.h"
#include "GetEnvPath.h"
#include "resource.h"
#include "misc/automemmanage.h"
#include "trashdefine.h"


//////////////////////////////////////////////////////////////////////////
DIRECTORYWORK::DIRECTORYWORK() :
bRecursive(FALSE),
bDefaultSelect(FALSE),
bRSelect(FALSE)
{}

DIRECTORYWORK::DIRECTORYWORK(const DIRECTORYWORK& cpy)
{
    bRecursive     = cpy.bRecursive;
    bDefaultSelect = cpy.bDefaultSelect ;
    bRSelect       = cpy.bRSelect;
    strFriendName  = cpy.strFriendName;
    iconindex      = cpy.iconindex;
    ulSize         = cpy.ulSize;
    id             = cpy.id;
    parent         = cpy.parent;
    bIsLeaf        = cpy.bIsLeaf;

    std::vector<CString>::const_iterator iter = cpy.strExtArray.begin();
    for(;iter != cpy.strExtArray.end();iter++)
        strExtArray.push_back(*iter);

    iter = cpy.strFilt.begin();
    for(;iter != cpy.strFilt.end();iter++)
        strFilt.push_back(*iter);

    iter = cpy.strDirectory.begin();
    for(;iter != cpy.strDirectory.end();iter++)
        strDirectory.push_back(*iter);
    
}

DIRECTORYWORK& DIRECTORYWORK::operator= (const DIRECTORYWORK& cpy)
{
    bRecursive     = cpy.bRecursive;
    bDefaultSelect = cpy.bDefaultSelect;
    strFriendName  = cpy.strFriendName;
    bRSelect       = cpy.bRSelect;
    iconindex      = cpy.iconindex;
    ulSize         = cpy.ulSize;
    id             = cpy.id;
    parent         = cpy.parent;
    bIsLeaf        = cpy.bIsLeaf;

    std::vector<CString>::const_iterator iter = cpy.strExtArray.begin();
    for(;iter != cpy.strExtArray.end();iter++)
        strExtArray.push_back(*iter);

    iter = cpy.strFilt.begin();
    for(;iter != cpy.strFilt.end();iter++)
        strFilt.push_back(*iter);

    iter = cpy.strDirectory.begin();
    for(;iter != cpy.strDirectory.end();iter++)
        strDirectory.push_back(*iter);
    return *this;
}

CConfigData::CConfigData()
{
    m_ConfigData.reserve(100);
}

CConfigData::~CConfigData()
{

}

int CConfigData::ReadConfig()
{
    _ReadConfigFile();
    return RC_SUCCESS;;
}

int CConfigData::ReadOneKeyConfig()
{

    dataCBro1.bRecursive = TRUE;
    dataCBro1.bDefaultSelect = TRUE;
    dataCBro1.bRSelect = dataCBro1.bDefaultSelect;
    dataCBro1.strDirectory.push_back(L"%CSIDL_INTERNET_CACHE%\\Content.IE5");
    dataCBro1.strDirectory.push_back(L"%CSIDL_INTERNET_CACHE%\\Content.MSO");
    dataCBro1.strDirectory.push_back(L"%CSIDL_INTERNET_CACHE%\\Content.Outlook");
    dataCBro1.strDirectory.push_back(L"%CSIDL_INTERNET_CACHE%\\Content.Word");
	dataCBro1.strDirectory.push_back(L"%CSIDL_INTERNET_CACHE%\\AntiPhishing");
	dataCBro1.strDirectory.push_back(L"%CSIDL_INTERNET_CACHE%\\Virtualized");
    dataCBro1.strFriendName = L"IE浏览器缓存";
    dataCBro1.strExtArray.push_back(L"*");
    dataCBro1.strFilt.push_back(L"%specfiles%");
	dataCBro1.id = WIN_TRASH;
    m_ConfigData.push_back(dataCBro1);

    dataCWin2.bRecursive = TRUE;
    dataCWin2.bDefaultSelect = TRUE;
    dataCWin2.bRSelect = dataCWin2.bDefaultSelect;
    dataCWin2.strDirectory.push_back(L"%windir%\\temp");
    dataCWin2.strFriendName = L"Windows 临时目录";
    dataCWin2.strExtArray.push_back(L"*");
    dataCWin2.strFilt.push_back(L"%specfiles%");
	dataCWin2.id = WIN_TEMP;
    m_ConfigData.push_back(dataCWin2);

	dataCWin9.bRecursive = TRUE;
	dataCWin9.bDefaultSelect = TRUE;
	dataCWin9.bRSelect = dataCWin9.bDefaultSelect;
	dataCWin9.strDirectory.clear();
	dataCWin9.strDirectory.push_back(L"%CSIDL_LOCAL_APPDATA%\\Microsoft\\Windows\\Explorer");
	dataCWin9.strFriendName = L"缩略图缓存";
	dataCWin9.strExtArray.clear();
	dataCWin9.strExtArray.push_back(L"db");
	dataCWin9.strFilt.clear();
	dataCWin9.strFilt.push_back(L"%specfiles%");
	dataCWin9.id = WIN_SUOLIETU;
	m_ConfigData.push_back(dataCWin9);

    dataCWin3.bRecursive = TRUE;
    dataCWin3.bDefaultSelect = TRUE;
    dataCWin3.bRSelect = dataCWin3.bDefaultSelect;
    dataCWin3.strDirectory.push_back(L"");
    dataCWin3.strFriendName = L"回收站";
    dataCWin3.strFilt.push_back(L"");
	dataCWin3.id = WIN_HUISHOUZHAN;
    m_ConfigData.push_back(dataCWin3);

    dataCWin4.bRecursive = TRUE;
    dataCWin4.bDefaultSelect = TRUE;
    dataCWin4.bRSelect = dataCWin4.bDefaultSelect;
    dataCWin4.strDirectory.push_back(L"%windir%\\SoftwareDistribution\\Download");
    dataCWin4.strFriendName = L"Windows 更新补丁";
    dataCWin4.strExtArray.push_back(L"*");
    dataCWin4.strFilt.push_back(L"%specfiles%");
	dataCWin4.id = WIN_UPDATE;
    m_ConfigData.push_back(dataCWin4);

    dataCWin5.bRecursive = TRUE;
    dataCWin5.bDefaultSelect = FALSE;
    dataCWin5.bRSelect = dataCWin5.bDefaultSelect;
    dataCWin5.strDirectory.push_back(L"%systemdrive%\\MSOCache");
    dataCWin5.strFriendName = L"Office 安装缓存目录";
    dataCWin5.strExtArray.push_back(L"*");
    dataCWin5.strFilt.push_back(L"%specfiles%");
	dataCWin5.id = WIN_OFFICE;
    m_ConfigData.push_back(dataCWin5);

    dataCWin6.bRecursive = TRUE;
    dataCWin6.bDefaultSelect = TRUE;
    dataCWin6.bRSelect = dataCWin6.bDefaultSelect;
    dataCWin6.strDirectory.push_back(L"%windir%\\Prefetch");
    dataCWin6.strFriendName = L"Windows 预读文件";
    dataCWin6.strExtArray.push_back(L"*");
    dataCWin6.strFilt.push_back(L"%specfiles%");
	dataCWin6.id = WIN_PREFETCH;
    m_ConfigData.push_back(dataCWin6);

    dataCWin7.bRecursive = TRUE;
    dataCWin7.bDefaultSelect = FALSE;
    dataCWin7.bRSelect = dataCWin7.bDefaultSelect;
    dataCWin7.strDirectory.push_back(L"%windir%\\Downloaded Program Files");
    dataCWin7.strFriendName = L"已下载程序文件";
    dataCWin7.strExtArray.push_back(L"*");
    dataCWin7.strFilt.push_back(L"%downloadlist%");
    dataCWin7.strFilt.push_back(L"%specfiles%");
	dataCWin7.id = WIN_DOWNLOAD;
    m_ConfigData.push_back(dataCWin7);

    dataCWin8.bRecursive = TRUE;
    dataCWin8.bDefaultSelect = TRUE;
    dataCWin8.bRSelect = dataCWin8.bDefaultSelect;
    dataCWin8.strDirectory.push_back(L"%TEMP%");
    dataCWin8.strFriendName = L"用户临时文件夹";
    dataCWin8.strExtArray.push_back(L"*");
    dataCWin8.strFilt.push_back(L"%windir%");
    dataCWin8.strFilt.push_back(L"%driver%");
    dataCWin8.strFilt.push_back(L"%program%");
    dataCWin8.strFilt.push_back(L"%specfiles%");
	dataCWin8.id = WIN_USERTEMP;
    m_ConfigData.push_back(dataCWin8);

	dataCWin10.bRecursive = TRUE;
	dataCWin10.bDefaultSelect = TRUE;
	dataCWin10.bRSelect = dataCWin10.bDefaultSelect;
	dataCWin10.strDirectory.clear();
	dataCWin10.strDirectory.push_back(L"%archivefiles%");
	dataCWin10.strDirectory.push_back(L"%queuefiles%");
	dataCWin10.strDirectory.push_back(L"%systemarchivefiles%");
	dataCWin10.strDirectory.push_back(L"%systemqueuefiles%");
	dataCWin10.strFriendName = L"Windows错误报告";
	dataCWin10.strExtArray.clear();
	dataCWin10.strExtArray.push_back(L"*");
	dataCWin10.strFilt.clear();
	dataCWin10.strFilt.push_back(L"%specfiles%");
	dataCWin10.id = WIN_ERRPORT;
	m_ConfigData.push_back(dataCWin10);

	dataCWin11.bRecursive = FALSE;
	dataCWin11.bDefaultSelect = TRUE;
	dataCWin11.bRSelect = dataCWin11.bDefaultSelect;
	dataCWin11.strDirectory.clear();
	dataCWin11.strDirectory.push_back(L"%windir%");
	dataCWin11.strDirectory.push_back(L"%windir%\\debug");
	dataCWin11.strDirectory.push_back(L"%windir%\\system32\\wbem");
	dataCWin11.strDirectory.push_back(L"%windir%\\security\\logs");
	dataCWin11.strFriendName = L"系统日志文件";
	dataCWin11.strExtArray.clear();
	dataCWin11.strExtArray.push_back(L"log");
	dataCWin11.strExtArray.push_back(L"lo_");
	dataCWin11.strExtArray.push_back(L"log.txt");
	dataCWin11.strExtArray.push_back(L"bak");
	dataCWin11.strExtArray.push_back(L"old");
	dataCWin11.strFilt.clear();
	dataCWin11.strFilt.push_back(L"%specfiles%");
	dataCWin11.id = WIN_LOG;
	m_ConfigData.push_back(dataCWin11);

    return RC_SUCCESS;;
}

int CConfigData::WriteConfig()
{
    return RC_FAILED;
}

BOOL CConfigData::ConvToLocatePath()
{
    if (m_ConfigData.size() <= 0)
        return FALSE;

    CGetEnvPath evnPath;


    for (int i = 0; i < m_ConfigData.size(); i++)
    {
        DIRECTORYWORK& dirword = m_ConfigData[i];


        for(int iIndex=0;iIndex<dirword.strDirectory.size();iIndex++)
        {
            CString strPath = dirword.strDirectory.at(iIndex);
            strPath = evnPath.GetRealPath(strPath);
            strPath.TrimLeft();
            strPath.TrimRight();
            dirword.strDirectory.at(iIndex) = strPath;
        }
   
    }

    return TRUE;
}


CConfigData::DirWorkArray& CConfigData::GetConfigData()
{
    return m_ConfigData;
}

const CConfigData::DirWorkArray& CConfigData::GetConfigData() const
{
    return m_ConfigData;
}

int CConfigData::_ReadConfigFile()
{
    dataPWin.bRecursive = FALSE;
    dataPWin.bDefaultSelect = FALSE;
    dataPWin.bRSelect = dataPWin.bDefaultSelect;
    dataPWin.strFriendName = L"Windows系统垃圾文件";
    dataPWin.iconindex = 5;
    dataPWin.id = WIN_TRASH;
    dataPWin.parent = NONE;
    dataPWin.bIsLeaf = FALSE;
    m_ConfigData.push_back(dataPWin);

    

    

    dataCWin2.bRecursive = TRUE;
    dataCWin2.bDefaultSelect = TRUE;
    dataCWin2.bRSelect = dataCWin2.bDefaultSelect;
    dataCWin2.strDirectory.push_back(L"%windir%\\temp");
    dataCWin2.strFriendName = L"Windows 临时目录";
    dataCWin2.strExtArray.push_back(L"*");
    dataCWin2.strFilt.push_back(L"%specfiles%");
    dataCWin2.iconindex = 5;
    dataCWin2.id = WIN_TEMP;
    dataCWin2.parent = WIN_TRASH;
    dataCWin2.bIsLeaf = TRUE;
    m_ConfigData.push_back(dataCWin2);

//      dataCWin3.bRecursive = TRUE;
//      dataCWin3.bDefaultSelect = TRUE;
//      dataCWin3.bRSelect = dataCWin3.bDefaultSelect;
//      dataCWin3.strDirectory.push_back(L"%CSIDL_PROGRAMS%");
//      dataCWin3.strFriendName = L"开始菜单无效的快捷方式";
//      dataCWin3.strExtArray.push_back(L"lnk");
//      dataCWin3.strFilt.push_back(L"%specfiles%");
//      dataCWin3.iconindex = 21;
//      dataCWin3.id = WIN_LNK;
//      dataCWin3.parent = WIN_TRASH;
//      dataCWin3.bIsLeaf = TRUE;
//      m_ConfigData.push_back(dataCWin3);

	dataCWin13.bRecursive = TRUE;
	dataCWin13.bDefaultSelect = TRUE;
	dataCWin13.bRSelect = dataCWin13.bDefaultSelect;
	dataCWin13.strDirectory.clear();
	dataCWin13.strDirectory.push_back(L"");
	dataCWin13.strFriendName = L"回收站";
	dataCWin13.strExtArray.clear();
	dataCWin13.strExtArray.push_back(L"");
	dataCWin13.strFilt.clear();
	dataCWin13.strFilt.push_back(L"");
	dataCWin13.iconindex = 30;
	dataCWin13.id = WIN_HUISHOUZHAN;
	dataCWin13.parent = WIN_TRASH;
	dataCWin13.bIsLeaf = TRUE;
	m_ConfigData.push_back(dataCWin13);
// 
 	 dataCWin3.bRecursive = TRUE;
 	 dataCWin3.bDefaultSelect = TRUE;
 	 dataCWin3.bRSelect = dataCWin3.bDefaultSelect;
 	 dataCWin3.strDirectory.clear();
     dataCWin3.strDirectory.push_back(L"%CSIDL_LOCAL_APPDATA%\\Microsoft\\Windows\\Explorer");
 	 dataCWin3.strFriendName = L"缩略图缓存";
 	 dataCWin3.strExtArray.clear();
 	 dataCWin3.strExtArray.push_back(L"db");
 	 dataCWin3.strFilt.clear();
 	 dataCWin3.strFilt.push_back(L"%specfiles%");
 	 dataCWin3.iconindex = 29;
 	 dataCWin3.id = WIN_SUOLIETU;
 	 dataCWin3.parent = WIN_TRASH;
 	 dataCWin3.bIsLeaf = TRUE;
 	 m_ConfigData.push_back(dataCWin3);

	 dataCWin3.bRecursive = FALSE;
	 dataCWin3.bDefaultSelect = FALSE;
	 dataCWin3.bRSelect = dataCWin3.bDefaultSelect;
	 dataCWin3.strDirectory.clear();
	 dataCWin3.strDirectory.push_back(L"%minidump%");
	 dataCWin3.strDirectory.push_back(L"%memdump%");
	 dataCWin3.strFriendName = L"内存转储文件";
	 dataCWin3.strExtArray.clear();
	 dataCWin3.strExtArray.push_back(L"dmp");
	 dataCWin3.strFilt.clear();
	 dataCWin3.strFilt.push_back(L"%specfiles%");
	 dataCWin3.iconindex = 22;
	 dataCWin3.id = WIN_DUMP;
	 dataCWin3.parent = WIN_TRASH;
	 dataCWin3.bIsLeaf = TRUE;
	 m_ConfigData.push_back(dataCWin3);

	 dataCWin1.bRecursive = TRUE;
	 dataCWin1.bDefaultSelect = TRUE;
	 dataCWin1.bRSelect = dataCWin1.bDefaultSelect;
	 dataCWin1.strDirectory.clear();
	 dataCWin1.strDirectory.push_back(L"%archivefiles%");
	 dataCWin1.strDirectory.push_back(L"%queuefiles%");
	 dataCWin1.strDirectory.push_back(L"%systemarchivefiles%");
	 dataCWin1.strDirectory.push_back(L"%systemqueuefiles%");
	 dataCWin1.strFriendName = L"Windows错误报告";
	 dataCWin1.strExtArray.clear();
	 dataCWin1.strExtArray.push_back(L"*");
	 dataCWin1.strFilt.clear();
	 dataCWin1.strFilt.push_back(L"%specfiles%");
	 dataCWin1.iconindex = 28;
	 dataCWin1.id = WIN_ERRPORT;
	 dataCWin1.parent = WIN_TRASH;
	 dataCWin1.bIsLeaf = TRUE;
	 m_ConfigData.push_back(dataCWin1);

	 dataCWin9.bRecursive = FALSE;
	 dataCWin9.bDefaultSelect = FALSE;
	 dataCWin9.bRSelect = dataCWin9.bDefaultSelect;
	 dataCWin9.strDirectory.clear();
	 dataCWin9.strDirectory.push_back(L"%windir%\\");
	 dataCWin9.strDirectory.push_back(L"%windir%\\debug");
	 dataCWin9.strDirectory.push_back(L"%windir%\\system32\\wbem");
	 dataCWin9.strDirectory.push_back(L"%windir%\\security\\logs");
	 dataCWin9.strFriendName = L"系统日志文件";
	 dataCWin9.strExtArray.clear();
	 dataCWin9.strExtArray.push_back(L"log");
	 dataCWin9.strExtArray.push_back(L"lo_");
	 dataCWin9.strExtArray.push_back(L"log.txt");
	 dataCWin9.strExtArray.push_back(L"bak");
	 dataCWin9.strExtArray.push_back(L"old");
	 dataCWin9.strFilt.clear();
	 dataCWin9.strFilt.push_back(L"%specfiles%");
	 dataCWin9.iconindex = 27;
	 dataCWin9.id = WIN_LOG;
	 dataCWin9.parent = WIN_TRASH;
	 dataCWin9.bIsLeaf = TRUE;
	 m_ConfigData.push_back(dataCWin9);

    dataCWin4.bRecursive = TRUE;
    dataCWin4.bDefaultSelect = TRUE;
    dataCWin4.bRSelect = dataCWin4.bDefaultSelect;
    dataCWin4.strDirectory.push_back(L"%windir%\\SoftwareDistribution\\Download");
    dataCWin4.strFriendName = L"Windows 更新补丁";
    dataCWin4.strExtArray.push_back(L"*");
    dataCWin4.strFilt.push_back(L"%specfiles%");
    dataCWin4.iconindex = 6;
    dataCWin4.id = WIN_UPDATE;
    dataCWin4.parent = WIN_TRASH;
    dataCWin4.bIsLeaf = TRUE;
    m_ConfigData.push_back(dataCWin4);


    dataCWin6.bRecursive = TRUE;
    dataCWin6.bDefaultSelect = TRUE;
    dataCWin6.bRSelect = dataCWin6.bDefaultSelect;
    dataCWin6.strDirectory.push_back(L"%windir%\\Prefetch");
    dataCWin6.strFriendName = L"Windows 预读文件";
    dataCWin6.strExtArray.push_back(L"*");
    dataCWin6.strFilt.push_back(L"%specfiles%");
    dataCWin6.iconindex = 4;
    dataCWin6.id = WIN_PREFETCH;
    dataCWin6.parent = WIN_TRASH;
    dataCWin6.bIsLeaf = TRUE;
    m_ConfigData.push_back(dataCWin6);

    dataCWin8.bRecursive = TRUE;
    dataCWin8.bDefaultSelect = TRUE;
    dataCWin8.bRSelect = dataCWin8.bDefaultSelect;
    dataCWin8.strDirectory.push_back(L"%TEMP%");
    dataCWin8.strFriendName = L"用户临时文件夹";
    dataCWin8.strExtArray.push_back(L"*");
    dataCWin8.strFilt.push_back(L"%windir%");
    dataCWin8.strFilt.push_back(L"%driver%");
    dataCWin8.strFilt.push_back(L"%program%");
    dataCWin8.strFilt.push_back(L"%specfiles%");
    dataCWin8.iconindex = 7;
    dataCWin8.id = WIN_USERTEMP;
    dataCWin8.parent = WIN_TRASH;
    dataCWin8.bIsLeaf = TRUE;
    m_ConfigData.push_back(dataCWin8);

    dataCWin7.bRecursive = TRUE;
    dataCWin7.bDefaultSelect = FALSE;
    dataCWin7.bRSelect = dataCWin7.bDefaultSelect;
    dataCWin7.strDirectory.push_back(L"%windir%\\Downloaded Program Files");
    dataCWin7.strFriendName = L"已下载程序文件";
    dataCWin7.strExtArray.push_back(L"*");
    dataCWin7.strFilt.push_back(L"%downloadlist%");
    dataCWin7.strFilt.push_back(L"%specfiles%");
    dataCWin7.iconindex = 1;
    dataCWin7.id = WIN_DOWNLOAD;
    dataCWin7.parent = WIN_TRASH;
    dataCWin7.bIsLeaf = TRUE;
    m_ConfigData.push_back(dataCWin7);

    dataCWin5.bRecursive = TRUE;
    dataCWin5.bDefaultSelect = FALSE;
    dataCWin5.bRSelect = dataCWin5.bDefaultSelect;
    dataCWin5.strDirectory.push_back(L"%systemdrive%\\MSOCache");
    dataCWin5.strFriendName = L"Office 安装缓存目录";
    dataCWin5.strExtArray.push_back(L"*");
    dataCWin5.strFilt.push_back(L"%specfiles%");
    dataCWin5.iconindex = 0;
    dataCWin5.id = WIN_OFFICE;
    dataCWin5.parent = WIN_TRASH;
    dataCWin5.bIsLeaf = TRUE;
    m_ConfigData.push_back(dataCWin5);


    dataPSYS.bRecursive = TRUE;
    dataPSYS.bDefaultSelect = FALSE;
    dataPSYS.bRSelect = dataPSYS.bDefaultSelect;
    dataPSYS.strFriendName = L"浏览器缓存目录";
    dataPSYS.iconindex = 9;
    dataPSYS.id = BROWER_TRASH;
    dataPSYS.parent = NONE;
    dataPSYS.bIsLeaf = FALSE;
    m_ConfigData.push_back(dataPSYS);


    dataCBro1.bRecursive = TRUE;
    dataCBro1.bDefaultSelect = TRUE;
    dataCBro1.bRSelect = dataCBro1.bDefaultSelect;
    dataCBro1.strDirectory.push_back(L"%CSIDL_INTERNET_CACHE%\\Content.IE5");
    dataCBro1.strDirectory.push_back(L"%CSIDL_INTERNET_CACHE%\\Content.MSO");
    dataCBro1.strDirectory.push_back(L"%CSIDL_INTERNET_CACHE%\\Content.Outlook");
    dataCBro1.strDirectory.push_back(L"%CSIDL_INTERNET_CACHE%\\Content.Word");
	dataCBro1.strDirectory.push_back(L"%CSIDL_INTERNET_CACHE%\\AntiPhishing");
	dataCBro1.strDirectory.push_back(L"%CSIDL_INTERNET_CACHE%\\Virtualized");
	dataCBro1.strDirectory.push_back(L"%CSIDL_INTERNET_CACHE%\\Low");
	dataCBro1.strFriendName = L"IE\\360\\腾讯TT\\傲游\\世界之窗\\搜狗浏览器";
    dataCBro1.strExtArray.push_back(L"*");
    dataCBro1.strFilt.push_back(L"%specfiles%");
    dataCBro1.iconindex = 3;
    dataCBro1.id = BROWER_IE;
    dataCBro1.parent = BROWER_TRASH;
    dataCBro1.bIsLeaf = TRUE;
    m_ConfigData.push_back(dataCBro1);

	dataCBro5.bRecursive = TRUE;
	dataCBro5.bDefaultSelect = FALSE;
	dataCBro5.bRSelect = dataCBro5.bDefaultSelect;
	dataCBro5.strDirectory.push_back(L"%firefox%");
	dataCBro5.strFriendName = L"火狐（Firefox）";
	dataCBro5.strExtArray.push_back(L"*");
	dataCBro5.strFilt.push_back(L"%specfiles%");
	dataCBro5.iconindex = 26;
	dataCBro5.id = BROWER_FIREFOX;
	dataCBro5.parent = BROWER_TRASH;
	dataCBro5.bIsLeaf = TRUE;
	m_ConfigData.push_back(dataCBro5);

	dataCBro4.bRecursive = TRUE;
	dataCBro4.bDefaultSelect = FALSE;
	dataCBro4.bRSelect = dataCBro4.bDefaultSelect;
	dataCBro4.strDirectory.push_back(L"%CSIDL_APPDATA%\\Maxthon3\\Temp\\Webkit\\Cache");
	dataCBro4.strFriendName = L"傲游3（极速模式）";
	dataCBro4.strExtArray.push_back(L"*");
	dataCBro4.strFilt.push_back(L"%specfiles%");
	dataCBro4.iconindex = 23;
	dataCBro4.id = BROWER_MATHRON;
	dataCBro4.parent = BROWER_TRASH;
	dataCBro4.bIsLeaf = TRUE;
	m_ConfigData.push_back(dataCBro4);

	dataCBro3.bRecursive = TRUE;
	dataCBro3.bDefaultSelect = FALSE;
	dataCBro3.bRSelect = dataCBro3.bDefaultSelect;
	dataCBro3.strDirectory.push_back(L"%CSIDL_APPDATA%\\SogouExplorer\\Webkit\\Cache");
	dataCBro3.strFriendName = L"搜狗浏览器（高速模式）";
	dataCBro3.strExtArray.push_back(L"*");
	dataCBro3.strFilt.push_back(L"%specfiles%");
	dataCBro3.iconindex = 25;
	dataCBro3.id = BROWER_SOGO;
	dataCBro3.parent = BROWER_TRASH;
	dataCBro3.bIsLeaf = TRUE;
	m_ConfigData.push_back(dataCBro3);

	dataCBro2.bRecursive = TRUE;
	dataCBro2.bDefaultSelect = FALSE;
	dataCBro2.bRSelect = dataCBro2.bDefaultSelect;
	dataCBro2.strDirectory.push_back(L"%CSIDL_LOCAL_APPDATA%\\Opera\\Opera\\opcache");
	dataCBro2.strDirectory.push_back(L"%CSIDL_LOCAL_APPDATA%\\Opera\\Opera\\cache");
	dataCBro2.strFriendName = L"Opera";
	dataCBro2.strExtArray.push_back(L"*");
	dataCBro2.strFilt.push_back(L"%specfiles%");
	dataCBro2.iconindex = 24;
	dataCBro2.id = BROWER_OPERA;
	dataCBro2.parent = BROWER_TRASH;
	dataCBro2.bIsLeaf = TRUE;
	m_ConfigData.push_back(dataCBro2);

    dataCBro6.bRecursive = TRUE;
    dataCBro6.bDefaultSelect = FALSE;
    dataCBro6.bRSelect = dataCBro6.bDefaultSelect;
    dataCBro6.strDirectory.push_back(L"%CSIDL_LOCAL_APPDATA%\\Google\\Chrome\\User Data\\Default\\cache");
    dataCBro6.strFriendName = L"谷歌(Chrome)";
    dataCBro6.strExtArray.push_back(L"*");
    dataCBro6.strFilt.push_back(L"%specfiles%");
    dataCBro6.iconindex = 10;
    dataCBro6.id = BROWER_CHROME;
    dataCBro6.parent = BROWER_TRASH;
    dataCBro6.bIsLeaf = TRUE;
    m_ConfigData.push_back(dataCBro6);

    dataPSYS.bRecursive = TRUE;
    dataPSYS.bDefaultSelect = FALSE;
    dataPSYS.bRSelect = dataPSYS.bDefaultSelect;
    dataPSYS.strFriendName = L"视频播放缓存目录";
    dataPSYS.iconindex = 11;
    dataPSYS.id = VIDEO_TRASH;
    dataPSYS.parent = NONE;
    dataPSYS.bIsLeaf = FALSE;
    m_ConfigData.push_back(dataPSYS);

    dataCVdo1.bRecursive = TRUE;
    dataCVdo1.bDefaultSelect = FALSE;
    dataCVdo1.bRSelect = dataCVdo1.bDefaultSelect;
    dataCVdo1.strDirectory.push_back(L"%tudou%");
    dataCVdo1.strFriendName = L"飞速土豆";
    dataCVdo1.strExtArray.push_back(L"tdd!");
    dataCVdo1.strFilt.push_back(L"%specfiles%");
    dataCVdo1.iconindex = 17;
    dataCVdo1.id = VIDEO_TUDOU;
    dataCVdo1.parent = VIDEO_TRASH;
    dataCVdo1.bIsLeaf = TRUE;
    m_ConfigData.push_back(dataCVdo1);

    dataCVdo3.bRecursive = TRUE;
    dataCVdo3.bDefaultSelect = FALSE;
    dataCVdo3.bRSelect = dataCVdo3.bDefaultSelect;
    dataCVdo3.strDirectory.push_back(L"%xunleikankan%");
    dataCVdo3.strFriendName = L"迅雷看看";
    dataCVdo3.strExtArray.push_back(L"cfg");
    dataCVdo3.strExtArray.push_back(L"dat");
    dataCVdo3.strFilt.push_back(L"%specfiles%");
    dataCVdo3.iconindex = 18;
    dataCVdo3.id = VIDEO_XUNLEI;
    dataCVdo3.parent = VIDEO_TRASH;
    dataCVdo3.bIsLeaf = TRUE;
    m_ConfigData.push_back(dataCVdo3);

    dataCVdo4.bRecursive = TRUE;
    dataCVdo4.bDefaultSelect = FALSE;
    dataCVdo4.bRSelect = dataCVdo4.bDefaultSelect;
    dataCVdo4.strDirectory.push_back(L"%youku%");
    dataCVdo4.strFriendName = L"优酷(iku)加速器";
    dataCVdo4.strExtArray.push_back(L"dat");
    dataCVdo4.strFilt.push_back(L"%specfiles%");
    dataCVdo4.iconindex = 19;
    dataCVdo4.id = VIDEO_YOUKU;
    dataCVdo4.parent = VIDEO_TRASH;
    dataCVdo4.bIsLeaf = TRUE;
    m_ConfigData.push_back(dataCVdo4);

    dataCVdo5.bRecursive = TRUE;
    dataCVdo5.bDefaultSelect = FALSE;
    dataCVdo5.bRSelect = dataCVdo5.bDefaultSelect;
    dataCVdo5.strDirectory.push_back(L"%ku6%");
    dataCVdo5.strFriendName = L"极速酷6";
    dataCVdo5.strExtArray.push_back(L"k6p");
    dataCVdo5.strFilt.push_back(L"%specfiles%");
    dataCVdo5.iconindex = 12;
    dataCVdo5.id = VIDEO_KU6;
    dataCVdo5.parent = VIDEO_TRASH;
    dataCVdo5.bIsLeaf = TRUE;
    m_ConfigData.push_back(dataCVdo5);

    dataCVdo7.bRecursive = TRUE;
    dataCVdo7.bDefaultSelect = FALSE;
    dataCVdo7.bRSelect = dataCVdo7.bDefaultSelect;
    dataCVdo7.strDirectory.push_back(L"%pptv%");
    dataCVdo7.strFriendName = L"PPTV网络电视";
    dataCVdo7.strFilt.push_back(L"%specfiles%");
    dataCVdo7.iconindex = 14;
    dataCVdo7.id = VIDEO_PPTV;
    dataCVdo7.parent = VIDEO_TRASH;
    dataCVdo7.bIsLeaf = TRUE;
    m_ConfigData.push_back(dataCVdo7);

    dataCVdo8.bRecursive = TRUE;
    dataCVdo8.bDefaultSelect = FALSE;
    dataCVdo8.bRSelect = dataCVdo8.bDefaultSelect;
    dataCVdo8.strDirectory.push_back(L"%fengxing%");
    dataCVdo8.strFriendName = L"风行网络电视";
    dataCVdo8.strFilt.push_back(L"%specfiles%");
    dataCVdo8.iconindex = 20;
    dataCVdo8.id = VIDEO_FENGXING;
    dataCVdo8.parent = VIDEO_TRASH;
    dataCVdo8.bIsLeaf = TRUE;
    m_ConfigData.push_back(dataCVdo8);

    dataCVdo9.bRecursive = TRUE;
    dataCVdo9.bDefaultSelect = FALSE;
    dataCVdo9.bRSelect = dataCVdo9.bDefaultSelect;
    dataCVdo9.strDirectory.push_back(L"%qqlive%");
    dataCVdo9.strFriendName = L"QQLive网络电视";
    dataCVdo9.strExtArray.push_back(L"tdl");
    dataCVdo9.strExtArray.push_back(L"tdi");
    dataCVdo9.strExtArray.push_back(L"tii");
    dataCVdo9.strExtArray.push_back(L"tpt");
    dataCVdo9.strFilt.push_back(L"%specfiles%");
    dataCVdo9.iconindex = 15;
    dataCVdo9.id = VIDEO_QQLIVE;
    dataCVdo9.parent = VIDEO_TRASH;
    dataCVdo9.bIsLeaf = TRUE;
    m_ConfigData.push_back(dataCVdo9);

    dataPSYS.bRecursive = TRUE;
    dataPSYS.bDefaultSelect = FALSE;
    dataPSYS.bRSelect = dataPSYS.bDefaultSelect;
    dataPSYS.strDirectory.push_back(L"%systemdrive%");
    dataPSYS.strFriendName = L"系统盘垃圾文件深度搜索";
    dataPSYS.strExtArray.push_back(L"tmp");
    dataPSYS.strExtArray.push_back(L"_mp");
    dataPSYS.strExtArray.push_back(L"gid");
    dataPSYS.strExtArray.push_back(L"chk");
	dataPSYS.strExtArray.push_back(L"??$");
	dataPSYS.strExtArray.push_back(L"^*");
	dataPSYS.strExtArray.push_back(L"temp");
	dataPSYS.strExtArray.push_back(L"~mp");
	dataPSYS.strExtArray.push_back(L"$*");
	dataPSYS.strExtArray.push_back(L"___");
	dataPSYS.strExtArray.push_back(L"$$$");
	dataPSYS.strExtArray.push_back(L"@@@");
	dataPSYS.strExtArray.push_back(L"---");
	dataPSYS.strExtArray.push_back(L"ftg");
	dataPSYS.strExtArray.push_back(L"%%%");
	dataPSYS.strExtArray.push_back(L"fts");
	dataPSYS.strExtArray.push_back(L"prv");
	dataPSYS.strExtArray.push_back(L"fnd");
	dataPSYS.strExtArray.push_back(L"err");
	dataPSYS.strExtArray.push_back(L"diz");
	dataPSYS.strExtArray.push_back(L"mch");
	dataPSYS.strExtArray.push_back(L"mscreate.dir");
	dataPSYS.strExtArray.push_back(L"imagedb.aid");
	dataPSYS.strExtArray.push_back(L"errorlog.txt");
	dataPSYS.strExtArray.push_back(L"comsetup.txt");
	dataPSYS.strExtArray.push_back(L"directx.txt");
	dataPSYS.strExtArray.push_back(L"mpcsetup.txt");
	dataPSYS.strExtArray.push_back(L"netsetup.txt");
	dataPSYS.strExtArray.push_back(L"thumbs.db");
    dataPSYS.strFilt.push_back(L"%specfiles%");
    dataPSYS.iconindex = 2;
    dataPSYS.id = SYS_TRASH;
    dataPSYS.parent = NONE;
    dataPSYS.bIsLeaf = TRUE;
    m_ConfigData.push_back(dataPSYS);




    return TRUE;
}

int CConfigData::_WriteConfigFile()
{
    int nResult = TRUE;
    
    return nResult;
}

BOOL CConfigData::InitFiltDir()
{
   

    CGetEnvPath evnPath;

    unsigned int uFilterCount = sizeof(g_pszFilterDirs) / sizeof(LPCTSTR);

    for (int i = 0; i < uFilterCount; i++)
    {
        CString strPath;
        strPath = evnPath.GetRealPath(g_pszFilterDirs[i].GetBuffer());
        g_pszFilterDirs[i] = strPath.GetBuffer();
        
    }

    return TRUE;
}
