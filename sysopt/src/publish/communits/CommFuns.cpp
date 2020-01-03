
// dummyz@126.com
//
#include "stdafx.h"
#include "CommFuns.h"
#include "md5/md5.h"
#include <strsafe.h>
#include <Iphlpapi.h>
#include <Wininet.h>
#include <TlHelp32.h>
#include <assert.h>
#include <ShlObj.h>
#include <list>
#include <ShellAPI.h>
#include <atlenc.h>

#include <Psapi.h>
#include <Shlwapi.h>
#include "winmod/winwow64.h"

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "psapi.lib")

using namespace std;

static WinMod::CWinWow64 _Wow64;

#if DBG_FMT
void DebugPrintf(LPCWSTR lpFormat, ...)
{
	const int nBufLen = MAX_PATH * 2;
	WCHAR szBuf[nBufLen];
	va_list ap;
	
	va_start(ap, lpFormat);
	StringCchVPrintfW(szBuf, nBufLen, lpFormat, ap);

	OutputDebugString(szBuf);

#if 0
	FILE* lpFile = _tfopen(_T("c:\\ksafemon_1.txt"), _T("a+"));
	if ( lpFile == NULL )
	{
		lpFile = _tfopen(_T("c:\\ksafemon_2.txt"), _T("a+"));
		if ( lpFile == NULL )
		{
			lpFile = _tfopen(_T("c:\\ksafemon_3.txt"), _T("a+"));
		}
	}

	if ( lpFile != NULL )
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		
		_ftprintf(lpFile, _T("[%02d:%02d:%02d]%s\n"), st.wHour, st.wMinute, st.wSecond, szBuf);
		fclose(lpFile);
	}
#endif

	va_end(ap);
}

void DebugPrintf(LPCSTR lpFormat, ...)
{
	const int nBufLen = MAX_PATH * 2;
	CHAR szBuf[nBufLen];
	va_list ap;

	va_start(ap, lpFormat);
	StringCchVPrintfA(szBuf, nBufLen, lpFormat, ap);
	va_end(ap);

	OutputDebugStringA(szBuf);
}
#endif

#if UNFLY_DEBUG_PRINT_LOG

void UnflyLogW( LPCWSTR format, ... )
{
	int Length;
	WCHAR *Buffer;
	va_list ap;
	WCHAR StaticBuffer[512];
	va_start(ap, format);
	Length = _vscwprintf(format, ap);
	va_end(ap);

	Length += 64;

	if( Length >= sizeof(StaticBuffer) )
		Buffer = new WCHAR[Length];
	else
		Buffer = StaticBuffer;

	ZeroMemory(Buffer, sizeof(Buffer));

	va_start(ap, format);
	Length = _vsnwprintf(Buffer, Length, format, ap);
	va_end(ap);
	if( Length > 0 )
	{
		Buffer[Length] = 0;
	}
	else
	{
		StringCchCopyW(Buffer, Length, L"AgentTaskLog _vsnwprintf error");
	}

#if UNFLY_LOG_TO_FILE
	FILE* lpFile = _tfopen(_T("unflydbg.txt"), _T("a+"));
	if ( lpFile == NULL )
	{
		return ;
	}

	if ( lpFile != NULL )
	{
		SYSTEMTIME st;
		GetLocalTime(&st);

		_ftprintf(lpFile, _T("[%d-%02d-%02d %02d:%02d:%02d] %s\n"), 
			st.wYear,
			st.wMonth,
			st.wDay,
			st.wHour, 
			st.wMinute, 
			st.wSecond, 
			Buffer);
		fclose(lpFile);
	}
#else
	OutputDebugStringW(Buffer);
#endif

	if( Buffer != StaticBuffer )
	{	
		delete [] Buffer;
		Buffer = NULL;
	}
}

