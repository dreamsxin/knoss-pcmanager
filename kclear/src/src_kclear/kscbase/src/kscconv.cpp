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

#include "kscconv.h"

//////////////////////////////////////////////////////////////////////////

std::wstring AnsiToUnicode(const std::string& strAnsi) 
{
	std::wstring retval;

	if (strAnsi.size())
		retval = KANSI_TO_UTF16(strAnsi.c_str());

	return retval;
}

std::string UnicodeToAnsi(const std::wstring& strUnicode) 
{
	std::string retval;

	if (strUnicode.size())
		retval = KUTF16_To_ANSI(strUnicode.c_str());

	return retval;
}

std::string UnicodeToUtf8(const std::wstring& strUnicode) 
{
	std::string retval;

	if (strUnicode.size())
		retval = KUTF16_To_UTF8(strUnicode.c_str());

	return retval;
}

std::wstring Utf8ToUnicode(const std::string& strUtf8) 
{
	std::wstring retval;

	if (strUtf8.size())
		retval = KUTF8_To_UTF16(strUtf8.c_str());

	return retval;
}

std::string Utf8ToAnsi(const std::string& strUtf8) 
{
	std::string retval;
	std::wstring strTemp;

	strTemp = Utf8ToUnicode(strUtf8);
	retval = UnicodeToAnsi(strTemp);

	return retval;
}

std::string AnsiToUtf8(const std::string& strAnsi) 
{
	std::string retval;
	std::wstring strTemp;

	strTemp = AnsiToUnicode(strAnsi);
	retval = UnicodeToUtf8(strTemp);

	return retval;
}

//////////////////////////////////////////////////////////////////////////
