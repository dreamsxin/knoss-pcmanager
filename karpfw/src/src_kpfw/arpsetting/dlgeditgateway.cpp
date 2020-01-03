#include "stdafx.h"
#include "arpsetting.h"
#include "dlgeditgateway.h"
#include "kpfw_dpt_def.h"

//////////////////////////////////////////////////////////////////////////

CEditGatewayDlg::CEditGatewayDlg()
{
    m_bkBrush.CreateSolidBrush(RGB(0xff, 0xff, 0xff));
    m_bmpCloseBtn.LoadBitmap(IDB_CLOSE_BTN);
    m_bmpCommonBtn.LoadBitmap(IDB_COMMON_BTN);
    m_bmpSkin.LoadBitmap(IDB_SKIN);
    InterlockedExchange((volatile LONG*)&m_bGetMacing, FALSE);
    m_hGetMacThread = NULL;
}

CEditGatewayDlg::~CEditGatewayDlg()
{

}

//////////////////////////////////////////////////////////////////////////

LRESULT CEditGatewayDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
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
    SetWindowPos(NULL, 0, 0, 351, 196, SWP_NOMOVE);
    CenterWindow();

    m_btnOK.InitButton(m_bmpCommonBtn, 0, 0);
    m_btnCancel.InitButton(m_bmpCommonBtn, 0, 0);

    if (!m_strIp.IsEmpty())
    {
        USES_CONVERSION;
        ULONG dwIp = inet_addr(T2A(m_strIp));
        m_ctlIpAddress.SetAddress(ntohl(dwIp));
        m_ctrlMacAddress.SetWindowText(m_strMac);
    }

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////

LRESULT CEditGatewayDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    USES_CONVERSION;
    std::string mac;
    BYTE mac_addr[6] = { 0 };
    CString strMac;

    m_ctrlMacAddress.GetWindowText(strMac);
    mac = T2A(strMac);
    if (!str_to_mac(mac, mac_addr))
    {
        MessageBeep(MB_ICONQUESTION);
        return 0;
    }

    m_strMac = strMac;
    m_ctlIpAddress.GetWindowText(m_strIp);

    EndDialog(wID);
    return 0;
}

//////////////////////////////////////////////////////////////////////////

LRESULT CEditGatewayDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    EndDialog(wID);
    return 0;
}

//////////////////////////////////////////////////////////////////////////

void CEditGatewayDlg::OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    if (IDC_LNK_GETMAC == nID)
    { 
        if (m_bGetMacing)
            return;

        InterlockedExchange((volatile LONG*)&m_bGetMacing, TRUE);

        m_lnkGetMac.EnableWindow(FALSE);
        m_lnkGetMac.InvalidateRect(NULL);
        m_hGetMacThread = (HANDLE) _beginthreadex(NULL, 0, ThreadFun, (LPVOID)this, 0, NULL);
    }
}

unsigned __stdcall CEditGatewayDlg::ThreadFun(void* params)
{
    CEditGatewayDlg* pThis = (CEditGatewayDlg* )params;
    if(NULL != pThis)
    {
        KxEPipeClientT<IArpInfoDpt> spArpFw;
        Kxe_MacIp_Param ip = { 0 };
        Kxe_MacIp_Ret ret;
        int nRetCode;
        DWORD dwIp;
        std::string mac;
        CString strMac;

        pThis->m_ctlIpAddress.GetAddress(&dwIp);
        ip.ip = htonl(dwIp);

        nRetCode = spArpFw.GetMacFromIp(ip, ret);
        if (nRetCode)
            goto clean0;

        mac_to_str((const BYTE*)&ret.macIp.mac, mac);
        {
            USES_CONVERSION;
            strMac = A2T(mac.c_str());
        }
        pThis->m_ctrlMacAddress.SetWindowText(strMac);   
        pThis->m_lnkGetMac.EnableWindow(TRUE);  
        pThis->m_lnkGetMac.InvalidateRect(NULL);
    }

clean0:
    pThis->m_hGetMacThread = NULL;
    InterlockedExchange((volatile LONG*)&pThis->m_bGetMacing, FALSE);

    return 0;
}

void CEditGatewayDlg::OnGetMac()
{
//     KxEPipeClientT<IArpInfoDpt> spArpFw;
//     Kxe_MacIp_Param ip = { 0 };
//     Kxe_MacIp_Ret ret;
//     int nRetCode;
//     DWORD dwIp;
//     std::string mac;
//     CString strMac;
// 
//     m_ctlIpAddress.GetAddress(&dwIp);
//     ip.ip = htonl(dwIp);
// 
//     nRetCode = spArpFw.GetMacFromIp(ip, ret);
//     if (nRetCode)
//         goto clean0;
// 
//     mac_to_str((const BYTE*)&ret.macIp.mac, mac);
//     {
//         USES_CONVERSION;
//         strMac = A2T(mac.c_str());
//     }
//     m_ctrlMacAddress.SetWindowText(strMac);
// 
// clean0:
    return;
}

void CEditGatewayDlg::OnPaint(CPaintDC& dc)
{

}

HRESULT CEditGatewayDlg::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CDCHandle dc = GetDC();
    dc.SelectBrush(m_bkBrush);
    return (HRESULT)(ULONG_PTR)(HBRUSH)m_bkBrush;
}

//////////////////////////////////////////////////////////////////////////

BOOL mac_to_str(const BYTE* pMac, std::string& strMac)
{
    BOOL retval = FALSE;
    char szMac[64] = { 0 };

    if (!pMac)
        goto clean0;

    sprintf(szMac, "%02x-%02x-%02x-%02x-%02x-%02x",
        pMac[0], pMac[1], pMac[2], pMac[3], pMac[4], pMac[5]);
    strMac = szMac;

    retval = TRUE;

clean0:
    return retval;
}

BOOL str_to_mac(const std::string& strMac, BYTE* pMac)
{
    BOOL retval = FALSE;
    size_t i;

    if (!pMac)
        goto clean0;

    if (strMac.length() != 12 && strMac.length() != 17)
        goto clean0;

    if (strMac.length() == 12)
    {
        for (i = 0; i < 6; ++i)
        {
            char szByte[3] = { 0 };
            int nTemp;
            szByte[0] = strMac[i * 2];
            szByte[1] = strMac[i * 2 + 1];
            sscanf(szByte, "%02X", &nTemp);
            pMac[i] = nTemp & 0xFF;
        }
    }

    if (strMac.length() == 17)
    {
        for (i = 0; i < 6; ++i)
        {
            char szByte[3] = { 0 };
            int nTemp;
            szByte[0] = strMac[i * 3];
            szByte[1] = strMac[i * 3 + 1];
            sscanf(szByte, "%02X", &nTemp);
            pMac[i] = nTemp & 0xFF;
        }
    }

    retval = TRUE;

clean0:
    return retval;
}

void CEditGatewayDlg::OnDestroy()
{
    if (m_hGetMacThread)
    {
        if (::WaitForSingleObject(m_hGetMacThread, 500) != WAIT_OBJECT_0)
        {
            ::TerminateThread(m_hGetMacThread, 1);
        }

        ::CloseHandle(m_hGetMacThread);
        m_hGetMacThread = NULL;
    }
}

//////////////////////////////////////////////////////////////////////////