void UnflyLogA( char *format, ... )
{
	int Length;
	char *Buffer;
	va_list ap;
	char StaticBuffer[512];
	va_start(ap, format);
	Length = _vscprintf(format, ap);
	va_end(ap);

	Length += 64;

	if( Length >= sizeof(StaticBuffer) )
		Buffer = new char[Length];
	else
		Buffer = StaticBuffer;

	ZeroMemory(Buffer, sizeof(Buffer));

	va_start(ap, format);
	Length = vsnprintf(Buffer, Length, format, ap);
	va_end(ap);
	if( Length > 0 )
	{
		Buffer[Length] = 0;
	}
	else
	{
		strcpy(Buffer, "AgentTaskLog vsnprintf error");
	}

#if UNFLY_LOG_TO_FILE
	FILE* lpFile = _tfopen(_T("unflydbg.txt"), _T("a+"));
	if ( lpFile == NULL )
	{
		return ;
	}

	if ( lpFile != NULL )
	{
		SYSTEMTIME st;
		GetLocalTime(&st);

		fprintf(lpFile, "[%d-%02d-%02d %02d:%02d:%02d] %s\n", 
			st.wYear,
			st.wMonth,
			st.wDay,
			st.wHour, 
			st.wMinute, 
			st.wSecond, 
			Buffer);
		fclose(lpFile);
	}
#else
	OutputDebugStringA(Buffer);
#endif

	if( Buffer != StaticBuffer )
	{	
		delete [] Buffer;
		Buffer = NULL;
	}
}
#endif

LPCTSTR	MyStrStrI(LPCTSTR lpStr1, LPCTSTR lpStr2)
{
	size_t nStr2Len = _tcslen(lpStr2);
	while ( *lpStr1 != 0 )
	{
		if ( _tcsnicmp(lpStr1, lpStr2, nStr2Len) == 0 )
		{
			return lpStr1;
		}
		
		lpStr1++;
	}
	
	return NULL;
}

HKEY MyOpenRegKey(HKEY hKey, LPCTSTR lpSubKey, BOOL bReadOnly)
{
	LRESULT lResult;
	HKEY hSubKey;
	
	if ( bReadOnly )
	{
		lResult = RegOpenKeyEx(hKey,
			lpSubKey,
			0,
			KEY_READ,
			&hSubKey
			);
	}
	else
	{
		lResult =  RegCreateKeyEx(hKey,
			lpSubKey,
			0,
			NULL,
			0,
			(KEY_READ | KEY_WRITE),
			NULL,
			&hSubKey,
			NULL
			);
	}

	return (lResult == ERROR_SUCCESS) ? hSubKey : NULL;
}

void MyCloseRegKey(HKEY hKey)
{
	RegCloseKey(hKey);
}

BOOL MyReadRegValue(HKEY hKey, LPCTSTR lpValueName, LPTSTR lpValue, DWORD dwLength)
{
	LRESULT lResult;
	DWORD dwType;
	DWORD dwSize = dwLength * sizeof (TCHAR);
	
	lResult = RegQueryValueEx(hKey, 
		lpValueName, 
		NULL, 
		&dwType, 
		(LPBYTE)lpValue, 
		&dwSize
		);
	if ( lResult == ERROR_SUCCESS && dwType == REG_SZ )
	{
		return TRUE;
	}
	
	return FALSE;
}

BOOL MyReadRegValue(HKEY hKey, LPCTSTR lpValueName, LPDWORD lpdwValue)
{
	LRESULT lResult;
	DWORD dwType;
	DWORD dwSize = sizeof (DWORD);
	
	lResult = RegQueryValueEx(hKey, 
		lpValueName, 
		NULL, 
		&dwType, 
		(LPBYTE)lpdwValue, 
		&dwSize
		);
	if ( lResult == ERROR_SUCCESS && dwType == REG_DWORD )
	{
		return TRUE;
	}
	
	return FALSE;
}

BOOL MyWriteRegValue(HKEY hKey, LPCTSTR lpValueName, LPCTSTR lpValue)
{
	LRESULT lResult;
	DWORD dwType = REG_SZ;
	size_t dwSize = _tcslen(lpValue) * sizeof (TCHAR);
	
	lResult = RegSetValueEx(hKey, lpValueName, 0, dwType, (LPBYTE)lpValue, (DWORD)dwSize);
	return (lResult == ERROR_SUCCESS);
}

BOOL MyWriteRegValue(HKEY hKey, LPCTSTR lpValueName, DWORD dwValue)
{
	LRESULT lResult;
	DWORD dwType = REG_DWORD;
	DWORD dwSize = sizeof (DWORD);
	
	lResult = RegSetValueEx(hKey, lpValueName, 0, dwType, (LPBYTE)&dwValue, dwSize);
	return (lResult == ERROR_SUCCESS);
}



LPCTSTR MyGetFileName(LPCTSTR lpPath)
{
	LPCTSTR lpFileName = _tcsrchr(lpPath, _T('\\'));
	if ( lpFileName == NULL )
	{
		lpFileName = _tcsrchr(lpPath, _T('/'));
		if ( lpFileName == NULL )
		{
			lpFileName = lpPath;
		}
		else
		{
			lpFileName++;
		}
	}
	else
	{
		lpFileName++;
	}

	return lpFileName;
}

