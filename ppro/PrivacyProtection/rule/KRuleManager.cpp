// KRuleManager.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "KFileExtensionRule.h"
#include "KProcFileRule.h"
#include "KFileDirRule.h"
#include "KFilePathRule.h"
#include "KFileExtRelateRule.h"
#include "KFileNameRelateRule.h"
#include "KDirRelateRule.h"
#include "KProcPathRule.h"
#include "KSignDataRule.h"
#include "KVariableString.h"
#include "KRuleConfig.h"
#include "KTree.h"
#include "KRuleImpl.h"
#include "KRuleTestImpl.h"
#include "signverify.h"
#include "KSignCache.h"
#include "KProcNameCache.h"

void TestFileExtensionRule(KAccessData* pData)
{
	KFileExtensionRule rule;

	rule.AddFileExt(_T(".jpg"));
	rule.AddFileExt(_T(".html"));
	rule.AddFileExt(_T(".dll"));
	rule.AddFileExt(_T(".html"));
	rule.AddFileExt(_T(".DLL"));
	rule.RemoveFileExt(_T(".dll"));

	BOOL bRetCode = rule.Match(pData);
	DPrintA("KFileExtensionRule::Match return:%d\n", bRetCode);
}

void TestKProcFileRule(KAccessData* pData)
{
	KProcFileRule rule;

	BOOL bRetCode = rule.Match(pData);
	DPrintA("KProcFileRule::Match return:%d\n", bRetCode);
}

void TestKFileDirRule(KAccessData* pData)
{
	KFileDirRule rule;
	
	rule.AddFileDir(_T("c:\\a"));
	rule.AddFileDir(_T("c:\\a\\b"));
	rule.AddFileDir(_T("c:\\a\\b\\c"));
	rule.AddFileDir(_T("c:\\d\\e\\f"));
	rule.AddFileDir(_T("c:\\d\\e"));
	rule.AddFileDir(_T("c:\\d"));
	rule.RemoveFileDir(_T("c:\\a"));
	rule.RemoveFileDir(_T("c:\\d"));

	BOOL bRetCode = rule.Match(pData);
	DPrintA("KFileDirRule::Match return:%d\n", bRetCode);
}

void TestKFilePathRule(KAccessData* pData)
{
	KFilePathRule rule;

	rule.AddFilePath(_T("c:\\a\\b\\c.doc"));
	rule.AddFilePath(_T("c:\\a\\b\\e.doc"));
	rule.AddFilePath(_T("c:\\a\\b\\f.doc"));
	rule.RemoveFilePath(_T("c:\\a\\b\\F.DOC"));

	BOOL bRetCode = rule.Match(pData);
	DPrintA("KFilePathRule::Match return:%d\n", bRetCode);
}

void TestKFileExtRelateRule(KAccessData* pData)
{
	KFileExtRelateRule rule;

	rule.AddExtRelate(_T(".mp3"), _T("D:\\Program Files\\StormII\\Storm.exe"));
	rule.AddExtRelate(_T(".mp3"), _T("D:\\Program Files\\StormII\\a.exe"));
	rule.AddExtRelate(_T(".mp3"), _T("D:\\Program Files\\StormII\\A.EXE"));
	rule.RemoveExtRelate(_T(".mp3"), _T("D:\\Program Files\\StormII\\A.EXE"));

	BOOL bRetCode = rule.Match(pData);
	DPrintA("KFileExtRelateRule::Match return:%d\n", bRetCode);
}

void TestKFileNameRelateRule(KAccessData* pData)
{
	KFileNameRelateRule rule;

	rule.AddFileNameRelate(_T("a.doc"), _T("D:\\Program Files\\StormII\\Storm.exe"));
	rule.AddFileNameRelate(_T("B.DOC"), _T("D:\\Program Files\\StormII\\a.exe"));
	rule.AddFileNameRelate(_T("b.DOC"), _T("D:\\Program Files\\StormII\\A.EXE"));
	rule.RemoveFileNameRelate(_T("A.doc"), _T("D:\\Program Files\\StormII\\Storm.exe"));

	BOOL bRetCode = rule.Match(pData);
	DPrintA("KFileNameRelateRule::Match return:%d\n", bRetCode);
}

