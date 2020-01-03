/************************************************************************
* @file      : tool.h
* @author    : WangBin5 <WangBin5.com>
* @date      : 26/11/2009 AM 11:53:11
* @brief     : 通用跨平台的IPC所使用的各种工具函数
*
* $Id: $
/************************************************************************/

//////////////////////////////////////////////////////////////////////////

#ifndef __COM_TOOL_H__
#define __COM_TOOL_H__

#include <string.h>
#include <MEMORY.h>
#include <windows.h>
#include "ucipc_def.h"
#include <sddl.h>

#define IPC_Min(a,b) (((a) < (b)) ? (a) : (b))

INT KWcslen(WCHAR* str);

INT KWcscpySafe( WCHAR* wcDest, INT numElement, WCHAR *wcSrc);

INT KWcsncpySafe(
	WCHAR* wcDest, 
	INT numElement,
	WCHAR *wcSrc,
	INT count
	);

INT KWcscatSafe( WCHAR* wcDest, INT numElement, WCHAR* wcSrc);

#define __IS_WINNT(ver)             ((ver) < (DWORD)0x80000000)
#define __GET_MAJOR_VERSION(ver)    ((DWORD)(LOBYTE(LOWORD(ver))))

#define _IS_WINNT()                 (__IS_WINNT(GetVersion()))

__inline int is_WinNT(void)
{   return _IS_WINNT(); }

__inline PSECURITY_ATTRIBUTES KAVCreateSecurityAttribute(void)
{
    BOOL  bRes = FALSE;
    BOOL  bRet = FALSE;
	
    PSECURITY_ATTRIBUTES psa = NULL;
    PSECURITY_DESCRIPTOR psd = NULL;
	
    if (is_WinNT())
    {
        psa  = (PSECURITY_ATTRIBUTES)malloc(sizeof(SECURITY_ATTRIBUTES));
        if (!psa)   goto cleanup;
		
        psa->nLength = sizeof(SECURITY_ATTRIBUTES);
        psa->lpSecurityDescriptor = NULL;
        psa->bInheritHandle = TRUE;
		
		/*    psd  = (PSECURITY_DESCRIPTOR)malloc(SECURITY_DESCRIPTOR_MIN_LENGTH);
        if (!psd)   goto cleanup;
		
		  bRet = InitializeSecurityDescriptor(psd, SECURITY_DESCRIPTOR_REVISION);
        if (!bRet)  goto cleanup;*/
		
#if (_WIN32_WINNT >= 0x0501)    // Windows 2000 or later supported API
        // DACL for your application. 
        const TCHAR string_sd[] = 
            TEXT("D:")                  // Discretionary ACL
            TEXT("(D;OICI;GA;;;BG)")    // Deny access to Built-in Guests
            TEXT("(D;OICI;GA;;;AN)")    // Deny access to Anonymous Logon
            TEXT("(A;OICI;GA;;;AU)")    // Allow full control to Authenticated Users
            TEXT("(A;OICI;GA;;;BA)");   // Allow full control to Administrators
		
        ULONG size;
        bRet = ConvertStringSecurityDescriptorToSecurityDescriptor(
            string_sd,
            SDDL_REVISION_1,
            &(psa->lpSecurityDescriptor),
            &size
			);
        if (!bRet)  goto cleanup;
		
#else //_WIN32_WINNT            // All Windows NT OS supported API
        // Set DACL to Security Descriptor
        //bRet = SetSecurityDescriptorDacl(
        //    psd, 
        //    TRUE, NULL,                 // Allowing all access to the object 
        //    FALSE
        //);
        //if (!bRet)  goto cleanup;
		
#endif//_WIN32_WINNT
		
        psa->nLength = sizeof(SECURITY_ATTRIBUTES);
        psa->bInheritHandle = TRUE;
        //psa->lpSecurityDescriptor = psd;
    }
	
    bRes = TRUE;
	
cleanup:
	
    if (!bRes)
    {
		if (psa)
		{
			if (psa->lpSecurityDescriptor) 
			{
				LocalFree(psa->lpSecurityDescriptor);  
				psa->lpSecurityDescriptor = NULL; 
			}

			free(psa);  
			psa = NULL;
		}
    }
	
    return psa;
}
__inline int KAVFreeSecurityAttribute(PSECURITY_ATTRIBUTES psa)
{
    if (psa)
    {
        LocalFree(psa->lpSecurityDescriptor);
        free(psa);
    }
    return TRUE;
}


#endif