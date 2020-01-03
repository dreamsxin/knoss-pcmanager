#pragma once

class KDhcpFilter
{
public:
	BOOLEAN Init( PDRIVER_OBJECT DriverObject = NULL, PUNICODE_STRING RegistryPath = NULL );
	BOOLEAN Uninit( PDRIVER_OBJECT DriverObject = NULL );

	IPRESULT Filter( PKANTIARP_ADAPTER adapter, PVOID pBuffer, ULONG uBufferLength, PACKET_DIR enumDir );

private:
	IPRESULT FilterRecv( PKANTIARP_ADAPTER adapter, PVOID pBuffer, ULONG uBufferLength);
	IPRESULT FilterSend( PKANTIARP_ADAPTER adapter, PVOID pBuffer, ULONG uBufferLength);
};
