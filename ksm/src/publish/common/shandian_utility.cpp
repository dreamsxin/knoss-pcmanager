#include "shandian_utility.h"
#include <vulfix/BeikeUtils.h>

LPCTSTR CShandianPath::GetProductVersion()
{
	static TCHAR szVersion[MAX_PATH] = {0};
	static BOOL bInited = FALSE;	

	if ( !bInited )
	{
		bInited = TRUE;

		CString strVersion;
		CString strFilePath;

		CShandianPath::Instance().GetProductAppPath(strFilePath);
		strFilePath += _T("\\");
		strFilePath += BKPLAT__DLL_VER;

		LARGE_INTEGER ll;
		if ( _GetFileVersion(strFilePath, ll) )
		{
			strVersion.Format(_T("%u.%u.%u.%u"), HIWORD(ll.HighPart), LOWORD(ll.HighPart), HIWORD(ll.LowPart), LOWORD(ll.LowPart) );
			_tcsncpy(szVersion, strVersion, MAX_PATH);
			szVersion[MAX_PATH - 1] = 0;
		}
	}
	return szVersion;
}

BOOL CShandianPath::_GetFileVersion( LPCTSTR szPath, LARGE_INTEGER &lgVersion )
{
	if( szPath == NULL)
		return FALSE;

	DWORD dwHandle;
	UINT  cb;
	cb = GetFileVersionInfoSize( szPath, &dwHandle );
	if (cb > 0)
	{
		BYTE* pFileVersionBuffer = new BYTE[cb];
		if( pFileVersionBuffer == NULL )
			return FALSE;

		if (GetFileVersionInfo( szPath, 0, cb, pFileVersionBuffer))
		{
			VS_FIXEDFILEINFO* pVersion = NULL;
			if (VerQueryValue(pFileVersionBuffer, TEXT("\\"), (VOID**)&pVersion, &cb) && 
				pVersion != NULL) 
			{
				lgVersion.HighPart = pVersion->dwFileVersionMS;
				lgVersion.LowPart  = pVersion->dwFileVersionLS;
				delete[] pFileVersionBuffer;
				return TRUE;
			}
		}

		delete[] pFileVersionBuffer;
	}
	return FALSE;
}