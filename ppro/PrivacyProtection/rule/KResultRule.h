#ifndef __KResultRule_h__
#define __KResultRule_h__

#include "IRule.h"

class KResultRule : IRule
{
public:
	DECLARE_RULE_NAME(KResultRule);

	virtual BOOL Match(KAccessData* pData)
	{
		pData->nResult = m_nResult;
		if (!m_strInfo.IsEmpty())
		{
			_tcsncpy(pData->szInfo, m_strInfo, 99);
			pData->szInfo[99] = 0;
		}
		return TRUE;
	}

	virtual void DbgPrint(void)
	{
		DPrintA("KResultRule::DbgPrint name:%ws, result:%d, info:%ws\n\n", Name(), m_nResult, m_strInfo);
	}

	void SetResult(int nResult, LPCTSTR szInfo)
	{
		m_nResult = nResult;
		m_strInfo = szInfo;
	}

private:
	int m_nResult;
	CString m_strInfo;
};

#endif