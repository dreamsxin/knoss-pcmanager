/********************************************************************
* CreatedOn: 2007-10-17   11:07
* FileName: KASAntiArpDevC.h
* CreatedBy: lidengwang <lidengwang@kingsoft.net>
* $LastChangedDate$
* $LastChangedRevision$
* $LastChangedBy$
* $HeadURL:  $
* Purpose:
*********************************************************************/

#pragma once

#include "antiarp/AntiArpInterface.h"


class KASAntiArpDevC : 
	public IAntiArpDevC
{
public:

    KASAntiArpDevC(void);
    ~KASAntiArpDevC(void);

	virtual STDMETHODIMP_(BOOL) InitDevC();
	virtual STDMETHODIMP_(VOID) UninitDevC();

	virtual STDMETHODIMP_(BOOL) GetKfwState( __out ARP_STATE *arpState ); 
	virtual STDMETHODIMP_(VOID) SetCallback( __in IAntiArpCallback * pCallback );
	virtual STDMETHODIMP_(BOOL) SetModuleFlag( __in ULONG uFlag ); 
	virtual STDMETHODIMP_(BOOL) SetLogFlag( __in ULONG uFlag );
	virtual STDMETHODIMP_(BOOL) EnableNetwork( __in BOOL bEnable ); 

	virtual STDMETHODIMP_(BOOL) SetArpProtectList( p_mac_ip plist, int ncount );
	virtual STDMETHODIMP_(BOOL) ClearArpProtectList();
	virtual STDMETHODIMP_(BOOL) SetArpBlockFlag( ULONG uFlag );

	virtual STDMETHODIMP_(BOOL) AddIpRules( __in KIP_FILTER_INFO* pIpFilterInfo, __in ULONG dwFilterCount );
	virtual STDMETHODIMP_(BOOL) RemoveIpRule( __in GUID ruleId );
	virtual STDMETHODIMP_(BOOL) RemoveAllIpRules();

	virtual STDMETHODIMP_(BOOL) AddAdvFilter( __in KPackTemplate *pPackTemplate, ULONG uTemplateLen );
	virtual STDMETHODIMP_(BOOL) RemoveAdvFilter( GUID ruleId );
	virtual STDMETHODIMP_(BOOL) RemoveAllAdvFilter();

	virtual STDMETHODIMP_(int) GetAdapter( __out ADAPTER_INFO* pAdpaterList, __in int nCount );
	virtual STDMETHODIMP_(BOOL) SetNotifyTimer( __in ULONG uTimer );
	virtual STDMETHODIMP_(BOOL) SetPassGuid( __in GUID guidPass );
	
	virtual STDMETHODIMP_(int) GetRasConnection( __out PRAS_CONNECTION pRasConnection, __in int nCount );

    virtual STDMETHODIMP_(BOOL) SelectGatewayMac();
    virtual STDMETHODIMP_(BOOL) CleanGatewayMac();

	KAS_BEGIN_COM_MAP( KASAntiArpDevC )
	KAS_COM_INTERFACE_ENTRY( IAntiArpDevC )
	KAS_END_COM_MAP()


protected:
	static DWORD WINAPI ThreadLogIPFunc(LPVOID lpParam);
    BOOL GetCommonEvent( PVOID pBuffer, ULONG uBufferSize );
	void RunGetLogFunc();
    
	BOOL RegisterEvent( p_antiarp_config_event p_event );
protected:
    HANDLE            m_hDevcHandle;
    IAntiArpCallback* m_piCallback;
    antiarp_config_event m_events;
    HANDLE m_hThread;
    HANDLE m_hExitEvent;
};



