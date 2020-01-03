//////////////////////////////////////////////////////////////////////
///		@file		idl.h
///		@author		luopeng
///		@date		2008-9-8 09:07:58
///
///		@brief		用于http的中间封装
///		
//////////////////////////////////////////////////////////////////////

#pragma once
#include <string>
#include <map>
#include "../json/json.h"
#include "../include/kxe_serialize.h"
#include "../common/kxe_stl.h"
#include "../common/kxe_thread.h"
#include <sstream>
#include "../kxebase/base_module_clsid.h"
#include "../scom/scom/scom.h"
#include "../scom/scom/singleton.h"
#include "../kxebase/kxebase.h"

#include "../loki/TypeManip.h"
#include "../loki/static_check.h"
#include "../loki/Typelist.h"

#if _MSC_VER == 1200
#include "../loki/HierarchyGenerators.h"
#include "../common/kxeconv_atl.h"
#endif

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

/**
 @page kxehttp_json_data_struct http_json_data_struct
 @section kxe_http_json_input_one 客户端调用的Json数据结构:仅有一个参数的调用
 @code
 root :
 {
	 request:
	{
		param_count : "1"
		param_1
		{
			// any json data
		}
	}
 }
 @endcode
 @section kxe_http_json_input_two 客户端调用的Json数据结构:有两个参数的调用
 @code
 root :
 {
 	request:
	{
		param_count : "2"
		param_1
		{
			// any json data
		}
		param_2
		{
			// any json data
		}
	}
 }
 @endcode
 @section kxehttp_json_output 客户端返回的Json数据结构
 @code
 root
 {
	error:
	{
		error_code:0
		error_msg:""
	}
	result:
	{
		// any json data
	}
 }
 @endcode
 */

/**
 * @defgroup kxeipc_idl_interface_group KXEIPC IDL Interface
 * @{
 */

#if defined JSON_DLL
	#if _MSC_VER > 1200
		#pragma comment(lib, "json.lib")
	#else
		#pragma comment(lib, "jsonv6.lib")
	#endif

#else

	#if defined _DEBUG && defined _MT
		#if defined _DLL
			#pragma comment(lib, "lib_jsonD.lib")
		#else
			#pragma comment(lib, "lib_jsonMTD.lib")
		#endif // defined _MT
	#elif defined _MT
		#if defined _DLL
			#pragma comment(lib, "lib_json.lib")
		#else
			#pragma comment(lib, "lib_jsonMT.lib")
		#endif // defined _DLL
	#endif // #defined _DEBUG

#endif

#define DECLARE_VERSION(Major, Minor) \
	const static int nCurrentVersion = Major << 16 | Minor;\
	int nVersion;

// template
// <
// 	typename T1,
// 	typename T2
// >
// struct KxERequestT2Param
// {
// 	int nCount;
// 	T1 param1;
// 	T2 param2;
// };

_KAN_DOM_BEGIN

#if _MSC_VER > 1200
template
<
	typename _KDOMCursor, 
	typename T1,
	typename T2,
	template
	<
		typename,
		typename
	>
	class _Request
>
inline long KXMLDataExchange(_KDOMCursor& _domCur, _Request<T1, T2>& _obj, bool _bWrite)						 
{
	int _nRetCode = -1;

	KXE_JSON_DDX_MEMBER("param_count", nCount);
	KXE_JSON_DDX_MEMBER("param_1", param1);
	KXE_JSON_DDX_MEMBER("param_2", param2);

	return 0;
}
#endif

_KAN_DOM_END

// template
// <
// 	typename T
// >
// struct KxERequestT1Param
// {
// 	int nCount;
// 	T param1;
// };

_KAN_DOM_BEGIN

#if _MSC_VER > 1200
template
<
	typename _KDOMCursor, 
	typename T,
	template
	<
		typename
	>
	class _Request
>
inline long KXMLDataExchange(_KDOMCursor& _domCur, _Request<T>& _obj, bool _bWrite)						 
{
	int _nRetCode = -1;

	KXE_JSON_DDX_MEMBER("param_count", nCount);
	KXE_JSON_DDX_MEMBER("param_1", param1);

	return 0;
}
#endif

_KAN_DOM_END

/**
 * @brief 用于IPC-RPC调用时返回标志错误的结构
 */
