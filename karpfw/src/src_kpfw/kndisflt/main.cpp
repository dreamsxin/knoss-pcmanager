#include "pch.h" 
#include "main.tmh"
#pragma hdrstop

unsigned long DefaultPoolTag = KANTIARP_POOL_TAG;


NDIS_MEDIUM         MediumArray[4] =
                    {
                        NdisMedium802_3,    // Ethernet
                        NdisMedium802_5,    // Token-ring
                        NdisMediumFddi,     // Fddi
                        NdisMediumWan       // NDISWAN
                    };


enum _DEVICE_STATE
{
	PS_DEVICE_STATE_READY = 0,    // ready for create/delete
	PS_DEVICE_STATE_CREATING,    // create operation in progress
	PS_DEVICE_STATE_DELETING    // delete operation in progress
} ControlDeviceState = PS_DEVICE_STATE_READY;


KANTIARP_DATA g_Data;
KNdisFirewall *g_ndisFirewall = NULL;


//////////////////////////////////////////////////////////////////////////
PDEVICE_OBJECT g_pDeviceEtw = NULL;
VOID InitEtw( IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath );
VOID UnInitEtw();
NTSTATUS BakRegisterPath( IN  PUNICODE_STRING RegistryPath );


NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT        DriverObject,
    IN PUNICODE_STRING       RegistryPath
    )
{
    NDIS_STATUS                        Status;
    NDIS_PROTOCOL_CHARACTERISTICS      PChars;
    NDIS_MINIPORT_CHARACTERISTICS      MChars;
    NDIS_STRING                        Name;

    Status = NDIS_STATUS_SUCCESS;

	NdisZeroMemory(&g_Data, sizeof(KANTIARP_DATA));

	Status = BakRegisterPath( RegistryPath );
	if( Status != STATUS_SUCCESS )
	{
		return Status;
	}

	g_Data.pDriverObject = DriverObject;

	InitEtw( DriverObject, RegistryPath );

    NdisAllocateSpinLock(&g_Data.GlobalLock);
	
	// NdisMInitializeWrapper notifies NDIS that a new miniport driver is initializing.
    NdisMInitializeWrapper(&g_Data.NdisWrapperHandle, DriverObject, RegistryPath, NULL);

    do
    {
        //
        // Register the miniport with NDIS. Note that it is the miniport
        // which was started as a driver and not the protocol. Also the miniport
        // must be registered prior to the protocol since the protocol's BindAdapter
        // handler can be initiated anytime and when it is, it must be ready to
        // start driver instances.
        //

        NdisZeroMemory(&MChars, sizeof(NDIS_MINIPORT_CHARACTERISTICS));

        MChars.MajorNdisVersion = PASSTHRU_MAJOR_NDIS_VERSION;
        MChars.MinorNdisVersion = PASSTHRU_MINOR_NDIS_VERSION;

        MChars.InitializeHandler = MPInitialize;
        MChars.QueryInformationHandler = MPQueryInformation;
        MChars.SetInformationHandler = MPSetInformation;
        MChars.ResetHandler = NULL;
        MChars.TransferDataHandler = MPTransferData;
        MChars.HaltHandler = MPHalt;
#ifdef NDIS51_MINIPORT
        MChars.CancelSendPacketsHandler = MPCancelSendPackets;
        MChars.PnPEventNotifyHandler = MPDevicePnPEvent;
        MChars.AdapterShutdownHandler = MPAdapterShutdown;
#endif // NDIS51_MINIPORT

        //
        // We will disable the check for hang timeout so we do not
        // need a check for hang handler!
        //
        MChars.CheckForHangHandler = NULL;
        MChars.ReturnPacketHandler = MPReturnPacket;

        //
        // Either the Send or the SendPackets handler should be specified.
        // If SendPackets handler is specified, SendHandler is ignored
        //
        MChars.SendHandler = NULL;    // MPSend;
        MChars.SendPacketsHandler = MPSendPackets;

        Status = NdisIMRegisterLayeredMiniport(g_Data.NdisWrapperHandle,
                                                  &MChars,
                                                  sizeof(MChars),
                                                  &g_Data.DriverHandle);
        if (Status != NDIS_STATUS_SUCCESS)
        {
            break;
        }

#ifndef WIN9X
        NdisMRegisterUnloadHandler(g_Data.NdisWrapperHandle, PtUnload);
#endif

        //
        // Now register the protocol.
        //
        NdisZeroMemory(&PChars, sizeof(NDIS_PROTOCOL_CHARACTERISTICS));
        PChars.MajorNdisVersion = PASSTHRU_PROT_MAJOR_NDIS_VERSION;
        PChars.MinorNdisVersion = PASSTHRU_PROT_MINOR_NDIS_VERSION;

        //
        // Make sure the protocol-name matches the service-name
        // (from the INF) under which this protocol is installed.
        // This is needed to ensure that NDIS can correctly determine
        // the binding and call us to bind to miniports below.
        //
        NdisInitUnicodeString(&Name, L"KNdisFlt");    // Protocol name
        PChars.Name = Name;
        PChars.OpenAdapterCompleteHandler = PtOpenAdapterComplete;
        PChars.CloseAdapterCompleteHandler = PtCloseAdapterComplete;
        PChars.SendCompleteHandler = PtSendComplete;
        PChars.TransferDataCompleteHandler = PtTransferDataComplete;
    
        PChars.ResetCompleteHandler = PtResetComplete;
        PChars.RequestCompleteHandler = PtRequestComplete;
        PChars.ReceiveHandler = PtReceive;
        PChars.ReceiveCompleteHandler = PtReceiveComplete;
        PChars.StatusHandler = PtStatus;
        PChars.StatusCompleteHandler = PtStatusComplete;
        PChars.BindAdapterHandler = PtBindAdapter;
        PChars.UnbindAdapterHandler = PtUnbindAdapter;
        PChars.UnloadHandler = PtUnloadProtocol;

        PChars.ReceivePacketHandler = PtReceivePacket;
        PChars.PnPEventHandler= PtPNPHandler;

        NdisRegisterProtocol(&Status,
                             &g_Data.ProtocolHandle,
                             &PChars,
                             sizeof(NDIS_PROTOCOL_CHARACTERISTICS));

        if (Status != NDIS_STATUS_SUCCESS)
        {
            NdisIMDeregisterLayeredMiniport(g_Data.DriverHandle);
            break;
        }

        NdisIMAssociateMiniport(g_Data.DriverHandle, g_Data.ProtocolHandle);
    }
    while (FALSE);

    if (Status != NDIS_STATUS_SUCCESS)
    {
        NdisTerminateWrapper(g_Data.NdisWrapperHandle, NULL);
    }

    return(Status);
}


