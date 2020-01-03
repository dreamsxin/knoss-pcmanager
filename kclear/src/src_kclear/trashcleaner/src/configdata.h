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

// ConfigData.h: interface for the CConfigData class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class DIRECTORYWORK
{
public:
    std::vector<CString> strDirectory;
    CString strFriendName;
    std::vector<CString> strExtArray;
    std::vector<CString> strFilt;
    BOOL bRecursive;     // 是否查找子目录
    BOOL bDefaultSelect;
    BOOL bRSelect;//推荐选项
    int  iconindex;
    ULONGLONG ulSize;
    int id;
    int parent;
    BOOL bIsLeaf;

public: 
    DIRECTORYWORK();

    DIRECTORYWORK(const DIRECTORYWORK& cpy);

    DIRECTORYWORK& operator= (const DIRECTORYWORK& cpy);

};

class CConfigData  
{
public:

    CConfigData();

    virtual ~CConfigData();

public:

    typedef std::vector<DIRECTORYWORK> DirWorkArray;


    virtual int ReadConfig();
    virtual int WriteConfig();
    int ReadOneKeyConfig();

    BOOL ConvToLocatePath();
    BOOL InitFiltDir();

    DirWorkArray& GetConfigData();
    const DirWorkArray& GetConfigData() const;

    DirWorkArray m_ConfigData;

    DIRECTORYWORK dataPWin;
	DIRECTORYWORK dataCWin1;
    DIRECTORYWORK dataCWin2;
    DIRECTORYWORK dataCWin3;
    DIRECTORYWORK dataCWin4;
    DIRECTORYWORK dataCWin5;
    DIRECTORYWORK dataCWin6;
    DIRECTORYWORK dataCWin7;
    DIRECTORYWORK dataCWin8;
	DIRECTORYWORK dataCWin9;
	DIRECTORYWORK dataCWin10;
	DIRECTORYWORK dataCWin11;
	DIRECTORYWORK dataCWin12;
	DIRECTORYWORK dataCWin13;

    DIRECTORYWORK dataPSYS;
    DIRECTORYWORK dataPVideo;

    DIRECTORYWORK dataCVdo1;
    DIRECTORYWORK dataCVdo2;
    DIRECTORYWORK dataCVdo3;
    DIRECTORYWORK dataCVdo4;
    DIRECTORYWORK dataCVdo5;
    DIRECTORYWORK dataCVdo6;
    DIRECTORYWORK dataCVdo7;
    DIRECTORYWORK dataCVdo8;
    DIRECTORYWORK dataCVdo9;


    DIRECTORYWORK dataPBrowser;

    DIRECTORYWORK dataCBro1;
    DIRECTORYWORK dataCBro2;
    DIRECTORYWORK dataCBro3;
    DIRECTORYWORK dataCBro4;
    DIRECTORYWORK dataCBro5;
    DIRECTORYWORK dataCBro6;


protected:
    int _ReadConfigFile();
    int _WriteConfigFile();
};



