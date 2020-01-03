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

#ifndef _REGCLEAN_H_
#define _REGCLEAN_H_

#define  DLL_API

#include "CleanProc.h"
#include "i_clean.h"

#include "winosver.h"
#include "fileopt.h"
#include "regopt.h"
#include "CmdLineParser.h"

/************************************************************************/
//功能:对注册表进行清理
//
/************************************************************************/

extern ScanFileCallBack g_fnScanFile;
extern ScanRegCallBack  g_fnScanReg;
extern void*			g_pMain;

class CRegClean:IClean
{
public:
	CRegClean();
	~CRegClean();
public:
	void	Start()
	{	
		m_bScan=TRUE; 
		m_fileOpt.UserBreadFlag(FALSE);
		m_regOpt.UserBreadFlag(FALSE);
	
	};
	void	StopScan()
	{
		m_bScan=FALSE; 
		m_fileOpt.UserBreadFlag(TRUE);
		m_regOpt.UserBreadFlag(TRUE);
	};
	
	//无效dll
	BOOL ScanInvalidDll();
	BOOL ScanInvalidDll(LPCTSTR lpcszShareDllPath);
	BOOL InvalidDll(HKEY hRootKey,LPCTSTR lpSubKey,LPCTSTR lpValueName,LPCTSTR lpValueData);
				//处理值名
	static BOOL WINAPI InvalidDll_myEnumerateReg(HKEY rootKey,LPCTSTR lpItemPath,LPCTSTR lpName,DWORD dwValueNameSize,LPBYTE lpValue ,DWORD dwValueSize,DWORD dwType,void* pUserData);
	
	//无效右键菜单
	BOOL ScanInvalidMenu();
	BOOL ScanInvalidMenu(LPCTSTR lpcszMenuHandler);

	//无效的应用程序路径
	BOOL ScanInvalidAppPath();
	BOOL ScanInvalidAppPath(LPCTSTR lpcszAppPath);

	//无效的防火墙设置
	BOOL ScanIvalidFire();
	BOOL ScanIvalidFire(LPCTSTR lpcszShareDllPath);
	BOOL IvalidFire(HKEY hRootKey,LPCTSTR lpSubKey,LPCTSTR lpValueName,LPCTSTR lpValueData);
	
	//无效的MUI缓存
	BOOL ScanIvalidMUICache();
	BOOL ScanIvalidMUICache(LPCTSTR lpcszMUIPath);
	BOOL IvalidMUICache(HKEY hRootKey,LPCTSTR lpSubKey,LPCTSTR lpValueName,LPCTSTR lpValueData);
	
	//无效的帮助文档
	BOOL ScanIvalidHelp();
	BOOL ScanIvalidHelp(LPCTSTR lpcszHelpPath);
	BOOL IvalidHelp(HKEY hRootKey,LPCTSTR lpSubKey,LPCTSTR lpValueName,LPCTSTR lpValueData);
				//处理值数据
	static BOOL WINAPI IvalidHelp_myEnumerateReg(HKEY rootKey,LPCTSTR lpItemPath,LPCTSTR lpName,DWORD dwValueNameSize,LPBYTE lpValue ,DWORD dwValueSize,DWORD dwType,void* pUserData);

	//无效的字体
	BOOL		ScanIvalidFont();
	BOOL		ScanIvalidFont(LPCTSTR lpcszFontsPath);
	BOOL		IvalidFont(HKEY hRootKey,LPCTSTR lpSubKey,LPCTSTR lpValueName,LPCTSTR lpValueData);
	
	//无效的安装程序
	BOOL		ScanIvalidInstall();
	BOOL		ScanIvalidInstall(LPCTSTR lpcszInstallPath);
	BOOL		ScanIvalidInstall2(LPCTSTR lpcszInstallPath);
	BOOL		IvalidInstall(HKEY hRootKey,LPCTSTR lpSubKey,LPCTSTR lpValueName,LPCTSTR lpValueData);
	//无效的反安装程序
	BOOL		ScanInvalidUnInstall();
	BOOL		ScanInvalidUnInstall(LPCTSTR lpcszUnInstallPath);
	//无效的开始菜
	BOOL		ScanInvalidStartMenu();
	BOOL		ScanInvalidStartMenu(LPCTSTR lpcszStartMenu,LPCTSTR lpcszFolderPath);
	
	//无效的ACTIVEX信息
	BOOL		ScanInvelidActiveX();
	BOOL		ScanInvelidActiveX(LPCTSTR strActiveXPath);
	//无效启动项
	BOOL		ScanInvelidRun();
	BOOL		ScanInvelidRun(LPCTSTR lpcszRunPath);
	BOOL		InvelidRun(HKEY hRootKey,LPCTSTR lpSubKey,LPCTSTR lpValueName,LPCTSTR lpValueData);

	//将存在环境变量的路径进行转化
	BOOL		ConvetPath(CString& strSourcPat);
	
	//验证驱动器
	BOOL		ValidDervice(CString& strPath);
	//无效的文件关联
	BOOL		ScanValidFileAss();
	BOOL		ScanValidFileAss(LPCTSTR strFileAssPath);
	//无效的类信息
	BOOL		ScanInvalidClass();
	BOOL		ScanInvalidClass(LPCTSTR lpcszRegClassPath);

	BOOL		ScanInvalidClass2();
	BOOL		ScanInvalidClass2(LPCTSTR lpcszRegClassPath);
	//冗余信息
	BOOL		ScanRegRedundancey();
	BOOL		ScanRegRedundancey(LPCTSTR lpcszRegPath);
	static BOOL WINAPI RegRedancey_myEnumerateReg(HKEY rootKey,LPCTSTR lpItemPath,LPCTSTR lpName,DWORD dwValueNameSize,LPBYTE lpValue ,DWORD dwValueSize,DWORD dwType,void* pUserData);
	BOOL		RegRedundancey(HKEY hRootKey,LPCTSTR lpSubKey,LPCTSTR lpValueName,LPCTSTR lpValueData);
private:
	CFileOpt				m_fileOpt;
public:
	CRegOpt					m_regOpt;
	BOOL					m_bScan;
public:
	static BOOL WINAPI myEnumerateFile(LPCTSTR lpFileOrPath, void* pUserData,LARGE_INTEGER filesize);
	static BOOL WINAPI myEnumerateReg(HKEY rootKey,LPCTSTR lpItemPath,LPCTSTR lpName,DWORD dwValueNameSize,LPBYTE lpValue ,DWORD dwValueSize,DWORD dwType,void* pUserData);
public:
	
};


#endif