#ifndef __KFileNameRelateRule_h__
#define __KFileNameRelateRule_h__

#include "IRule.h"

/*
ClassName	: KFileNameRelateRule
Creator		: zhongzhendong
Brief		: 文件名关联进程规则，判断文件默认打开进程是否在系统定义进程或者自定义进程列表
*/

class KFileNameRelateRule : public IRule
{
public:
	DECLARE_RULE_NAME(KFileNameRelateRule);

	virtual BOOL Match(KAccessData* pData);
	virtual void DbgPrint(void);

	void AddFileNameRelate(LPCTSTR szFileName, LPCTSTR szRelateProc);
	void RemoveFileNameRelate(LPCTSTR szFileName, LPCTSTR szRelateProc);
	void Clear(void);

private:
	std::map<CString, std::vector<CString>> m_mapFileNameRelates;
};

#endif