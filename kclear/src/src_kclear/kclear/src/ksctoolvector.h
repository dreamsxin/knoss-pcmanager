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

//////////////////////////////////////////////////////////////////////////

class KscToolVector : public CWindowImpl<KscToolVector>
{
public:
	DECLARE_WND_CLASS(L"KscToolVector");

	KscToolVector();
	~KscToolVector();

	BEGIN_MSG_MAP_EX(KscToolVector)
		MSG_WM_SIZE(OnSize)
		MSG_WM_INITDIALOG(OnInitDialog)
	END_MSG_MAP()

	BOOL InitTools();
	void UnInitTools();
	BOOL SwitchTool(int nIndex);

protected:
	void OnSize(UINT nType, CSize size);
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

	BOOL LoadTool(int nIndex, const std::wstring& strDll);
	void UpdateToolSize(HWND hWnd);

	typedef HWND (__stdcall *ShowDlg_t)(HWND hParent);

	typedef struct tagKscTool {
		HMODULE hDll;
		ShowDlg_t pShowDlg;
		HWND hWnd;
	} KscTool;

	KscTool m_hTools[6];
	BOOL m_fSingleMode;
	RECT m_rcClient;
	BOOL m_fForKSafe;
};

//////////////////////////////////////////////////////////////////////////
