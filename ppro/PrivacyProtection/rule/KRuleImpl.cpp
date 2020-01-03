#include "stdafx.h"
#include "KRuleImpl.h"
#include "KFunction.h"
#include "KRuleConfig.h"
#include "signverify.h"
#include "KXmlWriter.h"
#include "KXmlReader.h"
#include "kutfconv.h"
#include "KPathParser.h"
#include "KProcNameCache.h"
#include "KSignDataRule.h"


KRuleImpl::KRuleImpl()
{
	m_lMatching = 0;
	m_bStopMatch = FALSE;
	m_bEnableMatch = FALSE;
	m_pMainRuleTree = NULL;
	m_hNotifyProcessEvent = NULL;
	m_hNotifyResultEvent = NULL;
	ZeroMemory(m_hMatchThread, sizeof(HANDLE) * defThreadNum);
}

KRuleImpl::~KRuleImpl()
{
	this->Clear();
}

BOOL KRuleImpl::Init(BOOL bAsync /* = TRUE */)
{
	BOOL bReturn = FALSE;
	BOOL bRetCode = FALSE;
	KRuleConfig config;
	CString strModulePath;

	strModulePath = KFunction::GetModulePath(NULL);

	std::wstring strRuleFile = strModulePath + _T("rule.dat");
	std::wstring strCustomRuleFile = strModulePath + _T("custom_rule.dat");

	KXmlWriter m_RuleXml;
	KXmlWriter m_CustomRuleXml;


	bRetCode = m_RuleXml.LoadFile(CStringA(strRuleFile.c_str()));
	if (!bRetCode)
		goto Exit0;	

	m_XmlStreamRule.str("");
	m_XmlStreamRule << m_RuleXml.ToXml();
	m_XmlStreamRule << std::ends;


	bRetCode = m_CustomRuleXml.LoadFile(CStringA(strCustomRuleFile.c_str()));
	if (!bRetCode)
		goto Exit0;	

	m_XmlStreamCustomRule.str("");
	m_XmlStreamCustomRule << m_CustomRuleXml.ToXml();
	m_XmlStreamCustomRule << std::ends;


	bRetCode = config.Init(m_XmlStreamRule, m_XmlStreamCustomRule);
	if (!bRetCode) goto Exit0;

	bRetCode = CSignVerify::Instance().Initialize();
	if (!bRetCode) goto Exit0;

	m_pMainRuleTree = config.GetMainRuleTree();
	m_vecRules = config.GetRules();
	m_vecResults = config.GetDebugResults();
	
#ifdef PRINT_RULE_RESULT
	DbgPrint();
#endif

	m_bMultiThreadMode = bAsync;
	if (bAsync)
	{
		bRetCode = InitMultiScan();
		if (!bRetCode) goto Exit0;
	}

	m_bEnableMatch = TRUE;
	bReturn = TRUE;

Exit0:
	DPrintA("KRuleImpl::Init return:%d\n", bReturn);
	return bReturn;
}

void KRuleImpl::Uninit(void)
{
	m_bStopMatch = TRUE;
	if (m_bMultiThreadMode)
	{
		if (m_hNotifyProcessEvent)
			SetEvent(m_hNotifyProcessEvent);
		if (m_hNotifyResultEvent)
			SetEvent(m_hNotifyResultEvent);

		::WaitForMultipleObjects(defThreadNum, m_hMatchThread, TRUE, INFINITE);
		::WaitForSingleObject(m_hResultThread, INFINITE);
	}

	CSignVerify::Instance().UnInitialize();
}

BOOL KRuleImpl::ReloadRule(void)
{
	BOOL bReturn = FALSE;
	BOOL bRetCode = FALSE;
	KRuleConfig config;
	CString strModulePath;

	m_bEnableMatch = FALSE;

	while (m_lMatching)
		Sleep(1);

	this->Clear();

	strModulePath = KFunction::GetModulePath(NULL);


	bRetCode = config.Init(m_XmlStreamRule, m_XmlStreamCustomRule);
	if (!bRetCode) goto Exit0;

	m_pMainRuleTree = config.GetMainRuleTree();
	m_vecRules = config.GetRules();

	m_bEnableMatch = TRUE;
	bReturn = TRUE;

Exit0:
	DPrintA("KRuleImpl::ReloadRule return:%d\n", bReturn);
	return bReturn;
}

