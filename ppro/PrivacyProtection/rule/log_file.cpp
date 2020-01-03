#include "stdafx.h"
#include "log_file.h"
#include <atlstr.h>

#ifdef __log_file__

static DWORD __GetFileSize(LPCSTR szPathName)
{
	DWORD dwFileSize = 0;
	HANDLE hFile = ::CreateFileA(
		szPathName, 
		GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	dwFileSize = ::GetFileSize(hFile, NULL);
	if (dwFileSize == INVALID_FILE_SIZE)
	{
		dwFileSize = 0;
	}

	CloseHandle(hFile);

	return dwFileSize;
}

static BOOL __MoveFile_Replace(LPCSTR szDstPathName, LPCSTR szSrcPathName)
{
	::SetFileAttributesA(szDstPathName, 0);
	::DeleteFileA(szDstPathName);
	::SetFileAttributesA(szSrcPathName, 0);

	return ::MoveFileA(szSrcPathName, szDstPathName);
}

static void __CheckLogFileSize(LPCSTR szFileName)
{
	const DWORD dwMaxLogFileSize = 256 * 1024;

	DWORD dwFileSize = __GetFileSize(szFileName);
	if (dwFileSize >= dwMaxLogFileSize)
	{
		CStringA strFileName(szFileName);
		strFileName += "0";
		__MoveFile_Replace(strFileName, szFileName);
	}
}

class __KCriticalSesion
{
public:
	__KCriticalSesion();
	virtual ~__KCriticalSesion();

	virtual void Lock(void);
	virtual void Unlock(void);

private:
	CRITICAL_SECTION m_sesion;
};

class __KLocker
{
public:
	__KLocker(__KCriticalSesion* pISession);
	~__KLocker();

private:
	__KCriticalSesion* m_pISession;
};

class __KBuffer
{
public:
	__KBuffer(size_t size = 0);
	~__KBuffer();
	
	void alloc(size_t size);
	inline void* buf(void);
	inline size_t size(void);

private:
	void* m_p_buf;
	size_t m_size;
};

class __KLogFileName
{
public:
	__KLogFileName();
	~__KLogFileName();

	operator const char*(){return m_name;}
private:
	CStringA m_name;	
};

__KLogFileName::__KLogFileName()
{
	char szFileName[MAX_PATH];
	if (::GetModuleFileNameA(NULL, szFileName, MAX_PATH))
	{
		CStringA strTemp;
		strTemp = szFileName;
		strTemp.Replace("/", "\\");
		int nPos = strTemp.ReverseFind('\\');
		if (nPos != -1)
		{
			m_name = strTemp.Left(nPos + 1);
			m_name += "rule.log";
		}
		else
		{
			m_name = strTemp + ".log";
		}
	}
	else
	{
		m_name = ".\\unknown_module.log";
	}
	__CheckLogFileSize(m_name);
}

__KLogFileName::~__KLogFileName()
{
}

__KBuffer::__KBuffer(size_t size)
{
	m_p_buf = NULL;
	if (size) m_p_buf = malloc(size);
	m_size = size;
	if (!m_p_buf) m_size = 0;
}

__KBuffer::~__KBuffer()
{
	if (m_p_buf) free(m_p_buf);
	m_p_buf = NULL;
	m_size = 0;
}

inline void __KBuffer::alloc(size_t size)
{
	if (m_p_buf) free(m_p_buf);
	m_p_buf = malloc(size);
	m_size = size;
	if (!m_p_buf) m_size = 0;
}

inline void* __KBuffer::buf()
{
	return m_p_buf;
}

size_t __KBuffer::size()
{
	return m_size;
}

__KCriticalSesion::__KCriticalSesion()
{
	::InitializeCriticalSection(&m_sesion);
}

__KCriticalSesion::~__KCriticalSesion()
{
	::DeleteCriticalSection(&m_sesion);
}

void __KCriticalSesion::Lock()
{
	::EnterCriticalSection(&m_sesion);
}

void __KCriticalSesion::Unlock()
{
	::LeaveCriticalSection(&m_sesion);
}

__KLocker::__KLocker(__KCriticalSesion* pISession)
{
	m_pISession = pISession;
	if (m_pISession)
	{
		m_pISession->Lock();
	}
}

__KLocker::~__KLocker()
{
	if (m_pISession)
	{
		m_pISession->Unlock();
	}
}

static __KCriticalSesion	g_log_lock;
static __KBuffer			g_log_buf;
static __KLogFileName		g_log_file_name;
static const size_t			g_log_buf_size = 50 * 1024;

void __buf_to_file(const char* buf, const char* file, BOOL addtime = TRUE)
{
	FILE* fp = NULL;
	size_t writen = 0;

	fp = fopen(file, "a");
	if (!fp) return;
	
	if (addtime)
	{
		SYSTEMTIME time;
		CStringA strTime;

		::GetLocalTime(&time);
		strTime.Format("%04d-%02d-%02d %02d:%02d:%02d ",
			time.wYear,
			time.wMonth,
			time.wDay,
			time.wHour,
			time.wMinute,
			time.wSecond);
		fwrite((LPCSTR)strTime, 1, strTime.GetLength(), fp);
		//fprintf(fp, strTime);
	}
	
	fwrite(buf, 1, strlen(buf), fp);
	//fprintf(fp, buf);
	fclose(fp);
}


int __cdecl __log_to_fileA(const char* format, ...)
{
	int n_ret = 0;

	va_list arglist;
	char* p_buf;

	if (!format) return 0;
	
	__KLocker locker(&g_log_lock);

	if (!g_log_buf.size()) g_log_buf.alloc(g_log_buf_size);
	if (!g_log_buf.size()) return 0;

	va_start(arglist, format);

	p_buf = (char*)g_log_buf.buf(); 
	n_ret = _vsnprintf(p_buf, g_log_buf.size() - 1, format, arglist);
	p_buf[g_log_buf.size() - 1] = 0;
	__buf_to_file(p_buf, g_log_file_name);

	va_end(arglist);

	return n_ret;
}

int __cdecl __log_to_fileW(const wchar_t* format, ...)
{
	int n_ret = 0;

	va_list arglist;
	wchar_t* p_buf;
	char*	 p_buf_a;

	if (!format) return 0;

	__KLocker locker(&g_log_lock);

	if (!g_log_buf.size()) g_log_buf.alloc(g_log_buf_size);
	if (!g_log_buf.size()) return 0;
	
	va_start(arglist, format);

	p_buf = (wchar_t*)g_log_buf.buf(); 
	n_ret = _vsnwprintf(p_buf, g_log_buf.size()/2 - 2, format, arglist);
	p_buf[n_ret] = 0;
	
	USES_CONVERSION;
	p_buf_a = W2A(p_buf);
	
	__buf_to_file(p_buf_a, g_log_file_name);

	va_end(arglist);

	return n_ret;
}

#endif	//#ifdef __log_file__