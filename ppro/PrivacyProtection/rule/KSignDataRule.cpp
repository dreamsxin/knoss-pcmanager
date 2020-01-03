#include "stdafx.h"
#include "KFunction.h"
#include "KSignDataRule.h"
#include "signverify.h"
#include "KSignCache.h"

BOOL KSignDataRule::Match(KAccessData* pData)
{
	KLocker lock(m_lckProcList);
	int nMemSize = pData->nProcPathLen * 2;
	std::vector<CString>::iterator iter;

	for (iter = m_vecBlockProcList.begin(); iter != m_vecBlockProcList.end(); iter++)
	{
		CString& strPath = *iter;
		if (pData->nProcPathLen == strPath.GetLength() &&
			memcmp(pData->szProcPath, (LPCTSTR)strPath, nMemSize) == 0)
		{
			return FALSE;
		}
	}

	for (iter = m_vecPassProcList.begin(); iter != m_vecPassProcList.end(); iter++)
	{
		CString& strPath = *iter;
		if (pData->nProcPathLen == strPath.GetLength() &&
			memcmp(pData->szProcPath, (LPCTSTR)strPath, nMemSize) == 0)
		{
			return TRUE;
		}
	}

	BOOL bRetCode = FALSE;
	BOOL bMicorSoft = FALSE;
	std::string strSign;
	static int s_bPassMicrosoft = -1;
	std::vector<CStringA>::iterator iterSign;
	if (s_bPassMicrosoft == -1)
	{
		s_bPassMicrosoft = FALSE;
		for (iterSign = m_vecSigns.begin(); iterSign != m_vecSigns.end(); iterSign++)
		{
			CStringA& strTemp = *iterSign;
			if (strTemp == "ms")
				s_bPassMicrosoft = TRUE;
		}
	}

	LONGLONG lFieSize = KFunction::GetFileSize(pData->szProcPath);
	if (lFieSize == 0)
	{
		m_vecBlockProcList.push_back(pData->szProcPath);
		return FALSE;
	}

	LPCSTR szSign = g_signCache.GetFileSign(pData->szProcPath, pData->nFilePathLen);
	if (szSign)
	{
		for (iterSign = m_vecSigns.begin(); iterSign != m_vecSigns.end(); iterSign++)
		{
			CStringA& strTemp = *iterSign;
			if (strTemp.CompareNoCase(szSign) == 0)
			{
				m_vecPassProcList.push_back(pData->szProcPath);
				return TRUE;
			}
		}
		
		if (strstr(szSign, "microsoft"))
		{
			m_vecPassProcList.push_back(pData->szProcPath);
			return TRUE;
		}
	}

	m_vecBlockProcList.push_back(pData->szProcPath);
	return FALSE;
}

void KSignDataRule::DbgPrint(void)
{
	DPrintA("KSignDataRule::DbgPrint\n");

	DPrintA("sign data count %d:\n", m_vecSigns.size());
	for (size_t i = 0; i < m_vecSigns.size(); i++)
	{
		DPrintA("\t%d:%s\n", i, m_vecSigns[i]);
	}

	DPrintA("\n\n");
}

void KSignDataRule::AddSign(LPCSTR szSign)
{
	CStringA strSign(szSign);

	strSign.MakeLower();

	for (size_t i = 0; i < m_vecSigns.size(); i++)
	{
		if (m_vecSigns[i] == strSign)
			return;
	}

	m_vecSigns.push_back(strSign);
}

void KSignDataRule::RemoveSign(LPCSTR szSign)
{
	CStringA strSign(szSign);
	std::vector<CStringA>::iterator iter;

	strSign.MakeLower();

	for (iter = m_vecSigns.begin(); iter != m_vecSigns.end();)
	{
		if ((*iter) == szSign)
		{
			iter = m_vecSigns.erase(iter);
		}
		else
			++iter;
	}
}

void KSignDataRule::Clear(void)
{
	m_vecSigns.clear();
}
