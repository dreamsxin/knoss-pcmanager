#include "EncryptFile.h"

extern "C"
{
#include "3rdparty/CRC32.h"
}

#include <stdio.h>
#include <tchar.h>
#include <shlobj.h>


int EncryptFileData(BYTE* pbyData, DWORD dwDataLen)
{
	int	nResult = false;

	DWORD dwEncryptLen	= 1024;
	int	  nKeyValue		= 1288;	

	int	nC1 = 2;
	int	nC2 = 126;

	size_t	i	= 0;

	if (dwDataLen < 1024)
		dwEncryptLen = dwDataLen;

	for (i = 0; i< dwEncryptLen; i++)
	{
		pbyData[i] = pbyData[i] ^ nKeyValue;
		//nKeyValue = (pbyData[i] + nKeyValue) * nC1 + nC2;
	}

	nResult = true;

	return nResult;
}

HRESULT Decrypt2File(OUT char* pszSaveFileName, IN int nFileNameSize)
{
	HRESULT hrResult		= E_FAIL;
	HRESULT	hrRetCode		= E_FAIL;
	HRSRC hrsrcFileName		= NULL;
	HRSRC hrsrcFileData		= NULL;
	HRSRC hrsrcChecksum		= NULL;

	HGLOBAL hglobeFileName	= NULL;
	HGLOBAL hglobeFileData	= NULL;
	HGLOBAL hglobeChecksum	= NULL;

	DWORD	dwSizeOfFileName		= 0;	
	DWORD	dwSizeOfFileData		= 0;
	DWORD	dwSizeOfFileChecksum	= 0;

	TCHAR*	pszFileNameBuf			= NULL;
	BYTE*	pbyFileDataBuf			= NULL;
	TCHAR*	pszChecksumBuf			= NULL;

	UINT	nCRCValue = 0;

	TCHAR	szCRCValue[32] = {0};

	TCHAR	szFileName[MAX_PATH]	= {0};
	TCHAR	szTempPath[MAX_PATH]	= {0};

	int		i			= 0;
	int		nRetCode	= 0;
	TCHAR*  pszTemp		= NULL;
	HANDLE	hFile		= NULL;
	DWORD	dwWrited	= 0;
	size_t	uPathLen	= 0;


	if (!pszSaveFileName)
		goto Exit0;	

	hrsrcFileName = FindResource(NULL, _T("1"), RT_RCDATA);
	hrsrcFileData = FindResource(NULL, _T("2"), RT_RCDATA);
	hrsrcChecksum = FindResource(NULL, _T("3"), RT_RCDATA);	
	if(!hrsrcFileName || !hrsrcFileData || !hrsrcChecksum)
	{
		MessageBox(NULL, _T("One Resource is Lost!"), _T("Error"), MB_ICONEXCLAMATION);
		goto Exit0;
	}

	dwSizeOfFileName = SizeofResource(NULL, hrsrcFileName);
	dwSizeOfFileData = SizeofResource(NULL, hrsrcFileData);
	dwSizeOfFileChecksum = SizeofResource(NULL, hrsrcChecksum);

	hglobeFileName = LoadResource(NULL, hrsrcFileName);
	hglobeFileData= LoadResource(NULL, hrsrcFileData);
	hglobeChecksum = LoadResource(NULL, hrsrcChecksum);

	pszFileNameBuf	= (TCHAR*)LockResource(hglobeFileName);
	pbyFileDataBuf  = (BYTE*)LockResource(hglobeFileData);
	pszChecksumBuf	= (TCHAR*)LockResource(hglobeChecksum);

	if (!pszFileNameBuf || dwSizeOfFileName <= 0 || 
		!pbyFileDataBuf || dwSizeOfFileData <= 0 ||
		!pszChecksumBuf || dwSizeOfFileChecksum <= 0
		)
	{
		MessageBox(NULL, _T("One Resource is NULL!"), _T("Error"), MB_ICONEXCLAMATION);
		goto Exit0;
	}

	nRetCode = EncryptFileData(pbyFileDataBuf, dwSizeOfFileData);
	if (!nRetCode)
	{
		MessageBox(NULL, _T("Encrypt File Failed!"), _T("Error"), MB_ICONEXCLAMATION);
		goto Exit0;	
	}

	nCRCValue = CRC32(0, pbyFileDataBuf, dwSizeOfFileData);
	_stprintf(szCRCValue, "%u", nCRCValue);


	nRetCode = _tcsncmp(pszChecksumBuf, szCRCValue, dwSizeOfFileChecksum);
	if (nRetCode != 0)
	{
		MessageBox(NULL,pszChecksumBuf , _T("Error"), MB_ICONEXCLAMATION);
		MessageBox(NULL,szCRCValue , _T("Error"), MB_ICONEXCLAMATION);
		MessageBox(NULL, _T("Check sum failed!"), _T("Error"), MB_ICONEXCLAMATION);
		goto Exit0;
	}

	//_tcscat(szFileName , pszFileNameBuf);
	nRetCode = GetWinTempPath(MAX_PATH, szTempPath);
	if (!nRetCode)
	{
		MessageBox(NULL, _T("GetTempPath failed!"), _T("Error"), MB_ICONEXCLAMATION);
		goto Exit0;
	}

	nRetCode = GetTempFileName(szTempPath, "KAV", 0, szFileName);
	if (!nRetCode)
	{
		MessageBox(NULL, _T("GetWinTempPath failed!"), _T("Error"), MB_ICONEXCLAMATION);
		goto Exit0;
	}

	DeleteFile(szFileName);

	uPathLen = _tcslen(szFileName);
	if (uPathLen < 4)
		goto Exit0;

	szFileName[uPathLen - 4] = '.';
	szFileName[uPathLen - 3] = 'E';
	szFileName[uPathLen - 2] = 'X';
	szFileName[uPathLen - 1] = 'E';

	//_tcscat(szFileName , _T(".EXE"));	

	hrResult = S_FALSE;

	hFile = ::CreateFile(
		szFileName,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE,
		NULL
		);
	if (!hFile || hFile == INVALID_HANDLE_VALUE)
		goto Exit0;

	nRetCode = WriteFile(hFile, pbyFileDataBuf, dwSizeOfFileData, &dwWrited, NULL);
	if (!nRetCode || dwWrited != dwSizeOfFileData)
		goto Exit0;

	_tcsncpy(pszSaveFileName, szFileName, nFileNameSize);

	hrResult = S_OK;

Exit0:

	if (hFile)
	{
		CloseHandle(hFile);
		hFile = NULL;
	}
	return hrResult;
}

