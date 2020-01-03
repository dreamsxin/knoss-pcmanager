// Copyright (c) 2010 Kingsoft Corporation. All rights reserved.
// Copyright (c) 2010 The KSafe Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "stdafx.h"
#include "kvideocache.h"
#include "stubbornfiles.h"
#include "misc/KppUserLog.h"

BOOL _ScanTudouCache(ITraverseFile *pTF, CString& strPath, int nStopIndex)
{
	TCHAR szLongPathBuffer[MAX_PATH] = { 0 };
	CString strResult;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA wfd = {0};
	FINDFILEDATA filedata = {0};
	HANDLE hSubFile = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA wfdSub = {0};
	DWORD len = sizeof(szLongPathBuffer);
	BOOL bRet =FALSE;
	CString strTemp;
	CString strTempEx;
	int nPos = -1;
	if ( g_bStop[nStopIndex] )
	{
		bRet = TRUE;
		goto clean0;
	}
	GetRegistryValue(HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\飞速土豆",
		L"UninstallString",
		NULL,
		(LPBYTE)szLongPathBuffer,
		&len
		);
	if(wcslen(szLongPathBuffer) == 0)
	{
		GetRegistryValue(HKEY_CURRENT_USER,
			L"Software\\Tudou\\TudouVA",
			L"",
			NULL,
			(LPBYTE)szLongPathBuffer,
			&len
			);
		if (wcslen(szLongPathBuffer) == 0)
		{
			bRet = FALSE;
			goto clean0;
		}
		else
		{
			::PathAppend(szLongPathBuffer,L"tudouva.ini");
			IniEditor IniEdit;
			IniEdit.SetFile(szLongPathBuffer);
			std::wstring tmpPath = IniEdit.ReadString(L"public",L"savedirectory");
			if(tmpPath.length() == 0)
			{
				bRet = FALSE;
				goto clean0;
			}
			else
			{
				strResult = tmpPath.c_str();
			}
		}
	}
	else
	{
		::PathRemoveFileSpec(szLongPathBuffer);
		::PathAppend(szLongPathBuffer,L"tudouva.ini");
		IniEditor IniEdit;
		IniEdit.SetFile(szLongPathBuffer);
		std::wstring tmpPath = IniEdit.ReadString(L"public",L"savedirectory");
		if(tmpPath.length() == 0)
		{
			bRet = FALSE;
			goto clean0;
		}
		else
		{
			strResult = tmpPath.c_str();
		}
	}

//	strResult = L"D:\\work\\kclear_bug\\";
	strPath = strResult;

	nPos = strResult.ReverseFind('\\');
	if (nPos != strResult.GetLength() - 1)
	{
		strResult += L"\\";
	}
	strTemp = strResult;
	strTempEx = strResult;
	strTemp += L"*.*";
	hFile = FindFirstFile(strTemp, &wfd);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		bRet = FALSE;
		goto clean0;
	}

	do
	{
		if ( g_bStop[nStopIndex] )
		{
			bRet = TRUE;
			goto clean0;
		}
		CString strFileName;
		CString strTempResult;
		strFileName = wfd.cFileName;
		if (_wcsicmp(strFileName.GetBuffer(), L".") == 0 
			|| _wcsicmp(strFileName.GetBuffer(), L"..") == 0 
			|| !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			continue;
		}
		strTempResult = strResult;
		strTempResult += strFileName;

		strResult = strTempResult;

		strFileName += L"_*.*.tdd!";
		strTempResult += L"\\";
		strTempResult += strFileName;

		hSubFile = FindFirstFile(strTempResult, &wfdSub);
		if (hSubFile == INVALID_HANDLE_VALUE)
		{
			strResult = strTempEx;
			continue;
		}

		do
		{
			if ( g_bStop[nStopIndex] )
			{
				bRet = TRUE;
				goto clean0;
			}
			strFileName = wfdSub.cFileName;
			if (_wcsicmp(strFileName.GetBuffer(), L".") == 0 
				|| _wcsicmp(strFileName.GetBuffer(), L"..") == 0
				/*|| wfd.nFileSizeLow > 2 * 1024 * 1024*/)
			{
				continue;
			}
			strTempResult = strResult;
			filedata.pszPathName = strResult;
			filedata.pszFileNmae = strFileName;
			filedata.pszExtName = L"_*.*.tdd!";
			filedata.uFileSize   = wfdSub.nFileSizeHigh;
			filedata.uFileSize   = filedata.uFileSize << 32;
			filedata.uFileSize  |= wfdSub.nFileSizeLow;

			strTempResult +=L"\\";
			strTempResult += strFileName;

            if(CStubbornFiles::Instance().IsStubbornFile(strTempResult))
                continue;

			CString strLog = L"[Scan][TuDou]:";
			strLog += strTempResult;
			KppUserLog::UserLogInstance().WriteSingleLog(strLog.GetBuffer());
			//strResult = strTempResult;
			if (pTF)
			{
				pTF->TraverseFile(&filedata);
			}

		}while(FindNextFile(hSubFile, &wfdSub));
		strResult = strTempEx;

	}while(FindNextFile(hFile, &wfd));

	bRet = TRUE;
