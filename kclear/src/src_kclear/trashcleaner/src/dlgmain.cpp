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


#include "stdafx.h"
#include "trashcleaner.h"
#include "dlgmain.h"
#include "trashdefine.h"
#include "trashpublicfunc.h"
#include "kuimsgbox.h"
#include "kvideocache.h"
#include "stubbornfiles.h"
#include "misc/KppUserLog.h"
#include "misc/kregfunction.h"
#include "misc/IniEditor.h"
#include "kscviewlog.h"
#include "safeexam/beikesafeexam.h"
#include "beikesafe/bkmsgdefine.h"
#include "beikesafe/bksafeexamcall.h"
//#include "iefix/iefixeng.h"

extern HWND g_hMainHwnd;

CFindFileTraverse find;

CMainDialog::CMainDialog():
m_hThread(NULL),
m_dwThreadID(0)
{
	m_nDestroy = FALSE;
	m_nCurrentFindIndex = -1;
	is_init = true;
	Is_Scan = FALSE;
	m_nProIndex = -1;
	m_nDelTotalCount = 0;
	m_nDelTotalSize = 0;
	CStubbornFiles::Instance().Init();
	TCHAR szValueBuffer[MAX_PATH] = { 0 };
	DWORD len = MAX_PATH;
	DWORD dwType = REG_DWORD;
	m_hEvent = NULL;
	m_pdlgDel = NULL;
	m_insertThread = NULL;
	m_dwInsertThreadID = 0;

	GetRegistryValue(HKEY_CURRENT_USER,
		L"SOFTWARE\\KSafe\\KClear",
		L"DbgLog",
		&dwType,
		(LPBYTE)szValueBuffer,
		&len
		);

	if(wcslen(szValueBuffer)>0)
	{
		KppUserLog::UserLogInstance().m_bWrite = TRUE;
	}

	wchar_t szAppPath[MAX_PATH] = { 0 };
	GetModuleFileNameW(HMODULE_THIS, szAppPath, MAX_PATH);
	PathRemoveFileSpecW(szAppPath);
	::PathAppend(szAppPath,L"cfg\\lastrecord.inf");
	m_strCacheFile = szAppPath;

	


}

CMainDialog::~CMainDialog()
{
	IniEditor IniEdit;
	IniEdit.SetFile(m_strCacheFile.GetBuffer());

	for (int j = 0;j < m_config.m_ConfigData.size();j++)
	{
		if (m_config.m_ConfigData[j].bDefaultSelect)
		{
			IniEdit.WriteDWORD(L"kclear_checke_laji",m_config.m_ConfigData[j].strFriendName.GetBuffer(),1);

		}
		else 
		{
			IniEdit.WriteDWORD(L"kclear_checke_laji",m_config.m_ConfigData[j].strFriendName.GetBuffer(),0);
		}
	}
	m_pAppVersion.UnInit(); 
	CStubbornFiles::Instance().UnInit();

}

LRESULT CMainDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
{

	m_pAppVersion.Init(NULL);

	m_btn_rescan.SubclassWindow(GetDlgItem(IDC_RESCAN));
	m_btn_rescan.SetBitmapButtonExtendedStyle(BMPBTN_HOVER);
	m_btn_rescan.SetBtnImages(IDB_BUTTON_COMMON_NORMAL, IDB_BUTTON_COMMON_DOWN,IDB_BUTTON_COMMON_HOVER,IDB_BUTTON_COMMON_DISABLE );
	m_btn_rescan.SetImages(0, 1, 2, 3);
	m_btn_rescan.SetTitle(33);

	m_btn_clearfile.SubclassWindow(GetDlgItem(IDC_CLEARFILE));
	m_btn_clearfile.SetBitmapButtonExtendedStyle(BMPBTN_HOVER);
	m_btn_clearfile.SetBtnImages(IDB_BUTTON_COMMON_NORMAL, IDB_BUTTON_COMMON_DOWN,IDB_BUTTON_COMMON_HOVER,IDB_BUTTON_COMMON_DISABLE );
	m_btn_clearfile.SetImages(0, 1, 2, 3);
	m_btn_clearfile.SetTitle(34);

	SET_HYPERCR_LINK(m_recommand, IDC_RCOMD);



	m_check_all.Attach(GetDlgItem(IDC_CHECK1)); 

	m_check_static.SubclassWindow(GetDlgItem(IDC_CHECK_STATIC));
	m_check_static.SetTransparent(TRUE);
	m_check_static.SetTextColor(RGB(0,0,0));

	CBitmap bmpCache;
	CBitmap bmpDownload;
	CBitmap bmpExp;
	CBitmap bmpIE;
	CBitmap bmpPerson;
	CBitmap bmpRead;
	CBitmap bmpSys;
	CBitmap bmpUpdate;
	CBitmap bmpUTEMP;
	CBitmap bmpBrower;
	CBitmap bmpChrome;
	CBitmap bmpVedio;
	CBitmap bmpKU6;
	CBitmap bmpPPS;
	CBitmap bmpPPTV;
	CBitmap bmpQQLIVE;
	CBitmap bmpQVOD;
	CBitmap bmpTUDOU;
	CBitmap bmpXUNLEIKANKAN;
	CBitmap bmpYOUKU;
	CBitmap bmpFengxing;
	CBitmap bmpLNK;
	CBitmap bmpDump;
	CBitmap bmpMathon;
	CBitmap bmpOpera;
	CBitmap bmpFirefox;
	CBitmap bmpSogo;
	CBitmap bmpErrrpot;
	CBitmap bmpLogfile;
	CBitmap bmpSuolvtu;
	CBitmap bmpHuishouzhan;

	bmpCache.LoadBitmap(IDB_BMP_CACHE);
	bmpDownload.LoadBitmap(IDB_BMP_DOWNLOAD);
	bmpExp.LoadBitmap(IDB_BMP_EXP);
	bmpIE.LoadBitmap(IDB_BMP_IE);
	bmpRead.LoadBitmap(IDB_BMP_READ);
	bmpSys.LoadBitmap(IDB_BMP_SYS);
	bmpUpdate.LoadBitmap(IDB_BMP_UPDATE);
	bmpUTEMP.LoadBitmap(IDB_BMP_UTEMP);
	bmpPerson.LoadBitmap(IDB_BMP_PERSON);
	bmpBrower.LoadBitmap(IDB_IEPARENT);
	bmpChrome.LoadBitmap(IDB_CHROME);
	bmpVedio.LoadBitmap(IDB_VIDEO);
	bmpKU6.LoadBitmap(IDB_KU6);
	bmpPPS.LoadBitmap(IDB_PPS);
	bmpPPTV.LoadBitmap(IDB_PPTV);
	bmpQQLIVE.LoadBitmap(IDB_QQLIVE);
	bmpQVOD.LoadBitmap(IDB_QVOD);
	bmpTUDOU.LoadBitmap(IDB_TUDOU);
	bmpXUNLEIKANKAN.LoadBitmap(IDB_XUNLEIKANKAN);
	bmpYOUKU.LoadBitmap(IDB_YOUKU);
	bmpFengxing.LoadBitmap(IDB_FENGXING);
	bmpLNK.LoadBitmap(IDB_BITMAP_LNK);
	bmpDump.LoadBitmap(IDB_BITMAP_DUMP);
	bmpFirefox.LoadBitmap(IDB_BMP_FIREFOX);
	bmpOpera.LoadBitmap(IDB_BMP_OPERA);
	bmpSogo.LoadBitmap(IDB_BMP_SOGO);
	bmpMathon.LoadBitmap(IDB_BMP_MATHON);
	bmpLogfile.LoadBitmap(IDB_BMP_LOGFILE);
	bmpErrrpot.LoadBitmap(IDB_BMP_ERRPORT);
	bmpSuolvtu.LoadBitmap(IDB_BMP_SUOLVETU);
	bmpHuishouzhan.LoadBitmap(IDB_BMP_HUISHAOUZHAN);

	m_imlTreeIcons.Create(16, 16, ILC_COLOR32 | ILC_MASK, 31, 1);
	m_imlTreeIcons.Add((HBITMAP)bmpCache, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpDownload, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpExp, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpIE, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpRead, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpSys, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpUpdate, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpUTEMP, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpPerson, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpBrower, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpChrome, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpVedio, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpKU6, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpPPS, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpPPTV, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpQQLIVE, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpQVOD, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpTUDOU, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpXUNLEIKANKAN, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpYOUKU, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpFengxing, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpLNK, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpDump, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpMathon, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpOpera, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpSogo, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpFirefox, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpLogfile, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpErrrpot, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpSuolvtu, RGB(255, 0, 255));
	m_imlTreeIcons.Add((HBITMAP)bmpHuishouzhan, RGB(255, 0, 255));

	m_tree.SubclassWindow( GetDlgItem(IDC_TREE));
	m_tree.SetImageList( m_imlTreeIcons, TVSIL_NORMAL );
	m_tree.SetItemHeight( 20 );

	m_pass.m_ulTotalJunkFileSize = 0;
	m_pass.filepath = L"";

	int nRetCode = m_config.ReadConfig();

	if (nRetCode != RC_SUCCESS ||!m_config.InitFiltDir()|| !m_config.ConvToLocatePath())
	{
		m_btn_clearfile.EnableWindow(FALSE);
		m_check_all.EnableWindow(FALSE);
		m_btn_rescan.EnableWindow(FALSE);
		m_recommand.EnableWindow(FALSE);

	}
	else
	{
		if(::PathFileExists(m_strCacheFile.GetBuffer()))
		{
			IniEditor IniEdit;
			IniEdit.SetFile(m_strCacheFile.GetBuffer());

			int checkNum = 0;

			for (int j = 0;j < m_config.m_ConfigData.size();j++)
			{
				int nSelect = 0;
				BOOL bOk = IniEdit.ReadDWORD(L"kclear_checke_laji",m_config.m_ConfigData[j].strFriendName.GetBuffer(),nSelect);

				if(!bOk)
					break;

				if (nSelect == 1)
				{
					m_config.m_ConfigData[j].bDefaultSelect = 1;
					checkNum ++;
				}
				else 
				{
					m_config.m_ConfigData[j].bDefaultSelect = 0;
				}
			}

			if(checkNum == m_config.GetConfigData().size() - 3)
			{
				m_check_all.SetCheck(TRUE);
			}
			else 
				m_check_all.SetCheck(FALSE);


		}
		HTREEITEM	hTreeItem = NULL;
		HTREEITEM	hTreeCItem = NULL;
		for (int i = 0; i < m_config.m_ConfigData.size(); i++)
		{

			DIRECTORYWORK& work = m_config.m_ConfigData[i];

			m_IdDirMap[work.id] = work;

			if(!work.bIsLeaf)
			{
				int nCheck = KUIMulStatusTree::EM_TVIS_CHECK;
				if(!work.bDefaultSelect )
					nCheck = KUIMulStatusTree::EM_TVIS_UNCHECK;

				hTreeItem = m_tree.InsertItem( work.strFriendName
					, work.iconindex
					, work.iconindex,
					NULL, 
					NULL,  
					nCheck);
				if(work.bDefaultSelect)
					m_mapclearflag[work.id] = 1;
				else m_mapclearflag[work.id] = 0;

				m_IdTreeMap[work.id] = hTreeItem;
				m_TreeRootMap[hTreeItem] = work.id;
				for(int j = 0;j < m_config.m_ConfigData.size();j++)
				{
					DIRECTORYWORK& workChild = m_config.m_ConfigData[j];
					if(workChild.parent == work.id && workChild.bIsLeaf)
					{
						int nCheck = KUIMulStatusTree::EM_TVIS_CHECK;
						if(!workChild.bDefaultSelect )
							nCheck = KUIMulStatusTree::EM_TVIS_UNCHECK;
						hTreeCItem = m_tree.InsertItem( workChild.strFriendName
							, workChild.iconindex
							, workChild.iconindex,
							hTreeItem, 
							NULL,  
							nCheck);
						if(workChild.bDefaultSelect)
							m_mapclearflag[workChild.id] = 1;
						else m_mapclearflag[workChild.id] = 0;

						m_IdTreeMap[workChild.id] = hTreeCItem;
						m_TreeRootMap[hTreeCItem] = workChild.id;


					}
				}

			}else
			{
				if(m_IdTreeMap.find(work.id)==m_IdTreeMap.end())
				{
					int nCheck = KUIMulStatusTree::EM_TVIS_CHECK;
					if(!work.bDefaultSelect )
						nCheck = KUIMulStatusTree::EM_TVIS_UNCHECK;

					hTreeItem = m_tree.InsertItem( work.strFriendName
						, work.iconindex
						, work.iconindex,
						NULL, 
						NULL,  
						nCheck);
					m_IdTreeMap[work.id] = hTreeItem;
					m_TreeRootMap[hTreeItem] = work.id;

				}
			}



		}


		m_FileListData.m_itemArray.resize(m_config.m_ConfigData.size() );
		if (m_FileListData.m_itemArray.size() == m_config.m_ConfigData.size())
		{
			_StartFind(TRUE);
		}
		else
		{
			m_btn_rescan.EnableWindow(FALSE);

		}
	}

	m_btn_clearfile.EnableWindow(FALSE);
	SizeChildWindow();
	SetTimer(DEF_TIMER_UPDATE_SCANSTATUS, 50, NULL);


	return S_OK;  
}


