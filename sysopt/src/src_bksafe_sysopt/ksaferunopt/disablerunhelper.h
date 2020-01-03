
#pragma once

#include "runhelper.h"
#include <common/utility.h>
#include <atldef.h>
#include <globalstate/GetGlobalState.h>

#define KSAFE_DISABLE_HKLM_RUN_REG		_T("Software\\KSafe\\run\\Disable\\hklm")
#define KSAFE_DISABLE_HKCU_RUN_REG		_T("Software\\KSafe\\run\\Disable\\hkcu")
#define KSAFE_DISABLE_USER_RUN_DIR		_T("userrun")
#define KSAFE_DISABLE_COMM_RUN_DIR		_T("commrun")

#define S3XX_DISABLE_RUN_REG			_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\360Disabled")

class CAutoBuffer
{
public:
	CAutoBuffer(){
		m_buffer = NULL;
		m_nSize	 = 0;
	}
	virtual ~CAutoBuffer(){
		_free();
	}

	PVOID AllocBuffer(DWORD nSize)
	{
	//	ASSERT(nSize!=0);

		_free();

		if (nSize!=0)
		{
			m_buffer = new char[nSize];
			m_nSize	 = nSize;
		}
		return m_buffer;
	}

	PVOID GetPtr()
	{
		return m_buffer;
	}

	DWORD GetSize()
	{
		return m_nSize;
	}

protected:
	void _free()
	{
		if (m_buffer!=NULL)
		{
			delete m_buffer;
			m_buffer = NULL;
		}
		m_nSize = 0;
	}
protected:
	PVOID	m_buffer;	
	DWORD	m_nSize;
};

class IPreOpcallBack
{
public:
	virtual BOOL WriteRunKey(HKEY hRoot, LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpData) = 0;
	virtual BOOL WriteRunFile(LPCTSTR lpFile) = 0;
};

class CRegRunOper
{
public:
	CRegRunOper(){}

	virtual ~CRegRunOper(){}

public:
	BOOL ControlRun(BOOL bEnable, DWORD& nRegType, LPCTSTR lpStrValue,IPreOpcallBack* pcallBack=NULL)
	{
		if (bEnable)
			return EnableRun(nRegType,lpStrValue,pcallBack);
		else
			return DisableRun(nRegType,lpStrValue,pcallBack);
	}

	BOOL DeleteRun(DWORD nRegType, LPCTSTR lpstrValue)
	{
		HKEY		hRoot;
		CString		strKey;

		if (nRegType==REG_RUN_TYPE_KSAFE_HKLM)
		{
			hRoot  = HKEY_LOCAL_MACHINE;
			strKey = KSAFE_DISABLE_HKLM_RUN_REG;
		}
		else if (nRegType==REG_RUN_TYPE_KSAFE_HKCU)
		{
			hRoot  = HKEY_CURRENT_USER;
			strKey = KSAFE_DISABLE_HKCU_RUN_REG;
		}
		else if (nRegType==REG_RUN_TYPE_WINDOW_HKLM)
		{
			hRoot  = HKEY_LOCAL_MACHINE;
			strKey = STR_RUN_REG_KEY;
		}
		else if (nRegType==REG_RUN_TYPE_WINDOW_HKCU)
		{
			hRoot  = HKEY_CURRENT_USER;
			strKey = STR_RUN_REG_KEY;
		}
		BOOL bRes = _DeleteRegValue(hRoot,strKey,lpstrValue);

		// 把自己目录下的干掉
		if (nRegType==REG_RUN_TYPE_WINDOW_HKLM)
		{
			hRoot  = HKEY_LOCAL_MACHINE;
			strKey = KSAFE_DISABLE_HKLM_RUN_REG;
		}
		else if (nRegType==REG_RUN_TYPE_WINDOW_HKCU)
		{
			hRoot  = HKEY_CURRENT_USER;
			strKey = KSAFE_DISABLE_HKCU_RUN_REG;
		}
		_DeleteRegValue(hRoot,strKey,lpstrValue);
		return bRes;
	}

protected:
	BOOL _DeleteRegValue(HKEY hRoot, LPCTSTR lpKey, LPCTSTR lpValue)
	{
		return ::SHDeleteValue(hRoot,lpKey,lpValue)==ERROR_SUCCESS;
	}

