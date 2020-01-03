//////////////////////////////////////////////////////////////////////
///		@file		kxepipecallproduct.h
///		@author		liangguangcai
///		@date		[2009-4-30, 12:02:30]
///		@brief		向任意命名管道服务器端发送SP服务请求
///		@remark		注意 USE_KXE_DEBUG_LOG 宏的定义会影响到是否记录日志；
///					并且，如果定义了 USE_KXE_DEBUG_LOG，则程序会依赖于
///					kxebase(.lib)
///		@last changed:
///			$by:	    梁光彩<liangguangcai@kingsoft.com>
///			$changed:  增加了内存分配失败的错误码；修正代码风格
//////////////////////////////////////////////////////////////////////

#pragma once

#include <sstream>
#include <cassert>

#include <Windows.h>
#include "../kxeerror/kxeerror.h"
#include "../common/kxe_thread.h"
/*
* [注]
*	默认情况下不定义此宏，因此默认情况下不会依赖于 kxebase(.lib)，
*	若要记录 SP 的管道调用出错的日志，
*	则请在程序的工程预处理器中定义如下宏：
*		USE_KXE_DEBUG_LOG
*/
#ifdef USE_KXE_DEBUG_LOG
	#include "../include/kxe.h"
#endif


//------------------------------------------------------------------------------


#define KXEPIPE_WAIT_PIPE_INTERVAL 100	// WaitNamedPipe 时间间隔（毫秒）

#define KXEPIPE_MAX_CONNECT_TIMES 5		// 尝试连接指定的管道服务端的最大次数


//------------------------------------------------------------------------------


class KxEPipeCallProduct
{
public:
	KxEPipeCallProduct() : m_hConnectPipe(NULL)
	{

	}

	~KxEPipeCallProduct()
	{
		// 若成功建立过连接，则此时关闭这个管道实例
		if (NULL != m_hConnectPipe)
		{
			CloseHandle(m_hConnectPipe);
		}
	}

public:
	/**
	* @brief 向目标命名管道服务器发出调用请求数据并等待数据回复
	* @param[in] lpszPipeName 目标命名管道服务器端的命名管道名
	* @param[in] szSPAndMethod 目标客户端的服务提供者及方法组合 /ServiceProvider/Method
	* @param[in] lpszData 发出的数据
	* @param[in] uDataLength 发出的数据长度
	* @param[out] ppReturnData 返回的数据，需要传入一个指针地址，最后用ReleasePipeBuffer释放
	* @param[out] puDataLength 返回的数据长度
	* @return  0 成功，其他为失败错误码
	*/
	int __stdcall CallProductPipe(
		LPCWSTR lpszPipeName,
		const char* szSPAndMethod,	// 如果它为空，客户端会返回一个 “参数错误” 消息
		const char* lpszData,			// 如果它为空，客户端会返回一个 “参数错误” 消息
		unsigned int uDataLength,
		void** ppReturnData,
		unsigned int* puDataLength
		)