void CMainDialog::OnPaint(HDC hDC)
{
	CPaintDC	PaintDC(m_hWnd);
	CRect		WndRect;

	GetClientRect(WndRect);
	PaintDC.FillSolidRect(WndRect, RGB(255,255,255));
}


void CMainDialog::SizeChildWindow()
{


	CRect clientRect;
	CRect rcBtnReScan;
	CRect rcBtnClear;
	CRect rcBtnSel;
	GetClientRect(&clientRect);

	CRect rcDirList  = clientRect;
	CRect rcFileList = clientRect;
	const int nBottomSpace   = 35;

	rcFileList.bottom -= nBottomSpace;
	rcDirList.bottom  -= nBottomSpace;
	// if(!Is_Scan)
	rcDirList.right = clientRect.right-10;
	//else
	//    rcDirList.right = ( clientRect.right - clientRect.left ) / 4 + 170;
	rcDirList.left = rcDirList.left+10;
	rcFileList.left = rcDirList.right-1;
	rcFileList.right = rcFileList.right-10;
	m_tree.MoveWindow(rcDirList);

	rcBtnClear.bottom = clientRect.bottom - 4;
	rcBtnClear.top    = clientRect.bottom - 30;
	rcBtnClear.right  = clientRect.right  - 10;
	rcBtnClear.left   = rcBtnClear.right - 72;
	m_btn_clearfile.MoveWindow(&rcBtnClear);
	m_btn_clearfile.ShowWindow(TRUE);
	if(is_init)
		m_btn_clearfile.EnableWindow(FALSE);
	//else m_btn_clearfile.EnableWindow(FALSE);

	rcBtnReScan.top = clientRect.bottom - 30;
	rcBtnReScan.bottom  = clientRect.bottom - 4;
	rcBtnReScan.right = rcBtnClear.left - 10;
	rcBtnReScan.left  = rcBtnReScan.right - 72;
	m_btn_rescan.MoveWindow(&rcBtnReScan);
	m_btn_rescan.ShowWindow(TRUE);


	rcBtnSel.bottom = clientRect.bottom - 8;
	rcBtnSel.top = rcDirList.bottom + 12;;
	rcBtnSel.left = clientRect.left + 10;
	rcBtnSel.right = clientRect.left + 23;
	m_check_all.MoveWindow(&rcBtnSel);
	m_check_all.ShowWindow(TRUE);

	rcBtnSel.left = rcBtnSel.right + 4;
	rcBtnSel.right = rcBtnSel.left + 30;
	rcBtnSel.top = rcBtnSel.top + 1;
	m_check_static.MoveWindow(&rcBtnSel);
	m_check_static.ShowWindow(TRUE);
	rcBtnSel.left = rcBtnSel.right + 8;
	rcBtnSel.right = rcBtnSel.left + 80;
	rcBtnSel.bottom = clientRect.bottom - 9;
	m_recommand.MoveWindow(&rcBtnSel);
	m_recommand.ShowWindow(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CMainDialog message handlers

void CMainDialog::TraverseFile(LPFINDFILEDATA pFileData)
{
	if (!m_nDestroy)
		SendMessage(WM_TRAVERSEFILE, 0, (LPARAM)pFileData);
}

void CMainDialog::TraverseProcess(unsigned long uCurPos)
{
	if (!m_nDestroy)
		SendMessage(WM_TRAVERSEPROCESS, uCurPos);
}

void CMainDialog::TraverseProcessEnd(unsigned long uCurPos,CString vedioPath)
{
	if (!m_nDestroy)
		SendMessage(WM_TRAVERSEPROCESSEND, uCurPos,(LPARAM)vedioPath.GetBuffer());
}

void CMainDialog::TraverseFinished()
{
	if (!m_nDestroy)
		SendMessage(WM_TRAVERSEFINISHED);
}

LRESULT CMainDialog::OnTraverseFile(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_nDestroy)
		return TRUE;


	LPFINDFILEDATA pFileData = (LPFINDFILEDATA)lParam;

	m_pass.m_ulTotalJunkFileSize += m_FileListData.AddTrashFile(m_nCurrentFindIndex, pFileData);//pFileData->uFileSize;
	m_pass.filepath = pFileData->pszPathName;
	if (m_pass.m_ulTotalJunkFileSize >= 0xFFFFFFFF)
	{
		m_nScanStatus = 3;
	}

	m_nScanStatus = 1;

	return TRUE;
}

LRESULT CMainDialog::OnTraverseProcess(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_nDestroy)
		return TRUE;

	unsigned long uCurPos = wParam;

	if (m_nProIndex==uCurPos)
		return TRUE;

	m_nProIndex = m_nCurrentFindIndex;
	m_nCurrentFindIndex = uCurPos;

	if (uCurPos >=0 && !is_init)
	{
		m_pass.filepath = m_IdDirMap[uCurPos].strDirectory.at(0);
		std::wstring ScanText = m_IdDirMap[uCurPos].strFriendName;
		ScanText += L"(正在扫描...)";
		m_tree.SetItemText(m_IdTreeMap[uCurPos],ScanText.c_str());
		m_tree.SelectItem(m_IdTreeMap[uCurPos]);
	}

	return TRUE;
}


