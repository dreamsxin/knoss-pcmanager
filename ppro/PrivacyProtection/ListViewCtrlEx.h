#pragma once

#include <wtl/atlcrack.h>
#include <vector>
#include "KActor.h"


#define MSG_DEL_ALL		(WM_APP + 200)
#define MSG_ADD_ITEM	(WM_APP + 201)

class CHeaderCtrlEx : public CWindowImpl<CHeaderCtrlEx, CHeaderCtrl>
{
public:
	CHeaderCtrlEx();
	virtual ~CHeaderCtrlEx();
	void SetHeight(INT nHeight);


protected:
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);
	LRESULT OnHDMLayout(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT	OnMouseMove(UINT nFlags, CPoint point);

protected:
	CBitmap m_bitmapHeader;
	CFont m_font;
	INT m_nHeight;

	CRBMap<int, BOOL>	m_mapHeaderDrag;

public:
	BEGIN_MSG_MAP_EX(CHeaderCtrlEx) 
		MSG_WM_PAINT(OnPaint)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MESSAGE_HANDLER(HDM_LAYOUT, OnHDMLayout)
	END_MSG_MAP()
};

enum E_SubItemType
{
	SUBITEM_TEXT,
	SUBITEM_LINK,
	SUBITEM_ICON,
	SUBITEM_PNG
};

enum E_ListItemType
{
	LISTITEM_TITLE		= 1,
	LISTITEM_CHECKBOX	= 2,
	LISTITEM_RADIOBOX	= 4,
	LISTITEM_EXPANDABLE	= 8,
	LISTITEM_BOLD		= 16,
};

enum E_ListTitleType
{
	LISTTITLE_DEFAULT = 0x1,

	LISTITEM_THIRD_LINE_TOP = 0x2,
	LISTITEM_THIRD_LINE_MID = 0x4,
	LISTITEM_THIRD_LINE_BTM = 0x8,
	
	//æ”÷–
	LISTTILTE_CENTER=0x10,
};

