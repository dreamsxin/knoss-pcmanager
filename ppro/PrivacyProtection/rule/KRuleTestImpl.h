#ifndef __KRuleDbgImpl_h__
#define __KRuleDbgImpl_h__

#include "KRuleImpl.h"

class KRuleTestImpl
{
public:
	BOOL Init(LPCTSTR szConfigFile);
	void DebugMatch(void);

protected:
	BOOL LoadTestData(LPCTSTR szConfigFile);
	int WriteStringToFileA(LPCTSTR szPath, const char* format, ...);

private:
	KRuleImpl m_ruleImpl;
	std::vector<CString> m_vecTestData;
};

#endif