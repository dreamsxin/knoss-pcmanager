#pragma once


#include "beikecolordef.h"

#define	XX_COL_DANGER_PLUG_NAME		RGB(255,0,0)
#define XX_COL_COMMON_PLUG_NAME		RGB(16,16,16)
#define XX_COL_COMMON_PLUG_DEF		RGB(0,0,0)

#define	XX_WIDTH_LEVEL_COLUMN		80
#define	XX_WIDTH_TYPE_COLUMN		140
#define XX_WIDTH_COMPANY_COLUMN		160
#define XX_WIDTH_SCROLL_BAR			20
#define XX_WIDTH_RIGHT_THREE_COLUMN	(XX_WIDTH_LEVEL_COLUMN+XX_WIDTH_TYPE_COLUMN+XX_WIDTH_COMPANY_COLUMN)
#define XX_LIST_ITEM_HEIGHT			25
#define XX_TIMER_ID_TIPS			1

#include "beikesafetipdlg.h"


#define PLUG_LIST_TYPE_ALL			0
#define PLUG_LIST_TYPE_DANGER		1
#define PLUG_LIST_TYPE_UNK			2
#define PLUG_LIST_TYPE_SAFE			3
#define CFGName L"kprivacy.cfg"

typedef enum _MONITOR_ITEM_TYPE
{
	enumMonitorItemTypeNONE = 0,
	enumMonitorItemTypeFile,
	enumMonitorItemTypeDir,
	enumMonitorItemTypeMyComputer,
	enumMonitorItemTypeMyCamera,
	enumMonitorItemTypeProcess
} MONITOR_ITEM_TYPE;

typedef	struct _MONITOR_ITEM
{
	DWORD		dwId;
	int			nType;
	bool		bCheck;
	CString		strName;
	CString		strTip;
	CString		strMonitorItem;
	bool		bDel;
	bool		bDisable;
	CRect		rcName;
	DWORD		nImg;
	bool		bNew;
} MONITOR_ITEM;


class IClickLinkCallback
{
public:
	virtual void	OnCheck(DWORD nId, bool bCheck) = 0;
	virtual void	OnDelete(DWORD nId) = 0;
};