class CListViewCtrlEx : 
	public CWindowImpl<CListViewCtrlEx, CListViewCtrl>, 
	public COwnerDraw<CListViewCtrlEx>
	, public IActorCallBack
{
	typedef CWindowImpl<CListViewCtrlEx, CListViewCtrl> _super;

public:
	typedef enum _LEVEL
	{
		enumLevelSafe,
		enumLevelRisk,
		enumLevelUnknown
	} LEVEL;
	struct TListItem;
	struct TListSubItem;
	typedef std::vector<TListItem*> TListItemPtrs;
	typedef std::vector<TListSubItem> TListSubItems;
	struct TListSubItem
	{
		TListSubItem() : type(SUBITEM_TEXT)
		{
			nMarginLeft = 2;
			clr = RGB(0,0,0);
			rcOffset = CRect(0,0,0,0);
			nImg = 0;
		}

		TListSubItem(LPCTSTR szText, CRect rc, E_SubItemType aType = SUBITEM_TEXT)
			: str(szText), type(aType), rcOffset(rc)
		{
			nImg = 0;
		}

		TListSubItem(LPCTSTR szText, E_SubItemType aType = SUBITEM_TEXT)
			: str(szText), type(aType)
		{
			rcOffset = CRect(0,0,0,0);
			nMarginLeft = 2;
			clr = RGB(0,0,0);
			nImg = 0;
			ATLASSERT(szText);
		}

		TListSubItem(LPCTSTR szText, HICON hIcon, E_SubItemType aType = SUBITEM_ICON)
			: str(szText), nImg((DWORD)hIcon), type(aType)
		{
			rcOffset = CRect(0,0,0,0);
			nMarginLeft = 2;
			clr = RGB(0,0,0);
			ATLASSERT(szText);
		}

		TListSubItem(LPCTSTR szText, DWORD nImgId, E_SubItemType aType = SUBITEM_PNG)
			: str(szText), nImg(nImgId), type(aType)
		{
			rcOffset = CRect(0,0,0,0);
			nMarginLeft = 2;
			clr = RGB(0,0,0);
			ATLASSERT(szText);
		}

		E_SubItemType type;
		CString str;
		COLORREF clr;
		int	nMarginLeft;
		CRect	rcOffset;
		CString	strUrl;
		DWORD	nImg;
	};
	
	struct TListItem
	{
	public:
		TListItem()
		{
			dwFlags		=0;
			clrBg		= RGB(0xfb,0xfc,0xfd);
			clrBtmGapLine=RGB(234,233,225);
			nLeftmargin	=10;
			nTopMargin	=-1;
			bBold		=FALSE;
			nHeightAdd	=0;

			_isclapsed = FALSE;
		}
		~TListItem()
		{
			for(TListItemPtrs::iterator it=_clapsed_items.begin(); it!=_clapsed_items.end(); ++it)
				delete *it;
			_clapsed_items.clear();
		}
		
		int			nLeftmargin;
		int			nTopMargin;
		BOOL		bBold;
		int			nHeightAdd;
		DWORD		dwFlags;
		COLORREF	clrBg;
		COLORREF	clrBtmGapLine;
		TListSubItems subItems;

		LEVEL		nLevel;
		DWORD		dwMapId;
		
		BOOL _isclapsed;
		TListItemPtrs _clapsed_items;
		DWORD_PTR _itemData;
		int			nRiskRefIndex;
	};

public:
	CListViewCtrlEx(void);
	~CListViewCtrlEx(void);
	CHeaderCtrlEx m_ctlHeader;
	
protected:
	CFont m_fontLink, m_fontBold, m_fontTitle, m_fontDef;
	CBitmap m_bitmapCheck, m_bitmapExpand, m_bitmapRadio;
	CBitmap m_bitmapRadioMask;
	int m_nHoverItem;
	int m_nHoverSubItem;
	
	TListItemPtrs m_arrItems;
	HWND m_hWndObserver;

	CString m_strEmptyString;
	bool	m_bOnlyShowRisk;
	CSimpleArray<DWORD>	m_RiskItemIDArray;
	CSimpleArray<DWORD>	m_UnknownItemIDArray;
	CRITICAL_SECTION	m_hListLock;
	KActor				m_ActorRefresh;

public:
	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
		DWORD dwStyle = 0, DWORD dwExStyle = 0,
		_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL);
	BOOL GetCheckState(int nIndex, BOOL *pIsRadio=NULL) const;
	BOOL SetCheckState(int nItem, BOOL bCheck);
	int SetRadioState(int nItem, BOOL bCheck);	// -1: not valid , 0 : not checked, 1: checked 
	
	void ClearRadioCheck();
	void CleanCheck( int nItem=-1 );
	void CheckAll( );
	template<typename Func> void CheckAll(Func fn)
	{
		ClearRadioCheck();
		for(int i=0; i<_super::GetItemCount(); ++i)
		{
			T_VulListItemData *pItemData = (T_VulListItemData*)_super::GetItemData(i);
			TListItem *pItem = _GetItemData( i );
			if( pItem && (pItem->dwFlags&LISTITEM_CHECKBOX) )
			{
				_super::SetCheckState( i, fn(pItemData) );
			}
		}
	}
	void SetObserverWindow(HWND hWnd);
	
protected:
	DWORD SetExtendedListViewStyle(DWORD dwExStyle, DWORD dwExMask = 0)
	{
		ATLASSERT(FALSE);
		return _super::SetExtendedListViewStyle(dwExStyle, dwExMask);
	}
	int AddItem(int nItem, int nSubItem, LPCTSTR strItem, int nImageIndex = -1)
	{
		ATLASSERT(FALSE);
		return _super::AddItem(nItem, nSubItem, strItem, nImageIndex);
	}
	BOOL SetItemText(int nItem, int nSubItem, LPCTSTR lpszText)
	{
		ATLASSERT(FALSE);
		return _super::SetItemText(nItem, nSubItem, lpszText);
	}
	
