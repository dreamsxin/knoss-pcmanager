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

#ifndef _BACKUPDEF_H_
#define _BACKUPDEF_H_
#include <windows.h>
#include <winreg.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#define TYPE		"type"
#define ROOT		"root"
#define SUBKEY		"subkey"
#define VALUE		"value"
#define VALUETYPE	"valuetype"
#define DATA		"data"

#define MAX_VALUE_NAME	1024
#define	MAX_KEY_LENGTH	1024
#define BACKUPDBNAME	L"regbackup"
#define BACKUPHEAD		L"headinfo"
#define BACKUPDATA		L"datainfo"

typedef struct tagRegHkeyName
{
	string strHandle;
	HKEY hKey;
}HKEY_NAME, *PHKE_YNAME;

#define HKEY_NAME_ENTRY(ID) { #ID , ID }

__declspec(selectany) HKEY_NAME g_regHkeyNamelist[] = 
{
	HKEY_NAME_ENTRY(HKEY_CLASSES_ROOT),
	HKEY_NAME_ENTRY(HKEY_CURRENT_CONFIG),
	HKEY_NAME_ENTRY(HKEY_CURRENT_USER),
	HKEY_NAME_ENTRY(HKEY_LOCAL_MACHINE),
	HKEY_NAME_ENTRY(HKEY_PERFORMANCE_DATA),
	HKEY_NAME_ENTRY(HKEY_USERS)
};
#endif