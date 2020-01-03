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
#include "trashpublicfunc.h"
#include <Shlwapi.h>
#include "filtertool.h"
#include "linkchecker.h"
#include "stubbornfiles.h"
#include "misc/KppUserLog.h"
#include "getenvpath.h"
#include <intshcut.h>
#include <algorithm>

using namespace std;

void SplitString(CString str, WCHAR split, std::vector <CString>& vcResult)
{
    int iPos = 0; //分割符位置
    int iNums = 0; //分割符的总数
    CString strTemp = str;
    CString strRight;
    //先计算子字符串的数量

    CString strLeft;
    while (true)
    {
        iPos = strTemp.Find(split);
        if(iPos == -1)
            break;
        //左子串
        strLeft = strTemp.Left(iPos);
        //右子串
        strRight = strTemp.Mid(iPos + 1, strTemp.GetLength());
        strTemp = strRight;
        vcResult.push_back(strLeft);
    }
}


int KAEGetSystemVersion(SYSTEM_VERSION *pnSystemVersion)
{
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	// If that fails, try using the OSVERSIONINFO structure.

	*pnSystemVersion = enumSystem_Unkonw;

	memset(&osvi, 0, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			return FALSE;
	}

	switch (osvi.dwPlatformId)
	{
		// Test for the Windows NT product family.
	case VER_PLATFORM_WIN32_NT:

		// Test for the specific product.
		if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 )
			*pnSystemVersion = enumSystem_Win_Vista;

		// Test for the specific product.
		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
			*pnSystemVersion = enumSystem_Win_Ser_2003;

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
			*pnSystemVersion = enumSystem_Win_xp;

		if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
			*pnSystemVersion = enumSystem_Win_2000;

		if ( osvi.dwMajorVersion <= 4 )
			*pnSystemVersion = enumSystem_Win_nt;
        if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1 )
            *pnSystemVersion = enumSystem_Win_7;

		// Test for the Windows Me/98/95.
	case VER_PLATFORM_WIN32_WINDOWS:

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			
			*pnSystemVersion = enumSystem_Win_95;
		} 

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
		{
			
			*pnSystemVersion = enumSystem_Win_98;
		} 

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
		{
			*pnSystemVersion = enumSystem_Win_me;
		} 
		break;

	default:
		*pnSystemVersion = enumSystem_Unkonw;
		break;
	}
	return TRUE; 
}


CString KasLangFormatFileName( LPCTSTR pszName )
{
    CString strConfigName;

    strConfigName = _T("cfg\\trashconfig.dat");
    return strConfigName;
}



CString KasLangGetTrashConfigFilePath()
{
    TCHAR szDir[MAX_PATH] = {0};
    int nRet = ::GetModuleFileName( HMODULE_THIS, szDir, MAX_PATH );
    if( nRet == 0 )
        return TEXT( "" );

    ::PathRemoveFileSpec( szDir );
    ::PathAddBackslash( szDir );


    CString strFile = KasLangFormatFileName( FILENAME_TRASHCONFIG );
    if( strFile.GetLength() == 0 )
        return TEXT( "" );


    return szDir + strFile;
}

LPCTSTR GetFileSizeText(__int64 fNum)
{
    static TCHAR szResult[60] = { 0 };

    if (fNum < 1000)   // Byte
    {
        _stprintf(szResult, TEXT("%dB"), fNum);
    }
    else if (fNum < 1024 * 1000)  // KB
    {
        double dfNum = (double)fNum / 1024;

        _stprintf(szResult, 
            dfNum == double(int(dfNum)) ? TEXT("%.2fKB") : TEXT("%.2fKB"), 
            dfNum);
    }
    else if (fNum < 1024 * 1024 * 1000) // MB
    {
        double dfNum = (double)fNum / (1024 * 1024);

        _stprintf(szResult, 
            dfNum == double(int(dfNum)) ? TEXT("%.fMB") : TEXT("%.2fMB"), 
            dfNum);

    }
    else // GB
    {
        double dfNum = (double)fNum / (1024 * 1024 * 1024);

        _stprintf(szResult, 
            dfNum == double(int(dfNum)) ? TEXT("%.fGB") : TEXT("%.2fGB"), 
            dfNum);
    }    

    if (szResult[0] == TCHAR("-"))
    {
        
    }
    return szResult;
}

