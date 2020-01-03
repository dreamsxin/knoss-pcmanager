#pragma once

class KWdmThread
{
public:
	BOOLEAN Init( PVOID pThreadProc, PVOID pContext );
	VOID SetEvent();
	BOOLEAN Uninit();
	BOOLEAN Wait( ULONG uTimer );	//∫¡√Î
private:
	KEVENT m_eventWait;
	PVOID m_pThreadObj;
	volatile LONG  m_bExit;
};
