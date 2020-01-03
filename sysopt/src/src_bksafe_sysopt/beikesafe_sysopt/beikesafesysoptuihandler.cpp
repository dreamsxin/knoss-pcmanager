#include "stdafx.h"
#include "beikesafe.h"
#include "beikesafemaindlg.h"
#include "beikesafesysoptuihandler.h"
#include "beikesafemsgbox.h"
#include "beikesafesysoptignoredlg.h"
#include <inioperator/cinifile.h>

#define TIMER_ID_RUNOPT_AUTOOPT_TOPTIP 111

static BOOL IsOpTrayRunItem(DWORD nRunType, LPCTSTR lpstrName)
{
	if (_tcsicmp(lpstrName,_T("KSafeTray"))==0)
	{
		if (IsHKLMRunType(nRunType))
			return TRUE;
	}
	return FALSE;
}

void CBeikeSafeSysOptUIHandler::Init()
{
	m_runList.SetOpCallback(this);

	m_runList.Create( 
		m_dlg->GetViewHWND(), NULL, NULL, 
		WS_CLIPCHILDREN | WS_CHILD |WS_VSCROLL | LBS_OWNERDRAWVARIABLE| LBS_HASSTRINGS , 
		0, IDC_LST_RUNOPT, NULL);
	m_runList.CreateX();

	m_list.Create( m_dlg->GetViewHWND(), 
		NULL, NULL, 
		WS_CLIPCHILDREN | WS_CHILD | LVS_REPORT | LVS_SINGLESEL | LVS_OWNERDRAWFIXED | LVS_NOCOLUMNHEADER, 
		0, IDC_LST_RUN_AUTOOPT, NULL);
	m_list.CreateToolTip();

	m_list.SetCallback(this);
	m_list.InsertColumn(0,BkString::Get(IDS_SYSOPT_4016));
	m_list.SetColumnWidth(0,205);
	m_list.InsertColumn(1,BkString::Get(IDS_SYSOPT_4017));
	m_list.SetColumnWidth(1,300);
	m_list.InsertColumn(2,BkString::Get(IDS_SYSOPT_4018));
	m_list.SetColumnWidth(2,100);

	CAppPath::Instance().GetLeidianAppPath(m_strIgnoredIniPath);
	m_strIgnoredIniPath.Append(IGNORED_FILEPATH);
}

BOOL CBeikeSafeSysOptUIHandler::_FindRunItem( CKsafeRunInfo* pInfo )
{
	CKsafeRunInfo*	pNewInfo = pInfo;
	m_arrayItem.Add(pNewInfo);
	SHFILEINFO		hFileInfo = {0};
	if (!pNewInfo->bSystem)
	{
		::SHGetFileInfo(pNewInfo->strExePath,FILE_ATTRIBUTE_NORMAL,&hFileInfo,sizeof(SHFILEINFO),SHGFI_ICON);
	}

	m_runList.InsertItemX(pNewInfo,hFileInfo.hIcon);

	if (pInfo->bUnknown)
	{
		struct RUN_RPT_DATA
		{
			DWORD		nType;
			CString		strname;
			CString		strApp;
		};
		static CSimpleArray<RUN_RPT_DATA>	listReportX;

		BOOL	bFind = FALSE;
		for (int i=0; i<listReportX.GetSize(); i++)
		{
			RUN_RPT_DATA&	item = listReportX[i];

			if (item.nType==pInfo->nType&&
				item.strname.CompareNoCase(pInfo->strName)==0 &&
				item.strApp.CompareNoCase(pInfo->strExePath)==0)
			{
				bFind = TRUE;
			}
		}

		if (!bFind)
		{
			RUN_RPT_DATA	item;
			item.nType		= pInfo->nType;
			item.strname	= pInfo->strName;
			item.strApp		= pInfo->strExePath;
			listReportX.Add(item);
		}
	}

	return TRUE;
}

void CBeikeSafeSysOptUIHandler::FirstShow()
{
	::PostMessage(m_dlg->m_hWnd,MSG_SYSOPT_RUN_FIRST_IN,NULL,NULL);
}

BOOL CBeikeSafeSysOptUIHandler::OnBkRunTypeSelChange( int nTabItemIDOld, int nTabItemIDNew )
{
	if (!IsOpPermit())
		return FALSE;

	m_runList.SetShowType(nTabItemIDNew);
	m_dlg->SetItemVisible(IDC_LBL_RUNOPT_BTM_SERVICE_OP,(nTabItemIDNew == KSRUN_TYPE_SERVICE) );
	m_dlg->SetItemVisible(IDC_LBL_RUNOPT_TOP_MORE,(nTabItemIDNew != 0));
	m_dlg->SetItemVisible(IDC_DIV_RUNOPT_BOTTOM,(nTabItemIDNew != 0));
	m_dlg->SetItemVisible(IDC_DIV_AUTOOPT,(nTabItemIDNew == 0));
	m_dlg->SetItemVisible(IDC_LST_RUNOPT,(nTabItemIDNew != 0));
	m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP,(nTabItemIDNew == 0));
	m_dlg->SetItemVisible(IDC_DIV_RUN_TOPTIP,(nTabItemIDNew != 0));
	m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_BTM , nTabItemIDNew == 0);
	if (0 == nTabItemIDNew)
	{
		if ((!m_bOpting) || m_bRefreshList)
			InitAutoOptList();

		m_dlg->SetItemVisible(IDC_DIV_NOTASK_INFO , FALSE);
		m_bReportOnekeyOpt = TRUE;
	}
	else if (nTabItemIDNew == KSRUN_TYPE_TASK)
	{
		ShowNoTaskInfo();
	}
	
	BOOL bHideSystemChecked = m_dlg->GetItemCheck(IDC_CHECKBOX_RUNOPT_HIDE_SYSTEM);
	m_runList.SetShowSystem(!bHideSystemChecked);

	m_runList.ResetExpandState(FALSE);//重置所有bExpand = FALSE
	GetRunListByCheckState();
	RefreshTabNumber(nTabItemIDNew);
	SetTopRunDiv(nTabItemIDNew);
	return TRUE;
}

