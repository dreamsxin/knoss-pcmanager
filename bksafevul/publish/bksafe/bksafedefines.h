#pragma once

#define BKPLAT__MAIN_FILE_NAME          _T("knsd.exe")
#define BKPLAT__TRAY_FILE_NAME          _T("knsdtray.exe")
#define BKPLAT__SERVICE_FILE_NAME		_T("knsdsvc.exe")

#define BKPLAT__DLL_AUTORUNSCAN			_T("knatrun.dll")
#define BKPLAT__DLL_AVMGR				_T("knavmgr.dll")
#define BKPLAT__DLL_AVP					_T("knavp.dll")
#define BKPLAT__DLL_BAK					_T("knbak.dll")
#define BKPLAT__DLL_CACHE				_T("kncache.dll")

#define BKPLAT__DLL_DB					_T("kndb.dll")
#define BKPLAT__DLL_ENGINE				_T("kneng.dll")
#define BKPLAT__DLL_SAFEMON				_T("knsfmon.dll")
#define BKPLAT__DLL_UPLIVE				_T("knup.dll")
#define BKPLAT__DLL_VULFIX				_T("knvul.dll")
#define BKPLAT__DLL_VER					_T("knver.dll")


// AVP 升级设置 
#define DS						_T("\\")
#define AVP_LIB_INDEX_FILENAME	_T("u0607g.xml")
#define AVP_LIB_INDEX_INI		_T("avpup.ini")
//#define AVP_LIB_INDEX_URL       _T("C:\\KavSDK\\klupdate\\Updates\\index\\")AVP_LIB_INDEX_FILENAME
#define AVP_LIB_MIRROR_PATH		_T("http://up.sd.keniu.com/avpup/")
#define AVP_LIB_MIRROR_INDEX_PATH	AVP_LIB_MIRROR_PATH _T("index/")
#define AVP_LIB_MIRROR_BASES_PATH	AVP_LIB_MIRROR_PATH _T("bases/")
#define AVP_LIB_MIRROR_INDEXINI_URL	AVP_LIB_MIRROR_INDEX_PATH AVP_LIB_INDEX_INI
#define AVP_LIB_MIRROR_INDEX_URL	AVP_LIB_MIRROR_INDEX_PATH AVP_LIB_INDEX_INI

#define BKINI_SEC_AVPLIB			_T("avplib")
#define BKINI_SEC_AVPLIB_VER		_T("ver")
#define BKINI_SEC_AVPLIB_VER64		_T("ver64")


// Command Line 
#define BKSFNS_TRAYSTART                        L"TrayStart"
#define BKSFNS_UI                               L"UI"
#   define BKSFNS_PAGE_EXAM                     L"Exam"
#   define BKSFNS_PAGE_VIRSCAN                  L"VirScan"
#       define BKSFNS_VIRSCAN_FAST              L"Fast"
#			define BKSFNS_SCAN_RECURSIVE        L"Recursive"
#			define BKSFNS_SCAN_NOTRECURSIVE     L"NotRecursive"
#       define BKSFNS_VIRSCAN_FULL              L"Full"
#       define BKSFNS_VIRSCAN_CUSTOM            L"Custom"
#       define BKSFNS_VIRSCAN_FILE	            L"File"
#       define BKSFNS_VIRSCAN_SHOW_REPORT_LOG   L"ReportLog"
#   define BKSFNS_PAGE_LEAKSCAN                 L"LeakScan"
#   define BKSFNS_PAGE_LEAKSCAN2                L"LeakScan2"	// 当漏洞有更新后, 需要重新扫描漏洞 
#   define BKSFNS_PAGE_PLUGINSCAN               L"PluginScan"
#       define BKSFNS_PLUGINSCAN_SCAN           L"Scan"
#   define BKSFNS_PAGE_PROTECTION               L"Protection"
#       define BKSFNS_FRESH_PEOTECTION_STATE    L"Fresh"
#   define BKSFNS_PAGE_IEFIX                    L"IEFix"
#		define BKSFNS_BS_PEOTECTION				L"Protection"
#		define BKSFNS_BS_PEOTECTION_HISTORY		L"ProtectionHistory"
#		define BKSFNS_BS_FIX					L"Fix"
#		define BKSFNS_BS_PLUG					L"Plug"
#   define BKSFNS_PAGE_SYSTEM_OPTIMIZE          L"SysOpt"
#   define BKSFNS_SETTING                       L"Setting"
#       define BKSFNS_SETTING_PAGE_COMMON       L"Common"
#       define BKSFNS_SETTING_PAGE_VIRSCAN      L"VirScan"
#       define BKSFNS_SETTING_PAGE_VULSCAN      L"VulScan"
#       define BKSFNS_SETTING_PAGE_PROTECTION   L"Protection"
#       define BKSFNS_SETTING_PAGE_IEFIX        L"IEFix"
#   define BKSFNS_SETTING_CHANGE                L"SettingChange"
#   define BKSFNS_ECHO_FAILED                   L"EchoFail"
#   define BKSFNS_AVE_INSTALL_FINISH            L"avok"
#define BKSFNS_URL                              L"URL"
#define BKSFNS_JOIN_SKYLARK                     L"JoinSkylark"
#define BKSFNS_QUIT                             L"{B2C96360-9354-4686-A04B-0F9870D892C9}"

#define BKSFNS_MAKE(a, b) (a L"_" b)
#define BKSFNS_MAKE_3(a, b, c) (a L"_" b L"_" c)


#define CMDPARAM_NAME_NAVIGATE						L"do"
#define CMDPARAM_NAME_UPGRADEAVP					L"upgradeavp"
#define CMDPARAM_NAME_UPGRADEAVP_BACKGROUND			L"upgradeavp_background"		// 后台强制!! 不检查ini 版本升级avp 
#define CMDPARAM_NAME_RUN_MAINUI_FROM_TRAY			L"runfromtray"
#define CMDPARAM_NAME_INSTALL						L"install"
#define CMDPARAM_NAME_REINSTALL						L"reinstall"
#define CMDPARAM_NAME_INSTALL_CHECK					L"instcheck"
#define CMDPARAM_NAME_UNINSTALL						L"uninstall"
#define CMDPARAM_NAME_RESTART						L"restart"

#define BKANTIVIRUS_INSTANCE_OBJECT_NAME			L"{6A46F0B9-08F7-485b-BB28-EB911546ED7B}"
#define BKANTIVIRUS_DDE_WND_CLASS					L"{58CD53D3-0C91-48da-9120-7BE920B948EA}"

// Upgrade mutex 
#define BKANTIVIRUS_INSTANCE_UPGRADE_AVP			L"{DC9B400B-B69C-4612-863F-55A19590BA68}"
#define BKANTIVIRUS_MUTEX_UPGRADING_AVP				L"{CA2A423F-41CD-4abe-A177-2EDB05E6ACEC}"

#define BKANTIVIRUS_NAME_AVPUPGRAD_FORBIDDEN_MARK	L"Global\\KNSD_KAV_ENGINE_{45B6F8F3-300A-4857-B151-0EDE70469D73}"
#define BKANTIVIRUS_NAME_AVPUPGRAD_FORBIDDEN_MARK_SCANNING	L"KNSD_KAV_ENGINE_{0A220000-1F8D-48d9-AB63-27B78DB71D55}"

// path
#define UPDATE_EXE_DIR      L"uptemp"
#define MSG_INI_NAME        L"msg.ini"