CString GetExtName(CString fileName)
{
    int pos=fileName.Find(L"."); //获取 . 的位置
    if(pos==-1){ //如果没有找到，直接返回该字符串
        return fileName;
    }else{
        return GetExtName(fileName.Mid(pos+1)); //找到了的话，往深层遍历，直到最底层
    }
}

HANDLE KscOpenFile(const TCHAR cszFileName[], int nMode)
{
    static unsigned uModeToAccess[3] = 
    {
        GENERIC_READ,                   // Mode = 0 : Read Only | Open_Exist
        GENERIC_READ | GENERIC_WRITE,   // Mode = 1 : Read/Write | Open_Exist
        GENERIC_READ | GENERIC_WRITE    // Mode = 2 : Read/Write | Create_Always
    };
    static unsigned uModeToShareAccess[3] = 
    {
        FILE_SHARE_READ | FILE_SHARE_WRITE, // Mode = 0 : Read Only | Open_Exist
        0,                                  // Mode = 1 : Read/Write | Open_Exist
        0                                   // Mode = 2 : Read/Write | Create_Always
    };
    static unsigned uModeToCreate[3] = 
    {
        OPEN_EXISTING,  // Mode = 0 : Read Only | Open_Exist
        OPEN_EXISTING,  // Mode = 1 : Read/Write | Open_Exist
        CREATE_ALWAYS   // Mode = 2 : Read/Write | Create_Always
    };

    static unsigned uModeToAttribute[3] = 
    {
        FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE,
        FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE,
        FILE_ATTRIBUTE_NORMAL
    };

    return ::CreateFile(
        cszFileName,
        uModeToAccess[nMode],
        uModeToShareAccess[nMode],
        NULL,
        uModeToCreate[nMode],
        uModeToAttribute[nMode],
        NULL
        );
}

int KscCloseFile(HANDLE hFileHandle)
{
    return ::CloseHandle((HANDLE)hFileHandle) ? 0 : ::GetLastError();
}


