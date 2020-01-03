#include "stdafx.h"
#include "KFunction.h"
#include "KRuleConfig.h"
#include "KRuleTestImpl.h"

BOOL KRuleTestImpl::Init(LPCTSTR szConfigFile)
{
	BOOL bReturn = FALSE;
	BOOL bRetCode = FALSE;

	bRetCode = m_ruleImpl.Init();
	if (!bRetCode) goto Exit0;

	bRetCode = LoadTestData(szConfigFile);
	if (!bRetCode) goto Exit0;

	bReturn = TRUE;

Exit0:
	DPrintA("KRuleTestImpl::Init return:%d\n", bReturn);
	return bReturn;
}

/*
<KRuleTest>
	<Item proc="c:\a.exe" file="c:\a.doc">
	<Item proc="c:\a.exe" file="c:\a.doc">
</KRuleTest>
*/
BOOL KRuleTestImpl::LoadTestData(LPCTSTR szConfigFile)
{
	BOOL bReturn = FALSE;
	BOOL bRetCode = FALSE;
	CString strProcPath;
	CString strFilePath;
	KXmlReader xmlReader;
	KRuleConfig ruleConfig;
	CString strModulePath;

	strModulePath = KFunction::GetModulePath(NULL);
	bRetCode = ruleConfig.Init(strModulePath + _T("rule.xml"), strModulePath + _T("custom_rule.xml"));
	if (!bRetCode) goto Exit0;

	KVariableString* pVariable = ruleConfig.GetVariableString();
	
	if (szConfigFile)
	{
		USES_CONVERSION;
		bRetCode = xmlReader.LoadFile(T2A(szConfigFile));
		if (!bRetCode) goto Exit0;
	}
	else
	{
		USES_CONVERSION;
		bRetCode = xmlReader.LoadFile(T2A(strModulePath + _T("testrule.xml")));
		if (!bRetCode) goto Exit0;
	}

	bRetCode = xmlReader.Open("KRuleTest\\Item");
	if (!bRetCode) goto Exit0;

	do 
	{
		xmlReader.Read("proc", strProcPath);
		xmlReader.Read("file", strFilePath);
		
		if (strProcPath.IsEmpty() || strFilePath.IsEmpty())
		{
			DPrintW(L"KRuleTestImpl::LoadTestData error, proc:%s, file:%s\n", strProcPath, strFilePath);
		}
		else if (pVariable->RelpaceVariable(strProcPath) && pVariable->RelpaceVariable(strFilePath))
		{
			m_vecTestData.push_back(strProcPath);
			m_vecTestData.push_back(strFilePath);
		}
		else
		{
			DPrintW(L"KRuleTestImpl::LoadTestData RelpaceVariable error, proc:%s, file:%s\n", strProcPath, strFilePath);
		}

		bRetCode = xmlReader.NextSibling();
		if (!bRetCode) break;
		
	} while (true);

	bReturn = TRUE;
Exit0:
	return bReturn;
}

void KRuleTestImpl::DebugMatch(void)
{
	int nResult;
	std::vector<CString>::iterator iter;
	
	CString strLogPath = ::KFunction::GetModulePath(NULL);
	strLogPath += _T("testrule.log");

	WriteStringToFileA(strLogPath, "test data count:%d\n", m_vecTestData.size() / 2);
	for (size_t i = 0; i < m_vecTestData.size(); i += 2)
	{
		CString& strProcPath = m_vecTestData[i];
		CString& strFilePath = m_vecTestData[i + 1];
		
		nResult = m_ruleImpl.Match(strFilePath, strProcPath);
		DPrintW(L"%02d:result:%d, proc:%s, file:%s\n", i / 2 + 1, nResult, strProcPath, strFilePath);
		USES_CONVERSION;
		WriteStringToFileA(strLogPath, "%02d:result:%d, proc:%s, file:%s\n", i / 2 + 1, nResult, W2A_CP((LPCWSTR)strProcPath, CP_UTF8), W2A_CP((LPCWSTR)strFilePath, CP_UTF8));
	}
	WriteStringToFileA(strLogPath, "\n\n");
}

int KRuleTestImpl::WriteStringToFileA(LPCTSTR szPath, const char* format, ...)
{
	int n_return = 0;
	const int cn_buf_len = 500;
	char log_buffer[cn_buf_len] = {0};
	va_list arglist;
	FILE* fp = NULL;

	va_start(arglist, format);

	n_return = _vsnprintf(log_buffer, cn_buf_len - 1, format, arglist);

	va_end(arglist);

	fp = _tfopen(szPath, _T("a+"));
	if (!fp) return 0;

	fprintf(fp, "%s", log_buffer);
	printf("%s", log_buffer);

	fclose(fp);

	return n_return;
}