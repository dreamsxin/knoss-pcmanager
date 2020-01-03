#ifndef FWINSTALLER_INC_
#define FWINSTALLER_INC_

#include "antiarp/AntiArpInterface.h"

#define NDIS_ANTIARP_DRIVER       L"KNdisFlt"
//////////////////////////////////////////////////////////////////////////

class KArpInstaller : 
	public IArpInstaller
{
public:
    KArpInstaller();
    virtual ~KArpInstaller();

    STDMETHODIMP Install();
    STDMETHODIMP UnInstall();
	STDMETHODIMP SetConfig( DWORD dwEnableFlag, DWORD dwRequestFlag );
	STDMETHODIMP IsRuning( BOOL* bRuning );

	KAS_BEGIN_COM_MAP( KArpInstaller )
	KAS_COM_INTERFACE_ENTRY( IArpInstaller )
	KAS_END_COM_MAP()
};

//////////////////////////////////////////////////////////////////////////


#endif  // !FWINSTALLER_INC_

