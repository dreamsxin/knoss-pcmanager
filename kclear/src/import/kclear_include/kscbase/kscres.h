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

#ifndef KSCRES_INC_
#define KSCRES_INC_

//////////////////////////////////////////////////////////////////////////

#include <string>
#include <map>
#include <GdiPlus.h>
#include "kscmemfile.h"

//////////////////////////////////////////////////////////////////////////

extern HINSTANCE g_hInstance;

class KAppRes
{
public:
	static KAppRes& Instance();

	bool GetString(const std::string& strId, std::wstring& strText);
	const wchar_t* GetString(const std::string& strId);
	bool GetRawDataFromRes(const std::string& strId, void** ppBuffer, unsigned long& dwSize);
	void FreeRawData(void* pBuffer);
	HBITMAP GetImage(const std::string& strId);
	IUnknown* GetImageEx(const std::string& strId);
	Gdiplus::Image* GetImagePlus(const std::string& strId);
	HICON GetIcon(const std::string& strId);
	HICON GetOemIcon(const std::string& strId);
	bool GetXmlData(const std::string& strId, std::string& strXml);
	HFONT GetFont(const std::string& strId);
	COLORREF GetColor(const std::string& strId);

private:
	KAppRes();
	~KAppRes();

    bool PrepareRes();

	bool OpenResPack();
	void CloseResPack();

	bool LoadStringRes();
	bool LoadImageRes();
	bool LoadXmlRes();
	bool LoadFontRes();
	bool LoadColorRes();
    bool LoadStyleRes();

	typedef std::map<std::string, std::string> KResStore;
	typedef std::pair<unsigned long, unsigned long> KResInfo;
	typedef std::map<std::string, KResInfo> KResOffset;
	typedef std::map<std::string, std::wstring> KStringStore;
	typedef std::map<std::string, std::string> KImageStore;
	typedef std::map<std::string, HBITMAP> KImageCache;
	typedef std::map<std::string, IUnknown*> KImageCacheEx;
	typedef std::map<std::string, Gdiplus::Image*> KImageCachePlus;
	typedef std::map<std::string, std::string> KXmlStore;
	typedef std::map<std::string, HFONT> KFontStore;
	typedef std::map<std::string, COLORREF> KColorStore;

	KResOffset m_mapResOffset;
	KStringStore m_mapStringTable;
	KFontStore m_mapFontTable;
	KColorStore m_mapColorTable;
	KImageStore m_mapImageTable;
	KImageCache m_mapImageCache;
	KImageCacheEx m_mapImageExCache;
	KImageCachePlus m_mapImagePlusCache;
	std::wstring m_strResPackPath;
	KXmlStore m_mapXmlTable;
	void* m_pResPackData;

    HANDLE m_hTempRes;
	CMemFile m_memZipRes;
};

//////////////////////////////////////////////////////////////////////////

#endif	// KSCRES_INC_
