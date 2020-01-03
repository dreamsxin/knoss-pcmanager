//////////////////////////////////////////////////////////////////////
///		@file		idl_http.h
///		@author		luopeng
///		@date		2008-9-8 09:07:58
///
///		@brief		仅用于兼容以前的 HTTP-RPC 调用
///		
//////////////////////////////////////////////////////////////////////

#pragma once
#include "idl_pipe.h"

class KxEHttpClientCommunication : public KxEPipeClientCommunication
{
public:
	/**
	* @brief		
	* @remark		若调用了HTTP版本的KxEIPCClientT，
					则忽略其指定的URL(即 pwszServerPipeName 参数视为 NULL )
	* @param[in]	: pwszServiceProviderName 为服务提供者的名字
	* @param[in]	: pwszServerPipeName 为该服务提供者所在进程的命名管道名
	*/
	KxEHttpClientCommunication(
		const wchar_t* pwszServiceProviderName,
		const wchar_t* pwszServerPipeName
		) : KxEPipeClientCommunication(pwszServiceProviderName, NULL)
	{
		pwszServerPipeName;
		//just for remove warning
	}
};

/// 针对HTTP的一个SP调用封装版本
template
<
	typename _Interface
>
class KxEHttpClientT : public KxEIPCClientT<_Interface, KxEHttpClientCommunication>
{
public:
	KxEHttpClientT(const wchar_t* pwszServerName = L"127.0.0.1")
		: KxEIPCClientT<_Interface, KxEHttpClientCommunication>(pwszServerName)
	{
	}
};

/**
 * @brief 用于将Interface扩充定义及HTTP-RPC的注册去转换,兼容以前仅针对HTTP-RPC的处理
 * @remark 其中模板参数Interface可以为单个的Service Provider接口,也可以为Serivce Provider的TYPELIST接口,其类型为:
 *         TYPELIST_2(IKxEScanService, IKxEFileMonService)或更多
 */
template
<
	typename Interface,
	int nPort  ///< 指定监定的端口号，如果为0，将使用平台的方式链接，否则自己创建一个http服务器
>
class KxEHttpServerT : public KxEIPCServerT<Interface>
{
public:
	
};

/**
 * @}
 */