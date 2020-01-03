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

#pragma warning(disable: 4995)
#pragma warning(disable: 4996)
#pragma warning(disable: 4819)

#define WINVER          0x0500
#define _WIN32_WINNT    0x0501
#define _WIN32_IE       0x0601
#define _RICHEDIT_VER   0x0200

#define _SECURE_ATL 0

#include <atlbase.h>
#include <atlwin.h>
#include <atlstr.h>
#include <atltrace.h>
#include <atlfile.h>

#define _WTL_NO_CSTRING


#include <exdisp.h>
#include <exdispid.h>
#include <mshtml.h>
#include <mshtmdid.h>
#include <math.h>
#include <shlwapi.h>
#include <shlobj.h>

#ifndef ASSERT
#ifdef _DEBUG
#define ASSERT assert
#else
#define ASSERT
#endif
#endif
