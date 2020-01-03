#include "stdafx.h"
#include "KFunction.h"
#include "KFileExtensionRule.h"

BOOL KFileExtensionRule::Match(KAccessData* pData)
{
	if (pData->nFileExtLen == -1)
		return FALSE;
	
	int nMemSize = pData->nFileExtLen * sizeof(TCHAR);
	TCHAR* pExt = pData->szFilePath + pData->nFilePathLen - pData->nFileExtLen;
	std::vector<CString>::iterator iter;
	for (iter = m_vecFileExts.begin(); iter != m_vecFileExts.end(); ++iter)
	{
		if (memcmp(pExt, (LPCTSTR)(*iter), nMemSize) == 0)
			return TRUE;
	}

	return FALSE;
}

void KFileExtensionRule::DbgPrint(void)
{
	DPrintA("KFileExtensionRule::DbgPrint\n");

	DPrintA("file extension count %d:\n", m_vecFileExts.size());
	CString strExts;
	for (size_t i = 0; i < m_vecFileExts.size(); i++)
	{
		strExts += m_vecFileExts[i];
		strExts += _T(", ");
		
	}
	DPrintA("%ws\n", strExts);
	DPrintA("\n\n");
}

void KFileExtensionRule::AddFileExt(LPCTSTR szFileExt)
{
	CString strFileExt(szFileExt);

	strFileExt.MakeLower();

	for (size_t i = 0; i < m_vecFileExts.size(); i++)
	{
		if (m_vecFileExts[i] == strFileExt)
			return;
	}

	m_vecFileExts.push_back(strFileExt);
}

void KFileExtensionRule::RemoveFileExt(LPCTSTR szFileExt)
{
	CString strFileExt(szFileExt);
	std::vector<CString>::iterator iter;
	
	for (iter = m_vecFileExts.begin(); iter != m_vecFileExts.end();)
	{
		if ((*iter) == strFileExt)
		{
			iter = m_vecFileExts.erase(iter);
		}
		else
			++iter;
	}
}

void KFileExtensionRule::Clear(void)
{
	m_vecFileExts.clear();
}