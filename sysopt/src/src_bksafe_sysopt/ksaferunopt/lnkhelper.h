#pragma once

#include <shlobj.h>

class CLnkDecode
{
public:
	static BOOL IsLnkPostFile(LPCTSTR lpFileName)
	{
		LPCTSTR		lpPostFix = GetFilePostFix(lpFileName);

		if (lpPostFix==NULL)
			return FALSE;

		return (_tcsicmp(lpPostFix,_T("lnk"))==0);
	}
	static BOOL QueryLnk(LPCTSTR lpLinkPath,CString & strExePath,CString &strExeParam)
	{
		IShellLink*	pShellLink;
		LPCTSTR		lpPostFix = GetFilePostFix(lpLinkPath);

		BOOL bReturn = (::CoInitialize(NULL) == S_OK);
		if(TRUE || bReturn)
		{
			bReturn = ::CoCreateInstance (CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
				IID_IShellLink, (void **)&pShellLink) >= 0;
			if(bReturn)
			{
				IPersistFile *ppf;
				bReturn = pShellLink->QueryInterface(IID_IPersistFile, (void **)&ppf) >= 0;
				if(bReturn)
				{
					bReturn = ppf->Load(lpLinkPath, TRUE) >= 0;
					if(bReturn)
					{
						CString		strIePath;
						CString		strParam;
						pShellLink->GetPath( strIePath.GetBuffer(MAX_PATH+1), MAX_PATH, NULL, SLGP_UNCPRIORITY );
						pShellLink->GetArguments( strParam.GetBuffer(MAX_PATH + 1),MAX_PATH);
						strIePath.ReleaseBuffer();
						strParam.ReleaseBuffer();
						
						if (!strIePath.IsEmpty())
						{
							strExePath = (LPCTSTR)strIePath;
							
							if (!strParam.IsEmpty())
								strExeParam = strParam;

							bReturn = TRUE;
						}
						else
							bReturn = FALSE;
					}
					ppf->Release();
				}
				pShellLink->Release();
			}
			::CoUninitialize();
		}

		return bReturn;
	}

protected:
	static LPCTSTR GetFilePostFix(LPCTSTR lpFile)
	{
		LPCTSTR lpPostFix = _tcsrchr(lpFile, _T('.'));
		if ( lpPostFix != NULL )
			lpPostFix++;
		return lpPostFix;
	}
};