	BOOL EnableRun(DWORD& nRegType, LPCTSTR lpStrValue,IPreOpcallBack* pcallBack)
	{
		DWORD	nNewRegtype = 0;
		HKEY	hRoot1;
		CString	strKey1;

		HKEY	hRoot2;
		CString	strKey2;


		ATLASSERT(nRegType!=REG_RUN_TYPE_WINDOW_HKLM&&nRegType!=REG_RUN_TYPE_WINDOW_HKCU);

		if (nRegType==REG_RUN_TYPE_KSAFE_HKLM)
		{
			hRoot1  = HKEY_LOCAL_MACHINE;
			strKey1 = KSAFE_DISABLE_HKLM_RUN_REG;

			hRoot2 = HKEY_LOCAL_MACHINE;

			nNewRegtype = REG_RUN_TYPE_WINDOW_HKLM;
		}
		else if (nRegType==REG_RUN_TYPE_KSAFE_HKCU)
		{
			hRoot1  = HKEY_CURRENT_USER;
			strKey1 = KSAFE_DISABLE_HKCU_RUN_REG;

			hRoot2 = HKEY_CURRENT_USER;

			nNewRegtype = REG_RUN_TYPE_WINDOW_HKCU;
		}
		strKey2 = STR_RUN_REG_KEY;

		if ( _MoveRegKey(hRoot1,strKey1,hRoot2,strKey2,lpStrValue,pcallBack) )
		{
			nRegType = nNewRegtype;
			return TRUE;
		}
		else
			return FALSE;
	}

	BOOL DisableRun(DWORD& nRegType, LPCTSTR lpStrValue,IPreOpcallBack* pcallBack)
	{
		DWORD	nNewRegtype = 0;
		HKEY	hRoot1;
		CString	strKey1;

		HKEY	hRoot2;
		CString	strKey2;


		ATLASSERT(nRegType==REG_RUN_TYPE_WINDOW_HKLM||nRegType==REG_RUN_TYPE_WINDOW_HKCU);

		if (nRegType==REG_RUN_TYPE_WINDOW_HKLM)
		{
			hRoot1  = HKEY_LOCAL_MACHINE;
			strKey1 = STR_RUN_REG_KEY;

			hRoot2 = HKEY_LOCAL_MACHINE;
			strKey2 = KSAFE_DISABLE_HKLM_RUN_REG;

			nNewRegtype = REG_RUN_TYPE_KSAFE_HKLM;
		}
		else if (nRegType==REG_RUN_TYPE_WINDOW_HKCU)
		{
			hRoot1  = HKEY_CURRENT_USER;
			strKey1 = STR_RUN_REG_KEY;

			hRoot2  = HKEY_CURRENT_USER;
			strKey2 = KSAFE_DISABLE_HKCU_RUN_REG;

			nNewRegtype = REG_RUN_TYPE_KSAFE_HKCU;
		}

		if( _MoveRegKey(hRoot1,strKey1,hRoot2,strKey2,lpStrValue,pcallBack) )
		{
			nRegType = nNewRegtype;
			return TRUE;
		}
		return FALSE;
	}

protected:
	BOOL _MoveRegKey(HKEY hRoot1, LPCTSTR strKey1, HKEY hRoot2, LPCTSTR strKey2, LPCTSTR lpStrValue, IPreOpcallBack* pCB=NULL)
	{
		DWORD		nType = 0;
		CAutoBuffer	atBuffer;
		DWORD		nSize = 0;
		DWORD		nRet = ERROR_SUCCESS;
		DWORD		nAllocSize = MAX_PATH;

		do 
		{
			atBuffer.AllocBuffer(nAllocSize);

			nAllocSize+= MAX_PATH;
			nType = 0;
			nSize = atBuffer.GetSize();

			nRet = ::SHGetValue(hRoot1,strKey1,lpStrValue,&nType,atBuffer.GetPtr(),&nSize);

			if (nRet==ERROR_SUCCESS)
				break;
			else if (nRet!=ERROR_MORE_DATA)
				break;
		} while (TRUE);

		if (nRet==ERROR_SUCCESS)
		{
			if (pCB!=NULL && (nType==REG_SZ || nType==REG_EXPAND_SZ) )
			{
				if (!pCB->WriteRunKey(hRoot2,strKey2,lpStrValue,(LPCTSTR)atBuffer.GetPtr()) )
					return FALSE;
			}

			nRet = ::SHSetValue(hRoot2,strKey2,lpStrValue,nType,atBuffer.GetPtr(),nSize);
			if (nRet==ERROR_SUCCESS)
			{
				nRet = ::SHDeleteValue(hRoot1,strKey1,lpStrValue);

				if (nRet != ERROR_SUCCESS )
				{
					::SHDeleteValue(hRoot2,strKey2,lpStrValue);
				}
			}
		}
		return (nRet==ERROR_SUCCESS);
	}
};

