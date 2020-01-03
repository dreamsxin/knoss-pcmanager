#include "StdAfx.h"
#include "KASAntiArpDevC.h"
#include "antiarp/arpcommondef.h"
#include "antiarp/antiarpioctls.h"
#include "antiarp/arpcomfunc.h"

KASAntiArpDevC::KASAntiArpDevC(void) :
    m_hDevcHandle(INVALID_HANDLE_VALUE),
    m_piCallback(NULL),
    m_hThread(NULL),
    m_hExitEvent(NULL)
{
    memset(&m_events, 0, sizeof(m_events));
}

KASAntiArpDevC::~KASAntiArpDevC(void)
{        
}

STDMETHODIMP_(BOOL) KASAntiArpDevC::InitDevC( )
{
    if (m_hDevcHandle != INVALID_HANDLE_VALUE)
        return TRUE;

    DWORD	DesiredAccess;
    DWORD	ShareMode;
    LPSECURITY_ATTRIBUTES	lpSecurityAttributes = NULL;

    DWORD	CreationDistribution;
    DWORD	FlagsAndAttributes;
    HANDLE	TemplateFile;
	DWORD   dwThread = NULL;

    BOOL	 bResult = FALSE;

    DesiredAccess = GENERIC_READ | GENERIC_WRITE;//GENERIC_READ|GENERIC_WRITE;
    ShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    CreationDistribution = OPEN_EXISTING;
    FlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
    TemplateFile = NULL;;
    lpSecurityAttributes = NULL;
    
    m_hDevcHandle = CreateFile(
        L"\\\\.\\"KNDISFLT_DEVICE_NAME ,
        DesiredAccess,
        ShareMode,
        lpSecurityAttributes,
        CreationDistribution,
        FlagsAndAttributes,
        TemplateFile
        );

    if( m_hDevcHandle == INVALID_HANDLE_VALUE || m_hDevcHandle == NULL )
    {
        m_hDevcHandle = CreateFile(
            L"\\\\.\\Global\\"KNDISFLT_DEVICE_NAME,
            DesiredAccess,
            ShareMode,
            lpSecurityAttributes,
            CreationDistribution,
            FlagsAndAttributes,
            TemplateFile
            );
    }
    
    if (m_hDevcHandle == NULL || m_hDevcHandle == INVALID_HANDLE_VALUE)
    {
		OutputDebugStringA( __FUNCTION__" Open Dirver Failed\n");
        goto Exit0;
    }

	OutputDebugStringA( __FUNCTION__" Open Dirver succeeded \n");
    
	m_events.m_hCommSemaphore = ::CreateSemaphore( NULL, 0, 1024*4, 0);
	if( m_events.m_hCommSemaphore == NULL )
		goto Exit0;

    m_hExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if( m_hExitEvent == NULL )
		goto Exit0;
	
	m_events.m_uMarVer = KFW_NIDS_MAJOR_VER;
	m_events.m_uMinVer = KFW_NIDS_MINOR_VER;
    if ( !RegisterEvent( &m_events ) )
		goto Exit0;
    
    m_hThread = CreateThread(NULL, 1024, ThreadLogIPFunc, this, 0, &dwThread);
    if (m_hThread == NULL)
        goto Exit0;

    return TRUE;

Exit0:

    UninitDevC();

    return FALSE;
}

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(VOID) KASAntiArpDevC::UninitDevC()
{
	OutputDebugStringA(__FUNCTION__);

    if (m_events.m_hCommSemaphore != NULL)
    {
        CloseHandle(m_events.m_hCommSemaphore);
        m_events.m_hCommSemaphore = NULL;
    }

    RegisterEvent(&m_events);

    if (m_hDevcHandle != NULL && m_hDevcHandle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hDevcHandle);
        m_hDevcHandle = INVALID_HANDLE_VALUE;
    }

    if (m_hExitEvent != NULL)
    {
        SetEvent(m_hExitEvent);
        CloseHandle(m_hExitEvent);
        m_hExitEvent = NULL;
    }

    if (m_hThread != NULL)
    {
        DWORD dwRet = WaitForSingleObject(m_hThread, 5000);
        if (dwRet != WAIT_OBJECT_0)
		{
			// ::DebugBreak();
            TerminateThread(m_hThread, 0);
		}

        CloseHandle(m_hThread);
        m_hThread = NULL;
    }
}

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(BOOL) KASAntiArpDevC::GetKfwState( ARP_STATE *kfwState )
{
	DWORD dwReturnBytes = 0;
	BOOL bResult = DeviceIoControl( m_hDevcHandle, IOCTL_ANTIARP_GET_VERSION, 
		0, 0,
		(LPVOID)kfwState, sizeof(ARP_STATE),
		&dwReturnBytes, 0 );

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(BOOL) KASAntiArpDevC::SetArpProtectList( p_mac_ip plist, int ncount )
{
    BOOL  bResult = FALSE;
	DWORD dwBuffLen = sizeof(DWORD);

    if (m_hDevcHandle == INVALID_HANDLE_VALUE)
        return bResult;

    if (plist == NULL || ncount <= 0)
        return bResult;
	
    bResult = DeviceIoControl(
        m_hDevcHandle,
        IOCTL_ANTIARP_SET_PROCTECT_MACIP_LIST,
        (PVOID)plist,
        min(ncount, max_filter_count) * sizeof(mac_ip),
        (PVOID)plist,
        min(ncount, max_filter_count) * sizeof(mac_ip),
        &dwBuffLen,
        NULL
        );

    return bResult;
}

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(BOOL) KASAntiArpDevC::ClearArpProtectList()
{
	BOOL  bResult = FALSE;
	DWORD dwRetCode;

	if (m_hDevcHandle == INVALID_HANDLE_VALUE)
		return bResult;

	bResult = DeviceIoControl(
		m_hDevcHandle,
		IOCTL_ANTIARP_CLEAR_PROCTECT_MACIP_LIST,
		0,
		0,
		0,
		0,
		&dwRetCode,
		NULL
		);

	return bResult;
}


//////////////////////////////////////////////////////////////////////////

BOOL KASAntiArpDevC::RegisterEvent( p_antiarp_config_event p_event )
{
    BOOL  bResult = FALSE;
	int   nValue    = 0;
	DWORD dwBuffLen = sizeof(DWORD);

    if (m_hDevcHandle == INVALID_HANDLE_VALUE)
        return bResult;

    if (p_event == NULL)
        return bResult;

    bResult = DeviceIoControl(
        m_hDevcHandle,
        IOCTL_ANTIARP_SET_CONFIG_EVENT,
        p_event,
        sizeof(antiarp_config_event),
        &nValue,
        dwBuffLen,
        &dwBuffLen,
        NULL
        );
    return bResult;
}

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(VOID) KASAntiArpDevC::SetCallback( IAntiArpCallback* piCallback )
{
    m_piCallback = piCallback;
}

//////////////////////////////////////////////////////////////////////////

DWORD WINAPI KASAntiArpDevC::ThreadLogIPFunc(LPVOID lpParam)
{
    KASAntiArpDevC* pThis = (KASAntiArpDevC*)lpParam;
    pThis->RunGetLogFunc();
    return 0;
}

//////////////////////////////////////////////////////////////////////////

VOID KASAntiArpDevC::RunGetLogFunc()
{
    int    nResult        = 0;
    BOOL   bResult        = 0;
    DWORD  dwResult       = 0;
    HANDLE hWaitEvents[3] = { 0 };
	ULONG uEventType;
	const ULONG uBufferSize = max( sizeof(LOG_IP_RULE), sizeof(RAS_CONNECTION) );
	UCHAR  pBuffer[ uBufferSize ];

	hWaitEvents[0] = m_hExitEvent;
	hWaitEvents[1] = m_events.m_hCommSemaphore;
	
    while (true)
    {
        dwResult = WaitForMultipleObjects( 2, hWaitEvents, FALSE, INFINITE );
        
        switch ( dwResult )
        {
        case WAIT_OBJECT_0 + 0:		// ExitEvent
			{
				return;
			}
			break;
        case WAIT_OBJECT_0 + 1:		// CommonEvent
			{
				if( m_piCallback == NULL )
					break;
				
				memset( (char*)pBuffer, 0, sizeof(pBuffer) );
				if( !GetCommonEvent( pBuffer, sizeof(pBuffer) ) )
					break;
				
				uEventType = *((ULONG*)pBuffer);
				switch( uEventType )
				{
				case enumIpLog:
					{
						m_piCallback->IpLog( (PLOG_IP_RULE)pBuffer );
					}
					break;
				case enumRasEvent:
					{
						m_piCallback->RasEvent( (PRAS_CONNECTION)pBuffer );
					}
					break;
				}
			}
			break;

        default:
            return;
        }
    }

}

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(BOOL) KASAntiArpDevC::AddIpRules(
	KIP_FILTER_INFO*    pIpFilterInfo,
	ULONG               dwFilterCount
	)
{
	BOOL bResult = FALSE;
	DWORD dwRetCode = 0;
	do 
	{
		if ( pIpFilterInfo == NULL || dwFilterCount == 0 )
			break;
		
		for ( ULONG i = 0; i < dwFilterCount; i++ )
		{
			bResult = ::DeviceIoControl(
				m_hDevcHandle,
				(DWORD)IOCTL_NDIS_ADD_IP_RULES,
				(PVOID)&pIpFilterInfo[i],
				sizeof(KIP_FILTER_INFO),
				NULL,
				0,
				&dwRetCode,
				NULL
				);
			if ( !bResult )
				break;
		}
	} while (FALSE);

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(BOOL) KASAntiArpDevC::RemoveIpRule( GUID ruleID )
{
	BOOL bResult = FALSE;
	ULONG dwRetCode = 0;

	if (INVALID_HANDLE_VALUE == m_hDevcHandle)
		return bResult;

	bResult = ::DeviceIoControl(
		m_hDevcHandle,
		(DWORD)IOCTL_NDIS_REMOVE_IP_RULES,
		&ruleID,
		sizeof(GUID),
		NULL,
		0,
		&dwRetCode,
		NULL
		);

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(BOOL) KASAntiArpDevC::RemoveAllIpRules()
{
	BOOL bResult = FALSE;
	ULONG dwRetCode = 0;

	if (INVALID_HANDLE_VALUE == m_hDevcHandle)
		return bResult;

	bResult = ::DeviceIoControl(
		m_hDevcHandle,
		(DWORD)IOCTL_NDIS_CLEAR_IP_RULES,
		NULL,
		0,
		NULL,
		0,
		&dwRetCode,
		NULL
		);

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(BOOL) KASAntiArpDevC::AddAdvFilter( __in KPackTemplate *pPackTemplate, ULONG uTemplateLen )
{
	BOOL bResult = FALSE;
	ULONG dwRetCode = 0;

	if ( INVALID_HANDLE_VALUE == m_hDevcHandle )
		return bResult;

	bResult = ::DeviceIoControl(
		m_hDevcHandle,
		(DWORD)IOCTL_NDIS_ADD_ADV_IP_RULES,
		pPackTemplate,
		uTemplateLen,
		NULL,
		0,
		&dwRetCode,
		NULL
		);

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(BOOL) KASAntiArpDevC::RemoveAdvFilter( GUID ruleId )
{
	BOOL bResult = FALSE;
	ULONG dwRetCode = 0;

	if ( INVALID_HANDLE_VALUE == m_hDevcHandle )
		return bResult;

	bResult = ::DeviceIoControl(
		m_hDevcHandle,
		(DWORD)IOCTL_NDIS_REMOVE_ADV_IP_RULES,
		(PVOID)&ruleId,
		sizeof(GUID),
		NULL,
		0,
		&dwRetCode,
		NULL
		);

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(BOOL) KASAntiArpDevC::RemoveAllAdvFilter()
{
	BOOL bResult = FALSE;
	ULONG dwRetCode = 0;

	if (INVALID_HANDLE_VALUE == m_hDevcHandle)
		return bResult;

	bResult = ::DeviceIoControl(
		m_hDevcHandle,
		(DWORD)IOCTL_NDIS_CLEAR_ADV_IP_RULES,
		NULL,
		0,
		NULL,
		0,
		&dwRetCode,
		NULL
		);

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

BOOL KASAntiArpDevC::GetCommonEvent( PVOID pBuffer, ULONG uBufferSize )
{
	BOOL  bResult = FALSE;
	ULONG dwRetCode = 0;

	if ( INVALID_HANDLE_VALUE == m_hDevcHandle || NULL == pBuffer || uBufferSize < sizeof(ULONG) )
		return bResult;

	bResult = ::DeviceIoControl(
		m_hDevcHandle,
		IOCTL_NDIS_GET_COMMON_EVENT,
		NULL,
		0,
		pBuffer,
		uBufferSize,
		&dwRetCode,
		NULL
		);

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(int) KASAntiArpDevC::GetAdapter( ADAPTER_INFO* pAdpaterList, int nCount )
{
	BOOL  bResult = FALSE;
	ULONG dwResult = 0;

	if( pAdpaterList == NULL || nCount == 0 || INVALID_HANDLE_VALUE == m_hDevcHandle  )
		return 0;

	bResult = ::DeviceIoControl(
		m_hDevcHandle,
		IOCTL_NDIS_ENUM_ADAPTER,
		NULL,
		0,
		(PVOID)pAdpaterList,
		nCount*sizeof(ADAPTER_INFO),
		&dwResult,
		NULL
		);

	return bResult ? dwResult / sizeof(ADAPTER_INFO) : 0;
}

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(BOOL) KASAntiArpDevC::SetModuleFlag( ULONG uFlag )
{
	BOOL  bResult = FALSE;
	ULONG dwResult = 0;

	if( INVALID_HANDLE_VALUE == m_hDevcHandle  )
		return 0;

	bResult = ::DeviceIoControl(
		m_hDevcHandle,
		IOCTL_NDIS_ENABLE_MODULE_FLAG,
		(PVOID)&uFlag,
		sizeof(ULONG),
		NULL,
		0,
		&dwResult,
		NULL
		);

	return bResult;
}


//////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(BOOL) KASAntiArpDevC::SetArpBlockFlag( ULONG uFlag )
{
	BOOL bResult = FALSE;
	DWORD dwRetCode = 0;

	if ( INVALID_HANDLE_VALUE == m_hDevcHandle  )
		return bResult;
	
	bResult = ::DeviceIoControl(
		m_hDevcHandle,
		IOCTL_NDIS_SET_BLOCK_FLAG,
		(PVOID)&uFlag,
		sizeof(PVOID),
		0,
		0,
		&dwRetCode,
		NULL
		);

	return bResult;
}
//////////////////////////////////////////////////////////////////////////


STDMETHODIMP_(BOOL) KASAntiArpDevC::SetLogFlag( ULONG uFlag )
{
	BOOL bResult = FALSE;
	DWORD dwRetCode = 0;

	if ( INVALID_HANDLE_VALUE == m_hDevcHandle  )
		return bResult;

	bResult = ::DeviceIoControl(
		m_hDevcHandle,
		IOCTL_NDIS_SET_LOG_FLAG,
		(PVOID)&uFlag,
		sizeof(ULONG),
		0,
		0,
		&dwRetCode,
		NULL
		);

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(BOOL) KASAntiArpDevC::EnableNetwork( __in BOOL bEnable )
{
	BOOL bResult = FALSE;
	DWORD dwRetCode = 0;

	if ( INVALID_HANDLE_VALUE == m_hDevcHandle  )
		return bResult;

	bResult = ::DeviceIoControl(
		m_hDevcHandle,
		bEnable ? IOCTL_NDIS_ENABLE_NETWORK : IOCTL_NDIS_DISABLE_NETWORK,
		0,
		0,
		0,
		0,
		&dwRetCode,
		NULL
		);

	return bResult;
}

STDMETHODIMP_(BOOL) KASAntiArpDevC::SetNotifyTimer( __in ULONG uTimer )
{
	BOOL bResult = FALSE;
	DWORD dwRetCode = 0;

	if ( INVALID_HANDLE_VALUE == m_hDevcHandle  )
		return bResult;

	bResult = ::DeviceIoControl(
		m_hDevcHandle,
		IOCTL_NDIS_SET_NOTIFY_TIMER,
		(PVOID)&uTimer,
		sizeof(LONG),
		0,
		0,
		&dwRetCode,
		NULL
		);

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(BOOL) KASAntiArpDevC::SetPassGuid( __in GUID guidPass )
{
	BOOL bResult = FALSE;
	DWORD dwRetCode = 0;

	if ( INVALID_HANDLE_VALUE == m_hDevcHandle  )
		return bResult;

	bResult = ::DeviceIoControl(
		m_hDevcHandle,
		IOCTL_NDIS_SET_PASS_GUID,
		(PVOID)&guidPass,
		sizeof(GUID),
		0,
		0,
		&dwRetCode,
		NULL
		);

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

STDMETHODIMP_(int) KASAntiArpDevC::GetRasConnection( __out PRAS_CONNECTION pRasConnection, __in int nCount )
{
	BOOL  bResult = FALSE;
	ULONG dwResult = 0;

	if( pRasConnection == NULL || nCount == 0 || INVALID_HANDLE_VALUE == m_hDevcHandle  )
		return 0;

	bResult = ::DeviceIoControl(
		m_hDevcHandle,
		IOCTL_NDIS_ENUM_RAS_CONNECTION,
		NULL,
		0,
		(PVOID)pRasConnection,
		nCount*sizeof(RAS_CONNECTION),
		&dwResult,
		NULL
		);

	return bResult ? dwResult / sizeof(RAS_CONNECTION) : 0;
}

STDMETHODIMP_(BOOL) KASAntiArpDevC::SelectGatewayMac()
{
    BOOL bResult = FALSE;
    DWORD dwRetCode = 0;

    if ( INVALID_HANDLE_VALUE == m_hDevcHandle  )
        return bResult;

    bResult = ::DeviceIoControl(
        m_hDevcHandle,
        IOCTL_NDIS_SELECT_GATEWAY_MAC,
        0,
        0,
        0,
        0,
        &dwRetCode,
        NULL
        );

    return bResult;
}

STDMETHODIMP_(BOOL) KASAntiArpDevC::CleanGatewayMac()
{
    BOOL bResult = FALSE;
    DWORD dwRetCode = 0;

    if ( INVALID_HANDLE_VALUE == m_hDevcHandle  )
        return bResult;

    bResult = ::DeviceIoControl(
        m_hDevcHandle,
        IOCTL_NDIS_CLEAN_GATEWAY_MAC,
        0,
        0,
        0,
        0,
        &dwRetCode,
        NULL
        );

    return bResult;
}

//////////////////////////////////////////////////////////////////////////
