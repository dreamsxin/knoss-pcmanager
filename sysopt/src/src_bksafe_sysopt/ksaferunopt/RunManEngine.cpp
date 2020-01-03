#include "StdAfx.h"
#include "RunManEngine.h"
#include "runhelper.h"
#include "servicehelper.h"
#include "taskjobhelper.h"
#include <communits/CmdLineParser.h>
#include <common/utility.h>
#include "disablerunhelper.h"
#include "restorerunitem.h"
#include <common/RegKeyConvertString.h>
#include <inioperator/cinifile.h>
using namespace IniFileOperate;

#pragma comment(lib,"Version.lib")

#define IGNORED_FILEPATH			L"\\ignoredlist.ini"
#define SEC_IGNOREDLIST_MAIN		L"Main"
#define KEY_IGNOREDLIST_COMMENT		L"Comment"

void _GetFileInfo(BYTE *pbyInfo, LPCWSTR lpszFileInfoKey, LPWSTR lpszFileInfoRet)
{
	BOOL bRet = FALSE;
	UINT uCount = 0;
	UINT uRetSize = MAX_PATH - 1;
	LPWSTR lpszValue = NULL;

	struct LANGANDCODEPAGE
	{
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate;

	bRet = ::VerQueryValue(pbyInfo, L"\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &uCount);
	if (!bRet || uCount == 0)
		return;

	CString strSubBlock;

	strSubBlock.Format(
		L"\\StringFileInfo\\%04x%04x\\%s",
		lpTranslate[0].wLanguage,
		lpTranslate[0].wCodePage, 
		lpszFileInfoKey
		);
	bRet = ::VerQueryValue(
		pbyInfo, (LPWSTR)(LPCWSTR)strSubBlock, 
		(LPVOID *)&lpszValue, &uRetSize
		);
	if (!bRet)
		return;

	wcsncpy(lpszFileInfoRet, lpszValue, wcslen(lpszValue) + 1);
}

BOOL _GetPEFileInfo(LPCWSTR lpszFileName,LPCWSTR lpSection, CString &strProductVersion)
{
	BOOL bResult = FALSE;
	BOOL bRet = FALSE;
	DWORD dwHandle          = 0;
	DWORD dwFileInfoSize    = 0;
	VS_FIXEDFILEINFO *pFixFileInfo = NULL;
	BYTE *pbyInfo           = NULL;

	dwFileInfoSize = ::GetFileVersionInfoSize(lpszFileName, &dwHandle);
	if (0 == dwFileInfoSize)
		goto Exit0;

	pbyInfo = new BYTE[dwFileInfoSize];
	if (!pbyInfo)
		goto Exit0;

	bRet = ::GetFileVersionInfo(lpszFileName, dwHandle, dwFileInfoSize, pbyInfo);
	if (!bRet)
		goto Exit0;

	_GetFileInfo(pbyInfo, lpSection, strProductVersion.GetBuffer(MAX_PATH + 1));
	strProductVersion.ReleaseBuffer();

	bResult = TRUE;

Exit0:

	if (pbyInfo)
	{
		delete[] pbyInfo;
		pbyInfo = NULL;
	}

	return bResult;
}

void LoadIgnoredID(CSimpleArray<int>& arrayIgnoredID)
{
	arrayIgnoredID.RemoveAll();
	CString strIgnoredIniPath;
	CAppPath::Instance().GetLeidianAppPath(strIgnoredIniPath);
	strIgnoredIniPath.Append(IGNORED_FILEPATH);
	CIniFile ini_IgnoredList(strIgnoredIniPath);
	CString strGetValue;
	ini_IgnoredList.GetStrValue(SEC_IGNOREDLIST_MAIN,KEY_IGNOREDLIST_COMMENT,strGetValue.GetBuffer(MAX_PATH),MAX_PATH);
	strGetValue.ReleaseBuffer(MAX_PATH);

	WCHAR *szValue;
	WCHAR szTemp[10];
	szValue = strGetValue.GetBuffer();
	strGetValue.ReleaseBuffer();
	WCHAR* p = wcstok(szValue,L"|");
	while(p)
	{
		wcscpy_s(szTemp,p);
		arrayIgnoredID.Add(_wtoi(szTemp));
		p = wcstok(NULL,L"|");
	}
}

BOOL HasIgnored(int nID,CSimpleArray<int>& arrayIgnoredID)
{
	BOOL bFind = FALSE;
	for (int i = 0; i < arrayIgnoredID.GetSize();i++)
	{	
		if (nID == arrayIgnoredID[i])
		{
			bFind = TRUE;
			goto Exit0;
		}
	}
Exit0:
	return bFind;
}

void ConvertLib(CKsafeRunInfo& cinfo, KSRunInfo& libinfo)
{
	cinfo.nAdvice			= libinfo.nAdvice;
	cinfo.bSystem			= libinfo.bSystem;
	cinfo.bNoDealyShow		= libinfo.bNoDealyShow;
	cinfo.bUnknown			= FALSE;
	cinfo.nID				= libinfo.nItemId;
	cinfo.bShowRemain		= libinfo.bShowRemain;

	if (!libinfo.strDesc.IsEmpty())
		cinfo.strDesc		= libinfo.strDesc;
	if (!libinfo.strDisplay.IsEmpty())
		cinfo.strDisplay	= libinfo.strDisplay;
	if (!libinfo.strWarning.IsEmpty())
		cinfo.strWarning	= libinfo.strWarning;
	if (cinfo.IsAdviseStop())
	{
		CSimpleArray<int> arrayIgnoredId;
		LoadIgnoredID(arrayIgnoredId);
		cinfo.bHasIgnored = HasIgnored(cinfo.nID,arrayIgnoredId);
	}
}

void PostGetItemDesc(CKsafeRunInfo& cinfo)
{
	if (!cinfo.strDesc.IsEmpty())
		return;

	if (cinfo.strExePath.IsEmpty())
		return;

	CString	strDesc;
	CString	strProduct;

	_GetPEFileInfo(cinfo.strExePath, L"ProductName", strProduct);
	_GetPEFileInfo(cinfo.strExePath, L"FileDescription", strDesc);

	if (!strProduct.IsEmpty())
	{
		cinfo.strDesc = strProduct;
	}
	if ( strProduct.CompareNoCase(strDesc) == 0 || strDesc.IsEmpty() )
	{
		return;
	}
	if (!cinfo.strDesc.IsEmpty())
		cinfo.strDesc.Append(_T(" - "));
	cinfo.strDesc.Append(strDesc);
}

CRunManEngine::CRunManEngine(void)
{
	m_pLibLoader = NULL;
	m_hOpThread  = NULL;	
}

CRunManEngine::~CRunManEngine(void)
{
	if (m_hOpThread!=NULL)
	{
		::TerminateThread(m_hOpThread,0);
		m_hOpThread = NULL;
	}

	if (m_hOpThread_Exam!=NULL)
	{
		::TerminateThread(m_hOpThread_Exam,0);
		m_hOpThread_Exam = NULL;
	}

	if ( m_pLibLoader != NULL )
	{
		delete m_pLibLoader;
		m_pLibLoader = NULL;
	}
}

void CRunManEngine::EnumRegRunner(BOOL bEnable, CRunRegEnumer* pEnumer, IEnumRunCallBack* pCallback,CRunRegEnumer* pFilter)
{
	if (pEnumer->DoEnum())
	{
		for ( int i = 0; i < pEnumer->GetCount(); i++)
		{
			CKsafeRunInfo	cinfo;
			CRunRegEnumer::REG_RUN_INFO&	runreginfo = pEnumer->GetItem(i);

			if (pFilter != NULL)
			{
				BOOL	bExist = FALSE;
				for ( int k = 0; k< pFilter->GetCount(); k++)
				{
					CRunRegEnumer::REG_RUN_INFO&	cmpInfo = pFilter->GetItem(k);
					if ( IsSameRunType(cmpInfo.nRegType, runreginfo.nRegType) )
					{
						if (runreginfo.strName.CompareNoCase(cmpInfo.strName)==0)
						{
							bExist = TRUE;
							break;
						}
					}
				}
				if (bExist)
					continue;
			}

			CRunoptCmdLineParser cmdLine(runreginfo.strPath);
			CString strAppName = cmdLine.GetCmd();

			cinfo.nType		= KSRUN_TYPE_STARTUP;
			cinfo.strName	= runreginfo.strName;
			cinfo.strDisplay= runreginfo.strName;
			cinfo.bEnable	= bEnable;
			cinfo.strPath	= strAppName;
			cinfo.strParam	= runreginfo.strPath;
			cinfo.strExePath= strAppName;
			cinfo.nRegType	= runreginfo.nRegType;
			cinfo.bDisabledBy3xx = runreginfo.bDisabledBy3XX;
// 			if (!m_wow64Switcher.IsWin64_2())
// 				cinfo.bFileIsExist = cmdLine.IsPEFileExist();
			if ((bEnable == FALSE) && (cinfo.bFileIsExist == FALSE))//如果是已禁用的启动项，而且目标文件不存在，那就不管他
				continue;

			KSRunInfo*	pKSInfo = NULL;
			if (m_pLibLoader->FindDatInfo(CFindData(KSRUN_TYPE_STARTUP,runreginfo.strName,strAppName),&pKSInfo))
				ConvertLib(cinfo,*pKSInfo);
			PostGetItemDesc(cinfo);
			if (pCallback)
			{
// 				if ((cinfo.bFileIsExist == FALSE) && (cinfo.bShowRemain == FALSE))
// 					continue;
// 				else
					pCallback->FindRunItem(&cinfo);
			}
		}
	}
}

void CRunManEngine::EnumDirRunner( BOOL bEnable, CRunDirEnumer* pEnumer, IEnumRunCallBack* pCallback,CRunDirEnumer* pFilter )
{
	if (pEnumer->DoEnum())
	{
		for ( int i = 0; i < pEnumer->GetCount(); i++)
		{
			CKsafeRunInfo	cinfo;
			CRunDirEnumer::RUN_DIR_INFO&	rundirinfo = pEnumer->GetItem(i);

			if (pFilter != NULL)
			{
				BOOL	bExist = FALSE;
				for ( int k = 0; k< pFilter->GetCount(); k++)
				{
					CRunDirEnumer::RUN_DIR_INFO&	cmpInfo = pFilter->GetItem(k);
					if ( IsSameRunType(cmpInfo.nDirType, rundirinfo.nDirType) )
					{
						if (GetFileName(rundirinfo.strName).CompareNoCase(GetFileName(cmpInfo.strName))==0)
						{
							bExist = TRUE;
							break;
						}
					}
				}
				if (bExist)
					continue;
			}
			
			CString strParam;
			strParam.Format(_T("%s %s"),rundirinfo.strPath,rundirinfo.strParam);

			cinfo.nType			= KSRUN_TYPE_STARTUP;
			cinfo.strName		= GetFileName(rundirinfo.strName);
			cinfo.strPath		= rundirinfo.strName;	// 填写全路径
			cinfo.bEnable		= bEnable;
			cinfo.strExePath	= rundirinfo.strPath;	// LNK指向的EXE路径
			cinfo.strParam		= strParam;	
			cinfo.strDisplay	= cinfo.strName;
			cinfo.nRegType		= rundirinfo.nDirType;
// 			if (!m_wow64Switcher.IsWin64_2())
// 				cinfo.bFileIsExist	= ::PathFileExists(rundirinfo.strPath);

			KSRunInfo*	pKSInfo = NULL;
			if (m_pLibLoader->FindDatInfo(CFindData(KSRUN_TYPE_STARTUP,rundirinfo.strName,rundirinfo.strPath),&pKSInfo))
				ConvertLib(cinfo,*pKSInfo);
			PostGetItemDesc(cinfo);
			if (pCallback)
			{
// 				if ((cinfo.bFileIsExist == FALSE) && (cinfo.bShowRemain == FALSE))
// 					continue;
// 				else
					pCallback->FindRunItem(&cinfo);
			}
		}
	}
}

void CRunManEngine::_InternEnumRunInfo( DWORD dwFlags,IEnumRunCallBack* pCallback )
{
	if (m_pLibLoader==NULL)
	{
		m_pLibLoader = new CLibLoader;
		m_pLibLoader->LoadLib(NULL);
	}

	if (TRUE)
	{
		CRunDirEnumer	dirEnumer;
		EnumDirRunner(TRUE,&dirEnumer,pCallback);

		CKSRunDirEnumer	ksdirEnumer;
		ksdirEnumer.SetCopyFilter(this);
		EnumDirRunner(FALSE,&ksdirEnumer,pCallback, &dirEnumer);
	}

	if (TRUE)
	{
		CRunRegEnumer	regEnumer;
		EnumRegRunner(TRUE,&regEnumer,pCallback);

		CKSRunRegEnumer	ksregEnumer;
		ksregEnumer.SetCopyFilter(this);
		EnumRegRunner(FALSE,&ksregEnumer,pCallback, &regEnumer);
	}

	if (TRUE)
	{
		CServiceEnumer	servEnumer;
		if (servEnumer.DoEnum())
		{
			for ( int i = 0; i < servEnumer.GetCount(); i++)
			{
				CKsafeRunInfo	cinfo;
				CServiceEnumer::SERVICE_CONFIG&	servInfo = servEnumer[i];
				CString strCmdLine = servInfo.strServDLL.IsEmpty()?servInfo.lpqscfg->lpBinaryPathName:servInfo.strServDLL;
				CRunoptCmdLineParser cmdLine(strCmdLine);
				CString strAppPath = cmdLine.GetCmd();

				cinfo.nType		= KSRUN_TYPE_SERVICE;
				cinfo.strName	= servInfo.strName;
				cinfo.bEnable	= IsTypeStartEnable(servInfo.lpqscfg->dwStartType);
				cinfo.strDesc	= servInfo.strDesc;
				cinfo.strParam	= strCmdLine;
				cinfo.strDisplay= servInfo.lpqscfg->lpDisplayName;
				cinfo.strExePath= strAppPath;
// 				if (!m_wow64Switcher.IsWin64_2())
// 					cinfo.bFileIsExist = cmdLine.IsPEFileExist();

				KSRunInfo*	pKSInfo = NULL;
				if (m_pLibLoader->FindDatInfo(CFindData(KSRUN_TYPE_SERVICE,servInfo.strName,cinfo.strExePath),&pKSInfo))
					ConvertLib(cinfo,*pKSInfo);

				PostGetItemDesc(cinfo);
				if (pCallback)
				{
// 					if ((cinfo.bFileIsExist == FALSE) && (cinfo.bShowRemain == FALSE))
// 						continue;
// 					else
						pCallback->FindRunItem(&cinfo);
				}
			}
		}
	}

	if (TRUE)
	{
		CTaskSchedularEnumer	taskEnumer;
		taskEnumer.SetCopyFilter(this);
		if (taskEnumer.DoEnum())
		{
			for ( int i = 0; i < taskEnumer.GetCount(); i++)
			{
				CKsafeRunInfo	cinfo;
				CTaskSchedularEnumer::TASK_JOB_INFO&	jobInfo = taskEnumer.GetItem(i);
				CRunoptCmdLineParser cmdLine(jobInfo.strExePath);
				CString strAppPath = cmdLine.GetCmd();

				cinfo.nType		= KSRUN_TYPE_TASK;
				cinfo.strName	= jobInfo.strJobPath;
				cinfo.strPath	= strAppPath;
				cinfo.strDisplay= jobInfo.strName;
				cinfo.strParam	= jobInfo.strParam;
				cinfo.bEnable	= jobInfo.bEnable;
				cinfo.strExePath= strAppPath;
				cinfo.nJobType	= jobInfo.nType;
				cinfo.strDesc	= jobInfo.strDesc;
// 				if (!m_wow64Switcher.IsWin64_2())
// 					cinfo.bFileIsExist = cmdLine.IsPEFileExist();

				KSRunInfo*	pKSInfo = NULL;
				if (m_pLibLoader->FindDatInfo(CFindData(KSRUN_TYPE_TASK,jobInfo.strJobPath,strAppPath),&pKSInfo))
					ConvertLib(cinfo,*pKSInfo);

				PostGetItemDesc(cinfo);
				if (pCallback)
				{
// 					if ((cinfo.bFileIsExist == FALSE) && (cinfo.bShowRemain == FALSE))
// 						continue;
// 					else
						pCallback->FindRunItem(&cinfo);
				}
			}
		}
	}

	if (pCallback)
		pCallback->EndScan();

	m_hOpThread = NULL;
	return;
}

struct THREAD_PARAM
{
	DWORD				dwFlags;
	IEnumRunCallBack*	Callback;
	CRunManEngine*		pThis;
};

void CRunManEngine::EnumRunInfo( DWORD dwFlags,IEnumRunCallBack* pCallback )
{
	THREAD_PARAM* pt = new THREAD_PARAM;

	pt->Callback	= pCallback;
	pt->dwFlags		= dwFlags;
	pt->pThis		= this;
	m_hOpThread = (HANDLE)_beginthread(threadstub,0,(PVOID)pt);
	return;
}

void CRunManEngine::EnumRunInfoForExam(DWORD dwFlags,IEnumRunCallBack* pCallback )
{
	THREAD_PARAM* pt = new THREAD_PARAM;

	pt->Callback	= pCallback;
	pt->dwFlags		= dwFlags;
	pt->pThis		= this;
	m_hOpThread_Exam = (HANDLE)_beginthread(threadstub_Exam,0,(PVOID)pt);
	return;
}

void CRunManEngine::threadstub( PVOID lParam )
{
	THREAD_PARAM* pt = (THREAD_PARAM*)lParam;
	pt->pThis->_InternEnumRunInfo(pt->dwFlags,pt->Callback);
	delete pt;
	return;
}

void CRunManEngine::threadstub_Exam( PVOID lParam )
{
	THREAD_PARAM* pt = (THREAD_PARAM*)lParam;
	pt->pThis->_InternEnumRunInfo_Exam(pt->dwFlags,pt->Callback);
	delete pt;
	return;
}

struct _ENUM_RESTORE_PARAM
{
	DWORD				nType;
	IEnumRunCallBack*	pCB;
};

BOOL CRunManEngine::EnumLog( RunLogItem* pItem, PVOID lParam )
{
	_ENUM_RESTORE_PARAM*	ptrParam = (_ENUM_RESTORE_PARAM*)lParam;
	BOOL  bNoDel = TRUE;
	DWORD nType  = ptrParam->nType;
	if (nType==KSRUN_TYPE_ALL || pItem->nType==nType)
	{
		if (pItem->nType==KSRUN_TYPE_SERVICE && !pItem->strName.IsEmpty() )
		{
			if ( CServiceOper().ControlRun(pItem->nInitState,pItem->strName) )
				bNoDel = FALSE;
		}
		if (pItem->nType==KSRUN_TYPE_TASK && !pItem->strName.IsEmpty() )
		{
			if ( CTaskSchedularOper().ControlRun(pItem->nInitState,pItem->strName,pItem->nExtType) )
				bNoDel = FALSE;
		}
		if (pItem->nType==KSRUN_TYPE_STARTUP && !pItem->strName.IsEmpty() )
		{
			DWORD	nControlType = pItem->nInitState;
			BOOL	bRes = FALSE;
			CString	strName = pItem->strName;

			if (nControlType<DIR_RUN_TYPE_BEGIN)
				bRes = CRegRunOper().ControlRun(IsTypeRunEnable(nControlType),pItem->nExtType,strName,this);
			else
				bRes = CDirRunOper().ControlRun2(IsTypeRunEnable(nControlType),pItem->nExtType,strName,this);

			if ( bRes )
				bNoDel = FALSE;

			if (bRes && ptrParam->pCB )
			{
				ptrParam->pCB->PostRestoreRunItem(pItem->nInitState,IsTypeRunEnable(nControlType),strName);
			}
		}
	}
	return bNoDel;
}
void CRunManEngine::RestoreRun( DWORD nType,IEnumRunCallBack* pCallback )
{
	if (TRUE)
	{
		_ENUM_RESTORE_PARAM	xparam;

		if (nType == 4)
		{
			xparam.pCB	 = pCallback;
			xparam.nType = 1;
			CRestoreRunner::GetPtr()->EnumItem(this,&xparam);
			xparam.nType = 2;
			CRestoreRunner::GetPtr()->EnumItem(this,&xparam);
			xparam.nType = 3;
			CRestoreRunner::GetPtr()->EnumItem(this,&xparam);
		}
		else
		{
			xparam.nType = nType;
			xparam.pCB	 = pCallback;
			CRestoreRunner::GetPtr()->EnumItem(this,&xparam);
		}
	}	

	if (pCallback)
	{
		pCallback->RestoreEnd();
	}
}

BOOL CRunManEngine::ControlRun( BOOL bEnable, CKsafeRunInfo* pInfo )
{
	BOOL	bRes = FALSE;
	if (pInfo->nType==KSRUN_TYPE_SERVICE)
	{
		bRes = CServiceOper().ControlRun(bEnable, pInfo->strName);
		if (bRes)
		{
			CRestoreRunner::GetPtr()->OperateService(pInfo->strName,pInfo->bEnable,bEnable, pInfo->bSystem);
			pInfo->bEnable = !pInfo->bEnable;
		}
	}
	else if ( pInfo->nType==KSRUN_TYPE_STARTUP )
	{
		DWORD nOldType = pInfo->nRegType;
		if (pInfo->nRegType < DIR_RUN_TYPE_BEGIN )
			bRes = CRegRunOper().ControlRun(bEnable, pInfo->nRegType,pInfo->strName,bEnable?this:NULL);
		else
			bRes = CDirRunOper().ControlRun(bEnable, pInfo->nRegType,pInfo->strPath,bEnable?this:NULL);

		if (bRes)
		{
			CRestoreRunner::GetPtr()->OperateRun(pInfo->strName,nOldType,pInfo->nRegType, pInfo->bSystem);
			pInfo->bEnable = !pInfo->bEnable;
		}
	}
	else if ( pInfo->nType==KSRUN_TYPE_TASK )
	{
		bRes = CTaskSchedularOper().ControlRun(bEnable, pInfo->strName, pInfo->nJobType);
		if (bRes)
		{
			CRestoreRunner::GetPtr()->OperateTaskJob(pInfo->strName,pInfo->nJobType, pInfo->bEnable,bEnable, pInfo->bSystem);
			pInfo->bEnable = !pInfo->bEnable;
		}
	}

	return bRes;
}

void CRunManEngine::RestoreRegRunner( CRunRegEnumer* pEnumer )
{
	if (pEnumer->DoEnum())
	{
		for ( int i=0; i<pEnumer->GetCount(); i++)
		{
			CRunRegEnumer::REG_RUN_INFO item = pEnumer->GetItem(i);
			DWORD	nRegType = item.nRegType;
			CRegRunOper().ControlRun(TRUE,nRegType,item.strName,this);
		}
	}
}

void CRunManEngine::RestoreDirRuner( CRunDirEnumer* pEnumer )
{
	if (pEnumer->DoEnum())
	{
		for ( int i=0; i<pEnumer->GetCount(); i++)
		{
			CRunDirEnumer::RUN_DIR_INFO item = pEnumer->GetItem(i);
			DWORD	nRegType = item.nDirType;
			CDirRunOper().ControlRun(TRUE,nRegType,item.strName,this);
		}
	}
}

BOOL CRunManEngine::WriteRunKey( HKEY hRoot, LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpData )
{
	return TRUE;
}

BOOL CRunManEngine::WriteRunFile( LPCTSTR lpFile )
{
	return TRUE;
}

BOOL CRunManEngine::DeleteRun( CKsafeRunInfo* pInfo )
{
	BOOL bRes = FALSE;
	if (pInfo->nType == KSRUN_TYPE_TASK )
	{
		bRes = CTaskSchedularOper().DeleteTask(pInfo->strName, pInfo->nJobType);
		if (bRes)
			CDelHistory::GetPtr()->LogDelTask(pInfo->nJobType,pInfo->strName);
	}
	if (pInfo->nType == KSRUN_TYPE_SERVICE )
	{
		bRes = CServiceOper().DeleteService(pInfo->strName);

		if (bRes)
			CDelHistory::GetPtr()->LogDelService(pInfo->strName);
	}
	if (pInfo->nType == KSRUN_TYPE_STARTUP )
	{
		if (pInfo->nRegType < DIR_RUN_TYPE_BEGIN )
			bRes = CRegRunOper().DeleteRun(pInfo->nRegType, pInfo->strName);
		else
			bRes = CDirRunOper().DeleteRun(pInfo->nRegType, pInfo->strPath);

		if (bRes)
			CDelHistory::GetPtr()->LogDelRun(pInfo->nRegType, pInfo->strName);
	}
	return bRes;
}

BOOL CRunManEngine::PreCopyFile( DWORD nType, LPCTSTR lpFileName )
{
	return !CDelHistory::GetPtr()->FindDelRunItem(nType, lpFileName);
}

BOOL CRunManEngine::PreCopyValue( DWORD nType, LPCTSTR lpValue )
{
	return !CDelHistory::GetPtr()->FindDelRunItem(nType, lpValue);
}

BOOL CRunManEngine::PreCopyTaskFile( DWORD nType, LPCTSTR lpFileName )
{
	return !CDelHistory::GetPtr()->FindDelTaskItem(nType,lpFileName);

}

VOID CRunManEngine::SynEnumRunInfo( DWORD dwFlags, IEnumRunCallBack* pCallBack )
{
	_InternEnumRunInfo(dwFlags,pCallBack);
}

DWORD CRunManEngine::GetRunCount( DWORD nFlags )
{
	DWORD	nCount = 0;
	if (nFlags == KSRUN_TYPE_STARTUP || nFlags == KSRUN_TYPE_ALL)
	{
		CRunDirEnumer*	pDirEnumer = new CRunDirEnumer;
		if (pDirEnumer != NULL)
		{
			pDirEnumer->DoEnum();
			for ( int i = 0; i < pDirEnumer->GetCount(); i++)
			{
				CRunDirEnumer::RUN_DIR_INFO&	rundirInfo = pDirEnumer->GetItem(i);
				CCmdLineParser cmdLine(rundirInfo.strPath);
				if (cmdLine.GetCmd()==NULL)
					continue;
				nCount++;
			}
		}
		CRunRegEnumer*	pRegEnumer = new CRunRegEnumer;
		if (pRegEnumer != NULL)
		{
			pRegEnumer->DoEnum();
			for ( int i = 0; i < pRegEnumer->GetCount(); i++)
			{
				CRunRegEnumer::REG_RUN_INFO&	runregInfo = pRegEnumer->GetItem(i);
				CCmdLineParser cmdLine(runregInfo.strPath);
				if (cmdLine.GetCmd()==NULL)
					continue;
				nCount++;
			}
		}
		if (nFlags != KSRUN_TYPE_ALL)
			goto Exit0;
	}
	if (nFlags == KSRUN_TYPE_SERVICE || nFlags == KSRUN_TYPE_ALL)
	{
		CServiceEnumer srvEnumer;
		nCount += srvEnumer.GetRunSrvCount();
		if (nFlags != KSRUN_TYPE_ALL)
			goto Exit0;
	}
	if (nFlags == KSRUN_TYPE_TASK || nFlags == KSRUN_TYPE_ALL)
	{
		CTaskSchedularEnumer* pTaskEnumer = new CTaskSchedularEnumer;

		if (pTaskEnumer != NULL)
		{
			pTaskEnumer->DoEnum();
			for ( int i = 0; i < pTaskEnumer->GetCount(); i++)
			{
				CTaskSchedularEnumer::TASK_JOB_INFO&	jobInfo = pTaskEnumer->GetItem(i);
				CString strAppName = GetFileName(jobInfo.strExePath);
				if (::PathFileExists(jobInfo.strExePath) && jobInfo.bEnable)
					nCount++;
			}
		}
		if (nFlags != KSRUN_TYPE_ALL)
			goto Exit0;
	}
Exit0:
	return nCount;
}

void CRunManEngine::_InternEnumRunInfo_Exam( DWORD dwFlags,IEnumRunCallBack* pCallback )
{
	if (m_pLibLoader==NULL)
	{
		m_pLibLoader = new CLibLoader;
		m_pLibLoader->LoadLib(NULL);
	}

	if (TRUE)
	{
		CRunDirEnumer	dirEnumer;
		if (EnumDirRunner2(TRUE,&dirEnumer,pCallback) > 0)
			goto Exit0;
	}

	if (TRUE)
	{
		CRunRegEnumer	regEnumer;
		if (EnumRegRunner2(TRUE,&regEnumer,pCallback))
			goto Exit0;
	}

	if (TRUE)
	{
		CServiceEnumer	servEnumer;
		if (EnumServiceRunner2(&servEnumer,pCallback) > 0)
			goto Exit0;
	}

	if (TRUE)
	{
		CTaskSchedularEnumer	taskEnumer;
		if (EnumTaskRunner2(&taskEnumer,pCallback) > 0)
			goto Exit0;
	}

Exit0:
	if (pCallback)
		pCallback->EndScan();

	m_hOpThread_Exam = NULL;
	return;
}

int CRunManEngine::EnumRegRunner2(BOOL bEnable, CRunRegEnumer* pEnumer, IEnumRunCallBack* pCallback)
{
	int nRet = 0;

	if (pEnumer->DoEnum())
	{
		for ( int i = 0; i < pEnumer->GetCount(); i++)
		{
			CKsafeRunInfo	cinfo;
			CRunRegEnumer::REG_RUN_INFO&	runreginfo = pEnumer->GetItem(i);

			CRunoptCmdLineParser cmdLine(runreginfo.strPath);
			CString strAppName = cmdLine.GetCmd();

			cinfo.nType		= KSRUN_TYPE_STARTUP;
			cinfo.strName	= runreginfo.strName;
			cinfo.strDisplay= runreginfo.strName;
			cinfo.bEnable	= bEnable;
			cinfo.strPath	= strAppName;
			cinfo.strParam	= runreginfo.strPath;
			cinfo.strExePath= strAppName;
			cinfo.nRegType	= runreginfo.nRegType;
			cinfo.bDisabledBy3xx = runreginfo.bDisabledBy3XX;
			if (!m_wow64Switcher.IsWin64_2())
				cinfo.bFileIsExist = cmdLine.IsPEFileExist();
			if ((bEnable == FALSE) && (cinfo.bFileIsExist == FALSE))//如果是已禁用的启动项，而且目标文件不存在，那就不管他
				continue;

			KSRunInfo*	pKSInfo = NULL;
			if (m_pLibLoader->FindDatInfo(CFindData(KSRUN_TYPE_STARTUP,runreginfo.strName,strAppName),&pKSInfo))
				ConvertLib(cinfo,*pKSInfo);
			PostGetItemDesc(cinfo);
			if (pCallback)
			{
				if ((cinfo.bFileIsExist == FALSE) && (cinfo.bShowRemain == FALSE))
					continue;
				else
				{
					if (pCallback->FindRunItem(&cinfo) == TRUE)
					{
						nRet++;
						goto Exit0;
					}
				}
			}
		}
	}

Exit0:
	return nRet;
}

int CRunManEngine::EnumDirRunner2(BOOL bEnable, CRunDirEnumer* pEnumer, IEnumRunCallBack* pCallback)
{
	int nRet = 0;
	if (pEnumer->DoEnum())
	{
		for ( int i = 0; i < pEnumer->GetCount(); i++)
		{
			CKsafeRunInfo	cinfo;
			CRunDirEnumer::RUN_DIR_INFO&	rundirinfo = pEnumer->GetItem(i);

			CString strParam;
			strParam.Format(_T("%s %s"),rundirinfo.strPath,rundirinfo.strParam);

			cinfo.nType			= KSRUN_TYPE_STARTUP;
			cinfo.strName		= GetFileName(rundirinfo.strName);
			cinfo.strPath		= rundirinfo.strName;	// 填写全路径
			cinfo.bEnable		= bEnable;
			cinfo.strExePath	= rundirinfo.strPath;	// LNK指向的EXE路径
			cinfo.strParam		= strParam;	
			cinfo.strDisplay	= cinfo.strName;
			cinfo.nRegType		= rundirinfo.nDirType;
			if (!m_wow64Switcher.IsWin64_2())
				cinfo.bFileIsExist	= ::PathFileExists(rundirinfo.strPath);

			KSRunInfo*	pKSInfo = NULL;
			if (m_pLibLoader->FindDatInfo(CFindData(KSRUN_TYPE_STARTUP,rundirinfo.strName,rundirinfo.strPath),&pKSInfo))
				ConvertLib(cinfo,*pKSInfo);
			PostGetItemDesc(cinfo);
			if (pCallback)
			{
				if ((cinfo.bFileIsExist == FALSE) && (cinfo.bShowRemain == FALSE))
					continue;
				else
				{
					if (pCallback->FindRunItem(&cinfo) == TRUE)
					{
						nRet++;
						goto Exit0;
					}
				}
			}
		}
	}
Exit0:
	return nRet;
}

int CRunManEngine::EnumServiceRunner2(CServiceEnumer* pEnumer, IEnumRunCallBack* pCallback)
{
	int nRet = 0;
	if (pEnumer->DoEnum())
	{
		for ( int i = 0; i < pEnumer->GetCount(); i++)
		{
			CKsafeRunInfo	cinfo;
			CServiceEnumer::SERVICE_CONFIG&	servInfo = pEnumer->GetItem(i);
			CString strCmdLine = servInfo.strServDLL.IsEmpty()?servInfo.lpqscfg->lpBinaryPathName:servInfo.strServDLL;
			CRunoptCmdLineParser cmdLine(strCmdLine);
			CString strAppPath = cmdLine.GetCmd();

			cinfo.nType		= KSRUN_TYPE_SERVICE;
			cinfo.strName	= servInfo.strName;
			cinfo.bEnable	= IsTypeStartEnable(servInfo.lpqscfg->dwStartType);
			if (cinfo.bEnable == FALSE)
				continue;
			cinfo.strDesc	= servInfo.strDesc;
			cinfo.strParam	= strCmdLine;
			cinfo.strDisplay= servInfo.lpqscfg->lpDisplayName;
			cinfo.strExePath= strAppPath;
			if (!m_wow64Switcher.IsWin64_2())
				cinfo.bFileIsExist = cmdLine.IsPEFileExist();

			KSRunInfo*	pKSInfo = NULL;
			if (m_pLibLoader->FindDatInfo(CFindData(KSRUN_TYPE_SERVICE,servInfo.strName,cinfo.strExePath),&pKSInfo))
				ConvertLib(cinfo,*pKSInfo);

			PostGetItemDesc(cinfo);
			if (pCallback)
			{
				if ((cinfo.bFileIsExist == FALSE) && (cinfo.bShowRemain == FALSE))
					continue;
				else
				{
					if (pCallback->FindRunItem(&cinfo) == TRUE)
					{
						nRet++;
						goto Exit0;
					}
				}
			}
		}
	}
Exit0:
	return nRet;
}

int CRunManEngine::EnumTaskRunner2(CTaskSchedularEnumer* pEnumer, IEnumRunCallBack* pCallback)
{	
	int nRet = 0;
	pEnumer->SetCopyFilter(this);
	if (pEnumer->DoEnum())
	{
		for ( int i = 0; i < pEnumer->GetCount(); i++)
		{
			CKsafeRunInfo	cinfo;
			CTaskSchedularEnumer::TASK_JOB_INFO&	jobInfo = pEnumer->GetItem(i);
			if (jobInfo.bEnable == FALSE)
				continue;
			CRunoptCmdLineParser cmdLine(jobInfo.strExePath);
			CString strAppPath = cmdLine.GetCmd();

			cinfo.nType		= KSRUN_TYPE_TASK;
			cinfo.strName	= jobInfo.strJobPath;
			cinfo.strPath	= strAppPath;
			cinfo.strDisplay= jobInfo.strName;
			cinfo.strParam	= jobInfo.strParam;
			cinfo.bEnable	= jobInfo.bEnable;
			if (cinfo.bEnable == FALSE)
				continue;
			cinfo.strExePath= strAppPath;
			cinfo.nJobType	= jobInfo.nType;
			cinfo.strDesc	= jobInfo.strDesc;
			if (!m_wow64Switcher.IsWin64_2())
				cinfo.bFileIsExist = cmdLine.IsPEFileExist();

			KSRunInfo*	pKSInfo = NULL;
			if (m_pLibLoader->FindDatInfo(CFindData(KSRUN_TYPE_TASK,jobInfo.strJobPath,strAppPath),&pKSInfo))
				ConvertLib(cinfo,*pKSInfo);

			PostGetItemDesc(cinfo);
			if (pCallback)
			{
				if ((cinfo.bFileIsExist == FALSE) && (cinfo.bShowRemain == FALSE))
					continue;
				else
				{
					if (pCallback->FindRunItem(&cinfo) == TRUE)
					{
						nRet++;
						goto Exit0;
					}
				}
			}
		}
	}
Exit0:
	return nRet;
}