class IEnumRunFilter
{
public:
	virtual BOOL PreCopyFile(DWORD nType, LPCTSTR lpFileName) = 0;
	virtual BOOL PreCopyValue(DWORD nType, LPCTSTR lpValue) = 0;
};

class CKSRunDirEnumer : public CRunDirEnumer
{
public:
	CKSRunDirEnumer(){
		m_pCopyFilter = NULL;
	}
	virtual ~CKSRunDirEnumer(){}

	void SetCopyFilter(IEnumRunFilter* pFilter)
	{
		m_pCopyFilter = pFilter;
	}
public:

	virtual BOOL DoEnum()
	{
		_PreEnum();

		CString	strDir;

		if (GetEnumDir(FALSE,strDir))
			_EnumRunDir(strDir, DIR_RUN_TYPE_KSAFE_COMM);
		if (GetEnumDir(TRUE,strDir))
			_EnumRunDir(strDir, DIR_RUN_TYPE_KSAFE_USER);

		return TRUE;
	}

	static BOOL GetEnumDir(BOOL bCurrentUser, CString& strpath)
	{

		if ( bCurrentUser )
		{
			TCHAR	szBuffer[MAX_PATH] = {0};
			if ( SHGetSpecialFolderPath(NULL,szBuffer,CSIDL_APPDATA,TRUE) )
			{
				CString	strDir = szBuffer;
				if (strDir.IsEmpty())
				{
					return FALSE;
				}
				strDir+=_T("\\KSafe\\");
				if (!PathFileExists(strDir))
				{
					::CreateDirectory(strDir,NULL);
				}
				if (PathIsDirectory(strDir))
				{
					strDir += KSAFE_DISABLE_USER_RUN_DIR;
				}
				if (!PathFileExists(strDir))
				{
					::CreateDirectory(strDir,NULL);
				}
				if (PathIsDirectory(strDir))
				{
					strpath = strDir;
					return TRUE;
				}
			}
			return FALSE;
		}
		else
		{
			CString	strCache;
			CAppPath::Instance().GetLeidianCachePath(strCache,TRUE);

			strCache.Append(_T("\\"));
			strCache.Append(KSAFE_DISABLE_COMM_RUN_DIR);
			strpath = strCache;
			if (!::PathIsDirectory(strCache))
			{
				return ::CreateDirectory(strCache,NULL);
			}
			else
				return TRUE;
		}
	}

protected:
	BOOL Get3XXDisDir(BOOL bCurrent, CString& strDir)
	{
		CString	strBuf;
		DWORD	nFlags = bCurrent?CSIDL_APPDATA:CSIDL_COMMON_APPDATA;

		if ( ::SHGetSpecialFolderPath(NULL,strBuf.GetBuffer(MAX_PATH),nFlags,FALSE) )
		{
			strBuf.ReleaseBuffer();
			strBuf += _T("\\360safe\\360Disabled");
			
			if (PathIsDirectory(strBuf))
			{
				strDir = strBuf;
				return TRUE;
			}
		}
		else
			strBuf.ReleaseBuffer();

		return FALSE;
	}

