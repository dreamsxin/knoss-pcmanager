//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KLogic.h
//  Creator     :   YangXD(yxdh)
//  Date        :   2010-7-25 10:14:50
//  Comment     :   Interface for the KLogic class.
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef	_K_LOGIC_H_ 
#define	_K_LOGIC_H_ 
#include "KActor.h"


class KLogic : public IActorCallBack
{
public:
	KLogic();
	~KLogic();


	HRESULT				Startup();
	HRESULT				Stop();

	BOOL				GetFileIcon( const TCHAR cszFileName[], HICON& hIcon);


	//////////////////////////////////////////////////////////////////////////
	//  µœ÷IActorCallBack
	virtual void		OnActivate(KActor* pActor);




private:

	KActor				m_ActorStartup;
};
#endif	// #ifndef	_K_LOGIC_H_ 