BOOL CBeikeSafeSysOptUIHandler::OnClickButton( CKsafeRunInfo* pInfo )
{
	if (!IsOpPermit())
		return FALSE;

	if (pInfo->bEnable && !pInfo->strWarning.IsEmpty())
	{
		CString	strMsg;
        strMsg.Format(BkString::Get(IDS_SYSOPT_4000), pInfo->strWarning);
		if(CBkSafeMsgBox2::ShowMultLine(strMsg, NULL, MB_OKCANCEL|MB_ICONWARNING) != IDOK)
			return FALSE;
	}
	
	if (!pInfo->bEnable && Is3XXInstalled() && pInfo->IsDisabledBy3XX() && pInfo->nType == 1)
	{
		CString	strMsg;
		strMsg.Format(BkString::Get(IDS_SYSOPT_4058), pInfo->strWarning);
		CBkSafeMsgBox2::ShowMultLine(strMsg, NULL, MB_OK|MB_ICONWARNING);
	}

	if (m_pRunOptEng)
	{
		if (m_pRunOptEng->ControlRun(!pInfo->bEnable, pInfo))
		{
		}
		else
		{	
			CBkSafeMsgBox2::ShowMultLine(BkString::Get(IDS_SYSOPT_4001),NULL,MB_OK|MB_ICONWARNING);
		}
	}
	int i=m_runList.GetItemIndexByKSInfo(pInfo);

	if (i>=0)
	{
		CRect rcRow;
		m_runList.GetItemRect(i,&rcRow);
		m_runList.InvalidateRect(rcRow);
	}
	RefreshTabNumber();

	return TRUE;
}

void CBeikeSafeSysOptUIHandler::GetRunListByCheckState()
{
	BOOL bCheckState = m_dlg->GetItemCheck(IDC_CHECKBOX_SHOW_DISABLE);
	if (bCheckState)
		m_runList.SetSubShowType(SUB_SHOW_TYPE_ALL);
	else
		m_runList.SetSubShowType(SUB_SHOW_TYPE_ENABLE);
}

void CBeikeSafeSysOptUIHandler::OnBkShowDisableItem()
{
	GetRunListByCheckState();
	RefreshTabNumber();
}

void CBeikeSafeSysOptUIHandler::OnListReSize( CRect rcWnd )
{
	m_list.SetColumnWidth(1,rcWnd.Width()-322);
}

void CBeikeSafeSysOptUIHandler::OnBkLnkRefreshList()
{
	m_bSkipToOnekeyOpt = FALSE;
	RefreshItemList();
}

void CBeikeSafeSysOptUIHandler::RefreshItemList()
{
	if (!IsOpPermit())
		return;
	
	m_runList.ClearAll();
	m_arrayItem.RemoveAll();
	m_runList.SetLoadShow(TRUE);
	if (m_pRunOptEng!=NULL)
	{
		m_dlg->SetItemVisible(IDC_DIV_RUNOPT_BOTTOM,FALSE);
		m_pRunOptEng->EnumRunInfo(0,this);
		m_nState = STATE_RUNOPT_SCAN;
	}
	m_bRefreshList = TRUE;
}

void CBeikeSafeSysOptUIHandler::ShowNoTaskInfo()
{
	m_dlg->SetItemVisible(IDC_DIV_NOTASK_INFO , (0 == m_runList.GetDefShowTypeCount(KSRUN_TYPE_TASK)));
	m_dlg->SetItemVisible(IDC_DIV_RUN_TOPTIP , !(0 == m_runList.GetDefShowTypeCount(KSRUN_TYPE_TASK)));
	m_dlg->SetItemVisible(IDC_LST_RUNOPT , !(0 == m_runList.GetDefShowTypeCount(KSRUN_TYPE_TASK)));
}

void CBeikeSafeSysOptUIHandler::OnBkCancelAll()
{
	if (!IsOpPermit())
		return;

//	CString		str;
// 	DWORD		nType = m_dlg->GetTabCurSel(IDC_TAB_RUNOPT_TYPE);
// 	if (nType == 1)
// 		str = BkString::Get(IDS_SYSOPT_4002);
// 	else if (nType == 2)
// 		str = BkString::Get(IDS_SYSOPT_4003);
// 	else if (nType == 3)
// 		str = BkString::Get(IDS_SYSOPT_4004);

	CString	strShow;

// 	if ( BKSafeConfig::GetHasRunDelete() )
// 		strShow = BkString::Get(IDS_SYSOPT_4071);
// 	else
// 		strShow = BkString::Get(IDS_SYSOPT_4072);

	if ( CBkSafeMsgBox2::ShowMultLine(strShow, NULL, MB_YESNO|MB_ICONWARNING) != IDYES)
		return;

	if (m_pRunOptEng)
	{
		m_nState = STATE_RUNOPT_CANCEL;
		m_pRunOptEng->RestoreRun(4,this);
		m_dlg->SetItemVisible(IDC_DIV_RUNOPT_BOTTOM,FALSE);
	}
}

