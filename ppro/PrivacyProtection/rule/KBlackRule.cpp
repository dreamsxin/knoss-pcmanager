#include "stdafx.h"
#include "KFunction.h"
#include "KBlackRule.h"

BOOL KBlackRule::Match(KAccessData* pData)
{
	std::map<CString, std::vector<CString>>::iterator iter;

	iter = m_mapBlackProcFile.find(pData->szProcPath);
	if (iter != m_mapBlackProcFile.end())
	{
		std::vector<CString>::iterator iterFiles;
		std::vector<CString>& vecStr = iter->second;

		for (iterFiles = vecStr.begin(); iterFiles != vecStr.end(); ++iterFiles)
		{
			CString& strTemp = *iterFiles;
			if (strTemp.GetLength() <= pData->nFilePathLen &&
				memcmp((LPCTSTR)strTemp, pData->szFilePath, strTemp.GetLength() * sizeof(TCHAR)) == 0)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

void KBlackRule::DbgPrint(void)
{
	std::map<CString, std::vector<CString>>::iterator iter;

	DPrintA("KDirRelateRule::DbgPrint\n");
	for (iter = m_mapBlackProcFile.begin(); iter != m_mapBlackProcFile.end(); iter++)
	{
		const CString& strProcess = iter->first;
		std::vector<CString>& vecStr = iter->second;

		DPrintA("\tprocess:%ws\n", strProcess);
		for (size_t i = 0; i < vecStr.size(); i++)
		{
			DPrintA("\t\t%d path:%s\n", i, vecStr[i]);
		}
	}
	DPrintA("\n\n");
}

void KBlackRule::AddProcFile(LPCTSTR szProcess, LPCTSTR szFile)
{
	CString strProcess(szProcess);
	CString strFile(szFile);

	strProcess.MakeLower();
	strFile.MakeLower();

	std::map<CString, std::vector<CString>>::iterator iter;

	iter = m_mapBlackProcFile.find(strProcess);
	if (iter == m_mapBlackProcFile.end())
	{
		std::vector<CString> vecStr;
		vecStr.push_back(strFile);
		m_mapBlackProcFile.insert(std::make_pair(strProcess, vecStr));
	}
	else
	{
		std::vector<CString>& vecStr = iter->second;
		for (size_t i = 0; i < vecStr.size(); i++)
		{
			if (vecStr[i] == strFile)
				return;
		}
		vecStr.push_back(strFile);
	}
}

void KBlackRule::Clear()
{
	m_mapBlackProcFile.clear();
}