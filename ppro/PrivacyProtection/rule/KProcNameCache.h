#ifndef __KProcNameCache_h__
#define __KProcNameCache_h__

#include "IRule.h"

struct KProcNameData
{
	CStringA strSign;
	CString strFile;
	std::vector<CString> vecProcessList;
};

class KProcNameCache
{
public:
	void AddProcName(LPCTSTR szVariableName, LPCTSTR szProcName, LPCSTR szSign, LPCTSTR szFile);
	BOOL GetProcName(LPCTSTR szVariableName, KProcName& procname);
	BOOL MatchProc(LPCTSTR szProcPath, int nDirLen, CString& strProcName);
	void Clear();
	void DbgPrint(void);

protected:
	std::map<CString, KProcNameData> m_mapProcNameData;
	std::vector<CString> m_vecUnknownProcessList;
	std::map<CString, KProcName> m_mapProcNameList;
};

extern KProcNameCache g_procnameCache;

#endif