
/************************************************************************
* @file      : kxeuc.cpp
* @author    : WangBin5 <WangBin5.com>
* @date      : 26/11/2009 AM 11:53:11
* @brief     : 统一通信接口的实现层
*
* $Id: $
/************************************************************************/

#pragma warning(disable: 4786)
#include <map>
#include <windows.h>
//#include "kavutyctrl.h"
#include "kxeuc.h"
//#include "defendengine/kavutyu.h"
#include "ucipc.h"
#include "ucipc_tool.h"

#define KE_IPC_ECHO_EVENT						L"Global\\{65DB9744-7304-4063-8672-7BADF7C28F26}"
#define KAV_MSG_LINK_NAME	                    L"\\\\.\\ksafefilemon"


//#define FILE_DEVICE_KAVUTY						0x00008300

#define KAV_UC_CODE_BASE						0x900	

#define KAV_UC_CTL_CODE( _FunctionCode )		CTL_CODE( FILE_DEVICE_UNKNOWN, (_FunctionCode), METHOD_BUFFERED, FILE_ANY_ACCESS )

#define KAV_UC_CREATE_ECHO_PORT					KAV_UC_CTL_CODE( KAV_UC_CODE_BASE + 1 ) 

#define KAV_UC_ECHO_MESSAGE						KAV_UC_CTL_CODE( KAV_UC_CODE_BASE + 2 )

#define KAV_UC_CLOSE_ECHO_PORT					KAV_UC_CTL_CODE( KAV_UC_CODE_BASE + 3 )

#define KAV_UC_CREATE_COMMUNICATION_PORT		KAV_UC_CTL_CODE( KAV_UC_CODE_BASE + 4 )

#define KAV_UC_CLOSE_COMMUNICATION_PORT			KAV_UC_CTL_CODE( KAV_UC_CODE_BASE + 5 )

#define KAV_UC_THREAD_START						KAV_UC_CTL_CODE( KAV_UC_CODE_BASE + 6 )

#define KAV_UC_GET_MESSAGE						CTL_CODE( FILE_DEVICE_UNKNOWN, KAV_UC_CODE_BASE + 7, METHOD_OUT_DIRECT, FILE_ANY_ACCESS )

#define KAV_UC_REPLY_MESSAGE					CTL_CODE( FILE_DEVICE_UNKNOWN, KAV_UC_CODE_BASE + 8, METHOD_IN_DIRECT, FILE_ANY_ACCESS )

#define KAV_UC_SEND_MESSAGE						CTL_CODE( FILE_DEVICE_UNKNOWN, KAV_UC_CODE_BASE + 9, METHOD_OUT_DIRECT, FILE_ANY_ACCESS )

#define KAV_UC_CANCEL_GET_MESSAGE				KAV_UC_CTL_CODE( KAV_UC_CODE_BASE + 10 )

#define KAV_UC_KEIPC_INIT						CTL_CODE( FILE_DEVICE_UNKNOWN, KAV_UC_CODE_BASE + 11, METHOD_IN_DIRECT, FILE_ANY_ACCESS )

#define KAV_UC_KEIPC_COLSE						CTL_CODE( FILE_DEVICE_UNKNOWN, KAV_UC_CODE_BASE + 12, METHOD_IN_DIRECT, FILE_ANY_ACCESS )

#define KAV_UC_KEIPC_REINIT						CTL_CODE( FILE_DEVICE_UNKNOWN, KAV_UC_CODE_BASE + 13, METHOD_IN_DIRECT, FILE_ANY_ACCESS )

#define KAV_UC_SET_VIDEO_NAME   				CTL_CODE( FILE_DEVICE_UNKNOWN, KAV_UC_CODE_BASE + 14, METHOD_IN_DIRECT, FILE_ANY_ACCESS )

#define KAV_UC_ERROR_NOT_READY					0xE0000001

#define KAV_UC_ERROR_CONNECTION_UNAVAILABLE		0xE0000002

#define KAV_UC_ERROR_CONNECTION_DISCONNECTED	0xE0000003

#define KAV_UC_ERROR_CONNECTION_NONEXISTENCE	0xE0000004

