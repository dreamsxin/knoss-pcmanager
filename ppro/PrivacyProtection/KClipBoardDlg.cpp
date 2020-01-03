#include "stdafx.h"
#include "KClipBoardDlg.h"
#include "assert.h"
#include <wtypes.h>


KClipBoardDlg::~KClipBoardDlg()
{
	
}




LRESULT KClipBoardDlg::OnCreatePage(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	return 0;
}



BOOL KClipBoardDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{
	CopyToClipboard();
    return TRUE;
}

void KClipBoardDlg::OnBtnClose()
{

	EndDialog(IDCANCEL);
}

void KClipBoardDlg::OnButtonQuit()
{
	EndDialog(IDCANCEL);
}

//void KClipBoardDlg::OnBtnQuit()
//{
//
//	EndDialog(IDCANCEL);
//}


BOOL KClipBoardDlg::CopyToClipboard()
{
	BOOL bRet = FALSE;
	CStringW strData;
	CStringW strData2;
	CStringW strData3;
	strData = BkString::Get(116);
	strData2 = BkString::Get(117);
	strData3 = BkString::Get(118);

	strData.Append(_T("\r\n") + strData2 + _T("\r\n") + strData3);
	USES_CONVERSION;

	WCHAR *pszStr1;

	pszStr1 = strData.GetBuffer();
	bRet = CopyToClipboard_(pszStr1, wcslen(pszStr1) * sizeof(WCHAR));

	return bRet;
}
// 复制数据至剪切板
BOOL KClipBoardDlg::CopyToClipboard_(const WCHAR* pszData, const int nDataLen)
{
	if(::OpenClipboard(NULL))
	{
		::EmptyClipboard();
		HGLOBAL clipbuffer;
		WCHAR *buffer;
		clipbuffer = ::GlobalAlloc(GMEM_MOVEABLE, nDataLen + sizeof(WCHAR));
		buffer = (WCHAR *)::GlobalLock(clipbuffer);
		wcscpy(buffer, pszData);
		::GlobalUnlock(clipbuffer);
		::SetClipboardData(CF_UNICODETEXT, clipbuffer);
		::CloseClipboard();
		return TRUE;
	}
	return FALSE;
}

void KClipBoardDlg::EndDialog(UINT uRetCode)
{
    __super::EndDialog(uRetCode);
}