	{
		//1.... 检查参数正确性
		if (NULL == lpszPipeName ||NULL == szSPAndMethod ||
			NULL == lpszData || 0 == uDataLength)
		{
			return E_KXEPIPE_INCORRECT_PARAMETER;
		}

		//2.... 检查管道名与上次相比有无变化，若有变化则建立新的连接
		int nRet = 0;
		if (m_wstrServerPipeName != lpszPipeName)
		{
			m_wstrServerPipeName = lpszPipeName;
			m_hConnectPipe = NULL;	// 重新建立连接
		}

		// 检查管道是否存在
		nRet = this->CheckIfPipeExist();
		if (0 != nRet)
		{
			return nRet;
		}
		
		if (NULL == m_hConnectPipe)	// 防止重复建立连接
		{
			nRet = ConnectPipeServer();	
			if (0 != nRet)	// 如果失败，则m_hConnectPipe一定为NULL
			{
				// 将out参数置空后再返回
				*ppReturnData = NULL;
				*puDataLength = 0;
				return nRet;
			}
		}

		//3.... 向客户端发送数据
		std::ostringstream os;
		os << szSPAndMethod << "?json=" << lpszData;

		unsigned int uJsonDataLen = (unsigned int)(os.str().size()) * sizeof(char);
		char *pszSendBuffer = new char[sizeof(unsigned int) + uJsonDataLen];
		if (NULL == pszSendBuffer)
		{
			// 将out参数置空后再返回
			*ppReturnData = NULL;
			*puDataLength = 0;
			return E_KXEPIPE_NOT_ENOUGH_MEMORY;
		}

		memcpy(
			pszSendBuffer, 
			&uJsonDataLen, 
			sizeof(unsigned int)
			);
		memcpy(
			pszSendBuffer + (sizeof(unsigned int))/sizeof(char),
			os.str().c_str(), 
			uJsonDataLen);
	
		nRet = WritePipeSynchronously(
			pszSendBuffer,
			sizeof(unsigned int)+uJsonDataLen
			);
		if (0 != nRet)
		{
			FreeMemAndClosePipe(
				&pszSendBuffer,
				ppReturnData,
				puDataLength
				);
			return nRet;
		}
	
		/**
		 * 只要 WritePipeSynchronously 成功，即成功地发送出了数据，
		 * 则认为CallProductPipe成功，所以接着等待返回数据的到来
		 */

		//4.... 接收客户端的返回数据

		// 先读取返回数据的长度
		nRet = ReadPipeSynchronously(
			(char*)puDataLength,
			sizeof(unsigned int)
			);
		if (0 != nRet)
		{
			// 将out参数置空，释放动态分配的发送缓冲区，关闭管道句柄
			FreeMemAndClosePipe(
				&pszSendBuffer,
				ppReturnData,
				puDataLength
				);
			return nRet;
		}

		if ( 0 == *puDataLength)
		{
			// 将out参数置空，释放动态分配的发送缓冲区，关闭管道句柄
			FreeMemAndClosePipe(
				&pszSendBuffer, 
				ppReturnData, 
				puDataLength
				);
			return E_KXEPIPE_BAD_RETURN_DATA;
		}

		// 再读取返回数据
		*ppReturnData  = new char[ *puDataLength ];
		if (NULL == *ppReturnData)
		{
			// 将out参数置空，释放动态分配的发送缓冲区，关闭管道句柄
			FreeMemAndClosePipe(
				&pszSendBuffer, 
				ppReturnData, 
				puDataLength
				);
			return E_KXEPIPE_NOT_ENOUGH_MEMORY;
		}

		nRet = ReadPipeSynchronously(
			(char *)(*ppReturnData),
			*puDataLength
			);
		if (0 != nRet)
		{
			delete [] (char*)*ppReturnData;	
			FreeMemAndClosePipe(
				&pszSendBuffer, 
				ppReturnData, 
				puDataLength
				);
			return nRet;
		}

		//5.... CallProductPipe成功后，释放动态分配的发送缓冲区
		delete [] pszSendBuffer;
		return 0;
	}


	/**
	* @brief 释放通过此类分配的内存
	* @param[in] pBuffer 需要释放的内存地址
	* @return 0 成功，其他为失败错误码
	*/
	template<typename T>
	int __stdcall ReleasePipeBuffer(
		T** ppBuffer
		)
	{
		delete [] *ppBuffer;
		*ppBuffer = NULL;
		return 0;
	}


private:
	/**
	* @brief		测试指定名字的管道是否存在
	* @remark		
	* @return		0 成功，其他为错误码
	**/
	inline int __stdcall CheckIfPipeExist()
	{
		BOOL bSuccess = FALSE;

		bSuccess = WaitNamedPipe(
			m_wstrServerPipeName.c_str(), 
			NMPWAIT_USE_DEFAULT_WAIT
			);
		if (!bSuccess && ERROR_PIPE_BUSY != GetLastError())
		{
#ifdef USE_KXE_DEBUG_LOG
			KXE_DEBUG_LOG((
				LOG_ERROR,
				"kxepipecallproduct.cpp: WaitNamedPipe failed with error: 0x%8x",
				GetLastError()
				));
#endif
			return E_KXEPIPE_WAIT_NAMED_PIPE_FAILED;
		}

		return 0;
	}

	/**
	* @brief 建立与服务端命名管道服务器的连接
	* @return  0 成功，其他为失败错误码
	*/
	int __stdcall ConnectPipeServer()
 	{
		int nRet = this->CheckIfPipeExist();
		if (0 != nRet)
		{
			return nRet;
		}
		
		int nCount = 0;
		do 
		{
			m_hConnectPipe = CreateFile(
				m_wstrServerPipeName.c_str(),		// 命名管道名
				GENERIC_READ | GENERIC_WRITE,	// 管道读写模式：双向
				0, 
				(LPSECURITY_ATTRIBUTES)NULL, 
				OPEN_EXISTING, 
				FILE_ATTRIBUTE_NORMAL |
				SECURITY_SQOS_PRESENT |	// 允许服务器在本地系统上识别和模仿该客户
				SECURITY_IMPERSONATION,
				(HANDLE)NULL
				);
			if (INVALID_HANDLE_VALUE != m_hConnectPipe)
			{
				break;	// 成功，则 nCount 一定小于 KXEPIPE_MAX_CONNECT_TIMES
			}
			else
			{
				++nCount;
				Sleep(50);
			}
		} while (nCount != KXEPIPE_MAX_CONNECT_TIMES);

 		if (nCount == KXEPIPE_MAX_CONNECT_TIMES)
		{
#ifdef USE_KXE_DEBUG_LOG
			KXE_DEBUG_LOG((
				LOG_ERROR,
				"kxepipecallproduct.h: CreateFile failed with error: 0x%8x",
				GetLastError()
				));
#endif
			m_hConnectPipe = NULL;
			return E_KXEPIPE_CREATE_INSTANCE_ON_THE_PIPE_CLIENT_END_FAILED;
		}

		return 0;
	}

