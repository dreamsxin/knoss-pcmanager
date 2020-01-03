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

// FindFileTraverse.cpp: implementation of the CFindFileTraverse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string>
#include <map>
#include <shlwapi.h>
#include "trashcleaner.h"
#include "FindFileTraverse.h"
#include "TraverseFile.h"
#include "trashpublicfunc.h"
#include "filtertool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFindFileTraverse::CFindFileTraverse() :
    m_piTraverseFile(NULL)
{
        g_bStop[0]  = FALSE;
        Is_Dlg_Init = FALSE;
        m_hThread   = NULL;
        m_dwThreadID = 0;
}

CFindFileTraverse::~CFindFileTraverse()
{
}

BOOL CFindFileTraverse::IsFinding() const
{
    return m_hThread != NULL;
}

BOOL CFindFileTraverse::StartFindFileTraverse( 
        const CConfigData& config,
        ITraverseFile* piTraverseFile,
        BOOL Is_Init)
{
        BOOL bResult = FALSE;

        if (IsFinding())
        {
            bResult = TRUE;
            goto Exit0;
        }

        while ( true )
        {       

            m_piTraverseFile = piTraverseFile;

            CConfigData::DirWorkArray::const_iterator iter = config.GetConfigData().begin();

            m_DirWorks.clear();
            while(iter != config.GetConfigData().end())
            {
                m_DirWorks.push_back(*iter);
                iter++;
            }

            if (m_DirWorks.size() <= 0)
                break;

            g_bStop[0]  = FALSE;

            Is_Dlg_Init = Is_Init;

            m_hThread = CreateThread(NULL, 
                0, 
                FindFileThread, 
                (LPVOID)this,
                NULL,
                &m_dwThreadID);

            if (m_hThread == NULL)       
                break;

            bResult = TRUE;
            break;
        }
Exit0:
        return bResult;
}


DWORD WINAPI CFindFileTraverse::FindFileThread(LPVOID lpVoid)
{

        CFindFileTraverse* pThis = (CFindFileTraverse*)lpVoid;
        pThis->RunFindFile();
		::CloseHandle(pThis->m_hThread);
        InterlockedExchange((LONG*)&pThis->m_hThread, NULL);
        InterlockedExchange((LONG*)&pThis->m_dwThreadID, 0);
        return 0;
}

void CFindFileTraverse::RunFindFile()
{
        CString strDirectory = L"";

        for (int i = 0; i < m_DirWorks.size() && !g_bStop[0]; i++)
        {

            DIRECTORYWORK& dirwork = m_DirWorks[i];

            if(dirwork.bIsLeaf)
            {
              

                for (int iIndex = 0;iIndex<dirwork.strDirectory.size();iIndex++)
                {
                    strDirectory = dirwork.strDirectory.at(iIndex);

                    if(dirwork.bDefaultSelect && !Is_Dlg_Init)
                    {
                        if (m_piTraverseFile && !g_bStop[0])
                            m_piTraverseFile->TraverseProcess(dirwork.id);
                        
						if (dirwork.id >= VIDEO_TUDOU&&dirwork.id <= VIDEO_QQLIVE)
						{
							CString strPath;
							RunVideoCacheFile(dirwork.id, strPath);
                            

                            if(strPath.GetLength()!=0)
                            {
                                if ( strPath[ strPath.GetLength() - 1 ] != _T('\\') )
                                    strPath += _T('\\');
                            }
                           
							strDirectory = strPath;
						} 
						else
						{  
                            if (strDirectory.GetLength() <= 0 && dirwork.id != WIN_HUISHOUZHAN)
                            {
                                goto Exit0;
                                
                            }

                            BOOL bFilter = FALSE;

                            for(int ivIndex=0;ivIndex<dirwork.strFilt.size();ivIndex++)
                            {

                                if(CFilterTool::Instance()._DoFindFuncMap(dirwork.strFilt.at(ivIndex),strDirectory))
                                {
                                    bFilter = TRUE;
                                    break;
                                }
                            }

                            if(bFilter)
                            {
                                goto Exit0;
                                
                            }

                            if  (GetProcessIdFromName(L"chrome.exe")>=4&&dirwork.id==BROWER_CHROME)
                            {
                                goto Exit0;
                               
                            }
							if(GetProcessIdFromName(L"maxthon.exe")>=4&&(dirwork.id==BROWER_MATHRON))
							{
								goto Exit0;
							}
							if(GetProcessIdFromName(L"opera.exe")>=4&&dirwork.id==BROWER_OPERA)
							{
								goto Exit0;
							}
							if(GetProcessIdFromName(L"firefox.exe")>=4&&dirwork.id==BROWER_FIREFOX)
							{
								goto Exit0;
							}
							if(GetProcessIdFromName(L"sogouexplorer.exe")>=4&&dirwork.id==BROWER_SOGO)
							{
								goto Exit0;
							}
							if((GetProcessIdFromName(L"iexplore.exe")>=4 ||
								GetProcessIdFromName(L"360se.exe")>=4 ||
								GetProcessIdFromName(L"TTraveler.exe")>=4 ||
								GetProcessIdFromName(L"TheWorld.exe")>=4 ||
								GetProcessIdFromName(L"maxthon.exe")>=4 ||
								GetProcessIdFromName(L"sogouexplorer.exe")>=4) &&dirwork.id==BROWER_IE)
							{
								goto Exit0;
							}
							if (dirwork.id != WIN_HUISHOUZHAN)
							{
								if ( strDirectory[ strDirectory.GetLength() - 1 ] != _T('\\') )
									strDirectory += _T('\\');
							}

							FindFileInDirectory(strDirectory, 
								dirwork.strExtArray, dirwork.bRecursive ? 15 : 0,
								0,
								m_piTraverseFile,
								NULL,
								dirwork.id,
								dirwork.strFilt,
								dirwork.strFriendName);
						}
                    
                    }
                   
                }
Exit0:
				if (m_piTraverseFile&&!Is_Dlg_Init&&dirwork.bDefaultSelect)
					m_piTraverseFile->TraverseProcessEnd(dirwork.id,strDirectory);

                

            }
           
        }


        if ( m_piTraverseFile)
            m_piTraverseFile->TraverseFinished();


}

BOOL CFindFileTraverse::StopFileTraverse()
{
    g_bStop[0] = TRUE;
    return TRUE;
}


void CFindFileTraverse::RunVideoCacheFile(int id, CString& strPath)
{
	BOOL bRet = FALSE;
	switch(id)
	{
	case VIDEO_TUDOU:
		bRet = _ScanTudouCache(m_piTraverseFile, strPath, 0);
		break;
	case VIDEO_XUNLEI:
		bRet = _ScanKankanCache(m_piTraverseFile, strPath, 0);
		break;
	case VIDEO_YOUKU:
		bRet = _ScanYoukuCache(m_piTraverseFile, strPath, 0);
		break;
	case VIDEO_KU6:
		bRet = _ScanKu6Cache(m_piTraverseFile, strPath, 0);
		break;
	case VIDEO_PPTV:
		bRet = _ScanPPTVCache(m_piTraverseFile, strPath, 0);
		break;
	case VIDEO_FENGXING:
		bRet = _ScanFengxingCache(m_piTraverseFile, strPath, 0);
		break;
	case VIDEO_QQLIVE:
		bRet = _ScanQQLiveCache(m_piTraverseFile, strPath, 0);
		break;
	default:
		break;
	}
}