NDIS_STATUS
PtRegisterDevice(
    VOID
    )
{
    NDIS_STATUS            Status = NDIS_STATUS_SUCCESS;
	KNdisFirewall*		   ndisFireWall = NULL;

    DBGPRINT(("==>PtRegisterDevice\n"));

    NdisAcquireSpinLock(&g_Data.GlobalLock);

    ++g_Data.MiniportCount;
    
    if (1 == g_Data.MiniportCount)
    {
        ASSERT(ControlDeviceState != PS_DEVICE_STATE_CREATING);

        //
        // Another thread could be running PtDeregisterDevice on
        // behalf of another miniport instance. If so, wait for
        // it to exit.
        //
        while (ControlDeviceState != PS_DEVICE_STATE_READY)
        {
            NdisReleaseSpinLock(&g_Data.GlobalLock);
            NdisMSleep(1);
            NdisAcquireSpinLock(&g_Data.GlobalLock);
        }

        ControlDeviceState = PS_DEVICE_STATE_CREATING;

        NdisReleaseSpinLock(&g_Data.GlobalLock);

		//TODO:INIT
		if ( g_ndisFirewall == NULL )
		{
			ndisFireWall = new KNdisFirewall;
			if( ndisFireWall != NULL )
			{
				if( !ndisFireWall->Init( g_Data.pDriverObject, &g_Data.RegistryPath ) )
				{
					delete ndisFireWall;
					ndisFireWall = NULL;
				}
				else
				{
					g_ndisFirewall = ndisFireWall;
				}
			}
		}
		
        NdisAcquireSpinLock(&g_Data.GlobalLock);

        ControlDeviceState = PS_DEVICE_STATE_READY;
    }

    NdisReleaseSpinLock(&g_Data.GlobalLock);

    DBGPRINT(("<==PtRegisterDevice: %x\n", Status));

    return (Status);
}



