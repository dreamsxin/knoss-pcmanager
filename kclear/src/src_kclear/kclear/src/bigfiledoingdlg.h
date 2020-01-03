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

#include "kscbase/ksclock.h"

//////////////////////////////////////////////////////////////////////////

#define WM_BIGFILE_DOING_END        (WM_USER+150)
#define WM_BIGFILE_DOING_CANCELED   (WM_USER+151)
#define WM_BIGFILE_REPLACE_CONFIRM  (WM_USER+152)
#define WM_BIGFILE_SIZE_FAILED      (WM_USER+153)

//////////////////////////////////////////////////////////////////////////

#define KSC_FILE_MOVEFILE_SIZE         0x00010000     // 64K
#define KSC_MOVEFILE_SIZE_FAILED       10 * 1024 * 1024//10M

inline BOOL KscSizeOk(CString& _drv, ULONGLONG& llSize)
{
	BOOL bRet = FALSE;

	DWORD dwSectorsPerCluster;
	DWORD dwBytesPerSector;
	DWORD dwNumberOfFreeClusters;
	DWORD dwTotalNumberOfClusters;

	bRet = GetDiskFreeSpace(
		_drv,
		&dwSectorsPerCluster,
		&dwBytesPerSector,
		&dwNumberOfFreeClusters,
		&dwTotalNumberOfClusters
		);
	if (!bRet)
		goto clean0;
	ULARGE_INTEGER qwCurrentFreeSize;
	qwCurrentFreeSize.QuadPart = (ULONGLONG)dwNumberOfFreeClusters * dwSectorsPerCluster * dwBytesPerSector;

	ULONGLONG qwFreeSize;

	qwFreeSize = (ULONGLONG)qwCurrentFreeSize.HighPart << 32 ;
	qwFreeSize |= qwCurrentFreeSize.LowPart;

//	LONGLONG sTemp = 8791252990;

	if ((qwFreeSize - llSize) < KSC_MOVEFILE_SIZE_FAILED)
	{
		bRet = FALSE;
		goto clean0;
	}
	bRet = TRUE;
clean0:
	return bRet;
}

inline HANDLE KscOpenFile(const TCHAR cszFileName[], int nMode)
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

inline int KscReadFile(HANDLE hFileHandle, void *pvBuffer, int nLen)
{
	int nResult = -1;

	int nRetCode = false;

	nRetCode = ::ReadFile(
		(HANDLE)hFileHandle,
		pvBuffer,
		nLen,
		(unsigned long *)&nResult,
		NULL
		);
	if (!nRetCode)
		return -1;

	return nResult;
}

inline int KscWriteFile(HANDLE hFileHandle, void *pvBuffer, int nLen)
{
	int nResult = -1;

	int nRetCode = false;

	nRetCode = ::WriteFile(
		(HANDLE)hFileHandle,
		pvBuffer,
		nLen,
		(unsigned long *)&nResult,
		NULL
		);

	if (!nRetCode)
		return -1;

	return nResult;
}

inline int KscCloseFile(HANDLE hFileHandle)
{
	return ::CloseHandle((HANDLE)hFileHandle) ? 0 : ::GetLastError();
}

inline BOOL KscGetFileSize(HANDLE hFileHandle, DWORD& dwFileSizeLow, DWORD& dwFileSizeHigh)
{
    BOOL retval;
    LARGE_INTEGER llFileSize;
	retval = ::GetFileSizeEx(hFileHandle, &llFileSize);
    if (retval)
    {
        dwFileSizeLow = llFileSize.LowPart;
        dwFileSizeHigh = llFileSize.HighPart;
    }

    return retval;
}