	virtual void _PreEnum()
	{
		if (TRUE)
		{
			CString	str3XDir;
			CString	strKSdir;
			
			if ( Get3XXDisDir(TRUE,str3XDir) && GetEnumDir(TRUE,strKSdir) )
			{
				_CopyDir(DIR_RUN_TYPE_KSAFE_USER,strKSdir,str3XDir);
			}
		}
		if (TRUE)
		{
			CString	str3XDir;
			CString	strKSdir;

			if ( Get3XXDisDir(FALSE,str3XDir) && GetEnumDir(FALSE,strKSdir) )
			{
				_CopyDir(DIR_RUN_TYPE_KSAFE_COMM,strKSdir,str3XDir);
			}
		}
	}

	void _CopyDir(DWORD nType, LPCTSTR lpDstDir, LPCTSTR lpSrcDir)
	{
		CEnumFile	enumer(lpSrcDir,_T("*.*"));

		for ( int i=0; i<enumer.GetFileCount(); i++)
		{
			CString strDstF = lpDstDir;
			strDstF.Append(_T("\\"));
			strDstF.Append(enumer.GetFileName(i));

			if (m_pCopyFilter && !m_pCopyFilter->PreCopyFile(nType,enumer.GetFileFullPath(i)))
				continue;

			if (!PathFileExists(strDstF))
			{
				CopyFile(enumer.GetFileFullPath(i),strDstF,TRUE);
			}
		}
	}

protected:
	IEnumRunFilter* m_pCopyFilter;
};


class CDirRunOper
{
public:
	CDirRunOper(){}
	virtual ~CDirRunOper(){}

public:

	BOOL ControlRun2(BOOL bEnable, DWORD& nRegType, LPCTSTR lpFileName,IPreOpcallBack* pcallBack=NULL)
	{
		CString		strName = lpFileName;
		if (nRegType>DIR_RUN_TYPE_BEGIN)
		{
			CString	strDir;
			if (nRegType==DIR_RUN_TYPE_WINDOW_COMM)
				strDir = GetGlobalState()->GetCommonStartRunDir();
			else if (nRegType==DIR_RUN_TYPE_WINDOW_USER)
				strDir = GetGlobalState()->GetUserStartRunDir();
			else if (nRegType==DIR_RUN_TYPE_KSAFE_COMM)
				CKSRunDirEnumer::GetEnumDir(FALSE,strDir);
			else if (nRegType==DIR_RUN_TYPE_KSAFE_USER)
				CKSRunDirEnumer::GetEnumDir(TRUE,strDir);
			
			if (strDir.IsEmpty())
				return FALSE;

			strDir.Append(_T("\\"));
			strDir.Append(strName);
			strName = strDir;
		}

		if (bEnable)
			return EnableRun(nRegType,strName,pcallBack);
		else
			return DisableRun(nRegType,strName);
	}

	BOOL ControlRun(BOOL bEnable, DWORD& nRegType, CString& strFilePath,IPreOpcallBack* pcallBack=NULL)
	{
		if (bEnable)
			return EnableRun(nRegType,strFilePath,pcallBack);
		else
			return DisableRun(nRegType,strFilePath);
	}