int KRuleImpl::Match(LPCTSTR szFileName, LPCTSTR szProcName)
{
	if (m_bMultiThreadMode)
		return AsyncMatch(szFileName, szProcName);
	else
		return SyncMatch(szFileName, szProcName);
}

BOOL KRuleImpl::InitMultiScan(void)
{
	BOOL bReturn = FALSE;

	m_hNotifyProcessEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hNotifyResultEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!m_hNotifyProcessEvent || ! m_hNotifyResultEvent)
		goto Exit0;

	for (int i = 0; i < defThreadNum; i++)
	{
		m_hMatchThread[i] = ::CreateThread(NULL, 0, KRuleImpl::MatchThreadFunc, this, 0, NULL);
		if (!m_hMatchThread[i]) goto Exit0;
	}

	m_hResultThread = ::CreateThread(NULL, 0, KRuleImpl::ResultThreadFunc, this, 0, NULL);
	if (!m_hResultThread) goto Exit0;

	bReturn = TRUE;

Exit0:
	return bReturn;
}

int KRuleImpl::AsyncMatch(LPCTSTR szFileName, LPCTSTR szProcName)
{
	KAccessData* pData = new KAccessData;

	pData->nProcPathLen= _tcslen(szProcName);
	if (pData->nProcPathLen == 0 || pData->nProcPathLen >= MAX_PATH)
		return emRuleResult_Unknown;

	pData->nFilePathLen= _tcslen(szFileName);
	if (pData->nFilePathLen == 0 || pData->nFilePathLen >= MAX_PATH)
		return emRuleResult_Unknown;

	pData->nFileExtLen = KFunction::GetFileExtCount(szFileName);
	if (pData->nFileExtLen == -1)
		return emRuleResult_Disscard;

	_tcscpy(pData->szProcPath, szProcName);
	_tcscpy(pData->szFilePath, szFileName);

	_tcslwr(pData->szFilePath);
	_tcslwr(pData->szProcPath);

	pData->nProcDirLen = KFunction::GetDirCount(m_accessData.szProcPath);
	pData->nFileDirLen = KFunction::GetDirCount(m_accessData.szFilePath);
	pData->nResult = emRuleResult_Error;
	pData->szInfo[0] = 0;
	pData->pExtendData = NULL;

	{
		KLocker lock(m_lckDataList);
		m_lstDatasList.push_back(pData);
		::SetEvent(m_hNotifyProcessEvent);
	}

	return emRuleResult_Async;
}

int KRuleImpl::SyncMatch(LPCTSTR szFileName, LPCTSTR szProcName)
{
	if (!m_bEnableMatch)
		return emRuleResult_Unknown;
	
	m_accessData.nProcPathLen= _tcslen(szProcName);
	if (m_accessData.nProcPathLen == 0 || m_accessData.nProcPathLen >= MAX_PATH)
		return emRuleResult_Unknown;

	m_accessData.nFilePathLen= _tcslen(szFileName);
	if (m_accessData.nFilePathLen == 0 || m_accessData.nFilePathLen >= MAX_PATH)
		return emRuleResult_Unknown;

	m_accessData.nFileExtLen = KFunction::GetFileExtCount(szFileName);
	if (m_accessData.nFileExtLen == -1)
		return emRuleResult_Disscard;

	_tcscpy(m_accessData.szProcPath, szProcName);
	_tcscpy(m_accessData.szFilePath, szFileName);

	_tcslwr(m_accessData.szFilePath);
	_tcslwr(m_accessData.szProcPath);

	m_accessData.nProcDirLen = KFunction::GetDirCount(m_accessData.szProcPath);
	m_accessData.nFileDirLen = KFunction::GetDirCount(m_accessData.szFilePath);
	m_accessData.nResult = emRuleResult_Error;
	m_accessData.szInfo[0] = 0;
	m_accessData.pExtendData = NULL;

#ifdef PRINT_RULE_RESULT
	CStringA strDbgMsg;
	m_accessData.pExtendData = &strDbgMsg;
#endif
	::InterlockedExchange(&m_lMatching, 1);
	m_pMainRuleTree->Match(&m_accessData);
	::InterlockedExchange(&m_lMatching, 0);
	
#ifdef PRINT_RULE_RESULT
	for (size_t i = 0; i < m_vecResults.size(); i++)
	{
		if (m_vecResults[i] == m_accessData.nResult)
		{
			//DPrintA("result:%d, Process:%ws, File:%ws, match:\n%s\n\n", m_accessData.nResult, m_accessData.szProcPath, m_accessData.szFilePath, strDbgMsg);
			DPrintA("match progress:\n%s", (LPCSTR)strDbgMsg);
			DPrintW(L"result:%d, Process:%ws, File:%ws\n", m_accessData.nResult, m_accessData.szProcPath, m_accessData.szFilePath);
			DPrintW(L"\n\n");

			break;
		}
	}
	
#endif	

	return m_accessData.nResult;
}