inline BOOL CreateDirectory(const CString& strDir)
{
    BOOL retval = FALSE;
    CString strTemp = strDir;
    CString strBase;
    int nFind;

    if (GetFileAttributes(strDir) == FILE_ATTRIBUTE_DIRECTORY)
    {
        retval = TRUE;
        goto clean0;
    }

    if (strTemp[strTemp.GetLength() - 1] != _T('\\'))
        strTemp += _T("\\");

    nFind = strTemp.Find(_T("\\"));
    while (nFind != -1)
    {
        strBase += strTemp.Left(nFind + 1);
        strTemp.Delete(0, nFind + 1);

        if (GetFileAttributes(strBase) == INVALID_FILE_ATTRIBUTES)
        {
            if (!CreateDirectory(strBase, NULL))
                goto clean0;
        }

        nFind = strTemp.Find(_T("\\"));
    }

    retval = TRUE;

clean0:
    return retval;
}


class CDelJob
{
public:
	const CAtlList<CString>& GetJobs()
	{
		return m_jobs;
	}

	const CAtlList<CString>& GetSuccess()
	{
		return m_success;
	}

	const CAtlList<CString>& GetFailed()
	{
		return m_failed;
	}

	const CAtlList<CString>& GetFailedEx()
	{
		return m_failedEx;
	}

	const CAtlList<CString>& GetCanceled()
	{
		return m_canceled;
	}

	void AddJob(const CString& strDel)
	{
		KAutoLock lock(m_lock);
		m_jobs.AddTail(strDel);
	}

	void AddSuccess(const CString& strFile)
	{
		KAutoLock lock(m_lock);
		m_success.AddTail(strFile);
	}

	void AddFailed(const CString& strFile)
	{
		KAutoLock lock(m_lock);
		m_failed.AddTail(strFile);
	}

	void AddFailedEx(const CString& strFile)
	{
		KAutoLock lock(m_lock);
		m_failedEx.AddTail(strFile);
	}

	void AddCanceled(const CString& strFile)
	{
		KAutoLock lock(m_lock);
		m_canceled.AddTail(strFile);
	}

protected:
	CAtlList<CString> m_jobs;
	CAtlList<CString> m_success;
	CAtlList<CString> m_failed;
	CAtlList<CString> m_failedEx;
	CAtlList<CString> m_canceled;
	KLock m_lock;
};


class CMoveJob
{
public:
	typedef struct tagMoveTask {
		CString strDest;
		CString strSrc;
	} MoveTask;

	const CAtlList<MoveTask>& GetJobs()
	{
		return m_jobs;
	}

	const CAtlList<MoveTask>& GetCopyed()
	{
		return m_copyed;
	}

	const CAtlList<MoveTask>& GetSuccess()
	{
		return m_success;
	}

	const CAtlList<MoveTask>& GetFailed()
	{
		return m_failed;
	}

	const CAtlList<MoveTask>& GetFailedEx()
	{
		return m_failedEx;
	}

	const CAtlList<MoveTask>& GetCanceled()
	{
		return m_canceled;
	}

	void AddJob(const CString& strDest, const CString& strSrc)
	{
		KAutoLock lock(m_lock);
		MoveTask task;

		task.strDest = strDest;
		task.strSrc = strSrc;
		m_jobs.AddTail(task);
	}

	void AddCopyed(const MoveTask& task)
	{
		KAutoLock lock(m_lock);
		m_copyed.AddTail(task);
	}

	void AddSuccess(const MoveTask& task)
	{
		KAutoLock lock(m_lock);
		m_success.AddTail(task);
	}

	void AddFailed(const MoveTask& task)
	{
		KAutoLock lock(m_lock);
		m_failed.AddTail(task);
	}

	void AddFailedEx(const MoveTask& task)
	{
		KAutoLock lock(m_lock);
		m_failedEx.AddTail(task);
	}

	void AddCanceled(const MoveTask& task)
	{
		KAutoLock lock(m_lock);
		m_canceled.AddTail(task);
	}

protected:
	CAtlList<MoveTask> m_jobs;
	CAtlList<MoveTask> m_copyed;    // 中间状态(整体先拷贝，后删除)
	CAtlList<MoveTask> m_success;
	CAtlList<MoveTask> m_failed;
	CAtlList<MoveTask> m_failedEx;
	CAtlList<MoveTask> m_canceled;
	KLock m_lock;
};

