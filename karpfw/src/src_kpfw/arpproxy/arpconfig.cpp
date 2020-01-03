#include "StdAfx.h"
#include "arpconfig.h"
#include "kseribuffer.h"

namespace KIS
{
	ULONG GetSize( ArpConfig &t )
	{
		ULONG uSize = 0;
		uSize += sizeof(ArpConfig);
		uSize += GetSize( t.m_pMacIpList );
		return uSize;
	}

	VOID Serialize( KSeriBuffer &seriBuffer, ArpConfig &t )
	{
		if(seriBuffer.Loading())
		{
			seriBuffer << t.m_nVer;
			seriBuffer << t.m_bEnable;
			seriBuffer << t.m_bSafeMode;
			seriBuffer << t.m_bBlockRecvAttack;
			seriBuffer << t.m_bBlockRecvIPAttack;
            seriBuffer << t.m_bBlockSendAttack;
			seriBuffer << t.m_bNotifyGw;
			seriBuffer << t.m_bAutoGate;
			seriBuffer << t.m_nTimes;
			seriBuffer << t.m_pMacIpList;
		}
		else
		{
			seriBuffer >> t.m_nVer;
			seriBuffer >> t.m_bEnable;
			seriBuffer >> t.m_bSafeMode;
			seriBuffer >> t.m_bBlockRecvAttack;
			seriBuffer >> t.m_bBlockRecvIPAttack;
            seriBuffer >> t.m_bBlockSendAttack;
			seriBuffer >> t.m_bNotifyGw;
			seriBuffer >> t.m_bAutoGate;
			seriBuffer >> t.m_nTimes;
			seriBuffer >> t.m_pMacIpList;
		}	
	}
};

ArpConfig::ArpConfig(void)
{
	m_nVer                  = 1;
	m_bEnable               = TRUE;
	m_bSafeMode             = FALSE;
	m_bBlockRecvAttack      = TRUE;
	m_bBlockRecvIPAttack    = TRUE;
    m_bBlockSendAttack      = TRUE;
	m_bNotifyGw             = FALSE;
	m_bAutoGate             = TRUE;
	m_nTimes                = 1;
}

ArpConfig::~ArpConfig(void)
{
}

BOOL ArpConfig::GetArpEnable()
{
	return m_bEnable;
}

BOOL ArpConfig::GetArpSafe()
{
	return m_bSafeMode;
}

BOOL ArpConfig::SetArpSafe(BOOL bSafe)
{
	m_bSafeMode = bSafe;
	return Save();
}

BOOL ArpConfig::GetArpSetting(BOOL& bBlockRecvAttack, BOOL& bBlockRecvIPAttack, BOOL& bBlockSendAttack)
{
    bBlockRecvAttack     = m_bBlockRecvAttack;
    bBlockRecvIPAttack   = m_bBlockRecvIPAttack;
    bBlockSendAttack     = m_bBlockSendAttack;

	return TRUE;
}

BOOL ArpConfig::SetArpSetting(BOOL bBlockRecvAttack, BOOL bBlockRecvIPAttack, BOOL bBlockSendAttack)
{
    m_bBlockRecvAttack   = bBlockRecvAttack;
    m_bBlockRecvIPAttack = bBlockRecvIPAttack;
    m_bBlockSendAttack   = bBlockSendAttack;

	return Save();
}

BOOL ArpConfig::GetManualGateAddr(mac_ip*& pAddr, INT& nCnt)
{
    if (m_pMacIpList.size())
        pAddr = new mac_ip[m_pMacIpList.size()];
	nCnt = 0;
	for( list<mac_ip>::iterator item = m_pMacIpList.begin(); item != m_pMacIpList.end(); item ++ )
	{
		pAddr[nCnt] = *item;
		nCnt++;
	}
	return nCnt != 0;
}

