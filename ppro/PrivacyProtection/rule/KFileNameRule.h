#ifndef __KFileNameRule_h__
#define __KFileNameRule_h__

#include "IRule.h"

class KFileNameRule : public IRule
{
public:
	DECLARE_RULE_NAME(KFileNameRule);

	virtual BOOL Match(KAccessData* pData);
	virtual void DbgPrint(void);

	void AddFileName(LPCTSTR szFilePath);
	void Clear(void);

private:
	std::vector<CString> m_vecFileNames;
};

#endif