void CBeikeSafeSysOptUIHandler::OnBkCancelAll2()
{
	if (!IsOpPermit())
		return;

	CString	str;
	str = BkString::Get(IDS_SYSOPT_4057);
	CString	strShow;
	strShow.Format(BkString::Get(IDS_SYSOPT_4006),str);

	if ( CBkSafeMsgBox2::ShowMultLine(strShow, NULL, MB_YESNO|MB_ICONWARNING) != IDYES)
		return;
	
	if (m_pRunOptEng == NULL)
		return;

	for ( int i=0; i < m_list.GetItemCount(); i++)
	{
		_ARUN_ITEM_DATA* px = m_list.GetItemParamData(i);

		if (px && px->nType==ARUN_TYPE_ITEM && px->nStatus == ARUN_STATUS_SUCCESS && px->bOptimized)
		{
			if (m_pRunOptEng->ControlRun(!px->pInfo->bEnable, px->pInfo))
				px->nStatus = ARUN_STATUS_SUCCESS;
			else
				px->nStatus = ARUN_STATUS_FAILED;
		}
	}
	InitAutoOptList();
}

void CBeikeSafeSysOptUIHandler::EndScan()
{
	::PostMessage(m_dlg->m_hWnd,MSG_SYSOPT_RUN_SCAN_END,NULL,NULL);
}

void CBeikeSafeSysOptUIHandler::RestoreEnd()
{
	::PostMessage(m_dlg->m_hWnd,MSG_SYSOPT_RUN_RESTORE_END,NULL,NULL);
}

LRESULT CBeikeSafeSysOptUIHandler::OnWmScanEnd( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	m_bFirstShowed = TRUE;
	m_nState = STATE_RUNOPT_NULL;
	m_runList.SetShowDlgState();
	if ( !m_bFirstScanRunEnd )
	{
		CString	strReport;
		for ( int i=0; i < m_runList.GetAllCount(); i++)
		{
			CRunOptListView::_RUN_ITEM_DATA* pdata = m_runList.GetRunDataByIndex(i);
		}

		m_bFirstScanRunEnd = TRUE;
	}
	
	if (m_dlg->GetTabCurSel(IDC_TAB_RUNOPT_TYPE) != 0)
		m_dlg->SetItemVisible(IDC_DIV_RUNOPT_BOTTOM,TRUE);
	
	if (m_dlg->GetTabCurSel(IDC_TAB_RUNOPT_TYPE) == KSRUN_TYPE_TASK)
		ShowNoTaskInfo();

	if (m_bSkipToOnekeyOpt)
		m_dlg->SetTabCurSel(IDC_TAB_RUNOPT_TYPE, 0);
	RefreshTabNumber();
	return S_OK;
}

LRESULT CBeikeSafeSysOptUIHandler::OnWmRestoreEnd( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	m_dlg->SetItemVisible(IDC_DIV_RUNOPT_BOTTOM,TRUE);
	m_nState = STATE_RUNOPT_NULL;
	RefreshItemList();
	return S_OK;
}

LRESULT CBeikeSafeSysOptUIHandler::OnWmFindItem( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	_FindRunItem((CKsafeRunInfo*)wParam);
	RefreshTabNumber();
	return S_OK;
}

BOOL CBeikeSafeSysOptUIHandler::FindRunItem( CKsafeRunInfo* pInfo )
{
	CKsafeRunInfo*	pNew = new CKsafeRunInfo(*pInfo);
	::PostMessage(m_dlg->m_hWnd,MSG_SYSOPT_RUN_SCAN_FIND,(WPARAM)pNew,NULL);
	return TRUE;
}

void CBeikeSafeSysOptUIHandler::RefreshTabNumber(DWORD nCurTab)
{
	if (nCurTab==-1)
		nCurTab = m_dlg->GetTabCurSel(IDC_TAB_RUNOPT_TYPE);

	nCurTab++;

	CString	strfomat;
	DWORD	nShowCount = m_runList.GetShowCount();
	BOOL	bShowSystem = m_runList.IsShowSystem();

	strfomat.Format(BkString::Get(IDS_SYSOPT_4007), 
		m_runList.GetEnableTypeCount(KSRUN_TYPE_STARTUP),
		m_runList.GetDefShowTypeCount(KSRUN_TYPE_STARTUP)
		);
	m_dlg->SetTabTitle(IDC_TAB_RUNOPT_TYPE,1,strfomat);

	strfomat.Format(BkString::Get(IDS_SYSOPT_4008), 
		m_runList.GetEnableTypeCount(KSRUN_TYPE_SERVICE, bShowSystem),
		m_runList.GetDefShowTypeCount(KSRUN_TYPE_SERVICE, bShowSystem)
		);
	m_dlg->SetTabTitle(IDC_TAB_RUNOPT_TYPE,2,strfomat);

	strfomat.Format(BkString::Get(IDS_SYSOPT_4009), 
		m_runList.GetEnableTypeCount(KSRUN_TYPE_TASK),
		m_runList.GetDefShowTypeCount(KSRUN_TYPE_TASK)
		);
	m_dlg->SetTabTitle(IDC_TAB_RUNOPT_TYPE,3,strfomat);
}

BOOL CBeikeSafeSysOptUIHandler::OnBkSysOptSelChange( int nTabItemIDOld, int nTabItemIDNew )
{
	m_dlg->SetItemVisible(IDC_DIV_SYSOPT_RUN,nTabItemIDNew==0);
	m_dlg->SetItemVisible(IDC_DIV_SYSOPT_MORE,nTabItemIDNew==1);

	return TRUE;
}

void CBeikeSafeSysOptUIHandler::OnBkBtnWaitMore()
{
	::ShellExecute(NULL,_T("open"),_T("http://www.ijinshan.com/safe/optimize_advice.html?fr=client"),NULL,NULL,SW_SHOW);
}

void CBeikeSafeSysOptUIHandler::OnBkRunOptTopMore()
{
	::ShellExecute(NULL,_T("open"),_T("http://www.ijinshan.com/safe/optimize_intro.html?fr=client"),NULL,NULL,SW_SHOW);
}

void CBeikeSafeSysOptUIHandler::SetTopRunDiv( DWORD nType )
{
	m_dlg->SetItemVisible(IDC_DIV_RUNOPT_TOP_RUN,nType==1);
	m_dlg->SetItemVisible(IDC_DIV_RUNOPT_TOP_SERVICE,nType==2);
	m_dlg->SetItemVisible(IDC_DIV_RUNOPT_TOP_TASK,nType==3);
}