#define KAV_UC_ERROR_PORT_DISMATCH				0xE0000005

#define KE_IPC_BUF_MAX_SIZE						0x1000	// 姣忎釜channel鐨勫ぇ灏?

class KPortInfo
{
public:
    static KPortInfo* GetInst(
        VOID
        )
    {
        static KPortInfo* m_pInstance;
        if ( m_pInstance == NULL )
        {
            m_pInstance = new KPortInfo();		
        }

        return m_pInstance;
    }

    ULONG GetSize(ULONG nIpcAddr) 
    {
        return _GetInfo(nIpcAddr, 0);
    };

    ULONG GetSize2(ULONG nIpcAddr) 
    {
        return _GetInfo(nIpcAddr, 1);
    };

    ULONG GetLinkNum(ULONG nIpcAddr) 
    {
        return _GetInfo(nIpcAddr, 2);
    };

private:
    KPortInfo() 
    {
        // IPC对象信息。以下分别对应size, size2, 连接数
        // 下面这个对应于
        // typedef enum
        // {
        //     enumKxERuleMgr,
        //     enumKxELogger,
        //     enumKxEMisc,
        //     enumKxEInteractive,
        //     enumKxETrackSystem,
        //     enumKxECreateProcess,
        // 
        //     enumKxEMaximumSerivceID
        // };
        ULONG PortInfoArr[7][3] = 
        {
            {0x12000, 0x200, 1},
            {0x200, 0x200, 1},
            {0x200, 0x200, 1},
            {0x500, 0x200, 3},
            {0x200, 0x200, 1},
            {0x200, 0x200, 1}, 
            {0x200, 0x200, 1}
        };
        memcpy(m_PortInfoArr, PortInfoArr, sizeof(PortInfoArr));
    }

    KPortInfo* m_pInstance;

    ULONG m_PortInfoArr[7][3];

    ULONG _GetInfo(ULONG nIpcAddr, UINT Type) 
    {
        if (nIpcAddr > enumKxEMaximumSerivceID)
        {
            nIpcAddr = 0; 
        }

        return m_PortInfoArr[nIpcAddr][Type];
    }
};

typedef struct _KXEUC_MSG_HEAD
{
    ULONG  nIpcLinkPort;                                        // 以前的端口和链接混合而成的地址
	HANDLE hPid;
	PVOID  pUserAddr;
	ULONG  ulSize;
    ULONG  ulSize2;
}
KXEUC_MSG_HEAD, *PKXEUC_MSG_HEAD;

// 保存每个端口相关信息类，如每个端口有多少个连接数
// Ipc地址的每个位是这样安排的：
// 低 --->  高
// |||||    ||||    ||||....
// 端口号   连接数
#define IPC_PORT_BIT 5
#define IPC_LINK_BIT 4

class KIpcPort
{
public:

    KIpcPort()
    {
        m_nPortAddr = 0;
        m_nLinkNum = 0;
        m_IpcArr = NULL;
        m_bCloseStatus = TRUE;
        pCallbackRoutine = NULL;
    }

    ULONG GetLinNum() {return m_nLinkNum;}

    UcIpc* GetIpc(ULONG Link)
    {
        if (!m_IpcArr || Link > m_nLinkNum) {return NULL;}

        return m_IpcArr[Link];
    }

    BOOL InitPort(
        ULONG nPortAddr,
        ULONG nLinkNum,
        ULONG ulSize,
        ULONG ulSize2,
        KucPCALLBACK pCallbackRoutine,
        PVOID pvParam
        )
    {
        ULONG   i    = 0;
        UcIpc*  pIpc = NULL;

        if (!nLinkNum || !pCallbackRoutine) {return TRUE;}

        m_nPortAddr = nPortAddr;
        m_nLinkNum = nLinkNum;
        m_IpcArr = (UcIpc**)malloc(m_nLinkNum * sizeof(UcIpc*));
        if (!m_IpcArr) {return FALSE;}

        memset(m_IpcArr, 0, m_nLinkNum * sizeof(UcIpc*));

        for (i = 0; i < m_nLinkNum; ++i)
        {
            pIpc = new UcIpc();
            if (!pIpc->InitIpc(
                GetAddrByPortAndLink(nPortAddr, i),
                pCallbackRoutine, 
                pvParam,
                ulSize,
                ulSize2))
            {
                return FALSE;
            }
            m_IpcArr[i] = pIpc;
        }

        SetCloseStatus(TRUE);
        return TRUE;
    }