typedef struct _KXE_ERROR
{
	int nError;  ///< 返回的错误码
	std::string strErrorMsg; ///< 返回的错误内容,可以为空
} KXE_ERROR;

KXE_JSON_DDX_BEGIN(KXE_ERROR)
	KXE_JSON_DDX_MEMBER("error_code", nError);
	KXE_JSON_DDX_MEMBER("error_msg", strErrorMsg);
KXE_JSON_DDX_END()

/**
 * @brief 用于作为空参数时的结构,程序员需要空参数时,可以直接引用此结构
 */
typedef struct _KXE_NULL_PARAMETER
{
	int nNullValue;
} KXE_NULL_PARAEMETER;

KXE_JSON_DDX_BEGIN(KXE_NULL_PARAEMETER)
	KXE_JSON_DDX_MEMBER_DEFAULT("NullValue", nNullValue, 0);
KXE_JSON_DDX_END()

template
<
	typename out_type
>
int kxe_translate_out_to_data(const KXE_ERROR& error, 
							  const out_type& out,
							  IKxERequestDispatchReturnBlock* pReturnMemoryBlock)
{
	KANDOM::KJSONDataAccess dataAccess;
	int nRet = dataAccess.LoadDOMName("root");
	if (nRet != 0)
	{
		return nRet;
	}
	std::string strBuffer;
	nRet = dataAccess.WriteData("error", const_cast<KXE_ERROR&>(error));
	if (nRet != 0)
	{
		return nRet;
	}
	if (error.nError == 0)
	{
		nRet = dataAccess.WriteData("result", const_cast<out_type&>(out));
		if (nRet != 0)
		{
			return nRet;
		}
	}
	nRet = dataAccess.ToString(&strBuffer);
	if (nRet != 0)
	{
		return nRet;
	}
	pReturnMemoryBlock->SetBuffer(strBuffer.c_str(), strBuffer.size());
	return nRet;
}

#if _MSC_VER > 1200

template
<
	typename _ret,
	typename _class,
	typename _in,
	typename _out
>
int kxe_translate_to_call(_class* ObjectPtr,
						  _ret (__stdcall _class::*_Pm)(const _in&, _out&), 
						  const char* pszRequest, 
						  IKxERequestDispatchReturnBlock* pReturnMemoryBlock)
{
	KxERequestT1Param<_in> request;
	_out out;

	KXE_ERROR err;
	err.nError = 0;

	{
		KANDOM::KJSONDataAccess dataAccess;
		err.nError = dataAccess.LoadString(pszRequest);
		if (err.nError == 0)
		{
			err.nError = dataAccess.ReadData("request", request);  // 失败?
		}
	}

	if (err.nError == 0)
	{
		/*if ((request.nVersion & 0xFFFF0000) != (request.nCurrentVersion & 0xFFFF0000))
		{
		err.nError = -1;
		}
		else*/
		{
			err.nError = (ObjectPtr->*_Pm)(request.param1, out);
		}
	}
	
	return kxe_translate_out_to_data(err, out, pReturnMemoryBlock);
}

template
<
	typename _ret,
	typename _class,
	typename _in1,
	typename _in2,
	typename _out
>
int kxe_translate_to_call(_class* ObjectPtr,
						  _ret (__stdcall _class::*_Pm)(const _in1&, const _in2&, _out&), 
						  const char* pszRequest, 
						  IKxERequestDispatchReturnBlock* pReturnMemoryBlock)
{	
	KxERequestT2Param<_in1, _in2> request;
	_out out = _out();
	KXE_ERROR err;
	err.nError = 0;

	{
		KANDOM::KJSONDataAccess dataAccess;
		err.nError = dataAccess.LoadString(pszRequest);
		if (err.nError == 0)
		{
			err.nError = dataAccess.ReadData("request", request);  // 失败?
		}
	}

	if (err.nError == 0)
	{
		/*if ((request.nVersion & 0xFFFF0000) != (request.nCurrentVersion & 0xFFFF0000))
		{
		err.nError = -1;
		}
		else*/
		{
			err.nError = (ObjectPtr->*_Pm)(request.param1, request.param2, out);
		}
	}

	return kxe_translate_out_to_data(err, out, pReturnMemoryBlock);
}

