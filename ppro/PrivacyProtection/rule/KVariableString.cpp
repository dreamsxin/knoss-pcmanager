#include "stdafx.h"
#include "KVariableString.h"
#include "KSystemEnvirVar.h"

BOOL KVariableString::SetVariable(LPCTSTR szName, LPCTSTR szString)
{	
	BOOL bReturn = TRUE;
	CString strName(szName);
	CString strString(szString);
	std::map<CString, CString>::iterator iter;

	strName.MakeLower();
	strString.MakeLower();

	iter = m_mapVariable.find(strName);
	if (iter != m_mapVariable.end())
		bReturn = FALSE;
	else
		m_mapVariable.insert(std::make_pair(strName, strString));

	return bReturn;
}

CString KVariableString::GetVariable(LPCTSTR szName)
{
	CString strName(szName);
	std::map<CString, CString>::iterator iter;

	strName.MakeLower();

	iter = m_mapVariable.find(strName);
	if (iter != m_mapVariable.end())
		return iter->second;

	KSystemEnvirVar envir;
	CString strValue = envir.GetValue(szName);
	if (!strValue.IsEmpty())
	{
		strValue.MakeLower();
		m_mapVariable.insert(std::make_pair(strName, strValue));
		return strValue;
	}

	return _T("");
}

BOOL KVariableString::RelpaceVariable(CString& strString)
{
	BOOL bReturn = TRUE;
	int nStartPos;
	int nEndPos;

	CString strTemp(strString);
	CString strVariable;
	CString strValue;

	for (;;)
	{
		nStartPos = strTemp.Find(_T('%'), 0);
		if (nStartPos != -1)
			nEndPos = strTemp.Find(_T('%'), nStartPos + 1);

		if (nStartPos == -1 || nEndPos == -1)
			break;
		
		strVariable = strTemp.Mid(nStartPos, nEndPos - nStartPos + 1);
		strValue = GetVariable(strVariable);
		if (strValue.IsEmpty())
			break;

		strTemp.Replace(strVariable, strValue);
	}

	if (strTemp.Find(_T('%')) != -1)
	{
#ifdef OUTPUT_INIT_WARNING
#ifdef __log_file__
		CString strTempVar(strString);
		strTempVar.Replace(_T("%"), _T("%%"));
		DPrintW(L"KVariableString::RelpaceVariable fail, variablename:%s\n", strTempVar);
		CString strMsg;
		strMsg.Format(_T("查找环境变量失败:%s"), strString);
		::MessageBox(NULL, strMsg, _T("隐私保护器"), MB_OK);
#endif
#endif
		return FALSE;
	}

	do 
	{
		nStartPos = strTemp.Find(_T("\\\\"));
		if (nStartPos == -1)
			break;

		strTemp.Replace(_T("\\\\"), _T("\\"));

	} while (true);

	strString = strTemp;
	
	return TRUE;
}