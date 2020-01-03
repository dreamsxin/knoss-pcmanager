//////////////////////////////////////////////////////////////////////
///		@file		idl_pipe.h
///		@author		梁光彩
///		@date		2009-4-8 30:07:58
///
///		@brief		用于pipe的中间封装
///		
//////////////////////////////////////////////////////////////////////

#pragma once
#include "idl_common.h"
#include "idl_util_inl.h"
#include "../kxeerror/kxeerror.h"
#include "../common/kxe_thread.h"
#include "kxepipecallproduct.h"

/**
 * @defgroup kxepipe_idl_interface_group KXEPipe IDL Interface
 * @{
 */

class KxEPipeClientCommunication
{
public:

	/**
	* @brief		通过注册表查询 SP 管道名的构造方式
	* @remark		若 pwszServerPipeName 为空，则需从注册表中查询目标管道名
	* @param[in]	: pwszServiceProviderName 为服务提供者的名字
	* @param[in]	: pwszServerPipeName 为该服务提供者所在进程的命名管道名
	*/
	KxEPipeClientCommunication(
		const wchar_t* pwszServiceProviderName,
		const wchar_t* pwszServerPipeName
		):m_bGetPipeNameFromReg(TRUE)
	{
		if (NULL != pwszServiceProviderName)
		{
			m_wstrServiceProviderName = pwszServiceProviderName;
		}

		if (NULL != pwszServerPipeName)
		{
			m_wstrTargetPipeName		= pwszServerPipeName;
			m_bTargetPipeNameAccquired	= TRUE;
			m_bGetPipeNameFromReg		= FALSE;
		}
		else
		{
			int nRet = idl_util_get_sp_pipe_name_from_registry(
				m_wstrServiceProviderName,
				m_wstrTargetPipeName
				);
			if (0 != nRet)
			{
#ifdef USE_KXE_DEBUG_LOG
				KXE_DEBUG_LOG((LOG_ERROR, "It failed to query the specified SP's pipe name in the registry with error: 0x%8x!", nRet));
#endif
			}
			m_bTargetPipeNameAccquired = (0 == nRet);
		}
	}

	int Navigate(
		const wchar_t* pwszSpAndMethod,
		const char* pszParameter,
		unsigned int uParameterSize,
		kxe::string& strResult
		)
	{
		int nRet				= -1;
		BOOL bIsGetPipeNameNow	= FALSE;//在这次函数调用中从注册表获取了管道名
		KxEThreadMutexGuard guard(m_Mutex);
		if (!m_bTargetPipeNameAccquired && m_bGetPipeNameFromReg)
		{
			nRet = idl_util_get_sp_pipe_name_from_registry(
				m_wstrServiceProviderName,
				m_wstrTargetPipeName
				);
			if (0 != nRet)
			{
#ifdef USE_KXE_DEBUG_LOG
				KXE_DEBUG_LOG((LOG_ERROR, "It failed to query the specified SP's pipe name in the registry with error: 0x%8x!!", nRet));
#endif
				return E_KXEPIPE_SP_PIPE_NAME_NOT_FOUND_IN_REGISTRY;
			}

			bIsGetPipeNameNow = TRUE;
		}

		nRet = this->CheckIfPipeExist(m_wstrTargetPipeName.c_str());
		if (0 == nRet)
		{
			m_bTargetPipeNameAccquired = TRUE;
		}
		else if (!bIsGetPipeNameNow && m_bGetPipeNameFromReg)
		{
			nRet = idl_util_get_sp_pipe_name_from_registry(
				m_wstrServiceProviderName,
				m_wstrTargetPipeName
				);
			if (0 != nRet)
			{
#ifdef USE_KXE_DEBUG_LOG
				KXE_DEBUG_LOG((LOG_ERROR, "It failed to query the specified SP's pipe name in the registry with error: 0x%8x!!!", nRet));
#endif
				return E_KXEPIPE_SP_PIPE_NAME_NOT_FOUND_IN_REGISTRY;
			}

			nRet = this->CheckIfPipeExist(m_wstrTargetPipeName.c_str());
		}

		if (nRet != 0)
		{
			m_bTargetPipeNameAccquired	= FALSE;
			m_wstrTargetPipeName		= L"";
			return nRet;
		}

		char *pszOutParameter = NULL;
		unsigned int uOutParameterLen = 0;

		nRet = m_callProduct.CallProductPipe(
			m_wstrTargetPipeName.c_str(),
			CW2A(pwszSpAndMethod, CP_UTF8),
			pszParameter,
			uParameterSize,
			(void **)&pszOutParameter,
			&uOutParameterLen
			);
		if (0 != nRet) // 此时由 pszOutParameter 动态分配的内存将被CallProductPipe自动释放，所以不用再显式调用ReleasePipeBuffer
		{
			return nRet;
		}
		else
		{
			strResult = "";	// 以防 strResult 不空且长于 pszOutParameter
			strResult.insert(0, pszOutParameter, uOutParameterLen);
			m_callProduct.ReleasePipeBuffer(&pszOutParameter);
			return 0;
		}
	}

