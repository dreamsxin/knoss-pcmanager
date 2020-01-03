
/************************************************************************
* @file      : ucipc.cpp
* @author    : WangBin5 <WangBin5.com>
* @date      : 26/11/2009 AM 11:53:11
* @brief     : 
*
* $Id: $
/************************************************************************/

#include <WinError.h>
#include "ucipc_tool.h"
#include "ucipc.h"
#include "md5.h"

#define UC_IPC_TIME_OUT				0x3000

//////////////////////////////////////////////////////////////////////////
#define UC_IPC_DRIVER_MARK          0x80000000  // 驱动发生数据时的标记位。例如下面的通知码在驱动填充时，
                                                // 将由驱动发送部分自动标记此位

#define UC_IPC_DRIVER_NON			0x0		    // 驱动通知码，无操作
#define UC_IPC_DRIVER_BUF_SMALL		0x1		    // 驱动通知码，buf太小，已经在应用层重新分配
#define UC_IPC_DRIVER_NEED_REPLY	0x2		    // 驱动发送的数据需要应用层回复
#define UC_IPC_DRIVER_ZERO_LEN   	0x3		    // 应用层传回去的数据大小为零
//////////////////////////////////////////////////////////////////////////

UcIpc::UcIpc()
{
	m_hSendEventC1 = NULL;
    m_hRecvEventC1 = NULL;
	m_pSendSAC1 = NULL;
    m_pRecvSAC1 = NULL;
	
	m_hSendEventC2 = NULL;
	m_hRecvEventC2 = NULL;
	m_pSendSAC2 = NULL;
	m_pRecvSAC2 = NULL;
	
	m_hMapFile = NULL;
	m_pMapSA = NULL;

	m_pEndSA = NULL;
	m_hEndEvent = NULL;

	m_pvParam = NULL;
	m_BufSize = 0;
    m_BufSize2 = 0;

	m_pMapFileBuf = NULL;

    m_nIpcAddr = 0xFFFFFFFF;

	m_bInitFlag = FALSE;

    m_bCloseStatus = TRUE;
	m_hWork = NULL;
}

