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


/////////////////////////////////////////////////////////////////////////////

#include "traversefile.h"
#include "findfiletraverse.h"
#include "configdata.h"
#include "filelistdata.h"
#include "wtlctrls/atlbuttenstatc.h"
#include "wtlctrls/kbitmapbutton.h"
#include "wtlctrls/guidef.h"
#include "wtlctrls/hlhovercr.h"
#include "trashdefine.h"
#include "wtlctrls/kuimulstatustree.h"
#include "wtlctrls/kuitraversabletreectrl.h"
#include "kscver.h"
#include "kdlgdoingprocess.h"

typedef struct MessageP
{
     ULONGLONG m_ulTotalJunkFileSize;
     CString filepath;
}MPass;


class CMainDialog : 
    public CDialogImpl<CMainDialog>,public ITraverseFile
{
    // Construction
public:
    CMainDialog();   // standard constructor
    ~CMainDialog();
   
    enum { IDD = IDD_MAIN_DIALOG };

  
    BEGIN_MSG_MAP(CMainDialog)
        MESSAGE_HANDLER(WM_KAN_NOTIFY_TREE_CHECKED_CHANGE, OnCheckChange)
        MESSAGE_HANDLER(WM_INITDIALOG,OnInitDialog)
        MESSAGE_HANDLER(WM_OWN_MSGBOX, OnOwnMsgBox)
        MSG_WM_DESTROY(OnDestroy)
       // MSG_WM_CTLCOLORSTATIC(OnCtlColor)
        COMMAND_ID_HANDLER(IDC_CHECK1, OnSelall)
        COMMAND_ID_HANDLER(IDC_CHECK_STATIC, OnStaticSellAll)
        COMMAND_ID_HANDLER(IDC_RESCAN, OnRescan)
        COMMAND_ID_HANDLER(IDC_CLEARFILE, OnClearfile)
        MSG_WM_SIZE(OnSize)
        MESSAGE_HANDLER(WM_KAN_NOTIFY_TREE_SELECT_CHANGE,OnItemchangedDirectorylist)
        COMMAND_ID_HANDLER(IDM_OPENDIR, OnOpendir)
        COMMAND_ID_HANDLER(IDC_RCOMD, OnRecommand)
        MESSAGE_HANDLER(WM_TRAVERSEFILE, OnTraverseFile)
        MESSAGE_HANDLER(WM_TRAVERSEPROCESS, OnTraverseProcess)
        MESSAGE_HANDLER(WM_TRAVERSEPROCESSEND, OnTraverseProcessEnd)
        MESSAGE_HANDLER(WM_TRAVERSEFINISHED, OnTraverseFinished)
        MSG_WM_TIMER(OnTimer)
        MSG_WM_PAINT(OnPaint)
        REFLECT_NOTIFICATIONS()
     END_MSG_MAP()


   

protected:
    void _StartFind(BOOL Is_Init = FALSE);
    void SizeChildWindow();
    void EnableClearBtn();
    void RunClearFile();
    static DWORD WINAPI ClearFileThread(LPVOID lpVoid);
    static DWORD WINAPI WaitThread(LPVOID lpVoid);
	static DWORD WINAPI InsertThread(LPVOID lpVoid);
    void RefreshTree(CString);

private:
    CBrush      m_bkBrush;
    CConfigData m_config;
    MPass       m_pass;
    CFileListData  m_FileListData;
    int m_nCurrentFindIndex;
    int m_nProIndex;
    int m_nDestroy;
    BOOL Is_Scan;
    HANDLE m_hThread;
    HANDLE m_tmpThread;
	HANDLE m_insertThread;
    DWORD  m_dwThreadID;
	DWORD  m_dwInsertThreadID;
    int m_nScanStatus;
    bool is_init;
    bool is_finish;
    bool is_ok;
    bool is_havespecsuffix;
    CKscVersion m_pAppVersion;

    CButton m_check_all;
    KBitmapButton m_btn_rescan;
    KBitmapButton m_btn_clearfile;
    CStaticBtn m_check_static;
    CHyperLinkHoverCr m_recommand;
    std::map<int,int> m_mapclearflag;

    CImageList										m_imlTreeIcons;
    KUITraversableTreeCtrl<KUIMulStatusTree>		m_tree;
    typedef std::map<HTREEITEM,int>	                TREE_ROOT_MAP;
    typedef std::map<int,HTREEITEM>	                ID_TREE_MAP;
    typedef std::map<int,DIRECTORYWORK>	            ID_DIR_MAP;
    TREE_ROOT_MAP								    m_TreeRootMap;			// 所有根元素的列表
    ID_TREE_MAP										m_IdTreeMap;		// 被选中的根元素的列表
    ID_DIR_MAP                                      m_IdDirMap;
    std::vector<CString>                            m_SSuffix;
	CTrashDoingDlg*									m_pdlgDel;
	int                                             m_nDelTotalCount;
	ULONGLONG                                       m_nDelTotalSize;
	CString										m_strCacheFile;
	HANDLE m_hEvent;

	BOOL											m_bFindFileRunningSkip;
    
public:
     LRESULT OnCheckChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
     LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
     void OnDestroy();
     //HBRUSH OnCtlColor(HDC hDC, HWND hWnd);
     LRESULT OnSelall(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);
     LRESULT OnRescan(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);
     LRESULT OnClearfile(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);
     void OnSize(UINT wParam, const _WTYPES_NS::CSize& size);
     LRESULT OnItemchangedDirectorylist(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
     LRESULT OnOpendir(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);
     LRESULT OnRecommand(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);
     void OnTimer(UINT_PTR nIDEvent);
     LRESULT OnStaticSellAll(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);
     LRESULT OnTraverseFile(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
     LRESULT OnTraverseProcess(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
     LRESULT OnTraverseProcessEnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
     LRESULT OnTraverseFinished(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
     virtual void TraverseFile(LPFINDFILEDATA pFileData);
     virtual void TraverseProcess(unsigned long uCurPos);
     virtual void TraverseProcessEnd(unsigned long uCurPos,CString vedioPath);
     virtual void TraverseFinished();
     void OnPaint(HDC hDC);
     LRESULT OnOwnMsgBox(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    
  

     
};


