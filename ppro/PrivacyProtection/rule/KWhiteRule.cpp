#include "stdafx.h"
#include "KFunction.h"
#include "KWhiteRule.h"

BOOL KWhiteRule::Match(KAccessData* pData)
{
	int nMemSize = pData->nProcPathLen * sizeof(TCHAR);
	std::vector<CString>::iterator iter;
	
	for (iter = m_vecProcDirs.begin(); iter != m_vecProcDirs.end(); ++iter)
	{
		CString& strTemp = *iter;
		if (strTemp.GetLength() <= pData->nProcPathLen &&
			memcmp(pData->szProcPath, (LPCTSTR)strTemp, strTemp.GetLength() * sizeof(TCHAR)) == 0)
			return TRUE;
	}
	
	for (iter = m_vecProcPaths.begin(); iter != m_vecProcPaths.end(); ++iter)
	{
		CString& strTemp = *iter;
		if (strTemp.GetLength() == pData->nProcPathLen &&
			memcmp(pData->szProcPath, (LPCTSTR)strTemp, nMemSize) == 0)
			return TRUE;
	}

	for (iter = m_vecFileDirs.begin(); iter != m_vecFileDirs.end(); ++iter)
	{
		CString& strTemp = *iter;
		if (strTemp.GetLength() <= pData->nFileDirLen &&
			memcmp(pData->szFilePath, (LPCTSTR)strTemp, strTemp.GetLength() * sizeof(TCHAR)) == 0)
			return TRUE;
	}

	return FALSE;
}

void KWhiteRule::DbgPrint(void)
{
	DPrintA("KWhiteRule::DbgPrint\n");

	DPrintA("proc path count %d:\n", m_vecProcPaths.size());
	for (size_t i = 0; i < m_vecProcPaths.size(); i++)
	{
		DPrintA("\t%d:%ws\n", i + 1, m_vecProcPaths[i]);
	}

	DPrintA("\n\n");
}

void KWhiteRule::AddProcPath(LPCTSTR szFilePath)
{
	CString strFilePath(szFilePath);

	strFilePath.MakeLower();

	for (size_t i = 0; i < m_vecProcPaths.size(); i++)
	{
		if (m_vecProcPaths[i] == strFilePath)
			return;
	}

	m_vecProcPaths.push_back(strFilePath);
}

void KWhiteRule::AddProcDir(LPCTSTR szProcDir)
{
	CString strDir(szProcDir);
	
	strDir.MakeLower();
	KFunction::PathAddBackslash(strDir);
	
	for (size_t i = 0; i < m_vecProcDirs.size(); i++)
	{
		if (m_vecProcDirs[i] == strDir)
			return;
	}

	m_vecProcDirs.push_back(strDir);
}

void KWhiteRule::AddFileDir(LPCTSTR szFileDir)
{
	CString strDir(szFileDir);

	strDir.MakeLower();
	KFunction::PathAddBackslash(strDir);

	for (size_t i = 0; i < m_vecFileDirs.size(); i++)
	{
		if (m_vecFileDirs[i] == strDir)
			return;
	}

	m_vecFileDirs.push_back(strDir);
}

void KWhiteRule::RemoveProcPath(LPCTSTR szFilePath)
{
	CString strFilePath(szFilePath);
	std::vector<CString>::iterator iter;

	strFilePath.MakeLower();

	for (iter = m_vecProcPaths.begin(); iter != m_vecProcPaths.end();)
	{
		if ((*iter) == strFilePath)
		{
			iter = m_vecProcPaths.erase(iter);
		}
		else
			++iter;
	}
}

void KWhiteRule::RemoveProcDir(LPCTSTR szProcDir)
{
	CString strDir(szProcDir);
	std::vector<CString>::iterator iter;

	strDir.MakeLower();
	KFunction::PathAddBackslash(strDir);

	for (iter = m_vecProcDirs.begin(); iter != m_vecProcDirs.end();)
	{
		if ((*iter) == strDir)
		{
			iter = m_vecProcDirs.erase(iter);
		}
		else
			++iter;
	}
}

void KWhiteRule::Clear(void)
{
	m_vecProcPaths.clear();
	m_vecProcDirs.clear();
}