BOOL
UcIpc::InitIpc(
	IN ULONG nIpcAddr,
	IN KucPCALLBACK pCallbackRoutine,
	IN PVOID pvParam,
	IN ULONG ulSize,
    IN ULONG ulSize2
	)
{
	BOOL			bRet = FALSE;
	WCHAR			wProtocolType[MAX_PATH] = {0};
	std::wstring	wstTemp;
	DWORD			dwThreadId = 0;

    if (m_bInitFlag) {return TRUE;}

	swprintf(wProtocolType, L"%d", nIpcAddr);

    m_pSendSAC1 = KAVCreateSecurityAttribute();
    m_pRecvSAC1 = KAVCreateSecurityAttribute();

	m_pSendSAC2 = KAVCreateSecurityAttribute();
	m_pRecvSAC2 = KAVCreateSecurityAttribute();

	m_pMapSA = KAVCreateSecurityAttribute();

	m_pEndSA = KAVCreateSecurityAttribute();
    if (!m_pSendSAC1 || !m_pRecvSAC1 ||
		!m_pSendSAC2 || !m_pRecvSAC2 || !m_pMapSA || !m_pEndSA)
    {
        goto _Exit_;
    }

	wstTemp = UC_IPC_UNINIT_EVENT;
	wstTemp += wProtocolType;
    m_hEndEvent = ::CreateEventW(m_pSendSAC1, FALSE, FALSE, wstTemp.c_str());
	if (!m_hEndEvent) {goto _Exit_;}

	// Channel 1
	wstTemp = UC_IPC_EVENT_SEND_C1;
	wstTemp += wProtocolType;
    m_hSendEventC1 = ::CreateEventW(m_pSendSAC1, FALSE, FALSE, wstTemp.c_str());
    if (!m_hSendEventC1)
    {
        m_hSendEventC1 = ::OpenEventW(EVENT_ALL_ACCESS, FALSE, wstTemp.c_str());
        if (!m_hSendEventC1) {goto _Exit_;}
    }
    
    wstTemp = UC_IPC_EVENT_RECV_C1;
	wstTemp += wProtocolType;
    m_hRecvEventC1 = ::CreateEventW(m_pRecvSAC1, FALSE, TRUE, wstTemp.c_str());
    if (!m_hRecvEventC1)
    {
        m_hRecvEventC1 = ::OpenEventW(EVENT_ALL_ACCESS, FALSE, wstTemp.c_str());
        if (!m_hRecvEventC1) {goto _Exit_;}
    }

	SetEvent(m_hRecvEventC1);

	// Channel 2
	wstTemp = UC_IPC_EVENT_SEND_C2;
	wstTemp += wProtocolType;
	m_hSendEventC2 = ::CreateEventW(m_pSendSAC2, FALSE, FALSE, wstTemp.c_str());
	if (!m_hSendEventC2)
	{
		m_hSendEventC2 = ::OpenEventW(EVENT_ALL_ACCESS, FALSE, wstTemp.c_str());
		if (!m_hSendEventC2) {goto _Exit_;}
	}

	wstTemp = UC_IPC_EVENT_RECV_C2;
	wstTemp += wProtocolType;
	m_hRecvEventC2 = ::CreateEventW(m_pRecvSAC2, FALSE, TRUE, wstTemp.c_str());
	if (!m_hRecvEventC2)
	{
		m_hRecvEventC2 = ::OpenEventW(EVENT_ALL_ACCESS, FALSE, wstTemp.c_str());
		if (!m_hRecvEventC2) {goto _Exit_;}
	}

    wstTemp = UC_IPC_FILEMAPNAME_C1;
    wstTemp += wProtocolType;
	m_bInitFlag = InitMap(wstTemp.c_str(), ulSize + ulSize2);
	if (!m_bInitFlag) {goto _Exit_;}

	m_hWork = CreateThread(
		NULL,
		0,
		UcIpc::CommunicationWorker,
		this,
		0,
		&dwThreadId
		);
	if (!m_hWork) {goto _Exit_;}
	
    m_nIpcAddr = nIpcAddr;
	m_BufSize = ulSize;
    m_BufSize2 = ulSize2;
	m_pCallbackRoutine = pCallbackRoutine;
	m_pvParam = pvParam;
	m_bInitFlag = TRUE;
	
_Exit_:
	if (!m_bInitFlag)
	{
		m_bInitFlag = TRUE;
		Uninit();	
	}
	
    return m_bInitFlag;
}

BOOL
UcIpc::InitMap(
	IN LPCWSTR szMapName,
	IN ULONG ulSize
    )
{
    m_hMapFile = ::CreateFileMappingW(
        INVALID_HANDLE_VALUE,               // use paging file
        m_pMapSA,                           // default security 
        PAGE_READWRITE,                     // read/write access
        0,                                  // max. object size 
        ulSize,							// buffer size  
        szMapName);							// name of mapping object
	if (!m_hMapFile)
	{
		m_hMapFile = ::OpenFileMappingW(
			FILE_MAP_ALL_ACCESS,            // read/write access
			TRUE,                           // do not inherit the name
			szMapName);						// name of mapping object
	}
	if (!m_hMapFile) {return FALSE;}
	
    m_pMapFileBuf = (BYTE*) ::MapViewOfFile(
		m_hMapFile,							// handle to map object
        FILE_MAP_ALL_ACCESS,				// read/write permission
        0,                    
        0,                    
        ulSize);
	if (!m_pMapFileBuf) {return FALSE;}
	memset(m_pMapFileBuf, 0x0, ulSize);

	return TRUE;
}