void KRuleImpl::DbgMatch(KAccessData* pData)
{
	std::vector<IRule*>::iterator iter;

	for (iter = m_vecRules.begin(); iter != m_vecRules.end(); iter++)
	{
		(*iter)->Match(pData);
	}

	m_pMainRuleTree->Match(&m_accessData);
}

void KRuleImpl::DbgPrint(void)
{
	std::vector<IRule*>::iterator iter;

	for (iter = m_vecRules.begin(); iter != m_vecRules.end(); iter++)
	{
		(*iter)->DbgPrint();
	}

	g_procnameCache.DbgPrint();
}

DWORD WINAPI KRuleImpl::ResultThreadFunc(LPVOID p)
{
	KRuleImpl* pImpl = (KRuleImpl*)p;

	pImpl->RealResultFunc();

	return 0;
}

DWORD KRuleImpl::RealResultFunc(void)
{
	KAccessData* pData;
	std::list<KAccessData*>::iterator iter;
	std::list<KAccessData*> lstResultData;

	do 
	{
		::WaitForSingleObject(m_hNotifyResultEvent, INFINITE);

		if (m_bStopMatch) return 0;

		{
			KLocker lock(m_lckResultList);
			lstResultData = m_lstResultList;
			m_lstResultList.clear();
			::ResetEvent(m_hNotifyResultEvent);
		}

		for (iter = lstResultData.begin(); iter != lstResultData.end(); iter++)
		{
			pData = *iter;
			//DPrintA("result:%d, file:%ws, path:%ws\n", pData->nResult, pData->szFilePath, pData->szProcPath);
			delete pData;
		}

		lstResultData.clear();

	} while (true);

	return 0;
}

DWORD WINAPI KRuleImpl::MatchThreadFunc(LPVOID p)
{
	KRuleImpl* pImpl = (KRuleImpl*)p;

	return pImpl->RealMatchFunc();
}

DWORD KRuleImpl::RealMatchFunc(void)
{
	KAccessData* pAccessData;
	std::list<KAccessData*> lstDataList;
	std::list<KAccessData*>::iterator iter;

	do 
	{
		::WaitForSingleObject(m_hNotifyProcessEvent, INFINITE);

		if (m_bStopMatch) return 0;

		{
			KLocker lock(m_lckDataList);
			size_t size = m_lstDatasList.size();

			if (size < defThreadNum)
			{
				lstDataList = m_lstDatasList;
				m_lstDatasList.clear();
				::ResetEvent(m_hNotifyProcessEvent);
			}
			else
			{
				size = (size + defThreadNum - 1) / defThreadNum;

				do 
				{
					lstDataList.push_back(m_lstDatasList.front());
					m_lstDatasList.pop_front();
				} while (--size);
			}
		}

		for (iter = lstDataList.begin(); iter != lstDataList.end(); iter++)
		{
			m_pMainRuleTree->Match(*iter);
		}

		if (!lstDataList.empty())
		{
			KLocker lock(m_lckResultList);

			do 
			{
				m_lstResultList.push_back(lstDataList.front());
				lstDataList.pop_front();
			} while (!lstDataList.empty());

			::SetEvent(m_hNotifyResultEvent);
		}

	} while (!m_bStopMatch);
	
	return 0;
}

