#include "stdafx.h"
#include "KRegister.h"
#include "KFunction.h"
#include "KRuleConfig.h"
#include "KDirRelateRule.h"
#include "KFileDirRule.h"
#include "KFileExtensionRule.h"
#include "KFileExtRelateRule.h"
#include "KFileNameRelateRule.h"
#include "KFilePathRule.h"
#include "KProcFileRule.h"
#include "KProcFileRule.h"
#include "KProcPathRule.h"
#include "KRuleConfig.h"
#include "KSignDataRule.h"
#include "KResultRule.h"
#include "KBlackRule.h"
#include "KWhiteRule.h"
#include "KSystemEnvirVar.h"
#include "KRuleTree.h"
#include "KProcNameCache.h"
#include "KFileNameRule.h"
/*
<?xml version="1.0" encoding="UTF-8" ?>
<KRuleConfig>
	<Debug resultlist="0, 1, 2, 3, 4, 5" />

	<ProcName>
		<ProcItem name="xxx" procname="xxx" sign="xxx" filename="xxx" />
	</ProcName>
	
	<SystemEnvironmentPath>
		<PathItem>%UserDocumentPath%</PathItem>
		<PathItem>%SystemRoot%</PathItem>
	</SystemEnvironmentPath>
	
	<RegString>
		<RegItem name="%FreeimePath%" rootkey="HKEY_LOCAL_MACHINE" subkey="SOFTWARE\freeime" valuename="system"/>
	</RegString>

	<KRules>
		<KFileExtensionRule extension=".jpg .xml .doc">
			<Desc>文件扩展名规则，判断文件扩展名</Desc>
		</KFileExtensionRule>

		<KProcFileRule>
			<Desc>文件进程同一目录规则，判断文件是否在进程目录下</Desc>
		</KProcFileRule>

		<KWhiteRule>
			<Desc>文件全路径规则，判断文件路径是否在指定的文件列表中</Desc>
			<Item dir="%v%xxx" path="%v%xxx"/>
		</KWhiteRule>

		<KFilePathRule>
			<Desc>文件全路径规则，判断文件路径是否在指定的文件列表中</Desc>
			<PathItem>c:\a.doc</PathItem>
			<PathItem>%UserDocumentPath%\b.doc</PathItem>
		</KFilePathRule>

		<KFileDirRule>
			<Desc>文件目录规则，判断文件目录是否在指定的目录列表中</Desc>
			<PathItem>%FreeimePath%</PathItem>
			<PathItem>%UserDocumentPath%</PathItem>
		</KFileDirRule>

		<KProcPathRule>
			<Desc>进程全路径规则，判断进程路径是否在指定的文件列表中</Desc>
			<PathItem>c:\a.exe</PathItem>
			<PathItem>%UserDocumentPath%\b.exe</PathItem>
		</KProcPathRule>

		<KProcDirRule>
			<Desc>进程目录规则，判断进程目录是否在指定的目录列表中</Desc>
			<PathItem>c:\a.exe</PathItem>
			<PathItem>%UserDocumentPath%\b.exe</PathItem>
		</KProcDirRule>

		<KFileExtRelateRule>
			<Desc>文件扩展名关联进程与自定义关联进程规则，判断文件默认打开进程是否在系统定义进程或者自定义进程列表中</Desc>
			<ExtItem ext=".jpg" proc="%ImageViewPath%" />
			<ExtItem ext=".jpg" proc="%ImageViewPath2%" />
		</KFileExtRelateRule>

		<KFileNameRelateRule>
			<Desc>文件名关联进程规则，判断文件默认打开进程是否在系统定义进程或者自定义进程列表</Desc>
			<FileItem filename="msg2.0.db" proc="%QQPath%"/>
		</KFileNameRelateRule>

		<KDirRelateRule>
			<Desc>文件路径关联进程规则，判断访问目录的进程是否在自定义关联的进程列表中</Desc>
			<DirItem dir="%IEHistoryPath%" proc="%MathonPath%"/>
		</KDirRelateRule>

		<KBlackRule>
			<Desc>黑名单规则，判断进程名与文件名是否在列表中，目录必须以\结尾</Desc>
			<Item process="c:\a.exe" file="c:\a.doc" />
			<Item process="c:\a.exe" dir="c:\b\" />
		</KBlackRule>

		<KSignDataRule>
			<Desc>数字签名规则，判断文件的数字签名是否要放行</Desc>
			<SignItem sign="xxx" />
		</KSignDataRule>

	</KRules>

	<KRuleTree>
		<!-- type:rule, ruletree, result -->
		<SubTree>
			<Tree name="subtree1" >
			</Tree>
			<Tree name="subtree2" >
			</Tree>
		</SubTree>
		
		<MainTree>
			<RootNode type="rule" rule="KFileExtensionRule">
				<LeftNode type="result" result="1" info="xxx" />
				<RightNode type="rule" rule="KProcFileRule">
					<LeftNode type="rule" rule="KFilePathRule">
					</LeftNode>
					<RightNode type="ruletree" tree="subtree1"/>
				</RightNode>
			</RootNode>
		</MainTree>
		
	</KRuleTree>
</KRuleConfig>
*/

KRuleConfig::KRuleConfig()
{
	m_pMainRuleTree = NULL;
}

KRuleConfig::~KRuleConfig()
{
	
}

