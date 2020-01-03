#ifndef _NDISINSTALLIMP_H_
#define _NDISINSTALLIMP_H_


//#include "kis/kpfw/kndisinstall_public.h"

class CNdisInstallImp// : public INdisInstall, public KSComRoot<CNdisInstallImp>
{
public:
	CNdisInstallImp();
	~CNdisInstallImp();

	STDMETHODIMP_(BOOL)	InstallNdisDriver(
		LPCTSTR lpcszInfPath,
		ULONG uTimeOut, 
		BOOL* bTimeOut, 
		BOOL* bNeedReboot, 
		BOOL* bCanceled
		);

	STDMETHODIMP_(BOOL) UnInstallNdisDriver(
		ULONG uTimeOut, 
		BOOL* bTimeOut,
		BOOL* bNeedReboot
		);

	//SCOM
	//KS_DEFINE_GETCLSID(CLSID_INDISINSTALL);
	//DECLARE_INDEPENDENT_OBJECT();

	//KSCOM_QUERY_BEGIN
	//KSCOM_QUERY_ENTRY( INdisInstall )
	//KSCOM_QUERY_END

protected:

	static unsigned WINAPI InstallRoutine(void *pParam);

	static unsigned WINAPI UninstallRoutine(void *pParam);	

	typedef struct tagInstallParam {
		WCHAR const *pInfFilePath;
		BOOL* pNeedReboot;
		BOOL* pCancelled;
		CNdisInstallImp* pThis;
		BOOL fRetCode;
	} INSTALL_PARAM, *PINSTALL_PARAM;

	typedef struct tagUinstallParam {
		BOOL* pNeedReboot;
		CNdisInstallImp* pThis;
		BOOL fRetCode;
	} UNINSTALL_PARAM, *PUNINSTALL_PARAM;

	STDMETHODIMP InstallSpecifiedComponent(
		LPTSTR lpszInfFile,
		LPTSTR lpszPnpID,
		const GUID *pguidClass
		);

	BOOL InstallDriver(
		const WCHAR cwszInfFilePath[], 
		BOOL* pNeedReboot, 
		BOOL* pCancelled
		);

	BOOL UnInstallDriver(
		BOOL* pNeedReboot
		);

	void CleanOemInf();

	void CleanDriverFile();

	void CleanSvrReg();
};
#endif