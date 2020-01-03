//////////////////////////////////////////////////////////////////////////
//   File Name: bkresutil.h
// Description: Beike Resource Helper
//     Creator: Zhang Xiaoxuan
//     Version: 2009.5.13 - 1.0 - Create
//////////////////////////////////////////////////////////////////////////

#pragma once

#define BKRES_TYPE _T("bkres")
#define BKRES_PNG_TYPE _T("png")

namespace BkResUtil
{
	static BOOL	LoadResourse(UINT uResID, LPCTSTR lpType, HMODULE& hModule, HRSRC& hRsrc)
	{
#ifdef	KIS_OEM_DISABLE
		hModule = (HINSTANCE)&__ImageBase;
#else
		static HMODULE s_hModule = ::LoadLibrary(_T("kisoem.dll"));


		hModule = s_hModule;
		if (NULL == hModule)
			hModule = (HINSTANCE)&__ImageBase;
#endif


		if (NULL == lpType)
			return (hModule != NULL) ? TRUE : FALSE;


		hRsrc = ::FindResource(hModule, MAKEINTRESOURCE(uResID), lpType);
		if (hRsrc)
			return TRUE;


		hModule = (HINSTANCE)&__ImageBase;
		hRsrc = ::FindResource(hModule, MAKEINTRESOURCE(uResID), lpType);
		if (hRsrc)
			return TRUE;


		return FALSE;
	}

    static BOOL LoadBkXmlResource(UINT uResID, CStringA &strRet)
    {
		HMODULE		hModule = NULL;
		HRSRC		hRsrc	= NULL;


		if (!LoadResourse(uResID, BKRES_TYPE, hModule, hRsrc))
			return FALSE;


        DWORD dwSize = ::SizeofResource(hModule, hRsrc); 
        if (0 == dwSize)
            return FALSE;

        HGLOBAL hGlobal = ::LoadResource(hModule, hRsrc); 
        if (NULL == hGlobal)
            return FALSE;

        LPVOID pBuffer = ::LockResource(hGlobal); 
        if (NULL == pBuffer)
            return FALSE;

        memcpy(strRet.GetBuffer(dwSize + 1), pBuffer, dwSize);
        strRet.ReleaseBuffer(dwSize);

        ::FreeResource(hGlobal);

        return TRUE;
    }

	static HBITMAP	LoadBitmap(UINT uResID)
	{
		HMODULE		hModule	= NULL;
		HRSRC		hRsrc	= NULL;


		if (!LoadResourse(uResID, NULL, hModule, hRsrc))
			return FALSE;


		HBITMAP hbmpRet = ::LoadBitmap(hModule, MAKEINTRESOURCE(uResID));
		if (hbmpRet)
			return hbmpRet;


		hbmpRet = ::LoadBitmap((HMODULE)&__ImageBase, MAKEINTRESOURCE(uResID));
		return hbmpRet;
	}
};