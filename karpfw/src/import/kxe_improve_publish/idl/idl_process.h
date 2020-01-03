//////////////////////////////////////////////////////////////////////
///		@file		idl_process.h
///		@author		luopeng
///		@date		2008-9-8 09:07:58
///
///		@brief	    进程相关启动的封装
///
//////////////////////////////////////////////////////////////////////

#pragma once
#include "idl_common.h"
#include "../serviceprovider/ikxeservicemgr.h"

/**
 * @defgroup kxeipc_idl_process_interface_group KXEIPC IDL Process Interface
 * @{
 */
template
<
	typename Interface,   ///< 服务提供者接口
	template
	<
		typename
	>
	class KxEIPCClientType = KxEPipeClientT
>
class KxEIpcClientEx : public KxEIPCClientType<Interface>
{
public:
	int StartServer(void)
	{
		std::wstring strServiceName;
		strServiceName = KxEIPCClientType<Interface>::getWin32ServiceName();
		
		if (strServiceName.empty())
		{
			bool bSystemMode = true;
			bSystemMode = KxEIPCClientType<Interface>::isSystemMode();
			return StartServiceProvider(bSystemMode);
		}
		else
		{
			return StartServiceProviderAsWin32Service(strServiceName);
		}
	}

protected:
	int StartServiceProviderAsWin32Service(const std::wstring strWin32ServiceName)
	{
		const wchar_t* pwszServiceName = KxEIPCClientType<Interface>::getServiceName();
		int nRet = -1;
		int nProcessId = 0;
		if (!this->IsServiceProviderRunning(pwszServiceName))
		{
			// 启动服务
			KxEIPCClientType<IKxEServiceMgr> serviceMgrClient;
			KXE_NULL_PARAEMETER null;

			// 服务启动完成才返回的
			nRet = serviceMgrClient.KxEStartService(strWin32ServiceName, null); 
		}

		return nRet;
	}

	int StartServiceProvider(bool bSystemProcess)
	{
		const wchar_t* pwszServiceName = KxEIPCClientType<Interface>::getServiceName();

		if (this->IsServiceProviderRunning(pwszServiceName))
			return 0;

		KxEIPCClientType<IKxEServiceMgr> serviceMgrClient;
		int nRet = -1;
		DWORD dwProcessId = 0;
		KXE_PRODUCT_INFO productInfo;
		nRet = serviceMgrClient.GetProductInfo(KxEIPCClientType<Interface>::getProductID(), productInfo);
		if (0 == nRet)
		{
			std::wstring strImagePath = productInfo.strProductPath;
			if (strImagePath.empty())
			{
				nRet = -1;
			}
			else
			{
				if (*strImagePath.rbegin() != '\\')
				{
					strImagePath.push_back('\\');
				}
				strImagePath += KxEIPCClientType<Interface>::getImageName();
				if (bSystemProcess)
				{
					nRet = serviceMgrClient.CreateProcessAsServer(
						strImagePath,
						dwProcessId
						);
				}
				else
				{
					nRet = serviceMgrClient.CreateProcessAsClient(
						strImagePath,
						dwProcessId
						);
				}
			}
		}

		if (nRet == 0)
		{
			for (int i = 0; i < 50; ++i)
			{
				BOOL bExist = false;
				nRet = serviceMgrClient.QueryProcessIdExist(
					dwProcessId,
					bExist
					);
				if (nRet == 0 && bExist)
				{
					if (this->IsServiceProviderRunning(pwszServiceName))
					{
						break;
					}
					Sleep(100);
				}
				else if (nRet != 0)
				{
					break;
				}
				else if (!bExist)
				{
					nRet = -1;
					break;
				}
			}
		}

		return nRet;
	}

private:
	/**
	* @brief		查询指定的 SP 是否正在运行
	* @remark		当前判断依据是：SP 所在的服务进程是否存在
	* @param[in]	: pwszSPName
	*/
	BOOL IsServiceProviderRunning(const wchar_t *pwszSPName) const
	{
		BOOL bRunning = FALSE;
		WCHAR szBuf[MAX_PATH] = {0};
		swprintf_s(
			szBuf,
			MAX_PATH,
			L"%s%s",
			kxe_get_config_sp_root_path(),
			pwszSPName
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
			return bRunning;
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
			return bRunning;
		}

		// 再读取返回数据
		LPBYTE lpData = new BYTE[dwBytes + sizeof(WCHAR)];
		
		if (NULL == lpData)
		{
			return bRunning;
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
			std::wstring wstrTargetPipeName = L"\\\\.\\Pipe\\";
			wstrTargetPipeName += (WCHAR *)lpData;

			bRunning = ::WaitNamedPipeW(
				wstrTargetPipeName.c_str(),
				50
				);
			if (!bRunning)
			{
				bRunning = (::GetLastError() == ERROR_PIPE_BUSY);
			}
		}

		delete[] lpData;
		return bRunning;
	}
};

/**
 * @}
 */
