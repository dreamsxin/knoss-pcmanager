// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once
#pragma message("=======================\r\nChecking Compiler error...\r\n=======================\r\n")
#pragma warning(error: 4390 4129 4553 4067 4706)

// Change these values to use different versions
#define WINVER			0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE		0x0600
#define _RICHEDIT_VER	0x0200

#include "buildcfg/buildcfg.h"		

#include <atlbase.h>
#include <atlwin.h>
#include <atlstr.h>
#include <atltrace.h>

#define _WTL_NO_CSTRING


#include <wtl/atlapp.h>
#include <wtl/atlframe.h>
#include <wtl/atlctrls.h>
#include <wtl/atlctrlx.h>
#include <wtl/atldlgs.h>
#include <wtl/atlcrack.h>
#include <wtl/atlmisc.h>

#include <tinyxml/tinyxml.h>
#include <bkwin/bklistview.h>


#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <atlstr.h>
#include <vector>
#include <map>
#include <list>
#include <string>

#include "log_file.h"

#define  PRINT_RULE_RESULT

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