BOOL KRuleConfig::Init(LPCTSTR szDefConfigFile, LPCTSTR szUserConfigFile)
{
	BOOL bReturn = FALSE;
	BOOL bRetCode = FALSE;

	USES_CONVERSION;
	bRetCode = m_xmlDefReader.LoadFile(T2A(szDefConfigFile));
	if (!bRetCode) goto Exit0;

	if (_taccess(szUserConfigFile, 0) == 0)
	{
		bRetCode = m_xmlUserReader.LoadFile(T2A(szUserConfigFile));
		if (!bRetCode) goto Exit0;
	}

	bRetCode = LoadVariableString();
	if (!bRetCode) goto Exit0;

	bRetCode = CreateRules();
	if (!bRetCode) goto Exit0;

	bRetCode = CreateRuleTree();
	if (!bRetCode) goto Exit0;

	LoadDebugResults();

	bReturn = TRUE;
	
Exit0:
	return bReturn;
}

BOOL KRuleConfig::Init(const std::stringstream& strRuleStream, const std::stringstream& strCustomRuleStream)
{
	BOOL bReturn = FALSE;
	BOOL bRetCode = FALSE;


	bRetCode = m_xmlDefReader.Parse(strRuleStream.str().c_str());
	if (!bRetCode) 
		goto Exit0;

	bRetCode = m_xmlUserReader.Parse(strCustomRuleStream.str().c_str());
	if (!bRetCode) 
		goto Exit0;


	bRetCode = LoadVariableString();
	if (!bRetCode) goto Exit0;

	bRetCode = CreateRules();
	if (!bRetCode) goto Exit0;

	bRetCode = CreateRuleTree();
	if (!bRetCode) goto Exit0;

	LoadDebugResults();

	bReturn = TRUE;

Exit0:
	return bReturn;
}

void KRuleConfig::Uninit(void)
{
	
}

CString KRuleConfig::ReadRegString(LPCTSTR szRootKey, LPCTSTR szSubKey, LPCTSTR szValueName)
{
	BOOL bRetCode = FALSE;
	KRegister reg;
	CString strValue;

	HKEY hRootKey = GetRootKey(szRootKey);
	if (!hRootKey) goto Exit0;

	bRetCode = reg.Open(hRootKey, szSubKey);
	if (!bRetCode) goto Exit0;

	bRetCode = reg.Read(szValueName, strValue);
	if (!bRetCode) goto Exit0;

Exit0:
	return strValue;
}

HKEY KRuleConfig::GetRootKey(LPCTSTR szRootKey)
{
	HKEY hKey = NULL;

	if (_tcsicmp(szRootKey, _T("HKEY_CLASSES_ROOT")) == 0)
		hKey = HKEY_CLASSES_ROOT;
	else if (_tcsicmp(szRootKey, _T("HKEY_CURRENT_CONFIG")) == 0)
		hKey = HKEY_CURRENT_CONFIG;
	else if (_tcsicmp(szRootKey, _T("HKEY_CURRENT_USER")) == 0)
		hKey = HKEY_CURRENT_USER;
	else if (_tcsicmp(szRootKey, _T("HKEY_LOCAL_MACHINE")) == 0)
		hKey = HKEY_LOCAL_MACHINE;
	else if (_tcsicmp(szRootKey, _T("HKEY_USERS")) == 0)
		hKey = HKEY_USERS;

	return hKey;
}

BOOL KRuleConfig::LoadVariableString(void)
{
	BOOL bRetCode = FALSE;

	bRetCode = LoadRegVariableString();
	if (!bRetCode) return FALSE;

	bRetCode = LoadSystemVariableString();
	if (!bRetCode) return FALSE;

	bRetCode = LoadProcNameList();
	if (!bRetCode) return FALSE;

	return TRUE;
}

/*
<KRuleConfig>
	<ProcName>
		<ProcItem name="xxx" procname="xxx" sign="xxx" filename="xxx" />
	</ProcName>
</KRuleConfig>
*/
BOOL KRuleConfig::LoadProcNameList(void)
{
	BOOL bReturn = FALSE;
	BOOL bRetCode = FALSE;
	CString strName;
	CString strProcName;
	CString strSign;
	CString strFileName;

	bRetCode = m_xmlDefReader.Open("KRuleConfig\\ProcessName\\ProcItem");
	if (!bRetCode) 
	{
		bReturn = TRUE;
		goto Exit0;
	}

	do 
	{
		m_xmlDefReader.Read("name", strName);
		m_xmlDefReader.Read("procname", strProcName);
		m_xmlDefReader.Read("sign", strSign);
		m_xmlDefReader.Read("filename", strFileName);

		if (strName.IsEmpty() ||
			strProcName.IsEmpty() ||
			(strSign.IsEmpty() && strFileName.IsEmpty()))
		{
		}
		else
		{
			USES_CONVERSION;
			g_procnameCache.AddProcName(strName, strProcName, W2A(strSign), strFileName);
		}

		strName.Replace(_T("%"), _T("%%"));
		DPrintA("LoadProcNameList error, name:%ws, procname:%ws, sign:%ws, filename:%ws\n", strName, strProcName, strSign, strFileName);

		bRetCode = m_xmlDefReader.NextSibling();
		if (!bRetCode) break;

	} while (true);

	bReturn = TRUE;

Exit0:
	return bReturn;
}

