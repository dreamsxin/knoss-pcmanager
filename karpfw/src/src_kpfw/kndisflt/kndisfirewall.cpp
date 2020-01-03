#include "pch.h"
#include "kndisfirewall.h"
#include "kndisfirewall.tmh"
//////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisFirewall::Init( PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath )
{
	BOOLEAN bResult = FALSE;
	
	m_bInit = FALSE;
	m_eventSender = NULL;
	m_arpFilter = NULL;
	m_advIpFilter = NULL;
	m_ipFilter = NULL;
	m_rasConnections = NULL;
	m_uAcquireList = 0;
	m_uEnableNetwork = TRUE;
	m_uLoadFlag = 0xffffffff;
	m_uRequestFlag = 0;
	m_uEnableFlag = 0;
	m_uArpBlockFlag = 0;
	m_devControl = NULL;
	m_NdisDeviceHandle = NULL;
	m_kfwServiceID = 0;
	
	LoadConfig( RegistryPath );

	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, "KNdisFirewall::Init++");
	
	do
	{
		if( !PrivateInit( DriverObject, RegistryPath ) )
			break;

		if( !CreateAndInit( m_eventSender, DriverObject, RegistryPath ) )
			break;

		if( !CreateAndInit( m_arpFilter, DriverObject, RegistryPath ) )
			break;

		if( !CreateAndInit( m_advIpFilter, DriverObject, RegistryPath ) )
			break;
	
		if( !CreateAndInit( m_ipFilter, DriverObject, RegistryPath ) )
			break;
		
		if( !CreateAndInit( m_rasConnections, DriverObject, RegistryPath ) )
			break;

		m_bInit = TRUE;
		bResult = TRUE;

	}while(FALSE);

	if( !bResult )
	{
		Uninit( DriverObject );
	}
	
	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, "KNdisFirewall::Init-- bResult:%x\n", bResult);

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisFirewall::Uninit( PDRIVER_OBJECT DriverObject )
{
	BOOLEAN bResult = FALSE;
	
	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, "KNdisFirewall::Uninit++");

	do
	{
		PrivateUninit( DriverObject );
		UninitAndDel( m_arpFilter, DriverObject );
		UninitAndDel( m_eventSender, DriverObject );
		UninitAndDel( m_advIpFilter, DriverObject );
		UninitAndDel( m_ipFilter, DriverObject );	
		UninitAndDel( m_rasConnections, DriverObject );	
		m_bInit = FALSE;
		bResult = TRUE;

	}while(FALSE);
	
	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, "KNdisFirewall::Uninit-- %x\n", bResult);
	return bResult;
}

//////////////////////////////////////////////////////////////////////////