LRESULT CBeikeSafeSysOptUIHandler::OnWmFirstIn( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	InitEng();
	if (m_pRunOptEng==NULL)
	{
		CBkSafeMsgBox2::ShowMultLine( BkString::Get(IDS_SYSOPT_4010), NULL, MB_OK|MB_ICONWARNING);
		return S_OK;
	}
	m_nState = STATE_RUNOPT_SCAN;
	m_dlg->SetItemCheck(IDC_CHECKBOX_SHOW_DISABLE,TRUE);
	m_dlg->SetItemCheck(IDC_CHECKBOX_AUTOOPT_ALL,TRUE);
	m_dlg->SetItemCheck(IDC_CHECKBOX_RUNOPT_HIDE_SYSTEM,TRUE);
	m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_BTM_HAVE,FALSE);
	m_dlg->SetItemVisible(IDC_DIV_AUTOOPT,FALSE);
	m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_BTM_CONTINUE,FALSE);
	m_dlg->SetItemVisible(IDC_BTN_AUTOOPT_BTM_CONTINUE,FALSE);
	m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP,FALSE);
	m_dlg->SetItemVisible(IDC_DIV_RUN_TOPTIP,TRUE);
	m_dlg->SetItemVisible(IDC_DIV_RUNOPT_TOP_RUN,TRUE);
	m_runList.SetLoadShow();
	m_pRunOptEng->EnumRunInfo(0,this);
	return S_OK;
}

void CBeikeSafeSysOptUIHandler::PostRestoreRunItem(DWORD nType,BOOL bEnable,LPCTSTR lpName)
{
}

BOOL CBeikeSafeSysOptUIHandler::OnDeleteItem( CKsafeRunInfo* pInfo )
{
	CString			strMsg;
	BOOL			bNoNotify = FALSE;
	strMsg.Format(BkString::Get(IDS_SYSOPT_4011), pInfo->strDisplay);

	if ( !bNoNotify )
	{
		if ( CBkSafeMsgBox2::ShowMultLine(strMsg,NULL,MB_YESNO|MB_ICONWARNING,&bNoNotify) != IDYES )
			return FALSE;
	}

	BOOL bRes = m_pRunOptEng->DeleteRun(pInfo);

	if (bRes)
	{
		m_runList.DeleteItemByKSInfo(pInfo);
		m_runList.InvalidateRect(NULL);
		RefreshTabNumber();

		if (m_runList.XBASE_GetItemCount() == 0)
		{
			m_runList.SetShowDlgState();
		}

		for (int i = 0;i < m_arrayItem.GetSize();i++)
		{
			if ((pInfo->strName == m_arrayItem[i]->strName) &&
				(pInfo->strExePath == m_arrayItem[i]->strExePath) &&
				(pInfo->nID == m_arrayItem[i]->nID))
			{
				m_arrayItem.RemoveAt(i);
				break;
			}
		}
		m_runList.DeleteItemByKSInfo(pInfo);
		m_runList.InvalidateRect(NULL);
		RefreshTabNumber();

	}
	else
	{
		CBkSafeMsgBox2::ShowMultLine(BkString::Get(IDS_SYSOPT_4012),NULL,MB_OK|MB_ICONWARNING);
	}
	
	if (m_runList.GetDefShowTypeCount(KSRUN_TYPE_TASK) == 0)
	{
		RefreshItemList();
	}
	return bRes;
}

BOOL CBeikeSafeSysOptUIHandler::OnClickLabel( DWORD nIndex, CKsafeRunInfo* pInfo )
{
	if (nIndex==CLICK_INDEX_ENABLE)
	{
		if (!pInfo->bFileIsExist)
			return OnDeleteItem(pInfo);
		else
			return OnClickButton(pInfo);
	}
	else if (nIndex==CLICK_INDEX_DELETE)
		return OnDeleteItem(pInfo);
	else if (nIndex==CLICK_INDEX_OPEN_FOLDER)
	{
		if (pInfo->bFileIsExist)
		{
			CString	strFilePath(pInfo->strExePath);
			CString	strParam = _T("/select,\"") + strFilePath + _T("\"");
			::ShellExecute( m_runList.m_hWnd, _T("open"), _T("explorer.exe"), 
				strParam, NULL, SW_SHOW);
		}
		else
		{
			CBkSafeMsgBox2::ShowMultLine(BkString::Get(IDS_SYSOPT_4063),NULL,MB_OK|MB_ICONWARNING); 
		}
	}
	return FALSE;
}

void CBeikeSafeSysOptUIHandler::ClickCheck()
{
	m_dlg->EnableItem(IDC_BTN_RUN_AUTOOPT_DO, m_list.GetCheckedCount() > 0);
	m_dlg->EnableItem(IDC_LNK_ADDTO_IGNORED,m_list.GetCheckedCount() > 0);
	m_dlg->EnableItem(IDC_LNK_ADDTO_IGNORED2,m_list.GetCheckedCount() > 0);
	m_dlg->SetItemCheck(IDC_CHECKBOX_AUTOOPT_ALL,m_list.IsCheckAll());
	m_dlg->SetItemCheck(IDC_CHECKBOX_AUTOOPT_ALL2,m_list.IsCheckAll());
}