BOOL ArpConfig::SetManualGateAddr(mac_ip* pAddr, INT nCnt)
{
	m_pMacIpList.clear();
	for( int i=0; i<nCnt; i++ )
	{
		m_pMacIpList.push_back( pAddr[i] );
	}
	if( m_pMacIpList.size() == 0 )
		return FALSE;

	return Save();
}

BOOL ArpConfig::ClearArpProtectList()
{
	m_pMacIpList.clear();
	return Save();
}

BOOL ArpConfig::GetNotifyGateway(BOOL& bNotify, INT& nTimes)
{
	bNotify = m_bNotifyGw;
	nTimes = m_nTimes;
	return TRUE;
}

BOOL ArpConfig::SetNotifyGateway(BOOL bNotify, INT nTimes)
{
	m_bNotifyGw = bNotify;
	m_nTimes = nTimes;
	return Save();
}

BOOL ArpConfig::GetUseAutoGateAddr()
{
	return m_bAutoGate;
}

BOOL ArpConfig::SetUseAutoGateAddr(BOOL bEnable)
{
	m_bAutoGate = bEnable;
	return Save();
}

BOOL ArpConfig::SetArpEnable(BOOL bValue)
{
	m_bEnable = bValue;
	return Save();
}

BOOL ArpConfig::Load()
{
	BOOL bResult = FALSE;
	KIS::KSeriBuffer seriBuffer;
	DWORD dwSize = 0;
	DWORD dwRead = 0;
	
	WCHAR path[MAX_PATH + 1] = {0};
	::GetModuleFileName(NULL, path, MAX_PATH);

	*(wcsrchr( path, '\\' )) = NULL;
	wcscat_s( path, MAX_PATH, L"\\cfg" );
	CreateDirectory(path, NULL);
	wcscat_s( path, MAX_PATH, L"\\antiarp.cfg" );
	
	HANDLE hCfg = CreateFile( path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hCfg == INVALID_HANDLE_VALUE )
		goto Exit0;

	dwSize = GetFileSize( hCfg, NULL );
	if( dwSize == 0 || dwSize > 8192 )
		goto Exit0;

	seriBuffer.m_pBuffer.resize( dwSize );
	if( !ReadFile( hCfg, &seriBuffer.m_pBuffer[0], dwSize, &dwRead, NULL ) )
		goto Exit0;

	try
	{
		seriBuffer >> *this;
	}
	catch (...)
	{
		goto Exit0;
	}

	bResult = TRUE;

Exit0:

	if( hCfg != INVALID_HANDLE_VALUE )
	{
		CloseHandle( hCfg );
		hCfg = INVALID_HANDLE_VALUE;
	}

	return bResult;
}

BOOL ArpConfig::Save()
{
	BOOL bResult = FALSE;
	DWORD dwWrite = 0;
	KIS::KSeriBuffer seriBuffer;
	WCHAR path[MAX_PATH + 1] = {0};	
	HANDLE hCfg = INVALID_HANDLE_VALUE;

	try
	{
		seriBuffer << *this;
	}
	catch(...)
	{
		goto Exit0;
	}
	
	if( !seriBuffer.m_pBuffer.size() )
		goto Exit0;

	::GetModuleFileName(NULL, path, MAX_PATH);

	*(wcsrchr( path, '\\' )) = NULL;
	wcscat_s( path, MAX_PATH, L"\\cfg" );
	CreateDirectory(path, NULL);
	wcscat_s( path, MAX_PATH, L"\\antiarp.cfg" );

	hCfg = CreateFile( path, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hCfg == INVALID_HANDLE_VALUE )
		goto Exit0;

	if( !WriteFile( hCfg, &seriBuffer.m_pBuffer[0], (DWORD)seriBuffer.m_pBuffer.size(), &dwWrite, NULL) )
		goto Exit0;
	
	bResult = TRUE;

Exit0:

	if( hCfg != INVALID_HANDLE_VALUE )
	{
		CloseHandle( hCfg );
		hCfg = INVALID_HANDLE_VALUE;
	}

	return bResult;
}