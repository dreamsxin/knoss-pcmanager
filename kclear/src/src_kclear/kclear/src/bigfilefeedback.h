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

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "sysoptstatistics.h"
#include "miniutil/bkjson.h"

//////////////////////////////////////////////////////////////////////////

class CBigFileFeedback
{
public:
    typedef struct tagBigFileInfo {
        CString strFilePath;
        ULONG dwFileSizeLow;
        ULONG dwFileSizeHight;
    } BigFileInfo;

    void AddFile(const CString& strFilePath, ULONG dwFileSizeLow, ULONG dwFileSizeHight = 0)
    {
        BigFileInfo fileInfo;
        fileInfo.strFilePath = strFilePath;
        fileInfo.dwFileSizeLow = dwFileSizeLow;
        fileInfo.dwFileSizeHight = dwFileSizeHight;

        m_bigFiles.AddTail(fileInfo);
    }

    BOOL Report(LPCTSTR lpVer)
    {
        CStringA strEncodedList = GetString();
        if (!strEncodedList.GetLength())
            return FALSE;

        CSysOptStc::Instance().ReportKClearBigfiles(strEncodedList, lpVer);
        m_bigFiles.RemoveAll();
        return TRUE;
    }

protected:
    CStringA GetString()
    {
        BkJson::Document jDoc;
        BkJson::Handle jFileList = jDoc[L"files"];
        int i = 0;
        POSITION pos = NULL;
        CStringA strJson;
        int nSize;
        CAtlArray<CHAR> bufEncode;

        pos = m_bigFiles.GetHeadPosition();
        while (pos)
        {
            const BigFileInfo& _fileInfo = m_bigFiles.GetNext(pos);
            BkJson::Handle jFile = jFileList[i++];
            jFile[L"path"] = _fileInfo.strFilePath;
            jFile[L"size_l"] = _fileInfo.dwFileSizeLow;
            jFile[L"size_h"] = _fileInfo.dwFileSizeHight;
        }

        strJson = KUTF16_To_UTF8(jDoc.Dump());

        nSize = Base64EncodeGetRequiredLength(strJson.GetLength(), ATL_BASE64_FLAG_NOCRLF);
        bufEncode.SetCount(nSize + 1);
        RtlZeroMemory(bufEncode.GetData(), nSize + 1);
        Base64Encode(
            (const BYTE*)(LPCSTR)strJson, 
            strJson.GetLength(), 
            bufEncode.GetData(),
            &nSize,
            ATL_BASE64_FLAG_NOCRLF
            );

        return bufEncode.GetData();
    }

    CAtlList<BigFileInfo> m_bigFiles;
};

//////////////////////////////////////////////////////////////////////////

class CFsTypeFeedback
{
private:
    CFsTypeFeedback() {}
    ~CFsTypeFeedback() {}

public:
    static CFsTypeFeedback& Instance()
    {
        static CFsTypeFeedback _singleton;
        return _singleton;
    }

    BOOL Report(LPCTSTR lpVer)
    {
        BOOL retval = FALSE;
        CString strEncodedList;
        BOOL fRetCode;

        fRetCode = GetFsInfo();
        if (!fRetCode)
            goto clean0;

        strEncodedList = GetString();
        if (!strEncodedList.GetLength())
            goto clean0;

        CSysOptStc::Instance().ReportFileSystemList(strEncodedList, lpVer);
        m_fsInfs.RemoveAll();

        retval = TRUE;

clean0:
        return retval;
    }

private:
    typedef struct tagFsInfo {
        CString strDrv;
        CString strFsType;
        DWORD dwFreeSizeLow;
        DWORD dwFreeSizeHigh;
    } FsInfo;