BOOL FindFileInDirectory(LPCTSTR pszFullPath,
                         const std::vector<CString>& extArray, 
                         int nRecursive, 
                         int nStopIndex,
                         ITraverseFile *pTF,
                         CFileListData *pFilelist,
                         int nCurrentIndex,
                         std::vector<CString> &vFilt,
						 CString strScanItem)
{
    BOOL bResult = TRUE;
    WIN32_FIND_DATA ff = { 0 }; 

    if ( g_bStop[nStopIndex] )
    {
        bResult = TRUE;
        goto Exit0;
    }

	if (nCurrentIndex == WIN_HUISHOUZHAN)
	{
		SHQUERYRBINFO syscleInfo = {0};
		syscleInfo.cbSize = sizeof(SHQUERYRBINFO);
		CString str;
		str = L"双击查看详情";
		int iCount = SHQueryRecycleBin(NULL, &syscleInfo);
		FINDFILEDATA fileData;
		if (syscleInfo.i64NumItems == 0)
			goto Exit0;

		fileData.nCount = syscleInfo.i64NumItems;
		fileData.uFileSize = syscleInfo.i64Size;
		fileData.pszFileNmae = L"";
		fileData.pszPathName = str;
		fileData.pszExtName = L"";

		if (pTF != NULL)
		{
			pTF->TraverseFile(&fileData);
		}
		else if (pFilelist != NULL)
		{
			pFilelist->AddTrashFile(nCurrentIndex, &fileData);
		}
		goto Exit0;
	}

    if(CLinkChecker::Instance().IsDirectoryJunction(pszFullPath))
    {
        goto Exit0;
    }

    // 对本目录下的指定扩展名的文件作搜索
    for (int i = 0; i < extArray.size() && !g_bStop[nStopIndex]; i++)
    {
        bResult = FindFileWithExtInDirectory(pszFullPath, extArray[i],nStopIndex,pTF,pFilelist,nCurrentIndex,vFilt, strScanItem);
        if (!bResult)
            continue;

    }

     if ( nRecursive <= 0 || g_bStop[nStopIndex] )
         goto Exit0;

    // 递归搜索子目录
    TCHAR szFindName[FINDFILETRAVERSE_MAX_PATH] = {0};
    _tcsncpy_s( szFindName, FINDFILETRAVERSE_MAX_PATH, pszFullPath,  FINDFILETRAVERSE_MAX_PATH - 1 );
    _tcsncat_s( szFindName, FINDFILETRAVERSE_MAX_PATH, TEXT("*.*") , FINDFILETRAVERSE_MAX_PATH - _tcslen(pszFullPath) - 1 );
    HANDLE findhandle = ::FindFirstFile(szFindName, &ff); 

    if( findhandle == INVALID_HANDLE_VALUE )
    {
        bResult = FALSE;
        goto Exit0;
    }

    BOOL res = TRUE;

    while( res && !g_bStop[nStopIndex] )
    {
        if( ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )  
        {
            if (( _tcscmp( ff.cFileName, _T( "."  ) ) != 0) 
                && ( _tcscmp( ff.cFileName, _T( ".." ) ) != 0 ))
            {
                memset(szFindName, 0, sizeof(szFindName));
                _tcsncpy_s( szFindName, FINDFILETRAVERSE_MAX_PATH, pszFullPath,
                    FINDFILETRAVERSE_MAX_PATH - 1 );
                _tcsncat_s( szFindName, FINDFILETRAVERSE_MAX_PATH, ff.cFileName, 
                    FINDFILETRAVERSE_MAX_PATH - _tcslen(szFindName) - 1 );
                _tcsncat_s( szFindName, FINDFILETRAVERSE_MAX_PATH,TEXT("\\"),
                    FINDFILETRAVERSE_MAX_PATH - _tcslen(szFindName) - 1 );

                FindFileInDirectory( szFindName, extArray, nRecursive-1,nStopIndex,pTF,pFilelist,nCurrentIndex,vFilt, strScanItem);  
            }
        }

        res = ::FindNextFile( findhandle, &ff );
    }

    ::FindClose( findhandle );
Exit0:
    return bResult;       
}


BOOL FileFilter(LPCTSTR pszFileName)
{
    unsigned int uFilterCount = sizeof(g_pszFilterFiles) / sizeof(LPCTSTR);
    BOOL nRet = FALSE;
    for (int i = 0; i < uFilterCount; i++)
    {
        CString strFiltTmp(g_pszFilterFiles[i]);
        if (strFiltTmp.CompareNoCase(pszFileName) == 0)
        {
            nRet = TRUE;
            break;
        }
    }
    return nRet;
}

BOOL DirFilter(LPCTSTR pszDirName)
{
    unsigned int uFilterCount = sizeof(g_pszFilterDirs) / sizeof(LPCTSTR);
    BOOL bRet = FALSE;
    CString strDir(pszDirName);

    if (pszDirName == NULL)
        goto Exit0;

    if (strDir.GetLength()<=3)
    {
        bRet = TRUE;
        goto Exit0;
    }

    for (int i = 0; i < uFilterCount; i++)
    {
        if (strDir.CompareNoCase(g_pszFilterDirs[i]) == 0)
        {
            bRet = TRUE;
            break;
        }
    }
Exit0:
    return bRet;
}