	BOOL DeleteRun(DWORD nRegType, LPCTSTR lpFileName)
	{
		if (nRegType==DIR_RUN_TYPE_WINDOW_USER || nRegType==DIR_RUN_TYPE_WINDOW_COMM )
		{
			CString	strFileName = GetFileName(lpFileName);
			CString	strBakFileName;

			if (nRegType==DIR_RUN_TYPE_WINDOW_COMM)
				CKSRunDirEnumer::GetEnumDir(FALSE,strBakFileName);
			else if (nRegType==DIR_RUN_TYPE_WINDOW_USER)
				CKSRunDirEnumer::GetEnumDir(TRUE,strBakFileName);

			strBakFileName.Append(_T("\\"));
			strBakFileName.Append(strFileName);

			::DeleteFile(strBakFileName);
		}
		return ::DeleteFile(lpFileName);
	}

protected:
	BOOL DisableRun(DWORD & nType,CString& strFilePath)
	{
		CString	strFileName = GetFileName(strFilePath);
		CString	strNewFileName;

		ATLASSERT(nType==DIR_RUN_TYPE_WINDOW_COMM||nType==DIR_RUN_TYPE_WINDOW_USER);

		if (nType==DIR_RUN_TYPE_WINDOW_COMM)
			CKSRunDirEnumer::GetEnumDir(FALSE,strNewFileName);
		else
			CKSRunDirEnumer::GetEnumDir(TRUE,strNewFileName);

		strNewFileName.Append(_T("\\"));
		strNewFileName.Append(strFileName);

		if (::MoveFileEx(strFilePath,strNewFileName,MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING))
		{
			nType = (nType==DIR_RUN_TYPE_WINDOW_COMM)?DIR_RUN_TYPE_KSAFE_COMM:DIR_RUN_TYPE_KSAFE_USER;
			strFilePath = strNewFileName;
			return TRUE;
		}
		return FALSE;
	}

	BOOL EnableRun(DWORD& nType,CString& strFilePath,IPreOpcallBack* pcallBack=NULL)
	{
		CString	strFileName = GetFileName(strFilePath);
		CString	strNewFileName;

		ATLASSERT(nType==DIR_RUN_TYPE_KSAFE_COMM||nType==DIR_RUN_TYPE_KSAFE_USER);

		if (nType==DIR_RUN_TYPE_KSAFE_COMM)
			strNewFileName = GetGlobalState()->GetCommonStartRunDir();
		else
			strNewFileName = GetGlobalState()->GetUserStartRunDir();

		strNewFileName.Append(_T("\\"));
		strNewFileName.Append(strFileName);

		if (pcallBack)
		{
			if (!pcallBack->WriteRunFile(strNewFileName))
				return FALSE;
		}
		if (::MoveFileEx(strFilePath,strNewFileName,MOVEFILE_COPY_ALLOWED))
		{
			nType = (nType==DIR_RUN_TYPE_KSAFE_COMM)?DIR_RUN_TYPE_WINDOW_COMM:DIR_RUN_TYPE_WINDOW_USER;
			strFilePath = strNewFileName;
			return TRUE;
		}
		return FALSE;
	}
};

class CKSRunRegEnumer : public CRunRegEnumer
{
public:
	CKSRunRegEnumer(){
		m_pCopyFilter = NULL;
	}
	virtual ~CKSRunRegEnumer(){}

	void SetCopyFilter(IEnumRunFilter* pFilter)
	{
		m_pCopyFilter = pFilter;
	}
public:
	virtual void _PreEnum()
	{
		_Copy3XXToKSafe(REG_RUN_TYPE_KSAFE_HKLM, HKEY_LOCAL_MACHINE,KSAFE_DISABLE_HKLM_RUN_REG,HKEY_LOCAL_MACHINE,S3XX_DISABLE_RUN_REG);
		_Copy3XXToKSafe(REG_RUN_TYPE_KSAFE_HKCU, HKEY_CURRENT_USER,KSAFE_DISABLE_HKCU_RUN_REG,HKEY_CURRENT_USER,S3XX_DISABLE_RUN_REG);
	}

	virtual BOOL DoEnum()
	{
		_PreEnum();

		_EnumRegRun(HKEY_LOCAL_MACHINE,KSAFE_DISABLE_HKLM_RUN_REG, REG_RUN_TYPE_KSAFE_HKLM);
		_EnumRegRun(HKEY_CURRENT_USER,KSAFE_DISABLE_HKCU_RUN_REG, REG_RUN_TYPE_KSAFE_HKCU);

		return TRUE;
	}

protected:

