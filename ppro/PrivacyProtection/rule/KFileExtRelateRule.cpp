#include "stdafx.h"
#include "KFunction.h"
#include "KFileExtRelateRule.h"
#include "KRegister.h"

BOOL KFileExtRelateRule::Match(KAccessData* pData)
{
	int nMemSize = pData->nProcPathLen * sizeof(TCHAR);
	TCHAR* pStrExt = pData->szFilePath + pData->nFilePathLen - pData->nFileExtLen;

	std::map<CString, std::vector<CString>>::iterator iter;
	iter = m_mapExtRelates.find(pStrExt);
	if (iter != m_mapExtRelates.end())
	{
		std::vector<CString>::iterator iterRelate;
		std::vector<CString>& vecStr = iter->second;
		
		for (iterRelate = vecStr.begin(); iterRelate != vecStr.end(); ++iterRelate)
		{
			CString& strTemp = *iterRelate;
			if (strTemp.GetLength() == pData->nProcPathLen &&
				memcmp((LPCTSTR)strTemp, pData->szProcPath, nMemSize) == 0)
			{
				return TRUE;
			}
		}
	}

	/*
	此处可以尝试将GetSystemRelate的数据缓存起来，但是会产生多线程同步的问题
	*/
	CString strRelate;
	GetSystemRelate(pStrExt, strRelate);
	if (strRelate.GetLength() ==  pData->nProcPathLen&& 
		strRelate.CompareNoCase(pData->szProcPath) == 0)
		return TRUE;

	return FALSE;
}

void KFileExtRelateRule::DbgPrint(void)
{
	std::map<CString, std::vector<CString>>::iterator iter;

	DPrintA("KFileExtRelateRule::DbgPrint\n");
	DPrintA("file extension relate count:%d\n", m_mapExtRelates.size());
	for (iter = m_mapExtRelates.begin(); iter != m_mapExtRelates.end(); iter++)
	{
		const CString& strDir = iter->first;
		std::vector<CString>& vecStr = iter->second;

		DPrintA("\tfile ext:%ws\n", strDir);
		for (size_t i = 0; i < vecStr.size(); i++)
		{
			DPrintA("\t\t%d proc:%ws\n", i, vecStr[i]);
		}
	}
	DPrintA("\n\n");
}

void KFileExtRelateRule::AddExtRelate(LPCTSTR szExtName, LPCTSTR szRelateProc)
{
	CString strExtName(szExtName);
	CString strRelateProc(szRelateProc);

	strExtName.MakeLower();
	strRelateProc.MakeLower();

	std::map<CString, std::vector<CString>>::iterator iter;
	
	iter = m_mapExtRelates.find(strExtName);
	if (iter == m_mapExtRelates.end())
	{
		std::vector<CString> vecStr;
		vecStr.push_back(strRelateProc);
		m_mapExtRelates.insert(std::make_pair(strExtName, vecStr));
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

void KFileExtRelateRule::RemoveExtRelate(LPCTSTR szExtName, LPCTSTR szRelateProc)
{
	CString strExtName(szExtName);
	CString strRelateProc(szRelateProc);

	strExtName.MakeLower();
	strRelateProc.MakeLower();

	std::map<CString, std::vector<CString>>::iterator iter;

	iter = m_mapExtRelates.find(strExtName);
	if (iter != m_mapExtRelates.end())
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
					m_mapExtRelates.erase(iter);
				}
				
				return;
			}
		}
	}
}

void KFileExtRelateRule::Clear(void)
{
	m_mapExtRelates.clear();
}

void KFileExtRelateRule::GetSystemRelate(LPCTSTR szExtName, CString& strRelate)
{
	KRegister key;
	KRegister keyShell;
	CString strCmdLine;
	CString strFileDesc;
	CString strCmdRegPath;
	BOOL bRetCode = FALSE;
	
	strRelate.Empty();
	bRetCode = key.Open(HKEY_CLASSES_ROOT, szExtName);
	if (!bRetCode) goto Exit0;

	bRetCode = key.Read(_T(""), strFileDesc);
	if (!bRetCode) goto Exit0;

	strCmdRegPath.Format(_T("%s\\shell\\open\\command"), strFileDesc);
	bRetCode = keyShell.Open(HKEY_CLASSES_ROOT, strCmdRegPath);
	if (!bRetCode) goto Exit0;

	bRetCode = keyShell.Read(_T(""), strCmdLine);
	if (!bRetCode) goto Exit0;

	int nStartPos = 0;
	TCHAR chEndChar = _T(' ');
	if (strCmdLine.GetAt(0) == _T('\"'))
	{
		nStartPos = 1;
		chEndChar = _T('\"');
	}

	int nEndPos = strCmdLine.Find(chEndChar, nStartPos);
	if (nEndPos == -1)
		strRelate = strCmdLine;
	else
		strRelate = strCmdLine.Mid(nStartPos, nEndPos - nStartPos);

	TCHAR szLongPath[MAX_PATH];
	DWORD dwRetCode = GetLongPathName(strRelate, szLongPath, MAX_PATH);
	szLongPath[MAX_PATH - 1] = 0;
	if (dwRetCode != 0)
	{
		strRelate = szLongPath;
	}

Exit0:
	return;
}