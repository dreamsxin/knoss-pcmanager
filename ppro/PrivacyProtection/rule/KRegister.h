#ifndef __KRegister_h__
#define __KRegister_h__

#include <atlstr.h>

class KRegister
{
private:
	HKEY m_hKey;

public:
	KRegister() : m_hKey(NULL){}
	~KRegister()
	{
		Close();
	}

	void Attach(HKEY hKey)
	{
		Close();
		m_hKey = hKey;
	}

	HKEY Detach(void)
	{
		HKEY hKey = m_hKey;
		m_hKey = NULL;
		return hKey;
	}
	
	BOOL Open(HKEY hRootKey, LPCTSTR szSubKey, BOOL bReadOnly = TRUE)
	{
		BOOL bReturn = FALSE;
		LONG lRetCode;

		if (bReadOnly)
		{
			lRetCode = RegOpenKey(hRootKey, szSubKey, &m_hKey);
		}
		else
		{
			lRetCode = RegCreateKeyEx(hRootKey, szSubKey, 0L, NULL,
				REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, 
				&m_hKey, NULL);
		}

		if (lRetCode != ERROR_SUCCESS)
			goto Exit0;

		bReturn = TRUE;
Exit0:
		return bReturn;
	}

	void Close(void)
	{
		if (m_hKey) RegCloseKey(m_hKey);
		m_hKey = NULL;
	}

	BOOL Read(LPCTSTR szValueName, DWORD& dwValue)
	{
		BOOL bReturn = FALSE;
		LONG lRetCode = 0;

		DWORD dwType;
		DWORD dwSize = sizeof (DWORD);
		DWORD dwDest;

		if (!m_hKey)
			goto Exit0;

		lRetCode = RegQueryValueEx(m_hKey, szValueName, NULL, 
			&dwType, (BYTE *)&dwDest, &dwSize);
		if (lRetCode != ERROR_SUCCESS || dwType != REG_DWORD)
			goto Exit0;

		dwValue = dwDest;

		bReturn = TRUE;

Exit0:
		return bReturn;
	}

	BOOL Read(LPCTSTR szValueName, CString& strValue)
	{
		BOOL bReturn = FALSE;
		LONG lRetCode;
		DWORD dwType;
		DWORD dwSize = 0;

		if (!m_hKey) goto Exit0;

		lRetCode = RegQueryValueEx(m_hKey, szValueName, NULL,
			&dwType, NULL, &dwSize);
		if (dwType != REG_SZ) 
			goto Exit0;
		
		strValue.Empty();
		LPTSTR pBuffer = strValue.GetBuffer(dwSize / 2 + 1);

		lRetCode = RegQueryValueEx(m_hKey, szValueName, NULL,
			&dwType, (BYTE*)pBuffer, &dwSize);
		if (lRetCode != ERROR_SUCCESS)
			goto Exit0;
		pBuffer[dwSize / 2] = 0;

		strValue.ReleaseBuffer();

		bReturn = TRUE;

Exit0:
		return bReturn;
	}

	BOOL Read(LPCTSTR szValueName, BYTE* pBuffer, DWORD& dwSize)
	{
		BOOL bReturn = FALSE;
		LONG lRetCode;
		DWORD dwType;

		if (!m_hKey)
			goto Exit0;

		lRetCode = RegQueryValueEx(m_hKey, szValueName, NULL,
			&dwType, pBuffer, &dwSize);
		if (lRetCode != ERROR_SUCCESS)
			goto Exit0;

		bReturn = TRUE;

Exit0:
		return bReturn;
	}

	BOOL Write(LPCTSTR szValueName, DWORD dwValue)
	{
		BOOL bReturn = FALSE;
		LONG lRetCode;

		if (!m_hKey)
			goto Exit0;

		lRetCode = RegSetValueEx(m_hKey, szValueName, 0L, REG_DWORD,
			(CONST BYTE*)&dwValue, sizeof(DWORD));
		if (lRetCode != ERROR_SUCCESS)
			goto Exit0;

		bReturn = TRUE;

Exit0:
		return bReturn;
	}

	BOOL Write(LPCTSTR szValueName, LPCTSTR szValue)
	{
		BOOL bReturn = FALSE;
		LONG lRetCode;

		if (!m_hKey)
			goto Exit0;

		lRetCode = RegSetValueEx(m_hKey, szValueName, 0L, REG_SZ,
			(CONST BYTE*) szValue, ((int)_tcslen(szValue) + 1) * sizeof(TCHAR));	
		if (lRetCode != ERROR_SUCCESS)
			goto Exit0;

		bReturn = TRUE;

Exit0:
		return bReturn;
	}

	BOOL Write(LPCTSTR szValueName, BYTE* pBuffer, DWORD dwSize)
	{
		BOOL bReturn = FALSE;
		LONG lRetCode;

		if (!m_hKey)
			goto Exit0;

		lRetCode = RegSetValueEx(m_hKey, szValueName, 0L, REG_BINARY,
			pBuffer, dwSize);
		if (lRetCode != ERROR_SUCCESS)
			goto Exit0;

		bReturn = TRUE;

Exit0:
		return bReturn;
	}
};

#endif