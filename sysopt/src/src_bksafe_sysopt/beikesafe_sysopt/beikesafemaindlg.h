#pragma once

#include <wtlhelper/whwindow.h>
#include "bkmsgdefine.h"
#include "beikesafemsgbox.h"
#include "beikesafesysoptuihandler.h"

class CBeikeSafeMainDlg
    : public CBkDialogImpl<CBeikeSafeMainDlg>
    , public CWHRoundRectFrameHelper<CBeikeSafeMainDlg>
{
public:
    CBeikeSafeMainDlg();
    ~CBeikeSafeMainDlg();
protected:
	CBeikeSafeSysOptUIHandler*       m_sysoptHandler;
	void		EndDialog(UINT uRetCode);
    void		OnBkBtnClose();
    void		OnBkBtnMax();
    void		OnBkBtnMin();
	void		OnSysCommand(UINT nID, CPoint point);
	BOOL		OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/);
	LRESULT		OnDestroy();

public:

    BK_NOTIFY_MAP(IDC_RICHVIEW_WIN)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_SYS_CLOSE, OnBkBtnClose)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_SYS_MAX,	OnBkBtnMax)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_SYS_MIN,	OnBkBtnMin)
	BK_NOTIFY_MAP_END()

	BEGIN_MSG_MAP_EX_DECLARE(CBeikeSafeMainDlg)
};