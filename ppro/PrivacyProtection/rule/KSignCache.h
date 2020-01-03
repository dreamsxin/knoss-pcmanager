#ifndef __KSignCache_h__
#define __KSignCache_h__

class KSignCache
{
public:
	void AddFileSign(LPCTSTR szPath, LPCSTR szSign);
	LPCSTR GetFileSign(LPCTSTR szPath, int nPathLen);
	void Clear();

private:
	std::map<CString, CStringA> m_vecSignCache;
	std::vector<CString> m_vecNoSignFileList;
};

extern KSignCache g_signCache;

#endif