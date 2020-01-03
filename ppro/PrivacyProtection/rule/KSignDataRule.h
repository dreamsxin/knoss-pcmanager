#ifndef __KSignDataRule_h__
#define __KSignDataRule_h__

#include "IRule.h"
#include "Common\KLocker.h"

/*
ClassName	: KSignDataRule
Creator		: zhongzhendong
Brief		: 数字签名规则，判断文件的数字签名是否在列表中
Remark		: 可以增加有无数字签名的判断
*/

class KSignDataRule : public IRule
{
public:
	DECLARE_RULE_NAME(KSignDataRule);
	virtual BOOL Match(KAccessData* pData);
	virtual void DbgPrint(void);

	void AddSign(LPCSTR szSign);
	void RemoveSign(LPCSTR szSign);
	void Clear(void);


private:
	std::vector<CStringA> m_vecSigns;
	KCriticalSesion m_lckProcList;
	std::vector<CString> m_vecPassProcList;
	std::vector<CString> m_vecBlockProcList;
};

#endif