#ifndef __KProcFileRule_h__
#define __KProcFileRule_h__

#include "IRule.h"

/*
ClassName	: KFilePathRule
Creator		: zhongzhendong
Brief		: 文件进程同一目录规则，判断文件是否在进程目录下
*/

class KProcFileRule : public IRule
{
public:
	DECLARE_RULE_NAME(KProcFileRule);

	virtual BOOL Match(KAccessData* pData);
	virtual void DbgPrint(void);
};

#endif