/*
<KRuleConfig>
	<RegString>
		<RegItem name="%v3%" rootkey="HKEY_LOCAL_MACHINE", subkey="xxx" valuename="xxx"/>
		<RegItem name="%v4%" rootkey="HKEY_LOCAL_MACHINE", subkey="xxx" valuename="xxx"/>
	</RegString>
</KRuleConfig>
*/
BOOL KRuleConfig::LoadRegVariableString(void)
{
	BOOL bReturn = FALSE;
	BOOL bRetCode = FALSE;
	CString strName;
	CString strRootKey;
	CString strSubKey;
	CString strValueName;
	CString strValue;

	bRetCode = m_xmlDefReader.Open("KRuleConfig\\RegString\\RegItem");
	if (!bRetCode) 
	{
		bReturn = TRUE;
		goto Exit0;
	}

	do 
	{
		m_xmlDefReader.Read("name", strName);
		m_xmlDefReader.Read("rootkey", strRootKey);
		m_xmlDefReader.Read("subkey", strSubKey);
		m_xmlDefReader.Read("valuename", strValueName);

		if (strName.IsEmpty() ||
			strRootKey.IsEmpty() ||
			strSubKey.IsEmpty())
		{
			DPrintA("LoadRegVariableString error, name:%ws, rootkey:%ws, subkey:%ws, valuename:%ws\n", strName, strRootKey, strSubKey, strValueName);
		}
		else
		{
			strValue = ReadRegString(strRootKey, strSubKey, strValueName);
			if (!strValue.IsEmpty())
				m_varString.SetVariable(strName, strValue);

			CString strTempName(strName);
			strTempName.Replace(_T("%"), _T("%%"));
			DPrintW(L"LoadRegVariableString name:%s, rootkey:%ws, subkey:%ws, valuename:%ws, value:%s\n", 
				(LPCTSTR)strTempName, strRootKey, strSubKey, strValueName, (LPCTSTR)strValue);
		}

		bRetCode = m_xmlDefReader.NextSibling();
		if (!bRetCode) break;
		
	} while (true);
	
	bReturn = TRUE;

Exit0:
	return bReturn;
}

BOOL KRuleConfig::LoadSystemVariableString(void)
{
	CString strValue;
	KSystemEnvirVar sysVarString;
	TCHAR* arraySysVarName[] = {
		_T("%Desktop%"),
		_T("%Internet%"),
		_T("%Programs%"),
		_T("%Controls%"),
		_T("%Printers%"),
		_T("%Personal%"),
		_T("%Favorites%"),
		_T("%Startup%"),
		_T("%Recent%"),
		_T("%Sendto%"),
		_T("%Bitbucket%"),
		_T("%StartMenu%"),
		_T("%Mydocuments%"),
		_T("%Mymusic%"),
		_T("%Myvideo%"),
		_T("%Desktopdirectory%"),
		_T("%Drives%"),
		_T("%Network%"),
		_T("%Nethood%"),
		_T("%Fonts%"),
		_T("%Templates%"),
		_T("%CommonStartMenu%"),
		_T("%CommonPrograms%"),
		_T("%CommonStartup%"),
		_T("%LocalAppdate%"),
		_T("%CommonAppdata%"),
		_T("%Windows%"),
		_T("%System32%"),
		_T("%ProgramFilesComm%"),
		_T("%CommonDocuments%"),
	};

	for (size_t i = 0; i < sizeof(arraySysVarName) / sizeof(TCHAR*); i++)
	{
		strValue = sysVarString.GetValue(arraySysVarName[i]);
		{
			CString strName(arraySysVarName[i]);
			strName.Replace(_T("%"), _T("%%"));
			DPrintW(L"LoadSystemVariableString name:%s, value:%s\n", (LPCTSTR)strName, (LPCTSTR)strValue);
		}
		
		if (!strValue.IsEmpty())
			m_varString.SetVariable(arraySysVarName[i], strValue);
	}

	return TRUE;
}

BOOL KRuleConfig::CreateRules(void)
{
	IRule* pRule = NULL;

	pRule = CreateKDirRelateRule(m_xmlDefReader);
	AddRule(pRule);

	pRule = CreateKDirRelateRule(m_xmlUserReader);
	AddRule(pRule);

	pRule = CreateKFileDirRule(m_xmlDefReader);
	AddRule(pRule);

	pRule = CreateKFileDirRule(m_xmlUserReader);
	AddRule(pRule);

	pRule = CreateKFileExtensionRule(m_xmlDefReader);
	AddRule(pRule);

	pRule = CreateKFileExtensionRule(m_xmlUserReader);
	AddRule(pRule);

	pRule = CreateKFileExtRelateRule(m_xmlDefReader);
	AddRule(pRule);

	pRule = CreateKFileExtRelateRule(m_xmlUserReader);
	AddRule(pRule);

	pRule = CreateKFileNameRelateRule(m_xmlDefReader);
	AddRule(pRule);

	pRule = CreateKFileNameRelateRule(m_xmlUserReader);
	AddRule(pRule);

	pRule = CreateKFilePathRule(m_xmlDefReader);
	AddRule(pRule);

	pRule = CreateKFilePathRule(m_xmlUserReader);
	AddRule(pRule);

	pRule = CreateKProcFileRule(m_xmlDefReader);
	AddRule(pRule);

	pRule = CreateKProcFileRule(m_xmlUserReader);
	AddRule(pRule);

	pRule = CreateKProcPathRule(m_xmlDefReader);
	AddRule(pRule);

	pRule = CreateKProcPathRule(m_xmlUserReader);
	AddRule(pRule);

	pRule = CreateKSignDataRule(m_xmlDefReader);
	AddRule(pRule);

	pRule = CreateKSignDataRule(m_xmlUserReader);
	AddRule(pRule);

	pRule = CreateKWhiteRule(m_xmlDefReader);
	AddRule(pRule);

	pRule = CreateKWhiteRule(m_xmlUserReader);
	AddRule(pRule);

	pRule = CreateKBlackRule(m_xmlDefReader);
	AddRule(pRule);

	pRule = CreateKBlackRule(m_xmlUserReader);
	AddRule(pRule);

	pRule = CreateKFileNameRule(m_xmlDefReader);
	AddRule(pRule);

	pRule = CreateKFileNameRule(m_xmlUserReader);
	AddRule(pRule);

	return TRUE;
}