void CBeikeSafeSysOptUIHandler::InitAutoOptList()
{
	CString strTopTip;
	int nRunItemCount;
	if (m_pRunOptEng)
	{
		LoadIgnoredID();
		GetIgnoredItem();
		m_list.DeleteAllItems();
		if (!m_list.InitArray(m_arrayOnekeyOptItem))
		{
			CString strShowIgnored;
			strShowIgnored.Format(BkString::Get(IDS_SYSOPT_4070),m_nIgnoredCount);
			m_dlg->SetItemText(IDC_LNK_VIEW_IGNORED,strShowIgnored);

			m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP,FALSE);
			m_dlg->SetItemVisible(IDC_LST_RUN_AUTOOPT,FALSE);
			m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_NO_OPTITEM,TRUE);
			m_dlg->SetItemVisible(IDC_LNK_AUTOOPT_CANCELALL,FALSE);
			m_dlg->SetItemVisible(IDC_DIV_RUN_TOPTIP,FALSE);
			m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_BTM_HAVE,FALSE);
			m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_BTM_CONTINUE,TRUE);

			m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP_INIT,FALSE);
			m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP_DOING,FALSE);
			m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP_DONE_PART,FALSE);
			m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP_DONE,FALSE);
			m_dlg->SetItemVisible(IDC_CHECKBOX_AUTOOPT_ALL,FALSE);
			m_dlg->SetItemVisible(IDC_CHECKBOX_AUTOOPT_ALL2,FALSE);
			m_dlg->SetItemVisible(IDC_BTN_AUTOOPT_BTM_CONTINUE,FALSE);
			m_dlg->SetItemVisible(IDC_LNK_ADDTO_IGNORED2,FALSE);
		}
		else
		{
			CString strIgnored;
			strIgnored.Format(BkString::Get(IDS_SYSOPT_4073),m_nIgnoredCount);
			m_dlg->SetItemText(IDC_LNK_SHOW_IGNORED,strIgnored);

			m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP,TRUE);
			m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP_INIT,TRUE);
			m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP_DOING,FALSE);
			m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP_DONE_PART,FALSE);
			m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP_DONE,FALSE);
			
			m_dlg->SetItemVisible(IDC_CHECKBOX_AUTOOPT_ALL,TRUE);
			m_dlg->SetItemVisible(IDC_CHECKBOX_AUTOOPT_ALL2,TRUE);
			m_dlg->SetItemCheck(IDC_CHECKBOX_AUTOOPT_ALL,TRUE);
			m_dlg->EnableItem(IDC_CHECKBOX_AUTOOPT_ALL,TRUE);
			m_dlg->EnableItem(IDC_CHECKBOX_AUTOOPT_ALL2,TRUE);
			m_dlg->SetItemVisible(IDC_BTN_RUN_AUTOOPT_DO,TRUE);
			m_dlg->EnableItem(IDC_BTN_RUN_AUTOOPT_DO,TRUE);
			m_dlg->SetItemVisible(IDC_LST_RUN_AUTOOPT,TRUE);
			m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_NO_OPTITEM,FALSE);
			m_dlg->SetItemVisible(IDC_LNK_AUTOOPT_CANCELALL,TRUE);
			m_dlg->SetItemVisible(IDC_DIV_RUN_TOPTIP,TRUE);
			m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_BTM_HAVE,TRUE);
			m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_BTM_CONTINUE,FALSE);
			m_dlg->SetItemVisible(IDC_LNK_ADDTO_IGNORED2,TRUE);
			m_dlg->SetItemVisible(IDC_LNK_ADDTO_IGNORED,TRUE);
			m_dlg->EnableItem(IDC_LNK_ADDTO_IGNORED,TRUE);
			m_dlg->EnableItem(IDC_LNK_ADDTO_IGNORED2,TRUE);
			nRunItemCount = m_list.GetTotalCount();
			strTopTip.Format(BkString::Get(IDS_SYSOPT_4055),nRunItemCount);
			
			if (nRunItemCount < 10)
				m_dlg->SetItemAttribute(IDC_TXT_AUTOOPT_AFTER_TOTAL_NUM,"pos","185,2");
 			if ((nRunItemCount > 10) && (nRunItemCount < 100))
 				m_dlg->SetItemAttribute(IDC_TXT_AUTOOPT_AFTER_TOTAL_NUM,"pos","190,2");
			else if (nRunItemCount > 100)
				m_dlg->SetItemAttribute(IDC_TXT_AUTOOPT_AFTER_TOTAL_NUM,"pos","195,2");

			m_dlg->SetItemText(IDC_TXT_AUTOOPT_TOTAL_NUM,strTopTip);
			m_dlg->SetItemVisible(IDC_LNK_AUTOOPT_CANCELALL,FALSE);
		}
	}
	m_bRefreshList = FALSE;
}

void CBeikeSafeSysOptUIHandler::OnTimer(UINT_PTR nIDEvent)
{
	ShowOptDetail();
}

void CBeikeSafeSysOptUIHandler::ShowOptDetail()
{
	static int nPos = 0;
	++nPos;
	m_dlg->SetItemIntAttribute(IDC_IMG_AUTOOPT_TOP_DOING, "sub", (nPos)%12);
}

void CBeikeSafeSysOptUIHandler::OnBkBtnRefresh()
{
	m_bReportOnekeyOpt = FALSE;
	m_bSkipToOnekeyOpt = TRUE;
	m_dlg->SetTabCurSel(IDC_TAB_RUNOPT_TYPE,1);
	RefreshItemList();
}

void CBeikeSafeSysOptUIHandler::OnBkBtnContinueOpt()
{
	if (0 == m_list.GetCheckedCount())
	{
		if (m_list.GetTotalCount() == m_list.GetFailedNumber() + m_list.GetSuccessNumber())
		{
			OnBkBtnRefresh();
		}
		else
		{
			CBkSafeMsgBox2::ShowMultLine(BkString::Get(IDS_SYSOPT_4056), NULL, MB_OK|MB_ICONWARNING);
		}
	}
	else
	{
		OnBkBtnDoOpt();
	}
}