#else

#define KXE_TRANSLATE_TO_CALL(_class, _in, _out) \
	int kxe_translate_to_call(_class * ObjectPtr, \
							  int (__stdcall _class::*_Pm)(const _in&, _out&),\
							  const char* pszRequest, \
							  IKxERequestDispatchReturnBlock* pReturnMemoryBlock)\
	{\
		KxERequestT1Param<_in> request;\
		_out out;\
		KXE_ERROR err;\
		err.nError = 0;\
		{\
			KANDOM::KJSONDataAccess dataAccess;\
			err.nError = dataAccess.LoadString(pszRequest);\
			if (err.nError == 0)\
			{\
				err.nError = dataAccess.ReadData("request", request);\
			}\
		}\
		if (err.nError == 0)\
		{	\
			{\
				err.nError = (ObjectPtr->*_Pm)(request.param1, out);\
			}\
		}\
		return kxe_translate_out_to_data(err, out, pReturnMemoryBlock);\
	}


#define KXE_TRANSLATE_TO_CALL_2(_class, _in1, _in2, _out) \
	int kxe_translate_to_call(_class* ObjectPtr,\
							  int (__stdcall _class::*_Pm)(const _in1&, const _in2&, _out&), \
							  const char* pszRequest, \
							  IKxERequestDispatchReturnBlock* pReturnMemoryBlock)\
	{	\
		KxERequestT2Param<_in1, _in2> request;\
		_out out = _out();\
		KXE_ERROR err;\
		err.nError = 0;\
		{\
			KANDOM::KJSONDataAccess dataAccess;\
			err.nError = dataAccess.LoadString(pszRequest);\
			if (err.nError == 0)\
			{\
				err.nError = dataAccess.ReadData("request", request);\
			}\
		}\
		if (err.nError == 0)\
		{\
			err.nError = (ObjectPtr->*_Pm)(request.param1, request.param2, out);\
		}\
		return kxe_translate_out_to_data(err, out, pReturnMemoryBlock);\
	}

#endif

typedef 
int (*kxe_uri_method_callback)(IKxERequestDispatch* pDispatch, 
							   const char* pszRequest, 
							   IKxERequestDispatchReturnBlock* pReturnMemoryBlock
							   );

class KxEIPCServerDataManager
{
public:
	HRESULT Register(const wchar_t* pwszMethod, kxe_uri_method_callback pCallback)
	{
		m_mapMethodToCallback[pwszMethod] = pCallback;

		return S_OK;
	}

	kxe_uri_method_callback GetCallback(const wchar_t* pwszMethod)
	{
		std::map<std::wstring, kxe_uri_method_callback>::iterator iter = m_mapMethodToCallback.find(pwszMethod);
		if (iter == m_mapMethodToCallback.end())
		{
			return NULL;
		}
		else
		{
			return iter->second;
		}
	}

	DWORD GetTlsIndex()
	{
		return m_dwTlsIndex;
	}

	KxEIPCServerDataManager()
	{
		m_dwTlsIndex = TlsAlloc();
	}

	~KxEIPCServerDataManager()
	{
		TlsFree(m_dwTlsIndex);
	}

private:
	std::map<std::wstring, kxe_uri_method_callback> m_mapMethodToCallback;
	DWORD m_dwTlsIndex;
};

typedef SingletonHolder_Static<KxEIPCServerDataManager> KxEIPCServerDataManagerSinglton;

template
<
	typename IPCClient
>
class KxERegisterUriDummyT
{
public:
	KxERegisterUriDummyT(const wchar_t* pwszMethod, kxe_uri_method_callback pCallback)
	{
		std::wostringstream os;
		os << L"/" << IPCClient::getServiceName() << L"/" << pwszMethod;
		KxEIPCServerDataManagerSinglton::Instance().Register(os.str().c_str(), pCallback);
	}
};

/**
 * @brief 向IPC-RPC转换程序注册动作及处理函数
 * @param[in] _class 实现了该动作的类,并且从_interface中实现
 * @param[in] _interface 为程序员定义的接口,用于能提供的HTTP-RPC的功能
 * @param[in] _method 为处理函数,该函数已经在_interface中定义
 * @remark
 *    _method的定义结构为int __stdcall _method(struct A, struct B);
 */