BOOL KRuleConfig::CreateRuleTree(void)
{
	BOOL bRetCode = FALSE;

	bRetCode = CreateSubRuleTrees();
	if (!bRetCode) return FALSE;

	bRetCode = CreateMainRuleTree();
	if (!bRetCode) return FALSE;

	return TRUE;
}

/*
<KRuleConfig>
	<KRules>
		<KDirRelateRule>
			<DirItem dir="%v%xxx" proc="%v%xxx"/>
			<DirItem dir="%v%xxx" proc="%v%xxx"/>
		</KDirRelateRule>
	<KRules>
<KRuleConfig>
*/
IRule* KRuleConfig::CreateKDirRelateRule(KXmlReader& xmlReader)
{
	BOOL bRetCode = FALSE;
	CString strDir;
	CString strProc;
	CString strProcName;
	KDirRelateRule* pRule = NULL;

	pRule = (KDirRelateRule*)GetRuleByName(_T("KDirRelateRule"));
	if (!pRule) 
		pRule = new KDirRelateRule;

	bRetCode = xmlReader.Open("KRuleConfig\\KRules\\KDirRelateRule\\DirItem");
	if (!bRetCode) goto Exit0;

	do 
	{
		xmlReader.Read("dir", strDir);
		xmlReader.Read("proc", strProc);
		xmlReader.Read("procname", strProcName);

		if (strDir.IsEmpty() || (strProc.IsEmpty() && strProcName.IsEmpty()))
		{
			DPrintA("CreateKDirRelateRule error, dir:%ws, proc:%ws\n", (LPCTSTR)strDir, (LPCTSTR)strProc);
		}
		
		if (strProc.IsEmpty())
		{
			KProcName procname;

			if (m_varString.RelpaceVariable(strDir) && g_procnameCache.GetProcName(strProcName, procname))
			{
				pRule->AddRelateProcName(strDir, procname.strProcName);
			}
			CString strTemp(strProcName);
			strTemp.Replace(_T("%"), _T("%%"));
			DPrintA("CreateKDirRelateRule dir:%ws, variablename=%ws, procname:%ws, sign:%s, file:%ws\n", (LPCTSTR)strDir, strTemp, (LPCTSTR)procname.strProcName, procname.strSign, procname.strFileName);
		}
		else
		{
			if (m_varString.RelpaceVariable(strDir) &&
				m_varString.RelpaceVariable(strProc))
			{
				pRule->AddRelateProcPath(strDir, strProc);	
			}
			DPrintA("CreateKDirRelateRule dir:%ws, procpath:%ws\n", (LPCTSTR)strDir, (LPCTSTR)strProc);
		}

		bRetCode = xmlReader.NextSibling();
		if (!bRetCode) break;
		
	} while (true);

Exit0:
	return pRule;
}

/*
<KRuleConfig>
	<KRules>
		<KFileDirRule>
			<PathItem>%v%xxx</PathItem>
			<PathItem>%v%xxx</PathItem>
		</KFileDirRule>
	<KRules>
<KRuleConfig>
*/
IRule* KRuleConfig::CreateKFileDirRule(KXmlReader& xmlReader)
{
	BOOL bRetCode = FALSE;
	CString strFileDir;
	KFileDirRule* pRule = NULL;

	pRule = (KFileDirRule*)GetRuleByName(_T("KFileDirRule"));
	if (!pRule) 
		pRule = new KFileDirRule;

	bRetCode = xmlReader.Open("KRuleConfig\\KRules\\KFileDirRule\\PathItem");
	if (!bRetCode) goto Exit0;

	do 
	{
		xmlReader.ReadText(strFileDir);

		if (strFileDir.IsEmpty())
		{
			DPrintA("CreateKFileDirRule error, dir:%ws\n", (LPCTSTR)strFileDir);
		}
		else if (m_varString.RelpaceVariable(strFileDir))
		{
			pRule->AddFileDir(strFileDir);	
			DPrintA("CreateKFileDirRule privacy dir:%ws\n", (LPCTSTR)strFileDir);			
		}
		else
			DPrintA("CreateKFileDirRule repace fail privacy dir:%ws\n", (LPCTSTR)strFileDir);			

		bRetCode = xmlReader.NextSibling();
		if (!bRetCode) break;
	} while (true);

Exit0:
	return pRule;
}

/*
<KRuleConfig>
	<KRules>
		<KFileExtensionRule extension=".jpg .xml .doc">
		</KFileExtensionRule>
	<KRules>
<KRuleConfig>
*/
IRule* KRuleConfig::CreateKFileExtensionRule(KXmlReader& xmlReader)
{
	BOOL bRetCode = FALSE;
	CString strOneExt;
	CString strExtensions;
	KFileExtensionRule* pRule = NULL;

	pRule = (KFileExtensionRule*)GetRuleByName(_T("KFileExtensionRule"));
	if (!pRule) 
		pRule = new KFileExtensionRule;

	bRetCode = xmlReader.Open("KRuleConfig\\KRules\\KFileExtensionRule");
	if (!bRetCode) goto Exit0;

	xmlReader.Read("extension", strExtensions);
	if (strExtensions.IsEmpty()) goto Exit0;

	int nStartPos = 0;
	int nEndPos = 0;

	do 
	{
		nEndPos = strExtensions.Find(_T(' '), nStartPos);
		if (nEndPos == -1)
		{
			strOneExt = strExtensions.Mid(nStartPos, strExtensions.GetLength());
			strOneExt.Remove(_T(' '));
			if (!strOneExt.IsEmpty())
				pRule->AddFileExt(strOneExt);

			break;
		}
		
		strOneExt = strExtensions.Mid(nStartPos, nEndPos - nStartPos);
		strOneExt.Remove(_T(' '));
		if (!strOneExt.IsEmpty())
			pRule->AddFileExt(strOneExt);
		
		nStartPos = nEndPos + 1;
	} while (true);

Exit0:
	return pRule;
}