NDIS_STATUS
PtDeregisterDevice(
    VOID
    )
{
    NDIS_STATUS Status = NDIS_STATUS_SUCCESS;

    DBGPRINT(("==>PassthruDeregisterDevice\n"));

    NdisAcquireSpinLock(&g_Data.GlobalLock);

    ASSERT(g_Data.MiniportCount > 0);

    --g_Data.MiniportCount;
    
    if (0 == g_Data.MiniportCount)
    {
        //
        // All miniport instances have been halted. Deregister
        // the control device.
        //

        ASSERT(ControlDeviceState == PS_DEVICE_STATE_READY);

        //
        // Block PtRegisterDevice() while we release the control
        // device lock and deregister the device.
        // 
        ControlDeviceState = PS_DEVICE_STATE_DELETING;

        ControlDeviceState = PS_DEVICE_STATE_READY;
    }

    NdisReleaseSpinLock(&g_Data.GlobalLock);

    DBGPRINT(("<== PassthruDeregisterDevice: %x\n", Status));
    return Status;
    
}

VOID
PtUnload(
    IN PDRIVER_OBJECT        DriverObject
    )
{
    UNREFERENCED_PARAMETER(DriverObject);
    
    DBGPRINT(("PtUnload: entered\n"));
    
    PtUnloadProtocol();
    
    NdisIMDeregisterLayeredMiniport(g_Data.DriverHandle);
    
    NdisFreeSpinLock(&g_Data.GlobalLock);

	if( g_Data.RegistryPath.Buffer != NULL )
	{
		non_paged_pool::__DeallocateSystemMemory( g_Data.RegistryPath.Buffer );
		g_Data.RegistryPath.Buffer = NULL;
	}
	
	UnInitEtw();

    DBGPRINT(("PtUnload: done!\n"));
}

//////////////////////////////////////////////////////////////////////////

