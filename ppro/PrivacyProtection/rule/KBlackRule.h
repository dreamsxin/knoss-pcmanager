#ifndef __KBlackRule_h__
#define __KBlackRule_h__

#include "IRule.h"

class KBlackRule : public IRule
{
public:
	DECLARE_RULE_NAME(KBlackRule);

	virtual BOOL Match(KAccessData* pData);
	virtual void DbgPrint(void);

	void AddProcFile(LPCTSTR szProcess, LPCTSTR szFile);
	void Clear(void);

private:
	std::map<CString, std::vector<CString>> m_mapBlackProcFile;
};

#endif