void KRuleImpl::Clear(void)
{
	if (m_pMainRuleTree)
	{
		delete m_pMainRuleTree;
		m_pMainRuleTree = NULL;
	}

	for (int i = 0; i < m_vecRules.size(); i++)
	{
		delete m_vecRules[i];
	}
	m_vecRules.clear();
}

BOOL KRuleImpl::AddPrivacyFile(LPCTSTR szPath)
{
	BOOL bReturn = FALSE;
	BOOL bRetCode = FALSE;
	KXmlWriter xmlWriter;
	KXmlReader xmlReader;
	CString strPrivacyPath(szPath);


	strPrivacyPath.MakeLower();
	
	bRetCode = xmlReader.Parse(m_XmlStreamCustomRule.str().c_str());
	if (!bRetCode) goto Exit0;

	bRetCode = xmlReader.Open("KRuleConfig\\KRules\\KFilePathRule\\PathItem");
	if (bRetCode)
	{
		do 
		{
			CString strTemp;
			xmlReader.ReadText(strTemp);

			if (!strTemp.IsEmpty())
			{
				strTemp.MakeLower();

				if (strTemp == strPrivacyPath)
					return TRUE;
			}
			
			bRetCode = xmlReader.NextSibling();
			if (!bRetCode) break;
			
		} while (true);
	}

	bRetCode = xmlWriter.Parse(m_XmlStreamCustomRule.str().c_str());
	if (!bRetCode) goto Exit0;

	bRetCode = xmlWriter.Open("KRuleConfig\\KRules\\KFilePathRule");
	if (!bRetCode) goto Exit0;

	bRetCode = xmlWriter.AddNewElement("PathItem");
	if (!bRetCode) goto Exit0;

	bRetCode = xmlWriter.WriteText(strPrivacyPath);
	if (!bRetCode) goto Exit0;


	m_XmlStreamCustomRule.str("");
	m_XmlStreamCustomRule << xmlWriter.ToXml();
	m_XmlStreamCustomRule << std::ends;

	
	bReturn = TRUE;
	
Exit0:
	return bReturn;
}

BOOL KRuleImpl::AddPrivacyDir(LPCTSTR szDir)
{
	BOOL bReturn = FALSE;
	BOOL bRetCode = FALSE;
	KXmlWriter xmlWriter;
	KXmlReader xmlReader;
	CString strPrivacyPath(szDir);

	strPrivacyPath.MakeLower();
	KFunction::PathAddBackslash(strPrivacyPath);

	bRetCode = xmlReader.Parse(m_XmlStreamCustomRule.str().c_str());
	if (!bRetCode) goto Exit0;

	bRetCode = xmlReader.Open("KRuleConfig\\KRules\\KFileDirRule\\PathItem");
	if (bRetCode)
	{
		do 
		{
			CString strTemp;
			xmlReader.ReadText(strTemp);

			if (!strTemp.IsEmpty())
			{
				strTemp.MakeLower();
				KFunction::PathAddBackslash(strTemp);

				if (strTemp == strPrivacyPath)
					return TRUE;
			}

			bRetCode = xmlReader.NextSibling();
			if (!bRetCode) break;

		} while (true);
	}

	bRetCode = xmlWriter.Parse(m_XmlStreamCustomRule.str().c_str());
	if (!bRetCode) goto Exit0;

	bRetCode = xmlWriter.Open("KRuleConfig\\KRules\\KFileDirRule");
	if (!bRetCode) goto Exit0;

	bRetCode = xmlWriter.AddNewElement("PathItem");
	if (!bRetCode) goto Exit0;

	bRetCode = xmlWriter.WriteText(strPrivacyPath);
	if (!bRetCode) goto Exit0;

	m_XmlStreamCustomRule.str("");
	m_XmlStreamCustomRule << xmlWriter.ToXml();
	m_XmlStreamCustomRule << std::ends;


	bReturn = TRUE;

Exit0:
	return bReturn;
}

