
#pragma once

#include <runoptimize/interface.h>
#include <wtl/atlctrls.h>

#define __USE_BASE_LIST_VIEW2	1

#define ARUN_TYPE_TITLE_RUN		1
#define ARUN_TYPE_TITLE_SERVE	2
#define ARUN_TYPE_TITLE_TASK	3
#define ARUN_TYPE_ITEM			4

#define ARUN_STATUS_NULL		0
#define ARUN_STATUS_SUCCESS		1
#define ARUN_STATUS_FAILED		2

#define XX_ITEM_HEIGHT			26	//30
#define COL_STR_DEFAULT			RGB(0,0,0)
#define COL_CAPTION				RGB(243,243,243)
#define COL_ITEM_DEFAULT		RGB(255,255,255)
#define COL_ITEM_SELECT			RGB(185,219,255)
#define COL_ITEM_LINE			RGB(234,233,225)
#define COL_ITEM_SUCCESS		RGB(0xee,0xee,0xee)
#define COL_ITEM_FAIL			RGB(0xee,0xee,0xee)

#define COL_ITEM_TEXT_SUCCESS	RGB(0,128,0)
#define COL_ITEM_TEXT_FAILED	RGB(255,0,0)

class IAutoOptLstCBack
{
public:
	virtual void ClickCheck() = 0;
};

struct _ARUN_ITEM_DATA
{
public:
	_ARUN_ITEM_DATA()
	{
		nType	= 0;
		nStatus	= ARUN_STATUS_NULL;
		pInfo	= NULL;
		bCheck	= TRUE;
		bOptimized = FALSE;
		rcDesc	= CRect(0,0,0,0);
		rcDisplay = CRect(0,0,0,0);
	}
	DWORD			nType;
	DWORD			nStatus;
	BOOL			bCheck;
	BOOL			bOptimized;
	CKsafeRunInfo*	pInfo;
	CRect			rcDesc;
	CRect			rcDisplay;
};

