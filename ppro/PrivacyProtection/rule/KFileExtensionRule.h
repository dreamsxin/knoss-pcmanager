#ifndef __KFileExtensionRule_h__
#define __KFileExtensionRule_h__

#include "IRule.h"

/*
ClassName	: KFileExtensionRule
Creator		: zhongzhendong
Brief		: 文件扩展名规则，查看访问文件的扩展名是否在扩展名列表中
*/

class KFileExtensionRule : public IRule
{
public:
	DECLARE_RULE_NAME(KFileExtensionRule);

	virtual BOOL Match(KAccessData* pData);
	virtual void DbgPrint(void);

	void AddFileExt(LPCTSTR szFileExt);
	void RemoveFileExt(LPCTSTR szFileExt);
	void Clear(void);

private:
	std::vector<CString> m_vecFileExts;
};

#endif