clean0:
	if (hFile != INVALID_HANDLE_VALUE)
	{
		FindClose(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}

	if (hSubFile != INVALID_HANDLE_VALUE)
	{
		FindClose(hSubFile);
		hSubFile = INVALID_HANDLE_VALUE;
	}

	return TRUE;
}

BOOL _ScanKuaiboCache(ITraverseFile *pTF, CString& strPath, int nStopIndex)
{
	
	BOOL bRet = FALSE;
	WCHAR szLongPathBuffer[MAX_PATH] = {0};
	DWORD len = sizeof(szLongPathBuffer);
	CString strResult;
	GetRegistryValue(HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\QvodPlayer",
		L"UninstallString",
		NULL,
		(LPBYTE)szLongPathBuffer,
		&len
		);
	if(wcslen(szLongPathBuffer) == 0)
	{
		bRet = FALSE;
		goto clean0;
	}
	else
	{
		::PathRemoveFileSpec(szLongPathBuffer);
		::PathAppend(szLongPathBuffer,L"QvodPlayer.xml");
		TiXmlDocument xmlDoc;
		const TiXmlElement *pXmlQvodPlayer = NULL;
		const TiXmlElement *pXmlGeneral = NULL;
		strResult = L"%qvod%";
		if (!xmlDoc.LoadFile(UnicodeToAnsi(szLongPathBuffer).c_str(), TIXML_ENCODING_UTF8))
			goto clean0;
		pXmlQvodPlayer = xmlDoc.FirstChildElement("QvodPlayer");
		if (!pXmlQvodPlayer)
			goto clean0;
		pXmlGeneral = pXmlQvodPlayer->FirstChildElement("General");
		if (pXmlGeneral)
		{
			std::string strTmp(pXmlGeneral->Attribute("Defaultsavepath"));
			if(strTmp.length() == 0)
			{
				bRet = FALSE;
				goto clean0;
			}
			else strResult = Utf8ToUnicode(strTmp).c_str();
		}
	}
	ScanFileBasedPathAndName(L"Qvod",pTF, strResult, L"*.!mv", 0);
clean0:
	return bRet;
}

BOOL _ScanKankanCache(ITraverseFile *pTF, CString& strPath, int nStopIndex)
{
	BOOL bRet = FALSE;
	TCHAR szBuffer[MAX_PATH] = { 0 };
	DWORD len = sizeof(szBuffer);
	CString strTempVOD;
	CString strTempXMP;
	CString strTemp;
	int nPos = -1;

	GetRegistryValue(HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\Thunder network\\Xmp",
		L"storepath",
		NULL,
		(LPBYTE)szBuffer,
		&len
		);
	if(wcslen(szBuffer) == 0)
	{
		bRet = FALSE;
		goto clean0;
	}

	strTemp = szBuffer;
	nPos = strTemp.ReverseFind('\\');
	strPath = strTemp;
	if (nPos != strTemp.GetLength() - 1)
	{
		strTemp += L"\\";
	}
	strTempVOD = strTemp + L"VODCache";
	strTempXMP = strTemp + L"XMPCache";

	ScanKankanCacheEx(pTF, strTempVOD, nStopIndex, 1);
	ScanKankanCacheEx(pTF, strTempXMP, nStopIndex, 1);
	if (strTemp.GetLength() > 3)
	{
		ScanKankanCacheEx(pTF, strTemp, nStopIndex, 2);
	}
	//hFile = FindFirstFile()

clean0:
	return bRet;
}