//////////////////////////////////////////////////////////////////////////

class CBigFileDoingDlg 
    : public CKuiDialogImpl<CBigFileDoingDlg>
    , public CWHRoundRectFrameHelper<CBigFileDoingDlg>
{
public:
    CBigFileDoingDlg(const CString& strTitle, const CString& strSubTitle, CDelJob& job, BOOL bType)
        : CKuiDialogImpl<CBigFileDoingDlg>("IDR_DLG_DOING")
        , m_strTitle(strTitle)
        , m_strSubTitle(strSubTitle)
        , m_pDelJob(&job)
        , m_pMoveJob(NULL)
        , m_hWorkThread(NULL)
        , m_nPos(0)
        , m_fUseLastConfirm(FALSE)
        , m_nLastConfirm(-1)
		, m_nPercent(0)
		, m_llCurrentSize(0)
		, m_bType(bType)
    {
        m_hExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    }

    CBigFileDoingDlg(const CString& strTitle, const CString& strSubTitle, CMoveJob& job, ULONGLONG& llSize)
        : CKuiDialogImpl<CBigFileDoingDlg>("IDR_DLG_DOING")
        , m_strTitle(strTitle)
        , m_strSubTitle(strSubTitle)
        , m_pDelJob(NULL)
        , m_pMoveJob(&job)
        , m_hWorkThread(NULL)
        , m_nPos(0)
        , m_fUseLastConfirm(FALSE)
        , m_nLastConfirm(-1)
		, m_nPercent(0)
		, m_llSize(llSize)
		, m_llCurrentSize(0)
    {
        m_hExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    }

    ~CBigFileDoingDlg()
    {
        if (m_hExitEvent)
        {
            CloseHandle(m_hExitEvent);
            m_hExitEvent = NULL;
        }

        if (m_hWorkThread)
        {
            CloseHandle(m_hWorkThread);
            m_hWorkThread = NULL;
        }
	//	KillTimer(0);
	/*	KillTimer(1);*/
    }

    BOOL OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
    {
        SetItemText(100, m_strTitle);
        SetItemText(101, m_strSubTitle);

        if (m_pDelJob)
		{
			SetItemVisible(104,FALSE);
            SetItemVisible(60001, FALSE);
        }
		else
		{
			SetItemVisible(104,TRUE);
		}

		::SetTimer(m_hWnd, 0, 50, NULL);

        m_hWorkThread = (HANDLE)_beginthreadex(NULL, 0, ProcessJobThread, this, 0, NULL);

        return FALSE;
    }

    void Stop()
    {
        SetEvent(m_hExitEvent);
        WaitForSingleObject(m_hWorkThread, INFINITE);
    }

    void OnTimer(UINT_PTR nIDEvent)
    {
        KAutoLock lock(m_lock);
        TCHAR szFilePath[MAX_PATH] = { 0 };
        CString strFileName;
        CString strText1;
        CString strText2;
        CString pathTmp;

        if (m_pDelJob)
        {
            _tcscpy_s(szFilePath, MAX_PATH, m_strCurrentDel);
            TCHAR* pFlag = _tcsrchr(szFilePath, _T('\\'));
            if (pFlag)
            {
                strFileName = pFlag + 1;
                *pFlag = NULL;
            }

         
            CString strLastPath(szFilePath);
            pathTmp = szFilePath;
            if(strLastPath.GetLength()>30)
            {
                strLastPath = pathTmp.Left(5);
                strLastPath +=L"...";
                strLastPath += pathTmp.Mid(pathTmp.GetLength()-15);
            }

            strText1 = _T("文件名称：");
            strText1 += strFileName;
            strText2 = _T("所在路径：");
            strText2 += strLastPath;
        }
        else 
        {
            CString strLastSrcPath(m_strCurrentSrc.GetBuffer());
            CString strLastDestPath(m_strCurrentDest.GetBuffer());

            if (strLastSrcPath.GetLength()>30)
            {
                strLastSrcPath = m_strCurrentSrc.Left(5);
                strLastSrcPath +=L"...";
                strLastSrcPath += m_strCurrentSrc.Mid(m_strCurrentSrc.GetLength()-15);
            }

            if(strLastDestPath.GetLength()>30)
            {
                strLastDestPath = m_strCurrentDest.Left(5);
                strLastDestPath +=L"...";
                strLastDestPath += m_strCurrentDest.Mid(m_strCurrentDest.GetLength()-15);
            }


            strText1 = _T("原始路径：");
            strText1 += strLastSrcPath;
            strText2 = _T("目标路径：");
            strText2 += strLastDestPath;

        }

        // todo: 处理后在显示
        SetItemText(102, strText1);
        SetItemText(103, strText2);
		SetItemVisible(102, TRUE);
		SetItemVisible(103, TRUE);
		SetItemVisible(104, TRUE);

        SetItemIntAttribute(104, "value", m_nPos);
    }

    static void ProcessDelJobs(CBigFileDoingDlg* pThis)
    {
        if (!pThis)
            return;

        if (!pThis->m_pDelJob)
            return;

        CString strDel;
        const CAtlList<CString>&  jobs = pThis->m_pDelJob->GetJobs();
        POSITION pos = jobs.GetHeadPosition();
        int nCount = (int)jobs.GetCount();
        int nCurrent = 0;

        while (pos)
        {
            if (::WaitForSingleObject(pThis->m_hExitEvent, 0) == WAIT_OBJECT_0)
            {
                while (pos)
                {
                    strDel = jobs.GetNext(pos);
                    pThis->m_pDelJob->AddCanceled(strDel);
                }

                pThis->PostMessage(WM_BIGFILE_DOING_CANCELED);
                goto clean0;
            }

            strDel = jobs.GetNext(pos);

            {
                // 更新状态
                KAutoLock lock(pThis->m_lock);
                pThis->m_strCurrentDel = strDel;
                pThis->m_nPos = (nCurrent * 100) / nCount;
            }

			int nRet = pThis->Recycle(strDel, pThis->m_bType);

            if (/*::DeleteFile(strDel)*/nRet == 0)
            {
                pThis->m_pDelJob->AddSuccess(strDel);
            }
			else if (::GetFileAttributes(strDel) == INVALID_FILE_ATTRIBUTES)
			{
				CKuiMsgBox msg;
				CString str; 
				CString strName;
				int nIndex = strDel.ReverseFind(_T('\\'));
				strName = strDel.Right(strDel.GetLength() - nIndex - 1);
				str = L"\n源文件“";
				str += strName;
				str += L"”已不存在！";
				msg.ShowMsg(str, NULL, MB_OK | MB_ICONINFORMATION);

				pThis->m_pDelJob->AddFailedEx(strDel);
			}
            else
            {
                pThis->m_pDelJob->AddFailed(strDel);
            }

            ++nCount;
        }

        pThis->m_nPos = 100; 
        pThis->PostMessage(WM_BIGFILE_DOING_END);

clean0:
        return;
    }

    static void ProcessMoveJobs(CBigFileDoingDlg* pThis)
    {
        if (!pThis)
            return;

        if (!pThis->m_pMoveJob)
            return;

        const CAtlList<CMoveJob::MoveTask>&  jobs = pThis->m_pMoveJob->GetJobs();
        POSITION pos = jobs.GetHeadPosition();
        int nCount = (int)jobs.GetCount();
        int nCurrent = 0;

        while (pos)
        {
			if (::WaitForSingleObject(pThis->m_hExitEvent, 0) == WAIT_OBJECT_0)
            {
                while (pos)
                {
                    const CMoveJob::MoveTask& task = jobs.GetNext(pos);
                    pThis->m_pMoveJob->AddCanceled(task);
                }

                pThis->PostMessage(WM_BIGFILE_DOING_CANCELED);
                goto clean0;
            }

            const CMoveJob::MoveTask& task = jobs.GetNext(pos);

            DWORD fCopy = 1;
			CString str;
			CMoveJob::MoveTask task1;
			{
				KAutoLock lock(pThis->m_lock);
				pThis->m_strCurrentSrc = task.strSrc;
				pThis->m_strCurrentDest = task.strDest;
			}
            if (::GetFileAttributes(task.strDest) != INVALID_FILE_ATTRIBUTES)
            {
				pThis->SendMessage(WM_TIMER, 0, 0);

                LRESULT lRetCode = pThis->SendMessage(
                    WM_BIGFILE_REPLACE_CONFIRM, 
                    (WPARAM)(LPCTSTR)task.strDest, 
                    (LPARAM)(LPCTSTR)task.strSrc
                    );
                if (lRetCode == 0)
                {
                    fCopy = 0;
                    pThis->m_pMoveJob->AddCanceled(task);
                }
				else if (lRetCode == 2)
				{
					fCopy = 2;
					str = task.strDest;
					pThis->AutoChangeName(str);
					task1.strDest = str;
					task1.strSrc = task.strSrc;
				}
            }

            if (fCopy == 1)
            {
				DWORD dwRet = pThis->MyCopyFile(task.strSrc, task.strDest);
                if ( dwRet == 1)
                {
                    pThis->m_pMoveJob->AddCopyed(task);
					pThis->m_pMoveJob->AddCanceled(task);
                }
                else if ( dwRet == 2)
                {
					pThis->m_pMoveJob->AddCanceled(task);
					goto clean0;
                }
				else if (dwRet == 3)
				{
					pThis->PostMessage(WM_BIGFILE_SIZE_FAILED);
					goto clean0;
				}
				else if (dwRet == 4)
				{
					pThis->m_pMoveJob->AddFailedEx(task);
				}
				else
                {
                    pThis->m_pMoveJob->AddFailed(task);
                }
            }

			if (fCopy == 2)
			{
				DWORD dwRet = pThis->MyCopyFile(task1.strSrc, task1.strDest);
				if ( dwRet == 1)
				{
					pThis->m_pMoveJob->AddCopyed(task1);
					pThis->m_pMoveJob->AddCanceled(task1);
				}
				else if ( dwRet == 2)
				{
					pThis->m_pMoveJob->AddCanceled(task1);
					goto clean0;
				}
				else if (dwRet == 3)
				{
					pThis->PostMessage(WM_BIGFILE_SIZE_FAILED);
					goto clean0;
				}
				else if (dwRet == 4)
				{
					pThis->m_pMoveJob->AddFailedEx(task1);
				}
				else
				{
					pThis->m_pMoveJob->AddFailed(task1);
				}
			}

            ++nCurrent;
        }

        const CAtlList<CMoveJob::MoveTask>& copyed = pThis->m_pMoveJob->GetCopyed();
        pos = copyed.GetHeadPosition();
        while (pos)
        {
            const CMoveJob::MoveTask& task = copyed.GetNext(pos);
            if (::DeleteFile(task.strSrc))
            {
                pThis->m_pMoveJob->AddSuccess(task);
            }
            else
            {
                ::DeleteFile(task.strDest);
                pThis->m_pMoveJob->AddFailed(task);
            }
        }

        pThis->m_nPos = 100; 
        pThis->PostMessage(WM_BIGFILE_DOING_END);


clean0:
        return;
    }

    static UINT WINAPI ProcessJobThread(PVOID pSetting)
    {
        CBigFileDoingDlg* pThis = (CBigFileDoingDlg*)pSetting;

        if (pThis->m_pDelJob)
        {
            // 删除任务
            ProcessDelJobs(pThis);
        }
        else
        {
            // 迁移任务
            ProcessMoveJobs(pThis);
        }

        _endthreadex(0);
        return 0;
    }

    void OnSysBtnClose()
    {
        Stop();
    }

    LRESULT OnDoingEnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        EndDialog(IDOK);
        return 0L;
    }

    LRESULT OnDoingCanceled(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        EndDialog(IDCANCEL);
        return 0L;
    }

	LRESULT OnSizeFailed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		EndDialog(101);
		return 0L;
	}
    LRESULT OnReplaceConfirm(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if (m_fUseLastConfirm)
            return m_nLastConfirm;

        CKuiMsgBox msgBox;
        CString strFileName = (LPCTSTR)wParam;
        CString strMsg;
        BOOL fNoNotifyLater = TRUE;
        UINT_PTR uRetCode;

        int nFlag = strFileName.ReverseFind(_T('\\'));
        strFileName.Delete(0, nFlag + 1);

        strMsg = _T("目标目录中已存在名为“");
        strMsg += strFileName;
        strMsg += "”的文件，\n您希望如何处理本次迁移动作？";
        msgBox.AddButtonEx(_T("跳过"), 100, _T("-80,0,-0,-0"));
        msgBox.AddButtonEx(_T("替换"), 101, _T("-170,0,-90,-0"));
        msgBox.AddButtonEx(_T("自动重命名"), 102, _T("7,0,87,-0"));
        msgBox.SetNoNotifyText(_T("后续冲突同样处理，不再提示我"));
        uRetCode = msgBox.ShowMsg(strMsg, _T("是否替换已存在的文件"), MB_BK_CUSTOM_BUTTON|MB_ICONQUESTION, &fNoNotifyLater);
        if (fNoNotifyLater)
            m_fUseLastConfirm = TRUE;
        if (101 == uRetCode)
		{
            m_nLastConfirm = 1;
		}
        else if (102 == uRetCode)
        {
			m_nLastConfirm = 2;
        }
		else
		{
            m_nLastConfirm = 0;
		}

        return m_nLastConfirm;
    }

	DWORD MyCopyFile(const CString& strSrc, const CString& strDest)
	{
		DWORD retval = 0;
		int nIndex;
		CString strDestDir;
		HANDLE hFileSrc = INVALID_HANDLE_VALUE;
		HANDLE hFileDest = INVALID_HANDLE_VALUE;
		DWORD dwSizeHight;
		DWORD dwSizeLow;
		ULONGLONG qwSize;
		ULONGLONG qwCurrentPos = 0;
		int nPercent = 0;
		int nCurrentPercent = 0;
		int nPos = 0;
		BOOL bRet = FALSE;
		CString strDiskFree;
		BYTE* pBuffer = NULL;
		strDiskFree = strDest.Left(3);
		DWORD dwFileAttr = -1;
	
		if (!KscSizeOk(strDiskFree,  m_llSize))
		{
			CKuiMsgBox msg;
			msg.ShowMsg(L"对不起，目标磁盘剩余空间不足，无法迁移选中文件，请您重新选择磁盘", NULL, MB_OK|MB_ICONWARNING);
			retval = 3;
			goto clean0;
		}


		nIndex = strDest.ReverseFind(_T('\\'));
		if (-1 == nIndex)
			goto clean0;

		strDestDir = strDest.Left(nIndex);
		if (GetFileAttributes(strDestDir) == INVALID_FILE_ATTRIBUTES)
		{
		if (!CreateDirectory(strDestDir))
		goto clean0;
		}

		dwFileAttr = GetFileAttributes(strSrc);
		if (dwFileAttr == INVALID_FILE_ATTRIBUTES)
		{
			CKuiMsgBox msg;
			CString str; 
			CString strName;
			nIndex = strSrc.ReverseFind(_T('\\'));
			strName = strSrc.Right(strSrc.GetLength() - nIndex - 1);
			if(strName.GetLength() > 15)
			{
				CString strTemp;
				strTemp = strName;
				strName = strTemp.Left(6);
				strName += L"...";
				strName += strTemp.Right(7);
			}
			str = L"\n源文件“";
			str += strName;
			str += L"”已不存在！";
			msg.ShowMsg(str, NULL, MB_OK | MB_ICONINFORMATION);
			retval = 4;
			goto clean0;
		}

		hFileSrc = KscOpenFile(strSrc, 1);   // Open ReadWrite
		if (hFileSrc == INVALID_HANDLE_VALUE)
		{
			goto clean0;
		}

        if (!KscGetFileSize(hFileSrc, dwSizeLow, dwSizeHight))
        {
            goto clean0;
        }

		qwSize = (ULONGLONG)dwSizeHight << 32;
		qwSize |= dwSizeLow;

		hFileDest = KscOpenFile(strDest, 2); // Create Always
		if (hFileDest == INVALID_HANDLE_VALUE)
		{
			goto clean0;
		}

		pBuffer = new BYTE[KSC_FILE_MOVEFILE_SIZE];
		if (pBuffer == NULL)
		{
			goto clean0;
		}
		ZeroMemory(pBuffer, KSC_FILE_MOVEFILE_SIZE);
		//nPercent = qwSize / KSC_FILE_MOVEFILE_SIZE;
		while (qwCurrentPos < qwSize)
		{
			if (::WaitForSingleObject(m_hExitEvent, 0) == WAIT_OBJECT_0)
			{
				PostMessage(WM_BIGFILE_DOING_CANCELED);
				retval = 2;
				goto clean0;
			}
			{
				// 更新状态
				KAutoLock lock(m_lock);
				m_strCurrentSrc = strSrc;
				m_strCurrentDest = strDest;
				int nPos;
				nPos = int((m_llCurrentSize * 100) / m_llSize);
				if (nPos >= 100)
					nPos = 99;
			    m_nPos = nPos; 
			}
			int nReadByte = 0;
			nReadByte = KscReadFile(hFileSrc, pBuffer, KSC_FILE_MOVEFILE_SIZE);
			qwCurrentPos += nReadByte;
			int nWriteSize = 0;
			nWriteSize = KscWriteFile(hFileDest, pBuffer, nReadByte);
			ZeroMemory(pBuffer, KSC_FILE_MOVEFILE_SIZE);
			if (nWriteSize != nReadByte)
			{
				goto clean0;
			}
			m_llCurrentSize += nWriteSize;
		}

		//  retval = ::CopyFile(strSrc, strDest, FALSE);
		retval = 1;
clean0:
		if (hFileDest != INVALID_HANDLE_VALUE)
		{
			KscCloseFile(hFileDest);
			hFileDest = INVALID_HANDLE_VALUE;
		}
		if (hFileSrc != INVALID_HANDLE_VALUE)
		{
			KscCloseFile(hFileSrc);
			hFileSrc = INVALID_HANDLE_VALUE;
		}
		if (pBuffer != NULL)
		{
			delete []pBuffer;
			pBuffer = NULL;
		}
		return retval;
	}

	void AutoChangeName(CString& strDest)
	{
		CString strDestFull;
		CString strEx;
		CString strTemp;
		CString strRePath;
		int nPos = -1;;
		strDestFull = strDest;
		nPos = strDestFull.ReverseFind('.');
		if (nPos == -1)
		{
			strEx = L"";
			strDestFull = strDestFull;
		}
		else
		{
			strEx = strDestFull.Right(strDestFull.GetLength() - nPos);
			strDestFull = strDestFull.Left(nPos);
		}

		CString strTmp = strDestFull;

		nPos = -1;
		int n = 1;
		nPos = strDestFull.ReverseFind(')');
		if (nPos == strDestFull.GetLength() - 1)
		{
			strTemp = strDestFull.Left(nPos);
			strDestFull = strTemp;
			nPos = -1;
			WCHAR sz[MAX_PATH] = {0};
			nPos = strTemp.ReverseFind('(');
			strDestFull = strTemp.Left(nPos);
			strTemp = strTemp.Right(strTemp.GetLength() - nPos - 1);
			wcscpy(sz, strTemp);
			std::string str = UnicodeToAnsi(sz);
			n = atoi(str.c_str());
			if (n < 1)
			{
				strDestFull = strTmp;
			}
			n++;
			
		}
		strRePath.Format(_T("%s(%d)%s"), strDestFull, n, strEx);
		strDest = strRePath;
		if (::GetFileAttributes(strDest) != INVALID_FILE_ATTRIBUTES)
		{
			AutoChangeName(strDest);
		}
		return ;
	}

	BOOL IsNumber(TCHAR c)
	{
		return ( c >= '0' ) && ( c <= '9' );
	}

	int Recycle(LPCTSTR pszPath, BOOL bDelete/*=FALSE*/)
	{
		SHFILEOPSTRUCT  shDelFile;
		memset(&shDelFile,0,sizeof(SHFILEOPSTRUCT));
		shDelFile.fFlags |= FOF_SILENT;      // don't report progress
		shDelFile.fFlags |= FOF_NOERRORUI;     // don't report errors
		shDelFile.fFlags |= FOF_NOCONFIRMATION;    // don't confirm delete
		// Copy pathname to double-NULL-terminated string.
		//
		TCHAR buf[_MAX_PATH + 1]; // allow one more character
		_tcscpy(buf, pszPath);   // copy caller's pathname
		buf[_tcslen(buf)+1]=0;   // need two NULLs at end

		// Set SHFILEOPSTRUCT params for delete operation
		shDelFile.wFunc = FO_DELETE;       // REQUIRED: delete operation
		shDelFile.pFrom = buf;         // REQUIRED: which file(s)
		shDelFile.pTo = NULL;          // MUST be NULL
		if (bDelete)
		{         // if delete requested..
			shDelFile.fFlags &= ~FOF_ALLOWUNDO;    // ..don't use Recycle Bin
		}
		else
		{           // otherwise..
			shDelFile.fFlags |= FOF_ALLOWUNDO;    // ..send to Recycle Bin
		}
		return SHFileOperation(&shDelFile);    // do it!
	}

    KUI_NOTIFY_MAP(IDC_RICHVIEW_WIN)
        KUI_NOTIFY_ID_COMMAND(IDC_BTN_SYS_CLOSE, OnSysBtnClose)
        KUI_NOTIFY_ID_COMMAND(IDOK, OnSysBtnClose)
		KUI_NOTIFY_ID_COMMAND(105, OnSysBtnClose)
    KUI_NOTIFY_MAP_END()

    BEGIN_MSG_MAP_EX(CBigFileDoingDlg)
        MSG_KUI_NOTIFY(IDC_RICHVIEW_WIN)
        CHAIN_MSG_MAP(CKuiDialogImpl<CBigFileDoingDlg>)
        CHAIN_MSG_MAP(CWHRoundRectFrameHelper<CBigFileDoingDlg>)
        MSG_WM_TIMER(OnTimer)
        MESSAGE_HANDLER(WM_BIGFILE_DOING_END, OnDoingEnd)
        MESSAGE_HANDLER(WM_BIGFILE_DOING_CANCELED, OnDoingCanceled)
        MESSAGE_HANDLER(WM_BIGFILE_REPLACE_CONFIRM, OnReplaceConfirm)
		MESSAGE_HANDLER(WM_BIGFILE_SIZE_FAILED, OnSizeFailed)
        MSG_WM_INITDIALOG(OnInitDialog)
        REFLECT_NOTIFICATIONS_EX()
    END_MSG_MAP()

public:
		CMoveJob* m_pMoveJob;
		CDelJob* m_pDelJob;
protected:
    CString m_strTitle;
    CString m_strSubTitle;
    CString m_strCurrentDest;
    CString m_strCurrentSrc;
    CString m_strCurrentDel;
    KLock m_lock;
    HANDLE m_hExitEvent;
    HANDLE m_hWorkThread;
    int m_nPos;
    BOOL m_fUseLastConfirm;
    int m_nLastConfirm; // 0=跳过,1=覆盖,-1=未知
	int m_nPercent;
	ULONGLONG m_llSize;
	ULONGLONG m_llCurrentSize;
	BOOL m_bType;
};

//////////////////////////////////////////////////////////////////////////
