#ifndef __KTree_h__
#define __KTree_h__

template <typename T>
struct KTreeNode
{
	KTreeNode<T>* pLeftNode;
	KTreeNode<T>* pRightNode;
	KTreeNode<T>* pParentNode;
	T value;

	KTreeNode()
	{
		pLeftNode = NULL;
		pRightNode = NULL;
		pParentNode = NULL;
	}
};

template <typename T>
class KTree
{
public:
	KTree()
	{
		m_pCurrentNode = &m_root;
	}
	~KTree()
	{
		Clear();
	}
	
	bool Left()
	{
		if (m_pCurrentNode->pLeftNode)
		{
			m_pCurrentNode = m_pCurrentNode->pLeftNode;
			return true;
		}
		return false;
	}

	bool Right()
	{
		if (m_pCurrentNode->pRightNode)
		{
			m_pCurrentNode = m_pCurrentNode->pRightNode;
			return true;
		}
		return false;
	}
	
	bool Parent()
	{
		if (m_pCurrentNode->pParentNode)
		{
			m_pCurrentNode = m_pCurrentNode->pParentNode;
			return true;
		}
		return false;
	}

	void Root()
	{
		m_pCurrentNode = &m_root;
	}

	void SetValue(const T& t)
	{
		m_pCurrentNode->value = t;
	}

	T& GetValue()
	{
		return m_pCurrentNode->value;
	}

	KTreeNode<T>* GetRootNode(void)
	{
		return &m_root;
	}

	bool AddLeft(const T& t)
	{
		if (m_pCurrentNode->pLeftNode == NULL)
		{
			KTreeNode<T>* pNode = new KTreeNode<T>();
			pNode->pParentNode = m_pCurrentNode;
			pNode->value = t;
			m_pCurrentNode->pLeftNode = pNode;
			return true;
		}
		return false;
	}

	bool AddRight(const T& t)
	{
		if (m_pCurrentNode->pRightNode == NULL)
		{
			KTreeNode<T>* pNode = new KTreeNode<T>();
			pNode->pParentNode = m_pCurrentNode;
			pNode->value = t;
			m_pCurrentNode->pRightNode = pNode;
			return true;
		}
		return false;
	}

	void Clear(void)
	{
		KTreeNode<T>* pNode;
		_DeleteTree(m_root.pLeftNode);
		m_root.pLeftNode = NULL;
		_DeleteTree(m_root.pRightNode);
		m_root.pRightNode = NULL;
	}

protected:
	void _DeleteTree(KTreeNode<T>* pNode)
	{
		if (!pNode) return;

		if (pNode->pLeftNode)
		{
			_DeleteTree(pNode->pLeftNode);
			pNode->pLeftNode = NULL;
		}
		if (pNode->pRightNode)
		{
			_DeleteTree(pNode->pRightNode);
			pNode->pRightNode = NULL;
		}

		delete pNode;
	}

private:
	KTreeNode<T> m_root;
	KTreeNode<T>* m_pCurrentNode;
};

#endif