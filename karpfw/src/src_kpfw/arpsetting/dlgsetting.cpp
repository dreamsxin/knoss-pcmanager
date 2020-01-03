#include "stdafx.h"
#include "arpsetting.h"
#include "dlgsetting.h"
#include "dlgadvsetting.h"
#include "dlgeditgateway.h"
#include "common/utility.h"
#include "kpfw_dpt_def.h"

//////////////////////////////////////////////////////////////////////////

CArpSettingDlg::CArpSettingDlg()
{
    m_bkBrush.CreateSolidBrush(RGB(0xff, 0xff, 0xff));
    m_bmpCloseBtn.LoadBitmap(IDB_CLOSE_BTN);
    m_bmpCommonBtn.LoadBitmap(IDB_COMMON_BTN);
    m_bmpSkin.LoadBitmap(IDB_SKIN);

    m_bEnableFw = TRUE;
    m_bUseMyGw = FALSE;
    m_bSafeMode = FALSE;
    m_bBlockRecvAttack = TRUE;
    m_bBlockRecvIPAttack = TRUE;
    m_bBlockSendAttack = TRUE;
    m_bNoticeGw = FALSE;
    m_nNoticeTimer = 1;
}

CArpSettingDlg::~CArpSettingDlg()
{

}

//////////////////////////////////////////////////////////////////////////

LRESULT CArpSettingDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    // center the dialog on the screen
    // set icons
    HICON hIcon = (HICON)::LoadImage(_Module.GetInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
        IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
    SetIcon(hIcon, TRUE);
    HICON hIconSmall = (HICON)::LoadImage(_Module.GetInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
        IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
    SetIcon(hIconSmall, FALSE);

    DoDataExchange();

    CString strTitle;
    GetWindowText(strTitle);
    SetBkSkin(m_bmpSkin);
    InitSkinDialog(m_bmpCloseBtn, NULL, strTitle);
    SetWindowPos(NULL, 0, 0, 418, 368, SWP_NOMOVE);
    CenterWindow();

    m_btnOK.InitButton(m_bmpCommonBtn, 0, 0);
    m_btnCancel.InitButton(m_bmpCommonBtn, 0, 0);
    m_btnAdd.InitButton(m_bmpCommonBtn, 0, 0);
    m_btnEdit.InitButton(m_bmpCommonBtn, 0, 0);
    m_btnDel.InitButton(m_bmpCommonBtn, 0, 0);

    m_lstGateway.InsertColumn(0, _T("IP地址"));
    m_lstGateway.InsertColumn(1, _T("MAC地址"));
    m_lstGateway.SetColumnWidth(0, 120);
    m_lstGateway.SetColumnWidth(1, 180);
    //m_lstGateway.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

    DoDataExchange();

    if (m_bUseMyGw)
    {
        GetDlgItem(IDC_BTN_ADD).EnableWindow(TRUE);
        GetDlgItem(IDC_BTN_EDIT).EnableWindow(TRUE);
        GetDlgItem(IDC_BTN_DEL).EnableWindow(TRUE);
        GetDlgItem(IDC_LIST_GETWAY).EnableWindow(TRUE);
        GetDlgItem(IDC_LNK_GETGW).ShowWindow(SW_SHOW);
        UpdateMyGatewayList();
    }
    else
    {
        GetDlgItem(IDC_BTN_ADD).EnableWindow(FALSE);
        GetDlgItem(IDC_BTN_EDIT).EnableWindow(FALSE);
        GetDlgItem(IDC_BTN_DEL).EnableWindow(FALSE);
        GetDlgItem(IDC_LIST_GETWAY).EnableWindow(FALSE);
        GetDlgItem(IDC_LNK_GETGW).ShowWindow(SW_HIDE);
    }

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////

LRESULT CArpSettingDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if (DoDataExchange(TRUE))
    {
        EndDialog(wID);
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////

LRESULT CArpSettingDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    EndDialog(wID);
    return 0;
}

//////////////////////////////////////////////////////////////////////////

void CArpSettingDlg::OnPaint(CPaintDC& dc)
{

}

HRESULT CArpSettingDlg::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CDCHandle dc = GetDC();
    dc.SelectBrush(m_bkBrush);
    return (HRESULT)(ULONG_PTR)(HBRUSH)m_bkBrush;
}

void CArpSettingDlg::OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    BOOL bRetCode;

    if (IDC_LNK_ADV_SETTING == nID)
    {
        CArpAdvSettingDlg dlgAdvSetting;
        dlgAdvSetting.m_bNoticeGw = m_bNoticeGw;
        dlgAdvSetting.m_bSafeMode = m_bSafeMode;
        //dlgAdvSetting.m_bBlockRecvAttack = m_bBlockRecvAttack;
        dlgAdvSetting.m_bBlockRecvIPAttack = m_bBlockRecvIPAttack;
        dlgAdvSetting.m_bBlockSendAttack = m_bBlockSendAttack;
        dlgAdvSetting.m_nNoticeTimer = m_nNoticeTimer;
        if (dlgAdvSetting.DoModal() == IDOK)
        {
            m_bNoticeGw = dlgAdvSetting.m_bNoticeGw;
            m_bSafeMode = dlgAdvSetting.m_bSafeMode;
            m_bBlockRecvAttack = dlgAdvSetting.m_bBlockRecvAttack;
            m_bBlockRecvIPAttack = dlgAdvSetting.m_bBlockRecvIPAttack;
            m_bBlockSendAttack = dlgAdvSetting.m_bBlockSendAttack;
            m_nNoticeTimer = dlgAdvSetting.m_nNoticeTimer;

            // 保存设置
            bRetCode = SaveAdvSetting();
            if (!bRetCode)
            {
                //MessageBox();
            }
        }
    }

    if (IDC_LNK_GETGW == nID)
    {
        OnResetMyGw();
    }

    if (IDC_BTN_ADD == nID)
    {
        OnAddMyGw();
    }

    if (IDC_BTN_EDIT == nID)
    {
        OnEditMyGw();
    }

    if (IDC_BTN_DEL == nID)
    {
        OnDelMyGw();
    }

    if (IDC_CHK_USE_MYGW == nID)
    {
        CButton btn = GetDlgItem(nID);
        if (btn.GetCheck())
        {
            GetDlgItem(IDC_BTN_ADD).EnableWindow(TRUE);
            GetDlgItem(IDC_BTN_EDIT).EnableWindow(TRUE);
            GetDlgItem(IDC_BTN_DEL).EnableWindow(TRUE);
            GetDlgItem(IDC_LIST_GETWAY).EnableWindow(TRUE);
            GetDlgItem(IDC_LNK_GETGW).ShowWindow(SW_SHOW);
            UpdateMyGatewayList();
        }
        else
        {
            GetDlgItem(IDC_BTN_ADD).EnableWindow(FALSE);
            GetDlgItem(IDC_BTN_EDIT).EnableWindow(FALSE);
            GetDlgItem(IDC_BTN_DEL).EnableWindow(FALSE);
            GetDlgItem(IDC_LIST_GETWAY).EnableWindow(FALSE);
            GetDlgItem(IDC_LNK_GETGW).ShowWindow(SW_HIDE);
        }
    }
}

