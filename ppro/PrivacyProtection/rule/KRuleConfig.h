#ifndef __KRuleConfig_h__
#define __KRuleConfig_h__

#include "IRule.h"
#include "KVariableString.h"
#include "KXmlReader.h"
#include "KTree.h"
#include "KRuleTree.h"
#include <sstream>

/*
ClassName	: KRuleConfig
Creator		: zhongzhendong
Brief		: 规则创建工厂，从配置文件创建规则列表和逻辑树
Remark		: 可能有部分内存释放问题
*/

class KRuleConfig
{
public:
	KRuleConfig();
	~KRuleConfig();

	BOOL Init(LPCTSTR szDefConfigFile, LPCTSTR szUserConfigFile);
	BOOL Init(const std::stringstream& strRuleStream, const std::stringstream& strCustomRuleStream);
	void Uninit(void);

	std::vector<IRule*> GetRules(void);
	std::vector<int> GetDebugResults(void);
	KRuleTree* GetMainRuleTree(void);
	KVariableString* GetVariableString(void);

protected:
	CString ReadRegString(LPCTSTR szRootKey, LPCTSTR szSubKey, LPCTSTR szValueName);
	HKEY GetRootKey(LPCTSTR szRootKey);

	BOOL LoadVariableString(void);
	BOOL CreateRules(void);
	BOOL CreateRuleTree(void);
	BOOL LoadDebugResults(void);

	BOOL LoadProcNameList(void);
	BOOL LoadRegVariableString(void);
	BOOL LoadSystemVariableString(void);

	IRule* CreateKDirRelateRule(KXmlReader& xmlReader);
	IRule* CreateKFileDirRule(KXmlReader& xmlReader);
	IRule* CreateKFileExtensionRule(KXmlReader& xmlReader);
	IRule* CreateKFileExtRelateRule(KXmlReader& xmlReader);
	IRule* CreateKFileNameRelateRule(KXmlReader& xmlReader);
	IRule* CreateKFilePathRule(KXmlReader& xmlReader);
	IRule* CreateKProcFileRule(KXmlReader& xmlReader);
	IRule* CreateKProcPathRule(KXmlReader& xmlReader);
	IRule* CreateKSignDataRule(KXmlReader& xmlReader);
	IRule* CreateKWhiteRule(KXmlReader& xmlReader);
	IRule* CreateKBlackRule(KXmlReader& xmlReader);
	IRule* CreateKFileNameRule(KXmlReader& xmlReader);

	void AddRule(IRule* pRule);
	IRule* GetRuleByName(LPCTSTR szName);
	
	BOOL CreateSubRuleTrees(void);
	BOOL CreateMainRuleTree(void);
	BOOL _CreateRuleTree(CStringA strPath, KTree<IRule*>& tree, CStringA strSpace);

private:
	KXmlReader m_xmlDefReader;
	KXmlReader m_xmlUserReader;
	KVariableString m_varString;
	std::vector<IRule*> m_vecRules;
	std::vector<int> m_vecResults;
	KRuleTree* m_pMainRuleTree;
};

#endif