NTSTATUS BakRegisterPath( IN  PUNICODE_STRING RegistryPath )
{
	g_Data.RegistryPath.Buffer = (WCHAR*)non_paged_pool::__AllocateSystemMemory( RegistryPath->MaximumLength );	//no release
	if( g_Data.RegistryPath.Buffer == NULL )
	{
		return NDIS_STATUS_RESOURCES;
	}

	RtlZeroMemory( g_Data.RegistryPath.Buffer, RegistryPath->MaximumLength );
	RtlCopyMemory( g_Data.RegistryPath.Buffer, RegistryPath->Buffer, RegistryPath->Length );
	g_Data.RegistryPath.Length = RegistryPath->Length;
	g_Data.RegistryPath.MaximumLength = RegistryPath->MaximumLength;
	return STATUS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////

VOID InitEtw( IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath )
{
	UNICODE_STRING uStrDeviceName;
	NTSTATUS status = 0;
	ULONG  ulMajor, ulMinor, ulBuilderNumber;

	do 
	{
		PsGetVersion(&ulMajor, &ulMinor, &ulBuilderNumber, NULL);
		if( ulMajor == 5 && ulMinor == 0)		//W2k
			break;

		if( g_pDeviceEtw != NULL )
			break;

		RtlInitUnicodeString( &uStrDeviceName, L"\\Device\\KNdisEtw" );

		status = IoCreateDevice( DriverObject, 
			0, 
			&uStrDeviceName, 
			FILE_DEVICE_DISK_FILE_SYSTEM, 
			FILE_DEVICE_SECURE_OPEN,
			FALSE, 
			&g_pDeviceEtw );

		if( status != STATUS_SUCCESS )
			break;
		
		WPP_SYSTEMCONTROL( DriverObject );	//在W2k下使用这行，会造成IP无法获取，无法正常关机
		WPP_INIT_TRACING( g_pDeviceEtw, RegistryPath );

	} while (FALSE);
}

//////////////////////////////////////////////////////////////////////////

VOID UnInitEtw()
{
	if( g_pDeviceEtw != NULL )
	{
		IoDeleteDevice( g_pDeviceEtw );
		g_pDeviceEtw = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////

ULONG htonl(ULONG hostlong)
{
	ULONG ByteSwapped;

	ByteSwapped = ((hostlong & 0x00ff00ff) << 8) | ((hostlong & 0xff00ff00) >> 8);

	return (ByteSwapped << 16) | (ByteSwapped >> 16);
}

ULONG ntohl(ULONG netlong)
{
	ULONG ByteSwapped;

	ByteSwapped = ((netlong & 0x00ff00ff) << 8) | ((netlong & 0xff00ff00) >> 8);

	return (ByteSwapped << 16) | (ByteSwapped >> 16);
}

USHORT htons(USHORT hostshort)
{
	return ((hostshort << 8) | (hostshort >> 8));
}

USHORT ntohs(USHORT netshort)
{
	return ((netshort << 8) | (netshort >> 8));
}

__inline BOOLEAN isdigit( WCHAR c )
{
	return '0' <= c && c <='9';
}

__inline BOOLEAN isascii( WCHAR c )
{
	return c < 128;
}

__inline BOOLEAN isspace( WCHAR c )
{
	return c == ' ';
}


/* 
* Check whether "cp" is a valid ascii representation
* of an Internet address and convert to a binary address.
* Returns 1 if the address is valid, 0 if not.
* This replaces inet_addr, the return value from which
* cannot distinguish between failure and a local broadcast address.
*/
ULONG inet_aton(const WCHAR *cp, ULONG *addr) 
{
	static const ULONG max[4] = { 0xffffffff, 0xffffff, 0xffff, 0xff };
	UCHAR val;

	int base;

	WCHAR c;
	union iaddr {
		UCHAR bytes[4];
		ULONG word;
	} res;
	UCHAR *pp = res.bytes;
	int digit;


	res.word = 0;

	c = *cp;
	for (;;) {
		/*
		* Collect number up to ``.''.
		* Values are specified as for C:
		* 0x=hex, 0=octal, isdigit=decimal.
		*/
		if (!isdigit(c))
			goto ret_0;

		val = 0; base = 10; digit = 0;
		for (;;) {
			if (isascii(c) && isdigit(c)) {
				if (base == 8 && (c == '8' || c == '9'))
					return (0);
				val = (val * base) + (c - '0');
				c = *++cp;
				digit = 1;
			} 
			else
				break;
		}

		if (c == '.') {
			/*
			* Internet format:
			*	a.b.c.d
			*	a.b.c	(with c treated as 16 bits)
			*	a.b	(with b treated as 24 bits)
			*/
			if (pp > res.bytes + 2 || val > 0xff)
				goto ret_0;
			*pp++ = val;
			c = *++cp;
		} else
			break;
	}
	/*
	* Check for trailing characters.
	*/
	if (c != '\0' && (!isascii(c) || !isspace(c)))
		goto ret_0;
	/*
	* Did we get a valid digit?
	*/
	if (!digit)
		goto ret_0;

	/* Check whether the last part is in its limits depending on
	the number of parts in total.  */
	if (val > max[pp - res.bytes])
		goto ret_0;

	if (addr != NULL)
		*addr = res.word | htonl (val);

	return (1);

ret_0:

	return (0);
}