class CAutoRunOptList : 
	public CWindowImpl<CAutoRunOptList, CListViewCtrl>
	,public COwnerDraw<CAutoRunOptList>
{
	typedef CWindowImpl<CAutoRunOptList, CListViewCtrl> _super;
public:
	CAutoRunOptList(){
		m_rcCheck = CRect(9,(XX_ITEM_HEIGHT-13)/2,22,(XX_ITEM_HEIGHT+13)/2 );
		m_pCB = NULL;
		m_pHoverTip = 0;
	}
	virtual ~CAutoRunOptList(){}

public:
	BEGIN_MSG_MAP(CAutoRunOptList)   
		MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST, WM_MOUSELAST, OnToolTipEvent)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		CHAIN_MSG_MAP_ALT(COwnerDraw<CAutoRunOptList>,1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()  

	void CreateToolTip()
	{
		m_wndToolTip.Create(m_hWnd);

		CToolInfo ti(0, m_hWnd);
		m_wndToolTip.AddTool(ti);
		m_wndToolTip.Activate(TRUE);
		m_wndToolTip.SetMaxTipWidth(500);
		m_wndToolTip.SetDelayTime(TTDT_AUTOPOP,5*1000);
		m_wndToolTip.SetDelayTime(TTDT_INITIAL,500);
	}

	void SetCallback(IAutoOptLstCBack* pCB)
	{
		m_pCB = pCB;
	}

	int GetCheckedCount()
	{
		m_arrayCheckedId.RemoveAll();
		int nCount = 0;
		for ( int i=0; i < CListViewCtrl::GetItemCount(); i++)
		{
			_ARUN_ITEM_DATA*	pdata = GetItemParamData(i);

			if (pdata && pdata->nType==ARUN_TYPE_ITEM)
			{
				if (pdata->bCheck && !pdata->bOptimized)
				{
					nCount++;
					m_arrayCheckedId.Add(pdata->pInfo->nID);
				}
			}
		}
		return nCount;
	}

	BOOL GetCheckState2(int nItem)
	{
		BOOL bRet = FALSE;
		_ARUN_ITEM_DATA*	pdata = GetItemParamData(nItem);
		
		if (pdata && pdata->nType==ARUN_TYPE_ITEM)
		{
			if (pdata->bCheck && !pdata->bOptimized)
				bRet = TRUE;
		}

		return bRet;
	}

	int GetSuccessNumber()
	{
		int nCount = 0;
		for ( int i=0; i < CListViewCtrl::GetItemCount(); i++)
		{
			_ARUN_ITEM_DATA*	pdata = GetItemParamData(i);

			if (pdata && pdata->nType==ARUN_TYPE_ITEM)
			{
				if (pdata->bOptimized && pdata->nStatus == ARUN_STATUS_SUCCESS)
					nCount++;
			}
		}
		return nCount;
	}

	int GetFailedNumber()
	{
		int nCount = 0;
		for ( int i=0; i < CListViewCtrl::GetItemCount(); i++)
		{
			_ARUN_ITEM_DATA*	pdata = GetItemParamData(i);

			if (pdata && pdata->nType==ARUN_TYPE_ITEM)
			{
				if (pdata->bOptimized && pdata->nStatus == ARUN_STATUS_FAILED)
					nCount++;
			}
		}
		return nCount;
	}

	void OnLButtonDown(UINT uMsg, CPoint ptx)
	{
		BOOL	bHandle = FALSE;
		LVHITTESTINFO hti;
		hti.pt = ptx;
		SubItemHitTest( &hti );

		CRect	rcItem;
		CRect	rcButtons = m_rcCheck;
		GetItemRect(hti.iItem,&rcItem,LVIR_BOUNDS);

		rcButtons.OffsetRect(rcItem.left,rcItem.top);

		if ( hti.iSubItem==0 && rcButtons.PtInRect(ptx))
		{
			_ARUN_ITEM_DATA* pdata = GetItemParamData(hti.iItem);
			if (pdata && pdata->nStatus==ARUN_STATUS_NULL && pdata->nType==ARUN_TYPE_ITEM && !pdata->bOptimized)
			{
				pdata->bCheck = !pdata->bCheck;
				bHandle = TRUE;
				CListViewCtrl::RedrawItems(hti.iItem,hti.iItem);
				if (m_pCB)
					m_pCB->ClickCheck();
			}
		}

		SetMsgHandled(bHandle);
	}

	BOOL IsCheckAll()
	{
		BOOL bCheckAll = FALSE;
		int nChecked = 0;
		int nTitle = 0;
		for ( int i=0; i < CListViewCtrl::GetItemCount(); i++)
		{
			_ARUN_ITEM_DATA*	pdata = GetItemParamData(i);

			if (pdata && pdata->nType==ARUN_TYPE_ITEM)
			{
				if (pdata->bCheck || pdata->bOptimized)
					nChecked++;
			}
			else
				nTitle++;
		}
		return ((nChecked + nTitle) == CListViewCtrl::GetItemCount());
	}

	void AppendTitle(DWORD nType)
	{
		_ARUN_ITEM_DATA*	px = new _ARUN_ITEM_DATA;
		px->nType = nType;
		InsertItemX(px);
	}

	void InsertItemX(_ARUN_ITEM_DATA* param)
	{
		int i = CListViewCtrl::InsertItem( CListViewCtrl::GetItemCount(), _T(" "));
		CListViewCtrl::SetItemData(i,(DWORD_PTR)param);
	}

	DWORD GetTitleType(DWORD nRunType)
	{
		return nRunType;
	}

	void _InitArrayType(CSimpleArray<CKsafeRunInfo*>& arrayx,DWORD nType, DWORD & nCount)
	{
		nCount = 0;
		BOOL	bAppendTitle = FALSE;
		for ( int i=0; i < arrayx.GetSize(); i++)
		{
			CKsafeRunInfo*	pInfo = arrayx[i];
			if (pInfo->nType==nType && pInfo->bEnable && pInfo->IsAdviseStop() )
			{
				if (!bAppendTitle)
				{
					bAppendTitle=TRUE;
					AppendTitle(GetTitleType(nType));		
				}
				AppendItem(pInfo);
				nCount++;
			}
		}
	}

	BOOL InitArray(CSimpleArray<CKsafeRunInfo*>& arrayx)
	{
		_InitArrayType(arrayx,KSRUN_TYPE_STARTUP,m_nCountRun);
		_InitArrayType(arrayx,KSRUN_TYPE_SERVICE,m_nCountServ);
		_InitArrayType(arrayx,KSRUN_TYPE_TASK,m_nCountTask);
		return (GetItemCount()>0);
	}

	void AppendItem(CKsafeRunInfo* pInfo)
	{
		_ARUN_ITEM_DATA*	px = new _ARUN_ITEM_DATA;
		px->bCheck	= TRUE;
		px->nStatus	= ARUN_STATUS_NULL;
		px->nType	= ARUN_TYPE_ITEM;
		px->pInfo	= pInfo;

		InsertItemX(px);
	}

	_ARUN_ITEM_DATA* GetItemParamData(int index)
	{
		return (_ARUN_ITEM_DATA*)CListViewCtrl::GetItemData(index);
	}

	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		_ARUN_ITEM_DATA*	pData = GetItemParamData(lpDrawItemStruct->itemID);
		CDCHandle			dcx;
		CRect				rcItem = lpDrawItemStruct->rcItem;
		DWORD				nOldClr;
		BOOL				bSelect = FALSE ;
		DWORD				nTformat= DT_VCENTER|DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS;
		
		if ((lpDrawItemStruct->itemAction | ODA_SELECT) && (lpDrawItemStruct->itemState & ODS_SELECTED))
			bSelect = TRUE;

		dcx.Attach(lpDrawItemStruct->hDC);
		HFONT fntTmp = dcx.SelectFont(BkFontPool::GetFont(BKF_DEFAULTFONT));
		nOldClr = dcx.SetTextColor(COL_STR_DEFAULT);
		if (pData)
		{
			if (pData->nType!=ARUN_TYPE_ITEM)
			{
				if (TRUE)
				{
					CDC dcTmp;
					dcTmp.CreateCompatibleDC(dcx);
					HBITMAP hBmpCheck	= BkBmpPool::GetBitmap(IDB_RUNOPT_LIST_HEADER);
					HBITMAP hBmpOld		= dcTmp.SelectBitmap(hBmpCheck);
					dcx.StretchBlt( rcItem.left, rcItem.top, rcItem.Width(),rcItem.Height(), dcTmp, 0,0,1,24,SRCCOPY);
					dcTmp.SelectBitmap(hBmpOld);
				}
				_DrawTitle(dcx,&lpDrawItemStruct->rcItem,pData->nType);
			}
			else if (pData->pInfo)
			{
				CKsafeRunInfo*	pInfo = pData->pInfo;

				if ( bSelect && !pData->bOptimized)
					dcx.FillSolidRect(&rcItem,COL_ITEM_SELECT);
				else
					dcx.FillSolidRect(&rcItem,COL_ITEM_DEFAULT);

				CRect	rcFirst;
				CRect	rcSecond;
				CRect	rcThird;
				CRect	rcItem = lpDrawItemStruct->rcItem;
				GetSubItemRect(lpDrawItemStruct->itemID,0,LVIR_LABEL,&rcFirst);
				GetSubItemRect(lpDrawItemStruct->itemID,1,LVIR_LABEL,&rcSecond);
				GetSubItemRect(lpDrawItemStruct->itemID,2,LVIR_LABEL,&rcThird);
				
				if (TRUE)
				{
					if (TRUE)
					{
						CRect			rcCheckbox = m_rcCheck;
						CDC				dcTmp;
						dcTmp.CreateCompatibleDC(dcx);

						HBITMAP hBmpCheck	= BkBmpPool::GetBitmap(IDB_BITMAP_LISTCTRL_CHECK);
						HBITMAP hBmpOld		= dcTmp.SelectBitmap(hBmpCheck);

						rcCheckbox.OffsetRect(rcFirst.left,rcFirst.top);
						if (pData->bOptimized)
							dcx.BitBlt( rcCheckbox.left, 
										rcCheckbox.top, 
										rcCheckbox.Width(),
										rcCheckbox.Height(), 
										dcTmp, 
										(pData->nStatus == ARUN_STATUS_SUCCESS)?26:39,
										0,SRCCOPY);
						else
							dcx.BitBlt( rcCheckbox.left, 
										rcCheckbox.top, 
										rcCheckbox.Width(),
										rcCheckbox.Height(), 
										dcTmp, 
										pData->bCheck?0:13,
										0,SRCCOPY);
						dcTmp.SelectBitmap(hBmpOld);
					}

					if (TRUE)
					{
						CRect	rcText = rcFirst;
						rcText.left += m_rcCheck.right+10;
						CRect rcProbe;
						dcx.DrawText(pInfo->strDisplay,-1,&rcProbe,DT_CALCRECT|DT_VCENTER|DT_SINGLELINE|DT_LEFT);
						dcx.DrawText(pInfo->strDisplay, -1, &rcText, nTformat);
						rcProbe.bottom = rcProbe.top + 30;
						calcLeftRECT(rcText,rcProbe,pData->rcDisplay);
					}
				}

				if (TRUE)
				{
					CRect rcText = rcSecond;
					CRect rcProbe;
					rcText.left += 10;
					dcx.DrawText(pInfo->strDesc,-1,&rcProbe,DT_CALCRECT|DT_VCENTER|DT_SINGLELINE|DT_LEFT);
					dcx.DrawText(pInfo->strDesc, -1, &rcText, nTformat);
					rcProbe.bottom = rcProbe.top+30;
					calcLeftRECT(rcText,rcProbe,pData->rcDesc);
				}

				if (TRUE && (pData->bOptimized || pData->bCheck))
				{
					if (pData->nStatus==ARUN_STATUS_SUCCESS)
					{
						CRect	rcText = rcThird;
						rcText.left += 10;
						CString	strTip;

						if (pInfo->nType==KSRUN_TYPE_SERVICE)
							strTip = BkString::Get(IDS_SYSOPT_4019);
						else
							strTip = BkString::Get(IDS_SYSOPT_4020);

						dcx.SetTextColor(COL_ITEM_TEXT_SUCCESS);
						dcx.DrawText(strTip, -1, &rcText, nTformat);
					}
					else if (pData->nStatus==ARUN_STATUS_FAILED)
					{
						CRect	rcText = rcThird;
						rcText.left += 10;
						CString	strTip;

						strTip = BkString::Get(IDS_SYSOPT_4021);
						dcx.SetTextColor(COL_ITEM_TEXT_FAILED);
						dcx.DrawText(strTip, -1, &rcText, nTformat);
					}
					else if (pData->bCheck)
					{
						CRect	rcText = rcThird;
						rcText.left += 10;
						CString	strTip;
						if (pInfo->nType==KSRUN_TYPE_SERVICE)
							strTip = BkString::Get(IDS_SYSOPT_4022);
						else
							strTip = BkString::Get(IDS_SYSOPT_4023);
						dcx.DrawText(strTip, -1, &rcText, nTformat);
					}
				}
			}
		}
		dcx.SetTextColor(nOldClr);
		dcx.SelectFont(fntTmp);
		dcx.Detach();
	}

	void calcLeftRECT(CRect rcArea, CRect rcProbe, CRect& rcOut)
	{
		rcOut.left		= rcArea.left;
		rcOut.right		= rcOut.left + rcProbe.Width();
		rcOut.top		= rcArea.top;
		rcOut.bottom	= rcOut.top + rcProbe.Height();
	}

	void MeasureItem(LPMEASUREITEMSTRUCT lParam)
	{
		lParam->itemHeight = XX_ITEM_HEIGHT;
	}

	BOOL DeleteItem(int nItem)
	{
		return _super::DeleteItem(nItem);
	}

	void DeleteItem(LPDELETEITEMSTRUCT)
	{
		return;
	}

	void CheckAll(BOOL bCheck)
	{
		for ( int i=0; i < CListViewCtrl::GetItemCount(); i++)
		{
			_ARUN_ITEM_DATA*	pdata = GetItemParamData(i);

			if (pdata && pdata->nType==ARUN_TYPE_ITEM && !pdata->bOptimized)
			{
				pdata->bCheck = bCheck;
			}
		}
		CListViewCtrl::InvalidateRect(NULL);
	}

	DWORD GetTotalCount()
	{
		return m_nCountRun + m_nCountServ + m_nCountTask;
	}

	int GetRunCount()
	{
		int nCount = 0;
		for ( int i=0; i < CListViewCtrl::GetItemCount(); i++)
		{
			_ARUN_ITEM_DATA*	pdata = GetItemParamData(i);

			if (pdata && pdata->nType==ARUN_TYPE_ITEM && pdata->pInfo->nType == KSRUN_TYPE_STARTUP)
				nCount++;
		}
		return nCount;
	}

	int GetServiceCount()
	{
		int nCount = 0;
		for ( int i=0; i < CListViewCtrl::GetItemCount(); i++)
		{
			_ARUN_ITEM_DATA*	pdata = GetItemParamData(i);

			if (pdata && pdata->nType==ARUN_TYPE_ITEM && pdata->pInfo->nType == KSRUN_TYPE_SERVICE)
				nCount++;
		}
		return nCount;
	}

	int GetTaskCount()
	{
		int nCount = 0;
		for ( int i=0; i < CListViewCtrl::GetItemCount(); i++)
		{
			_ARUN_ITEM_DATA*	pdata = GetItemParamData(i);

			if (pdata && pdata->nType==ARUN_TYPE_ITEM && pdata->pInfo->nType == KSRUN_TYPE_TASK)
				nCount++;
		}
		return nCount;
	}

	void DeleteTitle(int nType)
	{
		for ( int i=0; i < CListViewCtrl::GetItemCount(); i++)
		{
			_ARUN_ITEM_DATA*	pdata = GetItemParamData(i);

			if (pdata && pdata->nType == nType)
			{
				DeleteItem(i);
				return;
			}
		}
	}

	void OnMouseMove(UINT uMsg, CPoint ptx)
	{
		BOOL	bShowTip = FALSE;
		int	iItem = XBASE_HitTest(ptx);

		CKsafeRunInfo*	kinfo = GetDataInfo(iItem);
		_ARUN_ITEM_DATA* pItemData = (_ARUN_ITEM_DATA*)XBASE_GetItemData(iItem);

		if (kinfo && pItemData)
		{
			int iIndex = LabelHitTest(ptx);		

			if (iIndex==CLICK_INDEX_DESC || iIndex==CLICK_INDEX_CAPTION)
			{
				if ( ((DWORD)kinfo!=m_pHoverTip) )
				{
					CString	strInfo;

					if (iIndex==CLICK_INDEX_DESC)
						strInfo.Append( kinfo->strDesc.IsEmpty()?BkString::Get(IDS_SYSOPT_4033):kinfo->strDesc );
					else if (iIndex==CLICK_INDEX_CAPTION )
						strInfo.Append( kinfo->strDisplay );

					m_wndToolTip.SetMaxTipWidth(500);
					m_wndToolTip.UpdateTipText((LPCTSTR)strInfo,m_hWnd);
					m_pHoverTip = (DWORD)kinfo;
				}
				bShowTip = TRUE;
			}
		}
		if (!bShowTip)
		{
			if (m_pHoverTip!=0)
			{
				m_pHoverTip = 0;
				m_wndToolTip.UpdateTipText((LPCTSTR)NULL,m_hWnd);
				m_wndToolTip.Pop();
			}
		}
	}

	LRESULT OnToolTipEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if(m_wndToolTip.IsWindow())
		{
			MSG msg = { m_hWnd, uMsg, wParam, lParam };

			m_wndToolTip.RelayEvent(&msg);
		}

		SetMsgHandled(FALSE);

		return 0;
	}

	int LabelHitTest(CPoint ptx)
	{
		int		iItem = XBASE_HitTest(ptx);

		CRect	rcItem;
		_ARUN_ITEM_DATA* pdata = (_ARUN_ITEM_DATA*)XBASE_GetItemData(iItem);

		if (pdata==NULL)
			return CLICK_INDEX_INVALID;
		
		if (pdata->rcDesc.PtInRect(ptx))
			return CLICK_INDEX_DESC;
		else if (pdata->rcDisplay.PtInRect(ptx))
			return CLICK_INDEX_CAPTION;
		else
			return CLICK_INDEX_NULL;
	}

	DWORD_PTR XBASE_GetItemData(int nItem)
	{
#if __USE_BASE_LIST_VIEW2
		return CListViewCtrl::GetItemData(nItem);
#else
		DWORD_PTR pvoid = CListBox::GetItemData(nItem);

		if ((DWORD)pvoid==-1)
			return NULL;
		return pvoid;
#endif
	}

	BOOL XBASE_GetItemRect(int i, LPRECT lprc)
	{
#if __USE_BASE_LIST_VIEW2
		return CListViewCtrl::GetItemRect(i,lprc,LVIR_BOUNDS);
#else
		return CListBox::GetItemRect(i,lprc);
#endif
	}

	int XBASE_HitTest(CPoint ptx)
	{
#if __USE_BASE_LIST_VIEW2
		LVHITTESTINFO hti;
		hti.pt = ptx;
		SubItemHitTest( &hti );
		return hti.iItem;
#else
		BOOL	bOut = FALSE;
		int i = (int)CListBox::ItemFromPoint(ptx,bOut);

		if (!bOut)
			return i;
		else
			return -1;
#endif
	}

	CKsafeRunInfo* GetDataInfo(int nItem)
	{
		_ARUN_ITEM_DATA*	pParam = (_ARUN_ITEM_DATA*)XBASE_GetItemData(nItem);

		if (pParam!=NULL && (DWORD)pParam != -1 )
			return pParam->pInfo;
		else
			return NULL;
	}

	int GetIgnoredId(int nItem)
	{
		return m_arrayCheckedId[nItem];
	}

	int GetIgnoredCount()
	{
		return m_arrayIgnoredId.GetSize();
	}