HRESULT Decrypt2File_Oem(IN char szSaveFileDir[], IN int nResourceStart)
{
	HRESULT hrResult		= E_FAIL;
	HRESULT	hrRetCode		= E_FAIL;
	HRSRC hrsrcFileName		= NULL;
	HRSRC hrsrcFileData		= NULL;
	HRSRC hrsrcChecksum		= NULL;

	HGLOBAL hglobeFileName	= NULL;
	HGLOBAL hglobeFileData	= NULL;
	HGLOBAL hglobeChecksum	= NULL;

	DWORD	dwSizeOfFileName		= 0;	
	DWORD	dwSizeOfFileData		= 0;
	DWORD	dwSizeOfFileChecksum	= 0;

	TCHAR*	pszFileNameBuf			= NULL;
	BYTE*	pbyFileDataBuf			= NULL;
	TCHAR*	pszChecksumBuf			= NULL;

	UINT	nCRCValue = 0;

	TCHAR	szCRCValue[32] = {0};

	TCHAR	szOemPath[MAX_PATH]	= {0};

	TCHAR   szCheckDir[MAX_PATH] = {0};
	TCHAR   *pszTemp = NULL;

	int		i			= 0;
	int		nRetCode	= 0;
	HANDLE	hFile		= NULL;
	DWORD	dwWrited	= 0;
	
	TCHAR szResName[10] = {0};

	if (!szSaveFileDir)
		goto Exit0;	

	hrResult = S_FALSE;
	
	for(int i = nResourceStart; i >= nResourceStart; i += 3)
	{
		_stprintf(szResName, _T("%d"), i);
		hrsrcFileName = FindResource(NULL, szResName, RT_RCDATA);
		_stprintf(szResName, _T("%d"), (i + 1));
		hrsrcFileData = FindResource(NULL, szResName, RT_RCDATA);
		_stprintf(szResName, _T("%d"), (i + 2));
		hrsrcChecksum = FindResource(NULL, szResName, RT_RCDATA);	
		if(!hrsrcFileName || !hrsrcFileData || !hrsrcChecksum)
			goto Exit1;

		dwSizeOfFileName = SizeofResource(NULL, hrsrcFileName);
		dwSizeOfFileData = SizeofResource(NULL, hrsrcFileData);
		dwSizeOfFileChecksum = SizeofResource(NULL, hrsrcChecksum);

		hglobeFileName = LoadResource(NULL, hrsrcFileName);
		hglobeFileData= LoadResource(NULL, hrsrcFileData);
		hglobeChecksum = LoadResource(NULL, hrsrcChecksum);

		pszFileNameBuf	= (TCHAR*)LockResource(hglobeFileName);
		pbyFileDataBuf  = (BYTE*)LockResource(hglobeFileData);
		pszChecksumBuf	= (TCHAR*)LockResource(hglobeChecksum);

		if (!pszFileNameBuf || dwSizeOfFileName <= 0 || 
			!pbyFileDataBuf || dwSizeOfFileData <= 0 ||
			!pszChecksumBuf || dwSizeOfFileChecksum <= 0
			)
		{
			MessageBox(NULL, _T("One Resource is NULL!"), _T("Error"), MB_ICONEXCLAMATION);
			goto Exit0;
		}

		nCRCValue = CRC32(0, pbyFileDataBuf, dwSizeOfFileData);
		_stprintf(szCRCValue, "%u", nCRCValue);


		nRetCode = _tcsncmp(pszChecksumBuf, szCRCValue, dwSizeOfFileChecksum);
		if (nRetCode != 0)
		{
			MessageBox(NULL,pszChecksumBuf , _T("Error"), MB_ICONEXCLAMATION);
			MessageBox(NULL,szCRCValue , _T("Error"), MB_ICONEXCLAMATION);
			MessageBox(NULL, _T("Check sum failed!"), _T("Error"), MB_ICONEXCLAMATION);
			goto Exit0;
		}
		
		_tcscpy(szOemPath, szSaveFileDir);
		_tcscat(szOemPath, _T("\\"));
		_tcsncat(szOemPath, pszFileNameBuf, dwSizeOfFileName);

		_tcscpy(szCheckDir, szOemPath);
		pszTemp = _tcsrchr(szCheckDir, _T('\\'));
		if (pszTemp != NULL)
			*pszTemp = _T('\0');

		if(GetFileAttributes(szCheckDir) == 0xffffffff)
		{
			//SHCreateDirectoryEx(NULL, szCheckDir, NULL);
			TCHAR szFindPath[MAX_PATH] = {0};
			int nStart = 3;
			TCHAR *pszFind = szFindPath;


			while(pszFind != NULL)
			{
				pszFind = _tcsstr(szCheckDir + nStart, _T("\\"));
				if(pszFind == NULL)
					continue;

				nStart = pszFind - szCheckDir;
				_tcsncpy(szFindPath, szCheckDir, nStart);
				nStart += 1;
				if(GetFileAttributes(szFindPath) == 0xffffffff)
					CreateDirectory(szFindPath, NULL);
			}

			if(GetFileAttributes(szCheckDir) == 0xffffffff)
				CreateDirectory(szCheckDir, NULL);
		}

		hFile = ::CreateFile(
			szOemPath,
			GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE,
			NULL
			);
		if (!hFile || hFile == INVALID_HANDLE_VALUE)
			goto Exit0;

		nRetCode = WriteFile(hFile, pbyFileDataBuf, dwSizeOfFileData, &dwWrited, NULL);
		if (!nRetCode || dwWrited != dwSizeOfFileData)
			goto Exit0;

		if (hFile)
		{
			CloseHandle(hFile);
			hFile = NULL;
		}
	}

Exit1:
	hrResult = S_OK;

Exit0:

	if (hFile)
	{
		CloseHandle(hFile);
		hFile = NULL;
	}
	return hrResult;
}