	virtual void _EnumRegRun(HKEY hRoot, LPCTSTR lpRunKey, DWORD nType)
	{
		HKEY	hKey = NULL;

		if (ERROR_SUCCESS==::RegOpenKeyEx(hRoot,lpRunKey,0,KEY_READ,&hKey))
		{
			DWORD	iIndex = 0;
			CString	strData;
			CString	strValue;
			while (SHEnumStringValue(hKey,iIndex++,strValue,strData)==ERROR_SUCCESS)
			{
				REG_RUN_INFO	runinfo;
				runinfo.strName	= strValue;
				runinfo.strPath	= strData;
				runinfo.nRegType= nType;
				runinfo.bDisabledBy3XX = IsDisabledBy3XX(strValue);

				m_runArray.Add(runinfo);
				strData.Empty();
				strValue.Empty();
			}
		}
	}

	BOOL IsDisabledBy3XX(CString strKeyValueName)
	{
		HKEY hKey = NULL;
		HKEY hRootKey = HKEY_LOCAL_MACHINE;
		HKEY hRootKey2 = HKEY_CURRENT_USER;
		BOOL bRet = FALSE;
		if (ERROR_SUCCESS==::RegOpenKeyEx( hRootKey,
			_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\360Disabled"),
			0,
			KEY_READ,
			&hKey))
		{
			DWORD	iIndex = 0;
			CString	strData;
			CString	strValue;
			while (SHEnumStringValue(hKey,iIndex++,strValue,strData)==ERROR_SUCCESS)
			{
				if (!strData.IsEmpty() && _tcslen(strData) > _tcslen(_T("rem ")) )
				{
					CString		strLeft = strData.Left(4);

					if (strValue.CompareNoCase(strKeyValueName) == 0 )
					{
						bRet = TRUE;
						goto Exit0;
					}
				}
				strData.Empty();
				strValue.Empty();
			}
		}
		if (ERROR_SUCCESS==::RegOpenKeyEx( hRootKey2,
			_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\360Disabled"),
			0,
			KEY_READ,
			&hKey))
		{
			DWORD	iIndex = 0;
			CString	strData;
			CString	strValue;
			while (SHEnumStringValue(hKey,iIndex++,strValue,strData)==ERROR_SUCCESS)
			{
				if (!strData.IsEmpty() && _tcslen(strData) > _tcslen(_T("rem ")) )
				{
					CString		strLeft = strData.Left(4);

					if (strValue.CompareNoCase(strKeyValueName) == 0 )
					{
						bRet = TRUE;
						goto Exit0;
					}
				}
				strData.Empty();
				strValue.Empty();
			}
		}
	Exit0:
		return bRet;
	}

	void _Copy3XXToKSafe(DWORD nRegType, HKEY hRootDst, LPCTSTR strKeyDst, HKEY hRootSrc, LPCTSTR strKeySrc)
	{
		HKEY	hKey = NULL;

		if (ERROR_SUCCESS==::RegOpenKeyEx(hRootSrc,strKeySrc,0,KEY_READ,&hKey))
		{
			DWORD	iIndex = 0;
			CString	strData;
			CString	strValue;
			while (SHEnumStringValue(hKey,iIndex++,strValue,strData)==ERROR_SUCCESS)
			{
				if (!strData.IsEmpty() && _tcslen(strData) > _tcslen(_T("rem ")) )
				{
					CString		strLeft = strData.Left(4);

					if (strLeft.CompareNoCase(_T("rem ")) == 0 )
					{
						CString	strNewData = strData.Mid(4);
						DWORD nType = REG_SZ;
						DWORD nRet	= ::SHGetValue(hRootDst,strKeyDst,strValue,&nType,NULL,0);

						if (nRet!=ERROR_MORE_DATA)
						{
							if ( !m_pCopyFilter || m_pCopyFilter->PreCopyValue(nRegType,strValue) )
								::SHSetValue(hRootDst,strKeyDst,strValue,REG_SZ,strNewData, (DWORD)(_tcslen(strNewData)*sizeof(TCHAR)));
						}
					}
				}

				strData.Empty();
				strValue.Empty();
			}
		}
	}
protected:
	IEnumRunFilter* m_pCopyFilter;
};