protected:
	void _DrawTitle(CDCHandle& dcx, LPRECT lpRc, DWORD nType)
	{
		HFONT fntTmp = dcx.SelectFont(BkFontPool::GetFont(FALSE,FALSE,FALSE));
		dcx.SetTextColor(COL_DEFAULT_LINK);

		CRect	rcItem = *lpRc;
		rcItem.left += 20;

		//dcx.FillSolidRect(lpRc,COL_CAPTION);

		DWORD	nFormat = DT_VCENTER|DT_LEFT|DT_SINGLELINE;
		CString	strShow;
		if (nType==ARUN_TYPE_TITLE_RUN)
		{
			strShow.Format(BkString::Get(IDS_SYSOPT_4024), m_nCountRun);
		}
		else if (nType==ARUN_TYPE_TITLE_SERVE)
		{
			strShow.Format(BkString::Get(IDS_SYSOPT_4025), m_nCountServ);
		}
		else if (nType==ARUN_TYPE_TITLE_TASK)
		{
			strShow.Format(BkString::Get(IDS_SYSOPT_4026), m_nCountTask);
		}

		dcx.DrawText(strShow,-1,&rcItem,nFormat);

		CPen	penx;
		HPEN	tmppen;
		penx.CreatePen(PS_SOLID,1,COL_ITEM_LINE);
		tmppen = dcx.SelectPen(penx);
		dcx.MoveTo(lpRc->left,rcItem.bottom-1);
		dcx.LineTo(rcItem.right,rcItem.bottom-1);
		dcx.MoveTo(lpRc->left,rcItem.top-1);
		dcx.LineTo(rcItem.right,rcItem.top-1);
		dcx.SelectPen(tmppen);

		dcx.SelectFont(fntTmp);
	}

protected:
	DWORD	m_nCountRun;
	DWORD	m_nCountServ;
	DWORD	m_nCountTask;
	CRect	m_rcCheck;
	IAutoOptLstCBack*	m_pCB;
	CToolTipCtrl		m_wndToolTip;
	DWORD				m_pHoverTip;
	CSimpleArray<int>	m_arrayCheckedId;
	CSimpleArray<int>	m_arrayIgnoredId;
};