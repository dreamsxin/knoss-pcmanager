#ifndef __KFunction_h__
#define __KFunction_h__

class KFunction
{
public:
	static int GetDirCount(LPCTSTR szPath);
	static int GetFileNameCount(LPCTSTR szPath);
	static int GetFileExtCount(LPCTSTR szPath);
	static void PathAddBackslash(CString& strPath);
	static CString GetModulePath(HMODULE hModule);
	static LONGLONG GetFileSize(LPCTSTR szPath);
	static BOOL IsWildcardString(CString& strString);
};

#endif