void CBeikeSafeSysOptUIHandler::OnBkBtnDoOpt()
{
	if (m_pRunOptEng==NULL)
		return;
	// 释放之前的一键优化线程
	if ( m_hOptimizeThread != NULL )
	{
		::CloseHandle(m_hOptimizeThread);
		m_hOptimizeThread = NULL;
	}
	m_bOpting = TRUE;
	m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP_INIT,FALSE);
	m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP_DONE,FALSE);
	m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP_DONE_PART,FALSE);
	m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP_DOING,TRUE);
	m_dlg->EnableItem(IDC_BTN_RUN_AUTOOPT_DO,FALSE);
	m_dlg->EnableItem(IDC_BTN_AUTOOPT_BTM_CONTINUE,FALSE);
	m_hOptimizeThread = ::CreateThread(NULL, 0, &CBeikeSafeSysOptUIHandler::OneKeyOptProc, this, 0, NULL);
}

DWORD WINAPI CBeikeSafeSysOptUIHandler::OneKeyOptProc(LPVOID lpParam)
{
	CBeikeSafeSysOptUIHandler* pThis = (CBeikeSafeSysOptUIHandler*)lpParam;
	BOOL	bFailed = FALSE;
	CString strTopTip;
	int		nItemCount = pThis->m_list.GetItemCount();
	pThis->m_dlg->SetTimer(TIMER_ID_RUNOPT_AUTOOPT_TOPTIP,50,NULL);
	for ( int i=0; i < nItemCount; i++)
	{
		_ARUN_ITEM_DATA*	px = pThis->m_list.GetItemParamData(i);
		
		if (px && px->nType==ARUN_TYPE_ITEM && px->nStatus==ARUN_STATUS_NULL && px->bCheck )
		{
			px->bOptimized = TRUE;
			strTopTip.Format(BkString::Get(IDS_SYSOPT_4054),px->pInfo->strDisplay);
			pThis->m_dlg->SetItemText(IDC_TXT_AUTOOPT_TOP_DETAIL,strTopTip);
			CRect rcText;
			pThis->m_dlg->GetItemRect(IDC_TXT_AUTOOPT_TOP_DETAIL,rcText);
			pThis->m_dlg->InvalidateRect(rcText);
			if (pThis->m_pRunOptEng->ControlRun(!px->pInfo->bEnable, px->pInfo))
			{
				px->nStatus = ARUN_STATUS_SUCCESS;
				px->bCheck = FALSE;
				Sleep(450);
			}
			else
			{
				px->nStatus = ARUN_STATUS_FAILED;
				bFailed = TRUE;
			}
		}
		CRect	rcItem;
		pThis->m_list.GetItemRect(i,&rcItem,LVIR_BOUNDS);
		pThis->m_list.InvalidateRect(rcItem);
	}
	::PostMessage(pThis->m_dlg->m_hWnd,MSG_SYSOPT_ONEKEYOPT_END,NULL,NULL);
	return 0;
}