LRESULT CMainDialog::OnTraverseProcessEnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_nDestroy)
		return TRUE;

	unsigned long uCurPos     = wParam;

	CString strPath = (LPCTSTR)lParam;

	if (m_nProIndex==uCurPos)
	{

	}
	else
	{
		m_nProIndex = m_nCurrentFindIndex;
		m_nCurrentFindIndex = uCurPos;


		if (uCurPos >=0 && !is_init)
		{
			if(uCurPos>=VIDEO_TUDOU&&uCurPos<=VIDEO_QQLIVE)
			{
				m_IdDirMap[uCurPos].strDirectory.at(0) = strPath;
				for(int j = 0;j < m_config.m_ConfigData.size();j++)
				{
					if( m_config.m_ConfigData[j].id == uCurPos)
					{
						m_config.m_ConfigData[j].strDirectory.at(0) = strPath;
						break;
					}
				}

			}
		}

	}
	
		std::wstring ScanText = m_IdDirMap[uCurPos].strFriendName;
		CString strDir = m_IdDirMap[uCurPos].strDirectory.at(0);

		if(GetProcessIdFromName(L"chrome.exe")>=4&&uCurPos==BROWER_CHROME)
		{
			m_bFindFileRunningSkip = TRUE;
			ScanText += L"（正在运行，跳过）";
			goto _Jump0_;
		}
		if(GetProcessIdFromName(L"maxthon.exe")>=4&&uCurPos==BROWER_MATHRON)
		{
			m_bFindFileRunningSkip = TRUE;
			ScanText += L"（正在运行，跳过）";
			goto _Jump0_;
		}
		if(GetProcessIdFromName(L"opera.exe")>=4&&uCurPos==BROWER_OPERA)
		{
			m_bFindFileRunningSkip = TRUE;
			ScanText += L"（正在运行，跳过）";
			goto _Jump0_;
		}
		if(GetProcessIdFromName(L"firefox.exe")>=4&&uCurPos==BROWER_FIREFOX)
		{
			m_bFindFileRunningSkip = TRUE;
			ScanText += L"（正在运行，跳过）";
			goto _Jump0_;
		}
		if(GetProcessIdFromName(L"sogouexplorer.exe")>=4&&uCurPos==BROWER_SOGO)
		{
			m_bFindFileRunningSkip = TRUE;
			ScanText += L"（正在运行，跳过）";
			goto _Jump0_;
		}
		if((GetProcessIdFromName(L"iexplore.exe")>=4 ||
			GetProcessIdFromName(L"360se.exe")>=4 ||
			GetProcessIdFromName(L"TTraveler.exe")>=4 ||
			GetProcessIdFromName(L"TheWorld.exe")>=4 ||
			GetProcessIdFromName(L"maxthon.exe")>=4 ||
			GetProcessIdFromName(L"sogouexplorer.exe")>=4)&&uCurPos==BROWER_IE)
		{
			m_bFindFileRunningSkip = TRUE;
			ScanText += L"（正在运行，跳过）";
			goto _Jump0_;
		}


		if(m_IdDirMap[uCurPos].bDefaultSelect&&::PathIsDirectory(strDir)&&m_FileListData.m_itemArray.at(uCurPos).itemArray.size()>0)
		{
			if (uCurPos != WIN_DUMP && uCurPos != WIN_LOG && uCurPos != WIN_ERRPORT && uCurPos != BROWER_OPERA)
			{
				ScanText += L"【";
				if(uCurPos==BROWER_IE)
				{
					strDir = strDir.Left(strDir.ReverseFind(L'\\'));
				}

				ScanText += strDir;
				if(m_IdDirMap[uCurPos].bRecursive&&ScanText.at(ScanText.length()-1)!=L'\\')
					ScanText += L"\\";
				ScanText += L"】";
			}
		}

		if(m_IdDirMap[uCurPos].bDefaultSelect)
		{
			ScanText += L"(";
			if (m_FileListData.m_itemArray.at(uCurPos).itemArray.size()>0)
				ScanText += GetFileSizeText(m_FileListData.m_itemArray.at(uCurPos).ulAllFileSize);
			else 
				ScanText += L"无";  
			ScanText += L")";
		}
_Jump0_:
		m_tree.SetItemText(m_IdTreeMap[uCurPos],ScanText.c_str());
		m_tree.SelectItem(m_IdTreeMap[uCurPos]);
		 
		
		 		int nCurId = uCurPos;
		 		int nInsertNum = 0;
		 		for (int nIndex = 0;
		 			nIndex <m_FileListData.m_itemArray.at(nCurId).itemArray.size();
		 			nIndex++)
		 		{
		 			int nCheck = KUIMulStatusTree::EM_TVIS_UNCHECK;
		 			if(m_IdDirMap[nCurId].bDefaultSelect)
		 				nCheck = KUIMulStatusTree::EM_TVIS_CHECK;
		 
		 			CString FileExt = GetExtName(m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).strFileName);
		 			CString strFileName = m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).strFileName;
		 			CString subDirFile = L"";
		 			CString strDir = m_IdDirMap[nCurId].strDirectory.at(0);
		 			if(nCurId == BROWER_IE)
		 			{
		 				strDir = strDir.Left(strDir.ReverseFind(L'\\'));
		 			}
//					else if(nCurId == WIN_DESKTOPLNK)
// 					{
// 						strDir = strDir.Left(strDir.ReverseFind(L'\\'));
// 						strDir = strDir.Left(strDir.ReverseFind(L'\\'));
// 					}
					
		 			if (nCurId >= VIDEO_TUDOU&&nCurId <= VIDEO_QQLIVE)
		 				subDirFile = strFileName.Mid(strDir.GetLength());
		 			else if(nCurId == WIN_DUMP || nCurId == WIN_ERRPORT || nCurId == WIN_LOG || nCurId == BROWER_OPERA)
						subDirFile = strFileName;
					else if(nCurId == WIN_HUISHOUZHAN)
						subDirFile = strFileName;
					else
						subDirFile = strFileName.Mid(strDir.GetLength()+1);
		 			for(int i = 0;i<m_SSuffix.size();i++)
		 			{
		 				if (FileExt.CompareNoCase(m_SSuffix[i]) == 0)
		 				{
		 					nCheck = KUIMulStatusTree::EM_TVIS_UNCHECK;
		 					break;
		 				}
		 			}
		 
		 			if (nCheck == KUIMulStatusTree::EM_TVIS_UNCHECK && (nCurId >=  VIDEO_TUDOU&& nCurId <= VIDEO_QQLIVE))
		 			{
						if(nInsertNum<9)
						{
							HTREEITEM hTmpItem = m_tree.InsertItem( 
								subDirFile
								, m_IdDirMap[nCurId].iconindex
								, m_IdDirMap[nCurId].iconindex,
								m_IdTreeMap[nCurId], 
								NULL,  
								KUIMulStatusTree::EM_TVIS_NONE);
							m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).hItem = hTmpItem;
						}
						m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).bIsClear = true;
						m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).ParentDirId = nCurId;
						nInsertNum++;
		 				if(nInsertNum == 9)
		 				{
		 					m_tree.InsertItem( L"..."//L"双击查看详情"
		 						, m_IdDirMap[nCurId].iconindex
		 						, m_IdDirMap[nCurId].iconindex,
		 						m_IdTreeMap[nCurId], 
		 						NULL,  
		 						KUIMulStatusTree::EM_TVIS_NONE);
		 
		 				}
		 				
		 
		 			}
		 			else if (nCheck == KUIMulStatusTree::EM_TVIS_CHECK && (nCurId >= VIDEO_TUDOU && nCurId <= VIDEO_QQLIVE))
		 			{
						if(nInsertNum<9)
						{
							HTREEITEM hTmpItem = m_tree.InsertItem( 
								subDirFile
								, m_IdDirMap[nCurId].iconindex
								, m_IdDirMap[nCurId].iconindex,
								m_IdTreeMap[nCurId], 
								NULL,  
								KUIMulStatusTree::EM_TVIS_NONE);
							m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).hItem = hTmpItem;
						}
						m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).bIsClear = true;
						m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).ParentDirId = nCurId;
						nInsertNum++;
						if(nInsertNum == 9)
						{
							m_tree.InsertItem( L"..."//L"双击查看详情"
								, m_IdDirMap[nCurId].iconindex
								, m_IdDirMap[nCurId].iconindex,
								m_IdTreeMap[nCurId], 
								NULL,  
								KUIMulStatusTree::EM_TVIS_NONE);

						}
		 				
		 			}
		 			else if (nCurId < VIDEO_TUDOU || nCurId > VIDEO_QQLIVE)
		 			{
		 				if(nInsertNum<9)
		 				{
		 					HTREEITEM hTmpItem = m_tree.InsertItem( 
		 						subDirFile
		 						, m_IdDirMap[nCurId].iconindex
		 						, m_IdDirMap[nCurId].iconindex,
		 						m_IdTreeMap[nCurId], 
		 						NULL,  
		 						KUIMulStatusTree::EM_TVIS_NONE);
		 					m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).hItem = hTmpItem;
		 				}
		 				m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).bIsClear = true;
		 
		 				m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).ParentDirId = nCurId;
		 				nInsertNum ++;
		 				if(nInsertNum == 9)
		 				{
		 					m_tree.InsertItem( L"..."//L"双击查看详情"
		 						, m_IdDirMap[nCurId].iconindex
		 						, m_IdDirMap[nCurId].iconindex,
		 						m_IdTreeMap[nCurId], 
		 						NULL,  
		 						KUIMulStatusTree::EM_TVIS_NONE);
		 
		 
		 				}
		 			}
		 
		 		}
		 



	return TRUE;
}