LPCTSTR	MyGetFileDir(LPCTSTR lpPath, LPTSTR lpDir)
{
	LPCTSTR lpFileName = MyGetFileName(lpPath);
	int nFileNameOffset = (int)(lpFileName - lpPath);

	_tcsncpy(lpDir, lpPath, nFileNameOffset);
	lpDir[nFileNameOffset] = 0;

	return lpDir;
}

LPCTSTR	MyAppendFileName(LPTSTR lpPath, LPCTSTR lpFileName)
{
	assert(lpPath[0] != _T('\0'));

	size_t nLen = _tcslen(lpPath);
	if ( nLen != 0 )
	{
		LPTSTR lp = lpPath + nLen;

		if ( lp[-1] != _T('\\') )
		{
			*lp++ = _T('\\');
			nLen++;
		}

		if ( *lpFileName == _T('\\') )
		{
			lpFileName++;
		}

		StringCchCopy(lp, MAX_PATH - nLen, lpFileName);
	}

	return lpPath;
}

DWORD MyGetOsVersion()
{
	return GetVersion();
}

static BOOL NetGatewayExists()
{
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	
	BOOL bExists = FALSE;
	DWORD dwRetVal = 0;
	
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO *) malloc(ulOutBufLen);
	if(pAdapterInfo)
	{
		dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen);
		if(dwRetVal == ERROR_BUFFER_OVERFLOW )
		{
			free(pAdapterInfo);
			
			pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen);
			if(pAdapterInfo)
				dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen);
		}
		
		if( dwRetVal == NO_ERROR)
		{
			pAdapter = pAdapterInfo;
			while (pAdapter)
			{
				if(pAdapter->GatewayList.IpAddress.String[0])
				{
					bExists = TRUE;
					break;
				}
				pAdapter = pAdapter->Next;
			}
		}
		
		free(pAdapterInfo);
	}
	
	return bExists;
}

BOOL HaveInternetConnection()
{
	DWORD dwFlag;
	
	//	如果函数返回FALSE,则肯定没有连接
	if( InternetGetConnectedState(&dwFlag, 0) )
	{
		//	必须有下列任意一种连接，才认为是有连接
		//	因为INTERNET_CONNECTION_CONFIGURED被置位也可能是没有连接
		if( (dwFlag&INTERNET_CONNECTION_LAN) ||
			(dwFlag&INTERNET_CONNECTION_MODEM) ||
			(dwFlag&INTERNET_CONNECTION_PROXY) )
			return TRUE;
	}
	
	return NetGatewayExists();
}

