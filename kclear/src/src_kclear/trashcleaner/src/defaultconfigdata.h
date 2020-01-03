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

// DefaultConfigData.h: interface for the CDefaultConfigData class.
//
//////////////////////////////////////////////////////////////////////



#pragma once

#include "ConfigData.h"

class CDefaultConfigData : public CConfigData
{
public:
	CDefaultConfigData();
	virtual ~CDefaultConfigData();

    virtual int ReadConfig();
    virtual int WriteConfig();
    
public:
    static CString CDefaultConfigData::GetDefaultConfigFile();

    //CString m_strConfigFile;
};