BOOL 
UcIpc::Uninit(
	VOID
	)
{
	if (!m_bInitFlag)
	{
		return TRUE;
	}

	m_bInitFlag = FALSE;
	
	OutputDebugStringA("Now TerminateThread ucipc!\n");
	
	SetEvent(m_hEndEvent);
	if (m_hWork)
	{
		if (WAIT_OBJECT_0 != ::WaitForSingleObject(m_hWork, 500))
			::TerminateThread(m_hWork, 0);
		::CloseHandle(m_hWork);
		m_hWork = NULL;
	}

	if (m_pSendSAC1)
	{
		KAVFreeSecurityAttribute(m_pSendSAC1);
		m_pSendSAC1 = NULL;
	}
	
	if (m_pRecvSAC1)
	{
		KAVFreeSecurityAttribute(m_pRecvSAC1);
		m_pRecvSAC1 = NULL;
	}
	
	if (m_pSendSAC2)
	{
		KAVFreeSecurityAttribute(m_pSendSAC2);
		m_pSendSAC2 = NULL;
	}
	
	if (m_pRecvSAC2)
	{
		KAVFreeSecurityAttribute(m_pRecvSAC2);
		m_pRecvSAC2 = NULL;
	}
	
	if (m_pMapSA)
	{
		KAVFreeSecurityAttribute(m_pMapSA);
		m_pMapSA = NULL;
	}
	
	//////////////////////////////////////////////////////////////////////////
	
	if (m_hSendEventC1)
	{
		CloseHandle(m_hSendEventC1);
		m_hSendEventC1 = NULL;
	}
	
	if (m_hRecvEventC1)
	{
		CloseHandle(m_hRecvEventC1);
		m_hRecvEventC1 = NULL;
	}
	
	if (m_hSendEventC2)
	{
		CloseHandle(m_hSendEventC2);
		m_hSendEventC2 = NULL;
	}
	
	if (m_hRecvEventC2)
	{
		CloseHandle(m_hRecvEventC2);
		m_hRecvEventC2 = NULL;
	}
	
	if (m_hMapFile)
	{
		CloseHandle(m_hMapFile);
		m_hMapFile = NULL;
	}

	if (m_pMapFileBuf)
    { 
        ::UnmapViewOfFile(m_pMapFileBuf);
        m_pMapFileBuf = NULL;
    }

	if (m_hEndEvent)
	{
		::CloseHandle(m_hEndEvent);
		m_hEndEvent = NULL;
	}

	OutputDebugStringA("end TerminateThread ucipc!\n");

	return TRUE;
}

DWORD 
UcIpc::SendMsg(
    IN BOOL bChannel,
    IN ULONG ulPort,
    IN PBYTE pInBuf1,
    IN ULONG ulInBufLen1,
    IN OPTIONAL PBYTE pInBuf2,
    IN OPTIONAL ULONG ulInBufLen2,
    IN DWORD Timeout
	)
