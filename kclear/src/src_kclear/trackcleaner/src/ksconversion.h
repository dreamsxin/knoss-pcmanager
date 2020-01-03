// Copyright (c) 2010 Kingsoft Corporation. All rights reserved.
// Copyright (c) 2010 The KSafe Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*
 *     kconersion.h
 *     purpose:	convert the string from UTF8 to Wide char 
 *
 *
 *
 *
 *
 */

#ifndef KS_CONVERSION_FOR_KSA_2009_07_04
#define KS_CONVERSION_FOR_KSA_2009_07_04
#pragma  once

typedef basic_string<TCHAR, char_traits<TCHAR>, allocator<TCHAR> > tstring;

//------------------------------------------------------------------------------------------
// ×Ö·û×ª»»µæÆ¬Àà
//------------------------------------------------------------------------------------------



class KUTF82W
{
public:
	KUTF82W(const char * pszUtf8Code)
		:m_szBuffer(NULL)
	{
		if (pszUtf8Code != NULL)
		{
			int  nLen = (MultiByteToWideChar(CP_UTF8, 0, pszUtf8Code, -1, NULL, 0) + 1);
			size_t uSize = nLen * 2;

			m_szBuffer = (LPWSTR)new(std::nothrow) char[uSize];
			if (m_szBuffer != NULL)
			{
				memset(m_szBuffer, 0, uSize);
				int nRet = MultiByteToWideChar(CP_UTF8, 0, pszUtf8Code, -1, m_szBuffer, nLen);
			}
		}
	}
	~KUTF82W()
	{
		if (m_szBuffer != NULL)
		{
			delete [] m_szBuffer;
			m_szBuffer = NULL;
		}
	}

	operator LPCWSTR(void) const throw()
	{
		return ( m_szBuffer );
	}

private:
	LPWSTR m_szBuffer;
};

//-----------------------------------------------------------------------------------------------------
class KW2UTF8
{
public:
	KW2UTF8(LPCWSTR lpszWideCode)
		:m_pBuffer(NULL)
	{

		if (lpszWideCode != NULL)
		{
			int nLen = WideCharToMultiByte(CP_UTF8, 0, lpszWideCode, -1, NULL, 0, NULL, NULL) + 1;
			size_t uSize = nLen * 2;
			int nChars = (int)uSize;
			m_pBuffer = new(std::nothrow) char[uSize];
			if (m_pBuffer != NULL)
			{
				memset(m_pBuffer, 0, uSize);
				WideCharToMultiByte(CP_UTF8, 0, lpszWideCode, -1, m_pBuffer, nChars, NULL, NULL);			
			}
		}

	}

	~KW2UTF8()
	{
		if (m_pBuffer != NULL)
		{
			delete [] m_pBuffer;
			m_pBuffer = NULL;
		}
	}


	operator const char *(void) const throw()
	{
		return ( m_pBuffer );
	}

private:
	char * m_pBuffer;

};


#endif