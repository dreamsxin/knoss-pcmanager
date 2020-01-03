#include "stdafx.h"
#include "KFunction.h"
#include "KProcNameCache.h"
#include "KDirRelateRule.h"
#include "KpsRule.h"

BOOL KDirRelateRule::Match(KAccessData* pData)
{
	BOOL bRetCode = FALSE;
	std::map<CString, std::vector<CString>>::iterator iter;

	for (iter = m_mapDirRelates.begin(); iter != m_mapDirRelates.end(); iter++)
	{
		const CString& strDir = iter->first;
		if (strDir.GetLength() <= pData->nFileDirLen &&
			memcmp((LPCTSTR)strDir, pData->szFilePath, strDir.GetLength() * sizeof(TCHAR)) == 0)
		{
			std::vector<CString>::iterator iterRelate;
			std::vector<CString>& vecStr = iter->second;

			for (iterRelate = vecStr.begin(); iterRelate != vecStr.end(); ++iterRelate)
			{
				int nMemSize = pData->nProcPathLen * sizeof(TCHAR);
				CString& strTemp = *iterRelate;
				if (strTemp.GetLength() == pData->nProcPathLen &&
					memcmp((LPCTSTR)strTemp, pData->szProcPath, nMemSize) == 0)
				{
					return TRUE; 
				}
			}
		}
	}

	int nProcNameLen = pData->nProcPathLen - pData->nProcDirLen;
	LPCTSTR pProcStr = pData->szProcPath + pData->nProcDirLen;
	std::map<CString, std::vector<CString>>::iterator iter2;

	for (iter2 = m_mapDirRelates2.begin(); iter2 != m_mapDirRelates2.end(); iter2++)
	{
		const CString& strDir = iter2->first;
		if (strDir.GetLength() <= pData->nFileDirLen &&
			memcmp((LPCTSTR)strDir, pData->szFilePath, strDir.GetLength() * sizeof(TCHAR)) == 0)
		{
			std::vector<CString>::iterator iterProcName;
			std::vector<CString>& vecProcName = iter2->second;
			for (iterProcName = vecProcName.begin(); iterProcName != vecProcName.end(); iterProcName++)
			{
				CString& procname = *iterProcName;
				if (procname.GetLength() == nProcNameLen &&
					memcmp((LPCTSTR)procname, pProcStr, nProcNameLen * 2) == 0)
				{
					return g_procnameCache.MatchProc(pData->szProcPath, pData->nProcDirLen, procname);
				}
			}
		}
	}

	for (iter = m_mapWildcardDirRelates.begin(); iter != m_mapWildcardDirRelates.end(); iter++)
	{
		const CString& strDir = iter->first;
		bRetCode = KRtl::StringWildCard(strDir, pData->szFilePath);
		if (bRetCode)
		{
			std::vector<CString>::iterator iterRelate;
			std::vector<CString>& vecStr = iter->second;

			for (iterRelate = vecStr.begin(); iterRelate != vecStr.end(); ++iterRelate)
			{
				int nMemSize = pData->nProcPathLen * sizeof(TCHAR);
				CString& strTemp = *iterRelate;
				if (strTemp.GetLength() == pData->nProcPathLen &&
					memcmp((LPCTSTR)strTemp, pData->szProcPath, nMemSize) == 0)
				{
					return TRUE; 
				}
			}
		}
	}

	for (iter2 = m_mapWildcardDirRelates2.begin(); iter2 != m_mapWildcardDirRelates2.end(); iter2++)
	{
		const CString& strDir = iter2->first;
		bRetCode = KRtl::StringWildCard(strDir, pData->szFilePath);
		if (bRetCode)
		{
			std::vector<CString>::iterator iterProcName;
			std::vector<CString>& vecProcName = iter2->second;
			for (iterProcName = vecProcName.begin(); iterProcName != vecProcName.end(); iterProcName++)
			{
				CString& procname = *iterProcName;
				if (procname.GetLength() == nProcNameLen &&
					memcmp((LPCTSTR)procname, pProcStr, nProcNameLen * 2) == 0)
				{
					return g_procnameCache.MatchProc(pData->szProcPath, pData->nProcDirLen, procname);
				}
			}
		}
	}

	return FALSE;
}

void KDirRelateRule::DbgPrint(void)
{
	std::map<CString, std::vector<CString>>::iterator iter;
	
	DPrintA("KDirRelateRule::DbgPrint\n");
	for (iter = m_mapDirRelates.begin(); iter != m_mapDirRelates.end(); iter++)
	{
		const CString& strDir = iter->first;
		std::vector<CString>& vecStr = iter->second;

		DPrintA("\tdir:%ws\n", strDir);
		for (size_t i = 0; i < vecStr.size(); i++)
		{
			DPrintA("\t\t%d procpath:%ws\n", i + 1, vecStr[i]);
		}
	}

	for (iter = m_mapWildcardDirRelates.begin(); iter != m_mapWildcardDirRelates.end(); iter++)
	{
		const CString& strDir = iter->first;
		std::vector<CString>& vecStr = iter->second;

		DPrintA("\tdir:%ws\n", strDir);
		for (size_t i = 0; i < vecStr.size(); i++)
		{
			DPrintA("\t\t%d procpath:%ws\n", i + 1, vecStr[i]);
		}
	}

	for (iter = m_mapDirRelates2.begin(); iter != m_mapDirRelates2.end(); iter++)
	{
		const CString& strDir = iter->first;
		std::vector<CString>& vecStr = iter->second;

		DPrintA("\tdir:%ws\n", strDir);
		for (size_t i = 0; i < vecStr.size(); i++)
		{
			DPrintA("\t\t%d procname:%ws\n", i + 1, vecStr[i]);
		}
	}

	for (iter = m_mapWildcardDirRelates2.begin(); iter != m_mapWildcardDirRelates2.end(); iter++)
	{
		const CString& strDir = iter->first;
		std::vector<CString>& vecStr = iter->second;

		DPrintA("\tdir:%ws\n", strDir);
		for (size_t i = 0; i < vecStr.size(); i++)
		{
			DPrintA("\t\t%d procname:%ws\n", i + 1, vecStr[i]);
		}
	}
	DPrintA("\n\n");
}

