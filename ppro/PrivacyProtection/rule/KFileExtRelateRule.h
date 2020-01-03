#ifndef __KFileExtRelateRule_h__
#define __KFileExtRelateRule_h__

#include "IRule.h"

/*
ClassName	: KFileExtRelateRule
Creator		: zhongzhendong
Brief		: 文件扩展名关联规则，查看访问此文件的程序是否是用户定义的关联程序，或我们自定义的关联程序
*/

class KFileExtRelateRule : public IRule
{
public:
	DECLARE_RULE_NAME(KFileExtRelateRule);

	virtual BOOL Match(KAccessData* pData);
	virtual void DbgPrint(void);

	void AddExtRelate(LPCTSTR szExtName, LPCTSTR szRelateProc);
	void RemoveExtRelate(LPCTSTR szExtName, LPCTSTR szRelateProc);
	void Clear(void);

protected:
	void GetSystemRelate(LPCTSTR szExtName, CString& strRelate);

private:
	std::map<CString, std::vector<CString>> m_mapExtRelates;
};

#endif