/*
<KRuleConfig>
	<KRules>
		<KFileExtRelateRule>
			<ExtItem ext=".jpg" proc="%v%xxx" />
			<ExtItem ext=".jpg" proc="%v%xxx" />
		</KFileExtRelateRule>
	<KRules>
<KRuleConfig>
*/
IRule* KRuleConfig::CreateKFileExtRelateRule(KXmlReader& xmlReader)
{
	BOOL bRetCode = FALSE;
	CString strExt;
	CString strProc;
	KFileExtRelateRule* pRule = NULL;

	pRule = (KFileExtRelateRule*)GetRuleByName(_T("KFileExtRelateRule"));
	if (!pRule) 
		pRule = new KFileExtRelateRule;

	bRetCode = xmlReader.Open("KRuleConfig\\KRules\\KFileExtRelateRule\\ExtItem");
	if (!bRetCode) goto Exit0;

	do 
	{
		xmlReader.Read("ext", strExt);
		xmlReader.Read("proc", strProc);

		if (strExt.IsEmpty() || strProc.IsEmpty())
		{
			DPrintA("CreateKDirRelateRule error, ext:%ws, proc:%ws\n", (LPCTSTR)strExt, (LPCTSTR)strProc);
		}
		else if (m_varString.RelpaceVariable(strProc))
		{
			pRule->AddExtRelate(strExt, strProc);	
		}
		else
		{
			DPrintA("CreateKFileExtRelateRule RelpaceVariable fail, ext:%ws, proc:%ws\n", (LPCTSTR)strExt, (LPCTSTR)strProc);
		}

		bRetCode = xmlReader.NextSibling();
		if (!bRetCode) break;

	} while (true);

Exit0:
	return pRule;
}

/*
<KRuleConfig>
	<KRules>
		<KFileNameRelateRule>
			<FileItem filename="xxx" relate="%v%xxx"/>
			<FileItem filename="xxx" relate="%v%xxx"/>
		</KFileNameRelateRule>
	<KRules>
<KRuleConfig>
*/
IRule* KRuleConfig::CreateKFileNameRelateRule(KXmlReader& xmlReader)
{
	BOOL bRetCode = FALSE;
	CString strFileName;
	CString strProc;
	KFileNameRelateRule* pRule = NULL;

	pRule = (KFileNameRelateRule*)GetRuleByName(_T("KFileNameRelateRule"));
	if (!pRule) 
		pRule = new KFileNameRelateRule;

	bRetCode = xmlReader.Open("KRuleConfig\\KRules\\KFileNameRelateRule\\FileItem");
	if (!bRetCode) goto Exit0;

	do 
	{
		xmlReader.Read("filename", strFileName);
		xmlReader.Read("proc", strProc);

		if (strFileName.IsEmpty() || strProc.IsEmpty())
		{
			DPrintA("CreateKFileNameRelateRule error, filename:%ws, proc:%ws\n", (LPCTSTR)strFileName, (LPCTSTR)strProc);
		}
		else if (m_varString.RelpaceVariable(strProc))
		{
			pRule->AddFileNameRelate(strFileName, strProc);	
		}
		else
		{
			DPrintA("CreateKFileExtRelateRule RelpaceVariable fail, ext:%ws, proc:%ws\n", (LPCTSTR)strFileName, (LPCTSTR)strProc);
		}

		bRetCode = xmlReader.NextSibling();
		if (!bRetCode) break;

	} while (true);

Exit0:
	return pRule;
}

/*
<KRuleConfig>
	<KRules>
		<KFilePathRule>
			<PathItem>%v%xxx</PathItem>
			<PathItem>%v%xxx</PathItem>
		</KFilePathRule>
	<KRules>
<KRuleConfig>
*/
IRule* KRuleConfig::CreateKFilePathRule(KXmlReader& xmlReader)
{
	BOOL bRetCode = FALSE;
	CString strFilePath;
	KFilePathRule* pRule = NULL;

	pRule = (KFilePathRule*)GetRuleByName(_T("KFilePathRule"));
	if (!pRule) 
		pRule = new KFilePathRule;

	bRetCode = xmlReader.Open("KRuleConfig\\KRules\\KFilePathRule\\PathItem");
	if (!bRetCode) goto Exit0;

	do 
	{
		xmlReader.ReadText(strFilePath);

		if (strFilePath.IsEmpty())
		{
			DPrintA("CreateKFileDirRule error, dir:%ws\n", (LPCTSTR)strFilePath);
		}
		else if (m_varString.RelpaceVariable(strFilePath))
		{
			pRule->AddFilePath(strFilePath);	
		}
		else
		{
			DPrintA("CreateKFileDirRule RelpaceVariable fail, dir:%ws\n", (LPCTSTR)strFilePath);
		}

		bRetCode = xmlReader.NextSibling();
		if (!bRetCode) break;
	} while (true);

Exit0:
	return pRule;
}

