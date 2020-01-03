#ifndef _K_AUTO_SYNC_H_
#define _K_AUTO_SYNC_H_

#ifdef WIN32
#include <windows.h>

class KAutoSync
{
public:
	
	KAutoSync(CRITICAL_SECTION* pCriticalSection)
	{
		if(!pCriticalSection)
			return;

        m_pCriticalSection = pCriticalSection;
		EnterCriticalSection(m_pCriticalSection);
	}

	~KAutoSync()
	{
		LeaveCriticalSection(m_pCriticalSection);
	}

private:
	CRITICAL_SECTION*	m_pCriticalSection;
};
#endif


#endif		// #ifndef _K_AUTO_SYNC_H_