BOOL IsXPLast()
{
	OSVERSIONINFOEX v = { 0 };

	v.dwOSVersionInfoSize = sizeof (v);
	if ( GetVersionEx((LPOSVERSIONINFO)&v) )
	{
		if ( v.dwMajorVersion >= 5 && v.dwMinorVersion != 0 )
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL IsWin64()
{
	BOOL bResult = FALSE;
	BOOL (WINAPI *pfnIsWow64Process)(HANDLE hProcess, BOOL* bWindows);
	BOOL (WINAPI* pfnGetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);

	HMODULE hDll = GetModuleHandle(_T("kernel32.dll"));
	if ( hDll != NULL )
	{
		(FARPROC&)pfnIsWow64Process = GetProcAddress(hDll, "IsWow64Process");
		if ( pfnIsWow64Process != NULL )
		{
			BOOL bWoW64;

			if ( pfnIsWow64Process(GetCurrentProcess(), &bWoW64) && bWoW64 )
			{
				(FARPROC&)pfnGetNativeSystemInfo = GetProcAddress(hDll, "GetNativeSystemInfo");
				if ( pfnGetNativeSystemInfo != NULL )
				{
					SYSTEM_INFO si = { 0 };

					if ( pfnGetNativeSystemInfo(&si) )
					{
						bResult = (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
							si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64);
					}
				}
			}
		}
	}

	return bResult;
}

BOOL IsDownloadFile(LPCTSTR lpPath)
{
/*
// 如果存在下面的键值，IE 下载的文件将没有安全锁定
[HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\Attachments]
"SaveZoneInformation"=dword:00000001
	*/
	
	BOOL bResult = FALSE;
	TCHAR szStreamPath[MAX_PATH];
	
	StringCchPrintf(szStreamPath, MAX_PATH, _T("%s:Zone.Identifier"), lpPath);
	
	HANDLE hFile = CreateFile(szStreamPath,
		GENERIC_READ,
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING,
		0,
		NULL
		);
	if ( hFile != INVALID_HANDLE_VALUE )
	{
		WCHAR cBuff[100];
		DWORD dwReadBytes = 0;

		ReadFile(hFile, cBuff, sizeof (cBuff), &dwReadBytes, NULL);
		cBuff[dwReadBytes / sizeof (WCHAR)] = 0;

		LPWSTR lp = wcsstr(cBuff, L"[ZoneTransfer]");
		if ( lp != NULL )
		{
			lp = (LPWSTR)((PBYTE)lp + sizeof (L"[ZoneTransfer]") - 2);
			lp = wcsstr(lp, L"ZoneId=");
			if ( lp != NULL )
			{
				lp = (LPWSTR)((PBYTE)lp + sizeof (L"ZoneId=") - 2);

				int nZoneId = _wtoi(lp);
				if ( nZoneId == URLZONE_INTERNET )
				{
					bResult = TRUE;
				}
			}
		}
		
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}

	return bResult;
}

int	MyGetPathType(LPCTSTR lpPath)
{
	DWORD dwErrMode = SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

	DWORD dwAttr = GetFileAttributes(lpPath);

	SetErrorMode(dwErrMode);

	if ( dwAttr == INVALID_FILE_ATTRIBUTES )
		return 0;

	return (dwAttr & FILE_ATTRIBUTE_DIRECTORY) ? 2 : 1;
}

DWORD MyGetFileSize(LPCTSTR lpPath)
{
	WIN32_FILE_ATTRIBUTE_DATA wfd;

	if ( GetFileAttributesEx(lpPath, GetFileExInfoStandard, &wfd) )
	{
		return (wfd.nFileSizeHigh != 0) ? 0xffffffff : wfd.nFileSizeLow;
	}

	return 0;
}

LPWSTR Ansi2Unicode(LPCSTR lpStr,  DWORD nMaxLen)
{
	LPWSTR lpResult = NULL;

	size_t nLen = strlen(lpStr);
	DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, lpStr, (DWORD)nLen, NULL, 0);
	
	lpResult = new WCHAR[max(dwNum, nMaxLen) + 1];
	if ( lpResult != NULL )
	{
		lpResult[0] = 0;
		MultiByteToWideChar(CP_ACP, 0, lpStr, (DWORD)nLen, lpResult, dwNum);
		lpResult[dwNum] = 0;
	}

	return lpResult;
}

LPSTR Unicode2Ansi(LPCWSTR lpStr, DWORD nMaxLen)
{
	LPSTR lpResult = NULL;

	size_t nLen = wcslen(lpStr);
	DWORD dwNum = WideCharToMultiByte(CP_ACP, 0, lpStr, (DWORD)nLen, NULL, 0, NULL, NULL);

	lpResult = new CHAR[max(dwNum, nMaxLen) + 1];
	if ( lpResult != NULL )
	{
		lpResult[0] = 0;
		WideCharToMultiByte(CP_ACP, 0, lpStr, (DWORD)nLen, lpResult, dwNum, NULL, NULL);
		lpResult[dwNum] = 0;
	}

	return lpResult;
}


LPSTR ToUTF8Str(LPCSTR lpStr)
{
	LPSTR lpResult = NULL;

	LPWSTR lpTemp = Ansi2Unicode(lpStr);
	if ( lpTemp != NULL )
	{
		lpResult = ToUTF8Str(lpTemp);

		delete lpTemp;
		lpTemp = NULL;
	}

	return lpResult;
}

LPSTR ToUTF8Str(LPCWSTR lpStr, DWORD nMaxLen)
{
	LPSTR lpResult = NULL;

	size_t nLen = wcslen(lpStr);
	DWORD dwNum = WideCharToMultiByte(CP_UTF8, 0, lpStr, (DWORD)nLen, NULL, 0, NULL, NULL);
	
	lpResult = new CHAR[max(dwNum, nMaxLen) + 1];
	if ( lpResult != NULL )
	{
		lpResult[0] = 0;
		WideCharToMultiByte(CP_UTF8, 0, lpStr, (DWORD)nLen, lpResult, dwNum, NULL, NULL);
		lpResult[dwNum] = 0;
	}
	
	return lpResult;
}

LPSTR UTF8ToAnsi(LPCSTR lpStr)
{
	LPSTR lpResult = NULL;

	// utf8 -> unicode -> ansi
	LPWSTR lpTemp = Ansi2Unicode(lpStr);
	if ( lpTemp != NULL )
	{
		size_t nLen = wcslen(lpTemp);
		DWORD dwNum = WideCharToMultiByte(CP_ACP, 0, lpTemp, (DWORD)nLen, NULL, 0, NULL, NULL);
		
		lpResult = new CHAR[dwNum + 1];
		if ( lpResult != NULL )
		{
			lpResult[0] = 0;
			WideCharToMultiByte(CP_ACP, 0, lpTemp, (DWORD)nLen, lpResult, dwNum, NULL, NULL);
			lpResult[dwNum] = 0;
		}

		delete lpTemp;
		lpTemp = NULL;
	}

	return lpResult;
}

LPSTR ToBase64Str(LPCWSTR lpStr)
{
	LPSTR lpResult = NULL;

	LPSTR lpUTF8Str = ToUTF8Str(lpStr);
	if ( lpUTF8Str != NULL )
	{
		int nSrcLen = (int)strlen(lpUTF8Str);
		int nDstLen = Base64EncodeGetRequiredLength(nSrcLen, ATL_BASE64_FLAG_NOCRLF);
		
		lpResult = new CHAR[nDstLen + 1];
		if ( lpResult != NULL )
		{
			if ( !Base64Encode((PBYTE)lpUTF8Str, nSrcLen, lpResult, &nDstLen, ATL_BASE64_FLAG_NOCRLF) )
			{
				delete []lpResult;
				lpResult = NULL;
			}
			else
			{
				lpResult[nDstLen] = 0;
			}
		}

		delete []lpUTF8Str;
		lpUTF8Str = NULL;
	}

	return lpResult;
}

void	ToBase64Str(LPCWSTR lpStr, CString& strBase64)
{
	strBase64.Empty();

	LPSTR lpBuff = ToBase64Str(lpStr);
	if ( lpBuff != NULL )
	{
		strBase64 = lpBuff;

		delete []lpBuff;
	}
}

static void get_host_name(LPCTSTR lpUrl, LPCTSTR& lpHost_Begin, LPCTSTR& lpHost_End)
{
	lpHost_Begin = _tcsstr(lpUrl, _T("://"));
	if ( lpHost_Begin == NULL )
	{
		lpHost_Begin = lpUrl;
	}
	else
	{
		lpHost_Begin += 3;
	}

	lpHost_End = _tcschr(lpHost_Begin, _T('/'));
	if ( lpHost_End == NULL )
	{
		lpHost_End = lpHost_Begin + _tcslen(lpHost_Begin);
	}
}

BOOL IsEquHostName(LPCTSTR lpUrl1, LPCTSTR lpUrl2)
{
	LPCTSTR lpHost1_Begin, lpHost1_End;
	LPCTSTR lpHost2_Begin, lpHost2_End;

	get_host_name(lpUrl1, lpHost1_Begin, lpHost1_End);
	get_host_name(lpUrl2, lpHost2_Begin, lpHost2_End);

	size_t nHost1_Len = lpHost1_End - lpHost1_Begin;
	size_t nHost2_Len = lpHost2_End - lpHost2_Begin;
	return (nHost1_Len == nHost2_Len) && _tcsnicmp(lpHost1_Begin, lpHost2_Begin, nHost1_Len) == 0;
}

CString MyGetHostName(LPCTSTR lpUrl)
{
	LPCTSTR lpHost_Begin, lpHost_End;

	get_host_name(lpUrl, lpHost_Begin, lpHost_End);

	return CString(lpHost_Begin, (int)(lpHost_End - lpHost_Begin));
}

//
BOOL IsPackerFile(LPCTSTR lpPath)
{
	BOOL bResult = FALSE;
	
	HANDLE hFile = CreateFile(lpPath, 
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
		);
	if ( hFile != INVALID_HANDLE_VALUE )
	{
		IMAGE_DOS_HEADER DosHeader;
		IMAGE_NT_HEADERS NtHeader;
		IMAGE_SECTION_HEADER SectHeader;
		DWORD dwReadBytes;
		
		if ( ReadFile(hFile, &DosHeader, sizeof (DosHeader), &dwReadBytes, NULL) &&
			dwReadBytes == sizeof (DosHeader) &&
			DosHeader.e_magic == IMAGE_DOS_SIGNATURE &&
			DosHeader.e_lfanew != 0
			)
		{
			SetFilePointer(hFile, DosHeader.e_lfanew, NULL, FILE_BEGIN);
			if ( ReadFile(hFile, &NtHeader, sizeof (NtHeader), &dwReadBytes, NULL) &&
				dwReadBytes == sizeof (NtHeader) &&
				NtHeader.Signature == IMAGE_NT_SIGNATURE &&
				NtHeader.FileHeader.NumberOfSections != 0 
				)
			{
				SetFilePointer(hFile, 
					NtHeader.FileHeader.SizeOfOptionalHeader - sizeof (NtHeader.OptionalHeader),
					NULL,
					FILE_CURRENT
					);
				
				if ( ReadFile(hFile, &SectHeader, sizeof (IMAGE_SECTION_HEADER), &dwReadBytes, NULL) &&
					dwReadBytes == sizeof (IMAGE_SECTION_HEADER)
					)
				{
					if ( memcmp(SectHeader.Name, ".textbss", 8) != 0 &&
						SectHeader.SizeOfRawData == 0
						)
					{
						if ( NtHeader.OptionalHeader.AddressOfEntryPoint < NtHeader.OptionalHeader.BaseOfCode ||
							NtHeader.OptionalHeader.AddressOfEntryPoint >= NtHeader.OptionalHeader.BaseOfCode + NtHeader.OptionalHeader.SizeOfCode
							)
						{
							bResult = TRUE;
							goto _Exit_Return;
						}
						
						if ( NtHeader.OptionalHeader.AddressOfEntryPoint > SectHeader.VirtualAddress + SectHeader.Misc.VirtualSize ||
							(memcmp(SectHeader.Name, ".text\x0\x0\x0", 8) != 0 && memcmp(SectHeader.Name, "CODE\x0\x0\x0\x0", 8) != 0)
							)
						{
							bResult = TRUE;
							goto _Exit_Return;
						}
					}
				}
			}
		}
		
_Exit_Return:
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
	
	return bResult;
}

struct PATH_ITEM {
	TCHAR path[MAX_PATH];
	};

BOOL IsRiskPath(LPCTSTR lpPath)
{
	static list<PATH_ITEM> paths;
	static BOOL bInitPaths = FALSE;

	if ( !bInitPaths )
	{
		PATH_ITEM item;

		bInitPaths = TRUE;

		if ( SHGetSpecialFolderPath(NULL, item.path, CSIDL_INTERNET_CACHE, FALSE) )
		{
			paths.push_back(item);
		}
		
		if ( SHGetSpecialFolderPath(NULL, item.path, CSIDL_PROFILE, FALSE) )
		{
			paths.push_back(item);
		}
		
		if ( GetTempPath(MAX_PATH, item.path) )
		{
			paths.push_back(item);
		}
	}

	list<PATH_ITEM>::iterator i = paths.begin();
	while ( i != paths.end() )
	{
		PATH_ITEM& a = *i++;
		if ( _tcsicmp(a.path, lpPath) == 0 )
		{
			return TRUE;
		}
	}

	return FALSE;
}

/*
BOOL IsVerifyFile(LPCTSTR lpPath)
{
	int nResult = 0;

	HANDLE hHandler = AcquireVerifyHandle();
	if ( hHandler != NULL )
	{
		nResult = VerifyFile(hHandler, lpPath);
		ReleaseVerifyHandle(hHandler);
	}

	return nResult;
}
*/


void CalcMD5(PVOID lpBuff, DWORD dwSize, LPBYTE lpMd5)
{
	struct MD5Context ctx;
	
	MD5Init(&ctx);
	MD5Update(&ctx, (PBYTE)lpBuff, dwSize);
	MD5Final(lpMd5, &ctx);
}

BOOL CalcFileMD5(LPCTSTR lpFilePath, LPBYTE lpMD5)
{
	BOOL bResult = FALSE;
	HANDLE hFile = CreateFile(lpFilePath, 
		GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL,
		OPEN_EXISTING,
		0,
		INVALID_HANDLE_VALUE
		);
	if ( hFile != INVALID_HANDLE_VALUE )
	{
		const DWORD dwBuffLen = 1024;
		DWORD dwReadBytes;
		BYTE* lpBuff = new BYTE[dwBuffLen];
		
		struct MD5Context ctx;
		MD5Init(&ctx);

		while ( ReadFile(hFile, lpBuff, dwBuffLen, &dwReadBytes, NULL) && dwReadBytes != 0 )
		{
			MD5Update(&ctx, (PBYTE)lpBuff, dwReadBytes);
		}

		MD5Final(lpMD5, &ctx);
		CloseHandle(hFile);

		delete lpBuff;
		lpBuff = NULL;

		bResult = TRUE;
	}

	return bResult;
}

BOOL IsPEFile(LPCTSTR lpPath)
{
	BOOL bResult = FALSE;

	HANDLE hFile = CreateFile(lpPath, 
		GENERIC_READ, 
		FILE_SHARE_READ, 
		NULL,
		OPEN_EXISTING, 
		0,
		NULL
		);
	if ( hFile != INVALID_HANDLE_VALUE )
	{
		IMAGE_DOS_HEADER DosHeader;
		IMAGE_NT_HEADERS NtHeader;
		DWORD dwReadBytes;

		if ( ReadFile(hFile, &DosHeader, sizeof (DosHeader), &dwReadBytes, NULL) &&
			dwReadBytes == sizeof (DosHeader) &&
			DosHeader.e_magic == IMAGE_DOS_SIGNATURE &&
			DosHeader.e_lfanew != 0 
			)
		{
			SetFilePointer(hFile, DosHeader.e_lfanew, NULL, FILE_BEGIN);
			if ( ReadFile(hFile, &NtHeader, sizeof (NtHeader), &dwReadBytes, NULL) &&
				dwReadBytes == sizeof (NtHeader) &&
				NtHeader.Signature == IMAGE_NT_SIGNATURE &&
				NtHeader.FileHeader.NumberOfSections != 0 
				)
			{
				bResult = TRUE;
			}
		}

		CloseHandle(hFile);
	}

	return bResult;
}

BOOL IsPE(PVOID lpData, DWORD dwSize)
{
	BOOL bResult = FALSE;
	__try
	{
		PVOID lpBuff_End = (PBYTE)lpData + dwSize;
		PIMAGE_DOS_HEADER lpDosHeader = (PIMAGE_DOS_HEADER)lpData;
		if ( lpDosHeader + 1 < lpBuff_End &&
			lpDosHeader->e_magic == IMAGE_DOS_SIGNATURE
			)
		{
			PIMAGE_NT_HEADERS lpNtHeader = (PIMAGE_NT_HEADERS)((PBYTE)lpDosHeader + lpDosHeader->e_lfanew);
			if ( lpNtHeader + 1 < lpBuff_End &&
				lpNtHeader->Signature == IMAGE_NT_SIGNATURE &&
				lpNtHeader->FileHeader.NumberOfSections != 0 
				)
			{
				bResult = TRUE;
			}
		}
	}
	__except ( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return TRUE;
}

HICON MyGetFileIconHandle(LPCTSTR lpFilePath)
{
	SHFILEINFO sfi = { 0 };

	DWORD_PTR dwRet = SHGetFileInfo(lpFilePath, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES);

	return SUCCEEDED(dwRet) ? sfi.hIcon: NULL;
}

DWORD MyFindProcess(LPCTSTR lpName)
{
	DWORD dwResult = 0;

	HANDLE hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if ( hProcSnap != INVALID_HANDLE_VALUE )
	{
		PROCESSENTRY32 pe = { 0 };
		pe.dwSize = sizeof (pe);

		BOOL bContinue = Process32First(hProcSnap, &pe);
		while ( bContinue )
		{
			if ( _tcsicmp(pe.szExeFile, lpName) == 0 )
			{
				dwResult = pe.th32ProcessID;
				break;
			}

			bContinue = Process32Next(hProcSnap, &pe);
		}

		CloseHandle(hProcSnap);
		hProcSnap = NULL;
	}

	return dwResult;
}

BOOL MyGetModulePath(DWORD dwProcessId, LPTSTR lpPath)
{
	BOOL bResult = FALSE;

	HANDLE hModSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
	if ( hModSnap != INVALID_HANDLE_VALUE )
	{
		MODULEENTRY32 me = { 0 };
		me.dwSize = sizeof (me);

		BOOL bContinue = Module32First(hModSnap, &me);
		if ( bContinue )
		{
			_tcsncpy(lpPath, me.szExePath, MAX_PATH);
			lpPath[MAX_PATH - 1] = 0;

			bResult = TRUE;
		}

		CloseHandle(hModSnap);
		hModSnap = NULL;
	}

	return bResult;
}

ULARGE_INTEGER MyGetProcessUniqID(HANDLE hProcess, DWORD dwProcessID)
{
	FILETIME tmCreatetion, tmExit, tmKernel, tmUser;
	ULARGE_INTEGER r;

	if ( hProcess == NULL )
	{
		hProcess = GetCurrentProcess();
		dwProcessID = GetCurrentProcessId();
	}

	if ( !GetProcessTimes(hProcess, &tmCreatetion, &tmExit, &tmKernel, &tmUser) )
	{
		tmCreatetion.dwHighDateTime = 0;
		tmCreatetion.dwLowDateTime = 0;
	}

	r.LowPart = dwProcessID;
	r.HighPart = tmCreatetion.dwLowDateTime;

	return r;
}

static BOOL GetProcessPath(DWORD dwPid, LPTSTR lpFilePath)
{
	BOOL bResult = FALSE;
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);
	if ( hProcess != NULL )
	{
		if ( 0 != GetModuleFileNameEx(hProcess, NULL, lpFilePath, MAX_PATH) )
		{
			bResult = TRUE;
		}

		CloseHandle(hProcess);
	}

	return bResult;
}

static BOOL CheckWnd(POINT pt, HWND hWnd, DWORD dwPid)
{
	HWND hWnd2 = WindowFromPoint(pt);
	if ( hWnd2 != NULL )
	{
		if ( hWnd2 == hWnd )
		{
			return TRUE;
		}

		hWnd2 = GetAncestor(hWnd2, 2);
		if ( hWnd2 == hWnd )
		{
			return TRUE;
		}

		if ( dwPid != 0 )
		{
			DWORD dwPid2;
			return (GetWindowThreadProcessId(hWnd2, &dwPid2) && dwPid2 == dwPid);
		}
	}

	return FALSE;
}

BOOL IsFullScreen()
{//
	HWND hWnd = GetForegroundWindow();
//	if ( hWnd == NULL )
//	{
//		hWnd = GetActiveWindow();
//	}

	if ( hWnd != NULL && hWnd != GetDesktopWindow() )
	{
		DWORD dwPid;
		TCHAR szFilePath[MAX_PATH];

		GetWindowThreadProcessId(hWnd, &dwPid);
		if ( dwPid >= 4 )
		{
			if ( GetProcessPath(dwPid, szFilePath) )
			{
				LPTSTR lpFileName = PathFindFileName(szFilePath);
				if ( lpFileName != NULL )
				{
					if ( _tcsicmp(lpFileName, _T("explorer.exe")) == 0 ||
						 _tcsicmp(lpFileName, _T("taskmgr.exe")) == 0 ||
						 _tcsicmp(lpFileName, _T("qq.exe")) == 0 ||
						 _tcsicmp(lpFileName, _T("iexplore.exe")) == 0 || 
						 _tcsicmp(lpFileName, _T("crossfire.exe")) == 0
						)
					{
						return FALSE;
					}
				}
			}
		}
		else
		{
			dwPid = 0;
		}

		DWORD cx = GetSystemMetrics(SM_CXSCREEN) - 1;
		DWORD cy = GetSystemMetrics(SM_CYSCREEN) - 1;
		POINT pt = { 0, 0 };

		if ( CheckWnd(pt, hWnd, dwPid) )
		{
			pt.x = cx;
			pt.y = cy;
			if ( CheckWnd(pt, hWnd, dwPid) )
			{
				pt.x = 0;
				pt.y = cy;
				if ( CheckWnd(pt, hWnd, dwPid) )
				{
					pt.x = cx;
					pt.y = 0;
					if ( CheckWnd(pt, hWnd, dwPid) )
					{
						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}


HRESULT MyCoCreateInstance(LPCTSTR lpDll, const CLSID& rclsid, const IID& riid, void** ppv)
{
	HRESULT hr = E_FAIL;

	HMODULE hDll = LoadLibrary(lpDll);
	if ( hDll != NULL )
	{
		HRESULT (WINAPI *pfnDllGetClassObject)(const CLSID& rclsid, const IID& riid, void** ppv);

		(FARPROC&)pfnDllGetClassObject = GetProcAddress(hDll, "DllGetClassObject");
		if ( pfnDllGetClassObject != NULL )
		{
			IClassFactory* lpClassFactory = NULL;

			hr = pfnDllGetClassObject(rclsid, IID_IClassFactory, (PVOID*)&lpClassFactory);
			if ( SUCCEEDED(hr) && lpClassFactory != NULL )
			{
				hr = lpClassFactory->CreateInstance(NULL, riid, ppv);

				lpClassFactory->Release();
				lpClassFactory = NULL;
			}
		}
	}

	return hr;
}