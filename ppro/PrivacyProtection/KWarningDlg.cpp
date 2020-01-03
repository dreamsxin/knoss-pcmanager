#include "stdafx.h"
#include "KWarningDlg.h"
#include "assert.h"
#include <wtypes.h>

KWarningDlg::~KWarningDlg()
{
	
}


LRESULT KWarningDlg::OnCreatePage(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	return 0;
}



BOOL KWarningDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{
	int nLeft = 114;
	int nTop = 32;
	if (m_strText.IsEmpty())
	{
		SetTextAndIcon(m_nType);
	}
	else
	{
		RECT rect;
		CString strPos;
		SetIcon(m_nType);
		SetText(m_strText);
		strPos.Format(_T("%d,%d"), nLeft + m_nOffset, nTop);
		SetItemStringAttribute(151, "pos", strPos);
	}
    return TRUE;
}

void KWarningDlg::OnBtnClose()
{
	EndDialog(IDCANCEL);
}

void KWarningDlg::OnButtonQuit()
{
	EndDialog(IDCANCEL);
}

//void KWarningDlg::OnBtnQuit()
//{
//
//	EndDialog(IDCANCEL);
//}

void KWarningDlg::EndDialog(UINT uRetCode)
{
    __super::EndDialog(uRetCode);
}

BOOL KWarningDlg::SetTextAndIcon(int nType )
{
	BOOL bRet = TRUE;
	switch (nType)
	{
	case 1:
		bRet &= SetItemText(151, BkString::Get(400));
		bRet &= SetItemStringAttribute(160, "skin" , _T("iconok"));
		return TRUE;
	case 2:
		bRet &= SetItemText(151, BkString::Get(401));
		bRet &= SetItemStringAttribute(160, "skin" , _T("iconwarn"));
		return TRUE;
	case 3:

		return TRUE;
	default:
		return FALSE;
	}

	return FALSE;
}

BOOL KWarningDlg::SetText( CString str )
{
	BOOL bRet = TRUE;

	bRet &= SetItemText(151, str);
	return bRet;
}

BOOL KWarningDlg::SetTextPos( int nLeft, int nTop )
{
	BOOL bRet = TRUE;
	CString strTemp;
	strTemp.Format(_T("%d,%d"), nLeft, nTop);
	bRet = SetItemStringAttribute(151, "pos" ,strTemp);
	return bRet;
}

BOOL KWarningDlg::SetIcon( int nType )
{
	BOOL bRet = TRUE;
	switch (nType)
	{
	case 1:
		bRet &= SetItemStringAttribute(160, "skin" , _T("iconok"));
		return TRUE;
	case 2:
		bRet &= SetItemStringAttribute(160, "skin" , _T("iconwarn"));
		return TRUE;
	case 3:

		return TRUE;
	default:
		return FALSE;
	}

	return FALSE;
}