LRESULT CMainDialog::OnTraverseFinished(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_nDestroy)
		return TRUE;

	m_nCurrentFindIndex = -1;
	m_nScanStatus = 2;

	if(is_init)
	{
		Is_Scan = FALSE;
		m_nScanStatus = DLG_INIT;
	}
	else
	{
		Is_Scan = TRUE;
		if(m_pass.m_ulTotalJunkFileSize == 0)
			m_nScanStatus = UN_SIZE;
	}
	m_pass.filepath = L"";

	::SendMessage(GetParent().m_hWnd, WM_TRASH_SCAN_STATUS_CHANGED, (WPARAM)m_nScanStatus, (LPARAM)&(m_pass));
	//////////////////////////////////////////////////////////////////////////

	int nStatus = em_ds_uncheck;



	ULONGLONG MaxTmp = 0;
	int dwMaxIndex = 0;
	int dwIndex = 0;
	while(dwIndex < m_FileListData.m_itemArray.size())
	{
		if(m_FileListData.m_itemArray.at(dwIndex).ulAllFileSize > MaxTmp)
		{
			MaxTmp = m_FileListData.m_itemArray.at(dwIndex).ulAllFileSize;
			dwMaxIndex = dwIndex;
		}
		dwIndex++;
	}


	if(!is_init)
	{
		if(m_FileListData.m_itemArray.at(dwMaxIndex).ulAllFileSize == 0)
		{
			Is_Scan = FALSE;
			dwMaxIndex = 1;
		}
	}
	SizeChildWindow();
	m_tree.SelectItem(m_IdTreeMap[dwMaxIndex]);
	if(is_init)
	{
		m_tree.Expand(m_IdTreeMap[0]);
		m_tree.Expand(m_IdTreeMap[2],TVE_COLLAPSE);
		m_tree.Expand(m_IdTreeMap[3],TVE_COLLAPSE);
	}
	::SendMessage(m_hWnd,WM_KAN_NOTIFY_TREE_SELECT_CHANGE,(WPARAM)m_IdTreeMap[dwMaxIndex],0);

	m_check_all.EnableWindow(TRUE);
	m_btn_rescan.EnableWindow(TRUE);
	m_recommand.EnableWindow(TRUE);
	CString strCurText = _T("");
	strCurText = m_btn_rescan.GetTitle();
	if(strCurText == _T("停止扫描"))
		m_btn_rescan.SetTitle(32);

	m_btn_clearfile.ShowWindow(TRUE);
	if(is_init)
		m_btn_clearfile.EnableWindow(FALSE);
	else m_btn_clearfile.EnableWindow(TRUE);

	if (m_bFindFileRunningSkip)  // 有查找文件跳过的情况
	{
		::SendMessage(GetParent().m_hWnd, WM_TRASH_TIPS_SHOW_STATUS, (WPARAM)TRUE, (LPARAM)TEXT(""));
	}
	else
	{
		::SendMessage(GetParent().m_hWnd, WM_TRASH_TIPS_CLOSE_STATUS, (WPARAM)NULL, (LPARAM)NULL);
	}

	return FALSE;
}

LRESULT CMainDialog::OnItemchangedDirectorylist(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
	SetMsgHandled(TRUE);

	HTREEITEM hTreeItem = (HTREEITEM)wParam;


	if (hTreeItem != NULL)
	{
		WCHAR dirDesc[MAX_PATH]; 
		m_tree.GetItemText(hTreeItem,dirDesc,MAX_PATH);
		CString strTmp(dirDesc);
		if(strTmp == _T("双击查看详情"))
		{
			ShellExecute(NULL, L"open", L"Explorer.exe", L"/n,::{645FF040-5081-101B-9F08-00AA002F954E}", NULL, SW_SHOWNORMAL);
		}
// 			ShellExecute( m_hWnd, 
// 			_T("open"), 
// 			m_IdDirMap[m_TreeRootMap[m_tree.GetParentItem(hTreeItem)]].strDirectory.at(0).GetBuffer(), 
// 			NULL, NULL, SW_SHOWNORMAL );
// 		else if(strTmp == _T("双击查看详情"))
 		{
			/*CString strDesc = L"以下是在";
			strDesc += m_IdDirMap[m_TreeRootMap[m_tree.GetParentItem(hTreeItem)]].strFriendName;
			strDesc += L"中扫描到的垃圾文件，对此项执行清理后，下列文件将会被删除。";
 			CKscViewLog viewLog(L"查看详情", 
 				strDesc,
 				m_FileListData.m_itemArray[m_TreeRootMap[m_tree.GetParentItem(hTreeItem)]].strfileList);
 			viewLog.DoModal();*/
		}
	}
	return 0;
}



LRESULT CMainDialog::OnCheckChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetMsgHandled(TRUE);
	BOOL bEnable = FALSE;
	int  checkNum = 0;
	HTREEITEM hTreeItem = (HTREEITEM)wParam;
	int	nCheck = (int)lParam;

	if(m_TreeRootMap.find(hTreeItem) != m_TreeRootMap.end())
	{
		DIRECTORYWORK& work = m_IdDirMap[m_TreeRootMap[hTreeItem]];
		if(!work.bIsLeaf)
		{
			for(int j = 0;j < m_config.m_ConfigData.size();j++)
			{
				if( m_config.m_ConfigData[j].parent == work.id &&  m_config.m_ConfigData[j].bIsLeaf)
				{
					if(nCheck == KUIMulStatusTree::EM_TVIS_CHECK)
					{
						m_config.m_ConfigData[j].bDefaultSelect = TRUE;
						m_IdDirMap[m_config.m_ConfigData[j].id].bDefaultSelect = TRUE;
						for (int i = 0; i < m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.size(); i++)
						{
							m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.at(i).bIsClear = true;

						}

					}
					else if(nCheck == KUIMulStatusTree::EM_TVIS_UNCHECK) 
					{
						m_config.m_ConfigData[j].bDefaultSelect = FALSE;
						m_IdDirMap[m_config.m_ConfigData[j].id].bDefaultSelect = FALSE;
						for (int i = 0; i < m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.size(); i++)
						{

							m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.at(i).bIsClear = false;


						}
					}
				}

			}

		}
		else
		{
			for(int j = 0;j < m_config.m_ConfigData.size();j++)
			{
				if( m_config.m_ConfigData[j].id == work.id &&  m_config.m_ConfigData[j].bIsLeaf)
				{
					if(nCheck == KUIMulStatusTree::EM_TVIS_CHECK)
					{
						m_config.m_ConfigData[j].bDefaultSelect = TRUE;
						m_IdDirMap[m_config.m_ConfigData[j].id].bDefaultSelect = TRUE;
						for (int i = 0; i < m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.size(); i++)
						{

							m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.at(i).bIsClear = true;


						}


					}
					else if(nCheck == KUIMulStatusTree::EM_TVIS_UNCHECK) 
					{
						m_config.m_ConfigData[j].bDefaultSelect = FALSE;
						m_IdDirMap[m_config.m_ConfigData[j].id].bDefaultSelect = FALSE;
						for (int i = 0; i < m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.size(); i++)
						{

							m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.at(i).bIsClear = false;


						}
					}
				}

			}

		}

	}


	for(int j = 0;j < m_config.m_ConfigData.size();j++)
	{


		for (int i = 0; i < m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.size(); i++)
		{
			if(m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.at(i).hItem == hTreeItem || 
				hTreeItem == m_IdTreeMap[m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.at(i).ParentDirId])
			{
				if(nCheck == KUIMulStatusTree::EM_TVIS_UNCHECK)
				{
					m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.at(i).bIsClear = false;

				}

				else if(nCheck == KUIMulStatusTree::EM_TVIS_CHECK)
				{
					m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.at(i).bIsClear = true;

				}

				int nParCheck = KUIMulStatusTree::EM_TVIS_UNCHECK;
				if(m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.at(i).hItem == hTreeItem)
					m_tree.GetCheck(m_tree.GetParentItem(hTreeItem),nParCheck);
				else
					nParCheck = nCheck;

				if (nParCheck != KUIMulStatusTree::EM_TVIS_UNCHECK
					&&m_config.m_ConfigData[j].id >= VIDEO_TUDOU
					&&m_config.m_ConfigData[j].id <= VIDEO_QQLIVE)
				{
					m_IdDirMap[m_config.m_ConfigData[j].id].bDefaultSelect = TRUE;
					m_config.m_ConfigData[j].bDefaultSelect = TRUE;
				}
				else if (nParCheck == KUIMulStatusTree::EM_TVIS_UNCHECK
					&&m_config.m_ConfigData[j].id >= VIDEO_TUDOU
					&&m_config.m_ConfigData[j].id <= VIDEO_QQLIVE)
				{
					m_IdDirMap[m_config.m_ConfigData[j].id].bDefaultSelect = FALSE;
					m_config.m_ConfigData[j].bDefaultSelect = FALSE;
				}
			}

		}



	}

	for(int j = 0;j < m_config.m_ConfigData.size();j++)
	{
		if( m_config.m_ConfigData[j].bIsLeaf)
		{
			if(m_config.m_ConfigData[j].bDefaultSelect)
			{

				bEnable = TRUE;
				checkNum ++ ;

			}

		}

	}

	if(checkNum == m_config.GetConfigData().size() - 3)
	{
		m_check_all.SetCheck(TRUE);
	}
	else 
		m_check_all.SetCheck(FALSE);


	return 0;
}

void CMainDialog::EnableClearBtn()
{

}

void CMainDialog::OnSize(UINT wParam, const _WTYPES_NS::CSize& size) 
{
	SizeChildWindow();
}

void CMainDialog::_StartFind(BOOL Is_Init)
{
	m_nCurrentFindIndex = -1;
	m_nProIndex = -1;

	// 清空各目录的文件列表
	for (int i = 0; i < m_FileListData.m_itemArray.size(); i++)
	{
		m_FileListData.m_itemArray.at(i).ulAllFileSize = 0;
		m_FileListData.m_itemArray.at(i).itemArray.clear();
		m_FileListData.m_itemArray.at(i).strfileList = L"";
	}

	if(!Is_Init)
		RefreshTree(L"");
	m_bFindFileRunningSkip = FALSE;
	// 查找
	if ( !find.StartFindFileTraverse(m_config, this,Is_Init) )
	{

		m_check_all.EnableWindow(TRUE);


	}
	else
	{
		m_check_all.EnableWindow(FALSE);

	}

}

