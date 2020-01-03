#include "stdafx.h"
#include "KFunction.h"
#include "KFileDirRule.h"
#include "KpsRule.h"

BOOL KFileDirRule::Match(KAccessData* pData)
{
	std::vector<CString>::iterator iter;
	for (iter = m_vecFileDirs.begin(); iter != m_vecFileDirs.end(); iter++)
	{
		CString& strFileDir = *iter;
		if (strFileDir.GetLength() <= pData->nFileDirLen &&
			memcmp((LPCTSTR)strFileDir, pData->szFilePath, strFileDir.GetLength() * sizeof(TCHAR)) == 0)
		{
			return TRUE;
		}
	}

	for (iter = m_vecWildcardFileDirs.begin(); iter != m_vecWildcardFileDirs.end(); iter++)
	{
		CString& strFileDir = *iter;
		if (KRtl::StringWildCard(strFileDir, pData->szFilePath))
		{
			return TRUE;
		}
	}

	return FALSE;
}

void KFileDirRule::DbgPrint(void)
{
	DPrintA("KFileDirRule::DbgPrint\n");

	DPrintA("file dir count %d:\n", m_vecFileDirs.size());
	for (size_t i = 0; i < m_vecFileDirs.size(); i++)
	{
		DPrintA("\t%d:%ws\n", i + 1, m_vecFileDirs[i]);
	}

	DPrintA("not used file dir count%d:\n", m_vecNotUsed.size());
	for (size_t i = 0; i < m_vecNotUsed.size(); i++)
	{
		DPrintA("\t%d:%ws\n", i + 1, m_vecNotUsed[i]);
	}

	DPrintA("wildcard file dir count%d:\n", m_vecWildcardFileDirs.size());
	for (size_t i = 0; i < m_vecWildcardFileDirs.size(); i++)
	{
		DPrintA("\t%d:%ws\n", i + 1, m_vecWildcardFileDirs[i]);
	}

	DPrintA("\n\n");
}

void KFileDirRule::AddFileDir(LPCTSTR szFileDir)
{
	CString strFileDir(szFileDir);
	std::vector<CString>::iterator iter;

	strFileDir.MakeLower();

	if (KFunction::IsWildcardString(strFileDir))
	{
		for (iter = m_vecWildcardFileDirs.begin(); iter != m_vecWildcardFileDirs.end(); iter++)
		{
			if (*iter == strFileDir)
				return;
		}
		m_vecWildcardFileDirs.push_back(strFileDir);
		return;
	}

	KFunction::PathAddBackslash(strFileDir);

	for (iter = m_vecFileDirs.begin(); iter != m_vecFileDirs.end();)
	{
		CString& strTemp = *iter;
		if (strTemp == strFileDir)
			return;

		if (strTemp.GetLength() < strFileDir.GetLength() &&
			_tcsncmp(strTemp, strFileDir, strTemp.GetLength()) == 0)
		{
			AddNotusedFileDir(strFileDir);
			return;
		}

		if (strTemp.GetLength() > strFileDir.GetLength() &&
			_tcsncmp(strTemp, strFileDir, strFileDir.GetLength()) == 0)
		{
			AddNotusedFileDir(strTemp);
			iter = m_vecFileDirs.erase(iter);
			continue;
		}

		++iter;
	}

	m_vecFileDirs.push_back(strFileDir);
}

void KFileDirRule::RemoveFileDir(LPCTSTR szFileDir)
{
	CString strFileDir(szFileDir);
	std::vector<CString> vecTemp;
	std::vector<CString>::iterator iter;

	strFileDir.MakeLower();
	if (KFunction::IsWildcardString(strFileDir))
	{
		for (iter = m_vecWildcardFileDirs.begin(); iter != m_vecWildcardFileDirs.end(); iter++)
		{
			if (*iter == strFileDir)
			{
				m_vecWildcardFileDirs.erase(iter);
				return;
			}
		}
		return;
	}

	for (size_t i = 0; i < m_vecFileDirs.size(); i++)
	{
		vecTemp.push_back(m_vecFileDirs[i]);
	}

	for (size_t i = 0; i < m_vecNotUsed.size(); i++)
	{
		vecTemp.push_back(m_vecNotUsed[i]);
	}

	m_vecFileDirs.clear();
	m_vecNotUsed.clear();

	KFunction::PathAddBackslash(strFileDir);

	for (iter = vecTemp.begin(); iter != vecTemp.end();)
	{
		if (*iter == strFileDir)
		{
			iter = vecTemp.erase(iter);
		}
		else
		{
			AddFileDir(*iter);
			++iter;
		}
	}
}

void KFileDirRule::Clear(void)
{
	m_vecFileDirs.clear();
	m_vecNotUsed.clear();
	m_vecWildcardFileDirs.clear();
}

void KFileDirRule::AddNotusedFileDir(LPCTSTR szFileDir)
{
	for (size_t i = 0; i < m_vecNotUsed.size(); i++)
	{
		if (m_vecNotUsed[i] == szFileDir)
			return;	
	}

	m_vecNotUsed.push_back(szFileDir);
}

void KFileDirRule::UsesNotusedFileDir(void)
{
}