	/**
	*@brief 用于释放发送缓冲区、重置返回参数、关闭管道句柄
	*@param[in] pszSendBuffer 需要释放的发送缓冲区
	*@param[out] ppReturnData 为需要置空的返回数据地址
	*@param[out] puDataLength 为需要置为0的返回数据长度
	*@return void
	*/
	inline void FreeMemAndClosePipe(
		char** pszSendBuffer,
		void** ppReturnData,
		unsigned int * puDataLength
		)
	{
		// 将out参数置空后再返回
		*ppReturnData = NULL;
		*puDataLength = 0;
		delete [] *pszSendBuffer;  
		CloseHandle(m_hConnectPipe);
		m_hConnectPipe = NULL;
	}
	
	/**
	 * @brief 向管道同步地写入指定数目的数据
	 * @param[in] pszSendBuffer 发送缓冲区首址
	 * @param[in] dwSendDataLength 要发送的数据长度
	 * @return  0 成功，其他为错误码
	 */
	int __stdcall WritePipeSynchronously(
		const char* pszSendBuffer, 
		const DWORD dwSendDataLength
		)
	{
		assert(NULL != m_hConnectPipe);

		DWORD dwBytesWritten = 0;
		DWORD dwTransferOffset = 0;
		BOOL bSuccess = FALSE;
		while (dwSendDataLength != dwTransferOffset)
		{
			dwBytesWritten = 0;
			bSuccess = WriteFile(
				m_hConnectPipe, 
				(LPCVOID)(pszSendBuffer+dwTransferOffset),	// 每次循环都要调整文件指针
				dwSendDataLength - dwTransferOffset,
				&dwBytesWritten,
				NULL	// 同步方式
				);
			if (!bSuccess)
			{
#ifdef USE_KXE_DEBUG_LOG
				KXE_DEBUG_LOG((
					LOG_ALERT,
					"kxepipecallproduct.h: WritePipeSynchronously failed with error: 0x%8x",
					GetLastError()
					));
#endif
				return E_KXEPIPE_CLIENT_WRITE_FILE_FAILED;
			}
			else
			{
				dwTransferOffset += dwBytesWritten;
			}
		}

		return 0;
	}
	
	/**
	 * @brief 从管道同步地读取指定数目的数据
	 * @param[in, out] pszReceiveBuffer 接收缓冲区首址
	 * @param[in] dwReceiveDataLength 要接收的数据长度
	 * @return  0 成功，其他为错误码
	 */
	int __stdcall ReadPipeSynchronously(
		char* pszReceiveBuffer, 
		const DWORD dwReceiveDataLength
		)
	{
		assert(m_hConnectPipe);

		DWORD dwBytesRead = 0;
		DWORD dwTransferOffset = 0;
		BOOL bSuccess = 0;
		while (dwReceiveDataLength != dwTransferOffset)
		{
			dwBytesRead = 0;
			bSuccess = ReadFile(
				m_hConnectPipe,
				(LPVOID)(pszReceiveBuffer+dwTransferOffset),
				dwReceiveDataLength - dwTransferOffset,
				&dwBytesRead,
				NULL
				);
			if (!bSuccess)
			{
#ifdef USE_KXE_DEBUG_LOG
				KXE_DEBUG_LOG((
					LOG_ALERT,
					"kxepipecallproduct.h: ReadPipeSynchronously failed with error: 0x%8x",
					GetLastError()
					));
#endif
				return E_KXEPIPE_CLIENT_READ_FILE_FAILED;
			}
			else
			{
				dwTransferOffset += dwBytesRead;
			}
		}

		return 0;
	}

private:
	std::wstring m_wstrServerPipeName;
	HANDLE m_hConnectPipe;
};