template<class T>
BOOLEAN KNdisFirewall::CreateAndInit(T* &pInitT,  PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath )
{
	BOOLEAN bResult = FALSE;

	pInitT = new T;
	if( pInitT != NULL )
	{
		bResult = pInitT->Init( DriverObject, RegistryPath );
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

template<class T>
BOOLEAN KNdisFirewall::UninitAndDel(T* &pInitT,  PDRIVER_OBJECT DriverObject )
{
	BOOLEAN bResult = FALSE;

	if( pInitT != NULL )
	{
		bResult = pInitT->Uninit( DriverObject );
		delete pInitT;
		pInitT = NULL;
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisFirewall::PrivateInit( PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath )
{	
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING uStrDeviceName;
	UNICODE_STRING uStrDeviceLinkName;
	PDRIVER_DISPATCH    DispatchTable[IRP_MJ_MAXIMUM_FUNCTION+1];
	OBJECT_ATTRIBUTES oa;  

	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, "KNdisFirewall::PrivateInit++");

	do
	{
		NdisZeroMemory(DispatchTable, (IRP_MJ_MAXIMUM_FUNCTION+1) * sizeof(PDRIVER_DISPATCH));

		DispatchTable[IRP_MJ_CREATE] = DeviceDispatch;
		DispatchTable[IRP_MJ_CLEANUP] = DeviceDispatch;
		DispatchTable[IRP_MJ_CLOSE] = DeviceDispatch;
		DispatchTable[IRP_MJ_DEVICE_CONTROL] = DeviceDispatch;
		DispatchTable[IRP_MJ_INTERNAL_DEVICE_CONTROL] = DeviceDispatch;

		RtlInitUnicodeString( &uStrDeviceName, KNDISFLT_NT_DEVICE_NAME );
		RtlInitUnicodeString( &uStrDeviceLinkName, KNDISFLT_DOS_DEVICE_NAME );

		//
		// Create a device object and register our dispatch handlers
		//
		status = NdisMRegisterDevice(
			g_Data.NdisWrapperHandle, 
			&uStrDeviceName,
			&uStrDeviceLinkName,
			&DispatchTable[0],
			&m_devControl,
			&m_NdisDeviceHandle
			);
		
		if( status != STATUS_SUCCESS )
			break;
	
		m_devControl->Flags |= DO_BUFFERED_IO;

		InitializeObjectAttributes( &oa, 0, OBJ_CASE_INSENSITIVE, 0, 0 );
		m_threadGetAddr.Init( (PVOID)ThreadGetAddr, (PVOID)this );

		//日志计数后发送
		KeInitializeSpinLock( &m_cacheLogsLock );
		m_cacheLogIdx = 0;
		RtlZeroMemory( (char*)m_cacheLogs, sizeof(m_cacheLogs) );
		m_threadSendLogs.Init( (PVOID)ThreadSendLogs, (PVOID)this );

	}while(FALSE);
	
	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, "KNdisFirewall::PrivateInit-- %x\n", status );

	return status == STATUS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisFirewall::PrivateUninit( PDRIVER_OBJECT DriverObject )
{
	UNREFERENCED_PARAMETER( DriverObject );

	if (m_NdisDeviceHandle != NULL)
	{
		NdisMDeregisterDevice(m_NdisDeviceHandle);
		m_NdisDeviceHandle = NULL;
	}

	m_threadGetAddr.Uninit();
	m_threadSendLogs.Uninit();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS KNdisFirewall::DeviceDispatch (
	__in PDEVICE_OBJECT DeviceObject,
	__in PIRP irp 
	)
{
	NTSTATUS status = STATUS_ACCESS_DENIED;
	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, "KNdisFirewall::PrivateInit++");

	do 
	{
		if( g_ndisFirewall == NULL )
			break;

		if( !g_ndisFirewall->m_bInit )
			break;

		if( g_ndisFirewall->m_devControl != DeviceObject )
			break;
		
		status = g_ndisFirewall->HandleIRP( irp );
		
	}while(FALSE);

	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, "KNdisFirewall::PrivateInit--%x\n", status);
	return status;
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS KNdisFirewall::HandleIRP( __in PIRP irp )
{
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION pIrps = IoGetCurrentIrpStackLocation( irp );
	

	switch( pIrps->MajorFunction )
	{
	case IRP_MJ_CREATE:
		break;
	case IRP_MJ_CLOSE:
		{
			if( m_kfwServiceID == PsGetCurrentProcessId() )
			{
				if ( m_eventSender != NULL )
					m_eventSender->Uninit( NULL );
				InterlockedExchange( &m_uEnableFlag, 0 );
			}
		}

		break;
	case IRP_MJ_CLEANUP:
		break;
	case IRP_MJ_DEVICE_CONTROL:
		{
			ULONG uIoCtlCode = pIrps->Parameters.DeviceIoControl.IoControlCode;
			ULONG uInBufferLen = 0;
			ULONG uOutBufferLen = 0;
			UCHAR *pInBuffer = NULL;
			UCHAR *pOutBuffer = NULL;
			ULONG uResultLen = 0;

			uInBufferLen = pIrps->Parameters.DeviceIoControl.InputBufferLength;
			uOutBufferLen = pIrps->Parameters.DeviceIoControl.OutputBufferLength;

			if ( METHOD_NEITHER != IOCTL_TRANSFER_TYPE( uIoCtlCode ) )
			{
				pInBuffer = (UCHAR *)irp->AssociatedIrp.SystemBuffer;
				pOutBuffer = (UCHAR *)irp->AssociatedIrp.SystemBuffer;
			}

			status = ProcessRequest( irp, uIoCtlCode, pInBuffer, uInBufferLen, pOutBuffer, uOutBufferLen, &uResultLen );

			irp->IoStatus.Information = uResultLen;
		}
		break;
	}
	
	if( status != STATUS_PENDING )
	{
		irp->IoStatus.Status = status;
		IoCompleteRequest(irp, IO_NO_INCREMENT);
	}
	
	return status;
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS KNdisFirewall::ProcessRequest (
	PIRP irp,
	ULONG uIoCtlCode, 
	UCHAR *pInBuffer,
	ULONG uInBufferLen, 
	UCHAR *pOutBuffer,
	ULONG uOutBufferLen,
	ULONG *puResultLen
	)
{
	NTSTATUS status = STATUS_NOT_SUPPORTED;
	
	switch (uIoCtlCode)
	{
	case IOCTL_ANTIARP_GET_VERSION:	//获取状态
		{
			status = GetNdisState( pOutBuffer, uOutBufferLen, puResultLen );
		}
		break;

	case IOCTL_NDIS_ENABLE_MODULE_FLAG:
		{
			if( uInBufferLen == sizeof(LONG) )
			{
				InterlockedExchange( &m_uEnableFlag, *((LONG*)pInBuffer) );
				status = STATUS_SUCCESS;
			}
		}
		break;

	case IOCTL_NDIS_ENABLE_NETWORK:
		{
			InterlockedExchange( &m_uEnableNetwork, TRUE );
			status = STATUS_SUCCESS;
		}
		break;

	case IOCTL_NDIS_DISABLE_NETWORK:
		{
			InterlockedExchange( &m_uEnableNetwork, FALSE );
			status = STATUS_SUCCESS;
		}
		break;

	case IOCTL_NDIS_GET_COMMON_EVENT:
		{
			if( m_eventSender == NULL )
				break;

			if( !m_eventSender->GetCommonEvent( pOutBuffer,	uOutBufferLen , puResultLen ) )
				break;
			
			status = STATUS_SUCCESS;
		}
		break;
	
	case IOCTL_ANTIARP_SET_PROCTECT_MACIP_LIST:	//ARP过滤：设置IP和MAC表
		{
			if( m_arpFilter == NULL )
				break;

			status = m_arpFilter->BindAddr( pInBuffer, uInBufferLen );

			if( status == STATUS_SUCCESS )
			{
				BindMacForGateway();
			}
		}
		break;
	case IOCTL_ANTIARP_CLEAR_PROCTECT_MACIP_LIST:	//ARP过滤：设置IP和MAC表
		{
			if( m_arpFilter == NULL )
				break;

			UnBindMacForGateway();

			status = m_arpFilter->ClearBindAddr();
		}
		break;
	
	case IOCTL_ANTIARP_SET_CONFIG_EVENT:
		{
			antiarp_config_event* eventReg = (antiarp_config_event*)pInBuffer;

			if( m_eventSender == NULL || eventReg == NULL || uInBufferLen < sizeof(antiarp_config_event) )
				break;
			
			if( (eventReg->m_uMarVer != KFW_NIDS_MAJOR_VER)
				|| (eventReg->m_uMinVer != KFW_NIDS_MINOR_VER) )
				break;

			status = m_eventSender->RegisterEvent( eventReg->m_hCommSemaphore );

			if( NT_SUCCESS(status) )
			{
				m_kfwServiceID = PsGetCurrentProcessId();
			}
		}
		break;

	//高级包过滤
	case IOCTL_NDIS_ADD_ADV_IP_RULES:
		{
			if( m_advIpFilter == NULL )
				break;
			status = m_advIpFilter->AddIpRule( pInBuffer, uInBufferLen );
		}
		break;

	case IOCTL_NDIS_REMOVE_ADV_IP_RULES:
		{
			if( m_advIpFilter == NULL )
				break;
			status = m_advIpFilter->RemoveIpRule( pInBuffer, uInBufferLen );
		}
		break;

	case IOCTL_NDIS_CLEAR_ADV_IP_RULES:
		{
			if( m_advIpFilter == NULL )
				break;
			status = m_advIpFilter->ClearIpRule();
		}
		break;

	//IP过滤
	case IOCTL_NDIS_ADD_IP_RULES:
		{
			if( m_ipFilter == NULL )
				break;
			status = m_ipFilter->AddIpRule( pInBuffer, uInBufferLen );
		}
		break;

	case IOCTL_NDIS_REMOVE_IP_RULES:
		{
			if( m_ipFilter == NULL )
				break;
			status = m_ipFilter->RemoveIpRule( pInBuffer, uInBufferLen );
		}
		break;

	case IOCTL_NDIS_CLEAR_IP_RULES:
		{
			if( m_ipFilter == NULL )
				break;
			status = m_ipFilter->ClearIpRule();
		}
		break;
	case IOCTL_ANTIARP_SEND_ARP_PACKET:	//发送自定义数据包
		{
			//PIO_STACK_LOCATION  irpSp = IoGetCurrentIrpStackLocation( irp );
			//PVOID inBuf = irpSp->Parameters.DeviceIoControl.Type3InputBuffer;
			//PMDL mdl = NULL;

			//__try 
			//{
			//	ProbeForRead( inBuf, uInBufferLen, sizeof( UCHAR ) );
			//}
			//__except(EXCEPTION_EXECUTE_HANDLER)
			//{
			//	status = GetExceptionCode();
			//	break;
			//}


			//mdl = IoAllocateMdl(inBuf, uInBufferLen,  FALSE, TRUE, NULL);
			//if (!mdl)
			//{
			//	status = STATUS_INSUFFICIENT_RESOURCES;
			//	break;
			//}

			//__try
			//{MmMapLockedPages 
			//	MmProbeAndLockPages(mdl, UserMode, IoReadAccess);
			//}
			//__except( EXCEPTION_EXECUTE_HANDLER )
			//{

			//	status = GetExceptionCode();
			//	IoFreeMdl(mdl);
			//	break;
			//}
			//
			//status = SendUserPacket( irp, mdl );
			
		}
		break;
	case IOCTL_NDIS_SET_PASS_GUID:
		{

			if( uInBufferLen != sizeof(GUID) )
				break;

			if( m_ipFilter != NULL )
			{
				m_ipFilter->SetPassGuid( *((GUID*)pInBuffer) );
			}
			status = STATUS_SUCCESS;
		}
		break;
	case IOCTL_NDIS_ENUM_ADAPTER:	//获取网卡信息,状态
		{
			status = EnumAdapter( pOutBuffer, uOutBufferLen, puResultLen );
		}
		break;

	case IOCTL_NDIS_SET_BLOCK_FLAG:
		{
			if( m_arpFilter != NULL )
				status = m_arpFilter->SetBlockAttack( pInBuffer, uInBufferLen );
		}
		break;

	case IOCTL_NDIS_SET_LOG_FLAG:
		{
			if( uInBufferLen == sizeof(ULONG) )
			{
				m_uRequestFlag = *((ULONG*)pInBuffer);
				status = STATUS_SUCCESS;
			}
		}
		break;

	case IOCTL_NDIS_SET_NOTIFY_TIMER:
		{
			if( uInBufferLen == sizeof(ULONG) )
			{
				if( m_arpFilter != NULL )
					m_arpFilter->SetNotifyTimer( *((ULONG*)pInBuffer) );
				status = STATUS_SUCCESS;
			}
		}
		break;
	case IOCTL_NDIS_ENUM_RAS_CONNECTION:
		{
			if( m_rasConnections != NULL )
				status = m_rasConnections->EnumRasConnection( pOutBuffer, uOutBufferLen, puResultLen );
		}
		break;
	case IOCTL_NDIS_SELECT_GATEWAY_MAC:
		{
			status = SelectGatewayMac();
		}
		break;
	case IOCTL_NDIS_CLEAN_GATEWAY_MAC:
		{
			status = CleanGatewayMac();
		}
	}

	return status;
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS KNdisFirewall::GetNdisState( UCHAR *uOutBuffer, ULONG uBufferLen, ULONG *puResultLen )
{
	ARP_STATE *arpState = (ARP_STATE*)uOutBuffer;

	if ( arpState == NULL )
		return STATUS_INVALID_PARAMETER;

	if ( uBufferLen < sizeof(ARP_STATE) )
		return STATUS_INFO_LENGTH_MISMATCH;

	arpState->usMajorVer = KFW_NIDS_MAJOR_VER;
	arpState->usMinorVer = KFW_NIDS_MINOR_VER;
	arpState->uEnableNet = m_uEnableNetwork;
	arpState->uEnableFlag = m_uEnableFlag;
	arpState->uRequestFlag = m_uRequestFlag;
	arpState->uArpBlockFlag = (m_arpFilter == NULL) ? 0 : m_arpFilter->GetBlockFlag();
	//arpState->uTdiInterface = (m_ipFilter == NULL) ? 0 : m_ipFilter->GetTdiInterface();
	*puResultLen = sizeof(ARP_STATE);

	return STATUS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////

IPRESULT KNdisFirewall::Filter( PKANTIARP_ADAPTER adapter, PVOID pBuffer, ULONG uBufferLength, PACKET_DIR enumDir )
{
	IPRESULT bResult = enumIP_Unknow;

	do 
	{
		if( !m_uEnableNetwork )
		{
			bResult = enumIP_Block;
			break;
		}

		if( enumDir == enumPD_Send )
		{
			ExInterlockedAddLargeStatistic( &adapter->OutTraffic, uBufferLength );
		}
		else
		{
			ExInterlockedAddLargeStatistic( &adapter->InTraffic, uBufferLength );
		}

		if( m_rasConnections != NULL )
			m_rasConnections->PassThru( adapter, pBuffer, uBufferLength, enumDir );

		if( bResult == enumIP_Unknow && m_arpFilter != NULL )
			bResult = m_arpFilter->Filter( adapter, pBuffer, uBufferLength, enumDir );

		//if( bResult == enumIP_Unknow && m_dhcpFilter != NULL )
		//	bResult = m_dhcpFilter->Filter( adapter, pBuffer, uBufferLength, enumDir );

		if( bResult == enumIP_Unknow && m_ipFilter != NULL )
			bResult = m_ipFilter->Filter( adapter, pBuffer, uBufferLength, enumDir );
		
		if( bResult == enumIP_Unknow && m_advIpFilter != NULL )
			bResult = m_advIpFilter->Filter( adapter, pBuffer, uBufferLength, enumDir );

	} while (FALSE);

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

ULONG KNdisFirewall::ReadPacket( PNDIS_PACKET pPacket, PVOID pBuffer, ULONG size )
{
	PVOID           virtualAddress;
	PNDIS_BUFFER    firstBuffer, nextBuffer;
	UINT			len;
	ULONG           count = 0;

	NdisQueryPacket(pPacket, NULL, NULL, &firstBuffer, NULL);

	while (firstBuffer)
	{
		NdisQueryBufferSafe( firstBuffer, &virtualAddress, &len, NormalPagePriority );
		if( !virtualAddress || count + len > size )
			break;

		NdisMoveMemory( &((PUCHAR)pBuffer)[count], virtualAddress, len );
		count += len;

		NdisGetNextBuffer( firstBuffer,  &nextBuffer );
		firstBuffer = nextBuffer;
	}

	return count;
}

//////////////////////////////////////////////////////////////////////////

IPRESULT KNdisFirewall::FilterPackets(  PKANTIARP_ADAPTER adapter, PNDIS_PACKET localPacket, PACKET_DIR enumDir )
{
	IPRESULT bResult = enumIP_Unknow;
	UINT PacketSize;
	PVOID pBuffer = NULL;
	PETHHEADER pEthHdr = NULL;

	do 
	{
		if( !m_uEnableNetwork )
		{
			bResult = enumIP_Block;
			break;
		}

		NdisQueryPacket( localPacket, NULL, NULL, NULL, &PacketSize );
		if( PacketSize < sizeof(ETHHEADER) )
			break;

		pBuffer = non_paged_pool::__AllocateSystemMemory( PacketSize );
		if( pBuffer == NULL )
			break;

		ReadPacket( localPacket, pBuffer, PacketSize );
		
		bResult = Filter( adapter, pBuffer, PacketSize, enumDir );

	} while (FALSE);

	if( pBuffer != NULL )
	{
		non_paged_pool::__DeallocateSystemMemory( pBuffer );
		pBuffer = NULL;
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

//NDIS_STATUS KNdisFirewall::SendUserPacket( PIRP irp, PMDL pUserBuffer )
//{
//	PNDIS_PACKET  pNdisPacket;
//	NDIS_STATUS   Status = NDIS_STATUS_SUCCESS;
//	PLIST_ENTRY pList = NULL;
//	PKANTIARP_ADAPTER pAdapter = NULL;
//	PKANTIARP_SEND_CONTEXT pSendContext = NULL;
//
//	for( pList = g_Data.AdapterList.Flink; pList != &g_Data.AdapterList; pList = pList->Flink )
//	{
//		pAdapter = (PKANTIARP_ADAPTER)pList;
//		
//		if( !KAntiarpAcquireMiniportLock( pAdapter ) )
//			continue;
//			
//		NdisAllocatePacket( &Status, &pNdisPacket, pAdapter->SendPacketPool ); 
//		if ( Status != NDIS_STATUS_SUCCESS ) 
//			continue;
//		
//		IoMarkIrpPending( irp );
//		NDIS_SET_PACKET_STATUS( pNdisPacket,NDIS_STATUS_PENDING );
//
//		//e1e6032.sys蓝屏
//		memset((char*)pNdisPacket + pNdisPacket->Private.NdisPacketOobOffset,0,sizeof(NDIS_PACKET_OOB_DATA));
//		{ 
//			PULONG pUnknown=(PULONG)((char*)pNdisPacket+pNdisPacket->Private.NdisPacketOobOffset+sizeof(NDIS_PACKET_OOB_DATA));
//			if ( pUnknown!=NULL )
//			{ 
//				*pUnknown&=0xFFFFFFe0; 
//				//NdisGetPacketFlags(pNdisPacket)&=0xFFFFFFe0;
//			}
//		}
//
//		NdisChainBufferAtFront( pNdisPacket, pUserBuffer );
//		NdisSetPacketFlags( pNdisPacket, NDIS_FLAGS_DONT_LOOPBACK );
//		pSendContext = (PKANTIARP_SEND_CONTEXT)pNdisPacket->ProtocolReserved;
//		pSendContext->bNeedFree = TRUE;
//		pSendContext->OriginalPacket = NULL;
//		pSendContext->pIrp = irp;
//		
//		Status = STATUS_PENDING;
//
//		NdisSendPackets( pAdapter->BindingHandle, &pNdisPacket, 1 );
//
//	}
//	
//	return Status;
//}

//////////////////////////////////////////////////////////////////////////

NDIS_STATUS KNdisFirewall::SendUserPacket( PKANTIARP_ADAPTER pAdapter, PVOID pBuffer, ULONG uBufferLength )
{
	NDIS_STATUS   Status = NDIS_STATUS_SUCCESS;
	PNDIS_PACKET  pNdisPacket = NULL;
	PLIST_ENTRY pList = NULL;
	PSEND_RSVD pSendContext = NULL;
	PNDIS_BUFFER mdl;
	
	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, "KNdisFirewall::SendUserPacket++");

	do 
	{
		if( pAdapter->MiniportPowerState > NdisDeviceStateD0 )
		{
			Status = NDIS_STATUS_FAILURE;
			break;
		}

		NdisAcquireSpinLock(&pAdapter->Lock);
		if (pAdapter->PTDeviceState > NdisDeviceStateD0)
		{
			NdisReleaseSpinLock(&pAdapter->Lock);
			Status = NDIS_STATUS_FAILURE;
			break;
		}
		pAdapter->OutstandingSends++;
		NdisReleaseSpinLock(&pAdapter->Lock);
		
		NdisAllocatePacket( &Status, &pNdisPacket, pAdapter->SendPacketPoolHandle ); 
		if ( Status != NDIS_STATUS_SUCCESS ) 
			break;
		
		NdisAllocateBuffer( &Status, &mdl, pAdapter->hSendBufferPool, pBuffer, uBufferLength );
		if ( Status != NDIS_STATUS_SUCCESS ) 
		{
			NdisFreePacket( pNdisPacket );
			pNdisPacket = NULL;
			break;
		}

		NDIS_SET_PACKET_STATUS( pNdisPacket,NDIS_STATUS_PENDING );
		//e1e6032.sys蓝屏
		RtlZeroMemory( (char*)pNdisPacket + pNdisPacket->Private.NdisPacketOobOffset, sizeof(NDIS_PACKET_OOB_DATA) );
		{ 
			PULONG pUnknown = (PULONG)((char*)pNdisPacket+pNdisPacket->Private.NdisPacketOobOffset+sizeof(NDIS_PACKET_OOB_DATA));
			if ( pUnknown != NULL )
			{ 
				*pUnknown &= 0xFFFFFFe0;
				//NdisGetPacketFlags(pNdisPacket)&=0xFFFFFFe0;
			}
		}

		NdisChainBufferAtFront( pNdisPacket, mdl );
		NdisSetPacketFlags( pNdisPacket, NDIS_FLAGS_DONT_LOOPBACK );
		pSendContext = (PSEND_RSVD)pNdisPacket->ProtocolReserved;
		pSendContext->bNeedFree = TRUE;
		pSendContext->OriginalPkt = NULL;

		Status = NDIS_STATUS_PENDING;

		NdisSendPackets( pAdapter->BindingHandle, &pNdisPacket, 1 );
		
	} while (FALSE);

	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, "KNdisFirewall::SendUserPacket--");

	return Status;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisFirewall::GetAdapterMac( PKANTIARP_ADAPTER pAdapter )
{
	NDIS_REQUEST requestInfo = {0};
	NDIS_STATUS Status;
	BOOLEAN bResult = FALSE;
	ULONG BytesNeeded = 0;
	ULONG BytesReadOrWritten = 0;

	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, "KNdisFirewall::GetAdapterMac++");

	if( pAdapter->Medium == NdisMedium802_3 )
	{
		pAdapter->BytesNeeded = &BytesNeeded;
		pAdapter->BytesReadOrWritten = &BytesReadOrWritten;
		NdisAcquireSpinLock(&pAdapter->Lock);
		pAdapter->OutstandingRequests = TRUE;
		NdisReleaseSpinLock(&pAdapter->Lock);

		requestInfo.RequestType = NdisRequestQueryInformation;
		requestInfo.DATA.QUERY_INFORMATION.Oid = OID_802_3_CURRENT_ADDRESS;
		requestInfo.DATA.QUERY_INFORMATION.InformationBuffer = (PVOID)&pAdapter->LocalMac;
		requestInfo.DATA.QUERY_INFORMATION.InformationBufferLength = 6;

		NdisRequest( &Status, pAdapter->BindingHandle, (PNDIS_REQUEST)&requestInfo );
		if ( Status == NDIS_STATUS_PENDING ) 
		{
			NdisWaitEvent( &pAdapter->hQueryEvent, 0 );
			NdisResetEvent( &pAdapter->hQueryEvent );
		}
		else
		{
			NdisAcquireSpinLock(&pAdapter->Lock);
			pAdapter->OutstandingRequests = FALSE;
			NdisReleaseSpinLock(&pAdapter->Lock);
		}

		KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, "KNdisFirewall::GetAdapterMac %02X-%02X-%02X-%02X-%02X-%02X",
			pAdapter->LocalMac[0],
			pAdapter->LocalMac[1],
			pAdapter->LocalMac[2],
			pAdapter->LocalMac[3],
			pAdapter->LocalMac[4],
			pAdapter->LocalMac[5]);

		bResult = TRUE;
	}
	

	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, "KNdisFirewall::GetAdapterMac--");
	return bResult;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisFirewall::GetAdapterPhysicalMedium( PKANTIARP_ADAPTER pAdapter )
{
	NDIS_REQUEST requestInfo = {0};
	NDIS_STATUS Status;
	BOOLEAN bResult = FALSE;
	NDIS_PHYSICAL_MEDIUM physicalMedium = NdisPhysicalMediumUnspecified; 
	ULONG BytesNeeded = 0;
	ULONG BytesReadOrWritten = 0;

	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, "KNdisFirewall::GetAdapterPhysicalMedium++");
	
	pAdapter->BytesNeeded = &BytesNeeded;
	pAdapter->BytesReadOrWritten = &BytesReadOrWritten;

	NdisAcquireSpinLock(&pAdapter->Lock);
	pAdapter->OutstandingRequests = TRUE;
	NdisReleaseSpinLock(&pAdapter->Lock);

	requestInfo.RequestType = NdisRequestQueryInformation;
	requestInfo.DATA.QUERY_INFORMATION.Oid = OID_GEN_PHYSICAL_MEDIUM;
	requestInfo.DATA.QUERY_INFORMATION.InformationBuffer = (PVOID)&physicalMedium;
	requestInfo.DATA.QUERY_INFORMATION.InformationBufferLength = sizeof(NDIS_PHYSICAL_MEDIUM);

	NdisRequest( &Status, pAdapter->BindingHandle, (PNDIS_REQUEST)&requestInfo );
	if ( Status == NDIS_STATUS_PENDING ) 
	{
		NdisWaitEvent( &pAdapter->hQueryEvent, 0 );
		NdisResetEvent( &pAdapter->hQueryEvent );
	}
	else
	{
		NdisAcquireSpinLock(&pAdapter->Lock);
		pAdapter->OutstandingRequests = FALSE;
		NdisReleaseSpinLock(&pAdapter->Lock);
	}

	pAdapter->physicalMedium = physicalMedium;
	bResult = TRUE;
	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, "KNdisFirewall::GetAdapterPhysicalMedium-- %0X", physicalMedium );
	return bResult;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisFirewall::GetAdapterStatus( PKANTIARP_ADAPTER pAdapter )
{
	NDIS_REQUEST requestInfo = {0};
	NDIS_STATUS Status;
	BOOLEAN bResult = FALSE;
	NDIS_MEDIA_STATE mediaState = NdisMediaStateDisconnected; 
	ULONG BytesNeeded = 0;
	ULONG BytesReadOrWritten = 0;

	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, "KNdisFirewall::GetAdapterStatus++");

	pAdapter->BytesNeeded = &BytesNeeded;
	pAdapter->BytesReadOrWritten = &BytesReadOrWritten;

	NdisAcquireSpinLock(&pAdapter->Lock);
	pAdapter->OutstandingRequests = TRUE;
	NdisReleaseSpinLock(&pAdapter->Lock);

	requestInfo.RequestType = NdisRequestQueryInformation;
	requestInfo.DATA.QUERY_INFORMATION.Oid = OID_GEN_MEDIA_CONNECT_STATUS;
	requestInfo.DATA.QUERY_INFORMATION.InformationBuffer = (PVOID)&mediaState;
	requestInfo.DATA.QUERY_INFORMATION.InformationBufferLength = sizeof(NDIS_MEDIA_STATE);

	NdisRequest( &Status, pAdapter->BindingHandle, (PNDIS_REQUEST)&requestInfo );
	if ( Status == NDIS_STATUS_PENDING ) 
	{
		NdisWaitEvent( &pAdapter->hQueryEvent, 0 );
		NdisResetEvent( &pAdapter->hQueryEvent );
	}
	else
	{
		NdisAcquireSpinLock(&pAdapter->Lock);
		pAdapter->OutstandingRequests = FALSE;
		NdisReleaseSpinLock(&pAdapter->Lock);
	}

	if( mediaState == NdisMediaStateDisconnected )
	{
		pAdapter->LastIndicatedStatus = NDIS_STATUS_MEDIA_DISCONNECT;
	}
	bResult = TRUE;

	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, "KNdisFirewall::GetAdapterStatus--:%0X", mediaState );
	return bResult;
}

//////////////////////////////////////////////////////////////////////////

VOID KNdisFirewall::SetAdapterIp( PKANTIARP_ADAPTER adapter, ULONG *uLocalIP, ULONG uGatewayIP )
{
	PLIST_ENTRY pList = NULL;
	int i = 0;
	//KAntiarpDebugPrint(DBG_KFW, DBG_TRACE, __FUNCTION__"++");
	
	RtlCopyMemory( (char*)adapter->LocalIP, (char*)uLocalIP, sizeof(ULONG)*MAX_ADAPTER_IP_NUM );
	
	if( adapter->GatewayIP != uGatewayIP )
	{
		adapter->GatewayIP = uGatewayIP;
		RtlZeroMemory( adapter->GatewayMac, 6 );

		KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, "KNdisFirewall::SetAdapterIp LocalIP:%08X  GatewayIP:%08X  %02X-%02X-%02X-%02X-%02X-%02X",
			uLocalIP[0],
			adapter->GatewayIP,
			adapter->GatewayMac[0],
			adapter->GatewayMac[1],
			adapter->GatewayMac[2],
			adapter->GatewayMac[3],
			adapter->GatewayMac[4],
			adapter->GatewayMac[5] );

		if( uGatewayIP != 0 && m_arpFilter != 0)
		{
			m_arpFilter->GetProtectMac( uGatewayIP, adapter->GatewayMac );
		}
	}
	//KAntiarpDebugPrint(DBG_KFW, DBG_TRACE, __FUNCTION__"--");

}

//////////////////////////////////////////////////////////////////////////

IPRESULT KNdisFirewall::IsGateway( PKANTIARP_ADAPTER adapter, ULONG uIP, UCHAR* pMac )
{
	const UCHAR szBroadcastAddr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

	if( adapter->GatewayIP != 0 && adapter->GatewayIP == uIP )
	{
		//MAC和网关相同, 网关的MAC通过"DHCP解析"或"用户自定义"获取
		if( RtlCompareMemory( adapter->GatewayMac, "\0\0\0\0\0\0\0", 6 ) == 6 ) 
		{
			return enumIP_Pass;
		}

		//MAC和网关相同, 网关的MAC通过"DHCP解析"或"用户自定义"获取
		if( RtlCompareMemory( adapter->GatewayMac, pMac, 6 ) == 6 ) 
		{
			return enumIP_Pass;
		}
		
		//请求网关的MAC
		if( RtlCompareMemory( pMac, "\0\0\0\0\0\0\0", 6 ) == 6 )
		{
			return 	enumIP_Pass;
		}

		//请求网关的MAC modified for 360safe
		if( RtlCompareMemory( pMac, szBroadcastAddr, 6 ) == 6 )
		{
			return 	enumIP_Pass;
		}

		//网关IP一样，但是MAC不一样. 有可能本机伪造了网关的ARP回复包，修改目标机器网关MAC(或造成网关IP冲突)，达到攻击目的
		return enumIP_Block;
	}

	return enumIP_Unknow;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisFirewall::CompMacIp( PKANTIARP_ADAPTER adapter, ULONG uIP, UCHAR *pMac )
{
	BOOLEAN bResult = FALSE;
	int i = 0;

	while( RtlCompareMemory( pMac, adapter->LocalMac, 6 ) == 6 )
	{
		if ( uIP == 0 || (adapter->LocalIP[0] == 0 ) )
		{
			bResult  = TRUE;
			break;
		}

		for( i = 0; i<MAX_ADAPTER_IP_NUM; i++)
		{
			if( adapter->LocalIP[i] == uIP )
			{
				bResult = TRUE;
				break;
			}

			if( adapter->LocalIP[i] == 0 && ( uIP & 0xffff ) == 0xFEA9 ) 		//169.254.x.x 特殊处理
			{
				bResult = TRUE;
				break;
			}
		}
		break;
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisFirewall::CompIpMac( PKANTIARP_ADAPTER adapter, ULONG uIP, UCHAR *pMac )
{
	BOOLEAN bResult = TRUE;
	int i = 0;

	for( i = 0; i<MAX_ADAPTER_IP_NUM; i++ )
	{
		if ( adapter->LocalIP[i] == 0 )
			continue;

		if ( uIP == adapter->LocalIP[i] )
		{
			if( RtlCompareMemory( pMac, adapter->LocalMac, 6 ) != 6 )
				bResult = FALSE;
		}
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisFirewall::IncRecvArp( PKANTIARP_ADAPTER adapter, ULONG uIP,  UCHAR* pMac )
{
	BOOLEAN bResult = FALSE;
	int i = 0;

	do 
	{
		if( adapter->GatewayIP == 0 || adapter->GatewayIP != uIP )
			break;

		//已经获取到网关
		if( RtlCompareMemory( adapter->GatewayMac, "\0\0\0\0\0\0\0", 6 ) != 6 )
			break;
		
		if( adapter->arpCache == NULL )
			break;

		for( i=0; i<MAX_ARP_CACHE; i++ )
		{
			if( RtlCompareMemory( adapter->arpCache[i].GatewayMac, "\0\0\0\0\0\0\0", 6 ) == 6 )
			{
				RtlCopyMemory( adapter->arpCache[i].GatewayMac, pMac, 6 );
				InterlockedExchange( &adapter->arpCache[i].nRecvCount, 0 );
				break;
			}

			if( RtlCompareMemory( adapter->arpCache[i].GatewayMac, pMac, 6 ) == 6 )
			{
				InterlockedIncrement( &adapter->arpCache[i].nRecvCount );
				break;
			}
		}
		
		if( i != MAX_ARP_CACHE )
		{
			KAntiarpDebugPrint( TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, 
				"KNdisFirewall::IncRecvArp CacheIdx:%d GatewayIP:%08X  %02X-%02X-%02X-%02X-%02X-%02X Count:%d",
				i,
				adapter->GatewayIP,
				adapter->arpCache[i].GatewayMac[0],
				adapter->arpCache[i].GatewayMac[1],
				adapter->arpCache[i].GatewayMac[2],
				adapter->arpCache[i].GatewayMac[3],
				adapter->arpCache[i].GatewayMac[4],
				adapter->arpCache[i].GatewayMac[5],
				adapter->arpCache[i].nRecvCount
				);
		}
		else
		{
			for( i=0; i<MAX_ARP_CACHE; i++ )
			{
				if( adapter->arpCache[i].nRecvCount < 0 )
				RtlZeroMemory( (char*)adapter->arpCache[i].GatewayMac, sizeof(ARP_CACHE) );
			}
		}
		
		bResult = TRUE;
	} while (FALSE);

	return bResult;
}

//////////////////////////////////////////////////////////////////////////

VOID KNdisFirewall::DecRecvArp( PKANTIARP_ADAPTER adapter )
{
	int i = 0;
	for( i=0; i<MAX_ARP_CACHE; i++ )
	{
		if( RtlCompareMemory( adapter->arpCache[i].GatewayMac, "\0\0\0\0\0\0\0", 6 ) == 6 )
			break;
		
		InterlockedDecrement( &adapter->arpCache[i].nRecvCount );
	}

	KAntiarpDebugPrint( TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, "KNdisFirewall::DecRecvArp %d", i );
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS KNdisFirewall::EnumAdapter( PVOID pBuffer, ULONG uBufferLength, ULONG *puResultLen )
{
	NTSTATUS status = STATUS_SUCCESS;
	PKANTIARP_ADAPTER pAdapter = NULL;
	int i = 0;
	int nCount = uBufferLength / sizeof(ADAPTER_INFO);
	PADAPTER_INFO pAdapterInfo = (PADAPTER_INFO)pBuffer;

	if( pBuffer == NULL ||  uBufferLength == 0 || puResultLen == NULL || nCount == 0 )
		return STATUS_INVALID_PARAMETER;
	
	*puResultLen = 0;
	
	NdisAcquireSpinLock(&g_Data.GlobalLock);
	for( pAdapter = g_Data.AdapterList; pAdapter != NULL; pAdapter = pAdapter->Next )
	{
		if( i >= nCount )
		{
			status = STATUS_INFO_LENGTH_MISMATCH;
			break;
		}

		RtlCopyMemory( pAdapterInfo[i].wsDeviceName, pAdapter->UpperDeviceName.Buffer, min(MAX_ADAPTER_NAME_LENGTH + sizeof(WCHAR), pAdapter->UpperDeviceName.Length + sizeof(WCHAR)) ); 	
		pAdapterInfo[i].uGatewagIP = pAdapter->GatewayIP;
		RtlCopyMemory( pAdapterInfo[i].uGatewagMac, pAdapter->GatewayMac, 6 );
		RtlCopyMemory( (char*)pAdapterInfo[i].uLocalIP, pAdapter->LocalIP, MAX_ADAPTER_IP_NUM * sizeof(ULONG) );
		RtlCopyMemory( pAdapterInfo[i].uLocalMac, pAdapter->LocalMac, 6 );
		pAdapterInfo[i].uIsConnect = ( pAdapter->LastIndicatedStatus == NDIS_STATUS_MEDIA_CONNECT );
		pAdapterInfo[i].enumMediumType = pAdapter->Medium;
		pAdapterInfo[i].physicalMedium = pAdapter->physicalMedium;
		pAdapterInfo[i].inTraffic = pAdapter->InTraffic.QuadPart;
		pAdapterInfo[i].outTraffic = pAdapter->OutTraffic.QuadPart;
		i++;
	}
	NdisReleaseSpinLock(&g_Data.GlobalLock);
	*puResultLen = i * sizeof(ADAPTER_INFO);

	return status;
}


//////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisFirewall::SendArpLog( PKANTIARP_ADAPTER adapter, PARPPACKET pArpPack, PacketAttackType blockReason, PACKET_DIR enumDir )
{
	LOG_IP_RULE logIpRule;
	if( adapter == NULL || pArpPack == NULL || m_eventSender == NULL )
		return FALSE;
	
	if( !(m_uRequestFlag & REQUEST_ARP_LOG ) )
		return FALSE;

	RtlZeroMemory( (char*)&logIpRule, sizeof(LOG_IP_RULE) );
	
	logIpRule.id = ArpFilterGuid;
	logIpRule.byDirection = enumDir;
	logIpRule.nAttackType = blockReason;
	logIpRule.nOperation = enumIP_Block;
	logIpRule.ExtraInfo.ARP_DATA.gateway_tpa = adapter->GatewayIP;
	RtlCopyMemory( (UCHAR*)&logIpRule.ExtraInfo.ARP_DATA.gateway_tha, adapter->GatewayMac, 6 );
	RtlCopyMemory( (UCHAR*)&logIpRule.ExtraInfo.ARP_DATA.local_tpa, adapter->LocalIP, sizeof(ULONG)*MAX_ADAPTER_IP_NUM );
	RtlCopyMemory( (UCHAR*)&logIpRule.ExtraInfo.ARP_DATA.local_tha, adapter->LocalMac, 6 );
	RtlCopyMemory( (UCHAR*)&logIpRule.ExtraInfo.ARP_DATA.eh_dst, (UCHAR*)&pArpPack->ehhdr.eh_dst, 6 );
	RtlCopyMemory( (UCHAR*)&logIpRule.ExtraInfo.ARP_DATA.arp_sha, (UCHAR*)&pArpPack->arphdr.arp_sha, 6 * 2 + 4 *2 );
	CacheLogs( &logIpRule );
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisFirewall::SendIpLog( LOG_IP_RULE *ipLog )
{
	if( ipLog == NULL )
		return FALSE;

	if( !(m_uRequestFlag & REQUEST_IP_LOG ) )
		return FALSE;

	CacheLogs( ipLog );
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisFirewall::SendAdvIpLog( LOG_IP_RULE *ipLog )
{
	if( ipLog == NULL )
		return FALSE;

	if( !(m_uRequestFlag & REQUEST_ADV_IP_LOG ) )
		return FALSE;

	CacheLogs( ipLog );
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS KNdisFirewall::GetIpFromReg( PUNICODE_STRING pDeviceName, ULONG *pLocalIp, ULONG *pGatewayIP )
{
	NTSTATUS ntRetCode = STATUS_UNSUCCESSFUL;
	
	WCHAR REG_DEVICE_PATH[128] = L"\\REGISTRY\\MACHINE\\SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
	const WCHAR EnableDhcp[] = L"EnableDHCP";
	const WCHAR DhcpIPAddress[] = L"DhcpIPAddress";
	const WCHAR DhcpDefaultGateway[] = L"DhcpDefaultGateway";
	const WCHAR IPAddress[] = L"IPAddress";
	const WCHAR DefaultGateway[] = L"DefaultGateway";
	const WCHAR IPAutoconfigurationAddress[] = L"IPAutoconfigurationAddress";
	UNICODE_STRING usRegPath;
	OBJECT_ATTRIBUTES ObjectAttributes;
	HANDLE hKey = NULL;

	ULONG uValueType = 0;
	ULONG uEnableDhcp = 0;
	ULONG uRetLength = 0;
	ULONG uIPAddr = 0;
	ULONG uLocalIpCount = 0;

	WCHAR szLocalIp[64] = {0};
	WCHAR szGatewayIp[64] = {0};
	WCHAR szIPAutoconfigurationAddress[64] = {0};
	
	if( pDeviceName == NULL || pLocalIp == NULL || pGatewayIP == NULL )
		return ntRetCode;

	RtlStringCchCatW( REG_DEVICE_PATH, 128, &pDeviceName->Buffer[8] );	//Skip "\\Device\\"
	RtlInitUnicodeString( &usRegPath, REG_DEVICE_PATH );
	
	InitializeObjectAttributes( &ObjectAttributes, &usRegPath, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL );

	do 
	{
		ntRetCode = ZwOpenKey( &hKey, KEY_ALL_ACCESS, &ObjectAttributes );
		if ( NT_SUCCESS(ntRetCode) == FALSE )
			break;
		
		ntRetCode = GetRegistryKeyValue( hKey, (PWCHAR)EnableDhcp, sizeof(EnableDhcp), 
							(PVOID)&uEnableDhcp, sizeof(uEnableDhcp), 
							&uValueType, &uRetLength );

		if( uEnableDhcp )
		{
			ntRetCode = GetRegistryKeyValue( hKey, (PWCHAR)DhcpIPAddress, sizeof(DhcpIPAddress), 
				(PVOID)&szLocalIp, sizeof(szLocalIp), 
				&uValueType, &uRetLength );

			ntRetCode = GetRegistryKeyValue( hKey, (PWCHAR)DhcpDefaultGateway, sizeof(DhcpDefaultGateway), 
				(PVOID)&szGatewayIp, sizeof(szGatewayIp), 
				&uValueType, &uRetLength );
			if( !NT_SUCCESS(ntRetCode) )
			{
				ntRetCode = GetRegistryKeyValue( hKey, (PWCHAR)DefaultGateway, sizeof(DefaultGateway), 
					(PVOID)&szGatewayIp, sizeof(szGatewayIp), 
					&uValueType, &uRetLength );
			}

			ntRetCode = GetRegistryKeyValue( hKey, (PWCHAR)IPAutoconfigurationAddress, sizeof(IPAutoconfigurationAddress), 
				(PVOID)&szIPAutoconfigurationAddress, sizeof(szIPAutoconfigurationAddress), 
				&uValueType, &uRetLength );

		}
		else
		{
			ntRetCode = GetRegistryKeyValue( hKey, (PWCHAR)IPAddress, sizeof(IPAddress), 
				(PVOID)&szLocalIp, sizeof(szLocalIp), 
				&uValueType, &uRetLength );

			ntRetCode = GetRegistryKeyValue( hKey, (PWCHAR)DefaultGateway, sizeof(DefaultGateway), 
				(PVOID)&szGatewayIp, sizeof(szGatewayIp), 
				&uValueType, &uRetLength );
		}
		
		ZwClose( hKey );
		hKey = NULL;
		
		if( inet_aton( szLocalIp, &uIPAddr ) )
			pLocalIp[uLocalIpCount++] = uIPAddr;
		
		if( inet_aton( szIPAutoconfigurationAddress, &uIPAddr ) )
			pLocalIp[uLocalIpCount++] = uIPAddr;

		if( inet_aton( szGatewayIp, &uIPAddr ) )
			*pGatewayIP = uIPAddr;
		
		ntRetCode = STATUS_SUCCESS;

	} while (FALSE);

	return ntRetCode;
}


//////////////////////////////////////////////////////////////////////////

NTSTATUS KNdisFirewall::GetRegistryKeyValue(
	HANDLE Handle,
	const PWCHAR KeyNameString,
	ULONG KeyNameStringLength,
	PVOID Data,
	ULONG DataLength,
	PULONG Type,
	PULONG RetrunLength )
{
	UNICODE_STRING sKeyName;
	ULONG ulLength;
	PKEY_VALUE_FULL_INFORMATION pFullInfo;
	NTSTATUS ntRetCode = STATUS_INSUFFICIENT_RESOURCES;

	RtlInitUnicodeString (&sKeyName, KeyNameString);

	ulLength = sizeof(KEY_VALUE_FULL_INFORMATION) + KeyNameStringLength + DataLength;
	pFullInfo = (PKEY_VALUE_FULL_INFORMATION)non_paged_pool::__AllocateSystemMemory( ulLength );
	if (pFullInfo == NULL)
		return ntRetCode;

	ntRetCode = ZwQueryValueKey( Handle, &sKeyName, KeyValueFullInformation, pFullInfo, ulLength, &ulLength );
	if ( NT_SUCCESS(ntRetCode) ) 
	{
		if ( DataLength >= pFullInfo->DataLength ) 
		{
			RtlCopyMemory( Data, ((PUCHAR)pFullInfo) + pFullInfo->DataOffset, pFullInfo->DataLength );
		}
		*RetrunLength = pFullInfo->DataLength;
		*Type = pFullInfo->Type;
	}

	non_paged_pool::__DeallocateSystemMemory( pFullInfo );

	return ntRetCode;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisFirewall::IsModuleEnable( ULONG uModuleCode )
{
	return (uModuleCode & m_uEnableFlag) != 0;
}

//////////////////////////////////////////////////////////////////////////

VOID KNdisFirewall::ThreadGetAddr( PVOID StartContext )
{
	KNdisFirewall* pThis = (KNdisFirewall*)StartContext;
	PKANTIARP_ADAPTER pAdapter = NULL;
	
	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, "KNdisFirewall::ThreadGetAddr++");
	while( TRUE )
	{
		if( g_ndisFirewall != NULL )
		{
			NdisAcquireSpinLock(&g_Data.GlobalLock);
			for( pAdapter = g_Data.AdapterList; pAdapter != NULL; pAdapter = pAdapter->Next )
			{
				PtReferenceAdapt( pAdapter );
				NdisReleaseSpinLock(&g_Data.GlobalLock);

				pThis->SetAdapterIPFromReg( pAdapter );

				NdisAcquireSpinLock(&g_Data.GlobalLock);
				if( PtDereferenceAdapt( pAdapter ) )
					break;
			}
			NdisReleaseSpinLock(&g_Data.GlobalLock);
		}

		if( pThis->WaitForGetAddrEvent() )
			break;
	}
	KAntiarpDebugPrint(TRACE_LEVEL_INFORMATION, FLAG_NDIS_MAIN, "KNdisFirewall::ThreadGetAddr--");
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisFirewall::BindMacForGateway()
{
	PKANTIARP_ADAPTER pAdapter = NULL;

	if( m_arpFilter == NULL )
		return FALSE;

	NdisAcquireSpinLock(&g_Data.GlobalLock);
	for( pAdapter = g_Data.AdapterList; pAdapter != NULL; pAdapter = pAdapter->Next )
	{
		if( m_arpFilter->GetProtectMac( pAdapter->GatewayIP, pAdapter->GatewayMac ) )
			pAdapter->bManualGatewayMac = TRUE;
	}
	NdisReleaseSpinLock(&g_Data.GlobalLock);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisFirewall::UnBindMacForGateway()
{
	PKANTIARP_ADAPTER pAdapter = NULL;

	if( m_arpFilter == NULL )
		return FALSE;

	NdisAcquireSpinLock(&g_Data.GlobalLock);

	for( pAdapter = g_Data.AdapterList; pAdapter != NULL; pAdapter = pAdapter->Next )
	{
		if( m_arpFilter->GetProtectMac( pAdapter->GatewayIP, pAdapter->GatewayMac ) )
		{
			RtlZeroMemory( pAdapter->GatewayMac , 6 );
			pAdapter->bManualGatewayMac = FALSE;
		}
	}

	NdisReleaseSpinLock(&g_Data.GlobalLock);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS KNdisFirewall::LoadConfig( PUNICODE_STRING pRegisterPath )
{
	NTSTATUS ntRetCode = STATUS_SUCCESS;
	HANDLE hKey = NULL;
	ULONG uValueType = 0;
	ULONG uRetLength = 0;
	OBJECT_ATTRIBUTES ObjectAttributes;
	const WCHAR wsLoadFlag[] = L"LoadModule";
	const WCHAR wsEnableFlag[] = L"Enable";
	const WCHAR wsArpBlockFlag[] = L"ArpBlock";
	
	InitializeObjectAttributes( &ObjectAttributes, pRegisterPath, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL );

	do 
	{
		ntRetCode = ZwOpenKey( &hKey, KEY_ALL_ACCESS, &ObjectAttributes );
		if ( NT_SUCCESS(ntRetCode) == FALSE )
			break;	
		
		ntRetCode = GetRegistryKeyValue( hKey, (PWCHAR)wsLoadFlag, sizeof(wsLoadFlag), 
			(PVOID)&m_uLoadFlag, sizeof(m_uLoadFlag), 
			&uValueType, &uRetLength );

		ntRetCode = GetRegistryKeyValue( hKey, (PWCHAR)wsEnableFlag, sizeof(wsEnableFlag), 
			(PVOID)&m_uEnableFlag, sizeof(m_uEnableFlag), 
			&uValueType, &uRetLength );

		ntRetCode = GetRegistryKeyValue( hKey, (PWCHAR)wsArpBlockFlag, sizeof(wsArpBlockFlag), 
			(PVOID)&m_uArpBlockFlag, sizeof(m_uArpBlockFlag), 
			&uValueType, &uRetLength );
		
		ZwClose( hKey );
		hKey = NULL;
	} while (FALSE);
	
	return ntRetCode;
}

//////////////////////////////////////////////////////////////////////////

ULONG KNdisFirewall::GetArpBlockFlag()
{
	return m_uArpBlockFlag;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisFirewall::SetAdapterIPFromReg( PKANTIARP_ADAPTER pAdapter )
{
	ULONG uLocalIP[MAX_ADAPTER_IP_NUM] = {0};
	ULONG uGatewayIP = 0;

	if( GetIpFromReg( &pAdapter->UpperDeviceName, uLocalIP, &uGatewayIP ) >= 0  )
	{
		SetAdapterIp( pAdapter, uLocalIP, uGatewayIP );
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisFirewall::WaitForGetAddrEvent()
{
	return m_threadGetAddr.Wait( 3000 );
}

//////////////////////////////////////////////////////////////////////////

VOID KNdisFirewall::SetAddrEvent()
{
	m_threadGetAddr.SetEvent();
}

//////////////////////////////////////////////////////////////////////////

VOID KNdisFirewall::WanLineUp( PNDIS_WAN_LINE_UP pNdisWanLineUp )
{
	if( m_rasConnections != NULL )
		m_rasConnections->LineUp( pNdisWanLineUp );
}

//////////////////////////////////////////////////////////////////////////

VOID KNdisFirewall::WanLineDown( PNDIS_WAN_LINE_DOWN pNdisWanLineDown )
{
	if( m_rasConnections != NULL )
		m_rasConnections->LineDown( pNdisWanLineDown );
}

//////////////////////////////////////////////////////////////////////////

VOID KNdisFirewall::NotifyRasEvent( PRAS_CONNECTION pRasConn )
{
	if( m_eventSender != NULL )
		m_eventSender->SendCommonEvent( (UCHAR*)pRasConn, sizeof(RAS_CONNECTION) );
}

//////////////////////////////////////////////////////////////////////////

VOID KNdisFirewall::ThreadSendLogs( PVOID StartContext )
{
	KNdisFirewall* pThis = (KNdisFirewall*)StartContext;
	KIRQL oldIrq;
	while( !pThis->m_threadSendLogs.Wait( 5000 ) )
	{
		KeAcquireSpinLock( &pThis->m_cacheLogsLock, &oldIrq );
		
		for( int i=0; i<MAX_CACHE_LOGS; i++ )
		{
			if( pThis->m_cacheLogs[i].nCount != 0 )
			{
				pThis->m_eventSender->SendCommonEvent( (UCHAR*)&pThis->m_cacheLogs[i], sizeof(LOG_IP_RULE) );
				pThis->m_cacheLogs[i].nCount = 0;
			}
		}

		KeReleaseSpinLock( &pThis->m_cacheLogsLock , oldIrq );
	}
}

//////////////////////////////////////////////////////////////////////////

VOID KNdisFirewall::CacheLogs( LOG_IP_RULE* ipLog )
{
	ULONG i = 0;
	KIRQL oldIrq;
	KeAcquireSpinLock( &m_cacheLogsLock, &oldIrq );
	
	for( i=0; i<MAX_CACHE_LOGS; i++ )
	{
		ULONG uCurIdx = (m_cacheLogIdx + i) % MAX_CACHE_LOGS;
		if( m_cacheLogs[uCurIdx].nCount != 0 && IsEqualLog( ipLog, &m_cacheLogs[uCurIdx] ) )
		{
			 m_cacheLogs[uCurIdx].nCount++;
			 break;
		}
	}
	
	if( i == MAX_CACHE_LOGS )
	{
		if( m_cacheLogs[m_cacheLogIdx].nCount != 0 )
			m_eventSender->SendCommonEvent( (UCHAR*)&m_cacheLogs[m_cacheLogIdx], sizeof(LOG_IP_RULE) );

		RtlCopyMemory( (UCHAR*)&m_cacheLogs[m_cacheLogIdx], (UCHAR*)ipLog, sizeof(LOG_IP_RULE) );
		m_cacheLogs[m_cacheLogIdx].nCount = 1;
		m_cacheLogIdx = (m_cacheLogIdx + 1) % MAX_CACHE_LOGS;
	}

	KeReleaseSpinLock( &m_cacheLogsLock , oldIrq );
}

//////////////////////////////////////////////////////////////////////////

BOOLEAN KNdisFirewall::IsEqualLog( LOG_IP_RULE* ipLog1, LOG_IP_RULE* ipLog2 )
{
	const ULONG uCmpLen = sizeof(LOG_IP_RULE) - sizeof(ULONG);
	return RtlCompareMemory( (UCHAR*)ipLog1, (UCHAR*)ipLog2, uCmpLen ) == uCmpLen;
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS KNdisFirewall::CleanGatewayMac()
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PKANTIARP_ADAPTER pAdapter = NULL;

	NdisAcquireSpinLock(&g_Data.GlobalLock);
	for( pAdapter = g_Data.AdapterList; pAdapter != NULL; pAdapter = pAdapter->Next )
	{
		PtReferenceAdapt( pAdapter );
		NdisReleaseSpinLock(&g_Data.GlobalLock);

		pAdapter->bRequestGatewayMac = FALSE;
		RtlZeroMemory( pAdapter->GatewayMac, 6 );
		status = STATUS_SUCCESS;

		NdisAcquireSpinLock(&g_Data.GlobalLock);
		if( PtDereferenceAdapt( pAdapter ) )
			break;
	}
	NdisReleaseSpinLock(&g_Data.GlobalLock);

	return status;
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS KNdisFirewall::SelectGatewayMac()
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PKANTIARP_ADAPTER pAdapter = NULL;

	NdisAcquireSpinLock(&g_Data.GlobalLock);
	for( pAdapter = g_Data.AdapterList; pAdapter != NULL; pAdapter = pAdapter->Next )
	{
		PtReferenceAdapt( pAdapter );
		NdisReleaseSpinLock(&g_Data.GlobalLock);

		do 
		{
			int i = 0;
			int nMinIdx = 0;
			long nMinRecvCount = 0;

			if( pAdapter->arpCache == NULL )
				break;
			
			if( RtlCompareMemory( pAdapter->arpCache[0].GatewayMac, "\0\0\0\0\0\0\0", 6 ) == 6 )
				break;

			nMinIdx = 0;
			nMinRecvCount = abs( pAdapter->arpCache[0].nRecvCount );
			for( i=1; i<MAX_ARP_CACHE; i++ )
			{
				if( RtlCompareMemory( pAdapter->arpCache[i].GatewayMac, "\0\0\0\0\0\0\0", 6 ) == 6 )
					break;

				if( nMinRecvCount > abs(pAdapter->arpCache[i].nRecvCount) )
				{
					nMinRecvCount = abs(pAdapter->arpCache[i].nRecvCount);
					nMinIdx = i;
				}
			}

			
			if( nMinRecvCount < 3 )
			{
				RtlCopyMemory( pAdapter->GatewayMac, pAdapter->arpCache[nMinIdx].GatewayMac, 6 );
				status = STATUS_SUCCESS;
			}

			RtlZeroMemory( (char*)pAdapter->arpCache, sizeof( ARP_CACHE) * MAX_ARP_CACHE );
			
		} while(FALSE);

		NdisAcquireSpinLock(&g_Data.GlobalLock);
		if( PtDereferenceAdapt( pAdapter ) )
			break;
	}
	NdisReleaseSpinLock(&g_Data.GlobalLock);
	
	return status;
}