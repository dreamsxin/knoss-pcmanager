/************************************************************************
* @file      : ucipc.h
* @author    : WangBin5 <WangBin5.com>
* @date      : 26/11/2009 AM 11:53:11
* @brief     : 与内核通信IPC的用户层代码
*
* $Id: $
/************************************************************************/

#ifndef _UC_IPC_H_
#define _UC_IPC_H_

#include <windows.h>
#include <list>
//#include "defendengine/kavutyu.h"
#include "kxeuc.h"
#include "ucipc_def.h"

#define UC_IPC_EVENT_SEND_C1            L"Global\\{1B42EA3C-727D-4366-BDC6-5940B29F34E3}"
#define UC_IPC_EVENT_RECV_C1            L"Global\\{54DDC4D8-66B5-416f-A436-02D71456F9C6}"

#define UC_IPC_EVENT_SEND_C2			L"Global\\{96DC960C-5F03-47c4-BAFC-9A386D321296}"
#define UC_IPC_EVENT_RECV_C2			L"Global\\{F25FB848-C89D-42fc-9005-1854F5187777}"

#define UC_IPC_FILEMAPNAME_C1		    L"Global\\{C82FCCB5-4070-4a60-A627-FF1143DE4BEF}"

#define UC_IPC_UNINIT_EVENT			    L"Global\\{2EDB1C62-344F-45ad-82ED-9506A77FC541}"

#define UC_IPC_MD5_SIZE					64	    // 用于校检的MD5符串大小

typedef struct _KeIpcMsgHead
{
	ULONG ulPort;
	ULONG ulInBufLen1;
	ULONG ulInBufLen2;
	CHAR  Md5[UC_IPC_MD5_SIZE];
}
KeIpcMsgHead, *PKeIpcMsgHead;

class CKxEUCAllocator : public KucIAllocator
{
#define ArrListSize 63
public:
	CKxEUCAllocator() 
	{
		InitializeCriticalSection(&m_cs);
		memset(m_ArrList, 0, (ArrListSize + 1) * sizeof(LPVOID));
		m_nArrConut = 0;
	}
	
	virtual LPVOID WINAPI Alloc(
		/* [in] */ ULONG ulSize                     // 要分配的内存大小
		)
	{
		LPVOID pvData = NULL;
		EnterCriticalSection(&m_cs);
		
		if (m_nArrConut < ArrListSize)
		{
			pvData = (LPVOID)malloc(ulSize);
			if (!pvData) {goto _Exit_;}

			m_ArrList[m_nArrConut++] = pvData;
		}

	_Exit_:
		LeaveCriticalSection(&m_cs);
		return pvData;
	}
	
	void Clear()
	{
		int i = 0;
		EnterCriticalSection(&m_cs);

		for (i = 0; i < m_nArrConut; i++)
		{
			free (m_ArrList[i]);
			m_ArrList[i] = NULL;
		}

		m_nArrConut = 0;
		LeaveCriticalSection(&m_cs);
	}
	
private:
	
	int		m_nArrConut;
	LPVOID	m_ArrList[ArrListSize + 1];
	CRITICAL_SECTION m_cs;
	#undef ArrListSize
};

class UcIpc
{
public:
	UcIpc();
	
	~UcIpc();
	
    BOOL InitIpc(	
		IN ULONG nIpcAddr,
		IN KucPCALLBACK pCallbackRoutine,
		IN PVOID pvParam,
		IN ULONG ulSize,
        IN ULONG ulSize2
		);
	
    BOOL Uninit(
		VOID
		);

	BOOL InitMap(
		IN LPCWSTR szMapName,
		IN ULONG ulSize
		);

	static DWORD WINAPI CommunicationWorker( 
		LPVOID lpPrarm 
		);

	DWORD SendMsg(
        IN BOOL bChannel,
        IN ULONG ulPort,
        IN PBYTE pInBuf1,
        IN ULONG ulInBufLen1,
        IN OPTIONAL PBYTE pInBuf2,
        IN OPTIONAL ULONG ulInBufLen2,
        IN DWORD Timeout
	    );

	DWORD RecvMsg(
        IN CKxEUCAllocator* allocator,
        IN BOOL bChannel,
	    IN OUT PULONG pulPort,
        IN OUT PBYTE* ppInBuf1,
        IN OUT PULONG pulInBufLen1,
        IN OUT PBYTE* ppInBuf2,
        IN OUT PULONG pulInBufLen2,
        IN DWORD  Timeout
	    );

    ULONG GetAddr() {return m_bInitFlag ? m_nIpcAddr : -1;}

	PBYTE GetMapBuf() {return m_pMapFileBuf;}

	ULONG GetMapSize() {return m_BufSize;}

    ULONG GetMapSize2() {return m_BufSize2;}

    BOOL  GetCloseStatus() {return m_bCloseStatus;}

    BOOL  SetCloseStatus(BOOL Status) {return m_bCloseStatus = Status;}
	
private:

    BOOL m_bInitFlag;

	HANDLE m_hWork;
	
    HANDLE m_hSendEventC1;
    HANDLE m_hRecvEventC1;
    PSECURITY_ATTRIBUTES m_pSendSAC1;
    PSECURITY_ATTRIBUTES m_pRecvSAC1;
	
	HANDLE m_hSendEventC2;
	HANDLE m_hRecvEventC2;
	PSECURITY_ATTRIBUTES m_pSendSAC2;
	PSECURITY_ATTRIBUTES m_pRecvSAC2;

	HANDLE m_hEndEvent;
	PSECURITY_ATTRIBUTES m_pEndSA;					// 通知各线程退出的事件

	HANDLE m_hMapFile;
	PBYTE m_pMapFileBuf;
	PSECURITY_ATTRIBUTES m_pMapSA;

	KucPCALLBACK m_pCallbackRoutine;			// 回调函数
	PVOID m_pvParam;								// 回调函数自定义的参数
	ULONG m_BufSize;
    ULONG m_BufSize2;                               // 第2个channel的大小
    ULONG m_nIpcAddr;
    BOOL  m_bCloseStatus;                           // 是否被临时的关闭.false表示被关闭
};


#endif

