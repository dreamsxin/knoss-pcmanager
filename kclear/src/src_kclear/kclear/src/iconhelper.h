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

#pragma once

//////////////////////////////////////////////////////////////////////////

class CIconCache
{
public:
    CIconCache()
    {
        m_nLength = 0;
    }

    ~CIconCache()
    {
    }

    class IconInfo 
    {
    public:
        IconInfo(CString strExt, HICON hIcon)
        {
            m_strExt = strExt;
            m_hIcon = DuplicateIcon(NULL, hIcon);
        }

        IconInfo(const IconInfo& iconInfo)
            : m_strExt(iconInfo.m_strExt)
        {
            m_hIcon = DuplicateIcon(NULL, iconInfo.m_hIcon);
        }

        ~IconInfo()
        {
            DestroyIcon(m_hIcon);
        }

        CString m_strExt;
        HICON m_hIcon;
    };

    HICON GetIcon(const CString& strFilePath)
    {
        CString strExt;
        int nFlag1, nFlag2;

        nFlag1 = strFilePath.ReverseFind(_T('\\'));
        nFlag2 = strFilePath.ReverseFind(_T('.'));
        if (nFlag1 != -1 && nFlag2 != -1)
        {
            if (nFlag1 < nFlag2)
                strExt = strFilePath.Right(strFilePath.GetLength() - nFlag2);
        }

        if (-1 == nFlag1 && nFlag1 != -1)
        {
            strExt = strFilePath.Right(strFilePath.GetLength() - nFlag2);
        }

        return GetIcon(strFilePath, strExt);
    }

    HICON GetIcon(const CString& strPath, const CString& strExt)
    {
        HICON hIcon = NULL;
        HICON hOldIcon = NULL;

        if (strExt.CompareNoCase(_T(".exe")) == 0 ||
            strExt.CompareNoCase(_T(".scr")) == 0 ||
            strExt.CompareNoCase(_T(".lnk")) == 0)
        {
            goto clean0;
        }
        
        GetIcon(strExt, hIcon);
        if (hIcon)
            goto clean0;
        
        hOldIcon = GetIconFromFilePath(strPath);
        if (!hOldIcon)
        {
            hIcon = KAppRes::Instance().GetIcon("IDI_UNKNOWN_SMALL");
            goto clean0;
        }
        else
        {
            SaveIcon(strExt, hOldIcon);
            GetIcon(strExt, hIcon);
        }

clean0:
        if (hOldIcon)
        {
            ::DestroyIcon(hOldIcon);
            hOldIcon = NULL;
        }

        return hIcon;
    }

    void SaveIcon(const CString& strExt, HICON hIcon)
    {
        IconInfo iconInfo(strExt, hIcon);
        m_listIcons.push_front(iconInfo);
        if (++m_nLength > 30)
        {
            m_listIcons.pop_back();
            --m_nLength;
        }
    }

    void GetIcon(const CString& strExt, HICON& hIcon)
    {
        std::deque<IconInfo>::iterator i;

        for (i = m_listIcons.begin(); i != m_listIcons.end(); ++i)
        {
            if (i->m_strExt.CompareNoCase(strExt) == 0)
            {
                hIcon = i->m_hIcon;
                break;
            }
        }
    }

    void Clean()
    {
        m_listIcons.clear();
    }

    HICON GetIconFromFilePath(const CString& strFilePath)
    {
        SHFILEINFO fileInfo = { 0 };

        ::SHGetFileInfo(strFilePath, 0, &fileInfo, sizeof(fileInfo), SHGFI_ICON | SHGFI_SMALLICON);

        return fileInfo.hIcon;
    }

private:
    std::deque<IconInfo> m_listIcons;
    int m_nLength;
};

//////////////////////////////////////////////////////////////////////////