LRESULT CBeikeSafeSysOptUIHandler::OnOnekeyOptEnd(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int		nSuccessCount = 0;
	int		nFailedCount  = 0;
	int		nUnOptedCount = 0;
	int		nRunItemCount = m_list.GetTotalCount();
	CString strTopTip;

	m_dlg->SetItemVisible(IDC_LNK_AUTOOPT_CANCELALL,TRUE);
	nSuccessCount = m_list.GetSuccessNumber();
	nFailedCount  = m_list.GetFailedNumber();
	if (nSuccessCount == nRunItemCount)
	{
		m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP_DONE_PART,FALSE);
		m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP_DOING,FALSE);
		m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP_INIT,FALSE);
		m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP_DONE,TRUE);
		strTopTip.Format(L"%d",nRunItemCount);
		m_dlg->SetItemText(IDC_TXT_AUTOOPT_TOTAL_DONE_NUM,strTopTip);
		if (nRunItemCount < 10)
			m_dlg->SetItemAttribute(IDC_TXT_AUTOOPT_AFTER_TOTALDONE_NUM,"pos","158,2");
		else if ((nRunItemCount > 10) && (nRunItemCount < 100))
			m_dlg->SetItemAttribute(IDC_TXT_AUTOOPT_AFTER_TOTALDONE_NUM,"pos","164,2");
		else if (nRunItemCount > 100)
			m_dlg->SetItemAttribute(IDC_TXT_AUTOOPT_AFTER_TOTALDONE_NUM,"pos","170,2");
		
		m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_BTM_HAVE,FALSE);
		m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_BTM_CONTINUE,TRUE);
		m_dlg->EnableItem(IDC_CHECKBOX_AUTOOPT_ALL2,FALSE);
		m_dlg->SetItemVisible(IDC_BTN_AUTOOPT_BTM_CONTINUE,FALSE);
		m_dlg->SetItemVisible(IDC_LNK_AUTOOPT_BTM_REFRESH,TRUE);
		m_dlg->SetItemVisible(IDC_LNK_AUTOOPT_CANCELALL,TRUE);
		m_bOpting = FALSE;
	}
	else if (nSuccessCount < nRunItemCount)
	{
		if (nFailedCount + nSuccessCount == nRunItemCount)
		{
			m_dlg->EnableItem(IDC_CHECKBOX_AUTOOPT_ALL,FALSE);
			m_dlg->EnableItem(IDC_CHECKBOX_AUTOOPT_ALL2,FALSE);
			m_bOpting = FALSE;
		}
		m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_BTM_HAVE,FALSE);
		m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_BTM_CONTINUE,TRUE);
		m_dlg->SetItemVisible(IDC_LNK_AUTOOPT_BTM_REFRESH,FALSE);
		m_dlg->SetItemVisible(IDC_BTN_AUTOOPT_BTM_CONTINUE,TRUE);

		m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP_DONE_PART,TRUE);
		m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP_DOING,FALSE);
		m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP_INIT,FALSE);
		m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP_DONE,FALSE);
		strTopTip.Format(L"%d",nSuccessCount);
		m_dlg->SetItemText(IDC_TXT_AUTOOPT_OPTED_NUM,strTopTip);
		nUnOptedCount = nRunItemCount - nSuccessCount;
		strTopTip.Format(L"%d",nUnOptedCount);
		m_dlg->SetItemText(IDC_TXT_AUTOOPT_NOOPTED_NUM,strTopTip);

		if (nSuccessCount < 10)
		{
			m_dlg->SetItemAttribute(IDC_TXT_AUTOOPT_AFTER_OPTED_NUM,"pos","160,2");
			m_dlg->SetItemAttribute(IDC_TXT_AUTOOPT_NOOPTED_NUM,"pos","213,2");

			if (nUnOptedCount < 10)
				m_dlg->SetItemAttribute(IDC_TXT_AUTOOPT_AFTER_NOOPTED_NUM,"pos","228,2");
			else if ((nUnOptedCount > 10) && (nUnOptedCount < 100))
				m_dlg->SetItemAttribute(IDC_TXT_AUTOOPT_AFTER_NOOPTED_NUM,"pos","233,2");
			else if (nUnOptedCount > 100)
				m_dlg->SetItemAttribute(IDC_TXT_AUTOOPT_AFTER_NOOPTED_NUM,"pos","238,2");
		}
		else if ((nSuccessCount > 10) && (nSuccessCount < 100))
		{
			m_dlg->SetItemAttribute(IDC_TXT_AUTOOPT_AFTER_OPTED_NUM,"pos","165,2");
			m_dlg->SetItemAttribute(IDC_TXT_AUTOOPT_NOOPTED_NUM,"pos","218,2");

			if (nUnOptedCount < 10)
				m_dlg->SetItemAttribute(IDC_TXT_AUTOOPT_AFTER_NOOPTED_NUM,"pos","233,2");
			else if ((nUnOptedCount > 10) && (nUnOptedCount < 100))
				m_dlg->SetItemAttribute(IDC_TXT_AUTOOPT_AFTER_NOOPTED_NUM,"pos","238,2");
			else if (nUnOptedCount > 100)
				m_dlg->SetItemAttribute(IDC_TXT_AUTOOPT_AFTER_NOOPTED_NUM,"pos","243,2");
		}
		else if (nSuccessCount > 100)
		{
			m_dlg->SetItemAttribute(IDC_TXT_AUTOOPT_AFTER_OPTED_NUM,"pos","170,2");
			m_dlg->SetItemAttribute(IDC_TXT_AUTOOPT_NOOPTED_NUM,"pos","223,2");

			if (nUnOptedCount < 10)
				m_dlg->SetItemAttribute(IDC_TXT_AUTOOPT_AFTER_NOOPTED_NUM,"pos","238,2");
			else if ((nUnOptedCount > 10) && (nUnOptedCount < 100))
				m_dlg->SetItemAttribute(IDC_TXT_AUTOOPT_AFTER_NOOPTED_NUM,"pos","243,2");
			else if (nUnOptedCount > 100)
				m_dlg->SetItemAttribute(IDC_TXT_AUTOOPT_AFTER_NOOPTED_NUM,"pos","248,2");
		}
	}
	m_dlg->EnableItem(IDC_BTN_RUN_AUTOOPT_DO,TRUE);
	m_dlg->EnableItem(IDC_BTN_AUTOOPT_BTM_CONTINUE,TRUE);
	m_dlg->KillTimer(TIMER_ID_RUNOPT_AUTOOPT_TOPTIP);
	m_dlg->SetItemVisible(IDC_DIV_AUTOOPT_TOP_DOING,FALSE);
	ClickCheck();

	return 0;
}

void CBeikeSafeSysOptUIHandler::OnAutoOptCheckAll()
{
	BOOL bAutoOptAllChecked = m_dlg->GetItemCheck(IDC_CHECKBOX_AUTOOPT_ALL);
	m_list.CheckAll(bAutoOptAllChecked);
	ClickCheck();
}

void CBeikeSafeSysOptUIHandler::OnAutoOptCheckAll2()
{
	BOOL bAutoOptAllChecked = m_dlg->GetItemCheck(IDC_CHECKBOX_AUTOOPT_ALL2);
	m_list.CheckAll(bAutoOptAllChecked);
	ClickCheck();
}

void CBeikeSafeSysOptUIHandler::OnBkLnkViewRun()
{	
	m_dlg->SetTabCurSel(IDC_TAB_RUNOPT_TYPE,1);
}

void CBeikeSafeSysOptUIHandler::OnBkLnkViewServ()
{
	m_dlg->SetTabCurSel(IDC_TAB_RUNOPT_TYPE,2);
}

void CBeikeSafeSysOptUIHandler::OnBkLnkViewTask()
{
	m_dlg->SetTabCurSel(IDC_TAB_RUNOPT_TYPE,3);
}

void CBeikeSafeSysOptUIHandler::OnBkCheckHideSystem()
{
	BOOL bHideSystemChecked = m_dlg->GetItemCheck(IDC_CHECKBOX_RUNOPT_HIDE_SYSTEM);
	m_runList.SetShowSystem(!bHideSystemChecked);
	RefreshTabNumber();
}

BOOL CBeikeSafeSysOptUIHandler::Is3XXInstalled()
{
	BOOL bResult = FALSE;
	TCHAR szFilePath[MAX_PATH + 1];
	DWORD cbData;
	DWORD dwType;
	LRESULT lResult;
	DWORD dwAttr;
	cbData = MAX_PATH * sizeof (TCHAR);
	lResult = SHGetValue(HKEY_LOCAL_MACHINE, 
		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\360safe.exe"),
		_T("Path"), 
		&dwType, 
		szFilePath, 
		&cbData
		);
	if ( lResult == ERROR_SUCCESS && dwType == REG_SZ )
	{
		szFilePath[MAX_PATH] = 0;
		_tcsncat(szFilePath, _T("\\360safe.exe"), MAX_PATH);
		dwAttr = GetFileAttributes(szFilePath);
		if ( dwAttr == INVALID_FILE_ATTRIBUTES )
		{
			bResult = FALSE;
			goto Exit0;
		}
		else
		{
			bResult = dwAttr & FILE_ATTRIBUTE_DIRECTORY ? FALSE:TRUE;
			goto Exit0;
		}
	}
Exit0:
	return bResult;
}