    BOOL  GetCloseStatus() {return m_bCloseStatus;}

    BOOL  SetCloseStatus(BOOL Status) {return m_bCloseStatus = Status;}

private:
    inline ULONG GetAddrByPortAndLink(ULONG Port, ULONG Link)
    {
        return ( (Port&0x1F) + ((Link&0xF)<<IPC_PORT_BIT) );
    }

    ULONG   m_nPortAddr;
    ULONG   m_nLinkNum;
    UcIpc** m_IpcArr;
    BOOL    m_bCloseStatus;
    KucPCALLBACK pCallbackRoutine;
};

class KUcFactory
{
public:
	
	static KUcFactory* GetInstance(
		VOID
		)
	{
		static KUcFactory* m_pInstance;
		if ( m_pInstance == NULL )
		{
			m_pInstance = new KUcFactory();		
		}
		
		return m_pInstance;
	}

	static DWORD WINAPI EchoWorker( 
		LPVOID lpPrarm 
		)
	{	
		KUcFactory*				Factory			= (KUcFactory*)lpPrarm;
		KXEUC_MSG_HEAD			MsgHdr			= {0};
		DWORD					dwReturnedBytes = 0;
		IPC_MAP_SET::iterator	iter;
		BOOL					bSuccess		= FALSE;
        ULONG                   i               = 0;
		if (!Factory) {return 0;}

		MsgHdr.hPid = (HANDLE)GetCurrentProcessId();

        OutputDebugStringA("Begin EchoWorker !\n");

		while (Factory->m_bInitFlag)
		{			
			if (!Factory->m_hMsgDevice || INVALID_HANDLE_VALUE == Factory->m_hMsgDevice)
			{
				Factory->m_hMsgDevice = CreateFileW(  
					KAV_MSG_LINK_NAME,
					GENERIC_READ | GENERIC_WRITE,
					0,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL
					);
                if (!Factory->m_hMsgDevice || 
                    INVALID_HANDLE_VALUE == Factory->m_hMsgDevice)
                {
                    //OutputDebugStringA("INVALID_HANDLE_VALUE!\n");
                    Factory->m_hMsgDevice = NULL;
                    continue;
                }
                else
                {
                    OutputDebugStringA("CreateFile KxEUc Suc!\n");
                }
			}

			// 定时向驱动通知初始化消息，以防驱动断开后就联系不上了
			if (Factory->m_hMsgDevice && INVALID_HANDLE_VALUE != Factory->m_hMsgDevice)
			{
				for(iter = Factory->m_IpcSet.begin(); 
					iter != Factory->m_IpcSet.end(); iter++)
				{
                    EnterCriticalSection(&(Factory->m_SetCs));
                    if (!iter->second->GetCloseStatus())
                    {
                        LeaveCriticalSection(&(Factory->m_SetCs));
                        continue;
                    }
                    for (i = 0; i < iter->second->GetLinNum(); ++i)
                    {
                        MsgHdr.nIpcLinkPort = iter->second->GetIpc(i)->GetAddr();
                        MsgHdr.pUserAddr = iter->second->GetIpc(i)->GetMapBuf();
                        MsgHdr.ulSize = iter->second->GetIpc(i)->GetMapSize();
                        MsgHdr.ulSize2 = iter->second->GetIpc(i)->GetMapSize2();
                        if (!MsgHdr.pUserAddr || !MsgHdr.ulSize|| !MsgHdr.ulSize2 ||
                            -1 == MsgHdr.nIpcLinkPort)
                        {
                            LeaveCriticalSection(&(Factory->m_SetCs));
                            continue;
                        }

                        bSuccess = DeviceIoControl(
                            Factory->m_hMsgDevice,
                            KAV_UC_KEIPC_REINIT,
                            &MsgHdr,
                            sizeof( KXEUC_MSG_HEAD ),
                            NULL,
                            0,
                            &dwReturnedBytes,
                            NULL
                            );
                    }
                    LeaveCriticalSection(&(Factory->m_SetCs));
				}
			}

			WaitForSingleObject(Factory->m_hEchoEvent, 600);
		}

        OutputDebugStringA("End EchoWorker\n");
		return 0;
	}