HRESULT Decrypt2File_One(IN OUT char szSaveFileDir[], IN int nResourceStart)
{
	HRESULT hrResult		= E_FAIL;
	HRESULT	hrRetCode		= E_FAIL;
	HRSRC hrsrcFileName		= NULL;
	HRSRC hrsrcFileData		= NULL;
	HRSRC hrsrcChecksum		= NULL;

	HGLOBAL hglobeFileName	= NULL;
	HGLOBAL hglobeFileData	= NULL;
	HGLOBAL hglobeChecksum	= NULL;

	DWORD	dwSizeOfFileName		= 0;	
	DWORD	dwSizeOfFileData		= 0;
	DWORD	dwSizeOfFileChecksum	= 0;

	TCHAR*	pszFileNameBuf			= NULL;
	BYTE*	pbyFileDataBuf			= NULL;
	TCHAR*	pszChecksumBuf			= NULL;

	UINT	nCRCValue = 0;

	TCHAR	szCRCValue[32] = {0};

	TCHAR	szOemPath[MAX_PATH]	= {0};

	TCHAR   szCheckDir[MAX_PATH] = {0};
	TCHAR   *pszTemp = NULL;

	int		i			= 0;
	int		nRetCode	= 0;
	HANDLE	hFile		= NULL;
	DWORD	dwWrited	= 0;

	TCHAR szResName[10] = {0};

	if (!szSaveFileDir)
		goto Exit0;	

	hrResult = S_FALSE;

	_stprintf(szResName, _T("%d"), nResourceStart);
	hrsrcFileName = FindResource(NULL, szResName, RT_RCDATA);
	_stprintf(szResName, _T("%d"), (nResourceStart + 1));
	hrsrcFileData = FindResource(NULL, szResName, RT_RCDATA);
	_stprintf(szResName, _T("%d"), (nResourceStart + 2));
	hrsrcChecksum = FindResource(NULL, szResName, RT_RCDATA);	
	if(!hrsrcFileName || !hrsrcFileData || !hrsrcChecksum)
		goto Exit1;

	dwSizeOfFileName = SizeofResource(NULL, hrsrcFileName);
	dwSizeOfFileData = SizeofResource(NULL, hrsrcFileData);
	dwSizeOfFileChecksum = SizeofResource(NULL, hrsrcChecksum);

	hglobeFileName = LoadResource(NULL, hrsrcFileName);
	hglobeFileData = LoadResource(NULL, hrsrcFileData);
	hglobeChecksum = LoadResource(NULL, hrsrcChecksum);

	pszFileNameBuf	= (TCHAR*)LockResource(hglobeFileName);
	pbyFileDataBuf  = (BYTE*)LockResource(hglobeFileData);
	pszChecksumBuf	= (TCHAR*)LockResource(hglobeChecksum);

	if (!pszFileNameBuf || dwSizeOfFileName <= 0 || 
		!pbyFileDataBuf || dwSizeOfFileData <= 0 ||
		!pszChecksumBuf || dwSizeOfFileChecksum <= 0
		)
	{
		MessageBox(NULL, _T("One Resource is NULL!"), _T("Error"), MB_ICONEXCLAMATION);
		goto Exit0;
	}

	nCRCValue = CRC32(0, pbyFileDataBuf, dwSizeOfFileData);
	_stprintf(szCRCValue, "%u", nCRCValue);

	nRetCode = _tcsncmp(pszChecksumBuf, szCRCValue, dwSizeOfFileChecksum);
	if (nRetCode != 0)
	{
		MessageBox(NULL,pszChecksumBuf , _T("Error"), MB_ICONEXCLAMATION);
		MessageBox(NULL,szCRCValue , _T("Error"), MB_ICONEXCLAMATION);
		MessageBox(NULL, _T("Check sum failed!"), _T("Error"), MB_ICONEXCLAMATION);
		goto Exit0;
	}

	_tcscpy(szOemPath, szSaveFileDir);
	_tcscat(szOemPath, _T("\\"));
	_tcsncat(szOemPath, pszFileNameBuf, dwSizeOfFileName);

	_tcscpy(szCheckDir, szOemPath);
	pszTemp = _tcsrchr(szCheckDir, _T('\\'));
	if (pszTemp != NULL)
		*pszTemp = _T('\0');

	if(GetFileAttributes(szCheckDir) == 0xffffffff)
	{
		//SHCreateDirectoryEx(NULL, szCheckDir, NULL);
		TCHAR szFindPath[MAX_PATH] = {0};
		int nStart = 3;
		TCHAR *pszFind = szFindPath;


		while(pszFind != NULL)
		{
			pszFind = _tcsstr(szCheckDir + nStart, _T("\\"));
			if(pszFind == NULL)
				continue;

			nStart = pszFind - szCheckDir;
			_tcsncpy(szFindPath, szCheckDir, nStart);
			nStart += 1;
			if(GetFileAttributes(szFindPath) == 0xffffffff)
				CreateDirectory(szFindPath, NULL);
		}

		if(GetFileAttributes(szCheckDir) == 0xffffffff)
			CreateDirectory(szCheckDir, NULL);
	}

	hFile = ::CreateFile(
		szOemPath,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE,
		NULL
		);
	if (!hFile || hFile == INVALID_HANDLE_VALUE)
		goto Exit0;

	nRetCode = WriteFile(hFile, pbyFileDataBuf, dwSizeOfFileData, &dwWrited, NULL);
	if (!nRetCode || dwWrited != dwSizeOfFileData)
		goto Exit0;

	if (hFile)
	{
		CloseHandle(hFile);
		hFile = NULL;
	}

	_tcscpy(szSaveFileDir, szOemPath);

Exit1:
	hrResult = S_OK;

Exit0:

	if (hFile)
	{
		CloseHandle(hFile);
		hFile = NULL;
	}
	return hrResult;
}

DWORD GetWinTempPath(DWORD nBufferLength, LPTSTR lpBuffer)
{
	DWORD dwResult = 0;
	dwResult = GetWindowsDirectory(lpBuffer, nBufferLength - 5);
	if (!dwResult)
		goto Exit0;

	if (lpBuffer[strlen(lpBuffer) - 1] != '\\')
		strcat(lpBuffer, "\\");
	
	strcat(lpBuffer, "Temp\\");

	if(GetFileAttributes(lpBuffer) == 0xffffffff)
	{
		TCHAR szTemp[MAX_PATH + 1] = {0};

		for(int i = 0; i < strlen(lpBuffer); i++)
		{
			if(lpBuffer[i] == '\\')
			{
				if(GetFileAttributes(szTemp) == 0xffffffff)
					CreateDirectory(szTemp, NULL);
			}

			szTemp[i] = lpBuffer[i];
		}
	}
	
Exit0:
	return dwResult;
}