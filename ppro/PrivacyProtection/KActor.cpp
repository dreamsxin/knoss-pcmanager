#include "KActor.h"
#include "KAVPublic.h"
#include <process.h>


#define	WAIT_THREAD_TIMEOUT				(5000LL)


KActor::KActor()
{
	m_uThreadID			= 0L;
	m_pThread			= NULL;
	m_hStopEvent		= NULL;
	m_pCallBack			= NULL;
}

KActor::~KActor()
{
	Stop();
}

HRESULT KActor::Startup(IActorCallBack* pCallBack)
{
	HRESULT lResult		= E_FAIL;
	HRESULT lRetCode	= E_FAIL;


	KAV_COM_PROCESS_ERROR(DeActivate());


	m_pCallBack = pCallBack;


	lRetCode = Activate();
	KAV_COM_PROCESS_ERROR(lRetCode);


	lResult = S_OK;

Exit0:

	if (FAILED(lResult))
	{
		Stop();
	}

	return lResult;
}

HRESULT KActor::Stop()
{
	return DeActivate();
}

bool KActor::IsActive()
{
	return (m_uThreadID == GetCurrentThreadId());
}

bool KActor::WaitForStop(DWORD dwMilliseconds)
{
	DWORD dwRet = ::WaitForSingleObject(m_hStopEvent, dwMilliseconds);
	return (dwRet == WAIT_OBJECT_0);
}

HRESULT KActor::Activate()
{
	HRESULT lResult	= E_FAIL;


	m_hStopEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	KAV_PROCESS_ERROR(m_hStopEvent);


	KAV_PROCESS_ERROR(NULL == m_pThread);
	m_pThread = (HANDLE)::_beginthreadex(NULL, 0, OnActivate, this, 0, &m_uThreadID);
	KAV_PROCESS_ERROR(m_pThread);


	lResult = S_OK;

Exit0:

	return lResult;
}

HRESULT KActor::DeActivate()
{
	HRESULT lResult	= E_FAIL;


	if (m_hStopEvent)
	{
		::SetEvent(m_hStopEvent);
	}


	while (m_pThread)
	{
		KAV_PROCESS_ERROR(m_uThreadID != GetCurrentThreadId());

		DWORD dwWaitCode = ::WaitForSingleObject(m_pThread, WAIT_THREAD_TIMEOUT);
		if (dwWaitCode == WAIT_TIMEOUT)
		{
			continue;
		}

		CloseHandle(m_pThread);
		m_pThread = NULL;
	}


	m_uThreadID	= 0L;
	if (NULL != m_hStopEvent)
	{
		::CloseHandle(m_hStopEvent);
		m_hStopEvent = NULL;
	}


	lResult = S_OK;

Exit0:

	return lResult;
}

unsigned __stdcall KActor::OnActivate(void* lParam)
{
	HRESULT lResult = E_FAIL;
	KActor* pThis	= (KActor*)lParam;


	KAV_PROCESS_ERROR(pThis);
	KAV_PROCESS_ERROR(pThis->m_pCallBack);
	
	pThis->m_pCallBack->OnActivate(pThis);


	lResult = S_OK;

Exit0:
	return lResult;
}