void CMainDialog::OnDestroy() 
{

	m_nDestroy = TRUE;
	m_FileListData.m_itemArray.clear();
	KillTimer(DEF_TIMER_UPDATE_SCANSTATUS);

	if ( find.IsFinding() )
		find.StopFileTraverse();



}

LRESULT CMainDialog::OnSelall(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
	BOOL nStatus = TRUE;


	if (m_check_all.GetCheck())
	{
		nStatus = TRUE;
	}
	else
	{
		nStatus = FALSE;
	}


	for(int j = 0;j < m_config.m_ConfigData.size();j++)
	{
		if( m_config.m_ConfigData[j].bIsLeaf)
		{
			if(nStatus)
			{
				m_config.m_ConfigData[j].bDefaultSelect = TRUE;
				m_IdDirMap[m_config.m_ConfigData[j].id].bDefaultSelect = TRUE;
				m_tree.SetCheck(m_IdTreeMap[m_config.m_ConfigData[j].id],KUIMulStatusTree::EM_TVIS_CHECK);

			}
			else
			{
				m_config.m_ConfigData[j].bDefaultSelect = FALSE;
				m_IdDirMap[m_config.m_ConfigData[j].id].bDefaultSelect = FALSE;
				m_tree.SetCheck(m_IdTreeMap[m_config.m_ConfigData[j].id],KUIMulStatusTree::EM_TVIS_UNCHECK);
			}
		}

	}

	for(int j = 0;j < m_config.m_ConfigData.size();j++)
	{


		for (int i = 0; i < m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.size(); i++)
		{

			if(nStatus)
				m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.at(i).bIsClear = true;
			else 
				m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.at(i).bIsClear = false;

		}

	}


	return S_OK;
}

LRESULT CMainDialog::OnRescan(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
	SetMsgHandled(TRUE);
	is_init = false;
	CString strCurText = m_btn_rescan.GetTitle();
	if (strCurText.Compare(_T("开始扫描")) == 0 || strCurText.Compare(_T("重新扫描")) == 0)
	{
		m_btn_clearfile.EnableWindow(FALSE);


		BOOL bEnable = FALSE;

		for (int i = 0; i < m_config.m_ConfigData.size(); i++)
		{
			if (m_config.m_ConfigData[i].bIsLeaf)
			{
				if(m_config.m_ConfigData[i].bDefaultSelect)
				{
					bEnable = TRUE;
					m_mapclearflag[m_config.m_ConfigData[i].id] = 1;
				}
				else m_mapclearflag[m_config.m_ConfigData[i].id] = 0;
			}


		}

		if(!bEnable)
		{

			::PostMessage(*this, WM_OWN_MSGBOX, (WPARAM)TEXT("\n您尚未选择需要扫描的项目，请先选择，然后进行扫描！"), 
				(LPARAM)(MB_OK | MB_ICONINFORMATION));
		}
		else
		{
			m_config.ConvToLocatePath();
			m_btn_rescan.SetTitle(35);
			Is_Scan = FALSE;
			SizeChildWindow();
			m_recommand.EnableWindow(FALSE);
			m_pass.m_ulTotalJunkFileSize = 0;
			m_pass.filepath = L"";
			m_nScanStatus = 1;
			::SendMessage(GetParent().m_hWnd, WM_TRASH_SCAN_STATUS_CHANGED, (WPARAM)m_nScanStatus, (LPARAM)&m_pass);
			m_check_all.EnableWindow(FALSE);
			m_btn_clearfile.EnableWindow(FALSE);
			_StartFind(FALSE);
		}

	}
	else  
	{
		Is_Scan = FALSE;
		SizeChildWindow();
		m_btn_rescan.EnableWindow(FALSE);
		find.StopFileTraverse();
	}

	return 0;
}


DWORD WINAPI CMainDialog::ClearFileThread(LPVOID lpVoid)
{


	CMainDialog* pthis = (CMainDialog*)lpVoid;
	while(1)
	{
		if(pthis->m_pdlgDel == NULL || pthis->m_pdlgDel->IsWindowExist())
			break;
	}
	if(pthis->m_pdlgDel)
		pthis->RunClearFile();

	::CloseHandle(pthis->m_hThread);
	InterlockedExchange((LONG*)&pthis->m_hThread, NULL);
	InterlockedExchange((LONG*)&pthis->m_dwThreadID, 0);

	if(pthis->m_hEvent)
	{
		CloseHandle(pthis->m_hEvent);
		pthis->m_hEvent = NULL;
	}

	pthis->m_nScanStatus = CLEAR_END;
	::SendMessage(pthis->GetParent().m_hWnd, WM_TRASH_SCAN_STATUS_CHANGED, (WPARAM)pthis->m_nScanStatus, (LPARAM)&pthis->m_pass);
	pthis->m_nScanStatus = CLEAR_HIDE;
	::SendMessage(pthis->GetParent().m_hWnd, WM_TRASH_SCAN_STATUS_CHANGED, (WPARAM)pthis->m_nScanStatus, (LPARAM)&pthis->m_pass);

	pthis->Is_Scan = FALSE;
	pthis->SizeChildWindow();
	pthis->m_btn_clearfile.EnableWindow(TRUE);
	pthis->m_btn_rescan.EnableWindow(TRUE);
	pthis->Invalidate();

	return 0;
}

void CMainDialog::RunClearFile()
{
	int nPos = 0;

	int pClearCount[3] = { 0 };

	ULONGLONG uClearFileSize = 0;

	CFileDelete deleteFile;

	int nPosEx = 1;
	int nRemainingCount = m_nDelTotalCount;
	ULONGLONG nRemainingSize = m_nDelTotalSize;
	CString strRemaining;

	for (int j = 0;j < m_config.m_ConfigData.size();j++)
	{
		if (m_config.m_ConfigData[j].bIsLeaf)
		{
			if (m_config.m_ConfigData[j].bDefaultSelect)
			{
				std::vector<TRASHFILEITEM> itemArray = 
					m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray;

				ULONGLONG ulCurSize = m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].ulAllFileSize;

				for (int i = 0; i < itemArray.size(); i++)
				{
					if (itemArray[i].bIsClear)
					{
						if (m_pdlgDel == NULL || m_pdlgDel->IsStop())
						{
							goto clean0;
						}
						int delRes = 0;
						if (m_config.m_ConfigData[j].id == WIN_HUISHOUZHAN)
						{
							SHEmptyRecycleBin(NULL, NULL, SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND);
							delRes = Delete_File_Success;
						}
						else
						{
							delRes = deleteFile.TrashFileDelete(itemArray[i].strFileName);
						}
						if (delRes==Delete_File_Failed)
						{
							CStubbornFiles::Instance().AddFile(itemArray[i].strFileName);
							CString strLog = L"[Clean][";
							strLog += m_config.m_ConfigData[j].strFriendName;
							strLog += L"][FAILED]:";
							strLog += itemArray[i].strFileName;
							KppUserLog::UserLogInstance().WriteSingleLog(strLog.GetBuffer());
						}
						else 
						{
							CString strLog = L"[Clean][";
							strLog += m_config.m_ConfigData[j].strFriendName;
							strLog += L"][OK]:";
							strLog += itemArray[i].strFileName;
							KppUserLog::UserLogInstance().WriteSingleLog(strLog.GetBuffer());
						}
						if (m_pdlgDel)
						{
							nRemainingSize = nRemainingSize - itemArray[i].ulFileSize;
							--nRemainingCount; 
							LPCTSTR strSize;
							CString strPosFile;
							strSize = GetFileSizeText(nRemainingSize);
							strRemaining.Format(_T("%d（%s）"), nRemainingCount, strSize);
							m_pdlgDel->SetFileDel(itemArray[i].strFileName);
							strSize = GetFileSizeText( ulCurSize);
							strPosFile.Format(_T("%s（%s）"), m_config.m_ConfigData[j].strFriendName, strSize);

							m_pdlgDel->SetFilePos(strPosFile);
							m_pdlgDel->SetFileRemaining(strRemaining);
							int nPosCur = (int)(((double)nPosEx++ / (double)m_nDelTotalCount) * 100);
							m_pdlgDel->SetProcessPos(nPosCur);
						}
						uClearFileSize += itemArray[i].ulFileSize;
						m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].ulAllFileSize -= itemArray[i].ulFileSize;
						m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray[i].ulFileSize = 0;
						m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray[i].bIsClear = true;

					}
				}

			}

		}

	}
clean0:
	m_nDelTotalCount = 0;
	m_nDelTotalSize = 0;
	RefreshTree(_T("(已清理完毕)"));   
	CStubbornFiles::Instance().Sync();

	REMOVE_EXAM_RESULT_ITEM RemoveItemInfo;
	RemoveItemInfo.uRemoveItem.RemoveItemInfo.Reset();
	RemoveItemInfo.uRemoveItem.RemoveItemInfo.dwItemID = BkSafeExamItem::EXAM_TRASH;