BOOL KRuleImpl::RemovePrivacyFile(LPCTSTR szPath)
{
	BOOL bReturn = FALSE;
	BOOL bRetCode = FALSE;
	TiXmlDocument m_xmlDoc;
	CString strPrivacyPath(szPath);

	strPrivacyPath.MakeLower();


	m_xmlDoc.Parse(m_XmlStreamCustomRule.str().c_str());
	if (m_xmlDoc.Error()) goto Exit0;

	{
		KPathParserA path;
		BOOL bRetCode = FALSE;
		TiXmlElement* pTempElement = NULL;
		TiXmlElement* pParentElement = NULL;
		size_t i = 0, max_size = 0;

		bRetCode = path.Parse("KRuleConfig\\KRules\\KFilePathRule\\PathItem");
		if (!bRetCode) goto Exit0;

		pTempElement = m_xmlDoc.RootElement();

		for (int i = 0; i < path.Size(); i++)
		{
			while (pTempElement)
			{		
				if (lstrcmpA(pTempElement->Value(), path.Get(i)) == 0)
					break;

				pTempElement = pTempElement->NextSiblingElement();
			}
			if (!pTempElement) goto Exit0;
			if (i != path.Size() - 1)
				pTempElement = pTempElement->FirstChildElement();
		}

		CString strText;

		do 
		{
			strText.Empty();
			if (pTempElement)
			{
				strText = UTF8ToTChar(pTempElement->GetText());
			}

			strText.MakeLower();
			if (strText == strPrivacyPath)
			{
				pTempElement->Parent()->RemoveChild(pTempElement);
				break;
			}

			pTempElement = pTempElement->NextSiblingElement();
			if (!pTempElement) goto Exit0;

		} while (true);


		TiXmlPrinter xmlPrinter;
		m_xmlDoc.Accept(&xmlPrinter);

		m_XmlStreamCustomRule.str("");
		m_XmlStreamCustomRule << xmlPrinter.CStr();
		m_XmlStreamCustomRule << std::ends;
	}

	bReturn = TRUE;
Exit0:
	return bReturn;
}

BOOL KRuleImpl::RemovePrivacyDir(LPCTSTR szDir)
{
	BOOL bReturn = FALSE;
	BOOL bRetCode = FALSE;
	TiXmlDocument m_xmlDoc;
	CString strPrivacyPath(szDir);

	strPrivacyPath.MakeLower();
	KFunction::PathAddBackslash(strPrivacyPath);

	m_xmlDoc.Parse(m_XmlStreamCustomRule.str().c_str());
	if (m_xmlDoc.Error()) goto Exit0;

	{
		KPathParserA path;
		BOOL bRetCode = FALSE;
		TiXmlElement* pTempElement = NULL;
		TiXmlElement* pParentElement = NULL;
		size_t i = 0, max_size = 0;

		bRetCode = path.Parse("KRuleConfig\\KRules\\KFileDirRule\\PathItem");
		if (!bRetCode) goto Exit0;

		pTempElement = m_xmlDoc.RootElement();

		for (int i = 0; i < path.Size(); i++)
		{
			while (pTempElement)
			{		
				if (lstrcmpA(pTempElement->Value(), path.Get(i)) == 0)
					break;

				pTempElement = pTempElement->NextSiblingElement();
			}
			if (!pTempElement) goto Exit0;
			if (i != path.Size() - 1)
				pTempElement = pTempElement->FirstChildElement();
		}

		CString strText;

		do 
		{
			strText.Empty();
			if (pTempElement)
			{
				strText = UTF8ToTChar(pTempElement->GetText());
			}

			strText.MakeLower();
			KFunction::PathAddBackslash(strText);
			
			if (strText == strPrivacyPath)
			{
				pTempElement->Parent()->RemoveChild(pTempElement);
				break;
			}

			pTempElement = pTempElement->NextSiblingElement();
			if (!pTempElement) goto Exit0;

		} while (true);


		TiXmlPrinter xmlPrinter;
		m_xmlDoc.Accept(&xmlPrinter);

		m_XmlStreamCustomRule.str("");
		m_XmlStreamCustomRule << xmlPrinter.CStr();
		m_XmlStreamCustomRule << std::ends;
	}

	bReturn = TRUE;
Exit0:
	return bReturn;
}

