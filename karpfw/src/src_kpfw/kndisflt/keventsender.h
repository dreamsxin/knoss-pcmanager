#pragma once

#define MAX_COMMON_EVENT  (1024)

struct RequestInfo
{
	ULONG uMagic;
	UCHAR *InBuffer;
	ULONG InBufferLen;
	UCHAR *OutBuffer;
	ULONG OutBufferLen;
	KEVENT pResponseEvent;
};


class KEventSender
{
public:
	BOOLEAN Init( PDRIVER_OBJECT DriverObject = NULL, PUNICODE_STRING RegistryPath = NULL);
	BOOLEAN Uninit( PDRIVER_OBJECT DriverObject = NULL);
	
	BOOLEAN RegisterEvent( HANDLE hCommonEvent );
	
	BOOLEAN SendCommonEvent(
		IN UCHAR *InBuffer,
		IN ULONG InBufferLen);
	
	BOOLEAN GetCommonEvent(
		OUT UCHAR *UserBuffer,
		IN ULONG UserBufferLen,
		OUT ULONG *uResultLen);

private:
	VOID ClearEventList( KDList<PVOID, RequestInfo*> *eventList, BOOLEAN bSetEvent );
	KDList<PVOID,RequestInfo*> m_commonEventList;
	PVOID m_CommonEventObj;

};