public:
	void Reset();

	int AppendTitle(LPCTSTR strItem, COLORREF clr, UINT uFlags=0);	
	int AppendTitleItem(int nItem, LPCTSTR strItem, CRect rc, E_SubItemType itemType, COLORREF clr, LPCTSTR szURL);
	int Append(LPCTSTR strItem, DWORD dwFlags=0, E_SubItemType itemType=SUBITEM_TEXT);	
	int Append(LPCTSTR strItem, HICON hIcon, LEVEL nLevel = enumLevelSafe, DWORD dwFlags = 0);	
	int Append(LPCTSTR strItem, HICON hIcon, DWORD dwMapId, DWORD dwFlags = 0);	
	int AppendSubItem(int nItem, LPCTSTR strItem, E_SubItemType itemType = SUBITEM_TEXT);	
	int AppendSubItem(int nItem, LPCTSTR strItem, DWORD dwImg, E_SubItemType itemType);
	bool SetSubItem(int nItem, int nSubItem, LPCTSTR lpszItem, E_SubItemType itemType=SUBITEM_TEXT, BOOL bRedraw=TRUE);
	bool GetSubItemText(int nItem, int nSubItem, CString &str);
	bool SetSubItemColor(int nItem, int nSubItem, COLORREF clr, BOOL bRedraw=TRUE);
	bool SetItemCheckX(int nItem, DWORD dwFlags);
	BOOL DeleteItem(int nItem);	
	BOOL DeleteAllItems();
	void SetEmptyString(LPCTSTR szEmptyString);
	void ExpandGroup(INT nItem, BOOL bExpand);
	
	void SetOnlyShowRisk(bool bShowRisk);
	DWORD GetAllSafeCount();
	DWORD GetAllRiskCount();
	DWORD GetAllUnknownCount();

	//////////////////////////z////////////////////////////////////////////////
	//  µœ÷IActorCallBack
	virtual void	OnActivate(KActor* pActor);

public:
	BEGIN_MSG_MAP(CListViewCtrlEx)   
		MSG_WM_PAINT(OnPaint)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
		MSG_WM_SETCURSOR(OnSetCursor)
		MSG_WM_TIMER(OnTimer)
		NOTIFY_CODE_HANDLER(NM_CLICK, OnClick)
		NOTIFY_CODE_HANDLER(HDN_DIVIDERDBLCLICK , OnTrackDbClickHeader)
		NOTIFY_CODE_HANDLER(HDN_TRACK , OnTrackMoveHeader)
		NOTIFY_CODE_HANDLER(HDN_ENDTRACK , OnHeaderEndTrack)
		MESSAGE_HANDLER_EX(MSG_DEL_ALL, OnDeleteAll)
		MESSAGE_HANDLER_EX(MSG_ADD_ITEM, OnAddItem)
		REFLECTED_NOTIFY_CODE_HANDLER(NM_CLICK, OnClick)
		CHAIN_MSG_MAP_ALT(COwnerDraw<CListViewCtrlEx>, 1)	
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()
	
	void OnPaint(CDCHandle dc);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnMouseLeave();
	void OnTimer(UINT_PTR nIDEvent);
	BOOL OnSetCursor(CWindow wnd, UINT nHitTest, UINT message);
	LRESULT OnClick(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
		
	LRESULT OnTrackDbClickHeader(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);	
	LRESULT OnTrackMoveHeader(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);	
	LRESULT OnHeaderEndTrack(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT	OnDeleteAll(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	LRESULT	OnAddItem(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);

	void MeasureItem(LPMEASUREITEMSTRUCT lpMes);
	void DrawItem ( LPDRAWITEMSTRUCT lpdis );
	VOID DeleteItem(LPDELETEITEMSTRUCT );

public:
	const TListItem *_GetItemData(int nItem) const;
	TListItem *_GetItemData(int nItem);
	const TListSubItem *_GetSubItemData(int nItem, int nSubItem);

protected:
	void _DrawTitleItem( LPDRAWITEMSTRUCT lpdis, const TListItem *pItem );
	void _DrawNormalItem( LPDRAWITEMSTRUCT lpdis, const TListItem *pItem );
	BOOL _PtInSubItemCheckBox( const POINT& pt, int nItem );
	BOOL _GetSubItemLinkRect( int nItem, int nSubItem, LPCTSTR szText, RECT &rc );
	bool _PtInSubItemLink( const POINT &pt, int nItem, int & nSubItem );
	int _DrawCheckBox( CDCHandle &dc, RECT &rcItem, BOOL bChecked, DWORD dwFlags );
	RECT _GetRectCheckBox( RECT &rcItem );
	RECT _GetRectMinus( RECT &rcItem );
	void _InvalidateRect( int nItem, int nSubItem );
	BOOL _FireEvent(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

	void _SetColumnNeedWidth(int i, DWORD nWidth);
	void _RedrawTitle();
	BOOL _ExpandItem( TListItem * pItem, INT iItem, BOOL expand );
protected:
	CRBMap<int,DWORD>	m_columnMaxWidth;
};
