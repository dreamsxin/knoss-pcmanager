#ifndef __KLocker_h__
#define __KLocker_h__

class KCriticalSesion
{
public:
	KCriticalSesion(){ ::InitializeCriticalSection(&m_sesion); }
	~KCriticalSesion(){ ::DeleteCriticalSection(&m_sesion); }

	void Lock(void){ ::EnterCriticalSection(&m_sesion); }
	void Unlock(void){ ::LeaveCriticalSection(&m_sesion); };

private:
	CRITICAL_SECTION m_sesion;
};

class KLocker
{
public:
	KLocker(KCriticalSesion& session) : m_session(session) { m_session.Lock(); }
	~KLocker(){ m_session.Unlock(); }
private:
	KCriticalSesion& m_session;
};


#endif