void TestKDirRelateRule(KAccessData* pData)
{
	KDirRelateRule rule;

	rule.AddRelateProcPath(_T("c:\\a"), _T("D:\\a\\a.exe"));
	rule.AddRelateProcPath(_T("c:\\a\\b"), _T("D:\\a\\b.exe"));
	rule.AddRelateProcPath(_T("c:\\a\\b"), _T("D:\\a\\c.exe"));
	rule.AddRelateProcPath(_T("c:\\c"), _T("D:\\Program Files\\StormII\\A.EXE"));
	rule.AddRelateProcPath(_T("c:\\c"), _T("D:\\Program Files\\StormII\\b.EXE"));
	rule.RemoveDirRelate(_T("c:\\c"), _T("D:\\Program Files\\StormII\\A.EXE"));
	rule.RemoveDirRelate(_T("c:\\c"), _T("D:\\Program Files\\StormII\\b.EXE"));
	
	BOOL bRetCode = rule.Match(pData);
	DPrintA("KDirRelateRule::Match return:%d\n", bRetCode);
}

void TestKProcPathRule(KAccessData* pData)
{
	KProcPathRule rule;

	rule.AddProcPath(_T("c:\\a\\b\\c.exe"));
	rule.AddProcPath(_T("c:\\a\\b\\e.exe"));
	rule.AddProcPath(_T("c:\\a\\b\\E.exe"));
	rule.RemoveProcPath(_T("c:\\a\\b\\c.exe"));

	BOOL bRetCode = rule.Match(pData);
	DPrintA("KProcPathRule::Match return:%d\n", bRetCode);
}

void TestKSignDataRule(KAccessData* pData)
{
	KSignDataRule rule;

	rule.AddSign("kingsoft");
	rule.AddSign("Microsoft");
	rule.AddSign("abcdef");
	rule.RemoveSign("abcdef");

	BOOL bRetCode = rule.Match(pData);
	DPrintA("KSignDataRule::Match return:%d\n", bRetCode);
}

void TestKVariableString(void)
{
	KVariableString var;
	BOOL bRetCode = FALSE;

	bRetCode = var.SetVariable(_T("%systemroot%"), _T("c:\\windows\\"));
	if (!bRetCode) DPrintA("SetVariable return false\n");
	bRetCode = var.SetVariable(_T("%temp%"), _T("c:\\windows\\Temp\\"));
	if (!bRetCode) DPrintA("SetVariable return false\n");
	
	CString strPath = _T("%systemroot%\\a.exe");
	var.RelpaceVariable(strPath);
}

void TestCreateRules(void)
{
	BOOL bRetCode = FALSE;
	KRuleConfig config;
	std::vector<IRule*> vecRules;

	bRetCode = config.Init(_T("c:\\rule.xml"), _T("c:\\custom_rule.xml"));
	DPrintA("config.Init return:%d\n", bRetCode);
	if (!bRetCode) goto Exit0;

	vecRules = config.GetRules();
	DPrintA("rule count:%d\n", vecRules.size());
	for (size_t i = 0; i < vecRules.size(); i++)
	{
		DPrintA("\t%d:%ws\n", i, vecRules[i]->Name());
	}

Exit0:
	return;
}

void TestTree(void)
{
	KTree<int> tree;
	
	tree.SetValue(0);
	tree.AddLeft(1);
	tree.AddRight(2);
}