// 	SAFE_CALL_FUN( m_pNotifyWnd, PostMessage( MSG_APP_EXAM_SCORE_CHANGE, 
// 		BkSafeExamItemExOperate::RemoveItem, (LPARAM)&RemoveItemInfo ));

	::SendMessage(g_hMainHwnd, MSG_APP_EXAM_SCORE_CHANGE, BkSafeExamItemExOperate::RemoveItem, (LPARAM)&RemoveItemInfo );
	m_pass.m_ulTotalJunkFileSize= uClearFileSize;

}


LRESULT CMainDialog::OnClearfile(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled) 
{

	SetMsgHandled(TRUE);
	BOOL bEnable = FALSE;
	BOOL bEnable1 = FALSE;
	BOOL bEnable2 = FALSE;
	BOOL bIsVideo = FALSE;
	is_finish = false;
	is_ok = false;
	is_havespecsuffix = false;
	int ClearCount = 0;
	m_pdlgDel = new CTrashDoingDlg;

	m_pass.m_ulTotalJunkFileSize = 0;

	for(int j = 0;j < m_config.m_ConfigData.size();j++)
	{
		if( m_config.m_ConfigData[j].bIsLeaf)
		{
			if(m_config.m_ConfigData[j].bDefaultSelect)
			{
				bEnable = TRUE;
				if(m_mapclearflag[m_config.m_ConfigData[j].id] == 0)
					bEnable1 = TRUE;

				if (m_mapclearflag[m_config.m_ConfigData[j].id] == 1 && 
					m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.size()>0)
				{
					bEnable2 = TRUE;

					m_pass.m_ulTotalJunkFileSize += m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).ulAllFileSize;

					if (m_config.m_ConfigData[j].id >= VIDEO_TUDOU&&m_config.m_ConfigData[j].id <= VIDEO_QQLIVE )
						bIsVideo = TRUE;

					for (int k = 0;k<m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.size();k++)
					{
						if(!(m_config.m_ConfigData[j].id >= VIDEO_TUDOU&&m_config.m_ConfigData[j].id <= VIDEO_QQLIVE))
							break;
						if(is_havespecsuffix)
							break;
						if (m_config.m_ConfigData[j].id == WIN_HUISHOUZHAN)
						{
							break;
						}
						CString Fileext = GetExtName(m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.at(k).strFileName);
						for(int i = 0;i<m_SSuffix.size();i++)
						{
							if(Fileext.CompareNoCase(m_SSuffix[i]) == 0)
							{
								is_havespecsuffix = true;
								break;
							}
						}

					}

					ClearCount += m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.size();



				}

			}
		}

	}


	if(!bEnable)
	{
		::PostMessage(*this, WM_OWN_MSGBOX, (WPARAM)TEXT("\n您尚未选择需要清理的项目，请先选择，然后再清理！"), 
			(LPARAM)(MB_OK | MB_ICONINFORMATION));
		return 0;
	}

	if(bEnable1 && !bEnable2)
	{
		::PostMessage(*this, WM_OWN_MSGBOX, (WPARAM)TEXT("\n您选择的项目中存在未扫描的项目，建议您先扫描，再进行清理！"), 
			(LPARAM)(MB_OK | MB_ICONINFORMATION));
		return 0;
	}


	if(m_pass.m_ulTotalJunkFileSize == 0 || ClearCount == 0)
	{
		::PostMessage(*this, WM_OWN_MSGBOX, (WPARAM)TEXT("\n在你指定的清理选项中未发现垃圾文件！"), 
			(LPARAM)(MB_OK | MB_ICONINFORMATION));
		return 0;
	}

	ULONGLONG dwCurTotalSize = 0;
	DWORD dwFileCount = 0;

	for (int j = 0;j < m_config.m_ConfigData.size();j++)
	{
		if (m_config.m_ConfigData[j].bIsLeaf)
		{
			if (m_config.m_ConfigData[j].bDefaultSelect)
			{
				dwFileCount += m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.size();
				dwCurTotalSize += m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].ulAllFileSize;
			}
		}
	}
	m_nDelTotalCount = dwFileCount;
	m_nDelTotalSize = dwCurTotalSize;

	if(is_havespecsuffix)
	{
		m_tmpThread = CreateThread(NULL, 
			0, 
			WaitThread, 
			(LPVOID)this,
			NULL,
			&m_dwThreadID);

		return 0;
	}


	if(bIsVideo)
	{ 
		m_tmpThread = CreateThread(NULL, 
			0, 
			WaitThread, 
			(LPVOID)this,
			NULL,
			&m_dwThreadID);



		return 0;
	}

	m_btn_clearfile.EnableWindow(FALSE);
	m_btn_rescan.EnableWindow(FALSE);
	m_nScanStatus = CLEAR_STATUS;
	::SendMessage(GetParent().m_hWnd, WM_TRASH_SCAN_STATUS_CHANGED, (WPARAM)m_nScanStatus, (LPARAM)&m_pass);


	m_hThread = CreateThread(NULL, 
		0, 
		ClearFileThread, 
		(LPVOID)this,
		NULL,
		&m_dwThreadID);

	if (m_hThread != NULL)
	{
	}
	else
	{
		m_btn_clearfile.EnableWindow(TRUE);
		m_btn_rescan.EnableWindow(TRUE);
	}
	if (m_pdlgDel)
	{
		m_pdlgDel->Init(dwFileCount,dwCurTotalSize);
		m_pdlgDel->DoModal();
	}

	if (m_pdlgDel != NULL)
	{   
		delete m_pdlgDel;
		m_pdlgDel = NULL;
	}
	return 0;
}




LRESULT CMainDialog::OnOpendir(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled) 
{

	return S_OK;
}





void CMainDialog::OnTimer(UINT_PTR nIDEvent)
{
	if (DEF_TIMER_UPDATE_SCANSTATUS == nIDEvent)
	{
		::SendMessage(GetParent().m_hWnd, WM_TRASH_SCAN_STATUS_CHANGED, (WPARAM)m_nScanStatus, (LPARAM)&m_pass);
	}
}




LRESULT CMainDialog::OnRecommand(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled)
{

	for(int j = 0;j < m_config.m_ConfigData.size();j++)
	{
		if( m_config.m_ConfigData[j].bIsLeaf)
		{
			if(m_config.m_ConfigData[j].bRSelect)
			{
				m_config.m_ConfigData[j].bDefaultSelect = TRUE;
				m_IdDirMap[m_config.m_ConfigData[j].id].bDefaultSelect = TRUE;
				m_tree.SetCheck(m_IdTreeMap[m_config.m_ConfigData[j].id],KUIMulStatusTree::EM_TVIS_CHECK);

			}
			else
			{
				m_config.m_ConfigData[j].bDefaultSelect = FALSE;
				m_IdDirMap[m_config.m_ConfigData[j].id].bDefaultSelect = FALSE;
				m_tree.SetCheck(m_IdTreeMap[m_config.m_ConfigData[j].id],KUIMulStatusTree::EM_TVIS_UNCHECK);
			}
		}

	}

	for(int j = 0;j < m_config.m_ConfigData.size();j++)
	{

		if( m_config.m_ConfigData[j].bIsLeaf)
		{
			for (int i = 0; i < m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.size(); i++)
			{

				if(m_config.m_ConfigData[j].bDefaultSelect)
					m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.at(i).bIsClear = true;
				else 
					m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.at(i).bIsClear = false;
			}
		}
	}
	m_check_all.SetCheck(FALSE);
	m_btn_rescan.EnableWindow(TRUE);
	return S_OK;

}


LRESULT CMainDialog::OnStaticSellAll(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled) 
{

	if(find.IsFinding())
		return S_OK;
	if(m_check_all.GetCheck())
	{
		m_check_all.SetCheck(FALSE);
	}
	else m_check_all.SetCheck(TRUE);

	OnSelall(wNotifyCode,wID,hWndCtl,bHandled);

	return S_OK;

}

LRESULT CMainDialog::OnOwnMsgBox(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	if(CKuiMsgBox::Show((LPCTSTR)wParam, L"提示", (UINT)lParam, NULL, m_hWnd)== IDOK)
		is_ok = true;
	else is_ok = false;
	bHandled = TRUE;
	is_finish = true;
	return 0;
}

DWORD WINAPI CMainDialog::WaitThread(LPVOID lpVoid)
{
	CMainDialog* pthis = (CMainDialog*)lpVoid;
	if(pthis->is_havespecsuffix)
	{
		if(CKuiMsgBox::Show(TEXT("您选择了清理视频播放缓存，其中存在已经下载完成的完整视频文件。\n\n执行清理后您的这些缓存都会被删除，您是否确认清理所有选中项？"), L"提示",MB_YESNO | MB_ICONINFORMATION , NULL, pthis->m_hWnd)== IDYES)
			pthis->is_ok = true;
		else pthis->is_ok = false;
	}
	else
	{
		if(CKuiMsgBox::Show(TEXT("您选择了清理视频播放缓存，您正在使用中的视频缓存会被删除。\n\n您是否确认要清理这些视频缓存？"), L"提示",MB_YESNO | MB_ICONINFORMATION , NULL, pthis->m_hWnd)== IDYES)
			pthis->is_ok = true;
		else pthis->is_ok = false;

	}

	if(pthis->is_ok)
	{

		pthis->m_btn_clearfile.EnableWindow(FALSE);
		pthis->m_btn_rescan.EnableWindow(FALSE);
		pthis->m_nScanStatus = CLEAR_STATUS;
		::SendMessage(pthis->GetParent().m_hWnd, WM_TRASH_SCAN_STATUS_CHANGED, 
			(WPARAM)pthis->m_nScanStatus, (LPARAM)&(pthis->m_pass));


		pthis->m_hThread = CreateThread(NULL, 
			0, 
			ClearFileThread, 
			(LPVOID)pthis,
			NULL,
			&(pthis->m_dwThreadID));

		if (pthis->m_pdlgDel)
		{
			pthis->m_pdlgDel->Init(pthis->m_nDelTotalCount,pthis->m_nDelTotalSize);
			pthis->m_pdlgDel->DoModal();
		}

		if (pthis->m_pdlgDel != NULL)
		{   
			delete pthis->m_pdlgDel;
			pthis->m_pdlgDel = NULL;
		}

		if (pthis->m_hThread != NULL)
		{
		}
		else
		{
			pthis->m_btn_clearfile.EnableWindow(TRUE);
			pthis->m_btn_rescan.EnableWindow(TRUE);
		}

	}

	::CloseHandle(pthis->m_tmpThread);
	InterlockedExchange((LONG*)&pthis->m_tmpThread, NULL);
	InterlockedExchange((LONG*)&pthis->m_dwThreadID, 0);

	return 0;

}

