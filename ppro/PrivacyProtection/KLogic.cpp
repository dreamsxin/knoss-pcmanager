#include "stdafx.h"
#include "KLogic.h"
#include "KAVPublic.h"


KLogic::KLogic()
{
}

KLogic::~KLogic()
{

}

HRESULT KLogic::Startup()
{
	m_ActorStartup.Startup(this);
	return S_OK;
}

HRESULT KLogic::Stop()
{
	return S_OK;
}

void KLogic::OnActivate(KActor* pActor)
{
	if (m_ActorStartup.IsActive())
	{
	}
}

BOOL KLogic::GetFileIcon( const TCHAR cszFileName[], HICON& hIcon)
{
	DWORD dwRet = 0;
	BOOL bRet = FALSE;
	::CoInitialize(NULL);
	SHFILEINFO fileInfo = {0};
	dwRet = ::SHGetFileInfo( cszFileName, 0, &fileInfo, sizeof(fileInfo), SHGFI_ICON | SHGFI_SMALLICON );
	if ( dwRet )
	{
		if (fileInfo.hIcon)
			hIcon = fileInfo.hIcon;
		bRet = TRUE;
	}
	else
		bRet = FALSE;

	::CoUninitialize();
	return bRet;
} 