BOOL CArpSettingDlg::SaveAdvSetting()
{
    BOOL retval = FALSE;
    KxEPipeClientT<IArpInfoDpt> spArpFw;
    Kxe_ArpAdv_Param _advset;
    Kxe_NotifyGetway_Param _notifygw;
    Kxe_Getway_Param _gw;
    Kxe_HRESULT_Ret ret;
    Kxe_VOID_Param nil;
    int nRetCode;

    _advset.bSafeMode = m_bSafeMode;
    _advset.bBlockRecvAttack = m_bBlockRecvAttack;
    _advset.bBlockRecvIPAttack = m_bBlockRecvIPAttack;
    _advset.bBlockSendAttack = m_bBlockSendAttack;
    nRetCode = spArpFw.SetAdvSetting(_advset, ret);
    if (nRetCode)
        goto clean0;

    _notifygw.bNotify = m_bNoticeGw;
    _notifygw.nTimes = m_nNoticeTimer;
    nRetCode = spArpFw.SetNotifyGateway(_notifygw, ret);
    if (nRetCode)
        goto clean0;

    nRetCode = spArpFw.ApplySetting(nil, ret);
    if (nRetCode)
        goto clean0;

    retval = TRUE;

clean0:
    return retval;
}

void CArpSettingDlg::OnAddMyGw()
{
    CString strIp;
    CString strMac;
    CEditGatewayDlg dlgEditGateway;

    if (dlgEditGateway.DoModal() == IDOK)
    {
        USES_CONVERSION;
        MyGw gw;

        strIp = dlgEditGateway.m_strIp;
        strMac = dlgEditGateway.m_strMac;

        gw.strIpAddress = T2A(strIp);
        gw.strMacAddress = T2A(strMac);
        
        m_vMyGws.push_back(gw);

        UpdateMyGatewayList();
    }

}

