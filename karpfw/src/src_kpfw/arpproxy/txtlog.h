#pragma once
#include <fstream>
#include <atlsync.h>
#include "netwatch.h"

class CTxtLog
{
public:
	CTxtLog(void);
	~CTxtLog(void);

	BOOL Init();
	VOID Unint();
	DWORD GetSize();
	BOOL AddLog( PLOG_IP_RULE pIpRule );
    void GetLastLog(std::string& strLog);

private:
	BOOL IsSameModeArpAttack(INT nType);

	CCriticalSection m_cs;
	HANDLE m_hLog;
    std::string m_strLast;
};