void CBeikeSafeSysOptUIHandler::OnBkLnkShowIgnored()
{
	LoadIgnoredID();
	GetIgnoredItem();
	CBeikeSafeSysoptIgnoreDlg dlg(m_arrayIgnoredItem,m_dlg);
	dlg.DoModal();
}

void CBeikeSafeSysOptUIHandler::OnBkLnkAddToIgnored()
{
	CString strMsg;
	strMsg.Format(BkString::Get(IDS_SYSOPT_4065), m_list.GetCheckedCount());

	if ( CBkSafeMsgBox2::ShowMultLine(strMsg,NULL,MB_YESNO|MB_ICONWARNING) != IDYES )
		return;
	SaveIgnoreID();
	
	for(int i = m_list.GetItemCount(); i >0 ;i--)
	{
		if (m_list.GetCheckState2(i-1) == TRUE)
			m_list.DeleteItem(i-1);
	}

	if (m_list.GetRunCount() == 0)
		m_list.DeleteTitle(ARUN_TYPE_TITLE_RUN);
	if (m_list.GetServiceCount() == 0)
		m_list.DeleteTitle(ARUN_TYPE_TITLE_SERVE);
	if (m_list.GetTaskCount() == 0)
		m_list.DeleteTitle(ARUN_TYPE_TITLE_TASK);

	::PostMessage(m_dlg->m_hWnd,MSG_SYSOPT_REFRESH_ONEKEYOPT,0,0);
}

void CBeikeSafeSysOptUIHandler::SaveIgnoreID()
{
	int nIgnoredCount = m_list.GetCheckedCount();
	CIniFile ini_IgnoredList(m_strIgnoredIniPath);
	CString strGetValue;
	ini_IgnoredList.GetStrValue(SEC_IGNOREDLIST_MAIN,KEY_IGNOREDLIST_COMMENT,strGetValue.GetBuffer(MAX_PATH),MAX_PATH);
	strGetValue.ReleaseBuffer(MAX_PATH);
	CString strValue;
	for (int i = 0;i < nIgnoredCount;i++)
	{
		CString strId;
		BOOL bExisted = FALSE;
		for (int n = 0;n < m_arrayIgnoredID.GetSize();n++)
		{
			if (m_list.GetIgnoredId(i) == m_arrayIgnoredID[n])
				bExisted = TRUE;
		}
		if (!bExisted)
		{
			strId.Format(_T("%d|"),m_list.GetIgnoredId(i));
			strValue.Append(strId);
		}
	}
	strGetValue.Format(_T("%s%s"),CString(strGetValue),strValue);
	ini_IgnoredList.SetStrValue(SEC_IGNOREDLIST_MAIN,KEY_IGNOREDLIST_COMMENT,strGetValue);
}

void CBeikeSafeSysOptUIHandler::LoadIgnoredID()
{
	m_arrayIgnoredID.RemoveAll();
	CIniFile ini_IgnoredList(m_strIgnoredIniPath);
	CString strGetValue;
	ini_IgnoredList.GetStrValue(SEC_IGNOREDLIST_MAIN,KEY_IGNOREDLIST_COMMENT,strGetValue.GetBuffer(MAX_PATH),MAX_PATH);
	strGetValue.ReleaseBuffer(MAX_PATH);
	
	WCHAR *szValue;
	WCHAR szTemp[10];
	szValue = strGetValue.GetBuffer();
	strGetValue.ReleaseBuffer();
	WCHAR* p = wcstok(szValue,L"|");
	while(p)
	{
		wcscpy_s(szTemp,p);
		m_arrayIgnoredID.Add(_wtoi(szTemp));
		p = wcstok(NULL,L"|");
	}
}

void CBeikeSafeSysOptUIHandler::GetIgnoredItem()
{
	m_arrayIgnoredItem.RemoveAll();
	m_arrayOnekeyOptItem.RemoveAll();
	m_nIgnoredCount = 0;
	for (int i = 0; i < m_arrayItem.GetSize();i++)
	{
		BOOL bFind = FALSE;
		for (int n = 0; n < m_arrayIgnoredID.GetSize();n++)
		{
			if (m_arrayItem[i]->nID == m_arrayIgnoredID[n])
			{
				bFind = TRUE;
				break;
			}
		}
		if (bFind == TRUE)
		{
			m_nIgnoredCount++;
			CKsafeRunInfo*	pNewInfo = m_arrayItem[i];
			m_arrayIgnoredItem.Add(pNewInfo);
		}
		else
		{
			CKsafeRunInfo*	pNewInfo = m_arrayItem[i];
			m_arrayOnekeyOptItem.Add(pNewInfo);
		}
	}
}

LRESULT CBeikeSafeSysOptUIHandler::OnRefreshOnekeyOpt(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = S_OK;
	m_bOpting = FALSE;
	OnBkBtnRefresh();
	LoadIgnoredID();
	CString strIgnored;
	strIgnored.Format(TEXT("%s（%d）"),TEXT("已忽略"),m_nIgnoredCount);
	m_dlg->SetItemText(IDC_LNK_SHOW_IGNORED,strIgnored);

	if (m_list.GetItemCount() == 0)
	{
		m_dlg->SetItemVisible(IDC_LNK_AUTOOPT_BTM_REFRESH,TRUE);
	}

	return hr;
}