	int GetServerPort(short& sPort)
	{
		sPort;//just for remove warning
		return -1;
	}

private:
	/**
	* @brief		测试指定名字的管道是否存在
	* @remark		
	* @return		0 成功，其他为错误码
	**/
	inline int __stdcall CheckIfPipeExist(LPCWSTR pwszPipeName)
	{
		BOOL bSuccess = FALSE;

		bSuccess = WaitNamedPipe(
			pwszPipeName, 
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

private:
	std::wstring m_wstrServiceProviderName;
	std::wstring m_wstrTargetPipeName;

	BOOL m_bTargetPipeNameAccquired;	/*	标志目标管道名是否已经获取到	*/
	BOOL m_bGetPipeNameFromReg;
	/**
	 * 成为对象后，此对象就可以保存与服务连接使用的管道实例句柄，
	 * 不用在每次调用CallProductPipe时都去与服务器重新建立一次连接
	 * 当KxEPipeClientCommunication对象被析构时，KxEPipeCallProduct的析构函数会被调用，
	 * 此时会关闭与服务连接使用的管道实例句柄
	 */
	KxEThreadMutex		m_Mutex;
	KxEPipeCallProduct	m_callProduct;	
};

//------------------------------------------------------------------------------

template
<
	typename _Interface
>
class KxEPipeClientT : public KxEIPCClientT<_Interface, KxEPipeClientCommunication>
{
public:

	/**
	* @param[in]	: pwszServerPipeName 为传入的目标命名管道名
	* @remark		如果为空，则自动从注册表中查找 _Interface 对应的 SP 所在进程的管道名;
	*				如果不空，则就以 pwszServerPipeName 为目标管道名
	*/
	KxEPipeClientT(const wchar_t* pwszServerPipeName = NULL)
		: KxEIPCClientT<_Interface, KxEPipeClientCommunication>(pwszServerPipeName)
	{
	}
};

//检查sp是否已经加载
inline int kxe_check_is_sp_loaded(const wchar_t* pszSPName, BOOL& bIsLoaded)
{
	if (NULL == pszSPName)
		return -1;

	std::wstring strSPName = pszSPName;
	std::wstring strTargetPipeName;

	bIsLoaded = FALSE;
	int nTmpResult = idl_util_get_sp_pipe_name_from_registry(
		strSPName,
		strTargetPipeName
		);
	if (0 == nTmpResult)
	{
		BOOL bSuccess = FALSE;
		bSuccess = WaitNamedPipe(strTargetPipeName.c_str(), 0);
		if (bSuccess || ERROR_PIPE_BUSY == GetLastError())
		{
			bIsLoaded = TRUE;
		}
	}
	return 0;
}

/**
 * @}
 */