#include "stdafx.h"
#include "KFunction.h"
#include "KSignCache.h"
#include "signverify.h"

KSignCache g_signCache;

void KSignCache::AddFileSign(LPCTSTR szPath, LPCSTR szSign)
{
	std::map<CString, CStringA>::iterator iter;

	iter = m_vecSignCache.find(szPath);
	if (iter != m_vecSignCache.end())
	{
		m_vecSignCache.insert(std::make_pair(szPath, szSign));
	}
	else
	{
		iter->second = szSign;
	}
}

LPCSTR KSignCache::GetFileSign(LPCTSTR szPath, int nPathLen)
{
	LPCSTR pszSign = NULL;
	std::map<CString, CStringA>::iterator iterMap;

	iterMap = m_vecSignCache.find(szPath);
	if (iterMap != m_vecSignCache.end())
	{
		pszSign = (LPCSTR)iterMap->second;
		goto Exit0;
	}
	else
	{
		int nMemSize = nPathLen * sizeof(TCHAR);
		std::vector<CString>::iterator iter;
		for (iter = m_vecNoSignFileList.begin(); iter != m_vecNoSignFileList.end(); iter++)
		{
			CString& strTemp = *iter;

			if (strTemp.GetLength() == nPathLen &&
				memcmp(szPath, (LPCTSTR)strTemp, nMemSize) == 0)
			{
				goto Exit0;
			}
		}

		LONGLONG lFieSize = KFunction::GetFileSize(szPath);
		if (lFieSize == 0)
		{
			m_vecNoSignFileList.push_back(szPath);
			goto Exit0;
		}

		CStringA strSign;
		BOOL bMicroSoft = FALSE;
		if (CSignVerify::Instance().GetSignData(szPath, bMicroSoft, strSign))
		{
			std::map<CString, CStringA>::_Pairib pairib = m_vecSignCache.insert(std::make_pair(szPath, strSign));
			pairib.first->second.MakeLower();
			pszSign = (LPCSTR)pairib.first->second;
			DPrintA("KSignCache::GetFileSign succeed, file:%ws, sign:%s\n", szPath, pszSign);
			goto Exit0;
		}
		else
		{
			DPrintA("KSignCache::GetFileSign fail, file:%ws\n", szPath);
		}
	}

Exit0:
	if (!pszSign)
		m_vecNoSignFileList.push_back(szPath);

	if (pszSign)
	{
		DPrintA("KSignCache::GetFileSign sign:%s, file:%ws \n", pszSign, szPath);
	}
	else
		DPrintA("KSignCache::GetFileSign fail, file:%ws\n", szPath);
	
	return pszSign;
}

void KSignCache::Clear()
{
	m_vecSignCache.clear();
}