	BOOL InitFactory()
	{
		DWORD dwThreadId = 0;

        if (m_bInitFlag) {return TRUE;}

		m_pEchoSAC = KAVCreateSecurityAttribute();
		m_hEchoEvent = ::CreateEventW(m_pEchoSAC, FALSE, TRUE, KE_IPC_ECHO_EVENT);
		if (!m_hEchoEvent) {return FALSE;}

        OutputDebugStringA("CreateThread!\n");
		if (!m_hEchoThread)
		{
			m_hEchoThread = CreateThread(
				NULL,
				0,
				KUcFactory::EchoWorker,
				this,
				0,
				&dwThreadId
				);
			if (!m_hEchoThread) 
			{
				KAVFreeSecurityAttribute(m_pEchoSAC);
				m_pEchoSAC = NULL;
				CloseHandle(m_hEchoEvent);
				m_hEchoEvent = NULL;
				return FALSE;
			}
		}

        m_bInitFlag = TRUE;
        
        if (!m_hMsgDevice || INVALID_HANDLE_VALUE == m_hMsgDevice)
        {
            m_hMsgDevice = CreateFileW(  
                KAV_MSG_LINK_NAME,
                GENERIC_READ | GENERIC_WRITE,
                0,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL
                );
        }

        if (INVALID_HANDLE_VALUE == m_hMsgDevice)
        {
            OutputDebugStringA("INVALID_HANDLE_VALUE!\n");
            m_hMsgDevice = NULL;
            return FALSE;
        }
        else
        {
            OutputDebugStringA("CreateFile KxEUc Suc!\n");
        }

		return TRUE;
	}
	
	BOOL Uninit()
	{
		m_bInitFlag = FALSE;
		SetEvent(m_hEchoEvent);
		if (m_hEchoThread)
		{
			if (WAIT_OBJECT_0 != ::WaitForSingleObject(m_hEchoThread, 500))
				::TerminateThread(m_hEchoThread, 0);
			::CloseHandle(m_hEchoThread);
			m_hEchoThread = NULL;
		}
		CloseHandle(m_hEchoEvent);
		KAVFreeSecurityAttribute(m_pEchoSAC);
		m_pEchoSAC = NULL;
		m_hEchoEvent = NULL;

		return DestoryAllIpc();
	}

	BOOL RegisterIpc(	
		/* [in] */ ULONG			ulClientID,              // 通信客户端ID
		/* [in] */ KucPCALLBACK	pCallbackRoutine,        // 回调函数指针
		/* [in] */ PVOID			pvParam                  // 给回调函数指定一个自定义参数
		)
	{
		IPC_MAP_SET::iterator iter;
        KXEUC_MSG_HEAD		  MsgHdr          = {0};
        DWORD                 dwReturnedBytes = 0;
        BOOL                  bRet            = FALSE;
        KIpcPort*             pIpcPort        = NULL;
        ULONG                 i               = 0;

        EnterCriticalSection(&m_SetCs);
        iter = m_IpcSet.find(ulClientID);
		if(iter == m_IpcSet.end())
		{
			pIpcPort = new KIpcPort();
            if (! pIpcPort->InitPort(
                ulClientID,
                KPortInfo::GetInst()->GetLinkNum(ulClientID),
                KPortInfo::GetInst()->GetSize(ulClientID),
                KPortInfo::GetInst()->GetSize2(ulClientID),
                pCallbackRoutine,
                pvParam
                )) 
			{
                bRet = FALSE;
                goto _Exit_;
			}
            
            for (i = 0; i < pIpcPort->GetLinNum(); ++i)
            {
                SendToDevice(
                    pIpcPort->GetIpc(i)->GetAddr(),
                    pIpcPort->GetIpc(i)->GetMapBuf(),
                    pIpcPort->GetIpc(i)->GetMapSize(),
                    pIpcPort->GetIpc(i)->GetMapSize2(),
                    KAV_UC_KEIPC_INIT
                    );
            }
            
            m_IpcSet.insert(IPC_MAP_SET::value_type(ulClientID, pIpcPort));
            bRet = TRUE;
		}
        else if (!iter->second->GetCloseStatus())
        {
            iter->second->SetCloseStatus(TRUE);
            for (i = 0; i < iter->second->GetLinNum(); ++i)
            {
                SendToDevice(
                    iter->second->GetIpc(i)->GetAddr(),
                    iter->second->GetIpc(i)->GetMapBuf(),
                    iter->second->GetIpc(i)->GetMapSize(),
                    iter->second->GetIpc(i)->GetMapSize2(),
                    KAV_UC_KEIPC_INIT
                    );
            }

            bRet = TRUE;
        }

        bRet =  TRUE;
_Exit_:
        LeaveCriticalSection(&m_SetCs);
		return bRet;	
	}

