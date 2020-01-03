#include "stdafx.h"
#include "KSignCache.h"
#include "KProcNameCache.h"

KProcNameCache g_procnameCache;

void KProcNameCache::AddProcName(LPCTSTR szVariableName, LPCTSTR szProcName, LPCSTR szSign, LPCTSTR szFile)
{
	KProcName procname;

	procname.strProcName = szProcName;
	procname.strSign = szSign;
	procname.strFileName = szFile;
	procname.strProcName.MakeLower();
	procname.strFileName.MakeLower();
	procname.strSign.MakeLower();
	
	m_mapProcNameList.insert(std::make_pair(szVariableName, procname));

	KProcNameData data;

	data.strSign = procname.strSign;
	data.strFile = procname.strFileName;

	m_mapProcNameData.insert(std::make_pair(procname.strProcName, data));
}

BOOL KProcNameCache::GetProcName(LPCTSTR szVariableName, KProcName& procname)
{
	std::map<CString, KProcName>::iterator iter;

	iter = m_mapProcNameList.find(szVariableName);
	if (iter != m_mapProcNameList.end())
	{
		procname = iter->second;
		return TRUE;
	}

#ifdef OUTPUT_INIT_WARNING
#ifdef __log_file__
	{
		CString strMsg;
		strMsg.Format(_T("查找进程名变量失败:%s"), szVariableName);
		::MessageBox(NULL, strMsg, _T("隐私保护器"), MB_OK);
	}
#endif
#endif
	return FALSE;
}

BOOL KProcNameCache::MatchProc(LPCTSTR szProcPath, int nDirLen, CString& strProcName)
{
	std::map<CString, KProcNameData>::iterator iter;

	iter = m_mapProcNameData.find(strProcName);
	if (iter == m_mapProcNameData.end())
	{
		m_vecUnknownProcessList.push_back(szProcPath);
		return FALSE;
	}
	else
	{
		KProcNameData& data = iter->second;
		std::vector<CString>::iterator iterProcPath;
		for (iterProcPath = data.vecProcessList.begin(); iterProcPath != data.vecProcessList.end(); ++iterProcPath)
		{
			CString& strTemp = *iterProcPath;
			if (strTemp == szProcPath)
				return TRUE;
		}

		for (iterProcPath = m_vecUnknownProcessList.begin(); iterProcPath != m_vecUnknownProcessList.end(); ++iterProcPath)
		{
			CString& strTemp = *iterProcPath;
			if (strTemp == szProcPath)
				return FALSE;
		}

		if (data.strSign.IsEmpty())
		{
			TCHAR szPath[MAX_PATH];
			_tcsncpy(szPath, szProcPath, nDirLen);
			szPath[nDirLen] = 0;

			_tcscat(szPath, data.strFile);
			if (_taccess(szPath, 0) == 0)
			{
				data.vecProcessList.push_back(szProcPath);
				return TRUE;
			}
			else
			{
				m_vecUnknownProcessList.push_back(szProcPath);
				return FALSE;
			}
		}
		else
		{
			LPCSTR szSign = g_signCache.GetFileSign(szProcPath, nDirLen + strProcName.GetLength());
			if (szSign && data.strSign == szSign) 
			{
				data.vecProcessList.push_back(szProcPath);
				return TRUE;
			}
			else
			{
				m_vecUnknownProcessList.push_back(szProcPath);
				return FALSE;
			}
		}
	}
}

void KProcNameCache::Clear()
{
	m_mapProcNameData.clear();
}

void KProcNameCache::DbgPrint(void)
{
	DPrintA("KProcNameCache::DbgPrint\n");
	DPrintA("ProcNameData:\n");

	std::map<CString, KProcNameData>::iterator iterProcName;
	for (iterProcName = m_mapProcNameData.begin(); iterProcName != m_mapProcNameData.end(); iterProcName++)
	{
		DPrintA("procname:%ws, sign:%s, file:%ws\n", iterProcName->first, iterProcName->second.strSign, iterProcName->second.strFile);
	}

	DPrintA("\n\n");
}