BOOL ScanKankanCacheEx(ITraverseFile *pTF, const CString strPath, int nStopIndex, int nFlag)
{
	BOOL bRet = FALSE;

	if(nFlag == 2)
	{
		ScanFileBasedPathAndName(L"xlkankan",pTF, strPath, L"*.xv", nStopIndex, TRUE, 48);
		ScanFileBasedPathAndName(L"xlkankan",pTF, strPath, L"*.!xm",nStopIndex, TRUE, 48);
	}
	else if (nFlag == 1)
	{
		HANDLE hFile = INVALID_HANDLE_VALUE;
		WIN32_FIND_DATA wfd = {0};

		CString strFullPath;
		CString strTemp;
		strFullPath = strPath;
		strFullPath += L"\\*.*";


		hFile = FindFirstFile(strFullPath, &wfd);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			bRet = FALSE;
			goto clean0;
		}
		do 
		{
			if ( g_bStop[nStopIndex] )
			{
				bRet = TRUE;
				goto clean0;
			}
			if (_wcsicmp(wfd.cFileName, L".") == 0 
				|| _wcsicmp(wfd.cFileName, L"..") == 0 
				|| !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				continue;
			}

			CString strFileName;
			strFileName = wfd.cFileName;
			if (strFileName.GetLength() != 40 && nFlag == 1)
			{
				continue;
			}
			CString strTempPath;
			strTempPath = strPath;
			strTempPath += L"\\";
			strTempPath += strFileName;

			if (nFlag == 1)
			{
				ScanFileBasedPathAndName(L"xlkankan",pTF, strTempPath, L"*.dat", nStopIndex, TRUE, 48);
				ScanFileBasedPathAndName(L"xlkankan",pTF, strTempPath, L"*.cfg", nStopIndex, TRUE, 48);
			}

		} while(FindNextFile(hFile, &wfd));

		bRet = TRUE;
clean0:
		if (hFile != INVALID_HANDLE_VALUE)
		{
			FindClose(hFile);
			hFile = INVALID_HANDLE_VALUE;
		}
	}
	return bRet;
}

BOOL ScanFileBasedPathAndName(CString strItem,ITraverseFile *pTF, const CString strPath, const CString strName,  int nStopIndex, BOOL bTime, DWORD dwTime)
{
	BOOL bRet = FALSE;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA wfd = {0};
	FINDFILEDATA filedata = {0};
	CString strFilePath;
	strFilePath = strPath;
	strFilePath += L"\\";
	strFilePath += strName;

	hFile = FindFirstFile(strFilePath, &wfd);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		bRet = FALSE;
		goto clean0;
	}
	do 
	{
		if ( g_bStop[nStopIndex] )
		{
			bRet = TRUE;
			goto clean0;
		}
		CString strFileName;
		strFileName = wfd.cFileName;

		filedata.pszPathName = strPath;
		filedata.pszFileNmae = strFileName;
		filedata.pszExtName = strName;
		filedata.uFileSize   = wfd.nFileSizeHigh;
		filedata.uFileSize   = filedata.uFileSize << 32;
		filedata.uFileSize  |= wfd.nFileSizeLow;

		if (bTime && FileTimeInnerEx(wfd.ftLastWriteTime) < dwTime)
		{
			continue;
		}

        if(CStubbornFiles::Instance().IsStubbornFile(strPath+L"\\"+strFileName))
            continue;

		CString strLog = L"[Scan][";
		strLog += strItem;
		strLog += L"]:";
		strLog += strPath;
		strLog += L"\\";
		strLog += strFileName;
		KppUserLog::UserLogInstance().WriteSingleLog(strLog.GetBuffer());

		if (pTF)
		{
			pTF->TraverseFile(&filedata);
		}
	} while(FindNextFile(hFile, &wfd));
	bRet = TRUE;