/*++	
	Routine Description:
		
		发送数据。
		共享内存被分为两大块，分别用做Channel1和Channel2。
		其中每块Channel又有pInBuf1和pInBuf2.
		当bChannel==TRUE时，拷贝至前KE_IPC_BUF_MAX_SIZE区域

	Arguments:

		pulPort - 以前表示端口。目前采用每个UcIpc绑定一个端口，故此字段做通知驱动采取某些特殊操作的标准

		pInBuf2 - 一般这块buf用做时间校检。可为空
--*/
{
	NTSTATUS      nStatus		= IPC_COM_ERROR;
	HANDLE        hRecvEvent	= NULL;
	HANDLE        hSendEvent	= NULL;
	PBYTE         pAddr         = NULL;
	KeIpcMsgHead  MsgHead		= {0};
	HANDLE		  ArrEvent[2]	= {0};

    WCHAR DbgStr[200] = {0};

	if ((!pInBuf1 || !m_bInitFlag)) {
		OutputDebugStringA("UcIpc::SendMsg invail !\n");
		goto _Exit_;
	}

	if (bChannel && (ulInBufLen1 + ulInBufLen2
		+ sizeof(KeIpcMsgHead) > m_BufSize) ||
        !bChannel && (ulInBufLen1 + ulInBufLen2
        + sizeof(KeIpcMsgHead) > m_BufSize2) )
	{
        swprintf(DbgStr, L"UcIpc::SendMsg IPC_COM_BUF_TOO_SMALL %x, %x, %x!!\n", ulInBufLen1, m_BufSize2, m_nIpcAddr);
		OutputDebugStringW(DbgStr);
		nStatus = IPC_COM_BUF_TOO_SMALL;
		goto _Exit_;
	}
	
	if (bChannel)
	{
		hRecvEvent = m_hRecvEventC1;
		hSendEvent = m_hSendEventC1;
		pAddr = m_pMapFileBuf;
	}
	else
	{
		hRecvEvent = m_hRecvEventC2;
		hSendEvent = m_hSendEventC2;
		pAddr = m_pMapFileBuf + m_BufSize;
	}

	ArrEvent[0] = hRecvEvent;
	ArrEvent[1] = m_hEndEvent;
	nStatus = WaitForMultipleObjects(2, ArrEvent,
 		FALSE, Timeout);
	if(WAIT_OBJECT_0 != nStatus) {
		//OutputDebugStringA("UcIpc::SendMsg wait fail!\n");
		nStatus = IPC_COM_WAIT_FAIL; goto _Exit_;}

    // 一个channel中的共享内存中的结构如下:
    // +--------------------+
    // |    KeIpcMsgHead    |
    // +--------------------+
    // |    Buf1            |
    // +--------------------+
    // |    Buf2            |
    // +--------------------+
	MsgHead.ulPort = ulPort;
	MsgHead.ulInBufLen1 = ulInBufLen1;
	MsgHead.ulInBufLen2 = ulInBufLen2;
	MDString (pInBuf1, ulInBufLen1, MsgHead.Md5);

	memcpy(pAddr, &MsgHead, sizeof(KeIpcMsgHead));
	memcpy(pAddr + sizeof(KeIpcMsgHead), 
		pInBuf1, ulInBufLen1);
	memcpy(pAddr + sizeof(KeIpcMsgHead) + ulInBufLen1,
		pInBuf2, ulInBufLen2);
	
	nStatus = IPC_COM_SUCCEED;
	//OutputDebugStringA("UcIpc::SendMsg succeed!\n");
	SetEvent(hSendEvent);

_Exit_:
	
	return nStatus;
}

DWORD 
UcIpc::RecvMsg(
    IN CKxEUCAllocator* allocator,
    IN BOOL bChannel,
	IN OUT PULONG pulPort,
    IN OUT PBYTE* ppInBuf1,
    IN OUT PULONG pulInBufLen1,
    IN OUT PBYTE* ppInBuf2,
    IN OUT PULONG pulInBufLen2,
    IN DWORD Timeout
	)