void TestRule(void)
{
	{
		KAccessData data;

		_tcscpy(data.szProcPath, _T("c:\\WINDOWS\\system32\\notepad.exe"));
		_tcscpy(data.szFilePath, _T("c:\\WINDOWS\\system32\\apphelp.dll"));
		data.nProcPathLen= _tcslen(data.szProcPath);
		data.nFilePathLen = _tcslen(data.szFilePath);

		TestFileExtensionRule(&data);
	}
	
	{
		KAccessData data;

		_tcscpy(data.szProcPath, _T("c:\\WINDOWS\\system32\\d\\notepad.exe"));
		_tcscpy(data.szFilePath, _T("c:\\WINDOWS\\system32\\abc\\apphelp.dll"));
		data.nProcPathLen= _tcslen(data.szProcPath);
		data.nFilePathLen = _tcslen(data.szFilePath);

		TestKProcFileRule(&data);
	}

	{
		KAccessData data;

		_tcscpy(data.szProcPath, _T("c:\\a\\b\\c\\a.exe"));
		_tcscpy(data.szFilePath, _T("c:\\a\\b\\c\\b.doc"));
		data.nProcPathLen= _tcslen(data.szProcPath);
		data.nFilePathLen = _tcslen(data.szFilePath);

		TestKFileDirRule(&data);
	}

	{
		KAccessData data;

		_tcscpy(data.szProcPath, _T("c:\\a\\b\\c\\a.exe"));
		_tcscpy(data.szFilePath, _T("c:\\a\\b\\c.doc"));
		data.nProcPathLen= _tcslen(data.szProcPath);
		data.nFilePathLen = _tcslen(data.szFilePath);

		TestKFilePathRule(&data);
	}

	{
		KAccessData data;

		_tcscpy(data.szProcPath, _T("d:\\program files\\stormii\\storm.exe"));
		_tcscpy(data.szFilePath, _T("c:\\a.mp3"));
		data.nProcPathLen= _tcslen(data.szProcPath);
		data.nFilePathLen = _tcslen(data.szFilePath);

		TestKFileExtRelateRule(&data);
	}

	{
		KAccessData data;

		_tcscpy(data.szProcPath, _T("c:\\program files\\winamp\\winamp.exe"));
		_tcscpy(data.szFilePath, _T("c:\\a.mp9"));
		data.nProcPathLen= _tcslen(data.szProcPath);
		data.nFilePathLen = _tcslen(data.szFilePath);

		TestKFileExtRelateRule(&data);
	}

	{
		KAccessData data;

		_tcscpy(data.szProcPath, _T("d:\\program files\\stormii\\a.exe"));
		_tcscpy(data.szFilePath, _T("c:\\b.doc"));
		data.nProcPathLen= _tcslen(data.szProcPath);
		data.nFilePathLen = _tcslen(data.szFilePath);


		TestKFileNameRelateRule(&data);
	}

	{
		KAccessData data;

		_tcscpy(data.szProcPath, _T("d:\\a\\b.exe"));
		_tcscpy(data.szFilePath, _T("c:\\a\\b\\b.doc"));
		data.nProcPathLen= _tcslen(data.szProcPath);
		data.nFilePathLen = _tcslen(data.szFilePath);

		TestKDirRelateRule(&data);
	}
	
	{
		KAccessData data;
		_tcscpy(data.szProcPath, _T("c:\\a\\b\\e.exe"));
		_tcscpy(data.szFilePath, _T("c:\\a\\b\\b.doc"));
		data.nProcPathLen= _tcslen(data.szProcPath);
		data.nFilePathLen = _tcslen(data.szFilePath);

		TestKProcPathRule(&data);
	}

	{
		KAccessData data;
		_tcscpy(data.szProcPath, _T("c:\\WINDOWS\\system32\\notepad.exe"));
		_tcscpy(data.szFilePath, _T("c:\\a\\b\\b.doc"));
		data.nProcPathLen= _tcslen(data.szProcPath);
		data.nFilePathLen = _tcslen(data.szFilePath);

		TestKSignDataRule(&data);
	}

	{
		KAccessData data;
		_tcscpy(data.szProcPath, _T("c:\\Program Files\\Common Files\\Kingsoft\\kiscommon\\kisuisp.exe"));
		_tcscpy(data.szFilePath, _T("c:\\a\\b\\b.doc"));
		data.nProcPathLen= _tcslen(data.szProcPath);
		data.nFilePathLen = _tcslen(data.szFilePath);

		TestKSignDataRule(&data);
	}

	TestKVariableString();

	TestCreateRules();

	TestTree();
}

TCHAR* g_pszStringArray[] = {
	_T("c:\\a.doc"), _T("c:\\a.exe"),
	_T("c:\\b\\b.xml"), _T("c:\\a.exe"),
	_T("F:\\MP3\\Backstreet Boys-The One.mp4"), _T("D:\\Program Files\\StormII\\Storm.exe"),
	_T("F:\\a.doc"), _T("c:\\windows\\system32\\notepad.exe"),
	_T("F:\\a.doc"), _T("c:\\windows\\notepad.exe"),
};

