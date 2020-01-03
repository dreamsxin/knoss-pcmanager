#pragma once

#include <wtlhelper/whwindow.h>
//#include "bkmsgdefine.h"

#include "beikesafemsgbox.h"
#include "vulfix/BeikeVulfixHandler.h"

#include "bksafeuihandlerbase.h"


class CBeikeSafeExamUIHandler;


class CBeikeSafeMainDlg
    : public CBkDialogImpl<CBeikeSafeMainDlg>
    , public CWHRoundRectFrameHelper<CBeikeSafeMainDlg>
    , public CBkNavigator
	, public CBkSafeUIHandlerMgr
{
public:
    CBeikeSafeMainDlg();
    ~CBeikeSafeMainDlg();

    void SetDefaultNavigate(LPCWSTR lpszNavigate);

    void EndDialog(UINT uRetCode);

 	void TabMainSelChange( int nTabOldItem, int nTabNewItem );
protected:

    CBeikeSafeExamUIHandler*         m_ExamHandler;

    CWHHtmlContainerWindow m_wndHtmlOnlineInfo;

//    CBeikeSafeSettingNavigator m_SettingCaller;

    BOOL m_bFirstPageChange;
    BOOL m_bPage0NeverShowed;
    BOOL m_bPage1NeverShowed;
    BOOL m_bPage3NeverShowed;
	BOOL m_bPage5NeverShowed;

    CString m_strDefaultNavigate;

    void OnBkBtnClose();
    void OnBkBtnMax();
    void OnBkBtnMin();

    BOOL OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/);
	LRESULT OnDestroy();
	LRESULT OnTimer( UINT_PTR uIdEvent );
    LRESULT OnAppDefaultNavigate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);

    LRESULT OnAppEchoFinish(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
    BOOL OnBkTabMainSelChange(int nTabItemIDOld, int nTabItemIDNew);
    void OnSysCommand(UINT nID, CPoint point);
	
	LRESULT OnAppDelayExam(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	LRESULT OnAppDelayNavigateIE(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam);
	void InitVulFix();


protected:
	// Vulfix 
	BOOL			m_bVulfixInited;
	BOOL			m_bVulfixRescanRequired;
	CEmbeddedView	m_viewVulfix;
	BOOL			m_bIeFixInit;
	BOOL			m_bKClearInit;

public:


    BK_NOTIFY_MAP(IDC_RICHVIEW_WIN)
        //BK_NOTIFY_ID_COMMAND(IDCANCEL, OnBkBtnClose)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_SYS_CLOSE, OnBkBtnClose)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_SYS_MAX, OnBkBtnMax)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_SYS_MIN, OnBkBtnMin)
        BK_NOTIFY_TAB_SELCHANGE(IDC_TAB_MAIN, OnBkTabMainSelChange)
    BK_NOTIFY_MAP_END()

	BEGIN_MSG_MAP_EX_DECLARE(CBeikeSafeMainDlg)
};