#define KXE_REGISTER_COMMAND_PROC(_class, _interface, _method) \
	int kxe_uri_callback_##_class##_##_method(IKxERequestDispatch* pDispatch, const char* pszRequest, IKxERequestDispatchReturnBlock* pReturnMemoryBlock)\
{\
	STATIC_CHECK(SUPERSUBCLASS_STRICT(_interface, _class), SuperSubClass);\
	_class* pObject = static_cast<_class*>(pDispatch);\
	return kxe_translate_to_call(pObject, &_class::_method, pszRequest, pReturnMemoryBlock);\
}\
	static KxERegisterUriDummyT<KxEIPCClientT<_interface, KxEPipeClientCommunication> > dummy_##_class##_method(L#_method, &kxe_uri_callback_##_class##_##_method);

template <typename _CommType>
inline int kxe_call_server_subsystem_method_with_parameter(
	_CommType& comm,												
	const wchar_t* pwszSubsystem,
	const wchar_t* pwszMethod,
	const char* pszParameter,
	unsigned int uParameterSize,
	std::string& strResult
	)	// 这里是通用版本，除了http是特化版本，其他对本函数的调用(包括命名管道)都将使用此通用版本
{
	std::wostringstream os;
	os << L"/" << pwszSubsystem << L"/" << pwszMethod;

	return comm.Navigate(
		os.str().c_str(), 
		pszParameter, 
		uParameterSize, 
		strResult
		);
}

template
<	
	typename _CommType,
	typename _in,
	typename _out
>
int kxe_call_server_subsystem_method_with_type(
	_CommType& comm,											 
	const wchar_t* pwszSubsystem,
	const wchar_t* pwszMethod,
	_in& in,
	_out& out
	)
{
	KxERequestT1Param<_in> request;
	request.nCount = 1;
	request.param1 = in;

	std::string strInParameter;
	{
		KANDOM::KJSONDataAccess dataAccess;
		int nRet = dataAccess.LoadDOMName("root");
		if (nRet != 0)
		{
			return nRet;
		}

		nRet = dataAccess.WriteData("request", request);
		if (nRet != 0)
		{
			return nRet;
		}

		dataAccess.ToCompactString(&strInParameter);
		strInParameter = strInParameter.substr(0, strInParameter.size() - 1);
	}

	std::string strOutParameter;
	int nRet = kxe_call_server_subsystem_method_with_parameter(
		comm,
		pwszSubsystem,
		pwszMethod,
		strInParameter.c_str(),
		(unsigned int)strInParameter.size(),
		strOutParameter
		);
	if (nRet == 0)
	{
		KXE_ERROR error;
		KANDOM::KJSONDataAccess dataAccess;
		nRet = dataAccess.LoadString(strOutParameter.c_str());
		if (nRet == 0)
		{
			nRet = dataAccess.ReadData("error", error);
			if (nRet == 0)
			{
				nRet = error.nError;
			}
		}

		if (nRet == 0)
		{
			nRet = dataAccess.ReadData("result", out);
		}
	}

	return nRet;
}

//////////////////////////////////////////////////////////////////////////kxe_call_server_subsystem_method_with_type

template
<
	typename _CommType,
	typename _in1,
	typename _in2,
	typename _out
>
int kxe_call_server_subsystem_method_with_type(
	_CommType& comm,											 
	const wchar_t* pwszSubsystem,
	const wchar_t* pwszMethod,
	_in1& in1,
	_in2& in2,
	_out& out
	)
{
	KxERequestT2Param<_in1, _in2> request;
	request.nCount = 2;
	request.param1 = in1;
	request.param2 = in2;

	std::string strInParameter;
	{
		KANDOM::KJSONDataAccess dataAccess;
		int nRet = dataAccess.LoadDOMName("root");
		if (nRet != 0)
		{
			return nRet;
		}

		nRet = dataAccess.WriteData("request", request);
		if (nRet != 0)
		{
			return nRet;
		}

		dataAccess.ToCompactString(&strInParameter);
		strInParameter = strInParameter.substr(0, strInParameter.size() - 1);
	}

	std::string strOutParameter;
	int nRet = kxe_call_server_subsystem_method_with_parameter(
		comm,
		pwszSubsystem,
		pwszMethod,
		strInParameter.c_str(),
		(unsigned int)strInParameter.size(),
		strOutParameter
		);
	if (nRet == 0)
	{
		KXE_ERROR error;
		KANDOM::KJSONDataAccess dataAccess;
		nRet = dataAccess.LoadString(strOutParameter.c_str());
		if (nRet == 0)
		{
			nRet = dataAccess.ReadData("error", error);
			if (nRet == 0)
			{
				nRet = error.nError;
			}
		}

		if (nRet == 0)
		{
			nRet = dataAccess.ReadData("result", out);
		}
	}

	return nRet;
}