void CMainDialog::RefreshTree(CString ItemDesc)
{
	m_tree.DeleteAllItems();
	m_IdDirMap.clear();
	m_IdTreeMap.clear();
	m_TreeRootMap.clear();

	HTREEITEM	hTreeItem = NULL;
	HTREEITEM	hTreeCItem = NULL;
	for (int i = 0; i < m_config.m_ConfigData.size(); i++)
	{

		DIRECTORYWORK& work = m_config.m_ConfigData[i];

		m_IdDirMap[work.id] = work;

		if(!work.bIsLeaf)
		{
			int nCheck = KUIMulStatusTree::EM_TVIS_CHECK;
			CString ItemText = work.strFriendName;

			if(!work.bDefaultSelect )
			{
				nCheck = KUIMulStatusTree::EM_TVIS_UNCHECK;
			}
			
			hTreeItem = m_tree.InsertItem( ItemText
				, work.iconindex
				, work.iconindex,
				NULL, 
				NULL,  
				nCheck);

			m_IdTreeMap[work.id] = hTreeItem;
			m_TreeRootMap[hTreeItem] = work.id;
			for(int j = 0;j < m_config.m_ConfigData.size();j++)
			{
				DIRECTORYWORK& workChild = m_config.m_ConfigData[j];
				if(workChild.parent == work.id && workChild.bIsLeaf)
				{
					int nCheck = KUIMulStatusTree::EM_TVIS_CHECK;
					CString ItemText = workChild.strFriendName;
					if (::PathIsDirectory(workChild.strDirectory.at(0))&&workChild.bDefaultSelect
						&&m_FileListData.m_itemArray[workChild.id].ulAllFileSize > 0)
					{
						CString strDir = workChild.strDirectory.at(0);
						if(workChild.id == BROWER_IE)
							strDir = strDir.Left(strDir.ReverseFind(L'\\'));
// 						else if(workChild.id  == WIN_DESKTOPLNK)
// 						{
// 							strDir = strDir.Left(strDir.ReverseFind(L'\\'));
// 							strDir = strDir.Left(strDir.ReverseFind(L'\\'));
// 						}
// 						
						ItemText += L"【";
						ItemText += strDir;
						if(workChild.bRecursive&&ItemText.GetAt(ItemText.GetLength()-1)!=L'\\')
							ItemText += L"\\";
						ItemText += L"】";
					}

					if(!workChild.bDefaultSelect )
					{
						nCheck = KUIMulStatusTree::EM_TVIS_UNCHECK;
						if(m_FileListData.m_itemArray[workChild.id].ulAllFileSize > 0)
						{
							ItemText += L"(";
							ItemText += GetFileSizeText(m_FileListData.m_itemArray[workChild.id].ulAllFileSize);
							ItemText += L")";
						}
					}
					else
					{
						if(GetProcessIdFromName(L"chrome.exe")>=4&&workChild.id==BROWER_CHROME)
						{
							ItemText += L"（正在运行，跳过）";
						}

						if(GetProcessIdFromName(L"maxthon.exe")>=4&&workChild.id==BROWER_MATHRON)
						{
							ItemText += L"（正在运行，跳过）";
						}
						if(GetProcessIdFromName(L"opera.exe")>=4&&workChild.id==BROWER_OPERA)
						{
							ItemText += L"（正在运行，跳过）";
						}
						if(GetProcessIdFromName(L"firefox.exe")>=4&&workChild.id==BROWER_FIREFOX)
						{
							ItemText += L"（正在运行，跳过）";
						}
						if(GetProcessIdFromName(L"sogouexplorer.exe")>=4&&workChild.id==BROWER_SOGO)
						{
							ItemText += L"（正在运行，跳过）";
						}
						if(GetProcessIdFromName(L"iexplore.exe")>=4&&workChild.id==BROWER_IE)
						{
							ItemText += L"（正在运行，跳过）";
						}
						else if (m_FileListData.m_itemArray[workChild.id].ulAllFileSize ==0)
							ItemText += ItemDesc;
					}



					hTreeCItem = m_tree.InsertItem( ItemText
						, workChild.iconindex
						, workChild.iconindex,
						hTreeItem, 
						NULL,  
						nCheck);


					m_IdTreeMap[workChild.id] = hTreeCItem;
					m_TreeRootMap[hTreeCItem] = workChild.id;


				}
			}

		}else
		{
			if(m_IdTreeMap.find(work.id)==m_IdTreeMap.end())
			{
				int nCheck = KUIMulStatusTree::EM_TVIS_CHECK;
				CString ItemText = work.strFriendName;
				if (::PathIsDirectory(work.strDirectory.at(0))&&work.bDefaultSelect
					&&m_FileListData.m_itemArray[work.id].ulAllFileSize > 0)
				{
					CString strDir = work.strDirectory.at(0);
					if (work.id == BROWER_IE)
						strDir = strDir.Left(strDir.ReverseFind(L'\\'));
// 					else if(work.id  == WIN_DESKTOPLNK)
// 					{
// 						strDir = strDir.Left(strDir.ReverseFind(L'\\'));
// 						strDir = strDir.Left(strDir.ReverseFind(L'\\'));
// 					}
					
					ItemText += L"【";
					ItemText += strDir;
					if(work.bRecursive&&ItemText.GetAt(ItemText.GetLength()-1)!=L'\\')
						ItemText += L"\\";
					ItemText += L"】";
				}
				if(!work.bDefaultSelect )
				{
					nCheck = KUIMulStatusTree::EM_TVIS_UNCHECK;
					if(m_FileListData.m_itemArray[work.id].ulAllFileSize > 0)
					{
						ItemText += L"(";
						ItemText += GetFileSizeText(m_FileListData.m_itemArray[work.id].ulAllFileSize);
						ItemText += L")";
					}
				}
				else
				{
					if (m_FileListData.m_itemArray[work.id].ulAllFileSize ==0)
						ItemText += ItemDesc;
				}

				hTreeItem = m_tree.InsertItem( ItemText
					, work.iconindex
					, work.iconindex,
					NULL, 
					NULL,  
					nCheck);
				m_IdTreeMap[work.id] = hTreeItem;
				m_TreeRootMap[hTreeItem] = work.id;

			}
		}
	}
	for(int j = 0;j < m_config.m_ConfigData.size();j++)
	{

		int nInsertNum = 0;  

		for (int i = 0; i < m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray.size(); i++)
		{
			CString strDir = L"";

			if(m_config.m_ConfigData[j].strDirectory.size()>0)
				strDir =  m_config.m_ConfigData[j].strDirectory.at(0);

			if (m_config.m_ConfigData[j].id == BROWER_IE)
			{
				strDir = strDir.Left(strDir.ReverseFind(L'\\'));
			}
// 			else if(m_config.m_ConfigData[j].id  == WIN_DESKTOPLNK)
// 			{
// 				strDir = strDir.Left(strDir.ReverseFind(L'\\'));
// 				strDir = strDir.Left(strDir.ReverseFind(L'\\'));
// 			}
			
			if(m_FileListData.m_itemArray[m_config.m_ConfigData[j].id].itemArray[i].ulFileSize != 0)
			{
				bool bSuffix = false;
				CString FileExt = GetExtName(m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).strFileName);
				CString strFileName = m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).strFileName;
				CString subDirFile = L"";
				if(m_config.m_ConfigData[j].id >= VIDEO_TUDOU&&m_config.m_ConfigData[j].id <= VIDEO_QQLIVE)
					subDirFile = strFileName.Mid(strDir.GetLength());
				else
				{
					subDirFile = strFileName.Mid(strDir.GetLength()+1);
				}

				for(int k = 0;k<m_SSuffix.size();k++)
				{
					if(FileExt.CompareNoCase(m_SSuffix[k]) == 0)
					{
						bSuffix = true;
						break;
					}
				}
				if(m_config.m_ConfigData[j].id >= VIDEO_TUDOU&&m_config.m_ConfigData[j].id <= VIDEO_QQLIVE&&bSuffix)
				{
// 					HTREEITEM hTmpItem = m_tree.InsertItem(
// 						subDirFile
// 						, m_IdDirMap[m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).ParentDirId].iconindex
// 						,  m_IdDirMap[m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).ParentDirId].iconindex,
// 						m_IdTreeMap[m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).ParentDirId], 
// 						NULL,  
// 						KUIMulStatusTree::EM_TVIS_UNCHECK);
					if(nInsertNum<9)
					{
						HTREEITEM hTmpItem = m_tree.InsertItem(
							subDirFile
							, m_IdDirMap[m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).ParentDirId].iconindex
							,  m_IdDirMap[m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).ParentDirId].iconindex,
							m_IdTreeMap[m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).ParentDirId], 
							NULL,  
							KUIMulStatusTree::EM_TVIS_NONE);

						m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).hItem = hTmpItem;
					}
					nInsertNum++;
					if (nInsertNum==9)
					{
						m_tree.InsertItem(L"..."//L"双击查看详情"
							, m_IdDirMap[m_config.m_ConfigData[j].id].iconindex
							,  m_IdDirMap[m_config.m_ConfigData[j].id].iconindex,
							m_IdTreeMap[m_config.m_ConfigData[j].id], 
							NULL,  
							KUIMulStatusTree::EM_TVIS_NONE);

					}
					//m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).hItem = hTmpItem;


				}
				if(m_config.m_ConfigData[j].id >= VIDEO_TUDOU
					&&m_config.m_ConfigData[j].id <= VIDEO_QQLIVE
					&& !bSuffix
					&&m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).bIsClear)
				{
// 					HTREEITEM hTmpItem = m_tree.InsertItem(
// 						subDirFile
// 						, m_IdDirMap[m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).ParentDirId].iconindex
// 						,  m_IdDirMap[m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).ParentDirId].iconindex,
// 						m_IdTreeMap[m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).ParentDirId], 
// 						NULL,  
// 						KUIMulStatusTree::EM_TVIS_CHECK);
					if(nInsertNum<9)
					{
						HTREEITEM hTmpItem = m_tree.InsertItem(
							subDirFile
							, m_IdDirMap[m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).ParentDirId].iconindex
							,  m_IdDirMap[m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).ParentDirId].iconindex,
							m_IdTreeMap[m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).ParentDirId], 
							NULL,  
							KUIMulStatusTree::EM_TVIS_NONE);

						m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).hItem = hTmpItem;
					}
					nInsertNum++;
					if (nInsertNum==9)
					{
						m_tree.InsertItem(L"..."//L"双击查看详情"
							, m_IdDirMap[m_config.m_ConfigData[j].id].iconindex
							,  m_IdDirMap[m_config.m_ConfigData[j].id].iconindex,
							m_IdTreeMap[m_config.m_ConfigData[j].id], 
							NULL,  
							KUIMulStatusTree::EM_TVIS_NONE);


					}
					//m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).hItem = hTmpItem;


				}
				else if(m_config.m_ConfigData[j].id < VIDEO_TUDOU||m_config.m_ConfigData[j].id > VIDEO_QQLIVE)
				{
					if(nInsertNum<9)
					{
						HTREEITEM hTmpItem = m_tree.InsertItem(
							subDirFile
							, m_IdDirMap[m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).ParentDirId].iconindex
							,  m_IdDirMap[m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).ParentDirId].iconindex,
							m_IdTreeMap[m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).ParentDirId], 
							NULL,  
							KUIMulStatusTree::EM_TVIS_NONE);

						m_FileListData.m_itemArray.at(m_config.m_ConfigData[j].id).itemArray.at(i).hItem = hTmpItem;
					}
					nInsertNum++;
					if (nInsertNum==9)
					{
						m_tree.InsertItem(L"..."//L"双击查看详情"
							, m_IdDirMap[m_config.m_ConfigData[j].id].iconindex
							,  m_IdDirMap[m_config.m_ConfigData[j].id].iconindex,
							m_IdTreeMap[m_config.m_ConfigData[j].id], 
							NULL,  
							KUIMulStatusTree::EM_TVIS_NONE);


					}
				}

			}
		}
	}
	m_tree.Expand(m_IdTreeMap[0]);
	m_tree.Expand(m_IdTreeMap[2],TVE_COLLAPSE);
	m_tree.Expand(m_IdTreeMap[3],TVE_COLLAPSE);
}

