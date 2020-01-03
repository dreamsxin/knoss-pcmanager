#pragma once

#include "wtlhelper/whwindow.h"
#include "bkres/bkres.h"
#include "kmsgdefine.h"
#include "KActor.h"
#include "PrivacyProtectionGUI.h"
#include "resource.h"
#include "ListViewCtrlEx.h"


class KAddSoftwareDlg
	: public CBkDialogImpl<KAddSoftwareDlg>
	, public CWHRoundRectFrameHelper<KAddSoftwareDlg>
	, public IActorCallBack
{
public:
	KAddSoftwareDlg()
		: CBkDialogImpl<KAddSoftwareDlg>(IDR_ADD_SOFTWARE_DLG)
	{
	}

	~KAddSoftwareDlg();

	//////////////////////////////////////////////////////////////////////////
	//  µœ÷IActorCallBack
	virtual void	OnActivate(KActor* pActor);


	typedef struct _SOFTWARE_INFO
	{
		CString					strName;
		CStringA				strDataSign;
		CString					strCompany;
		CString					strProcessFile;
		HICON					hIcon;
		bool					bExist;
	} SOFTWARE_INFO;
	
	
	CSimpleMap<CString, SOFTWARE_INFO>		m_SoftwareMap;
	CSimpleArray<DWORD>						m_IndexArray;		

protected:

	KActor				m_ActorInit;

	CListViewCtrlEx		m_ResultList;



	void			OnBtnClose();
	void			OnBtnCustom();
	void			OnBtnOk();
	void			OnSelectAllCheck();
	void			OnSelectAll();
	LRESULT			OnUpdateResult(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);

	BOOL			OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/);
	void			EndDialog(UINT uRetCode);
	void			OnSysCommand(UINT nID, CPoint point);

	bool			InitSoftwareData();

public:
	BK_NOTIFY_MAP(IDC_RICHVIEW_WIN)
		BK_NOTIFY_ID_COMMAND(IDC_BTN_SYS_CLOSE, OnBtnClose)
		BK_NOTIFY_ID_COMMAND(103, OnBtnCustom)
		BK_NOTIFY_ID_COMMAND(104, OnBtnOk)
		BK_NOTIFY_ID_COMMAND(109, OnSelectAllCheck)
		BK_NOTIFY_ID_COMMAND(110, OnSelectAll)
		BK_NOTIFY_MAP_END()

		BEGIN_MSG_MAP_EX(KAddSoftwareDlg)
		MSG_BK_NOTIFY(IDC_RICHVIEW_WIN)
		CHAIN_MSG_MAP(CBkDialogImpl<KAddSoftwareDlg>)
		CHAIN_MSG_MAP(CWHRoundRectFrameHelper<KAddSoftwareDlg>)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_SYSCOMMAND(OnSysCommand)
		MESSAGE_HANDLER_EX(MSG_UPDATE_RESULT, OnUpdateResult)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

};