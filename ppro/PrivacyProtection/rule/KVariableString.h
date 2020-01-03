#ifndef __KVariableString_h__
#define __KVariableString_h__

class KVariableString
{
public:
	BOOL SetVariable(LPCTSTR szName, LPCTSTR szString);
	CString GetVariable(LPCTSTR szName);

	BOOL RelpaceVariable(CString& strString);
	
private:
	std::map<CString, CString> m_mapVariable;
};

#endif