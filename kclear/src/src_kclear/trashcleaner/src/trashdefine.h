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


#pragma  once

#define MAX_CONFIG_LENGTH (20480)

enum{
    RC_FAILED_ALLOCMEMORY  = -4,
    RC_FAILED_NOFILE       = -3,
    RC_FAILED_INVALID_FILE = -2,
    RC_FAILED_READFILE     = -1,
    RC_FAILED  = 0,
    RC_SUCCESS = 1
};

#define WM_TRAVERSEFILE         (WM_USER + 2000)
#define WM_TRAVERSEPROCESS      (WM_USER + 2010)
#define WM_TRAVERSEPROCESSEND   (WM_USER + 2011)
#define WM_TRAVERSEFINISHED     (WM_USER + 2020)
#define	WM_OWN_MSGBOX			(WM_USER + 2021)

#define DEF_TIMER_UPDATE_SCANSTATUS     0x1571

#define FILENAME_TRASHCONFIG   TEXT( "trashconfig.dat" )

#define FINDFILETRAVERSE_MAX_PATH  (MAX_PATH * 2)

const LPCTSTR g_pszFilterFiles[] = {
    TEXT("desktop.ini"),
    TEXT("INFO2"),
    TEXT("FP_AX_CAB_INSTALLER.exe"),
    TEXT("swflash.inf"),
    TEXT("dwusplay.dll"),
    TEXT("dwusplay.exe"),
    TEXT("isusweb.dll"),
    TEXT("IDrop.ocx"),
    TEXT("IDropCHS.dll"),
    TEXT("IDropENU.dll"),
    TEXT("index.dat"),
    TEXT("certInStall.dll"),
    TEXT("icbc_gdgetdv.dll"),
    TEXT("ICBC_NetSign.dll"),
    TEXT("InputControl.dll"),
    TEXT("SubmitControl.dll"),
    TEXT("AxSafeControls.inf")

};//文件过滤

static CString g_pszFilterDirs[] = {
    TEXT("%windir%"),
    TEXT("%program%"),
    TEXT("%CSIDL_DESKTOPDIRECTORY%"),
    TEXT("%systemroot%"),
    TEXT("%boot%"),
    TEXT("%recovery%"),
    TEXT("%CSIDL_LOCAL_APPDATA%\\Microsoft"),
    TEXT("%CSIDL_LOCAL_APPDATA%\\Identities"),
    TEXT("%CSIDL_LOCAL_APPDATA%\\VirtualStore"),
    TEXT("%CSIDL_PERSONAL%\\Tencent"),
    TEXT("%CSIDL_PERSONAL%\\Tencent Files")

};//目录过滤

typedef enum _System_Version
{
    enumSystem_Unkonw,
    enumSystem_Win_95,
    enumSystem_Win_98,
    enumSystem_Win_me,
    enumSystem_Win_nt,
    enumSystem_Win_2000,
    enumSystem_Win_xp,
    enumSystem_Win_Ser_2003,
    enumSystem_Win_Vista,
    enumSystem_Win_7

} SYSTEM_VERSION;

enum{
    Delete_File_Success = 0,
    Delete_File_Success_Delay_Reboot = 1,
    Delete_File_Failed = 2
};

enum{
    em_cf_none    = -1,
    em_cf_success = Delete_File_Success,
    em_cf_success_delay_reboot = Delete_File_Success_Delay_Reboot,
    em_cf_failed  = Delete_File_Failed,
};

enum{
	NONE = -1,
	WIN_TRASH=0,
	SYS_TRASH,
	BROWER_TRASH,
	VIDEO_TRASH,
	WIN_TEMP,
// 	WIN_LNK,
// 	WIN_DESKTOPLNK,
	WIN_DUMP,
	WIN_HUISHOUZHAN,
	WIN_UPDATE,
	WIN_PREFETCH,
	WIN_USERTEMP,
	WIN_DOWNLOAD,
	WIN_SUOLIETU,
	WIN_ERRPORT,
	WIN_LOG,
	WIN_OFFICE,
	BROWER_IE,
	BROWER_FIREFOX,
	BROWER_MATHRON,
	BROWER_SOGO,
	BROWER_OPERA,
	BROWER_CHROME,
	VIDEO_TUDOU,
	VIDEO_XUNLEI,
	VIDEO_YOUKU,
	VIDEO_KU6,
	VIDEO_PPTV,
	VIDEO_FENGXING,
	VIDEO_QQLIVE
};



#define IDT_SCANTIMER  1001
#define IDT_SCANDELAY   150
#define LEFT_PADING 50
#define EDGE_PADING 3

#define WM_CHECK_CHANGE     (WM_USER + 0xFF)

enum{
    em_ds_none    = 0,
    em_ds_uncheck = 1,
    em_ds_check   = 2,
    em_ds_scaning = 3
};


#define WM_TRASH_SCAN_STATUS_CHANGED        (WM_USER + 0x1571)
#define CLEAR_STATUS                         5
#define CLEAR_END                            6
#define CLEAR_HIDE                           7
#define UN_CHECK                             8
#define UN_SIZE                              9
#define DLG_INIT                             10
#define IDS_SUB_KEY     TEXT("SOFTWARE\\Kingsoft\\AntiSpySTD")
#define IDS_VALUENAME   TEXT("FileListOptNotify")
#define IDC_CLOSEWINDOW  1000

#define WM_TRASH_TIPS_SHOW_STATUS			(WM_USER + 0x1572) 
#define WM_TRASH_TIPS_SHOW_EVENT			(WM_USER + 0x1573)
#define WM_TRASH_TIPS_CLOSE_STATUS			(WM_USER + 0x1574) 
#define DEF_TIPS_HEIHT						30
#define DEF_TIPS_SPEED						10   // 每次移动的像素
#define DEF_TIPS_SPEED_TIME					50  // 每多少 MS 移动一次