class CMonitorItemList : 
	public CWindowImpl<CMonitorItemList, CListBox>, 
	public COwnerDraw<CMonitorItemList>
{
public:
	CMonitorItemList(void)
	{
		m_linkerCB	= NULL;
		m_rcCheck	= CRect(10,8,23,25);
		m_rcName	= CRect(52,8,200,25);
		
		m_rcViewDetail	= CRect(195,8,252,25);

		m_bNoPaintItem	= FALSE;

		m_fntName.Attach( BkFontPool::GetFont(TRUE, FALSE, FALSE, 0) );
		m_fntLink.Attach( BkFontPool::GetFont(FALSE,TRUE,FALSE,0) );
		m_fntDefault.Attach( BkFontPool::GetFont(BKF_DEFAULTFONT));

		m_fntDanger.Attach( BkFontPool::GetFont(BKF_BOLDFONT) );
		m_nPlugShowType = PLUG_LIST_TYPE_ALL;
	}

	~CMonitorItemList(void)
	{
		m_fntName.Detach();
		m_fntLink.Detach();
		m_fntDefault.Detach();
		m_fntDanger.Detach();
	}

protected:
	CRect	m_rcCheck;			// 选择框
	CRect	m_rcName;			// 名字
	CRect	m_rcType;			// 类型
	CRect	m_rcCompany;		// 出品公司
	CRect	m_rcFilePath;		// 文件路径

	CRect	m_rcViewDetail;		// 查看详情
	CRect	m_rcReport;			// 上报到XX安全中心
	CRect	m_rcRptNoKill;		// 报告清理不掉
	CRect	m_rcKiller;			// 专杀
	CRect	m_rcBehavior;		// 
	DWORD	m_nPlugShowType;
	CBrush	m_hBGBrush;

	CFont		m_fntName;		// 插件名字使用字体
	CFont		m_fntLink;		// 链接的字体
	CFont		m_fntDefault;	// 一般字体
	CFont		m_fntDanger;	// 危险显示的字体
	BOOL	m_bNoPaintItem;

	IClickLinkCallback*	m_linkerCB;

	CSimpleArray<MONITOR_ITEM>	m_arrData;
public:
	BEGIN_MSG_MAP(CMonitorItemList)   
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MSG_WM_TIMER(OnTimer)
		MSG_OCM_CTLCOLORLISTBOX(OnCtlColor)
		CHAIN_MSG_MAP_ALT(COwnerDraw<CMonitorItemList>,1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()   

	LRESULT	OnCtlColor(HDC hDc, HWND hWnd)
	{
		if (hWnd==m_hWnd)
		{
			CDCHandle	dchan;
			dchan.Attach(hDc);

			dchan.SetBkMode(TRANSPARENT);
			SetMsgHandled(TRUE);

			if (m_hBGBrush.m_hBrush==NULL)
				m_hBGBrush.CreateSolidBrush(COL_DEFAULT_WHITE_BG);

			dchan.Detach();
			return (LRESULT)m_hBGBrush.m_hBrush;
		}
		SetMsgHandled(FALSE);
		return NULL;
	}

	void SetClickLinkCallback( IClickLinkCallback* opCB )
	{
		m_linkerCB = opCB;
	}

	HRESULT	OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bMsgHandled )
	{
		POINT	pts;
		BOOL	bOutSide;
		CRect	rcItem;

		bMsgHandled = FALSE;
		pts.x		= GET_X_LPARAM(lParam);
		pts.y		= GET_Y_LPARAM(lParam);

		INT nIndex = CListBox::ItemFromPoint(pts,bOutSide);// CListBox::HitTest(pts,&uFlags);

		if ( !bOutSide && nIndex >= 0 && nIndex < 0xffff && GetItemRect(nIndex,&rcItem,LVIR_BOUNDS) )
		{
			MONITOR_ITEM*	pData = (MONITOR_ITEM*)GetItemDataX(nIndex);
			if ( pData )
			{
				pts.x -= rcItem.left;
				pts.y -= rcItem.top;

				if ( m_rcCheck.PtInRect(pts) ||
					 m_rcViewDetail.PtInRect(pts)
				 )
				{
					bMsgHandled = TRUE;
				}
			}
		}

		CBkSafeTip::Hide();
		return S_OK;		
	}

	int Add(MONITOR_ITEM& dwItemData)
	{
		dwItemData.dwId = m_arrData.GetSize();

		m_arrData.Add(dwItemData);
		CListBox::AddString(dwItemData.strName);
		return SetItemData(dwItemData.dwId, (DWORD_PTR)dwItemData.dwId);
	}

	MONITOR_ITEM* GetItemDataX(int i)
	{
		if (i >= 0 && i < m_arrData.GetSize())
			return &m_arrData[i];
		else
			return NULL;
	}

	HRESULT	OnLButtonUp( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bMsgHandled )
	{
		POINT	pts;
		BOOL	bOutSide;
		CRect	rcItem;

		bMsgHandled = FALSE;
		pts.x		= GET_X_LPARAM(lParam);
		pts.y		= GET_Y_LPARAM(lParam);

		INT nIndex = CListBox::ItemFromPoint(pts,bOutSide);// CListBox::HitTest(pts,&uFlags);

		if ( !bOutSide && nIndex >= 0 && nIndex < 0xffff && GetItemRect(nIndex,&rcItem,LVIR_BOUNDS) )
		{
			MONITOR_ITEM*	pData = (MONITOR_ITEM*)GetItemDataX(nIndex);
			if ( pData )
			{
				pts.x -= rcItem.left;
				pts.y -= rcItem.top;
				
				if ( m_rcCheck.PtInRect(pts) )
				{
					if (!pData->bDisable)
					{
						pData->bCheck = !pData->bCheck;
						m_linkerCB->OnCheck(nIndex, pData->bCheck);
						InvalidateRect(NULL);
					}

					/*
					CPoint	ptMouse;
					::GetCursorPos(&ptMouse);
					CBkSafeTip::Show(pData->strName,ptMouse,m_hWnd);
					*/
				}
				else if ( m_rcViewDetail.PtInRect(pts) && pData->bDel )
				{
					m_linkerCB->OnDelete(nIndex);
				}
			}
		}

		return S_OK;		
	}

	HRESULT	OnMouseWheel( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bMsgHandled )
	{
		bMsgHandled = FALSE;
		CBkSafeTip::Hide();
		return S_OK;
	}

	HRESULT	OnMouseMove( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bMsgHandled )
	{
		POINT	pts;
		CRect	rcItem;
		BOOL	bOutSide;
		BOOL	bOntip=FALSE;
		BOOL	bMove=FALSE;

		bMsgHandled = FALSE;
		pts.x		= GET_X_LPARAM(lParam);
		pts.y		= GET_Y_LPARAM(lParam);

		static	CPoint	ptLast;

		if (ptLast!=pts)
			bMove=TRUE;

		ptLast = pts;

		INT nIndex = CListBox::ItemFromPoint(pts,bOutSide); //CListViewCtrl::HitTest(pts,&uFlags);

		if ( !bOutSide && nIndex >= 0 && nIndex < 0xffff && GetItemRect(nIndex,&rcItem,LVIR_BOUNDS) )
		{
			MONITOR_ITEM*	pData = (MONITOR_ITEM*)GetItemDataX(nIndex);
			if ( pData )
			{
				pts.x -= rcItem.left;
				pts.y -= rcItem.top;

				if ( m_rcViewDetail.PtInRect(pts) && pData->bDel)
				{
					::SetCursor(::LoadCursor(NULL,IDC_HAND));
				}
				else if ( m_rcName.PtInRect(pts) )
				{
					if (bMove)
						SetTimer(XX_TIMER_ID_TIPS,200);
					bOntip = TRUE;
				}
			}
		}
		if (!bOntip)
		{
			KillTimer(XX_TIMER_ID_TIPS);
			CBkSafeTip::Hide();
		}
		return S_OK;
	}

	BOOL IsInTipArea(CPoint pts, CString* pText)
	{
		BOOL	bOutSide;
		INT		nIndex = CListBox::ItemFromPoint(pts,bOutSide);// CListBox::HitTest(pts,&uFlags);
		CRect	rcItem;
		BOOL	bIn = FALSE;

		if ( !bOutSide && nIndex >= 0 && nIndex < 0xffff && GetItemRect(nIndex,&rcItem,LVIR_BOUNDS) )
		{
			MONITOR_ITEM*	pData = (MONITOR_ITEM*)GetItemDataX(nIndex);
			if ( pData )
			{
				pts.x -= rcItem.left;
				pts.y -= rcItem.top;

				if ( m_rcName.PtInRect(pts))
				{
					if (pText!=NULL)
					{
						*pText = pData->strTip.IsEmpty() ? pData->strName : pData->strTip;
					}
					bIn = TRUE;
				}
			}
		}
		return bIn;
	}

	void OnTimer(UINT_PTR nIDEvent)
	{
		if (nIDEvent == XX_TIMER_ID_TIPS)
		{
			CString	strtips;
			CPoint	ptx;
			CPoint	ptxbak;
			::GetCursorPos(&ptx);
			ptxbak		= ptx;
			ptxbak.y	-= 25;
			ScreenToClient(&ptx);
			if (IsInTipArea(ptx,&strtips))
			{
				CBkSafeTip::Show(strtips,ptxbak,m_hWnd);
			}

			KillTimer(nIDEvent);
		}
	}

	void MeasureItem(LPMEASUREITEMSTRUCT lpMes)
	{
		lpMes->itemHeight = XX_LIST_ITEM_HEIGHT;
	}

	VOID SetNoPaintItem(BOOL bNoPaint)
	{
		m_bNoPaintItem = bNoPaint;
	}

	CRect GetCenterRect(CRect rc, CSize sz,  BOOL bVCenter=TRUE, BOOL bHCenter=TRUE)
	{
		CRect	rcRes = rc;

		if (bVCenter)
			rcRes.top		= rc.top+(rc.Height()-sz.cy)/2;
		else
			rcRes.top		= rc.top;

		if (bHCenter)
			rcRes.left		= rc.left+(rc.Width()-sz.cx)/2;
		else
			rcRes.left		= rc.left;

		rcRes.bottom	= rcRes.top+sz.cy;
		rcRes.right		= rcRes.left+sz.cx;

		return rcRes;

	}

	void DrawItem ( LPDRAWITEMSTRUCT lpdis )
	{
		if ( m_bNoPaintItem )
			return;

		CRect rcWindow;
		INT	  nColumWidth = 0;
		MONITOR_ITEM*	pListData = GetItemDataX(lpdis->itemID);

		GetWindowRect(&rcWindow);
		nColumWidth	= rcWindow.Width();
		if ( nColumWidth <= 0 )
			nColumWidth = lpdis->rcItem.right-lpdis->rcItem.left;

		if ( pListData != NULL )
		{
			CDCHandle		dcx;
			CDC				dcTmp;
			HFONT			hFntTmp;
			DWORD			colTxtTmp;
			DWORD			nOldMod;
			dcx.Attach(lpdis->hDC);
			dcTmp.CreateCompatibleDC(dcx);

			nOldMod = dcx.GetBkMode();
			dcx.SetBkMode(TRANSPARENT);

			hFntTmp = dcx.SelectFont(m_fntDefault);
			colTxtTmp = dcx.GetTextColor();

			BOOL bSelect = FALSE ;
			if ((lpdis->itemAction | ODA_SELECT) &&
				(lpdis->itemState & ODS_SELECTED))
			{
				//bSelect = TRUE ;
			}

			CRect	rcCheckbox = m_rcCheck;
			rcCheckbox.OffsetRect( lpdis->rcItem.left, lpdis->rcItem.top);

			HBITMAP hBmpCheck	= BkBmpPool::GetBitmap(IDB_BITMAP_LISTCTRL_CHECK);
			HBITMAP hBmpOld		= dcTmp.SelectBitmap(hBmpCheck);
			if ( bSelect )
				dcx.FillSolidRect( lpdis->rcItem.left, lpdis->rcItem.top, nColumWidth, lpdis->rcItem.bottom-lpdis->rcItem.top, RGB(216,241,212));
			else
				dcx.FillSolidRect( lpdis->rcItem.left, lpdis->rcItem.top, nColumWidth, lpdis->rcItem.bottom-lpdis->rcItem.top, COL_DEFAULT_WHITE_BG);

			if (pListData->bDisable)
				dcx.BitBlt( rcCheckbox.left, rcCheckbox.top, rcCheckbox.Width(),rcCheckbox.Height(), dcTmp, 26,0,SRCCOPY);
			else if ( pListData->bCheck )
				dcx.BitBlt( rcCheckbox.left, rcCheckbox.top, rcCheckbox.Width(),rcCheckbox.Height(), dcTmp, 0,0,SRCCOPY);
			else
				dcx.BitBlt( rcCheckbox.left, rcCheckbox.top, rcCheckbox.Width(),rcCheckbox.Height(), dcTmp, 13,0,SRCCOPY);

			dcTmp.SelectBitmap(hBmpOld);


			if ((pListData->nType == enumMonitorItemTypeFile || pListData->nType == enumMonitorItemTypeDir || pListData->nType == enumMonitorItemTypeProcess) && pListData->nImg != IDB_CUSTOM)
			{
				dcx.DrawIconEx(
					rcCheckbox.left + 18, 
					rcCheckbox.top - 3, 
					(HICON)pListData->nImg, 16, 16, 0, 0, DI_NORMAL);
			}
			else
			{
				Gdiplus::Image* pImg = BkPngPool::Get(pListData->nImg);
				if (pImg)
				{
					Gdiplus::Graphics graphics(dcx);

					SIZE size = {0, 0};
					if (pImg)
					{
						size.cx = pImg->GetWidth();
						size.cy = pImg->GetHeight();
					}

					graphics.DrawImage(pImg, Gdiplus::Rect(rcCheckbox.left + 18, rcCheckbox.top - 3, size.cx, size.cy));
				}
			}



			CRect	rcName	= m_rcName;
			rcName.right = nColumWidth - m_rcViewDetail.Width();
			rcName.OffsetRect(lpdis->rcItem.left, lpdis->rcItem.top);
			
			dcx.SetTextColor( XX_COL_COMMON_PLUG_NAME );

			CString		strFormat;
			strFormat.Format( _T("%s"), pListData->strName);
			CRect		rcProbe;
			CRect		rcActive=rcName;
			dcx.DrawText(strFormat,-1,&rcProbe,DT_VCENTER|DT_SINGLELINE|DT_CALCRECT);
			if ( rcProbe.Width() > rcName.Width() - 8)
			{
				rcName.right -= 8;
				rcActive.left = rcName.right;
			}
			else
			{
				rcActive.left = rcName.left+rcProbe.Width();
			}
			pListData->rcName = rcName;
			dcx.DrawText( strFormat, -1, &rcName, DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS );
			dcx.SelectFont(m_fntDefault);

			if (pListData->bNew)
			{
				Gdiplus::Image* pImg = BkPngPool::Get(IDB_TIP_NEW);
				if (pImg)
				{
					Gdiplus::Graphics graphics(dcx);

					SIZE size = {0, 0};
					if (pImg)
					{
						size.cx = pImg->GetWidth();
						size.cy = pImg->GetHeight();
					}

					graphics.DrawImage(pImg, Gdiplus::Rect(rcName.left + rcProbe.right - 1, rcName.top - 2, size.cx, size.cy));
				}
			}

			if (pListData->bDel)
			{
				dcx.SelectFont(m_fntLink);
				dcx.SetTextColor(COL_DEFAULT_LINK);

				CRect	rcViewdetail = m_rcViewDetail;
				rcViewdetail.OffsetRect(lpdis->rcItem.left, lpdis->rcItem.top);
				dcx.DrawText(_T("删除"),-1,&rcViewdetail,DT_VCENTER|DT_SINGLELINE);
				dcx.SelectFont(m_fntDefault);
			}


			//HPEN	hPenTmp;
			//CPen	penx;
			//penx.CreatePen( PS_SOLID,1,RGB(211,211,211) );
			//hPenTmp = dcx.SelectPen(penx);
 		//	dcx.MoveTo(lpdis->rcItem.left,lpdis->rcItem.bottom-1);
 		//	dcx.LineTo(lpdis->rcItem.left+nColumWidth,lpdis->rcItem.bottom-1);

			//dcx.SelectPen(hPenTmp);
			dcx.SetTextColor(colTxtTmp);
			dcx.SelectFont(hFntTmp);
			dcx.SetBkMode(nOldMod);
		}
	}

	VOID DeleteItem(LPDELETEITEMSTRUCT )
	{

	}

	VOID DeleteItem(int nId)
	{
		if (nId < 0 || nId >= m_arrData.GetSize())
			return;

		m_arrData.RemoveAt(nId);
		CListBox::DeleteString(nId);
	}

	int	GetItemCount()
	{
		return CListBox::GetCount();
	}


	BOOL GetItemRect(int nItem, LPRECT lpRect, UINT nCode)
	{
		return  CListBox::GetItemRect(nItem,lpRect);
	}

	VOID DeleteAllItems()
	{
		m_arrData.RemoveAll();
		CListBox::ResetContent();
	}

	VOID CheckAllItem()
	{
		for ( INT i= 0; i < GetItemCount(); i++)
		{
			MONITOR_ITEM*	pData = (MONITOR_ITEM*)GetItemDataX(i);
			if ( pData )
			{
				pData->bCheck = TRUE;
			}
		}
		InvalidateRect(NULL);
	}

	BOOL IsItemInList(CString& strItem)
	{
		for ( int i= 0; i < m_arrData.GetSize(); i++)
		{
			if (m_arrData[i].strMonitorItem == strItem)
				return TRUE;
		}
		return FALSE;
	}
};