// 查找指定扩展名的文件
BOOL FindFileWithExtInDirectory(LPCTSTR pszFullPath, 
                                LPCTSTR pszExt, 
                                int nStopIndex, 
                                ITraverseFile *pTF,
                                CFileListData *pFilelist,
                                int nCurrentIndex,
                                std::vector<CString> &vFilt,
								 CString strScanItem)
{
    BOOL				bResult = TRUE;
    TCHAR				szFindName[FINDFILETRAVERSE_MAX_PATH] = {0};
    WIN32_FIND_DATA		ff = { 0 };
    FINDFILEDATA		TraverseFileData = { 0 };
    HANDLE				findhandle = INVALID_HANDLE_VALUE;
    BOOL				res = TRUE;
    bool				bNeedCheckFilter = false;
    std::wstring		strExt;

	std::wstring        strExtEx;
	int nPos = -1;
	strExtEx = pszExt;

    if ( g_bStop[nStopIndex] )
        return FALSE;

    // 拼装查找路径
	nPos = strExtEx.find('.');
	if (nPos != -1)
	{
		_tcsncpy_s( szFindName, FINDFILETRAVERSE_MAX_PATH, pszFullPath,  
			FINDFILETRAVERSE_MAX_PATH - 1 );
		_tcsncat_s( szFindName, FINDFILETRAVERSE_MAX_PATH, pszExt, 
			FINDFILETRAVERSE_MAX_PATH - _tcslen(szFindName) - 1 );
	}
	else
	{
		_tcsncpy_s( szFindName, FINDFILETRAVERSE_MAX_PATH, pszFullPath,  
			FINDFILETRAVERSE_MAX_PATH - 1 );
		_tcsncat_s( szFindName, FINDFILETRAVERSE_MAX_PATH, _T("*."), 
			FINDFILETRAVERSE_MAX_PATH - _tcslen(szFindName) - 1 );
		_tcsncat_s( szFindName, FINDFILETRAVERSE_MAX_PATH, pszExt, 
			FINDFILETRAVERSE_MAX_PATH - _tcslen(szFindName) - 1 );
	}


    TraverseFileData.pszPathName = pszFullPath;
    findhandle = ::FindFirstFile( szFindName, &ff );


    if( findhandle == INVALID_HANDLE_VALUE )
    {
        return FALSE;
    }


    // 查找文件
    while( res && !g_bStop[nStopIndex] )
    {
        if( ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )  
        {
        }
        else
        {
            TraverseFileData.pszExtName  = pszExt;
            TraverseFileData.pszFileNmae = ff.cFileName;

//             if (nCurrentIndex==BROWER_IE)
//             {
//                 if(!IsFileTimeInner(ff.ftLastAccessTime))
//                     goto Clean0;
// 
//             }

//             if (nCurrentIndex==0&&pFilelist!=NULL)
//             {
//                 if(!IsFileTimeInner(ff.ftLastAccessTime))
//                     goto Clean0;
//             }
            
            CString strFileFullName(TraverseFileData.pszPathName);
            strFileFullName += TraverseFileData.pszFileNmae;
            strFileFullName.MakeLower();
            if(CStubbornFiles::Instance().IsStubbornFile(strFileFullName))
                goto Clean0;

// 			if (nCurrentIndex==WIN_LNK ||nCurrentIndex==WIN_DESKTOPLNK)
// 			{
// 				BOOL bHaveRealPath = FALSE;
// 				FindLinkArgument(strFileFullName.GetBuffer(), 
// 					bHaveRealPath);
// 				if(bHaveRealPath)
// 				{
// 					goto Clean0;
// 				}
// 				else
// 				{
// 					FindLinkUrl(strFileFullName.GetBuffer(), 
// 						bHaveRealPath);
// 					if(bHaveRealPath)
// 						goto Clean0;
// 				}
// 			}
            
           
            for(int ivIndex=0;ivIndex<vFilt.size();ivIndex++)
            {
              
                if(CFilterTool::Instance()._DoFindFuncMap(vFilt.at(ivIndex),strFileFullName))
                    goto Clean0;
            }  

			if (!IsExtCompare(ff.cFileName, pszExt))
			{
				goto Clean0;
			}
			if (wcsicmp(ff.cFileName, L"suggestedsites.dat") == 0 && nCurrentIndex == BROWER_IE)
			{
				goto Clean0;
			}
            TraverseFileData.uFileSize   = ff.nFileSizeHigh;
            TraverseFileData.uFileSize   = TraverseFileData.uFileSize << 32;
            TraverseFileData.uFileSize  |= ff.nFileSizeLow;

			CString strLog = L"[Scan][";
			strLog += strScanItem;
			strLog += "]:";
			strLog += strFileFullName;
			KppUserLog::UserLogInstance().WriteSingleLog(strLog.GetBuffer());
            if (pTF != NULL)
                pTF->TraverseFile(&TraverseFileData);
            else if (pFilelist != NULL)
                pFilelist->AddTrashFile(nCurrentIndex, &TraverseFileData);
        }
Clean0:
        if(  g_bStop[nStopIndex] )
            break;

        res = ::FindNextFile( findhandle, &ff );
    }

    ::FindClose( findhandle );
    return bResult;       
}


 BOOL DeleteFileWithExtInDirectory(LPCTSTR pszFullPath, 
                                   LPCTSTR pszExt,
                                   int nStopIndex,
                                   CFileListData *pFilelist,
                                   int nCurIndex,
                                   ICleanCallback *pCcb,
                                   CFileDelete *pDelFile,
                                   std::wstring strDesc,
                                   std::vector<CString> &vFilt)
 {
     BOOL				bResult = TRUE;
     TCHAR				szFindName[FINDFILETRAVERSE_MAX_PATH] = {0};
     WIN32_FIND_DATA		ff = { 0 };
     FINDFILEDATA		TraverseFileData = { 0 };
     HANDLE				findhandle = INVALID_HANDLE_VALUE;
     BOOL				res = TRUE;
     bool				bNeedCheckFilter = false;
     std::wstring		strExt;
 
     if (g_bStop[nStopIndex])
     {
         bResult = FALSE;
         goto Exit0;
     }

     if(pFilelist == NULL||nCurIndex < 0||pCcb == NULL)
     {
         bResult = FALSE;
         goto Exit0;
     }
 
     _tcsncpy_s( szFindName, FINDFILETRAVERSE_MAX_PATH, pszFullPath,  
         FINDFILETRAVERSE_MAX_PATH - 1 );
     _tcsncat_s( szFindName, FINDFILETRAVERSE_MAX_PATH, _T("*."), 
         FINDFILETRAVERSE_MAX_PATH - _tcslen(szFindName) - 1 );
     _tcsncat_s( szFindName, FINDFILETRAVERSE_MAX_PATH, pszExt, 
         FINDFILETRAVERSE_MAX_PATH - _tcslen(szFindName) - 1 );
 
     TraverseFileData.pszPathName = pszFullPath;
     findhandle = ::FindFirstFile( szFindName, &ff );
 
 
     if( findhandle == INVALID_HANDLE_VALUE )
     {
         return FALSE;
     }
 
 
     // 查找文件
     while (res&&!g_bStop[nStopIndex])
     {
         if( ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )  
         {
         }
         else
         {
             TraverseFileData.pszExtName  = pszExt;
             TraverseFileData.pszFileNmae = ff.cFileName;
 
             TraverseFileData.uFileSize   = ff.nFileSizeHigh;
             TraverseFileData.uFileSize   = TraverseFileData.uFileSize << 32;
             TraverseFileData.uFileSize  |= ff.nFileSizeLow;

             if (nCurIndex==0)
             {
                 if(!IsFileTimeInner(ff.ftLastAccessTime))
                     goto Clean0;
             }
 
 
			 DWORD curSize = pFilelist->AddTrashFile(nCurIndex, &TraverseFileData);
			 CString filepathname(TraverseFileData.pszPathName);
			 filepathname += TraverseFileData.pszFileNmae;
			 filepathname.MakeLower();

			 if(CStubbornFiles::Instance().IsStubbornFile(filepathname))
				 goto Clean0;

			 for(int ivIndex=0;ivIndex<vFilt.size();ivIndex++)
			 {
				 if(CFilterTool::Instance()._DoFindFuncMap(vFilt.at(ivIndex),filepathname))
					 goto Clean0;
			 }

			 pCcb->OnCleanEntry((int)strDesc.size(),strDesc.c_str(),
				 wcslen(filepathname.GetBuffer()),
				 filepathname.GetBuffer(),
				 (double)curSize / (1024 * 1024));

			 if(Delete_File_Success == pDelFile->TrashFileDelete(filepathname.GetBuffer()))
			 {
			 }
			 else
			 {
				 CStubbornFiles::Instance().AddFile(filepathname);
			 }
		 }
Clean0:
		 if(  g_bStop[nStopIndex] )
			 break;

		 res = ::FindNextFile( findhandle, &ff );
	 }

	 ::FindClose( findhandle );
	 CStubbornFiles::Instance().Sync();
Exit0:
     return bResult;       
 }

 BOOL DeleteFileInDirectory(LPCTSTR pszFullPath,
                            const std::vector<CString>& extArray, 
                            int nRecursive,
                            int nStopIndex,
                            CFileListData *pFilelist,
                            int nCurIndex,
                            ICleanCallback *pCcb,
                            CFileDelete *pDelFile,
                            std::wstring strDesc,
                            std::vector<CString> &vFilt)
 {
     BOOL bResult = FALSE;
     WIN32_FIND_DATA ff = { 0 }; 

     if (g_bStop[nStopIndex])
     {
         bResult = TRUE;
         goto Exit0;
     }

     for (int i = 0; i < extArray.size() && !g_bStop[nStopIndex]; i++)
     {
         bResult = DeleteFileWithExtInDirectory(pszFullPath, 
                                         extArray[i],
                                         nStopIndex,
                                         pFilelist,
                                         nCurIndex,
                                         pCcb,
                                         pDelFile,
                                         strDesc,
                                         vFilt);
         if (!bResult)
             continue;
     }

     if (nRecursive <= 0||g_bStop[nStopIndex])
         goto Exit0;

     if(CLinkChecker::Instance().IsDirectoryJunction(pszFullPath))
     {
         goto Exit0;
     }

     // 递归搜索子目录
     TCHAR szFindName[FINDFILETRAVERSE_MAX_PATH] = {0};
     _tcsncpy_s( szFindName, FINDFILETRAVERSE_MAX_PATH, pszFullPath,  FINDFILETRAVERSE_MAX_PATH - 1 );
     _tcsncat_s( szFindName, FINDFILETRAVERSE_MAX_PATH, TEXT("*.*") , FINDFILETRAVERSE_MAX_PATH - _tcslen(pszFullPath) - 1 );
     HANDLE findhandle = ::FindFirstFile( szFindName, &ff ); 

     if (findhandle == INVALID_HANDLE_VALUE)
     {
         bResult = FALSE;
         goto Exit0;
     }

     BOOL res = TRUE;

     while (res&&!g_bStop[nStopIndex])
     {
         if( ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )  
         {
             if (( _tcscmp( ff.cFileName, _T( "."  ) ) != 0) 
                 && ( _tcscmp( ff.cFileName, _T( ".." ) ) != 0 ))
             {
                 memset(szFindName, 0, sizeof(szFindName));
                 _tcsncpy_s( szFindName, FINDFILETRAVERSE_MAX_PATH, pszFullPath,
                     FINDFILETRAVERSE_MAX_PATH - 1 );
                 _tcsncat_s( szFindName, FINDFILETRAVERSE_MAX_PATH, ff.cFileName, 
                     FINDFILETRAVERSE_MAX_PATH - _tcslen(szFindName) - 1 );
                 _tcsncat_s( szFindName, FINDFILETRAVERSE_MAX_PATH,TEXT("\\"),
                     FINDFILETRAVERSE_MAX_PATH - _tcslen(szFindName) - 1 );

                 DeleteFileInDirectory(szFindName,
                     extArray, 
                     nRecursive - 1,
                     nStopIndex,
                     pFilelist,
                     nCurIndex,
                     pCcb,
                     pDelFile,
                     strDesc,
                     vFilt);
             }
         }

         res = ::FindNextFile( findhandle, &ff );
     }

     ::FindClose( findhandle );
Exit0:
     return bResult;       
 }

 void FileTimeToTime_t( FILETIME ft, time_t *t )
 {
   
     ULARGE_INTEGER ui;

     ui.LowPart = ft.dwLowDateTime;
     ui.HighPart = ft.dwHighDateTime;

     *t = ((ULONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
 }

 DWORD GetDiffDays( SYSTEMTIME local,  FILETIME ftime )
 {
     FILETIME flocal;
     time_t ttlocal;
     time_t ttfile;

     SystemTimeToFileTime( &local, &flocal );

     FileTimeToTime_t( flocal, &ttlocal );
     FileTimeToTime_t( ftime, &ttfile );

     time_t difftime = ttlocal - ttfile;

     return difftime/(24*3600L) ;// 除以每天24小时3600秒
 }


 BOOL IsFileTimeInner(FILETIME tCurFile)
 {
    BOOL bRet = FALSE;
    SYSTEMTIME stLocal;   
    GetSystemTime(&stLocal);  
    DWORD dwDiff = GetDiffDays(stLocal,tCurFile);

    if(dwDiff>=2)
        bRet = TRUE;

    return bRet;

 }

 BOOL IsExtCompare(std::wstring strFileName, std::wstring strExt)
 {
	 BOOL bRet = FALSE;
	 int nSize = 0;
	 int nFileSize = 0;
	 std::wstring strSubName;
	 nSize = strExt.size();
	 nFileSize = strFileName.size();
	 if (strExt.compare(L"*") == 0)
	 {
		 bRet = TRUE;
		 goto clean0;
	 }
	 if (nFileSize < nSize)
	 {
		 goto clean0;
	 }
	strSubName = strFileName.substr(nFileSize-nSize, nSize);
	std::transform(strSubName.begin(), strSubName.end(), strSubName.begin(), ::tolower);
	std::transform(strExt.begin(), strExt.end(), strExt.begin(), ::tolower);
	if (strSubName.compare(strExt) == 0)
	{
		bRet = TRUE;
	}
	else
	{
		bRet = FALSE;
	}
clean0:
	return bRet;
 }
 DWORD GetProcessIdFromName(LPCTSTR szProcessName)
 {
     PROCESSENTRY32 pe;
     DWORD id = 0;
     HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
     pe.dwSize = sizeof(PROCESSENTRY32);
     if( !Process32First(hSnapshot,&pe) )
         return 0;
     while(1)
     {
         pe.dwSize = sizeof(PROCESSENTRY32);
         if( Process32Next(hSnapshot,&pe)==FALSE )
             break;
         if(wcsicmp(pe.szExeFile,szProcessName) == 0)
         {
             id = pe.th32ProcessID;
             break;
         }

     }
     CloseHandle(hSnapshot);
     return id;
 }


//  HRESULT FindLinkArgument(const WCHAR *pLnkFilePath, 
// 	 BOOL &bHave)
//  {
// 	 HRESULT hRetResult = E_FAIL;
// 	 bHave = FALSE;
// 	 IShellLink* pLink = NULL;
// 	 IPersistFile* pPersisFile = NULL;
// 	 WIN32_FIND_DATA wfd = {0};
// 	 WCHAR szShellLinkArgument[MAX_PATH] = {0};
// 	 std:: wstring outRealPath = L"";
// 
// 	 if (!pLnkFilePath || 0x00 == pLnkFilePath[0])
// 	 {
// 		 goto _abort;
// 	 }
// 
// 	 ::CoInitialize(NULL);
// 
// 	 if (SUCCEEDED(CoCreateInstance(
// 		 CLSID_ShellLink, 
// 		 NULL, 
// 		 CLSCTX_INPROC_SERVER, 
// 		 IID_IShellLink, 
// 		 (void **) &pLink)))
// 	 {
// 		 hRetResult = pLink->QueryInterface(IID_IPersistFile, (void **)&pPersisFile);
// 		 if (FAILED(hRetResult))
// 		 {
// 			 goto _abort;
// 		 }
// 
// 		 hRetResult = pPersisFile->Load(
// 			 pLnkFilePath, 
// 			 STGM_READ);
// 		 if (SUCCEEDED(hRetResult))
// 		 {  
// 			 hRetResult = pLink->Resolve(NULL, SLR_ANY_MATCH|SLR_NO_UI);
// 			 if (FAILED(hRetResult))
// 			 {
// 				 goto _abort;
// 			 }
// 
// 			 hRetResult = pLink->GetPath(szShellLinkArgument, MAX_PATH,(WIN32_FIND_DATA *)&wfd, 
// 				 SLGP_UNCPRIORITY);
// 			
// 			 if (wcslen(szShellLinkArgument)>0)
// 			 {
// 				 outRealPath = szShellLinkArgument;
// 				 if(::PathFileExists(outRealPath.c_str()))
// 					 bHave = TRUE;
// 				 hRetResult = S_OK;
// 			 }
// 			 else
// 			 {
// 				 pLink->GetWorkingDirectory(szShellLinkArgument,MAX_PATH);
// 				 if(::PathIsDirectory(szShellLinkArgument))
// 					bHave = TRUE;
// 				 hRetResult = S_OK;
//  				 				
// 			 }
// 			
// 		 }
// 		 else
// 		 {
// 			 hRetResult = E_FAIL;
// 		 }
// 	 }
// 
// _abort:
// 
// 	 if (pPersisFile)
// 	 {
// 		 pPersisFile->Release();
// 		 pPersisFile = NULL;
// 	 }
// 
// 	 if (pLink)
// 	 {
// 		 pLink->Release();
// 		 pLink = NULL;
// 	 }
// 
// 	 ::CoUninitialize();
// 
// 	 return hRetResult;
//  }
// 
// 
//  HRESULT FindLinkUrl(const WCHAR *pLnkFilePath, 
// 	 BOOL &bHave)
//  {
// 	 HRESULT hRetResult = E_FAIL;
// 	 bHave = FALSE;
// 	 IUniformResourceLocator * pLink = NULL;
// 	 IPersistFile* pPersisFile = NULL;
// 	 WCHAR szShellLinkArgument[MAX_PATH] = {0};
// 
// 	 if (!pLnkFilePath || 0x00 == pLnkFilePath[0])
// 	 {
// 		 goto _abort;
// 	 }
// 
// 	 ::CoInitialize(NULL);
// 
// 	 if (SUCCEEDED(CoCreateInstance(
// 		 CLSID_InternetShortcut	, 
// 		 NULL, 
// 		 CLSCTX_INPROC_SERVER, 
// 		 IID_IUniformResourceLocator, 
// 		 (void **) &pLink)))
// 	 {
// 		 hRetResult = pLink->QueryInterface(IID_IPersistFile, (void **)&pPersisFile);
// 		 if (FAILED(hRetResult))
// 		 {
// 			 goto _abort;
// 		 }
// 
// 		 hRetResult = pPersisFile->Load(
// 			 pLnkFilePath, 
// 			 STGM_READ);
// 		 if (SUCCEEDED(hRetResult))
// 		 {  
// 			 if (FAILED(hRetResult))
// 			 {
// 				 goto _abort;
// 			 }
// 
// 			 hRetResult = pLink->GetURL((LPWSTR*)&szShellLinkArgument);
// 
// 			 if (wcslen(szShellLinkArgument)>0)
// 			 {
// 				 bHave = TRUE;
// 				 hRetResult = S_OK;
// 			 }
// 			 else
// 			 {
// 
// 			 }
// 
// 		 }
// 		 else
// 		 {
// 			 hRetResult = E_FAIL;
// 		 }
// 	 }
// 
// _abort:
// 
// 	 if (pPersisFile)
// 	 {
// 		 pPersisFile->Release();
// 		 pPersisFile = NULL;
// 	 }
// 
// 	 if (pLink)
// 	 {
// 		 pLink->Release();
// 		 pLink = NULL;
// 	 }
// 
// 	 ::CoUninitialize();
// 
// 	 return hRetResult;
//  }
// 
