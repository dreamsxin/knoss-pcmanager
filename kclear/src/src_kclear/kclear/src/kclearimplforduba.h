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

#include "kclear/ikisgui.h"
#include "kscmaindlg.h"

class kclearimplforduba: public IModule
{
public:
	kclearimplforduba(void);
	~kclearimplforduba(void);

	STDMETHOD(Init)();
	STDMETHOD(Uninit)();
	STDMETHOD(Create)(HWND hwndParent, RECT rect, HWND* phwndCreated);
	STDMETHOD(Show)(BOOL bShow);
	STDMETHOD(Move)(RECT rect);
	STDMETHOD(CanQuitNow)(int* pbCondition);
	STDMETHOD(SetMainWndNotify)(IMainWndNotify* pNotify);
	STDMETHOD(GetName)(LPTSTR lpszName, UINT* len);
	STDMETHOD(GetPages)(ISettingPage** ppISettingPage, ULONG* pFetched);
	STDMETHOD(SetParam)(LPCTSTR lpcszParam);

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in ] */ REFIID riid,
		/* [iid_is][out] */ void __RPC_FAR* __RPC_FAR* ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);

private:
	LONG	m_cRef;

private:
	CKscMainDlg* m_pDlgMain;
	HWND m_hwnd;
	TCHAR m_szCmd[MAX_PATH];
};