/*++	
	Routine Description:
		
		接受数据。
		共享内存被分为两大块，分别用做Channel1和Channel2。
		其中每块Channel又有pInBuf1和pInBuf2.
		当bChannel==TRUE时，拷贝至前KE_IPC_BUF_MAX_SIZE区域

	Arguments:

		pInBuf2 - 一般这块buf用做时间校检。可为空
--*/
{
	DWORD         nStatus		= IPC_COM_ERROR;
	HANDLE        hRecvEvent	= NULL;
	HANDLE        hSendEvent	= NULL;
	PBYTE         pAddr         = NULL;
	PBYTE		  pNewOutBuf	= NULL;
	KeIpcMsgHead  MsgHead		= {0};
	HANDLE		  ArrEvent[2]	= {0};
	CHAR		  Md5[UC_IPC_MD5_SIZE] = {0};

	if (!ppInBuf1 || !pulInBufLen1 || !pulPort
		|| !m_bInitFlag) {goto _Exit_;}

	if (bChannel)
	{
		hRecvEvent = m_hRecvEventC1;
		hSendEvent = m_hSendEventC1;
		pAddr = m_pMapFileBuf;
	}
	else
	{
		hRecvEvent = m_hRecvEventC2;
		hSendEvent = m_hSendEventC2;
		pAddr = m_pMapFileBuf + m_BufSize;
	}
	
	ArrEvent[0] = hSendEvent;
	ArrEvent[1] = m_hEndEvent;
 	nStatus = WaitForMultipleObjects(2, ArrEvent,
 		FALSE, Timeout);
 	if(nStatus != WAIT_OBJECT_0) {nStatus = IPC_COM_WAIT_FAIL; goto _Exit_;}

	memcpy(&MsgHead, pAddr, sizeof(KeIpcMsgHead));

    if ( bChannel && (MsgHead.ulInBufLen1 + MsgHead.ulInBufLen2
        + sizeof(KeIpcMsgHead) > m_BufSize) ||
        !bChannel && (MsgHead.ulInBufLen1 + MsgHead.ulInBufLen2
        + sizeof(KeIpcMsgHead) > m_BufSize2) )
	{
        OutputDebugStringA("TOO_SMALL\n");
		nStatus = IPC_COM_BUF_TOO_SMALL; 
		goto _Exit_;
	}

	// 如果调用方不接受第二个参数，就不用申请相应空间了
	if (pulInBufLen2 && ppInBuf2)
	{
		pNewOutBuf = (PBYTE)allocator->Alloc ( 
			MsgHead.ulInBufLen1 + MsgHead.ulInBufLen2);
	}
	else
	{
		pNewOutBuf = (PBYTE)allocator->Alloc ( 
			MsgHead.ulInBufLen1);
	}
	if (!pNewOutBuf) {nStatus = IPC_COM_ERROR_MEM_ALLOT; goto _Exit_;}

	memcpy(pNewOutBuf, pAddr + sizeof(KeIpcMsgHead), 
		MsgHead.ulInBufLen1);
	MDString (pNewOutBuf, MsgHead.ulInBufLen1, Md5);
	if (strncmp(MsgHead.Md5, Md5, UC_IPC_MD5_SIZE))
	{
		OutputDebugStringA("Md5 Fail\n");
		nStatus = IPC_COM_ERROR_MEM_ALLOT;
		goto _Exit_;
	}

	*pulPort = MsgHead.ulPort;
	*ppInBuf1 = pNewOutBuf;
	*pulInBufLen1 = MsgHead.ulInBufLen1;

	if (pulInBufLen2 && ppInBuf2)
	{
		memcpy(
			pNewOutBuf + MsgHead.ulInBufLen1, 
			pAddr + sizeof(KeIpcMsgHead) + MsgHead.ulInBufLen1, 
			MsgHead.ulInBufLen2
			);
		*ppInBuf2 = pNewOutBuf + MsgHead.ulInBufLen1;
		*pulInBufLen2 = MsgHead.ulInBufLen2;
	}
	
	nStatus = IPC_COM_SUCCEED;
	SetEvent(hRecvEvent);

_Exit_:
	
	return nStatus;
}

DWORD
UcIpc::CommunicationWorker( 
	LPVOID lpPrarm 
	)