DWORD WINAPI CMainDialog::InsertThread(LPVOID lpVoid)
{


// 	CMainDialog* pthis = (CMainDialog*)lpVoid;
// 
// 
// 	for (int uCurPos = 0; uCurPos<pthis->m_config.m_ConfigData.size();uCurPos++)
// 	{
// 		int nCurId = pthis->m_config.m_ConfigData[uCurPos].id;
// 		int nInsertNum = 0;
// 		for (int nIndex = 0;
// 			nIndex <pthis->m_FileListData.m_itemArray.at(nCurId).itemArray.size();
// 			nIndex++)
// 		{
// 			int nCheck = KUIMulStatusTree::EM_TVIS_UNCHECK;
// 			if(pthis->m_IdDirMap[nCurId].bDefaultSelect)
// 				nCheck = KUIMulStatusTree::EM_TVIS_CHECK;
// 
// 			CString FileExt = GetExtName(pthis->m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).strFileName);
// 			CString strFileName = pthis->m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).strFileName;
// 			CString subDirFile = L"";
// 			CString strDir = pthis->m_IdDirMap[nCurId].strDirectory.at(0);
// 			if(nCurId == BROWER_IE)
// 			{
// 				strDir = strDir.Left(strDir.ReverseFind(L'\\'));
// 			}
// 			if (nCurId >= VIDEO_TUDOU&&nCurId <= VIDEO_QQLIVE)
// 				subDirFile = strFileName.Mid(strDir.GetLength());
// 			else subDirFile = strFileName.Mid(strDir.GetLength()+1);
// 
// 			for(int i = 0;i<pthis->m_SSuffix.size();i++)
// 			{
// 				if (FileExt.CompareNoCase(pthis->m_SSuffix[i]) == 0)
// 				{
// 					nCheck = KUIMulStatusTree::EM_TVIS_UNCHECK;
// 					break;
// 				}
// 			}
// 
// 			if (nCheck == KUIMulStatusTree::EM_TVIS_UNCHECK && (nCurId >=  VIDEO_TUDOU&& nCurId <= VIDEO_QQLIVE))
// 			{
// 
// // 				HTREEITEM hTmpItem = pthis->m_tree.InsertItem(  
// // 					subDirFile
// // 					, pthis->m_IdDirMap[uCurPos].iconindex
// // 					,  pthis->m_IdDirMap[uCurPos].iconindex,
// // 					pthis->m_IdTreeMap[uCurPos], 
// // 					NULL,  
// // 					nCheck);
// 				if(nInsertNum == 1)
// 				{
// 					pthis->m_tree.InsertItem( L"双击查看详情"
// 						, pthis->m_IdDirMap[nCurId].iconindex
// 						, pthis->m_IdDirMap[nCurId].iconindex,
// 						pthis->m_IdTreeMap[nCurId], 
// 						NULL,  
// 						KUIMulStatusTree::EM_TVIS_NONE);
// 
// 				}
// 				pthis->m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).bIsClear = true;
// 				//pthis->m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).hItem = hTmpItem;
// 				pthis->m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).ParentDirId = nCurId;
// 				nInsertNum++;
// 
// 			}
// 			else if (nCheck == KUIMulStatusTree::EM_TVIS_CHECK && (nCurId >= VIDEO_TUDOU && nCurId <= VIDEO_QQLIVE))
// 			{
// 
// // 				HTREEITEM hTmpItem = pthis->m_tree.InsertItem(
// // 					subDirFile
// // 					, pthis->m_IdDirMap[nCurId].iconindex
// // 					,  pthis->m_IdDirMap[nCurId].iconindex,
// // 					pthis->m_IdTreeMap[nCurId], 
// // 					NULL,  
// // 					nCheck);
// 				if(nInsertNum == 1)
// 				{
// 					pthis->m_tree.InsertItem( L"双击查看详情"
// 						, pthis->m_IdDirMap[nCurId].iconindex
// 						, pthis->m_IdDirMap[nCurId].iconindex,
// 						pthis->m_IdTreeMap[nCurId], 
// 						NULL,  
// 						KUIMulStatusTree::EM_TVIS_NONE);
// 				}
// 				pthis->m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).bIsClear = true;
// 				//pthis->m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).hItem = hTmpItem;
// 				pthis->m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).ParentDirId = nCurId;
// 				nInsertNum++;
// 
// 			}
// 			else if (nCurId < VIDEO_TUDOU || nCurId > VIDEO_QQLIVE)
// 			{
// 				if(nInsertNum<9)
// 				{
// 					HTREEITEM hTmpItem = pthis->m_tree.InsertItem( 
// 						subDirFile
// 						, pthis->m_IdDirMap[nCurId].iconindex
// 						, pthis->m_IdDirMap[nCurId].iconindex,
// 						pthis->m_IdTreeMap[nCurId], 
// 						NULL,  
// 						KUIMulStatusTree::EM_TVIS_NONE);
// 					pthis->m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).hItem = hTmpItem;
// 				}
// 				pthis->m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).bIsClear = true;
// 
// 				pthis->m_FileListData.m_itemArray.at(nCurId).itemArray.at(nIndex).ParentDirId = nCurId;
// 				nInsertNum ++;
// 				if(nInsertNum == 9)
// 				{
// 					pthis->m_tree.InsertItem( L"双击查看更多..."
// 						, pthis->m_IdDirMap[nCurId].iconindex
// 						, pthis->m_IdDirMap[nCurId].iconindex,
// 						pthis->m_IdTreeMap[nCurId], 
// 						NULL,  
// 						KUIMulStatusTree::EM_TVIS_NONE);
// 
// 
// 				}
// 			}
// 
// 		}
// 
// 	}
// 
// 	pthis->m_tree.Invalidate();

	

// 	::CloseHandle(pthis->m_insertThread);
// 	InterlockedExchange((LONG*)&pthis->m_insertThread, NULL);
// 	InterlockedExchange((LONG*)&pthis->m_dwInsertThreadID, 0);

	return 0;
}
