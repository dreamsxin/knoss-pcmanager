#include "stdafx.h"
#include "KFunction.h"
#include "KFileNameRule.h"

BOOL KFileNameRule::Match(KAccessData* pData)
{
	std::vector<CString>::iterator iter;
	TCHAR* pFileStr = pData->szFilePath + pData->nFileDirLen;
	int nMemSize = (pData->nFilePathLen - pData->nFileDirLen) * sizeof(TCHAR);

	for (iter = m_vecFileNames.begin(); iter != m_vecFileNames.end(); ++iter)
	{
		CString& strTemp = *iter;
		if (strTemp.GetLength() == pData->nFilePathLen - pData->nFileDirLen &&
			memcmp(pFileStr , (LPCTSTR)strTemp, nMemSize) == 0)
			return TRUE;
	}

	return FALSE;
}

void KFileNameRule::DbgPrint(void)
{
	DPrintA("KFileNameRule::DbgPrint\n");

	DPrintA("filename count %d:\n", m_vecFileNames.size());
	for (size_t i = 0; i < m_vecFileNames.size(); i++)
	{
		DPrintA("\t%d:%ws\n", i + 1, m_vecFileNames[i]);
	}

	DPrintA("\n\n");
}

void KFileNameRule::AddFileName(LPCTSTR szFileName)
{
	CString strFileName(szFileName);

	strFileName.MakeLower();

	for (size_t i = 0; i < m_vecFileNames.size(); i++)
	{
		if (m_vecFileNames[i] == strFileName)
			return;
	}

	m_vecFileNames.push_back(strFileName);
}

void KFileNameRule::Clear(void)
{
	m_vecFileNames.clear();
}