	BOOL UnregisterIpc(	
		/* [in] */ ULONG		ulClientID  // 通信客户端ID
		)
	{
        DWORD          dwReturnedBytes = 0;
        KXEUC_MSG_HEAD MsgHdr          = {0};
        ULONG          i               = 0;

        EnterCriticalSection(&m_SetCs);
		IPC_MAP_SET::iterator iter= m_IpcSet.find(ulClientID);
		if(iter != m_IpcSet.end())
		{
            iter->second->SetCloseStatus(FALSE);
            for (i = 0; i < iter->second->GetLinNum(); ++i)
            {
                MsgHdr.nIpcLinkPort = iter->second->GetIpc(i)->GetAddr();
                DeviceIoControl(
                    m_hMsgDevice,
                    KAV_UC_KEIPC_COLSE,
                    &MsgHdr,
                    sizeof( KXEUC_MSG_HEAD ),
                    NULL,
                    0,
                    &dwReturnedBytes,
                    NULL
                    );
            }
		}
        LeaveCriticalSection(&m_SetCs);
		
		return TRUE;
	}

    BOOL UnregisterAllIpc()
    {
        DWORD                 dwReturnedBytes = 0;
        KXEUC_MSG_HEAD        MsgHdr          = {0};
        ULONG                 i               = 0;
        IPC_MAP_SET::iterator iter;

        EnterCriticalSection(&m_SetCs);
        for(iter = m_IpcSet.begin(); 
            iter != m_IpcSet.end(); iter++)
        {
            if (iter->second->GetCloseStatus()){continue;}

            iter->second->SetCloseStatus(FALSE);
            for (i = 0; i < iter->second->GetLinNum(); ++i)
            {
                MsgHdr.nIpcLinkPort = iter->second->GetIpc(i)->GetAddr();
                DeviceIoControl(
                    m_hMsgDevice,
                    KAV_UC_KEIPC_COLSE,
                    &MsgHdr,
                    sizeof( KXEUC_MSG_HEAD ),
                    NULL,
                    0,
                    &dwReturnedBytes,
                    NULL
                    );
            }
        }
        LeaveCriticalSection(&m_SetCs);

        return TRUE;
    }

	BOOL DestoryAllIpc()
	{
        ULONG i = 0;
		IPC_MAP_SET::iterator  iter;
        EnterCriticalSection(&m_SetCs);
		for(iter = m_IpcSet.begin(); iter != m_IpcSet.end(); iter++)
		{
            for (i = 0; i < iter->second->GetLinNum(); ++i)
            {
                iter->second->GetIpc(i)->Uninit();
            }
		}
        LeaveCriticalSection(&m_SetCs);

		return TRUE;
	}

