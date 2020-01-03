#include "pch.h"
#include "keventsender.h"


BOOLEAN KEventSender::Init( PDRIVER_OBJECT /*DriverObject*/, PUNICODE_STRING /*RegistryPath*/)
{
	NTSTATUS status = STATUS_SUCCESS;

	m_CommonEventObj = NULL;
	m_commonEventList._Init();

	return status == STATUS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KEventSender::Uninit( PDRIVER_OBJECT /*DriverObject*/ )
{
	BOOLEAN bResult = FALSE;
	NTSTATUS status = STATUS_SUCCESS;
	
	KdPrint(("KEventSender::Uninit()\n"));
	
	__try
	{
		if( m_CommonEventObj != NULL )
		{
			ObDereferenceObject( m_CommonEventObj );
			m_CommonEventObj = NULL;
		}


		ClearEventList(&m_commonEventList, FALSE );
		
		bResult = (status == STATUS_SUCCESS);
	}
	__finally
	{
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KEventSender::RegisterEvent( HANDLE hCommonEvent )
{
	BOOLEAN bResult = FALSE;
	
	do 
	{
		if( m_CommonEventObj != NULL )
		{
			ObDereferenceObject( m_CommonEventObj );
			m_CommonEventObj = NULL;
		}
		
		if( hCommonEvent == NULL )
			break;

		if( STATUS_SUCCESS != ObReferenceObjectByHandle( hCommonEvent, SEMAPHORE_ALL_ACCESS, NULL,
						KernelMode, &m_CommonEventObj, NULL ) )
			break;

		KdPrint(("KEventSender::RegisterEvent: register.\n" ));
		bResult = TRUE;

	}while(FALSE);


	if( !bResult )
	{
		if( m_CommonEventObj != NULL )
		{
			ObDereferenceObject( m_CommonEventObj );
			m_CommonEventObj = NULL;
		}
	}
	return bResult;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KEventSender::SendCommonEvent( UCHAR *InBuffer,
	ULONG InBufferLen)
{

	BOOLEAN bResult = FALSE;
	RequestInfo *requestInfo = NULL;
	
	if ( m_commonEventList.size() >= MAX_COMMON_EVENT )
		return FALSE;
	
	__try
	{
		//此处申请的内存，在GetCommonEvent时释放
		if( m_CommonEventObj == NULL )
			__leave;

 		requestInfo = (RequestInfo *)non_paged_pool::__AllocateSystemMemory( sizeof(RequestInfo) );
		if( requestInfo == NULL )
			__leave;
		
		requestInfo->InBuffer = (UCHAR*)non_paged_pool::__AllocateSystemMemory( InBufferLen );
		if( requestInfo->InBuffer == NULL )
			__leave;
		
		requestInfo->uMagic = DefaultPoolTag;
		requestInfo->OutBuffer = NULL;
		requestInfo->OutBufferLen = 0;
		RtlCopyMemory( requestInfo->InBuffer, InBuffer, InBufferLen );
		requestInfo->InBufferLen = InBufferLen;
		
		if( !m_commonEventList.push_back( requestInfo ) )
			__leave;

		KeReleaseSemaphore( (PRKSEMAPHORE)m_CommonEventObj, IO_NO_INCREMENT, 1, FALSE);

		bResult = TRUE;
	}
	__finally
	{
		if ( !bResult )
		{
			if( requestInfo != NULL )
			{
				if( requestInfo->InBuffer != NULL )
					non_paged_pool::__DeallocateSystemMemory( (PVOID)requestInfo->InBuffer );

				non_paged_pool::__DeallocateSystemMemory( (PVOID)requestInfo );
			}
		}
	}

	return bResult;
}
 
//////////////////////////////////////////////////////////////////////////

BOOLEAN KEventSender::GetCommonEvent(
	OUT UCHAR *UserBuffer,
	IN ULONG UserBufferLen,
	OUT ULONG *uResultLen)
{
	BOOLEAN bResult = FALSE;
	RequestInfo *requestInfo = NULL;

	if ( UserBuffer != NULL && m_commonEventList.pop_front( &requestInfo ) )
	{
		if ( UserBufferLen >= requestInfo->InBufferLen )
		{
			RtlCopyMemory( UserBuffer, requestInfo->InBuffer, requestInfo->InBufferLen );
			*uResultLen = requestInfo->InBufferLen;
			bResult = TRUE;
		}

		if( requestInfo->InBuffer != NULL )
			non_paged_pool::__DeallocateSystemMemory( (PVOID)requestInfo->InBuffer );

		non_paged_pool::__DeallocateSystemMemory( (PVOID)requestInfo );
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

VOID KEventSender::ClearEventList( KDList<PVOID, RequestInfo*> *eventList, BOOLEAN bSetEvent )
{
	KIRQL oldirq;
	PVOID item = NULL;
	RequestInfo* requestInfo = NULL;
	
	if ( eventList == NULL )
		return ;

	eventList->lock( &oldirq );
	for ( item = eventList->begin(); item != eventList->end(); item = eventList->next(item) )
	{
		requestInfo = eventList->get_data( item );

		if ( bSetEvent )
			KeSetEvent( &requestInfo->pResponseEvent, 0, FALSE );
		else
			non_paged_pool::__DeallocateSystemMemory( (PVOID)requestInfo );
	}
	eventList->unlock( oldirq );
	eventList->_Uninit();
}

//////////////////////////////////////////////////////////////////////////
