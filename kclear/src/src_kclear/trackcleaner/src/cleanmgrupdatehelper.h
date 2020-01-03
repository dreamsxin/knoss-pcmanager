// Copyright (c) 2010 Kingsoft Corporation. All rights reserved.
// Copyright (c) 2010 The KSafe Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#include <atlbase.h>
#include <Unknwn.h>
#include "kclear/libupdhelper.h"

class  __declspec(uuid("81ab1cb2-e8f2-42e3-93a8-000632ded876")) CCleanMgrUpdateHelper:public IUpdateHelper
{
public:
	CCleanMgrUpdateHelper(void);
	virtual ~CCleanMgrUpdateHelper(void);

	virtual HRESULT STDMETHODCALLTYPE Combine( LPCWSTR lpwszDifflib ) ;
	virtual HRESULT STDMETHODCALLTYPE Notify( LPCWSTR lpwszFileName ) ;

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */  void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		ATLASSERT(ppvObject);
		if ( IsEqualIID(riid, __uuidof(IUpdateHelper)) )
		{
			IUpdateHelper *p = this;
			*ppvObject = p;
			return S_OK;
		}
		else if( IsEqualIID(riid, __uuidof(CCleanMgrUpdateHelper)) )
		{

			CCleanMgrUpdateHelper *p = this;
			*ppvObject = p;
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef( void)
	{
		return ++__ref;
	}

	virtual ULONG STDMETHODCALLTYPE Release( void) 
	{
		return __ref;
	}

	ULONG __ref;
	
};
