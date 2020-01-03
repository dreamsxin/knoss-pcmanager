#include "stdafx.h"
#include "KFunction.h"

int KFunction::GetDirCount(LPCTSTR szPath)
{
	LPCTSTR pszChr = _tcsrchr(szPath, _T('\\'));
	
	if (!pszChr) return -1;

	return pszChr - szPath + 1;
}

int KFunction::GetFileNameCount(LPCTSTR szPath)
{
	LPCTSTR pszChr = _tcsrchr(szPath, _T('\\'));

	if (!pszChr) return -1;

	return _tcslen(++pszChr);
}

int KFunction::GetFileExtCount(LPCTSTR szPath)
{
	LPCTSTR pszChr = _tcsrchr(szPath, _T('.'));

	if (!pszChr) return -1;

	return _tcslen(pszChr);
}

void KFunction::PathAddBackslash(CString& strPath)
{
	TCHAR ch = strPath.GetAt(strPath.GetLength() - 1);
	if (ch != _T('\\') && ch != _T('/'))
		strPath.AppendChar(_T('\\'));
}

CString KFunction::GetModulePath(HMODULE hModule)
{
	DWORD dwRet = 0;
	CString strTemp;
	TCHAR szFileName[MAX_PATH * 4 + 1] = {0};

	dwRet = GetModuleFileName(hModule, szFileName, MAX_PATH * 4);
	if (!dwRet) return _T("");

	strTemp = szFileName;
	int nPos = -1;
	nPos = strTemp.ReverseFind(_T('\\'));
	if (-1 == nPos)
	{
		nPos = strTemp.ReverseFind(_T('/'));
	}

	if (-1 == nPos) return _T("");

	return strTemp.Left(nPos + 1);
}

LONGLONG KFunction::GetFileSize(LPCTSTR szPath)
{
	LARGE_INTEGER			filesize = {0};

	HANDLE hFile = CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); 
	if (hFile == INVALID_HANDLE_VALUE) return 0;

	BOOL bRetCode = GetFileSizeEx(hFile, &filesize);
	if (!bRetCode) return 0;

	CloseHandle(hFile);

	return filesize.QuadPart;
}

BOOL KFunction::IsWildcardString(CString& strString)
{
	if (strString.Find(_T('*')) != -1 || 
		strString.Find(_T('?')) != -1)
	{
		return TRUE;
	}
	return FALSE;
}