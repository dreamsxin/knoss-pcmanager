// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

#pragma warning(disable: 4995)
#pragma warning(disable: 4996)
#pragma warning(disable: 4819)
#pragma warning(disable: 4018)
#pragma warning(disable: 4244)

#define WINVER          0x0500
#define _WIN32_WINNT    0x0501
#define _WIN32_IE       0x0600
#define _RICHEDIT_VER   0x0200

#define _SECURE_ATL 0

#ifdef _DEBUG

#   include <miniutil/bkconsole.h>

#   define DEBUG_TRACE bkconsole::printf

#else

#   define DEBUG_TRACE

#endif

#include <atlbase.h>
#include <atlwin.h>
#include <atlstr.h>
#include <atltrace.h>

#define _WTL_NO_CSTRING


#include <exdisp.h>
#include <exdispid.h>
#include <mshtml.h>
#include <mshtmdid.h>
#include <math.h>

#include <wtl/atlapp.h>
#include <wtl/atlframe.h>
#include <wtl/atlctrls.h>
#include <wtl/atlctrlx.h>
#include <wtl/atldlgs.h>
#include <wtl/atlcrack.h>
#include <wtl/atlmisc.h>

//extern DWORD g_dwMainThreadId;

#include <tinyxml/tinyxml.h>
#include <bkwin/bklistview.h>

#include <Shlwapi.h>

#if defined _M_IX86
#   pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#   pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#   pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#   pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

