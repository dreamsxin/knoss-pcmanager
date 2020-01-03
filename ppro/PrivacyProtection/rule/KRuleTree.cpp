#include "stdafx.h"
#include "KRuleTree.h"

KRuleTree::KRuleTree()
{
	m_pRuleTree = NULL;
}

KRuleTree::~KRuleTree()
{
	
}

LPCTSTR KRuleTree::Name(void)
{
	return m_strName;
}

BOOL KRuleTree::Match(KAccessData* pData)
{
	BOOL bReturn = FALSE;
	BOOL bRetCode = FALSE;
	KTreeNode<IRule*>* pTreeNode = NULL;

	pTreeNode = m_pRuleTree->GetRootNode();
	do 
	{
		bReturn = pTreeNode->value->Match(pData);

#ifdef PRINT_RULE_RESULT
		CStringA strMsg;
		strMsg.Format("\trule:%ws, return:%d\n", pTreeNode->value->Name(), bReturn);
		CStringA* pDbgData = (CStringA*)pData->pExtendData;
		*pDbgData += strMsg;
#endif

		if (bReturn)
		{
			pTreeNode = pTreeNode->pRightNode;
		}
		else
			pTreeNode = pTreeNode->pLeftNode;

	} while (pTreeNode);

	return bReturn;
}

void KRuleTree::DbgPrint(void)
{
	DPrintA("KRuleTree::DbgPrint name:%ws\n", m_strName);
	
	_Print(m_pRuleTree->GetRootNode(), _T("  "));

	DPrintA("\n\n");
}

void KRuleTree::PrintAll(void)
{
	DPrintA("KRuleTree::PrintAll name:%ws\n", m_strName);

	_PrintAll(m_pRuleTree->GetRootNode());

	DPrintA("\n\n");
}

void KRuleTree::_Print(KTreeNode<IRule*>* pNode, CString strSpace)
{
	IRule* pRule = pNode->value;

	DPrintA("%ws%ws\n", strSpace, pRule->Name());
	
	if (pNode->pLeftNode)
	{
		_Print(pNode->pLeftNode, strSpace + _T("  "));
	}
	if (pNode->pRightNode)
	{
		_Print(pNode->pRightNode, strSpace + _T("  "));
	}
}

void KRuleTree::_PrintAll(KTreeNode<IRule*>* pNode)
{
	IRule* pRule = pNode->value;

	pRule->DbgPrint();

	if (pNode->pLeftNode)
	{
		_PrintAll(pNode->pLeftNode);
	}
	if (pNode->pRightNode)
	{
		_PrintAll(pNode->pRightNode);
	}
}

void KRuleTree::SetName(LPCTSTR szName)
{
	m_strName = szName;
}

void KRuleTree::SetTree(KTree<IRule*>* pTree)
{
	m_pRuleTree = pTree;
}