/*
<KRuleConfig>
	<KRules>
		<KProcFileRule>
		</KProcFileRule>
	<KRules>
<KRuleConfig>
*/
IRule* KRuleConfig::CreateKProcFileRule(KXmlReader& xmlReader)
{
	KProcFileRule* pRule = NULL;

	pRule = (KProcFileRule*)GetRuleByName(_T("KProcFileRule"));
	if (!pRule) 
		pRule = new KProcFileRule;
	
	return pRule;
}

/*
<KRuleConfig>
	<KRules>
		<KProcPathRule>
			<PathItem>%v%xxx</PathItem>
			<PathItem>%v%xxx</PathItem>
		</KProcPathRule>
	<KRules>
<KRuleConfig>
*/
IRule* KRuleConfig::CreateKProcPathRule(KXmlReader& xmlReader)
{
	BOOL bRetCode = FALSE;
	CString strProcPath;
	KProcPathRule* pRule = NULL;

	pRule = (KProcPathRule*)GetRuleByName(_T("KProcPathRule"));
	if (!pRule) 
		pRule = new KProcPathRule;

	bRetCode = xmlReader.Open("KRuleConfig\\KRules\\KProcPathRule\\PathItem");
	if (!bRetCode) goto Exit0;

	do 
	{
		xmlReader.ReadText(strProcPath);

		if (strProcPath.IsEmpty())
		{
			DPrintA("CreateKProcPathRule error, dir:%ws\n", (LPCTSTR)strProcPath);
		}
		else if (m_varString.RelpaceVariable(strProcPath))
		{
			pRule->AddProcPath(strProcPath);	
		}
		else
		{
			DPrintA("CreateKFileDirRule RelpaceVariable fail, dir:%ws\n", (LPCTSTR)strProcPath);
		}

		bRetCode = xmlReader.NextSibling();
		if (!bRetCode) break;
	} while (true);

Exit0:
	return pRule;
}

/*
<KRuleConfig>
	<KRules>
		<KSignDataRule>
			<Desc>数字签名规则，判断文件的数字签名是否要放行</Desc>
			<SignItem sign="xxx" />
			<SignItem sign="xxx" />
		</KSignDataRule>
	<KRules>
<KRuleConfig>
*/

IRule* KRuleConfig::CreateKSignDataRule(KXmlReader& xmlReader)
{
	BOOL bRetCode = FALSE;
	CString strSign;
	KSignDataRule* pRule = NULL;

	pRule = (KSignDataRule*)GetRuleByName(_T("KSignDataRule"));
	if (!pRule) 
		pRule = new KSignDataRule;

	bRetCode = xmlReader.Open("KRuleConfig\\KRules\\KSignDataRule\\SignItem");
	if (!bRetCode) goto Exit0;

	do 
	{
		xmlReader.Read("sign", strSign);

		if (strSign.IsEmpty())
		{
			DPrintA("CreateKSignDataRule error\n");
		}
		else
		{
			USES_CONVERSION;
			pRule->AddSign(W2A(strSign));
			DPrintA("sign:%ws\n", strSign);
		}

		bRetCode = xmlReader.NextSibling();
		if (!bRetCode) break;
	} while (true);

Exit0:
	return pRule;
}

/*
<KRuleConfig>
	<KRules>
		<KWhiteRule>
			<Desc>文件全路径规则，判断文件路径是否在指定的文件列表中</Desc>
			<Item procdir="%v%xxx" "/>
			<Item procpath="%v%xxx" />
			<Item filedir="%v%xxx"/>
		</KWhiteRule>
	<KRules>
<KRuleConfig>
*/

IRule* KRuleConfig::CreateKWhiteRule(KXmlReader& xmlReader)
{
	BOOL bRetCode = FALSE;
	CString strDir;
	CString strPath;
	CString strFileDir;
	KWhiteRule* pRule = NULL;

	pRule = (KWhiteRule*)GetRuleByName(_T("KWhiteRule"));
	if (!pRule) 
		pRule = new KWhiteRule;

	bRetCode = xmlReader.Open("KRuleConfig\\KRules\\KWhiteRule\\Item");
	if (!bRetCode) goto Exit0;

	do 
	{
		xmlReader.Read("procdir", strDir);
		xmlReader.Read("procpath", strPath);
		xmlReader.Read("filedir", strFileDir);

		if (strDir.IsEmpty() && strPath.IsEmpty() && strFileDir.IsEmpty())
		{
			DPrintA("CreateKWhiteRule error\n");
		}
		else  if (m_varString.RelpaceVariable(strDir) && m_varString.RelpaceVariable(strPath) && m_varString.RelpaceVariable(strFileDir))
		{
			DPrintA("CreateKWhiteRule procdir:%ws, procpath:%ws, filedir:%ws\n", strDir, strPath, strFileDir);
			if (!strDir.IsEmpty())
				pRule->AddProcDir(strDir);
			else if (!strPath.IsEmpty())
			{
				pRule->AddProcPath(strPath);
			}
			else if (!strFileDir.IsEmpty())
			{
				pRule->AddFileDir(strFileDir);
			}
		}
		else
		{
			DPrintA("CreateKWhiteRule RelpaceVariable fail, path:%ws, dir:%ws\n", (LPCTSTR)strPath, (LPCTSTR)strDir);
		}

		bRetCode = xmlReader.NextSibling();
		if (!bRetCode) break;

	} while (true);

Exit0:
	return pRule;
}

