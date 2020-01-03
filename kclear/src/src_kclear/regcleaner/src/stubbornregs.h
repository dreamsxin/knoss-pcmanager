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

#pragma once


//////////////////////////////////////////////////////////////////////////
#include "kscbase/ksclock.h"


class CStubbornRegs
{
public:
    CStubbornRegs();
    ~CStubbornRegs();

    

    BOOL Init();
    void UnInit();

    static CStubbornRegs& Instance()
    {
        static CStubbornRegs SRegInst;
        return SRegInst;
    }

    void AddReg(const CStringW& strRegPath);
    BOOL Sync();

    BOOL IsStubbornReg(const CStringW& strRegPath);

private:
    BOOL _LoadFromFile();
    BOOL _SaveToFile();

private:
    CAtlMap<CStringW, BOOL> m_regMap;
    CAtlList<CStringW> m_regList;
    KLock klock;
};

//////////////////////////////////////////////////////////////////////////
