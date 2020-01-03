#pragma once

#pragma warning(disable : 4786)
#include <iostream>
#include <tchar.h>
#include <windows.h>
#include <string>
#include <shlwapi.h>
#include <ShlObj.h>
#include <vector>
#include "kuc/kxeuc.h"
#pragma comment(lib, "shlwapi.lib")

//////////////////////////////////////////////////////////////////////////

#define WCHAR_TO_UPPER towupper

#define IS_SLASH(cChar) ((cChar) == L'\\' || (cChar) == L'/')

#define FNM_NOESCAPE 1
#define FNM_PATHNAME 2
#define FNM_FILE_NAME 2
#define FNM_PERIOD 4
#define FNM_LEADING_DIR 8
#define FNM_CASEFOLD 16

#define FNM_NOMATCH FALSE
#define FNM_MATCH   TRUE
#define EOS 0
#define TOLOWER Mytowlower

class KRtl
{
public:

    const WCHAR *FindSlash(
        const WCHAR *psString)
    {
        while (*psString)
        {
            if (IS_SLASH(*psString))
                return psString;

            psString++;
        }

        return 0;
    }

    static wint_t __cdecl Mytowlower (
        wint_t c
        )
    {
        if ( (c >= L'A') && (c <= L'Z') )
        {c = c + (L'a' - L'A');}
        return c;
    }

    static int _StringWildCard(const WCHAR * pattern, const WCHAR * string, int flags)
    {
        register const WCHAR *p = pattern, *n = string;
        register WCHAR c;

#define FOLD(c)	((flags & FNM_CASEFOLD) ? TOLOWER (c) : (c))

        while ((c = *p++) != L'\0')
        {
            c = FOLD (c);

            switch (c)
            {
            case L'?':
                if (*n == L'\0')
                    return FNM_NOMATCH;
                else if ((flags & FNM_FILE_NAME) && *n == L'/')
                    return FNM_NOMATCH;
                else if ((flags & FNM_PERIOD) && *n == L'.' && (n == string || ((flags & FNM_FILE_NAME) && n[-1] == L'/')))
                    return FNM_NOMATCH;
                break;

            case L'\\':
                if (!(flags & FNM_NOESCAPE))
                {
                    c = *p++;
                    c = FOLD (c);
                }
                if (FOLD ((WCHAR)*n) != c)
                    return FNM_NOMATCH;
                break;

            case L'*':
                if ((flags & FNM_PERIOD) && *n == L'.' && (n == string || ((flags & FNM_FILE_NAME) && n[-1] == L'/')))
                    return FNM_NOMATCH;

                for (c = *p++; c == L'?' || c == L'*'; c = *p++, ++n)
                    if (((flags & FNM_FILE_NAME) && *n == L'/') || (c == L'?' && *n == L'\0'))
                        return FNM_NOMATCH;

                if (c == L'\0')
                    return FNM_MATCH;

                {
                    WCHAR c1 = (!(flags & FNM_NOESCAPE) && c == L'\\') ? *p : c;
                    c1 = FOLD (c1);
                    for (--p; *n != L'\0'; ++n)
                    {
                        if ((c == L'[' || FOLD ((WCHAR)*n) == c1) && _StringWildCard (p, n, flags & ~FNM_PERIOD) == FNM_MATCH)
                            return FNM_MATCH;
                    }

                    return FNM_NOMATCH;
                }
            case L'[':
                {
                    /* Nonzero if the sense of the character class is inverted.  */
                    register int not;

                    if (*n == L'\0')
                        return FNM_NOMATCH;

                    if ((flags & FNM_PERIOD) && *n == L'.' &&
                        (n == string || ((flags & FNM_FILE_NAME) && n[-1] == L'/')))
                        return FNM_NOMATCH;

                    not = (*p == L'!' || *p == L'^');
                    if (not)
                        ++p;

                    c = *p++;
                    for (;;)
                    {
                        register WCHAR cstart = c, cend = c;

                        if (!(flags & FNM_NOESCAPE) && c == L'\\')
                            cstart = cend = *p++;

                        cstart = cend = FOLD (cstart);

                        if (c == L'\0')
                            /* [ (unterminated) loses.  */
                            return FNM_NOMATCH;

                        c = *p++;
                        c = FOLD (c);

                        if ((flags & FNM_FILE_NAME) && c == L'/')
                            /* [/] can never match.  */
                            return FNM_NOMATCH;

                        if (c == L'-' && *p != L']')
                        {
                            cend = *p++;
                            if (!(flags & FNM_NOESCAPE) && cend == L'\\')
                                cend = *p++;
                            if (cend == L'\0')
                                return FNM_NOMATCH;
                            cend = FOLD (cend);

                            c = *p++;
                        }

                        if (FOLD ((WCHAR)*n) >= cstart && FOLD ((WCHAR)*n) <= cend)
                            goto matched;

                        if (c == L']')
                            break;
                    }
                    if (!not)
                        return FNM_NOMATCH;
                    break;

matched:;
                    /* Skip the rest of the [...] that already matched.  */
                    while (c != L']')
                    {
                        if (c == L'\0')
                            /* [... (unterminated) loses.  */
                            return FNM_NOMATCH;

                        c = *p++;
                        if (!(flags & FNM_NOESCAPE) && c == L'\\')
                            /* XXX 1003.2d11 is unclear if this is right.  */
                            ++p;
                    }
                    if (not)
                        return FNM_NOMATCH;
                }
                break;

            default:
                if (c != FOLD ((WCHAR)*n))
                    return FNM_NOMATCH;
            }

            ++n;
        }

        if (*n == L'\0')
            return FNM_MATCH;

        if ((flags & FNM_LEADING_DIR) && *n == L'/')
            /* The FNM_LEADING_DIR flag says that "foo*" matches "foobar/frobozz".  */
            return FNM_MATCH;

        return FNM_NOMATCH;
    }

    static BOOL StringWildCard(
        const WCHAR *psPattern, 
        const WCHAR *psString)
    {
        return _StringWildCard(psPattern, psString, FNM_NOESCAPE | FNM_CASEFOLD);
    }

    static void Replace(std::wstring& str, std::wstring oldstr, std::wstring newstr)
    {
        int pos = std::wstring::npos;
        int start_index=0;
        int old_str_len=oldstr.size(), new_str_len = new_str_len=newstr.size();
        while((pos=str.find(oldstr,start_index)) != std::wstring::npos)
        {
            str.replace(pos,old_str_len,newstr);
            start_index=pos+new_str_len;
        }

    }

protected:
private:
};