    BOOL SetVideoDevName(LPCWSTR lpName)
    {
        DWORD dwReturnedBytes = 0;
        WCHAR wszName[MAX_PATH + 1] = {0};
        if (!m_hMsgDevice || !lpName || wcslen(lpName) > MAX_PATH)
        {return FALSE;}

        wcscpy(wszName, lpName);
        BOOL bRet = DeviceIoControl(
            m_hMsgDevice,
            KAV_UC_SET_VIDEO_NAME,
            &wszName,
            sizeof(WCHAR) * (MAX_PATH + 1),
            NULL,
            0,
            &dwReturnedBytes,
            NULL
            );
        return bRet;
    }

private:
 	KUcFactory() 
    {
        m_hEchoThread = NULL;
        m_hMsgDevice = NULL;
        m_hEchoEvent = NULL;
        m_bInitFlag = FALSE;
        m_pEchoSAC = NULL;
        InitializeCriticalSection(&m_SetCs);
    }
 	~KUcFactory() {DeleteCriticalSection(&m_SetCs);}

    BOOL SendToDevice(
        ULONG nIpcLinkPort,
        PVOID pUserAddr,
        ULONG ulSize,
        ULONG ulSize2,
        DWORD IoControlCode
        )
    {   
        DWORD dwReturnedBytes = 0;
        KXEUC_MSG_HEAD MsgHdr = {0};

        if (!pUserAddr || !ulSize|| !ulSize2 ||
            -1 == nIpcLinkPort)
        {
            return FALSE;
        }

        MsgHdr.hPid = (HANDLE)GetCurrentProcessId();
        MsgHdr.nIpcLinkPort = nIpcLinkPort;
        MsgHdr.pUserAddr = pUserAddr;
        MsgHdr.ulSize = ulSize;
        MsgHdr.ulSize2 = ulSize2;
 
        BOOL bRet = DeviceIoControl(
            m_hMsgDevice,
            IoControlCode,
            &MsgHdr,
            sizeof( KXEUC_MSG_HEAD ),
            NULL,
            0,
            &dwReturnedBytes,
            NULL
            );

        return bRet;
    }

	typedef std::map<ULONG, KIpcPort*> IPC_MAP_SET;
	 
	IPC_MAP_SET m_IpcSet;

	HANDLE		m_hEchoThread;							// 心跳线程，让驱动超时重新链接
	HANDLE		m_hMsgDevice;							// 设备句柄
	BOOL		m_bInitFlag;
	HANDLE		m_hEchoEvent;
	PSECURITY_ATTRIBUTES m_pEchoSAC;
    CRITICAL_SECTION m_SetCs;
    
};
 
// 初始化统一通信
int WINAPI KucInit(
	/* [in] */ void* pvNotused /* = NULL */ 
	)
{
	return KUcFactory::GetInstance()->InitFactory();
}

void WINAPI KucUnInit()
{	
    // 此时不必清除所有资源，只需关闭即可
	//KUcFactory::GetInstance()->Uninit();
    KUcFactory::GetInstance()->UnregisterAllIpc();
}

int WINAPI KucRegisterCallback(
	/* [in] */ ULONG		ulClientID,              // 通信客户端ID
	/* [in] */ KucPCALLBACK	pCallbackRoutine,        // 回调函数指针
	/* [in] */ PVOID		pvParam                  // 给回调函数指定一个自定义参数
	)
{
	int nRet = E_FAIL;
    OutputDebugStringA("KXEUC::RegisterCallback\n");

	nRet = KUcFactory::GetInstance()->RegisterIpc(
		ulClientID, pCallbackRoutine, pvParam);
	if (!nRet) {nRet = CO_E_INIT_RPC_CHANNEL; goto _Exit_;}

	OutputDebugStringA("KXEUC::RegisterCallback Suc\n");
    nRet = S_OK;
_Exit_:
	return nRet;
}

int WINAPI KucUnregisterCallback(
	/* [in] */ ULONG    ulClientID					 // 通信客户端ID								   
	)
{
    KUcFactory::GetInstance()->UnregisterIpc(ulClientID);
	return 0;
}

BOOL WINAPI KucSetVideoDevName(LPCWSTR lpName)
{
    return KUcFactory::GetInstance()->SetVideoDevName(lpName);
}