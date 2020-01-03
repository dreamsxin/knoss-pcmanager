#include "stdafx.h"
#include "KFunction.h"
#include "KFileNameRelateRule.h"

BOOL KFileNameRelateRule::Match(KAccessData* pData)
{
	TCHAR* pFileName = pData->szFilePath + pData->nFileDirLen;

	std::map<CString, std::vector<CString>>::iterator iter;
	iter = m_mapFileNameRelates.find(pFileName);
	if (iter != m_mapFileNameRelates.end())
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

	return FALSE;
}

void KFileNameRelateRule::DbgPrint(void)
{
	std::map<CString, std::vector<CString>>::iterator iter;

	DPrintA("KFileNameRelateRule::DbgPrint\n");
	DPrintA("filename relate count:%d\n", m_mapFileNameRelates.size());
	for (iter = m_mapFileNameRelates.begin(); iter != m_mapFileNameRelates.end(); iter++)
	{
		const CString& strDir = iter->first;
		std::vector<CString>& vecStr = iter->second;

		DPrintA("\tfile name:%ws\n", strDir);
		for (size_t i = 0; i < vecStr.size(); i++)
		{
			DPrintA("\t\t%d proc:%ws\n", i, vecStr[i]);
		}
	}
	DPrintA("\n\n");
}

void KFileNameRelateRule::AddFileNameRelate(LPCTSTR szFileName, LPCTSTR szRelateProc)
{
	CString strFileName(szFileName);
	CString strRelate(szRelateProc);

	strFileName.MakeLower();
	strRelate.MakeLower();

	std::map<CString, std::vector<CString>>::iterator iter;

	iter = m_mapFileNameRelates.find(strFileName);
	if (iter == m_mapFileNameRelates.end())
	{
		std::vector<CString> vecStr;
		vecStr.push_back(strRelate);
		m_mapFileNameRelates.insert(std::make_pair(strFileName, vecStr));
	}
	else
	{
		std::vector<CString>& vecStr = iter->second;
		for (size_t i = 0; i < vecStr.size(); i++)
		{
			if (vecStr[i] == strRelate)
				return;
		}
		vecStr.push_back(strRelate);
	}
}

void KFileNameRelateRule::RemoveFileNameRelate(LPCTSTR szFileName, LPCTSTR szRelateProc)
{
	CString strFileName(szFileName);
	CString strRelateProc(szRelateProc);

	strFileName.MakeLower();
	strRelateProc.MakeLower();

	std::map<CString, std::vector<CString>>::iterator iter;

	iter = m_mapFileNameRelates.find(strFileName);
	if (iter != m_mapFileNameRelates.end())
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
					m_mapFileNameRelates.erase(iter);
				}
				
				return;
			}
		}
	}
}

void KFileNameRelateRule::Clear(void)
{
	m_mapFileNameRelates.clear();
}
