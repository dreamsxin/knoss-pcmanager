#ifndef __KWhiteRule_h__
#define __KWhiteRule_h__

#include "IRule.h"

class KWhiteRule : public IRule
{
public:
	DECLARE_RULE_NAME(KWhiteRule);

	virtual BOOL Match(KAccessData* pData);
	virtual void DbgPrint(void);

	void AddProcPath(LPCTSTR szProcPath);
	void AddProcDir(LPCTSTR szProcDir);
	void AddFileDir(LPCTSTR szFileDir);
	void RemoveProcPath(LPCTSTR szProcPath);
	void RemoveProcDir(LPCTSTR szProcDir);
	void Clear(void);

private:
	std::vector<CString> m_vecProcPaths;
	std::vector<CString> m_vecProcDirs;
	std::vector<CString> m_vecFileDirs;
};

#endif