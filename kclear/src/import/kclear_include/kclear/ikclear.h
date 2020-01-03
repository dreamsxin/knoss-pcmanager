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

/********************************************************************
	created:	2010/09/17
	created:	17:9:2010   14:36
	filename: 	ikclear.h
	author:		Jiang Fengbing
	
	purpose:	清理模块对外接口
*********************************************************************/

#pragma once

#include <unknwn.h>

//////////////////////////////////////////////////////////////////////////

[
    uuid(35729581-fc15-402e-a6c4-0cdeaf6f2357)
]
interface IKClear
{
    virtual void __cdecl Release() = 0;
    virtual HWND __cdecl CreateKClear(HWND hParent) = 0;
    virtual BOOL __cdecl Navigate(const wchar_t* szNavigate) = 0;
    //virtual void __cdecl OnSize(const RECT* lprcWnd) = 0;
};

//////////////////////////////////////////////////////////////////////////

typedef HRESULT (__cdecl *CreateKClearObject_t)(REFIID riid, void** ppObject);
HRESULT __cdecl CreateKClearObject(REFIID riid, void** ppObject);

//////////////////////////////////////////////////////////////////////////