//////////////////////////////////////////////////////////////////////////
//add by qiuruifeng 2010.5.10

class KxEGetConfigServiceName
{
public:
	KxEGetConfigServiceName(const wchar_t* pszBaseServiceName)
	{
		Init(pszBaseServiceName);
	}
	~KxEGetConfigServiceName()
	{
	}
	const wchar_t* getServiceName(void)
	{
		return m_ServeiceName.c_str();
	}
private:
	void Init(const wchar_t* pszBaseServiceName)
	{
		if (pszBaseServiceName)
		{
			if (S_OK != kxe_get_config_service_name(pszBaseServiceName, m_ServeiceName))
				m_ServeiceName = pszBaseServiceName;
		}
	}
private:
	std::wstring m_ServeiceName;
};

template<wchar_t const* BaseServiceName>
class KxEServiceNameGettor
{
public:
	static const wchar_t* getServiceName(void);
};

template<wchar_t const* BaseServiceName>
const wchar_t* KxEServiceNameGettor<BaseServiceName>::getServiceName(void)
{
	static KxEGetConfigServiceName service_name_gettor(BaseServiceName);
	return service_name_gettor.getServiceName();
}

//这个模板类是用于在全局变量的构造中调用一次getServiceName,
//让里面的静态局部变量在进入main函数前由crt进行构造,否则如果是进入main函数后构造,
//如果是多线程同时调用,可能会重复初始化
template<class T>
class KxeAutoCallGetServiceName
{
public:
	KxeAutoCallGetServiceName()
	{
		T::getServiceName();
	}
};

template<class T>
class KxeStaticInitUitl
{
public:
	static T ms_obj;
};

template<class T>
T KxeStaticInitUitl<T>::ms_obj;

#if _MSC_VER > 1200

class KxEPipeClientCommunication;

/// IPC调用的模板类
template
<
	typename Interface,
	typename _CommType = KxEPipeClientCommunication  /// 通信载体类型：：http 或 命名管道...
>
class KxEIPCClientT : public Interface
{

};

// 下面是针对于参数 Interface（某个服务提供者名） 的特化：(注意：特化版本仍然是个模板，这里只是第一次特化)

/**
 * @brief HTTP-RPC接口申明后的扩展定义
 * @param[in] _Interface 为程序员定义的接口,用于能提供的HTTP-RPC的功能
 * @param[in] _Name IPC-RPC中使用的名字
 * @remark
 */
