//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KActor.h
//  Creator     :   YangXD(yxdh)
//  Date        :   2010-7-25 17:30:50
//  Comment     :   Interface for the KActor class.
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _K_ACTOR_H_
#define _K_ACTOR_H_
#include <wtypes.h>


class KActor;


class IActorCallBack
{
public:
	virtual void						OnActivate(KActor* pActor) = 0;
};


class KActor
{
public:
	KActor();
	~KActor();

	HRESULT								Startup(IActorCallBack* pCallBack);
	HRESULT								Stop();

	bool								IsActive();
	bool								WaitForStop(DWORD dwMilliseconds);

protected:
	HRESULT								Activate();
	HRESULT								DeActivate();

	static unsigned __stdcall 			OnActivate(void* lParam);


private:
	HANDLE								m_pThread;
	unsigned							m_uThreadID;
	HANDLE								m_hStopEvent;

	IActorCallBack*						m_pCallBack;
};


#endif	// #ifndef _K_ACTOR_H_