/*++	
    Routine Description:
	    每个端口将开启一个此线程，用做处理驱动传过来的消息通过注册的回调处理后传回去

--*/
{
	CKxEUCAllocator allocator;
	UcIpc*			pUcIpc			= (UcIpc*)lpPrarm;
	PBYTE			pInBuf1			= NULL;
	ULONG			pulInBufLen1	= 0;
	PBYTE			pCheckTime		= NULL;
	ULONG			pulCheckTimeLen = 0;
	PBYTE			pvReplyData		= NULL;
	DWORD			dwDataLengthRet	= 0;
	ULONG			ulPort			= 0;
	NTSTATUS		nStatus			= IPC_COM_ERROR;
	DWORD           dwCallBackRet   = 0;

	if (!pUcIpc) {return 0;}

	while ( pUcIpc->m_bInitFlag )
	{
        // m_pCallbackRoutine中曾经由于不好的编程习惯未设置返回dwDataLengthRet的值导致崩溃
        // 故在此每次循环重新初始化一次.
        pInBuf1			= NULL;
        pulInBufLen1	= 0;
        pCheckTime		= NULL;
        pulCheckTimeLen = 0;
        pvReplyData		= NULL;
        dwDataLengthRet	= 0;
        ulPort			= 0;
        nStatus			= IPC_COM_ERROR;
        dwCallBackRet   = 0;

		nStatus = pUcIpc->RecvMsg(
			&allocator,
			TRUE,
			&ulPort,
			&pInBuf1,
			&pulInBufLen1,
			&pCheckTime,
			&pulCheckTimeLen,
			UC_IPC_TIME_OUT
			);
		if (IPC_COM_SUCCEED != nStatus)
        {
            if (IPC_COM_WAIT_FAIL != nStatus)
            {
                WCHAR wszTime[127 + 1] = {0};
                wsprintfW(wszTime, L"RecvMsg fail!! %x\n", nStatus);
                OutputDebugStringW(wszTime);
            }
            
            continue;
        }

        // 如果没有UC_IPC_DRIVER_MARK位，表示不是从驱动传来的
        if (!(ulPort & UC_IPC_DRIVER_MARK)) {continue;}
        ulPort &= ~UC_IPC_DRIVER_MARK;

		if (pUcIpc->m_pCallbackRoutine)
		{
			pUcIpc->m_pCallbackRoutine(
				&allocator,             // 返回数据使用的分配器
				pInBuf1,                // 数据
				pulInBufLen1,           // 数据长度
				pUcIpc->m_pvParam,      // 回调的自定义参数
				(PVOID*)&pvReplyData,   // 返回的数据, 如果不需要返回则ppvDataRet == NULL
				&dwDataLengthRet);      // 返回的数据长度, 如果不需要返回则pdwDataLengthRet == NULL);

			if (UC_IPC_DRIVER_NEED_REPLY == ulPort)
			{
                /////////////////////////////////////////////////////////////////////////
                // 下面这段检测数据头的代码本来不属于统一通信。但原来统一通信传递的数据都有个没用的
                // 数据头，只好在这里去掉。
//                 if (dwDataLengthRet <= sizeof(KXE_COMM_PACKET_HEADER))
//                 {
                    // 如果不带数据头，则随便传个脏数据。
                    // 驱动中可由UC_IPC_DRIVER_ZERO_LEN获知
                    nStatus = pUcIpc->SendMsg(
                        FALSE,
                        UC_IPC_DRIVER_ZERO_LEN,
                        (PBYTE)&pvReplyData,     // 无用的脏数据
                        sizeof(PVOID*),
                        pCheckTime,
                        pulCheckTimeLen,
                        UC_IPC_TIME_OUT
                        );
//                 }
//                 else
//                 {
//                     pvReplyData += sizeof(KXE_COMM_PACKET_HEADER);
//                     dwDataLengthRet -= sizeof(KXE_COMM_PACKET_HEADER);
//                     nStatus = pUcIpc->SendMsg(
//                         FALSE,
//                         0 != dwDataLengthRet ? UC_IPC_DRIVER_NON : UC_IPC_DRIVER_ZERO_LEN,
//                         0 != dwDataLengthRet ? (PBYTE)pvReplyData : (PBYTE)&pvReplyData, // 如果为零，随便传个脏数据
//                         0 != dwDataLengthRet ? dwDataLengthRet : sizeof(PVOID*),
//                         pCheckTime,
//                         pulCheckTimeLen,
//                         UC_IPC_TIME_OUT
//                         );
//                     //OutputDebugStringW(L"SendMsg Over\n\n");
//                 }
			}
		}

		allocator.Clear();
	}

	return 0;
}