void TestRuleImpl(void)
{
	BOOL bRetCode = FALSE;
	int nResult;
	KRuleImpl ruleImpl;

	bRetCode = ruleImpl.Init(FALSE);
	DPrintA("KRuleImpl::Init return:%d\n", bRetCode);
	if (!bRetCode) return;
	
	//do 
	//{
		for (size_t i = 0; i < sizeof(g_pszStringArray) / sizeof(TCHAR*); i += 2)
		{
			nResult = ruleImpl.Match(g_pszStringArray[i], g_pszStringArray[i + 1]);
			DPrintA("%2d result:%d, file:%ws, proc:%ws\n", i / 2, nResult, g_pszStringArray[i], g_pszStringArray[i + 1]);
			printf("%2d result:%d, file:%ws, proc:%ws\n", i / 2, nResult, g_pszStringArray[i], g_pszStringArray[i + 1]);
		}
		Sleep(1);
	//} while (true);
	
	ruleImpl.Uninit();
}

DWORD WINAPI MatchThreadFunc(LPVOID p)
{
	int nResult;
	KRuleImpl* pImpl = (KRuleImpl*)p;
	KAccessData accessData;

	do 
	{
		for (size_t i = 0; i < sizeof(g_pszStringArray) / sizeof(TCHAR*); i += 2)
		{
			accessData.nProcPathLen= _tcslen(g_pszStringArray[i + 1]);
			if (accessData.nProcPathLen == 0 || accessData.nProcPathLen >= MAX_PATH)
				return emRuleResult_Unknown;

			accessData.nFilePathLen= _tcslen(g_pszStringArray[i]);
			if (accessData.nFilePathLen == 0 || accessData.nFilePathLen >= MAX_PATH)
				return emRuleResult_Unknown;

			_tcscpy(accessData.szProcPath, g_pszStringArray[i + 1]);
			_tcscpy(accessData.szFilePath, g_pszStringArray[i]);

			_tcslwr(accessData.szFilePath);
			_tcslwr(accessData.szProcPath);

			pImpl->DbgMatch(&accessData);
			DPrintA("%2d result:%d, file:%ws, proc:%ws\n", i, nResult, g_pszStringArray[i], g_pszStringArray[i + 1]);
		}

	} while (true);
}

void TestRuleImplMultiThread(void)
{
	BOOL bRetCode = FALSE;
	int nResult;
	KRuleImpl ruleImpl;

	bRetCode = ruleImpl.Init();
	DPrintA("KRuleImpl::Init return:%d\n", bRetCode);
	if (!bRetCode) return;

	const int cnThreadCount = 4;
	HANDLE hThread[cnThreadCount];

	for (int i = 0; i < cnThreadCount; i++)
	{
		hThread[i] = ::CreateThread(NULL, 0, MatchThreadFunc, &ruleImpl, 0, NULL);
	}

	::WaitForMultipleObjects(cnThreadCount, hThread, TRUE, INFINITE);
}

void TestRuleTestImpl(void)
{
	BOOL bRetCode = 0;
	KRuleTestImpl testImpl;

	bRetCode = testImpl.Init(_T("c:\\testrule.xml"));
	if (!bRetCode)
	{
		printf("KRuleTestImpl::Init fail\n");
		return;
	}

	testImpl.DebugMatch();
}

void TestSign(void)
{
	CSignVerify::Instance().Initialize();
	BOOL bMicroSoft = FALSE;
	CStringA strSign;
	DWORD dwTickTime = ::GetTickCount();
	
	BOOL bReturn = CSignVerify::Instance().GetSignData(_T("D:\\Program Files\\Kingsoft\\Kingsoft Antivirus\\kmembers_.dll"), bMicroSoft, strSign);
	//BOOL bReturn = CSignVerify::Instance().GetSignData(_T("c:\\windows\\system32\\notepad.exe"), bMicroSoft, strSign);
	//BOOL bReturn = CSignVerify::Instance().GetSignData(_T("D:\\Program Files\\Tencent\\QQ\\Bin\\QQ.exe"), bMicroSoft, strSign);
	//BOOL bReturn = CSignVerify::Instance().GetSignData(_T("F:\\文件备份\\Esai_Cs1.6 2738.exe"), bMicroSoft, strSign);
	printf("GetSignData return:%d, bMicrosoft:%d, sign:%s, time:%d\n", bReturn, bMicroSoft, strSign, ::GetTickCount() - dwTickTime);
}