BOOL KRuleImpl::AddMonitorProcess(LPCTSTR szPath)
{
	BOOL bReturn = FALSE;
	BOOL bRetCode = FALSE;
	KXmlWriter xmlWriter;
	KXmlReader xmlReader;
	CString strPrivacyPath(szPath);

	strPrivacyPath.MakeLower();

	bRetCode = xmlReader.Parse(m_XmlStreamCustomRule.str().c_str());
	if (!bRetCode) goto Exit0;

	bRetCode = xmlReader.Open("KRuleConfig\\KRules\\KProcPathRule\\PathItem");
	if (bRetCode)
	{
		do 
		{
			CString strTemp;
			xmlReader.ReadText(strTemp);

			if (!strTemp.IsEmpty())
			{
				strTemp.MakeLower();
				KFunction::PathAddBackslash(strTemp);

				if (strTemp == strPrivacyPath)
					return TRUE;
			}

			bRetCode = xmlReader.NextSibling();
			if (!bRetCode) break;

		} while (true);
	}

	bRetCode = xmlWriter.Parse(m_XmlStreamCustomRule.str().c_str());
	if (!bRetCode) goto Exit0;

	bRetCode = xmlWriter.Open("KRuleConfig\\KRules\\KProcPathRule");
	if (!bRetCode) goto Exit0;

	bRetCode = xmlWriter.AddNewElement("PathItem");
	if (!bRetCode) goto Exit0;

	bRetCode = xmlWriter.WriteText(strPrivacyPath);
	if (!bRetCode) goto Exit0;


	m_XmlStreamCustomRule.str("");
	m_XmlStreamCustomRule << xmlWriter.ToXml();
	m_XmlStreamCustomRule << std::ends;

	bReturn = TRUE;

Exit0:
	return bReturn;
}

BOOL KRuleImpl::RemoveMonitorProcess(LPCTSTR szPath)
{
	BOOL bReturn = FALSE;
	BOOL bRetCode = FALSE;
	TiXmlDocument m_xmlDoc;
	CString strMonitorProcPath(szPath);

	strMonitorProcPath.MakeLower();
	KFunction::PathAddBackslash(strMonitorProcPath);


	m_xmlDoc.Parse(m_XmlStreamCustomRule.str().c_str());
	if (m_xmlDoc.Error()) goto Exit0;

	{
		KPathParserA path;
		BOOL bRetCode = FALSE;
		TiXmlElement* pTempElement = NULL;
		TiXmlElement* pParentElement = NULL;
		size_t i = 0, max_size = 0;

		bRetCode = path.Parse("KRuleConfig\\KRules\\KProcPathRule\\PathItem");
		if (!bRetCode) goto Exit0;

		pTempElement = m_xmlDoc.RootElement();

		for (int i = 0; i < path.Size(); i++)
		{
			while (pTempElement)
			{		
				if (lstrcmpA(pTempElement->Value(), path.Get(i)) == 0)
					break;

				pTempElement = pTempElement->NextSiblingElement();
			}
			if (!pTempElement) goto Exit0;
			if (i != path.Size() - 1)
				pTempElement = pTempElement->FirstChildElement();
		}

		CString strText;

		do 
		{
			strText.Empty();
			if (pTempElement)
			{
				strText = UTF8ToTChar(pTempElement->GetText());
			}

			strText.MakeLower();
			KFunction::PathAddBackslash(strText);

			if (strText == strMonitorProcPath)
			{
				pTempElement->Parent()->RemoveChild(pTempElement);
				break;
			}

			pTempElement = pTempElement->NextSiblingElement();
			if (!pTempElement) goto Exit0;

		} while (true);

		TiXmlPrinter xmlPrinter;
		m_xmlDoc.Accept(&xmlPrinter);

		m_XmlStreamCustomRule.str("");
		m_XmlStreamCustomRule << xmlPrinter.CStr();
		m_XmlStreamCustomRule << std::ends;
	}

	bReturn = TRUE;
Exit0:
	return bReturn;
}

