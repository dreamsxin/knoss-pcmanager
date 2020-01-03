
#pragma once

#include <runoptimize/interface.h>
#include <common/utility.h>
#include <common/markup.h>

#define IS_SAME_BOOL(x,y)	((x&&y)||(!x&&!y))
#define RESTORE_INI_PATH	_T("ksrunlog2.log")
#define DELETE_INI_PATH	_T("ksrundel.log")

class CDelHistory
{
public:
	struct DelLogItem
	{
		DWORD		nType;
		DWORD		nType2;
		CString		strName;
	};

	CDelHistory()
	{
		m_bDirty = FALSE;


		CAppPath::Instance().GetLeidianCfgPath(m_strFileName,TRUE);
		m_strFileName.Append(_T("\\"));
		m_strFileName.Append(DELETE_INI_PATH);
		Load();
	}
	virtual ~CDelHistory()
	{
		Save();
	}

protected:
	CString	m_strFileName;
	BOOL	m_bDirty;
#define STR_XML_ROOT		_T("xml")
#define STR_XML_ITEM		_T("item")
#define STR_ATTRIB_TYPE		_T("type")
#define STR_ATTRIB_TYPE2	_T("type2")
#define STR_ATTRIB_TYPE3	_T("type3")
#define STR_ATTRIB_NAME		_T("namex")
#define STR_ATTRIB_NAME2	_T("name2")

	BOOL Load()
	{
		ATLASSERT(m_logList.GetSize()==0);
		m_bDirty = FALSE;

		CMarkup	markup;
		if (markup.Load(m_strFileName) && markup.FindElem(STR_XML_ROOT))
		{
			markup.IntoElem();
			while (markup.FindElem(STR_XML_ITEM))
			{
				DelLogItem	pItem;
				pItem.nType		= _ttoi(markup.GetAttrib(STR_ATTRIB_TYPE));
				pItem.nType2	= _ttoi(markup.GetAttrib(STR_ATTRIB_TYPE2));
				pItem.strName	= markup.GetAttrib(STR_ATTRIB_NAME);

				if (!pItem.strName.IsEmpty())
					m_logList.Add(pItem);
			}
			markup.OutOfElem();
		}
		return TRUE;
	}

	BOOL Save()
	{
		if (m_bDirty)
		{
			::DeleteFile(m_strFileName);

			if ( m_logList.GetSize()>0 )
			{
				CMarkup	markup;

				markup.InsertElem(STR_XML_ROOT);
				markup.IntoElem();

				for (int i=0; i<m_logList.GetSize(); i++)
				{
					DelLogItem*	pItem = &m_logList[i];
					markup.InsertElem(STR_XML_ITEM);
					markup.SetAttrib(STR_ATTRIB_TYPE,pItem->nType);
					markup.SetAttrib(STR_ATTRIB_TYPE2,pItem->nType2);
					markup.SetAttrib(STR_ATTRIB_NAME,pItem->strName);
				}
				markup.OutOfElem();
				markup.Save(m_strFileName);
			}
		}

		return TRUE;
	}
public:

	static CDelHistory* GetPtr()
	{	
		static CDelHistory	run;
		return &run;
	}

	BOOL FindDelRunItem(DWORD nType, LPCTSTR lpstrName)
	{
		for (int i=0; i<m_logList.GetSize(); i++)
		{
			DelLogItem&	log = m_logList[i];

			if (log.nType == KSRUN_TYPE_STARTUP)
			{
				if ( IsSameRunType(nType, log.nType2) )
				{
					if ( IsTypeRunReg(nType) )
					{
						if (log.strName.CompareNoCase(lpstrName) == 0)
							return TRUE;
					}
					else
					{
						CString strName1 = GetFileName(lpstrName);
						CString strName2 = GetFileName(log.strName);

						if (strName1.CompareNoCase(strName2)==0)
							return TRUE;
					}
				}

			}
		}
		return FALSE;
	}

	void LogDelRun(DWORD nType, LPCTSTR lpstrName)
	{
		m_bDirty = TRUE;

		DelLogItem	item;
		item.nType	= KSRUN_TYPE_STARTUP;
		item.nType2	= nType;
		item.strName= lpstrName;

		m_logList.Add(item);
	}

	void LogDelService(LPCTSTR lpservName)
	{
		return;

		DelLogItem	item;
		item.nType	= KSRUN_TYPE_SERVICE;
		item.strName= lpservName;
		m_logList.Add(item);
	}

