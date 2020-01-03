//////////////////////////////////////////////////////////////////////
//  @ File		:	idl_util_inl.h
//  @ version	:	1.0
//  @ Author	:	梁光彩<liangguangcai@kingsoft.com>
//  @ Date		:	[2009-12-10, 17:53:12]
//  @ Brief		:	用于客户端与服务提供者直接通信的常用功能定义
//					
//  @ Last changed
//    #who		:
//    #when		:
//    #what		:
//////////////////////////////////////////////////////////////////////
#ifndef __IDLUTILINL__H__LGC__
#define __IDLUTILINL__H__LGC__

#if defined (_MSC_VER) && (_MSC_VER > 1020)
#pragma once
#endif

#include <windows.h>
#include <string>
#include "kxecommon/kxecommon.h"

//------------------------------------------------------------------------------

/**
* @brief		从注册表中读取目标 SP 命名管道名
* @remark		
* @param[in]	: wstrSPName 为目标 SP 的名字
* @param[out]	: wstrPipeName 为获取到的命名管道名
* @return		0 成功，其他为错误码
*/
inline int __stdcall idl_util_get_sp_pipe_name_from_registry(
	const std::wstring& wstrSPName,
	std::wstring& wstrPipeName
	)
{
	WCHAR szBuf[MAX_PATH] = {0};
	_snwprintf(
		szBuf,
		MAX_PATH,
		L"%s%s",
		kxe_get_config_sp_root_path(),
		wstrSPName.c_str()
		);
	
	HKEY hKey = NULL;
	LONG lRet = ::RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		szBuf,
		0,
		KEY_READ,
		&hKey
		);
	if (ERROR_SUCCESS != lRet)
	{
		return lRet;
	}
	
	DWORD dwType = 0;
	DWORD dwBytes = 0;
	
	// 先得到返回数据长度
	lRet = ::RegQueryValueExW(
		hKey,
		KXE_SP_PIPE_NAME_SUB_KEY_W,
		NULL,
		&dwType,
		NULL,
		&dwBytes
		);
	if (ERROR_SUCCESS != lRet)
	{
		::RegCloseKey(hKey);
		return lRet;
	}
	
	// 再读取返回数据
	LPBYTE lpData = new BYTE[dwBytes + sizeof(WCHAR)];
	if (NULL == lpData)
	{
		::RegCloseKey(hKey);
		return E_KXEPIPE_NOT_ENOUGH_MEMORY;
	}
	lRet = ::RegQueryValueExW(
		hKey,
		KXE_SP_PIPE_NAME_SUB_KEY_W,
		NULL,
		&dwType,
		lpData,
		&dwBytes
		);
	if (ERROR_SUCCESS == lRet && REG_SZ == dwType)
	{
		*(WCHAR *)(&lpData[dwBytes]) = L'\0';
		wstrPipeName = L"\\\\.\\Pipe\\";
		wstrPipeName += (WCHAR *)lpData;
	}
	
	::RegCloseKey(hKey);
	delete[] lpData;
	return lRet;
}

inline int __stdcall idl_util_check_if_sp_exist(
	const std::wstring wstrSPName,
	BOOL& bExist
	)
{
	std::wstring wstrPipeName;
	int nRet = idl_util_get_sp_pipe_name_from_registry(
		wstrSPName,
		wstrPipeName
		);
	if (0 != nRet)
	{
		bExist = FALSE;
		return nRet;
	}

	if (!::WaitNamedPipeW(wstrPipeName.c_str(), 100))
	{
		nRet = ::GetLastError();
		if (ERROR_PIPE_BUSY != nRet)
		{
			bExist = FALSE;
			return nRet;
		}
	}
	
	bExist = TRUE;
	return 0;
}
//------------------------------------------------------------------------------

#endif	/*	__IDLUTILINL__H__LGC__	*/