
#pragma once

#include "runoptlistview.h"
#include "runautooptlist.h"
#include <runoptimize/interface.h>
#include "bkmsgdefine.h"

#define STATE_RUNOPT_NULL		0
#define STATE_RUNOPT_SCAN		1
#define STATE_RUNOPT_CANCEL		2

class CBeikeSafeMainDlg;
class CBeikeSafeSysOptUIHandler :
	public IEnumRunCallBackNull,
	public IAutoOptLstCBack,
	public IListOpCallback
{
public:
	CBeikeSafeSysOptUIHandler(CBeikeSafeMainDlg* refDialog)
		: m_dlg(refDialog)
		, m_pRunOptEng(NULL)
		, m_nState(STATE_RUNOPT_NULL)
		, m_bFirstShowed(FALSE)
		, m_bFirstScanRunEnd(FALSE)
		, m_bSkipToOnekeyOpt(FALSE)
		, m_hOptimizeThread(NULL)
		, m_bOpting(FALSE)
		, m_bRefreshList(FALSE)
		, m_nIgnoredCount(0)
		, m_bReportOnekeyOpt(TRUE)
	{
	}

	virtual ~CBeikeSafeSysOptUIHandler()
	{
		if (m_pRunOptEng!=NULL)
		{
			CRunOptLoader::GetGlobalPtr()->FreeOptEng(m_pRunOptEng);
			m_pRunOptEng = NULL;
		}
		if (m_hOptimizeThread != NULL)
		{
			::CloseHandle(m_hOptimizeThread);
			m_hOptimizeThread = NULL;
		}
	}

public:
	BOOL IsOpPermit()
	{
		return (m_nState==STATE_RUNOPT_NULL);
	}

	void Init();
	void FirstShow();
	void SecondShow();
	BOOL OnBkRunTypeSelChange(int nTabItemIDOld, int nTabItemIDNew);
	BOOL OnBkSysOptSelChange(int nTabItemIDOld, int nTabItemIDNew);

	void InitEng()
	{
		if (m_pRunOptEng==NULL)
			m_pRunOptEng = CRunOptLoader::GetGlobalPtr()->GetOptEng();
	}

	BOOL FindRunItem(CKsafeRunInfo* pInfo);
	BOOL _FindRunItem(CKsafeRunInfo* pInfo);
	void EndScan();
	void RestoreEnd();
	void PostRestoreRunItem(DWORD nType,BOOL bEnable,LPCTSTR lpName);

	LRESULT OnWmScanEnd(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmRestoreEnd(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmFindItem(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWmFirstIn(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnOnekeyOptEnd(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnRefreshOnekeyOpt(UINT uMsg, WPARAM wParam, LPARAM lParam);

	BOOL OnClickButton(CKsafeRunInfo* pInfo);
	BOOL OnDeleteItem(CKsafeRunInfo* pInfo);
	BOOL OnClickLabel(DWORD nIndex, CKsafeRunInfo* pInfo);
	void ClickCheck();

    void UpdateClearInterface(int nSel);

public:

	void OnBkShowDisableItem();
	void OnListReSize(CRect rcWnd);
	void RefreshItemList();
	void OnBkCancelAll();
	void OnBkCancelAll2();
	void OnBkBtnWaitMore();
	void OnBkRunOptTopMore();

	void RefreshCLRState(int nSel=-1, BOOL bDowning=FALSE);
    void RefreshUpdateState(int nSel=-1, BOOL bDowning=FALSE);

	void RefreshTabNumber(DWORD nNewTab=-1);
	void SetTopRunDiv(DWORD nType);
	void GetRunListByCheckState();


	void InitAutoOptList();
	void OnBkBtnDoOpt();
	void OnAutoOptCheckAll();
	void OnAutoOptCheckAll2();
	void OnBkBtnRefresh();
	void OnBkBtnContinueOpt();
	void OnBkLnkViewRun();
	void OnBkLnkViewServ();
	void OnBkLnkViewTask();
	void OnTimer(UINT_PTR nIDEvent);//优化时定时器刷新显示loading动画
	void ShowOptDetail();
	void OnBkLnkCleanTrash();
	void OnBkLnkSoftMgr_ZJBJ();
	void OnBkCheckHideSystem();
	void ShowNoTaskInfo();
	static DWORD WINAPI	OneKeyOptProc(LPVOID lpParam);
	BOOL Is3XXInstalled();
	void OnBkLnkShowIgnored();
	void OnBkLnkAddToIgnored();
	void SaveIgnoreID();
	void LoadIgnoredID();
	void GetIgnoredItem();

	void OnBkLnkRefreshList();
	
protected:
	CBeikeSafeMainDlg*		m_dlg;
	CRunOptListView			m_runList;
	IRunOptimizeEng*		m_pRunOptEng;
	DWORD					m_nState;
	BOOL					m_bFirstShowed;
	BOOL					m_bFirstScanRunEnd;
	BOOL					m_bOpting;
	BOOL					m_bSkipToOnekeyOpt;
	BOOL					m_bRefreshList;		//runlist刷新后置为TRUE,为TRUE则InitAutoOptList,再置为FALSE
	BOOL					m_bReportOnekeyOpt;	//进入一键优化tab，是否要上报
	CAutoRunOptList			m_list;
	HANDLE					m_hOptimizeThread;
	CString					m_strIniFilePath;
	CString					m_strIgnoredIniPath;
	int						m_nIgnoredCount;
	CSimpleArray<int>		m_arrayIgnoredID;
	CSimpleArray<CKsafeRunInfo*>	m_arrayItem;			//所有启动项
	CSimpleArray<CKsafeRunInfo*>	m_arrayIgnoredItem;		//忽略项
	CSimpleArray<CKsafeRunInfo*>	m_arrayOnekeyOptItem;	//除去忽略项之后的启动项
public:
	BK_NOTIFY_MAP(IDC_RICHVIEW_WIN)
		BK_NOTIFY_TAB_SELCHANGE(IDC_TAB_RUNOPT_TYPE,OnBkRunTypeSelChange)
		BK_NOTIFY_TAB_SELCHANGE(IDC_TAB_SYSOPT_LEFT,OnBkSysOptSelChange)
		BK_NOTIFY_ID_COMMAND(IDC_CHECKBOX_SHOW_DISABLE,OnBkShowDisableItem)
		BK_NOTIFY_ID_COMMAND(IDC_LBL_RUNOPT_REFRESH,OnBkLnkRefreshList)
		BK_NOTIFY_ID_COMMAND(IDC_BTN_SYSOPT_WAIT_MORE,OnBkBtnWaitMore)
		BK_NOTIFY_ID_COMMAND(IDC_BTN_RUNOPT_CANCEL_ALL,OnBkCancelAll)
		BK_NOTIFY_ID_COMMAND(IDC_LBL_RUNOPT_TOP_MORE,OnBkRunOptTopMore)
		
		BK_NOTIFY_ID_COMMAND(IDC_BTN_RUN_AUTOOPT_DO,OnBkBtnDoOpt)
		BK_NOTIFY_ID_COMMAND(IDC_CHECKBOX_AUTOOPT_ALL,OnAutoOptCheckAll)
		BK_NOTIFY_ID_COMMAND(IDC_CHECKBOX_AUTOOPT_ALL2,OnAutoOptCheckAll2)
		BK_NOTIFY_ID_COMMAND(IDC_LNK_AUTOOPT_BTM_REFRESH,OnBkBtnRefresh)
		BK_NOTIFY_ID_COMMAND(IDC_BTN_AUTOOPT_BTM_CONTINUE,OnBkBtnContinueOpt)
		
		BK_NOTIFY_ID_COMMAND(IDC_LNK_SHOW_IGNORED,OnBkLnkShowIgnored)
		BK_NOTIFY_ID_COMMAND(IDC_LNK_VIEW_IGNORED,OnBkLnkShowIgnored)
		BK_NOTIFY_ID_COMMAND(IDC_LNK_ADDTO_IGNORED,OnBkLnkAddToIgnored)
		BK_NOTIFY_ID_COMMAND(IDC_LNK_ADDTO_IGNORED2,OnBkLnkAddToIgnored)
		BK_NOTIFY_ID_COMMAND(IDC_LNK_VIEW_RUN,OnBkLnkViewRun)
		BK_NOTIFY_ID_COMMAND(IDC_LNK_VIEW_SERV,OnBkLnkViewServ)
		BK_NOTIFY_ID_COMMAND(IDC_LNK_VIEW_TASK,OnBkLnkViewTask)
		BK_NOTIFY_ID_COMMAND(IDC_LNK_AUTOOPT_CANCELALL,OnBkCancelAll2)
		BK_NOTIFY_ID_COMMAND(IDC_CHECKBOX_RUNOPT_HIDE_SYSTEM,OnBkCheckHideSystem)
		BK_NOTIFY_REALWND_RESIZED(IDC_LST_RUNOPT, OnListReSize)
	BK_NOTIFY_MAP_END()

	BEGIN_MSG_MAP_EX(CBeikeSafeSysOptUIHandler)
		MSG_WM_TIMER(OnTimer)	//优化时定时器刷新显示loading动画
		MSG_BK_NOTIFY(IDC_RICHVIEW_WIN)
		MESSAGE_HANDLER_EX(MSG_SYSOPT_RUN_SCAN_END,		OnWmScanEnd)
		MESSAGE_HANDLER_EX(MSG_SYSOPT_RUN_RESTORE_END,	OnWmRestoreEnd)
		MESSAGE_HANDLER_EX(MSG_SYSOPT_RUN_SCAN_FIND,	OnWmFindItem)
		MESSAGE_HANDLER_EX(MSG_SYSOPT_RUN_FIRST_IN,		OnWmFirstIn)
		MESSAGE_HANDLER_EX(MSG_SYSOPT_ONEKEYOPT_END,	OnOnekeyOptEnd);
		MESSAGE_HANDLER_EX(MSG_SYSOPT_REFRESH_ONEKEYOPT,	OnRefreshOnekeyOpt)
	END_MSG_MAP()
};