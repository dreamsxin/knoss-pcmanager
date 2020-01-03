#ifndef __KFilePathRule_h__
#define __KFilePathRule_h__

#include "IRule.h"

/*
ClassName	: KFilePathRule
Creator		: zhongzhendong
Brief		: 文件全路径规则，判断文件路径是否在指定的文件列表中
*/

class KFilePathRule : public IRule
{
public:
	DECLARE_RULE_NAME(KFilePathRule);

	virtual BOOL Match(KAccessData* pData);
	virtual void DbgPrint(void);

	void AddFilePath(LPCTSTR szFilePath);
	void RemoveFilePath(LPCTSTR szFilePath);
	void Clear(void);

private:
	std::vector<CString> m_vecFilePaths;
};

#endif