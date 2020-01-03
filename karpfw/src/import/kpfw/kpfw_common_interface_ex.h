////////////////////////////////////////////////////////////////////////////////
//      
//      common include File for kpfw
//      
//      File      : kpfw_interface.h
//      Version   : 1.0
//      Comment   : 定义网镖服务和界面之间的rfc接口
//      
//      Create at : 2008-4-7
//      Create by : liupeng
//      
////////////////////////////////////////////////////////////////////////////////
#pragma once 

#include "SCOM/SCOM/SCOMBase.h"
#include "SCOM/SCOM/SCOMDef.h"

#ifndef STDPROC
#define STDPROC					virtual HRESULT __stdcall
#define STDPROC_(Type)			virtual Type __stdcall
#endif

//////////////////////////////////////////////////////////////////////////
// 流量信息
struct KPFWFLUX
{
	ULONGLONG	nRec;
	ULONGLONG	nSend;
};

// 实现类的CLSID
_declspec(selectany) extern const KSCLSID CLSID_KNetInfoImp = { 0x34b53966, 0x744b, 0x40c4, { 0xb6, 0xcd, 0xc8, 0xb7, 0x5f, 0x82, 0xcb, 0xb8 } };
