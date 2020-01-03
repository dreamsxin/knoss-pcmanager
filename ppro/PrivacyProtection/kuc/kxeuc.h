
/************************************************************************
* @file      : kxeuc.h
* @author    : WangBin5 <WangBin5.com>
* @date      : 26/11/2009 AM 11:53:11
* @brief     : 统一通信接口
*
* $Id: $
/************************************************************************/

#include "windows.h"
#include "ucipc.h"

#ifndef _KXE_UC_H_
#define _KXE_UC_H_

#pragma pack (2)
typedef struct _KsafeFileMonStr 
{
    WCHAR sProName[MAX_PATH + 1];  // 操作进程名
    WCHAR sOpenName[MAX_PATH + 1]; // 被打开的文件名
}
KsafeFileMonStr, *PKsafeFileMonStr;
#pragma pack ()

typedef enum
{
	enumKxERuleMgr,
	enumKxELogger,
	enumKxEMisc,
	enumKxEInteractive,
	enumKxETrackSystem,
	enumKxECreateProcess,
		
	enumKxEMaximumSerivceID
};

// 统一通信回调使用的内存分配器
interface KucIAllocator
{
	virtual LPVOID WINAPI Alloc(
		/* [in] */ ULONG ulSize                     // 要分配的内存大小
		) = 0;
};

// 统一通信回调函数声明
typedef DWORD (WINAPI *KucPCALLBACK)(
	/* [in] */ KucIAllocator *piAllocator,             // 返回数据使用的分配器
	/* [in] */ PVOID    pvData,                     // 数据
	/* [in] */ DWORD    dwDataLength,               // 数据长度
	/* [in] */ PVOID    pvParam,                    // 回调的自定义参数
	/* [in] */ PVOID    *ppvDataRet,                // 返回的数据, 如果不需要返回则ppvDataRet == NULL
	/* [in] */ DWORD    *pdwDataLengthRet           // 返回的数据长度, 如果不需要返回则pdwDataLengthRet == NULL
        );

int WINAPI KucInit(
	/* [in] */ void* pvNotused /* = NULL */ 
	);

void WINAPI KucUnInit();

int WINAPI KucRegisterCallback(
	/* [in] */ ULONG		ulClientID,              // 通信客户端ID
	/* [in] */ KucPCALLBACK	pCallbackRoutine,        // 回调函数指针
	/* [in] */ PVOID		pvParam                  // 给回调函数指定一个自定义参数
	);

int WINAPI KucUnregisterCallback(
	/* [in] */ ULONG    ulClientID					 // 通信客户端ID								   
	);

BOOL WINAPI KucSetVideoDevName(
    /* [in] */ LPCWSTR lpName
    );

#endif