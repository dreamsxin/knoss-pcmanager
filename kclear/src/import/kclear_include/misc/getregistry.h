//////////////////////////////////////////////////////////////////////////
// FileName:	GetRegistry.h
// Author:		Jonathan Xiang
// DateTime:	April 18th, 2008
// Description:
//////////////////////////////////////////////////////////////////////////

#ifndef GET_REGISTRY_H_20080418
#define GET_REGISTRY_H_20080418

#include <windows.h>

#ifdef __cplusplus
extern "C" 
{

	bool GetRegValueType(HKEY hRootKey, const TCHAR* lpszKeyName, const TCHAR* lpszValueName, LPDWORD lpdwType);

	bool GetRegValueLength(HKEY hRootKey, const TCHAR* lpszKeyName, const TCHAR* lpszValueName, LPDWORD lpcbData);

	bool GetRegValueData(HKEY hRootKey, const TCHAR* lpszKeyName, const TCHAR* lpszValueName, LPBYTE lpData, DWORD cbData);

	bool GetRegValueAll(HKEY hRootKey, const TCHAR* lpszKeyName, const TCHAR* lpszValueName, LPDWORD lpdwType, LPBYTE lpData, LPDWORD lpcbData);

};
#endif // __cplusplus

#endif // GET_REGISTRY_H_20080418
