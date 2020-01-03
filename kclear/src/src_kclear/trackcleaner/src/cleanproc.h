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

#ifndef _CLEANPROC_H_
#define _CLEANPROC_H_ 


#include <windows.h>
#include <atlstr.h>
#include <process.h>
#include <atlconv.h>
#include <atlbase.h>
#include <atlstr.h>
#include <SHLOBJ.H>
#include <shobjidl.h>
#include <atlcoll.h>
#include <shellapi.h>
#include <Tlhelp32.h>


#include "kclear/ICleanMgr.h"
#include "kclear/ICleanProc.h"
#include "3rdparty/tinyxml/tinyxml.h"


#define BEGIN	{
#define END		}


typedef struct _USERDATA
{
	int		iType;
	void*	pObject;
	void*	pObject2;
	void*	pObject3;

}USERDATA;

//typedef struct _SOFTWARE_INFO
//{
//	CString strTypeid;
//	CString strClearid;
//	CString strmethod;
//	CString strpath;
//	CString strpara;
//
//}SoftInfo, *PSoftInfo;


typedef struct  _SOFTWAREINFO
{
	int iType;
	CSimpleArray<SoftInfo> softInfo;

}SOFTWAREINFO,*PSOFTWAREINFO;

class CKClearProc :public IKClearProc
{
public:
	CKClearProc();
	~CKClearProc();
	
	//接口函数
	void DZ_SetCleanCallBack(ScanRegCallBack fnRegCallBack,ScanFileCallBack fnFileCallBack,void* pMain);
	void DZ_SetCleanCallBackError(ScanRegCallBackError fnRegCallBackError,ScanFileCallBackError fnFileCallBackError,void* pMainError);

	BOOL DZ_StartScan(int* iType,int iCount);
	BOOL DZ_StopScan();

	BOOL DZ_StartScanBage(GARBAINFO* garbInfo);
	BOOL DZ_StopScanBage();

	BOOL DZ_CleanReg(RegInfo* pRegInfo,int iCount);
	BOOL DZ_CleanFile(FileInfo* pFileInfo,int iCount);

	void DZ_IsScanClean(BOOL bClean = FALSE);
	
	BOOL DZ_ResetRegsiter(LPCTSTR strRegFilePath = NULL);

	BOOL DZ_GetNoinstallApp(int n);
	//子类函数
	//回调接口
	static BOOL __stdcall SoftInfoCallBack(void* pThis,SoftInfo softinfo);

public:
	static CAtlMap<int,SOFTWAREINFO> s_map_softwareinfo;
};

//判断进程是否运行，如进程正在运行，怎返回进程id
DWORD GetProcessIdFromName(LPCTSTR szProcessName);

DWORD GetCurrentProcessName();
#endif