    CString GetString()
    {
        BkJson::Document jDoc;
        BkJson::Handle jFileList = jDoc[L"vols"];
        int i = 0;
        POSITION pos = NULL;
        CStringA strJson;
        int nSize;
        CAtlArray<CHAR> bufEncode;

        pos = m_fsInfs.GetHeadPosition();
        while (pos)
        {
            const FsInfo& _fsInfo = m_fsInfs.GetNext(pos);
            BkJson::Handle jFile = jFileList[i++];
            jFile[L"drv"] = _fsInfo.strDrv;
            jFile[L"fs"] = _fsInfo.strFsType;
            jFile[L"free_l"] = _fsInfo.dwFreeSizeLow;
            jFile[L"free_h"] = _fsInfo.dwFreeSizeHigh;
        }

        strJson = KUTF16_To_UTF8(jDoc.Dump());

        nSize = Base64EncodeGetRequiredLength(strJson.GetLength(), ATL_BASE64_FLAG_NOCRLF);
        bufEncode.SetCount(nSize + 1);
        RtlZeroMemory(bufEncode.GetData(), nSize + 1);
        Base64Encode(
            (const BYTE*)(LPCSTR)strJson, 
            strJson.GetLength(), 
            bufEncode.GetData(),
            &nSize,
            ATL_BASE64_FLAG_NOCRLF
            );

        return KUTF8_To_UTF16(bufEncode.GetData());
    }

    BOOL GetFsInfo()
    {
        BOOL retval = FALSE;
        CAtlArray<TCHAR> buffer;
        TCHAR* pBuffer = NULL;
        DWORD dwSize;
        CAtlList<CString> logicalDrvs;
        CString strDrv;
        POSITION pos = NULL;
        DWORD dwSectorsPerCluster;
        DWORD dwBytesPerSector;
        DWORD dwNumberOfFreeClusters;
        DWORD dwTotalNumberOfClusters;
        TCHAR szVolName[MAX_PATH+1] = { 0 };
        TCHAR szFileSystem[MAX_PATH+1] = { 0 };
        BOOL fRetCode;

        SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

        m_fsInfs.RemoveAll();

        buffer.SetCount(512);
        pBuffer = buffer.GetData();
        dwSize = (DWORD)buffer.GetCount();
        memset(pBuffer, 0, dwSize * sizeof(TCHAR));
        dwSize = GetLogicalDriveStrings(dwSize, buffer.GetData());

        if (dwSize > 2) 
        {
            strDrv = pBuffer;
            logicalDrvs.AddTail(strDrv);

            for (DWORD i = 3; i < dwSize; ++i) 
            {
                if (pBuffer[i] != 0 && pBuffer[i - 1] == 0) 
                {
                    strDrv = pBuffer + i;
                    logicalDrvs.AddTail(strDrv);
                }
            }
        }

        pos = logicalDrvs.GetHeadPosition();
        while (pos)
        {
            POSITION current = pos;
            CString _drv = logicalDrvs.GetNext(pos);
            _drv.MakeLower();
            if (_drv == _T("a:\\") || _drv == _T("b:\\"))
            {
                continue;
            }

            UINT uType = GetDriveType(_drv);
            if (uType != DRIVE_FIXED)
            {
                continue;
            }

            RtlZeroMemory(szVolName, sizeof(szVolName));
            RtlZeroMemory(szFileSystem, sizeof(szFileSystem));
            fRetCode = GetVolumeInformation(
                _drv,
                szVolName,
                MAX_PATH+1,
                NULL,
                NULL,
                NULL,
                szFileSystem,
                MAX_PATH+1
                );
            if (!fRetCode)
            {
                continue;
            }

            fRetCode = GetDiskFreeSpace(
                _drv,
                &dwSectorsPerCluster,
                &dwBytesPerSector,
                &dwNumberOfFreeClusters,
                &dwTotalNumberOfClusters
                );
            if (!fRetCode)
                continue;

            ULARGE_INTEGER qwCurrentFreeSize;
            qwCurrentFreeSize.QuadPart = (ULONGLONG)dwNumberOfFreeClusters * dwSectorsPerCluster * dwBytesPerSector;

            FsInfo fsInfo;
            fsInfo.strDrv = _drv;
            fsInfo.strFsType = szFileSystem;
            fsInfo.dwFreeSizeLow = qwCurrentFreeSize.LowPart;
            fsInfo.dwFreeSizeHigh = qwCurrentFreeSize.HighPart;
            m_fsInfs.AddTail(fsInfo);
        }

        if (m_fsInfs.IsEmpty())
            goto clean0;

        retval = TRUE;

clean0:
        return retval;
    }

    CAtlList<FsInfo> m_fsInfs;
};
