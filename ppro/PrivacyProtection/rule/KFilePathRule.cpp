#include "stdafx.h"
#include "KFunction.h"
#include "KFilePathRule.h"

BOOL KFilePathRule::Match(KAccessData* pData)
{
	int nMemSize = pData->nFilePathLen * sizeof(TCHAR);
	std::vector<CString>::iterator iter;
	for (iter = m_vecFilePaths.begin(); iter != m_vecFilePaths.end(); ++iter)
	{
		CString& strTemp = *iter;
		if (strTemp.GetLength() == pData->nFilePathLen &&
			memcmp(pData->szFilePath, (LPCTSTR)strTemp, nMemSize) == 0)
			return TRUE;
	}

	return FALSE;
}

void KFilePathRule::DbgPrint(void)
{
	DPrintA("KFilePathRule::DbgPrint\n");

	DPrintA("file path count %d:\n", m_vecFilePaths.size());
	for (size_t i = 0; i < m_vecFilePaths.size(); i++)
	{
		DPrintA("\t%d:%ws\n", i, m_vecFilePaths[i]);
	}

	DPrintA("\n\n");
}

void KFilePathRule::AddFilePath(LPCTSTR szFilePath)
{
	CString strFilePath(szFilePath);

	strFilePath.MakeLower();

	for (size_t i = 0; i < m_vecFilePaths.size(); i++)
	{
		if (m_vecFilePaths[i] == strFilePath)
			return;
	}

	m_vecFilePaths.push_back(strFilePath);
}

void KFilePathRule::RemoveFilePath(LPCTSTR szFilePath)
{
	CString strFilePath(szFilePath);
	std::vector<CString>::iterator iter;

	strFilePath.MakeLower();

	for (iter = m_vecFilePaths.begin(); iter != m_vecFilePaths.end();)
	{
		if ((*iter) == strFilePath)
		{
			iter = m_vecFilePaths.erase(iter);
		}
		else
			++iter;
	}
}

void KFilePathRule::Clear(void)
{
	m_vecFilePaths.clear();
}