	BOOL FindDelTaskItem(DWORD nType, LPCTSTR lpstrName)
	{
		for (int i=0; i<m_logList.GetSize(); i++)
		{
			DelLogItem&	log = m_logList[i];

			if (log.nType == KSRUN_TYPE_TASK)
			{
				if (log.nType2 == nType )
				{
					if ( log.strName.CompareNoCase(lpstrName) == 0)
						return TRUE;
				}
			}
		}
		return FALSE;
	}

	void LogDelTask(DWORD nType, LPCTSTR lpstrName)
	{
		m_bDirty = TRUE;

		DelLogItem	item;
		item.nType	= KSRUN_TYPE_TASK;
		item.nType2	= nType;
		item.strName= lpstrName;
		m_logList.Add(item);
	}

protected:
	CSimpleArray<DelLogItem>	m_logList;
};

class RunLogItem
{
public:
	RunLogItem()
	{
		nExtType=0;
		bSystem =FALSE;
	}
	virtual ~RunLogItem()
	{}

	DWORD	nType;			// 类型
	DWORD	nInitState;		// 初始化状态
	CString	strName;		// 标记名字
	CString	strName2;		// 辅助标记名字
	DWORD	nExtType;
	BOOL	bSystem;		// 是否是系统分类
};


class IEnumLogCallBack
{
public:
	virtual BOOL EnumLog(RunLogItem* pItem, PVOID lParam) = 0;
};

class CRestoreRunner
{
public:
	CRestoreRunner()
	{
		m_bDirty = FALSE;

		CAppPath::Instance().GetLeidianCfgPath(m_strFileName,TRUE);

		m_strFileName.Append(_T("\\"));
		m_strFileName.Append(RESTORE_INI_PATH);
		Load();
	}
	virtual ~CRestoreRunner()
	{
		Save();
		_free();
	}

	static CRestoreRunner* GetPtr()
	{	
		static CRestoreRunner	run;
		return &run;
	}	

protected:

#define STR_XML_ROOT		_T("xml")
#define STR_XML_ITEM		_T("item")
#define STR_ATTRIB_TYPE		_T("type")
#define STR_ATTRIB_TYPE2	_T("type2")
#define STR_ATTRIB_TYPE3	_T("type3")
#define STR_ATTRIB_TYPE4	_T("type4")
#define STR_ATTRIB_NAME		_T("namex")
#define STR_ATTRIB_NAME2	_T("name2")

	BOOL Load()
	{
		ATLASSERT(m_logList.GetSize()==0);
		m_bDirty = FALSE;

		CMarkup	markup;
		if (markup.Load(m_strFileName) && markup.FindElem(STR_XML_ROOT))
		{
			markup.IntoElem();
			while (markup.FindElem(STR_XML_ITEM))
			{
				RunLogItem*	pItem = new RunLogItem;
				pItem->nType		= _ttoi(markup.GetAttrib(STR_ATTRIB_TYPE));
				pItem->nInitState	= _ttoi(markup.GetAttrib(STR_ATTRIB_TYPE2));
				pItem->nExtType		= _ttoi(markup.GetAttrib(STR_ATTRIB_TYPE3));
				pItem->strName		= markup.GetAttrib(STR_ATTRIB_NAME);
				pItem->strName2		= markup.GetAttrib(STR_ATTRIB_NAME2);
				pItem->bSystem		= _ttoi(markup.GetAttrib(STR_ATTRIB_TYPE4));

				if (!pItem->strName.IsEmpty())
					m_logList.Add(pItem);
				else
					delete pItem;
			}
			markup.OutOfElem();
		}
		return TRUE;
	}