/*
<KRuleConfig>
	<KRules>
		<KBlackRule>
			<Desc>黑名单规则，判断进程名与文件名是否在列表中，目录必须以\结尾</Desc>
			<Item process="c:\a.exe" file="c:\a.doc" />
			<Item process="c:\a.exe" dir="c:\b\" />
		</KBlackRule>
	</KRules>
</KRuleConfig>
*/
IRule* KRuleConfig::CreateKBlackRule(KXmlReader& xmlReader)
{
	BOOL bRetCode = FALSE;
	CString strProc;
	CString strDir;
	CString strFileName;
	KBlackRule* pRule = NULL;

	pRule = (KBlackRule*)GetRuleByName(_T("KBlackRule"));
	if (!pRule) 
		pRule = new KBlackRule;


	bRetCode = xmlReader.Open("KRuleConfig\\KRules\\KBlackRule\\Item");
	if (!bRetCode) goto Exit0;

	do 
	{
		xmlReader.Read("process", strProc);
		xmlReader.Read("file", strFileName);
		xmlReader.Read("dir", strDir);
		
		if (!strDir.IsEmpty())
		{
			KFunction::PathAddBackslash(strDir);
		}

		if (strProc.IsEmpty() || (strDir.IsEmpty() && strFileName.IsEmpty()))
		{
			DPrintA("CreateKBlackRule error, process:%ws, file:%ws, dir:%ws\n", (LPCTSTR)strProc, (LPCTSTR)strFileName, (LPCTSTR)strDir);
		}
		else if (m_varString.RelpaceVariable(strProc) && 
			m_varString.RelpaceVariable(strFileName) &&
			m_varString.RelpaceVariable(strDir)
			)
		{
			if (strFileName.IsEmpty())
				pRule->AddProcFile(strProc, strDir);	
			else
				pRule->AddProcFile(strProc, strFileName);	
		}
		else
		{
			DPrintA("CreateKBlackRule RelpaceVariable fail, process:%ws, file:%ws, dir:%ws\n", (LPCTSTR)strProc, (LPCTSTR)strFileName, (LPCTSTR)strDir);
		}

		bRetCode = xmlReader.NextSibling();
		if (!bRetCode) break;

	} while (true);

Exit0:
	return pRule;
}

/*
<KRuleConfig>
	<KRules>
		<KFileNameRule>
			<FileItem>xxx</FileItem>
			<FileItem>xxx</FileItem>
		</KFileNameRule>
	<KRules>
<KRuleConfig>
*/
IRule* KRuleConfig::CreateKFileNameRule(KXmlReader& xmlReader)
{
	BOOL bRetCode = FALSE;
	CString strFileName;
	KFileNameRule* pRule = NULL;

	pRule = (KFileNameRule*)GetRuleByName(_T("KFileNameRule"));
	if (!pRule) 
		pRule = new KFileNameRule;

	bRetCode = xmlReader.Open("KRuleConfig\\KRules\\KFileNameRule\\FileItem");
	if (!bRetCode) goto Exit0;

	do 
	{
		xmlReader.ReadText(strFileName);

		if (strFileName.IsEmpty())
		{
			DPrintA("CreateKFileDirRule error, dir:%ws\n", (LPCTSTR)strFileName);
		}
		else
		{
			pRule->AddFileName(strFileName);	
			DPrintA("CreateKFileDirRule filename:%ws\n", (LPCTSTR)strFileName);
		}

		bRetCode = xmlReader.NextSibling();
		if (!bRetCode) break;
	} while (true);

Exit0:
	return pRule;
}

/*
<KRuleConfig>
	<KRuleTree>
		<!-- type:rule, ruletree, result -->
		<SubTree>
			<Tree1 name="subtree1" >
				<RootNode type="rule" rule="KFileExtensionRule">
					<LeftNode type="result" result="not care" />
					<RightNode type="rule" rule="KProcFileRule">
						<LeftNode type="rule" rule="KFilePathRule">
						</LeftNode>
						<RightNode type="ruletree" ruletree="subtree1"/>
						</RightNode>
					</RootNode>
			</Tree1>
			<Tree2 name="subtree2" >
			</Tree2>
		</SubTree>
		<MainTree>
			<RootNode type="rule" rule="KFileExtensionRule">
				<LeftNode type="result" result="not care" />
				<RightNode type="rule" rule="KProcFileRule">
					<LeftNode type="rule" rule="KFilePathRule">
					</LeftNode>
					<RightNode type="ruletree" ruletree="subtree1"/>
					</RightNode>
			</RootNode>
		</MainTree>
	</KRuleTree>
<KRuleConfig>
*/
BOOL KRuleConfig::CreateSubRuleTrees(void)
{
	BOOL bReturn = FALSE;
	BOOL bRetCode = FALSE;
	CString strName;
	CStringA strPath = "KRuleConfig\\KRuleTree\\SubTree\\Tree";
	
	for (int i = 1; ;i++)
	{
		strPath.Format("KRuleConfig\\KRuleTree\\SubTree\\Tree%d", i);
		bRetCode = m_xmlDefReader.Open(strPath);
		if (!bRetCode) break;

		m_xmlDefReader.Read("name", strName);
		if (strName.IsEmpty()) goto Exit0;
		
		bRetCode = m_xmlDefReader.Open(strPath + "\\RootNode");
		if (!bRetCode) goto Exit0;

		KTree<IRule*>* pTree = new KTree<IRule*>;
		bRetCode = _CreateRuleTree(strPath + "\\RootNode\\", *pTree, "  ");
		if (!bRetCode) goto Exit0;

		DPrintA("load sub rule tree succeed, name:%ws\n\n", (LPCTSTR)strName);
		KRuleTree* pRuleTree = new KRuleTree;
		pRuleTree->SetName(strName);
		pRuleTree->SetTree(pTree);
		m_vecRules.push_back(pRuleTree);
	}
	
	bReturn = TRUE;

Exit0:
	return bReturn;
}