void TestSignCacheAndProcNameCache(void)
{
	BOOL bRetCode = FALSE;
	TCHAR* arrayFiles[] = 
	{
		_T("d:\\kismain.exe"),
		_T("c:\\windows\\notepad.exe"),
		_T("d:\\kxetray.exe"),
		_T("d:\\kxeserv.exe")
	};
	
	for (size_t i = 0; i < sizeof(arrayFiles) / sizeof(TCHAR*); i++)
	{
		LPCSTR szSign = g_signCache.GetFileSign(arrayFiles[i], _tcslen(arrayFiles[i]));
		printf("%d:sign:%s, file:%ws\n", i, szSign, arrayFiles[i]);
	}

	for (size_t i = 0; i < sizeof(arrayFiles) / sizeof(TCHAR*); i++)
	{
		LPCSTR szSign = g_signCache.GetFileSign(arrayFiles[i], _tcslen(arrayFiles[i]));
		printf("%d:sign:%s, file:%ws\n", i, szSign, arrayFiles[i]);
	}
	
	g_procnameCache.AddProcName(_T("%kismain%"), _T("kismain.exe"), "", _T("kxefmsys.log"));
	g_procnameCache.AddProcName(_T("%kxetray%"), _T("kxetray.exe"), "\"Zhuhai  Kingsoft Software Co.", _T(""));

	CString strProcName;
	CString strProcPath;

	strProcPath = _T("d:\\kismain.exe");
	strProcName = _T("kismain.exe");
	bRetCode = g_procnameCache.MatchProc(strProcPath, 3, strProcName);
	printf("MatchProc return:%d, procpat:%ws, procname:%ws\n", bRetCode, strProcPath, strProcName);
	
	strProcPath = _T("d:\\kxetray.exe");
	strProcName = _T("kxetray.exe");
	bRetCode = g_procnameCache.MatchProc(strProcPath, 3, strProcName);
	printf("MatchProc return:%d, procpat:%ws, procname:%ws\n", bRetCode, strProcPath, strProcName);


	strProcPath = _T("e:\\kingsoft\\kisengine\\trunk\\product\\win32\\kismain.exe");
	strProcName = _T("kismain.exe");
	bRetCode = g_procnameCache.MatchProc(strProcPath, 3, strProcName);
	printf("MatchProc return:%d, procpat:%ws, procname:%ws\n", bRetCode, strProcPath, strProcName);
}

void TestUserConfig(void)
{
	BOOL bRetCode = FALSE;
	KRuleImpl ruleImpl;

	bRetCode = ruleImpl.Init();
	if (!bRetCode) return;

	ruleImpl.AddPrivacyFile(_T("c:\\test\\a.dat"));
	ruleImpl.AddPrivacyDir(_T("c:\\test\\a\\"));
	ruleImpl.AddMonitorProcess(_T("c:\\test\\a\\a.exe"));

	ruleImpl.ReloadRule();
	
	ruleImpl.RemovePrivacyFile(_T("c:\\test\\a.dat"));
	ruleImpl.RemovePrivacyDir(_T("c:\\test\\a\\"));
	ruleImpl.RemoveMonitorProcess(_T("c:\\test\\a\\a.exe"));

	ruleImpl.ReloadRule();
}

void TestLoadRule(void)
{
	KRuleImpl ruleImpl;

	BOOL bRetCode = ruleImpl.Init();
	DPrintA("ruleImpl.Init return:%d\n", bRetCode);
	if (!bRetCode) return;

	ruleImpl.DbgPrint();
}

int _tmain(int argc, _TCHAR* argv[])
{
	CSignVerify::Instance().Initialize();
	//TestRule();
	//TestRuleImpl();
	//TestRuleImplMultiThread();
	//TestRuleTestImpl();
	//TestSign();
	//TestSignCacheAndProcNameCache();

	//TestUserConfig();
	TestLoadRule();
	return 0;
}

