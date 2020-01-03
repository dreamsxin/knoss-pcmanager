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

/************************************************************************
* @file      : kregcleantask.h
* @author    : zhaoxinxing <xinxing.zh@gmail.com>
* @date      : 2010/7/13
* @brief     : 
*
* $Id: $
/************************************************************************/
#ifndef __KTRASHCLEANTASK_H__
#define __KTRASHCLEANTASK_H__

#include "kclear/ionekeyclean.h"
#include "kclear/kclsiddef.h"
#include "common/kavcom.h"
#include "registrydata.h"
#include "ksclock.h"
#include "kbackupreg.h"

// -------------------------------------------------------------------------



class KRegCleanTask : public ICleanTask
{
public:
    KRegCleanTask();
    ~KRegCleanTask();

//     KIS_DEFINE_GETCLSID(CLSID_CLEAN);
// 
//     DECLARE_KISCOM_OBJECT();
// 
//     KISCOM_QUERY_BEGIN()
//         KISCOM_QUERY_ENTRY(ICleanTask)
//     KISCOM_QUERY_END()

public:

    //初始化
    HRESULT __stdcall Initialize();

    //反初始化
    HRESULT __stdcall Uninitialize();

    //设置回调
    void __stdcall SetCallback(ICleanCallback* pCallBack);

   //停止清除
    bool __stdcall StopClean();

    //开始清除
    bool __stdcall StartClean(int nTask,const char* szTask);

    void __stdcall Release()
    {
        delete this;
    }

    

private:
    void SelectCleanItem(int nTask,const char* szTask);
    bool SplitStrTask( 
        /*[in]*/  std::wstring str, 
        /*[out]*/ std::vector <std::wstring>& vcResult,
        /*[in]*/  char delim = ','
        );

    static DWORD WINAPI ScanThreadProc(LPVOID lpParameter);

    static bool	ScanCallback(CRegistryData::SCAN_RESULT* result, LPVOID lpParam);
    static bool	ScanTypeCallBack(CRegistryData::REG_SCAN_TYPE rstType, LPVOID lpParam);
    
    ICleanCallback* m_pCallBack;       //回调
    CRegistryData RegClean;
    bool m_bStop;			
    HANDLE m_hScanThread;	
    KLock m_lockTips;			
    KLock m_lockTree;	
    int	m_nRegistryCount;//扫描到的项
    typedef	std::vector<CRegistryData::SCAN_RESULT*>	VSR;
    typedef std::map<CRegistryData::REG_SCAN_TYPE, VSR>	MTRP;
    MTRP m_scanResultArray;
    typedef std::map<CRegistryData::REG_SCAN_TYPE, bool>	CLEAN_MAP;
    CLEAN_MAP m_CleanMap; 
    CKBackupReg *regBack;
    std::map<std::wstring , bool> TaskMap;
    std::vector<std::wstring> TaskKey;
    std::vector<std::wstring> TaskDesc;
};

bool __cdecl GetCleaner(const GUID& riid, void** ppv)
{
    bool bRet = false;
    KRegCleanTask *IRegClean = NULL;

    if (!ppv)
        goto Exit0;

    IRegClean = new KRegCleanTask;
    if (!IRegClean)
        goto Exit0;
   
    *ppv = IRegClean;
Exit0:
    return bRet;
}

// -------------------------------------------------------------------------
// $Log: $

#endif /* __CKSSCANPLUG_H__ */