BOOL KRuleConfig::CreateMainRuleTree(void)
{
	BOOL bReturn = FALSE;
	BOOL bRetCode = FALSE;

	m_pMainRuleTree = new KRuleTree;
	KTree<IRule*>* pTree = new KTree<IRule*>;

	bRetCode = m_xmlDefReader.Open("KRuleConfig\\KRuleTree\\MainTree\\RootNode");
	if (!bRetCode) goto Exit0;

	bRetCode = _CreateRuleTree("KRuleConfig\\KRuleTree\\MainTree\\RootNode\\", *pTree, "  ");
	if (!bRetCode) goto Exit0;

	DPrintA("load main rule tree succeed\n\n");
	m_pMainRuleTree->SetName(_T("MainRuleTree"));
	m_pMainRuleTree->SetTree(pTree);

	bReturn = TRUE;

Exit0:
	return bReturn;
}

BOOL KRuleConfig::_CreateRuleTree(CStringA strPath, KTree<IRule*>& tree, CStringA strSpace)
{
	BOOL bReturn = FALSE;
	BOOL bRetCode = FALSE;

	CString strType;
	CString strRuleName;
	m_xmlDefReader.Read("type", strType);
	if (strType.IsEmpty()) goto Exit0;

	if (strType.CompareNoCase(_T("rule")) == 0)
	{
		m_xmlDefReader.Read("rule", strRuleName);
		IRule* pRule = GetRuleByName(strRuleName);
		if (!pRule) goto Exit0;
		tree.SetValue(pRule);
		DPrintA("%stype:%ws, name:%ws\n", strSpace, strType, strRuleName);
	}
	else if (strType.CompareNoCase(_T("ruletree")) == 0)
	{
		m_xmlDefReader.Read("ruletree", strRuleName);
		IRule* pRule = GetRuleByName(strRuleName);
		if (!pRule) goto Exit0;
		tree.SetValue(pRule);
		DPrintA("%stype:%ws, name:%ws\n", strSpace, strType, strRuleName);
	}
	else if (strType.CompareNoCase(_T("result")) == 0)
	{
		int nResult = 0;
		bRetCode = m_xmlDefReader.Read("result", nResult);
		if (!bRetCode) goto Exit0;

		m_xmlDefReader.Read("info", strRuleName);

		KResultRule* pRule = new KResultRule;
		pRule->SetResult(nResult, strRuleName);
		tree.SetValue((IRule*)pRule);
		AddRule((IRule*)pRule);
		
		DPrintA("%stype:%ws, value:%d, Info:%ws\n", strSpace, strType, nResult, strRuleName);
	}
	else
	{
		goto Exit0;
	}

	bRetCode = m_xmlDefReader.Open(strPath + "LeftNode");
	if (bRetCode)
	{
		tree.AddLeft((IRule*)NULL);
		tree.Left();
		bRetCode = _CreateRuleTree(strPath + _T("LeftNode\\"), tree, strSpace + "  ");
		tree.Parent();
		if (!bRetCode) goto Exit0;
	}

	bRetCode = m_xmlDefReader.Open(strPath + "RightNode");
	if (bRetCode)
	{
		tree.AddRight((IRule*)NULL);
		tree.Right();
		bRetCode = _CreateRuleTree(strPath + _T("RightNode\\"), tree, strSpace + "  ");
		tree.Parent();
		if (!bRetCode) goto Exit0;
	}	

	bReturn = TRUE;
Exit0:
	return bReturn;
}

void KRuleConfig::AddRule(IRule* pRule)
{
	for (size_t i = 0; i < m_vecRules.size(); i++)
	{
		if (pRule == m_vecRules[i])
			return;
	}
	m_vecRules.push_back(pRule);
}

IRule* KRuleConfig::GetRuleByName(LPCTSTR szName)
{
	for (size_t i = 0; i < m_vecRules.size(); i++)
	{
		if (_tcsicmp(szName, m_vecRules[i]->Name()) == 0)
		{
			return m_vecRules[i];
		}
	}

	return NULL;
}

std::vector<IRule*> KRuleConfig::GetRules(void)
{
	return m_vecRules;
}

KRuleTree* KRuleConfig::GetMainRuleTree(void)
{
	return m_pMainRuleTree;
}

KVariableString* KRuleConfig::GetVariableString(void)
{
	return &m_varString;
}

/*
<KRuleConfig>
	<Debug resultlist="0, 1, 2, 3, 4, 5" />
<KRuleConfig>
*/
BOOL KRuleConfig::LoadDebugResults(void)
{
	BOOL bRetCode = FALSE;
	
	m_vecResults.clear();

	bRetCode = m_xmlDefReader.Open("KRuleConfig\\Debug");
	if (bRetCode)
	{
		CString strResults;
		m_xmlDefReader.Read("resultlist", strResults);
		if (strResults.IsEmpty()) return FALSE;
		
		DPrintA("KRuleConfig::LoadDebugResults resultlist:%ws\n", strResults);
		CString strTemp;
		for (int i = 0; i < 50; i++)
		{
			strTemp.Format(_T("%d"), i);
			if (strResults.Find(strTemp) != -1)
				m_vecResults.push_back(i);
		}
	}

	return TRUE;
}

std::vector<int> KRuleConfig::GetDebugResults(void)
{
	return m_vecResults;
}