clean0:
	if (hFile != INVALID_HANDLE_VALUE)
	{
		FindClose(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
	return bRet;
}

BOOL _ScanYoukuCache(ITraverseFile *pTF, CString& strPath, int nStopIndex)
{
	BOOL bRet = FALSE;
	TCHAR szBuffer[MAX_PATH] = {0};
	DWORD len = sizeof(szBuffer);
	CString strResult;
	CString strName;
	GetRegistryValue(HKEY_CURRENT_USER,
		L"SOFTWARE\\youku\\iKuAcc",
		L"CachePath",
		NULL,
		(LPBYTE)szBuffer,
		&len
		);
	if(wcslen(szBuffer) == 0)
	{
		bRet = FALSE;
		goto clean0;
	}
	else
	{
		strResult =szBuffer;
	}
    strPath = strResult;
	strName = L"cache_*.dat";

	bRet = ScanFileBasedPathAndName(L"YouKu",pTF, strResult, strName, nStopIndex);
	 
clean0:
	return bRet;
}

BOOL _ScanKu6Cache(ITraverseFile *pTF, CString& strPath, int nStopIndex)
{
	BOOL bRet = FALSE;
	WCHAR szLongPathBuffer[MAX_PATH] = {0};
	CString strResult;
	CString strTemp;

	DWORD len = sizeof(szLongPathBuffer);
	GetRegistryValue(HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Ku6SpeedUpper",
		L"UninstallString",
		NULL,
		(LPBYTE)szLongPathBuffer,
		&len
		);
	if(wcslen(szLongPathBuffer) == 0)
	{
		CString StrSuffix = _T("\\kvideo_cache");;
		strResult = GetDrive(StrSuffix);
	}
	else
	{
		::PathRemoveFileSpec(szLongPathBuffer);
		::PathAppend(szLongPathBuffer,L"Ku6SpeedUpper.ini");
		IniEditor IniEdit;
		IniEdit.SetFile(szLongPathBuffer);
		std::wstring tmpPath = IniEdit.ReadString(L"Settings",L"BufPath");
		if(tmpPath.length() > 0)
		{
			strResult = tmpPath.c_str();
		}
		else
		{
			CString StrSuffix = _T("\\kvideo_cache");;
			strResult = GetDrive(StrSuffix);

		}
	}
    strPath = strResult;

	bRet = ScanKu6CacheEx(pTF, strResult, nStopIndex);
	strTemp = strResult + L"\\cachetemp";
	bRet = ScanKu6CacheEx(pTF, strTemp, nStopIndex );
	strTemp = strResult + L"\\cachestatic";
	bRet = ScanKu6CacheEx(pTF, strTemp, nStopIndex );

	return bRet;
}

BOOL ScanKu6CacheEx(ITraverseFile *pTF, const CString strPath, int nStopIndex)
{
	BOOL bRet = FALSE;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA wfd = {0};

	CString strFullPath;
	CString strTemp;
	strFullPath = strPath;
	strFullPath += L"\\*.*";

	hFile = FindFirstFile(strFullPath, &wfd);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		bRet = FALSE;
		goto clean0;
	}
	do 
	{
		if ( g_bStop[nStopIndex] )
		{
			bRet = TRUE;
			goto clean0;
		}
		if (_wcsicmp(wfd.cFileName, L".") == 0 
			|| _wcsicmp(wfd.cFileName, L"..") == 0 
			|| !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			continue;
		}

		CString strFileName;
		strFileName = wfd.cFileName;
		if (strFileName.GetLength() != 32)
		{
			continue;
		}
		CString strTempPath;
		strTempPath = strPath;
		strTempPath += L"\\";
		strTempPath += strFileName;

		ScanFileBasedPathAndName(L"Ku6",pTF, strTempPath, L"Block*.k6p", nStopIndex);


	} while(FindNextFile(hFile, &wfd));

	bRet = TRUE;
clean0:
	if (hFile != INVALID_HANDLE_VALUE)
	{
		FindClose(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}

	return bRet;
}

CString GetDrive(CString suffix)  //根据路径．获取只读区域卷标名称
{
	int nPos = 0;
	CString strDrive = "?:";
	DWORD dwDriveList = ::GetLogicalDrives();
	CString strTmp;

	while (dwDriveList)
	{
		if (dwDriveList & 1)
		{	
			strDrive.SetAt (0, 'A' + nPos);
			///判断是移动磁盘
			{
				int nDiskType = GetDriveType(strDrive);   
				switch(nDiskType)
				{   
				case  DRIVE_NO_ROOT_DIR:
					break;
				case  DRIVE_REMOVABLE: 
					break;
				case  DRIVE_FIXED: 
					{
						strTmp += strDrive;
						strTmp += suffix;
						if(::PathIsDirectory(strTmp.GetBuffer()))
							return strTmp;
						strTmp = _T("");

					}
					break;
				case   DRIVE_REMOTE:
					break;   
				case   DRIVE_CDROM:
					break;
				}
			}
		}
		dwDriveList >>= 1;
		nPos++;
	}
	return strTmp;
}

BOOL _ScanPPTVCache(ITraverseFile *pTF, CString& strPath, int nStopIndex)
{
	BOOL bRet = FALSE;
	CString strResult;
	WCHAR szBuffer[MAX_PATH] = {0};
	CString strTmp;
	CString strName;
	CString strFullPath;

	::SHGetSpecialFolderPath( NULL, szBuffer, CSIDL_COMMON_APPDATA, FALSE);
	strTmp = szBuffer;
	strTmp += L"\\PPLive\\Core\\config.ini";
	IniEditor IniEdit;
	IniEdit.SetFile(strTmp.GetBuffer());
	std::wstring tmpPath = IniEdit.ReadString(L"peer",L"cachepath");
	std::string utf8str = KUTF16_To_ANSI(tmpPath.c_str());
	tmpPath = Utf8ToUnicode(utf8str.c_str());
	if(tmpPath.length() == 0)
	{
		bRet = FALSE;
		goto clean0;
	} 
	else
	{
		strResult = tmpPath.c_str();
	}
    strPath = strResult;
	strName = L"??????????????_*.mp4";
	ScanFileBasedPathAndName(L"PPLive",pTF, strResult, strName, nStopIndex);
	strName = L"*[*].mp4.tpp";
	ScanFileBasedPathAndName(L"PPLive",pTF, strResult, strName, nStopIndex);
	strName = L"*[*](*).mp4.tpp";
	ScanFileBasedPathAndName(L"PPLive",pTF, strResult, strName, nStopIndex);
	strName = L"*[*].mp4";
	ScanFileBasedPathAndName(L"PPLive",pTF, strResult, strName, nStopIndex);
	strName = L"*[*](*).mp4";
	ScanFileBasedPathAndName(L"PPLive",pTF, strResult, strName, nStopIndex);
	
	strFullPath = strResult;
	strFullPath += L"\\InvisibleFolder";

	strName = L"??????????????_*.swf";
	ScanFileBasedPathAndName(L"PPLive",pTF, strFullPath, strName, nStopIndex);

	strName = L"??????????????_*.jpg";
	ScanFileBasedPathAndName(L"PPLive",pTF, strFullPath, strName, nStopIndex);

	strName = L"??????????????_*.wmv";
	ScanFileBasedPathAndName(L"PPLive",pTF, strFullPath, strName, nStopIndex);

	strName = L"??????????????_*.gif";
	ScanFileBasedPathAndName(L"PPLive",pTF, strFullPath, strName, nStopIndex);

	strName = L"_db_allinonetoday*.zip";
	ScanFileBasedPathAndName(L"PPLive",pTF, strFullPath, strName, nStopIndex);

	strName = L"_db_big*.zip";
	ScanFileBasedPathAndName(L"PPLive",pTF, strFullPath, strName, nStopIndex);

	strName = L"_db_small*.zip";
	ScanFileBasedPathAndName(L"PPLive",pTF, strFullPath, strName, nStopIndex);

	strName = L"_db_small*.zip.ttp";
	ScanFileBasedPathAndName(L"PPLive",pTF, strFullPath, strName, nStopIndex);

	strName = L"vip_db_allinonetoday*.zip";
	ScanFileBasedPathAndName(L"PPLive",pTF, strFullPath, strName, nStopIndex);

	strName = L"vip_db_big*.zip";
	ScanFileBasedPathAndName(L"PPLive",pTF, strFullPath, strName, nStopIndex);

	strName = L"vip_db_small*.zip";
	ScanFileBasedPathAndName(L"PPLive",pTF, strFullPath, strName, nStopIndex);

clean0:
	return bRet;
}

BOOL _ScanFengxingCache(ITraverseFile *pTF, CString& strPath, int nStopIndex)
{
	BOOL bRet = FALSE;
	WCHAR* szBuffer;
	CString strTmp;
	CString strResult;
	//::SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_APPDATA, FALSE);
	szBuffer = _wgetenv(_T("USERPROFILE"));
	strTmp = szBuffer;
	/*int a = strTmp.ReverseFind(L'\\');
	strTmp = strTmp.Mid(0,strTmp.ReverseFind(L'\\'));*/
	strTmp += L"\\funshion.ini";
	IniEditor IniEdit;
	IniEdit.SetFile(strTmp.GetBuffer());
	std::wstring tmpPath = IniEdit.ReadString(L"FILE_PATH",L"MEDIA_PATH");
	if(tmpPath.length() == 0)
	{
		CString StrSuffix = _T("\\FunshionMedia");
		strResult = GetDrive(StrSuffix);
	}
	else
	{
		strResult = tmpPath.c_str();
	}

    strPath = strResult;
	bRet = ScanFengxingCacheEx(pTF, strResult, nStopIndex);


	return bRet;
}

BOOL ScanFengxingCacheEx(ITraverseFile *pTF, const CString strPath, int nStopIndex)
{
	BOOL bRet = FALSE;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA wfd = {0};

	CString strFullPath;
	CString strTemp;
	strFullPath = strPath;
	strFullPath += L"\\*.*";

	hFile = FindFirstFile(strFullPath, &wfd);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		bRet = FALSE;
		goto clean0;
	}
	do 
	{
		if ( g_bStop[nStopIndex] )
		{
			bRet = TRUE;
			goto clean0;
		}
		if (_wcsicmp(wfd.cFileName, L".") == 0 
			|| _wcsicmp(wfd.cFileName, L"..") == 0 
			|| !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			continue;
		}

		CString strFileName;
		strFileName = wfd.cFileName;
		//if (strFileName.GetLength() != 32)
		//{
		//	continue;
		//}
		CString strTempPath;
		strTempPath = strPath;
		strTempPath += L"\\";
		strTempPath += strFileName;

		ScanFileBasedPathAndName(L"FengXing",pTF, strTempPath, L"*.fc!", nStopIndex, TRUE, 48);


	} while(FindNextFile(hFile, &wfd));

	bRet = TRUE;
clean0:
	if (hFile != INVALID_HANDLE_VALUE)
	{
		FindClose(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}

	return bRet;
}

BOOL _ScanQQLiveCache(ITraverseFile *pTF, CString& strPath, int nStopIndex)
{
    BOOL bRet = FALSE;
	WCHAR szBuffer[MAX_PATH] = {0};
	CString strTmp;
	CString strResult;

	int nPos = -1;
	CString strTemp;
	CString strTempEx;

	::SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_APPDATA, FALSE);
	strTmp = szBuffer;
	strTmp += L"\\Tencent\\QQLive\\user.ini";
	IniEditor IniEdit;
	IniEdit.SetFile(strTmp.GetBuffer());
	std::wstring tmpPath = IniEdit.ReadString(L"Cache",L"Directory");
	if(tmpPath.length()>0)
	{
		tmpPath.erase(tmpPath.length()-1);
		strResult = tmpPath.c_str();
		//strResult += L"\\vodcache";
	}
	else
	{
		bRet = FALSE;
		goto clean0;
	}

    strPath = strResult;

	nPos = strResult.ReverseFind('\\');
	if (nPos != strResult.GetLength() - 1)
	{
		strResult += L"\\";
	}

    

	strTemp = strResult + L"vodcache\\";
	strTempEx = strTemp + L"advcache\\";

	bRet = ScanQQLiveCacheEx(pTF, strTemp, nStopIndex, TRUE, 3);
	bRet = ScanQQLiveCacheEx(pTF, strTempEx, nStopIndex);
	
	//bRet = TRUE;
clean0:

	return TRUE;
}