	BOOL Save()
	{
		if (m_bDirty)
		{
			::DeleteFile(m_strFileName);

			if ( m_logList.GetSize()>0 )
			{
				CMarkup	markup;

				markup.InsertElem(STR_XML_ROOT);
				markup.IntoElem();

				for (int i=0; i<m_logList.GetSize(); i++)
				{
					RunLogItem*	pItem = m_logList[i];
					markup.InsertElem(STR_XML_ITEM);
					markup.SetAttrib(STR_ATTRIB_TYPE,pItem->nType);
					markup.SetAttrib(STR_ATTRIB_TYPE2,pItem->nInitState);
					markup.SetAttrib(STR_ATTRIB_TYPE3,pItem->nExtType);
					markup.SetAttrib(STR_ATTRIB_TYPE4,pItem->bSystem);
					markup.SetAttrib(STR_ATTRIB_NAME,pItem->strName);
					markup.SetAttrib(STR_ATTRIB_NAME2,pItem->strName2);
				}
				markup.OutOfElem();
				markup.Save(m_strFileName);
			}
		}

		return TRUE;
	}

public:
	void OperateService(LPCTSTR lpServName, DWORD nInitState, DWORD nNewState, BOOL bSystem)
	{
		m_bDirty = TRUE;

		BOOL	bFindLog = FALSE;
		for (int i=0; i<m_logList.GetSize(); i++)
		{
			RunLogItem*	plogItem = m_logList[i];

			if (plogItem->nType==KSRUN_TYPE_SERVICE && plogItem->strName.CompareNoCase(lpServName)==0 )
			{
				if (IS_SAME_BOOL(plogItem->nInitState,nNewState))
				{
					delete plogItem;
					m_logList.RemoveAt(i);
					bFindLog = TRUE;
					break;
				}
			}
		}
		if (!bFindLog)
		{
			RunLogItem*	plogItem = new RunLogItem;
			plogItem->strName	 = lpServName;
			plogItem->nType		 = KSRUN_TYPE_SERVICE;
			plogItem->nInitState = nInitState;
			plogItem->bSystem	 = bSystem;
			m_logList.Add(plogItem);
		}
	}

	void OperateRun(LPCTSTR lpFileName, DWORD nInitType, DWORD nNewType, BOOL bSystem)
	{
		m_bDirty = TRUE;

		BOOL	bFindLog = FALSE;
		for (int i=0; i<m_logList.GetSize(); i++)
		{
			RunLogItem*	plogItem = m_logList[i];

			if (plogItem->nType==KSRUN_TYPE_STARTUP && plogItem->strName.CompareNoCase(lpFileName)==0 )
			{
				if ( plogItem->nInitState == nNewType )
				{
					delete plogItem;
					m_logList.RemoveAt(i);
					bFindLog = TRUE;
					break;
				}
			}
		}
		if (!bFindLog)
		{
			RunLogItem*	plogItem = new RunLogItem;
			plogItem->strName	 = lpFileName;
			plogItem->nType		 = KSRUN_TYPE_STARTUP;
			plogItem->nInitState = nInitType;
			plogItem->nExtType	 = nNewType;
			plogItem->bSystem	 = bSystem;
			m_logList.Add(plogItem);
		}
	}

	void OperateTaskJob(LPCTSTR lpJobPath,DWORD nJobType, BOOL bInitEnable, BOOL bNewEnable, BOOL bSystem)
	{
		m_bDirty = TRUE;

		BOOL	bFindLog = FALSE;
		for (int i=0; i<m_logList.GetSize(); i++)
		{
			RunLogItem*	plogItem = m_logList[i];

			if (plogItem->nType==KSRUN_TYPE_TASK && plogItem->strName.CompareNoCase(lpJobPath)==0 && plogItem->nExtType==nJobType)
			{
				if ( IS_SAME_BOOL(plogItem->nInitState,bNewEnable) )
				{
					delete plogItem;
					m_logList.RemoveAt(i);
					bFindLog = TRUE;
					break;
				}
			}
		}
		if (!bFindLog)
		{
			RunLogItem*	plogItem = new RunLogItem;
			plogItem->strName	 = lpJobPath;
			plogItem->nType		 = KSRUN_TYPE_TASK;
			plogItem->nInitState = bInitEnable;
			plogItem->nExtType	 = nJobType;
			plogItem->bSystem	 = bSystem;
			m_logList.Add(plogItem);
		}
	}

	void EnumItem(IEnumLogCallBack* pCallback, PVOID lParam)
	{
		for (int i=0; i<m_logList.GetSize(); i++)
		{
			RunLogItem*	plogItem = m_logList[i];
			if ( !pCallback->EnumLog(plogItem,lParam) )
			{
				m_bDirty = TRUE;
				m_logList.RemoveAt(i);
				delete plogItem;
				i--;
			}
		}
	}

	int GetCount()
	{
		return m_logList.GetSize();
	}
	RunLogItem* GetItem(int i)
	{
		return m_logList[i];
	}

protected:
	void _free()
	{
		for (int i=0; i<m_logList.GetSize(); i++)
		{
			delete m_logList[i];
		}
		m_logList.RemoveAll();
	}

protected:
	CSimpleArray<RunLogItem*>	m_logList;
	BOOL						m_bDirty;
	CString						m_strFileName;
};