#ifndef __KRuleTree_h__
#define __KRuleTree_h__

#include "IRule.h"
#include "KTree.h"

class KRuleTree : public IRule
{
public:
	KRuleTree();
	~KRuleTree();

	virtual LPCTSTR Name(void);
	virtual BOOL Match(KAccessData* pData);
	virtual void DbgPrint(void);

	void PrintAll(void);
	void SetName(LPCTSTR szName);
	void SetTree(KTree<IRule*>* pTree);

protected:
	void _Print(KTreeNode<IRule*>* pNode, CString strSpace);
	void _PrintAll(KTreeNode<IRule*>* pNode);

private:
	CString m_strName;
	KTree<IRule*>* m_pRuleTree;
};

#endif