void CArpSettingDlg::OnEditMyGw()
{
    CEditGatewayDlg dlgEditGateway;
    CString strIp;
    CString strMac;
    int nItem = m_lstGateway.GetSelectedIndex();
    std::vector<MyGw>::iterator it;

    if (-1 == nItem)
        return;

    m_lstGateway.GetItemText(nItem, 0, strIp);
    m_lstGateway.GetItemText(nItem, 1, strMac);

    dlgEditGateway.m_strIp = strIp;
    dlgEditGateway.m_strMac = strMac;

    if (dlgEditGateway.DoModal() == IDOK)
    {
        USES_CONVERSION;

        strIp = dlgEditGateway.m_strIp;
        strMac = dlgEditGateway.m_strMac;

        it = m_vMyGws.begin() + nItem;
        it->strIpAddress = T2A(strIp);
        it->strMacAddress = T2A(strMac);

        UpdateMyGatewayList();
    }
}

void CArpSettingDlg::OnDelMyGw()
{
    if (m_vMyGws.size() <= 1)
    {
        MessageBox(_T("需要至少保留一个网关条目！"), _T("金山卫士"), MB_ICONINFORMATION);
        return;
    }

    int nSelect = m_lstGateway.GetSelectedIndex();
    if ((size_t)nSelect >= m_vMyGws.size() || -1 == nSelect)
        return;

    m_vMyGws.erase(m_vMyGws.begin() + nSelect);
    UpdateMyGatewayList();
}

BOOL CArpSettingDlg::GetDefaultGetwayList()
{
    BOOL retval = FALSE;
    int nRetCode;
    KxEPipeClientT<IArpInfoDpt> spArpFw;
    Kxe_ArpHostInfo_Ret arphost; 
    Kxe_Getway_Ret gateway;
    Kxe_Getway_Param _gw;
    Kxe_VOID_Param nil;
    Kxe_HRESULT_Ret ret;
    size_t i;

    m_vDefaultGws.clear();

    nRetCode = spArpFw.ClearProtectList(nil, ret);
    if (nRetCode)
        goto clean0;

    nRetCode = spArpFw.GetHostInfo(nil, arphost);
    if (nRetCode)
        goto clean0;
    
     for (i = 0; i < (size_t)arphost.hostInfo.nGateWayCnt; ++i)
     {
         MyGw gw;
         struct in_addr ip;
         ip.s_addr = arphost.hostInfo.vecGetWay[i].ip;
         gw.strIpAddress = inet_ntoa(ip);
         mac_to_str((const BYTE*)&arphost.hostInfo.vecGetWay[i].mac, gw.strMacAddress);
         m_vDefaultGws.push_back(gw);
     }

     _gw.bAuto = FALSE;
     for (i = 0; i < m_vDefaultGws.size(); ++i)
     {
         Kxe_MacIp_Param macip;
         UINT uIp = inet_addr(m_vDefaultGws[i].strIpAddress.c_str());
         macip.ip = uIp;
         str_to_mac(m_vDefaultGws[i].strMacAddress, (BYTE*)&macip.mac);
         _gw.getway.push_back(macip);
     }
     nRetCode = spArpFw.SetAutoGateWay(_gw, ret);
     if (nRetCode)
         goto clean0;

     nRetCode = spArpFw.ApplySetting(nil, ret);
     if (nRetCode)
         goto clean0;

     retval = TRUE;

clean0:
    return retval;
}

void CArpSettingDlg::UpdateMyGatewayList()
{
    USES_CONVERSION;
    
    if (m_vMyGws.size() == 0)
    {
        for (size_t i = 0; i < m_vDefaultGws.size(); ++i)
        {
            m_vMyGws.push_back(m_vDefaultGws[i]);
        }
    }

    m_lstGateway.DeleteAllItems();
    for (int i = 0; i < (int)m_vMyGws.size(); ++i)
    {        
        m_lstGateway.InsertItem(i, A2T(m_vMyGws[i].strIpAddress.c_str()));
        m_lstGateway.SetItemText(i, 1, A2T(m_vMyGws[i].strMacAddress.c_str()));
    }
    
    if (m_vMyGws.size() <= 1)
    {
        m_btnDel.Enable(FALSE);
    }
    else
    {
        m_btnDel.Enable(TRUE);
    }
}

void CArpSettingDlg::OnResetMyGw()
{
    m_vMyGws.clear();
    GetDefaultGetwayList();
    UpdateMyGatewayList();
}

//////////////////////////////////////////////////////////////////////////
