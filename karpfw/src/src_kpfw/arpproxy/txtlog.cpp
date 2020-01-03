#include "StdAfx.h"
#include "txtlog.h"

static const char * StringBlockReason[] = {
	"None",
	"发送攻击包",
	"目标ＩＰ是网关,但ＭＡＣ不是",
	"目标ＩＰ是绑定地址,但ＭＡＣ不是",
	"安全模式下，发送ＡＲＰ给其它机器(除了网关和已绑定地址)",
	"收到源ＩＰ等于自己的ＡＲＰ包(ＩＰ冲突)",
	"源ＩＰ是网关,但是ＭＡＣ不是",
	"源ＩＰ是绑定地址,但是ＭＡＣ不是",
	"安全模式下，接收到ＡＲＰ从其它机器(除了网关和已绑定地址)"
};

static const char * StringBlockReasonLite[] = {
    "None",
    "ARP攻击",
    "ARP攻击(目标IP是网关,但MAC不是)",
    "ARP攻击(目标IP是绑定地址,但MAC不是)",
    "安全模式下的发送ARP消息",
    "IP地址冲突(收到源IP等于自己的ARP包)",
    "ARP攻击(源IP是网关,但是MAC不是)",
    "ARP攻击(源IP是绑定地址,但是MAC不是)",
    "安全模式下的接收ARP消息"
};

CTxtLog::CTxtLog()
{
	m_hLog = INVALID_HANDLE_VALUE;
}


CTxtLog::~CTxtLog()
{
}


//////////////////////////////////////////////////////////////////////////

BOOL CTxtLog::Init()
{
	WCHAR wsPath[MAX_PATH + 1] = {0};

	if( m_hLog != INVALID_HANDLE_VALUE )
		return TRUE;

	::GetModuleFileName(NULL, wsPath, MAX_PATH);

	*(wcsrchr( wsPath, '\\' )) = NULL;
	wcscat_s( wsPath, MAX_PATH, L"\\AppData" );
	CreateDirectory(wsPath, NULL);
	wcscat_s( wsPath, MAX_PATH, L"\\antiarp.log" );

	m_hLog = CreateFile( wsPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if( m_hLog != NULL )
	{
		SetFilePointer( m_hLog, 0, 0, FILE_END );
	}

	return m_hLog != INVALID_HANDLE_VALUE;
}

//////////////////////////////////////////////////////////////////////////

VOID CTxtLog::Unint()
{
	if( m_hLog != INVALID_HANDLE_VALUE )
	{
		CloseHandle( m_hLog );
		m_hLog = INVALID_HANDLE_VALUE;
	}
}

//////////////////////////////////////////////////////////////////////////

DWORD CTxtLog::GetSize()
{
	DWORD dwFileSize = 0;
	if( m_hLog != INVALID_HANDLE_VALUE )
		dwFileSize = GetFileSize( m_hLog, NULL );

	return dwFileSize;
}

//////////////////////////////////////////////////////////////////////////

BOOL CTxtLog::AddLog( PLOG_IP_RULE pIpRule )
{
	BOOL bResult = FALSE;
	string strLog;
    string strLogLite;
	char sMsg[128] = {0};
	SYSTEMTIME sysTime = {0};
	DWORD dwWritten = 0;

	do 
	{
		if( m_hLog == INVALID_HANDLE_VALUE )
			break;

		if( pIpRule->nAttackType >= (sizeof(StringBlockReason) / sizeof(char*)) )
			break;

		//安全模式下记录太多
		if (IsSameModeArpAttack(pIpRule->nAttackType))
			break;

		::GetLocalTime( &sysTime );
		sprintf_s( sMsg, 128, "%02d-%02d %02d:%02d:%02d\t", sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond );
		strLog += sMsg;

		sprintf_s( sMsg, 128, "%d.%d.%d.%d[%02X-%02X-%02X-%02X-%02X-%02X]\t%d.%d.%d.%d[%02X-%02X-%02X-%02X-%02X-%02X]\t", 
			((UCHAR*)&pIpRule->ExtraInfo.ARP_DATA.arp_spa)[0], 
			((UCHAR*)&pIpRule->ExtraInfo.ARP_DATA.arp_spa)[1], 
			((UCHAR*)&pIpRule->ExtraInfo.ARP_DATA.arp_spa)[2], 
			((UCHAR*)&pIpRule->ExtraInfo.ARP_DATA.arp_spa)[3], 
			pIpRule->ExtraInfo.ARP_DATA.arp_sha[0],
			pIpRule->ExtraInfo.ARP_DATA.arp_sha[1],
			pIpRule->ExtraInfo.ARP_DATA.arp_sha[2],
			pIpRule->ExtraInfo.ARP_DATA.arp_sha[3],
			pIpRule->ExtraInfo.ARP_DATA.arp_sha[4],
			pIpRule->ExtraInfo.ARP_DATA.arp_sha[5],
			((UCHAR*)&pIpRule->ExtraInfo.ARP_DATA.arp_tpa)[0], 
			((UCHAR*)&pIpRule->ExtraInfo.ARP_DATA.arp_tpa)[1], 
			((UCHAR*)&pIpRule->ExtraInfo.ARP_DATA.arp_tpa)[2], 
			((UCHAR*)&pIpRule->ExtraInfo.ARP_DATA.arp_tpa)[3], 
			pIpRule->ExtraInfo.ARP_DATA.arp_tha[0],
			pIpRule->ExtraInfo.ARP_DATA.arp_tha[1],
			pIpRule->ExtraInfo.ARP_DATA.arp_tha[2],
			pIpRule->ExtraInfo.ARP_DATA.arp_tha[3],
			pIpRule->ExtraInfo.ARP_DATA.arp_tha[4],
			pIpRule->ExtraInfo.ARP_DATA.arp_tha[5]);
		strLog += sMsg;

		sprintf_s( sMsg, 128, "%d\t", pIpRule->nCount );
		strLog += sMsg;

		sprintf_s( sMsg, 128, "%s\r\n", StringBlockReason[pIpRule->nAttackType] );
		strLog += sMsg;

        sprintf_s(sMsg, "%02d:%02d:%02d 拦截了%d次%s", 
            sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
            pIpRule->nCount,
            StringBlockReasonLite[pIpRule->nAttackType]);
        strLogLite += sMsg;

		m_cs.Enter();
        m_strLast = strLogLite;
		bResult = WriteFile( m_hLog, strLog.c_str(), (DWORD)strLog.length(), &dwWritten, NULL );
		m_cs.Leave();

	} while (FALSE);

	return bResult;
}

void CTxtLog::GetLastLog(std::string& strLog)
{
    m_cs.Enter();
    strLog = m_strLast;
    m_cs.Leave();
}

//////////////////////////////////////////////////////////////////////////

BOOL CTxtLog::IsSameModeArpAttack(INT nType)
{
	return  (nType == pa_arp_send_unexcept_arp_on_safe_mode ||
		nType == pa_arp_recv_unexcept_arp_on_safe_mode);
}