BOOL ScanQQLiveCacheEx(ITraverseFile *pTF, CString& strPath, int nStopIndex, BOOL bTime, DWORD dwTime)
{
	BOOL bRet = FALSE;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	CString strTemp;
	WIN32_FIND_DATA wfd = {0};
	strTemp = strPath;
	strTemp += L"*.*";
	hFile = FindFirstFile(strTemp, &wfd);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		bRet = FALSE;
		goto clean0;
	}
	do
	{
		if ( g_bStop[nStopIndex] )
		{
			bRet = TRUE;
			goto clean0;
		}

		CString strFileName;
		CString strTempResult;
		strFileName = wfd.cFileName;
		if (_wcsicmp(strFileName.GetBuffer(), L".") == 0 
			|| _wcsicmp(strFileName.GetBuffer(), L"..") == 0 
			|| !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			|| strFileName.GetLength() != 32)
		{
			continue;
		}
		strTempResult = strPath;
		strTempResult += strFileName;

		CString strFileTemp;
		strFileTemp = strFileName;
		strFileTemp += L".*.tdl";
		ScanFileBasedPathAndName(L"QQLive",pTF, strTempResult, strFileTemp, nStopIndex, bTime, dwTime);

		strFileTemp = strFileName;
		strFileTemp += L".tii";
		ScanFileBasedPathAndName(L"QQLive",pTF, strTempResult, strFileTemp, nStopIndex, bTime, dwTime);

		strFileTemp = strFileName;
		strFileTemp += L".tdi";
		ScanFileBasedPathAndName(L"QQLive",pTF, strTempResult, strFileTemp, nStopIndex, bTime, dwTime);

		ScanFileBasedPathAndName(L"QQLive",pTF, strTempResult,  L"*.tpt", nStopIndex, bTime, dwTime);
		//strPath = strTempEx;

	}while(FindNextFile(hFile, &wfd));

clean0:
	if (hFile != INVALID_HANDLE_VALUE)
	{
		FindClose(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}

	return bRet;
}

DWORD GetDiffDaysEx( SYSTEMTIME local,  FILETIME ftime )
{
	FILETIME flocal;
	time_t ttlocal;
	time_t ttfile;

	SystemTimeToFileTime( &local, &flocal );

	FileTimeToTime_tEx( flocal, &ttlocal );
	FileTimeToTime_tEx( ftime, &ttfile );

	time_t difftime = ttlocal - ttfile; 

	return difftime/3600L ;// 除以每天24小时3600秒
}

void FileTimeToTime_tEx( FILETIME ft, time_t *t )
{

	ULARGE_INTEGER ui;

	ui.LowPart = ft.dwLowDateTime;
	ui.HighPart = ft.dwHighDateTime;

	*t = ((ULONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
}

DWORD FileTimeInnerEx(FILETIME tCurFile)
{
	SYSTEMTIME stLocal;  
	GetSystemTime(&stLocal);  
	DWORD dwDiff = GetDiffDaysEx(stLocal,tCurFile);

	return dwDiff;

}

