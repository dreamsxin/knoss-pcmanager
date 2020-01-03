#ifndef __KProcPathRule_h__
#define __KProcPathRule_h__

#include "IRule.h"

/*
ClassName	: KProcPathRule
Creator		: zhongzhendong
Brief		: 进程全路径规则，判断进程路径是否在指定的文件列表中
*/

class KProcPathRule : public IRule
{
public:
	DECLARE_RULE_NAME(KProcPathRule);

	virtual BOOL Match(KAccessData* pData);
	virtual void DbgPrint(void);

	void AddProcPath(LPCTSTR szPath);
	void RemoveProcPath(LPCTSTR szPath);
	void Clear(void);

private:
	std::vector<CString> m_vecProcPaths;

};

#endif