void KDirRelateRule::AddRelateProcPath(LPCTSTR szDir, LPCTSTR szRelateProcPath)
{
	CString strDir(szDir);
	CString strRelateProc(szRelateProcPath);
	std::map<CString, std::vector<CString>>::iterator iter;

	strDir.MakeLower();
	strRelateProc.MakeLower();

	if (KFunction::IsWildcardString(strDir))
	{
		iter = m_mapWildcardDirRelates.find(strDir);
		if (iter == m_mapWildcardDirRelates.end())
		{
			std::vector<CString> vecStr;
			vecStr.push_back(strRelateProc);
			m_mapWildcardDirRelates.insert(std::make_pair(strDir, vecStr));
		}
		else
		{
			std::vector<CString>& vecStr = iter->second;
			for (size_t i = 0; i < vecStr.size(); i++)
			{
				if (vecStr[i] == strRelateProc)
					return;
			}
			vecStr.push_back(strRelateProc);
		}
		return;
	}

	KFunction::PathAddBackslash(strDir);

	iter = m_mapDirRelates.find(strDir);
	if (iter == m_mapDirRelates.end())
	{
		std::vector<CString> vecStr;
		vecStr.push_back(strRelateProc);
		m_mapDirRelates.insert(std::make_pair(strDir, vecStr));
	}
	else
	{
		std::vector<CString>& vecStr = iter->second;
		for (size_t i = 0; i < vecStr.size(); i++)
		{
			if (vecStr[i] == strRelateProc)
				return;
		}
		vecStr.push_back(strRelateProc);
	}
}

void KDirRelateRule::AddRelateProcName(LPCTSTR szDir, LPCTSTR szProcName)
{
	CString strDir(szDir);
	CString strRelateProc(szProcName);
	std::map<CString, std::vector<CString>>::iterator iter;

	strDir.MakeLower();
	strRelateProc.MakeLower();

	if (KFunction::IsWildcardString(strDir))
	{
		iter = m_mapWildcardDirRelates2.find(strDir);
		if (iter == m_mapWildcardDirRelates2.end())
		{
			std::vector<CString> vecStr;
			vecStr.push_back(strRelateProc);
			m_mapWildcardDirRelates2.insert(std::make_pair(strDir, vecStr));
		}
		else
		{
			std::vector<CString>& vecStr = iter->second;
			for (size_t i = 0; i < vecStr.size(); i++)
			{
				if (vecStr[i] == strRelateProc)
					return;
			}
			vecStr.push_back(strRelateProc);
		}
		return;
	}

	KFunction::PathAddBackslash(strDir);
	iter = m_mapDirRelates2.find(strDir);
	if (iter == m_mapDirRelates2.end())
	{
		std::vector<CString> vecStr;
		vecStr.push_back(strRelateProc);
		m_mapDirRelates2.insert(std::make_pair(strDir, vecStr));
	}
	else
	{
		std::vector<CString>& vecStr = iter->second;
		for (size_t i = 0; i < vecStr.size(); i++)
		{
			if (vecStr[i] == strRelateProc)
				return;
		}
		vecStr.push_back(strRelateProc);
	}
}

void KDirRelateRule::RemoveDirRelate(LPCTSTR szDir, LPCTSTR szRelateProc)
{
	CString strDir(szDir);
	CString strRelateProc(szRelateProc);

	strDir.MakeLower();
	strRelateProc.MakeLower();
	KFunction::PathAddBackslash(strDir);

	std::map<CString, std::vector<CString>>::iterator iter;

	iter = m_mapDirRelates.find(strDir);
	if (iter != m_mapDirRelates.end())
	{
		std::vector<CString>::iterator iterRelate;
		std::vector<CString>& vecStr = iter->second;

		for (iterRelate = vecStr.begin(); iterRelate != vecStr.end(); ++iterRelate)
		{
			if (*iterRelate == strRelateProc)
			{
				vecStr.erase(iterRelate);
				if (vecStr.empty())
				{
					m_mapDirRelates.erase(iter);
				}

				return;
			}
		}
	}
}

void KDirRelateRule::Clear(void)
{
	m_mapDirRelates.clear();
	m_mapDirRelates2.clear();
	m_mapWildcardDirRelates.clear();
	m_mapWildcardDirRelates2.clear();
}