#define KXE_RCF_BEGIN(_Interface, _Name) \
	extern __declspec(selectany) wchar_t const g_ServeiceBaseName##_Interface[]=L##_Name;\
	typedef KxeStaticInitUitl< KxeAutoCallGetServiceName< KxEServiceNameGettor< g_ServeiceBaseName##_Interface > > > KxeAutoCallGetServiceNameDummyClass##_Interface;\
	template<typename _CommType> \
	class KxEIPCClientT<_Interface, _CommType> : public _Interface\
	{\
	private:\
		_CommType m_comm;\
	public:\
		KxEIPCClientT(const wchar_t* pwszServer = NULL) : m_comm(getServiceName(), pwszServer){}\
		~KxEIPCClientT(){}\
		static const wchar_t* getServiceName() \
		{\
			return KxEServiceNameGettor< g_ServeiceBaseName##_Interface >::getServiceName();\
		}\
		static const wchar_t* getBaseServiceName() \
		{\
		   return L##_Name;\
		}\
		int GetServerPort(short& sPort)\
		{\
			return m_comm.GetServerPort(sPort);\
		}

/**
* @brief HTTP-RPC接口申明后的扩展定义
* @param[in] _Interface 为程序员定义的接口,用于能提供的HTTP-RPC的功能
* @param[in] _Name IPC-RPC中使用的名字
* @param[in] _ImageName SP的EXE名字
* @param[in] _ProductID SP所属的产品ID
* @param[in] _ServiceName 如果SP是服务程序的话，这里是SP服务的名字，否则是空串""
* @param[in] _bSystemMode 说明SP运行是否需要SYSTEM权限。
* @remark
*/
#define KXE_RCF_BEGIN_EX(_Interface, _Name, _ImageName, _ProductID, _ServiceName, _bSystemMode) \
	extern __declspec(selectany) wchar_t const g_ServeiceBaseName##_Interface[]=L##_Name;\
	typedef KxeStaticInitUitl< KxeAutoCallGetServiceName< KxEServiceNameGettor< g_ServeiceBaseName##_Interface > > > KxeAutoCallGetServiceNameDummyClass##_Interface;\
	template<typename _CommType> \
	class KxEIPCClientT<_Interface, _CommType> : public _Interface\
	{\
	private:\
		_CommType m_comm;\
	public:\
		KxEIPCClientT(const wchar_t* pwszServer = NULL) : m_comm(getServiceName(), pwszServer){}\
		~KxEIPCClientT(){}\
		static const wchar_t* getServiceName() \
		{\
			return KxEServiceNameGettor< g_ServeiceBaseName##_Interface >::getServiceName();\
		}\
		static const wchar_t* getBaseServiceName() \
		{\
			return L##_Name;\
		}\
		int GetServerPort(short& sPort)\
		{\
			return m_comm.GetServerPort(sPort);\
		}\
		static const wchar_t* getImageName() \
		{\
			return L##_ImageName;\
		}\
		static const wchar_t* getProductID() \
		{\
			return L##_ProductID;\
		}\
		static const wchar_t* getWin32ServiceName() \
		{\
			return L##_ServiceName;\
		}\
		static bool isSystemMode()\
		{\
			return _bSystemMode;\
		}

/**
 * @brief IPC-RPC接口申明后的扩展定义,扩充定义指定的函数
 * @param[in] _Function 为程序员定义的接口函数,用于能提供的HTTP-RPC的功能
 * @param[in] _In IPC-RPC中,客户端传入的数据结构
 * @param[in] _Out IPC-RPC中,客户端接受返回数据
 * @remark
 */
#define KXE_RCF_METHOD(_Function, _In, _Out)	\
	int __stdcall _Function(const _In& in, _Out& out)\
	{\
		return kxe_call_server_subsystem_method_with_type(\
		m_comm,\
		getServiceName(),\
		L#_Function,\
		const_cast<_In&>(in),\
		out\
		);\
	}

/**
* @brief RPC接口申明后的扩展定义,扩充定义指定的函数
* @param[in] _Function 为程序员定义的接口函数,用于能提供的HTTP-RPC的功能
* @param[in] _In1 IPC-RPC中,客户端传入的数据结构
* @param[in] _In2 IPC-RPC中,客户端传入的数据结构
* @param[in] _Out IPC-RPC中,客户端接受返回数据
* @remark
*/
#define KXE_RCF_METHOD_2(_Function, _In1, _In2, _Out)	\
	int __stdcall _Function(const _In1& in1, const _In2& in2, _Out& out)\
	{\
		return kxe_call_server_subsystem_method_with_type(\
		m_comm,\
		getServiceName(),\
		L#_Function,\
		const_cast<_In1&>(in1),\
		const_cast<_In2&>(in2),\
		out\
		);\
	}

/**
 * @brief 表示IPC-RPC接口申明后的扩展定义完成
 */
#define KXE_RCF_END() };

#else

class KxEPipeClientCommunication;

/// IPC调用的模板类
template
<
	typename Interface,
	typename CommType
>
class KxEIPCClientT : public Interface
{

};

// VC6 下不能对模板进行偏特化，所以只能完全特化

/**
 * @brief HTTP-RPC接口申明后的扩展定义
 * @param[in] _Interface 为程序员定义的接口,用于能提供的HTTP-RPC的功能
 * @param[in] _Name IPC-RPC中使用的名字
 * @remark
 */
#define KXE_RCF_BEGIN(_Interface, _Name) \
	extern __declspec(selectany) wchar_t const g_ServeiceBaseName##_Interface[]=L##_Name;\
	typedef KxeStaticInitUitl< KxeAutoCallGetServiceName< KxEServiceNameGettor< g_ServeiceBaseName##_Interface > > > KxeAutoCallGetServiceNameDummyClass##_Interface;\
	template<> \
	class KxEIPCClientT<_Interface, KxEPipeClientCommunication> : public _Interface\
	{\
	private:\
		KxEPipeClientCommunication m_comm;\
	public:\
		KxEIPCClientT(const wchar_t* pwszServer = NULL) : m_comm(getServiceName(), pwszServer){}\
		~KxEIPCClientT(){}\
		static const wchar_t* getServiceName() \
		{\
			return KxEServiceNameGettor<g_ServeiceBaseName##_Interface>::getServiceName();\
		}\
		static const wchar_t* getBaseServiceName() \
		{\
			return L##_Name;\
		}\
		int GetServerPort(short& sPort)\
		{\
			return m_comm.GetServerPort(sPort);\
		}

/**
 * @brief IPC-RPC接口申明后的扩展定义,扩充定义指定的函数
 * @param[in] _Function 为程序员定义的接口函数,用于能提供的HTTP-RPC的功能
 * @param[in] _In IPC-RPC中,客户端传入的数据结构
 * @param[in] _Out IPC-RPC中,客户端接受返回数据
 * @remark
 */
#define KXE_RCF_METHOD(_Function, _In, _Out)	\
	int __stdcall _Function(const _In& in, _Out& out)\
	{\
		return kxe_call_server_subsystem_method_with_type(\
		m_comm,\
		getServiceName(),\
		L#_Function,\
		const_cast<_In&>(in),\
		out\
		);\
	}

/**
* @brief RPC接口申明后的扩展定义,扩充定义指定的函数
* @param[in] _Function 为程序员定义的接口函数,用于能提供的HTTP-RPC的功能
* @param[in] _In1 IPC-RPC中,客户端传入的数据结构
* @param[in] _In2 IPC-RPC中,客户端传入的数据结构
* @param[in] _Out IPC-RPC中,客户端接受返回数据
* @remark
*/
#define KXE_RCF_METHOD_2(_Function, _In1, _In2, _Out)	\
	int __stdcall _Function(const _In1& in1, const _In2& in2, _Out& out)\
	{\
		return kxe_call_server_subsystem_method_with_type(\
		m_comm,\
		getServiceName(),\
		L#_Function,\
		const_cast<_In1&>(in1),\
		const_cast<_In2&>(in2),\
		out\
		);\
	}

/**
 * @brief 表示IPC-RPC接口申明后的扩展定义完成
 */
#define KXE_RCF_END() };

#endif

class KxETlsValueGuard
{
public:
	KxETlsValueGuard(void* pValue)
	{
		TlsSetValue(KxEIPCServerDataManagerSinglton::Instance().GetTlsIndex(), pValue);
	}

	~KxETlsValueGuard()
	{
		TlsSetValue(KxEIPCServerDataManagerSinglton::Instance().GetTlsIndex(), NULL);
	}
};



class KxEIPCServer : public IKxERequestDispatch
{
public:
	KxEIPCServer()
	{
		
	}

	~KxEIPCServer()
	{
	}

	virtual int __stdcall Dispatch(
		const char* pszMethod, 
		const char* pszRequest, 
		IKxERequestDispatchReturnBlock* pReturnBlock,
		IKxERequestControl* pControl
		)
	{
		KxETlsValueGuard guard(pControl);
		
		kxe_uri_method_callback pCallback = KxEIPCServerDataManagerSinglton::Instance().GetCallback(
			CA2WEX<>(pszMethod, CP_UTF8)
			);
		if (pCallback)
		{
			return pCallback(this, pszRequest, pReturnBlock);
		}
		else 
		{
			return E_KXEHTTP_SERVICE_METHOD_NOT_FOUND;
		}
	}

	int RegisterServiceToServer(const wchar_t* pwszServiceName)
	{
		return kxe_base_register_sp_dispatch(
			pwszServiceName,
			this
			);
	}

	int UnregisterServiceFromServer(const wchar_t* pwszServiceName)
	{
		return kxe_base_unregister_sp_dispatch(
			pwszServiceName,
			this
			);
	}

protected:
	std::vector<std::wstring> m_vecServiceName;
};

#if _MSC_VER > 1200

template
<
	typename T
>
class KxEIPCServerWrapper : public T,
							public KxEIPCServer
{
public:
	KxEIPCServerWrapper()
	{
		m_vecServiceName.push_back(KxEIPCClientT<T>::getServiceName());
	}

};

template
<
>
class KxEIPCServerWrapper<Loki::NullType> : public KxEIPCServer
{
};

template
<
	typename T1,
	typename T2
>
class KxEIPCServerWrapper<Loki::Typelist<T1, T2> > : public T1,
													 public KxEIPCServerWrapper<T2>
{
public:
	KxEIPCServerWrapper()
	{
		m_vecServiceName.push_back(KxEIPCClientT<T1>::getServiceName());
	}

};

#else

template
<
	typename T1,
	typename T2
>
class KxEIPCServerWrapperUnit : public T1,
							    public T2
{
public:
	KxEIPCServerWrapperUnit()
	{
		m_vecServiceName.push_back(KxEIPCClientT<T1, KxEPipeClientCommunication>::getServiceName());
	}

};

struct KxEIPCServerWrapperUnitWrapper 
{
	template <class T1, class T2> 
	struct In 
	{
		typedef KxEIPCServerWrapperUnit<T1, T2> type;
	}; 
};


template
<
	typename TList,
	typename Unit = KxEIPCServerWrapperUnitWrapper
>
class KxEIPCServerWrapper : public Loki::GenLinearHierarchy<TList, Unit, KxEIPCServer>
{
public:
	KxEIPCServerWrapper()
	{
		
	}

};

#endif


/**
 * @brief 用于将Interface扩充定义及IPC-RPC的注册去转换
 * @remark 其中模板参数Interface可以为单个的Service Provider接口,也可以为Serivce Provider的TYPELIST接口,其类型为:
 *         TYPELIST_2(IKxEScanService, IKxEFileMonService)或更多
 */
template
<
	typename Interface
>
class KxEIPCServerT : public KxEIPCServerWrapper<Interface>
{
public:
	/**
	 * @brief 用于向HTTP-RPC注册回调
	 * @return 0 成功, 其他为失败错误码
	 */
	int RegisterToServer()
	{
		for (unsigned int i = 0; i < m_vecServiceName.size(); ++i)
		{
			int nRet = RegisterServiceToServer(
				m_vecServiceName[i].c_str()
				);
			if (nRet != 0 && i > 0)
			{
				for (int j = i - 1; j >= 0; --j)
				{
					UnregisterServiceFromServer(
						m_vecServiceName[j].c_str()
						);
				}
				return nRet;
			}
		}

		return 0;
	}

	/**
	 * @brief 用于向HTTP-RPC取消回调
	 * @return 0 成功, 其他为失败错误码
	 */
	int UnregisterServer()
	{
		for (unsigned int i = 0; i < m_vecServiceName.size(); ++i)
		{
			UnregisterServiceFromServer(
				m_vecServiceName[i].c_str()
				);
		}
		return 0;
	}

	/**
	 * @brief 模拟客户端连接的权限
	 */
	int KxEImpersonateClient()
	{
		IKxERequestControl* pControl = (IKxERequestControl*)TlsGetValue(
			KxEIPCServerDataManagerSinglton::Instance().GetTlsIndex()
			);
		if (pControl == NULL)
		{
			return E_KXEHTTP_REQUEST_CONTROL_NOT_EXIST;
		}

		return pControl->KxEImpersonateClient();
	}

	/**
	 * @brief 恢复当前进程的权限
	 */
	int KxERevertToSelf()
	{
		IKxERequestControl* pControl = (IKxERequestControl*)TlsGetValue(
			KxEIPCServerDataManagerSinglton::Instance().GetTlsIndex()
			);
		if (pControl == NULL)
		{
			return E_KXEHTTP_REQUEST_CONTROL_NOT_EXIST;
		}

		return pControl->KxERevertSelf();
	}
};


/**
 * @}
 */
