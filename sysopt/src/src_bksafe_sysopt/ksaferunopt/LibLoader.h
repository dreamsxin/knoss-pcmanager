
#pragma once

//#include <libheader.h>

#include <common/markup.h>
#include "runoptimize/ksaferunstrdefine.h"
#include <libheader/libheader.h>
#include <common/utility.h>
#include <runoptimize/interface.h>

#define VERSION_RUN_DAT		1


class CFindData
{
public:
	CFindData(DWORD nTypex, LPCTSTR lpstrNamex, LPCTSTR lpAppNamex)
	{
		nType		= nTypex;
		lpstrName	= lpstrNamex;
		lpAppName	= lpAppNamex;
	}
	DWORD	nType;
	LPCTSTR	lpstrName;
	LPCTSTR	lpAppName;
};

class CLibLoader
{
public:
	CLibLoader()
	{
		m_bLoaded = FALSE;
	}
	virtual ~CLibLoader(){}

// 	static CLibLoader* GetPtr()
// 	{
// 		static CLibLoader x;
// 		return &x;
// 	}

	static	BOOL UpdateXML( BkDatLibHeader& libHeader,CSimpleArray<DWORD> & idDelList, CSimpleArray<CString> & strAddList)
	{
		CLibLoader	loadx;
		return loadx._internUpdateXML(libHeader,idDelList,strAddList);
	}

	BOOL _internUpdateXML( BkDatLibHeader& libHeader,CSimpleArray<DWORD> & idDelList, CSimpleArray<CString> & strAddList)
	{
		BOOL		bRes = FALSE;
		CString		szLibPath;
		CString		szLibPathTmp;

		szLibPath = GetDefaultLib();
		if ( !szLibPath.IsEmpty() )
		{
			szLibPathTmp	=  szLibPath + _T(".tmp");
			CMarkup	xmlDoc;
			BkDatLibHeader	headerOld;

			if (CDataFileLoader().GetLibDatHeader(szLibPath,headerOld) && headerOld.llVersion.QuadPart == libHeader.llUpdateForVer.QuadPart)
			{
				if ( LoadXMLToMarkUp(szLibPath,xmlDoc) && xmlDoc.FindElem(KSAFE_RUN_XML_ROOT) )
				{
					xmlDoc.IntoElem();
					while ( xmlDoc.FindElem(KSAFE_RUN_XML_ITEM) )
					{
						DWORD	nId = _ttoi(xmlDoc.GetAttrib(KSAFE_RUN_XML_ITEM_ID));
						if ( idDelList.Find(nId) != -1 )
						{
							xmlDoc.RemoveElem();
							bRes = TRUE; 
						}
					}

					xmlDoc.OutOfElem();
					for ( INT i = 0; i < strAddList.GetSize(); i++)
					{
						xmlDoc.AddChildSubDoc(strAddList[i]);
						bRes = TRUE; 
					}
				}
			}

			if ( bRes )
			{
				CString	strDOC = xmlDoc.GetDoc();
				BkDatLibEncodeParam2	px(headerOld.dwType,libHeader.llVersion,strDOC,libHeader.nFormatVer);
				bRes = CDataFileLoader().Save(szLibPathTmp,px.GetEncodeParam());
			}

			if ( bRes )
				bRes = ::MoveFileEx( szLibPathTmp, szLibPath, MOVEFILE_REPLACE_EXISTING);
		}

		return bRes;
	}


	BOOL LoadXMLToMarkUp( LPCTSTR lpFile, CMarkup& xmlDoc )
	{
		BOOL			bRes = FALSE;
		CDataFileLoader	loader;
		CString			strXML;
		BkDatLibHeader	header;

		::SetThreadLocale( MAKELANGID(LANG_CHINESE,SUBLANG_CHINESE_SIMPLIFIED) ); 

		if (loader.GetLibDatHeader(lpFile,header) && header.nFormatVer>1)
			return FALSE;
		if ( loader.LoadFile(lpFile,strXML) && xmlDoc.SetDoc(strXML) )
			bRes = TRUE;
#if 1	//yykxx
		else if ( xmlDoc.Load(lpFile) )
			bRes = TRUE;
#endif

		return bRes;
	}

	BOOL LoadLib(LPCTSTR lpLibDat=NULL)
	{
		BOOL bRes = FALSE;
		_DeInit();
		if (lpLibDat!=NULL)
			m_strDatPath = lpLibDat;
		else
			m_strDatPath = GetDefaultLib();

		CMarkup	markup;
		if (LoadXMLToMarkUp(m_strDatPath, markup))
		{
			LoadItem(markup,m_dataArray);
		}

		return bRes;
	}

