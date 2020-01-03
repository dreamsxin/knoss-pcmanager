#ifndef __KRuleImpl_h__
#define __KRuleImpl_h__

#include "KRuleTree.h"
#include "KLocker.h"
#include <sstream>

#define defThreadNum 4

class KRuleImpl
{
public:
	KRuleImpl();
	~KRuleImpl();

	BOOL Init(BOOL bAsync = FALSE);
	void Uninit(void);

	int Match(LPCTSTR szFileName, LPCTSTR szProcName);
	void DbgMatch(KAccessData* pData);
	void DbgPrint(void);

	BOOL AddPrivacyFile(LPCTSTR szPath);
	BOOL AddPrivacyDir(LPCTSTR szDir);
	BOOL RemovePrivacyFile(LPCTSTR szPath);
	BOOL RemovePrivacyDir(LPCTSTR szDir);

	BOOL AddMonitorProcess(LPCTSTR szPath);
	BOOL RemoveMonitorProcess(LPCTSTR szPath);

	BOOL ReloadRule(void);

protected:
	BOOL InitMultiScan(void);
	int AsyncMatch(LPCTSTR szFileName, LPCTSTR szProcName);
	int SyncMatch(LPCTSTR szFileName, LPCTSTR szProcName);

	static DWORD WINAPI ResultThreadFunc(LPVOID p);
	DWORD RealResultFunc(void);

	static DWORD WINAPI MatchThreadFunc(LPVOID p);
	DWORD RealMatchFunc(void);

	void Clear(void);

	void GetUserConfigFile(CStringA& strPath);

private:
	LONG m_lMatching;
	BOOL m_bEnableMatch;
	KAccessData m_accessData;
	KRuleTree* m_pMainRuleTree;
	std::vector<IRule*> m_vecRules;
	std::vector<int> m_vecResults;
	
	BOOL m_bStopMatch;
	BOOL m_bMultiThreadMode;
	HANDLE m_hNotifyProcessEvent;
	HANDLE m_hNotifyResultEvent;
	HANDLE m_hResultThread;
	HANDLE m_hMatchThread[defThreadNum];
	
	KCriticalSesion m_lckDataList;
	std::list<KAccessData*> m_lstDatasList;
	
	KCriticalSesion m_lckResultList;
	std::list<KAccessData*> m_lstResultList;

	std::stringstream m_XmlStreamRule;
	std::stringstream m_XmlStreamCustomRule;
};

#endif