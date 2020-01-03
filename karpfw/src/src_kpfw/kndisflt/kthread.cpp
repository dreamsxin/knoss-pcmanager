#include "pch.h"
#include "kthread.h"

BOOLEAN KWdmThread::Init( PVOID pThreadProc, PVOID pContext )
{
	HANDLE hThread = NULL;
	OBJECT_ATTRIBUTES oa; 
	
	m_bExit = FALSE;
	m_pThreadObj = NULL;
	KeInitializeEvent( &m_eventWait, SynchronizationEvent, FALSE );

	InitializeObjectAttributes( &oa, 0, OBJ_CASE_INSENSITIVE, 0, 0 );
	if( STATUS_SUCCESS == PsCreateSystemThread( &hThread, THREAD_ALL_ACCESS, 
					&oa, 0, 0, (PKSTART_ROUTINE)pThreadProc, pContext ) )
	{
		ObReferenceObjectByHandle( hThread, SYNCHRONIZE, NULL, KernelMode, &m_pThreadObj, NULL);
		ZwClose( hThread );
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KWdmThread::Uninit()
{
	if( m_pThreadObj == NULL )
		return FALSE;
	
	m_bExit = TRUE;
	KeSetEvent( &m_eventWait, IO_NO_INCREMENT, FALSE );
	KeWaitForSingleObject( m_pThreadObj, Executive, KernelMode, FALSE, NULL );
	ObDereferenceObject( m_pThreadObj );
	m_pThreadObj = NULL;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KWdmThread::Wait( ULONG uTimer )
{
	NTSTATUS status = STATUS_SUCCESS;
	LARGE_INTEGER waitTime;
	waitTime.QuadPart = -1l * 1000 *(1000 / 100);
	waitTime.QuadPart *= uTimer;

	status = KeWaitForSingleObject( &m_eventWait, Executive, KernelMode, FALSE, &waitTime );
	if( STATUS_TIMEOUT == status )
		return FALSE;
	
	return (BOOLEAN)m_bExit;
}

VOID KWdmThread::SetEvent()
{
	KeSetEvent( &m_eventWait, IO_NO_INCREMENT, FALSE );
}