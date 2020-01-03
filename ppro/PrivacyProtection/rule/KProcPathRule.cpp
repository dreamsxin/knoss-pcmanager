#include "stdafx.h"
#include "KFunction.h"
#include "KProcPathRule.h"

BOOL KProcPathRule::Match(KAccessData* pData)
{
	int nMemSize = pData->nProcPathLen * sizeof(TCHAR);
	std::vector<CString>::iterator iter;
	for (iter = m_vecProcPaths.begin(); iter != m_vecProcPaths.end(); ++iter)
	{
		CString& strTemp = *iter;
		if (strTemp.GetLength() == pData->nProcPathLen &&
			memcmp(pData->szProcPath, (LPCTSTR)strTemp, nMemSize) == 0)
			return TRUE;
	}

	return FALSE;
}

void KProcPathRule::DbgPrint(void)
{
	DPrintA("KProcPathRule::DbgPrint\n");

	DPrintA("proc path count %d:\n", m_vecProcPaths.size());
	for (size_t i = 0; i < m_vecProcPaths.size(); i++)
	{
		DPrintA("\t%d:%ws\n", i, m_vecProcPaths[i]);
	}

	DPrintA("\n\n");
}

void KProcPathRule::AddProcPath(LPCTSTR szPath)
{
	CString strProcPath(szPath);

	strProcPath.MakeLower();

	for (size_t i = 0; i < m_vecProcPaths.size(); i++)
	{
		if (m_vecProcPaths[i] == strProcPath)
			return;
	}

	m_vecProcPaths.push_back(strProcPath);
}

void KProcPathRule::RemoveProcPath(LPCTSTR szPath)
{
	CString strProcPath(szPath);
	std::vector<CString>::iterator iter;

	strProcPath.MakeLower();

	for (iter = m_vecProcPaths.begin(); iter != m_vecProcPaths.end();)
	{
		if ((*iter) == strProcPath)
		{
			iter = m_vecProcPaths.erase(iter);
		}
		else
			++iter;
	}
}

void KProcPathRule::Clear(void)
{
	m_vecProcPaths.clear();
}