#pragma once

#include "wtlhelper/whwindow.h"
#include "bkres/bkres.h"
#include "kmsgdefine.h"
#include "KActor.h"
#include "PrivacyProtectionGUI.h"
#include "resource.h"
#include "MonitorItemList.h"
#include "ListViewCtrlEx.h"
#include "./kuc//kxeuc.h"
#include <atltime.h>
#include "KRuleImpl.h"
#include "KPluginLoader.h"
#include "publish/ikisgui.h"


class KMainDlg
    : public CBkDialogImpl<KMainDlg>
    , public CWHRoundRectFrameHelper<KMainDlg>
	, public IActorCallBack
	, public IClickLinkCallback
{
public:
    KMainDlg()
        : CBkDialogImpl<KMainDlg>(IDR_MAIN_DIALOG)
    {
		m_bInit		= false;
		m_dwTime	= 0L;
		m_bStart	= false;

		m_hTrayMenu = NULL;

		m_uTaskbarCreatedMsg = RegisterWindowMessage(_T("TaskbarCreated"));


		//m_nSaveLimit = 0;
    }

    ~KMainDlg();

    void			SetDefaultNavigate(LPCWSTR lpszNavigate);

	//////////////////////////z////////////////////////////////////////////////
	// 实现IActorCallBack
	virtual void	OnActivate(KActor* pActor);

	//////////////////////////z////////////////////////////////////////////////
	// 实现IClickLinkCallback
	virtual void	OnCheck(DWORD nId, bool bCheck);
	virtual void	OnDelete(DWORD nId);

protected:

    CString				m_strDefaultNavigate;


	KActor				m_ActorInit;
	KActor				m_ActorCleaner;
	KActor				m_ActorMonitorWebCam;
	KLogic				m_Logic;
	KRuleImpl			m_RuleImpl;

	typedef struct _RESULT_INFO
	{
		CString		strName;
		HICON		hIcon;
		BOOL		bMonitor;
	} RESULT_INFO;

	CSimpleMap<CString, RESULT_INFO>		m_ResultInfoMap;
	CString									m_SaveResultFilePath;

	CMonitorItemList	m_MonitorItemlist;
	CMonitorItemList	m_MonitorSoftwarelist;
	CListViewCtrlEx		m_ResultList;
	CListViewCtrlEx		m_SoftwareResultList;

	bool				m_bInit;
	DWORD				m_dwTime;
	CString				m_savePath;		// 图片保存路径
	bool				m_bStart;

	HMENU				m_hTrayMenu;
	HICON				m_hIcons;
	UINT				m_uTaskbarCreatedMsg;

	CString				m_strCameraDeviceName;
	bool				m_bCamera;

	CString				m_strStatus;
	CString				m_strTime;
	CString				m_strResult;

	// 痕迹清理
	KPluginLoader<IModule> m_KClearLoader;

	CSimpleMap<CString, bool>		m_WhiteSignDataMap;

    void			OnBtnMin();
	void			OnBtnMax();
    void			OnBtnClose();
	void			OnBtnToFriend();
	void			OnBtnPic();
	void			OnSetting();
	void			OnUpdate();
	void			OnSelectAll();
	void			OnAddFile();
	void			OnAddDir();
	void			OnRiskResultCheck();
	void			OnRiskResult();
	void			OnStop();
	void			OnContinue();
	void			OnClearResult();
	void			OnExportResult();
	void			OnAddSoftware();

	/* @brief	保存路径，返回用户选择的文件格式Index */
	int				GetSaveAsPath(OUT CString &strPath);
	BOOL			GetSaveRect(OUT CRect &cRect);
	BOOL			SaveCapture();
	CString			GetGUID(int nIndex);

    BOOL			OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/);
    LRESULT			OnAppDefaultNavigate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	LRESULT			OnUpdateResult(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	LRESULT			OnStart(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	LRESULT			OnUpdateCameraInfo(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	LRESULT			OnUninstall(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	LRESULT			OnCameraReboot(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	void			EndDialog(UINT uRetCode);

    BOOL			OnBkTabMainSelChange(int nTabItemIDOld, int nTabItemIDNew);
    void			OnSysCommand(UINT nID, CPoint point);
	void			OnResultListReSize(CRect rcWnd);
	void			OnKClearReSize(CRect rcWnd);
	void			OnTimer(UINT_PTR nIDEvent);
	LRESULT			OnTrayIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT			OnMainApp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT			OnSOS(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT			OnExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT			OnTaskbarCreated(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	/* @brief	nType == 2 有NIF_TIP和NIF_INFO泡泡，nType == 1 只有NIF_TIP泡泡*/
	void			SetTipInfo(LPCTSTR lpText, LPCTSTR lpTittle, int nType);
	void			Showup();

	void			StartMon();
	BOOL			Filter(PVOID pvData, int nLevel);
	BOOL			AddMonitorList();
	void			AddMonitorSoftwareList();
	void			InitWhiteSignData();

	BOOL			SaveMonitorList();
	void			ShowTray();
	void			CloseByType(int nType);


	BOOL					KReleaseAndSatrtSys();
	static DWORD WINAPI		KpReleaseAndSatrtSys(LPVOID lpThreadParameter);


	static DWORD WINAPI KucCallBack(
		/* [in] */ KucIAllocator*   piAllocator,             // 返回数据使用的分配器
		/* [in] */ PVOID            pvData,                  // 数据
		/* [in] */ DWORD            dwDataLength,            // 数据长度
		/* [in] */ PVOID            pvParam,                 // 回调的自定义参数
		/* [in] */ PVOID*           ppvDataRet,              // 返回的数据, 如果不需要返回则ppvDataRet == NULL
		/* [in] */ DWORD*           pdwDataLengthRet         // 返回的数据长度, 如果不需要返回则pdwDataLengthRet == NULL
	);

public:
    BK_NOTIFY_MAP(IDC_RICHVIEW_WIN)
		BK_NOTIFY_ID_COMMAND(IDC_BTN_SYS_CLOSE, OnBtnClose)
		BK_NOTIFY_ID_COMMAND(IDC_BTN_SYS_MAX, OnBtnMax)
		BK_NOTIFY_ID_COMMAND(IDC_BTN_SYS_MIN, OnBtnMin)
		BK_NOTIFY_ID_COMMAND(103, OnBtnToFriend)
		BK_NOTIFY_ID_COMMAND(104, OnBtnPic)
		BK_NOTIFY_ID_COMMAND(106, OnSelectAll)
		BK_NOTIFY_ID_COMMAND(107, OnAddFile)
		BK_NOTIFY_ID_COMMAND(108, OnAddDir)
		BK_NOTIFY_ID_COMMAND(109, OnRiskResultCheck)
		BK_NOTIFY_ID_COMMAND(110, OnRiskResult)
		BK_NOTIFY_ID_COMMAND(111, OnClearResult)
		BK_NOTIFY_ID_COMMAND(112, OnAddSoftware)
		BK_NOTIFY_ID_COMMAND(113, OnAddSoftware)
		BK_NOTIFY_ID_COMMAND(114, OnExportResult)
		//BK_NOTIFY_ID_COMMAND(1000, OnStart)
		BK_NOTIFY_ID_COMMAND(1001, OnStop)
		BK_NOTIFY_ID_COMMAND(1002, OnContinue)
	    BK_NOTIFY_ID_COMMAND(207, OnSetting)
		BK_NOTIFY_ID_COMMAND(208, OnUpdate)
        BK_NOTIFY_TAB_SELCHANGE(136, OnBkTabMainSelChange)
		BK_NOTIFY_REALWND_RESIZED(2001, OnResultListReSize)
		BK_NOTIFY_REALWND_RESIZED(2004, OnKClearReSize)
    BK_NOTIFY_MAP_END()

	BEGIN_MSG_MAP_EX(KMainDlg)
        MSG_BK_NOTIFY(IDC_RICHVIEW_WIN)
        CHAIN_MSG_MAP(CBkDialogImpl<KMainDlg>)
        CHAIN_MSG_MAP(CWHRoundRectFrameHelper<KMainDlg>)
		MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_SYSCOMMAND(OnSysCommand)
		MSG_WM_TIMER(OnTimer)
		MESSAGE_HANDLER(MSG_TRAYICON, OnTrayIcon)
		COMMAND_ID_HANDLER(ID_MAIN_APP, OnMainApp)
		COMMAND_ID_HANDLER(ID_SOS, OnSOS)
		COMMAND_ID_HANDLER(ID_EXIT, OnExit)
		MESSAGE_HANDLER(m_uTaskbarCreatedMsg, OnTaskbarCreated)
        MESSAGE_HANDLER_EX(MSG_APP_DEFAULT_NAVIGATE, OnAppDefaultNavigate)
		MESSAGE_HANDLER_EX(MSG_UPDATE_RESULT, OnUpdateResult)
		MESSAGE_HANDLER_EX(MSG_START, OnStart)
		MESSAGE_HANDLER_EX(MSG_UPDATE_CAMERA_INFO, OnUpdateCameraInfo)
		MESSAGE_HANDLER_EX(MSG_UNINSTALL, OnUninstall)
		MESSAGE_HANDLER_EX(MSG_CAMERA_REBOOT, OnCameraReboot)
		REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()

};