	void LoadItem(CMarkup& markup, CSimpleArray<KSRunInfo>& dataArray)
	{
		if (markup.FindElem(KSAFE_RUN_XML_ROOT))
		{
			markup.IntoElem();
			while (markup.FindElem(KSAFE_RUN_XML_ITEM))
			{

#define GET_DATA_STRING(markup,tag,str)		{markup.SavePos();if(markup.FindElem(tag))str=markup.GetData();markup.RestorePos();}
#define GET_DATA_INT(markup,tag,nvalue)		{markup.SavePos();if(markup.FindElem(tag)){CString str=markup.GetData();nvalue=_ttoi(str);}markup.RestorePos();}

				KSRunInfo	runinfo;
				runinfo.nItemId = _ttoi(markup.GetAttrib(KSAFE_RUN_XML_ITEM_ID));

				markup.IntoElem();
				GET_DATA_STRING(markup,KSAFE_RUN_XML_ITEM_NAME,runinfo.strName);
				GET_DATA_STRING(markup,KSAFE_RUN_XML_ITEM_DISPLAY,runinfo.strDisplay);
				GET_DATA_STRING(markup,KSAFE_RUN_XML_ITEM_DESC,runinfo.strDesc);
				GET_DATA_STRING(markup,KSAFE_RUN_XML_ITEM_WARNING,runinfo.strWarning);
				GET_DATA_STRING(markup,KSAFE_RUN_XML_ITEM_APPNAME,runinfo.strAppName);
				GET_DATA_INT(markup,KSAFE_RUN_XML_ITEM_NODELAY,runinfo.bNoDealyShow);
				GET_DATA_INT(markup,KSAFE_RUN_XML_ITEM_SYSTEM,runinfo.bSystem);
				GET_DATA_INT(markup,KSAFE_RUN_XML_ITEM_TYPE,runinfo.nType);
				GET_DATA_INT(markup,KSAFE_RUN_XML_ITEM_ADVICE,runinfo.nAdvice);
				GET_DATA_INT(markup,KSAFE_RUN_XML_ITEM_REGPATH,runinfo.bRegPath);
				GET_DATA_INT(markup,KSAFE_RUN_XML_ITEM_SHOW_REMAINS,runinfo.bShowRemain);
				markup.OutOfElem();

				dataArray.Add(runinfo);
			}
			markup.OutOfElem();
		}
	}

	int GetCount()
	{
		return m_dataArray.GetSize();
	}

	KSRunInfo& GetInfo(int iIndex)
	{
		return m_dataArray[iIndex];
	}

	CString GetFileName(LPCTSTR lpFullPath)
	{
		LPCTSTR lpstr = _tcsrchr(lpFullPath,_T('\\'));

		if (lpstr==NULL)
			return CString(lpFullPath);
		else
			return CString(lpstr+1);
	}

	BOOL FindDatInfo(CFindData& data, KSRunInfo** pDatInfo = NULL)
	{
		BOOL	bFind = FALSE;
		for ( int j=0; j<this->GetCount(); j++)
		{
			KSRunInfo&	libinfo = this->GetInfo(j);

			if (data.nType==libinfo.nType)
			{
				CString	strName;
				
				if (data.nType!=KSRUN_TYPE_TASK)
					strName = GetFileName(data.lpstrName);
				else 
				{
					strName = data.lpstrName;
				}

				CString	strAppName	= GetFileName(data.lpAppName);
			
				if(libinfo.strName.CompareNoCase(strName)==0 &&
					libinfo.strAppName.CompareNoCase(strAppName)==0 )
				{
					bFind = TRUE;
				}

				if (!bFind && libinfo.bRegPath)
				{
					CString	strApp		= data.lpAppName;
					CString	strAppDat	= libinfo.strAppName;

					strApp.MakeLower();
					strAppDat.MakeLower();

					if (strApp.Find(strAppDat)!=-1)
						bFind = TRUE;
				}

				if (bFind)
				{
					if (pDatInfo!=NULL)
						*pDatInfo = &this->GetInfo(j);

					break;
				}
			}
		}
		return bFind;
	}

protected:
	CString GetDefaultLib()
	{
		CString	strpath;
		CAppPath().Instance().GetLeidianDataPath(strpath,TRUE);
		strpath+=STR_KSAFE_RUN_OPTIMIZE_DAT;
		return strpath;
	}	
	
	BOOL _DeInit()
	{
		if ( m_bLoaded )
		{
// 			for ( int i=0; i<m_dataArray.GetSize(); i++)
// 			{
// 				delete m_dataArray[i];
// 			}
			m_dataArray.RemoveAll();
			m_bLoaded = FALSE;
		}
		return TRUE;
	}

protected:
	BOOL						m_bLoaded;
	CString						m_strDatPath;
	CSimpleArray<KSRunInfo>		m_dataArray;
};