/* -------------------------------------------------------------------------
//	文件名		：	/Import/buildcfg/buildcfg.h
//	创建者		：	刘桂峰
//	创建时间	：	2008-1-6 13:27:52
//	功能描述	：	版本信息控制文件，修改自单机版2008的buildcfg.h
//
// -----------------------------------------------------------------------*/
#ifndef __BUILDCFG_BUILDCFG_H__
#define __BUILDCFG_BUILDCFG_H__

/*
0、在工程的stdafx.h中加入
	#include "buildcfg/buildcfg.h"		
	
1、打开本工程的主资源文件，删除原有的版本信息
	在 "3 TEXTINCLUDE" 段插入
	"#include ""buildcfg/VersionNo.rc2""\r\n"
	查找 "#ifndef APSTUDIO_INVOKED"，在中间接入 
	#include "buildcfg/VersionNo.rc2"

2、打开/include/buildcfg目录下对应的buildcfg.h文件
	找到该工程所在位置，请用英语定义以下三个宏，如：	

#ifdef KXE_PlATMAIN // size : 20000 工程名称的宏标记
	#pragma comment(linker, "/base:0x4DA00000")   
	#pragma message("\t    KXE_PLAT_MAIN")
	#define VI_FILEDESCRIPTION	"KXEngine Plat Main"	//该工程编译生成的文件的文件描述
	#define VI_INTERNALNAME		"KXMAIN"				//该工程编译生成的文件的内部名称
	#define VI_ORIGINALFILENAME	"kxemain.exe"			//该工程编译生成的文件的原始DLL文件名
#endif

3、在工程设置的 Resources 页的"Preprocessor definitions"中添加本工程对应的宏，如 KXE_PlATMAIN
	请注意所有配置都要加上
	在"Additionalresource include directories"中加上include目录的相对位置
	如果工程在src/xxxdll/x.proj，则需要添加引用目录 "../../include"
 */

// -------------------------------------------------------------------------
	

#ifdef _WINDOWS_
#error "include buildcfg.h before include all other files"
#endif
	
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES 1

/*
部分需要忽略的警告
#pragma warning(disable : 4018)
#pragma warning(disable : 4100)
#pragma warning(disable : 4189)	//local variable is initialized but not referenced
#pragma warning(disable : 4245)
#pragma warning(disable : 4995) //warning C4995: 'wcscpy': name was marked as #pragma deprecated
#pragma warning(disable : 4800)	//warning C4800: 'int' : forcing value to bool 'true' or 'false' (performance warning)
#pragma warning(disable : 4996)
#pragma warning(disable : 4267)	//'var' : conversion from 'size_t' to 'type', possible loss of data
#pragma warning(disable : 4244)	//'conversion' conversion from 'type1' to 'type2', possible loss of data
#pragma warning(disable : 4311)	//'variable' : pointer truncation from 'type' to 'type'
#pragma warning(disable : 4312)	//'operation' : conversion from 'type1' to 'type2' of greater size
#pragma warning(disable : 6248)	//Setting a SECURITY_DESCRIPTOR's DACL to NULL will result in an unprotected object
#pragma warning(disable : 6011)	//Dereferencing NULL pointer 'lpdis': Lines: 909, 911
#pragma warning(disable : 4353)	//nonstandard extension used: constant 0 as function expression. Use '__noop' function intrinsic instead
#pragma warning(disable : 4311)	//'type cast' : pointer truncation from 'void *' to 'int'
#pragma warning(disable : 6031)	//Return value ignored: 'SHGetSpecialFolderLocation')
#pragma warning(disable : 6297)	//Arithmetic overflow: 32-bit value is shifted, then cast to 64-bit value. Results might not be an expected value
#pragma warning(disable : 6211)	//Leaking memory 'pNewNode' due to an exception. Consider using a local catch block to clean up memory: Lines: 301, 30
#pragma warning(disable : 6242)	//A jump out of this try block forces local unwind. This incurs a severe performance penalty
#pragma warning(disable : 6258)	//Using TerminateThread does not allow proper thread clean up
#pragma warning(disable : 6246)	//Local declaration of 'strTaskFileName' hides declaration of the same name in outer scope. For additional information, see previous declaration at line '1156' of 'e:\kis2008\

//请打开VS 2005编译设置Advance中的“Enable Code Analysis for C/C++" (好像需要vs tream版的支持)
//下面这些一定要改
//printf("%s\n", wcs) -> printf("%S\n", wcs)
#pragma warning (error: 6303)	// Format string mismatch: wide character string passed as parameter '2' when character string is required in call to 'printf'

// i + 30 -> i += 30
#pragma warning (error: 4552)	

 // (!x & y) -> (!(x & y))
#pragma warning (error: 6290)	// Bitwise operation on logical result: ! has higher precedence than &. Use && or (!(x & y)) instead

//int a[2]; a[2] = 1; -> a[1] = 1
//#pragma warning (error: 6385)	// Invalid data: accessing 'argument 2', the readable size is 'sizeof(KAE_SCAN_FILE_STATUS)' bytes, but '404' bytes might be read: Lines: 442, 443
//#pragma warning (error: 6386)	// Buffer overrun: accessing 'argument 2', the writable size is '520' bytes, but '1040' bytes might be written: Lines: 62, 63
//GetShortPathName(lfn, sfn, sizeof(sfn)) - > GetShortPathName(lfn, sfn, countof(sfn))
//#pragma warning (error: 6057)	// Buffer overrun due to number of characters/number of bytes mismatch in call to 'GetShortPathNameW'
*/
// 主动重定位dll基地址
// 有的DLL需要的地址空间比较多，所以请不要在已有的重定位指示之间添加新的指示指令！
// DLL基地址的指定，基本按照dll的加载顺序递增

/////////////////////////////////
//exe
/*
#ifdef KISM_KAV32
	#pragma message("\t    KISM_KAV32")
	#define VI_FILEDESCRIPTION	"Kingsoft AntiVirus"
	#define VI_INTERNALNAME		"kav32"
	#define VI_ORIGINALFILENAME	"kav32.exe"
#endif
*/


/////////////////////////////////////////////////////////////
//dll
/*
#ifdef KISM_KIS
	#pragma comment(linker, "/base:0x41000000")
	#pragma message("\t    KISM_KIS")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Common Module"
	#define VI_INTERNALNAME		"kis"
	#define VI_ORIGINALFILENAME	"kis.dll"
#endif
*/

// -------------------------------------------------------------------------
#ifdef KXE_DE_CS_DLL
    #pragma message("\t    KXERelease_DefendEngineControlSystem")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Defend Engine Control System"
    #define VI_INTERNALNAME		"kxecs.dll"
    #define VI_ORIGINALFILENAME	"kxecs.dll"
#endif

#ifdef KXE_CORE_DLL
    #pragma message("\t    KXE_CORE_DLL")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Engine Control System"
    #define VI_INTERNALNAME		"kxecore.dll"
    #define VI_ORIGINALFILENAME	"kxecore.dll"
#endif

#ifdef KXE_DE_MCS_DLL
    #pragma message("\t    KXERelease_DefendEngineMonitorControlSystem")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Defend Engine Monitor Control System"
    #define VI_INTERNALNAME		"kxemcs.dll"
    #define VI_ORIGINALFILENAME	"kxemcs.dll"
#endif

#ifdef KXE_DE_RULEMGR_DLL
    #pragma message("\t    KXERelease_DefendEngineRuleManager")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Defend Engine Rule Manager"
    #define VI_INTERNALNAME		"krulemgr.dll"
    #define VI_ORIGINALFILENAME	"krulemgr.dll"
#endif

#ifdef KXE_DE_UTYU_DLL
    #pragma message("\t    KXERelease_DefendEngineUnicomClient")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Defend Engine Unicom Client"
    #define VI_INTERNALNAME		"kavutyu.dll"
    #define VI_ORIGINALFILENAME	"kavutyu.dll"
#endif

#ifdef KXE_DE_UIMGR_DLL
    #pragma message("\t    KXERelease_DefendEngineUserInteractiveSystemManager")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Defend Engine User Interactive System Manager"
    #define VI_INTERNALNAME		"kxeuimgr.dll"
    #define VI_ORIGINALFILENAME	"kxeuimgr.dll"
#endif

#ifdef KXE_DE_SAFEU_DLL
    #pragma message("\t    KXERelease_DefendEngineSafeModuleClient")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Defend Engine Safe Module Client"
    #define VI_INTERNALNAME		"kavsafeu.dll"
    #define VI_ORIGINALFILENAME	"kavsafeu.dll"
#endif

#ifdef KXE_DE_UTY_SYS
    #pragma message("\t    KXERelease_DefendEngineUilityModule")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Defend Engine Uility Module"
    #define VI_INTERNALNAME		"kavuty.sys"
    #define VI_ORIGINALFILENAME	"kavuty.sys"
#endif

#ifdef KXE_DE_PM_SYS
    #pragma message("\t    KXERelease_DefendEngineLogicModule")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Defend Engine Logic Module"
    #define VI_INTERNALNAME		"kavpm.sys"
    #define VI_ORIGINALFILENAME	"kavpm.sys"
#endif

#ifdef KXE_DE_FM_SYS
    #pragma message("\t    KXERelease_DefendEngineFileFilter")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Defend Engine File Filter"
    #define VI_INTERNALNAME		"kavfm.sys"
    #define VI_ORIGINALFILENAME	"kavfm.sys"
#endif

#ifdef KXE_DE_APIM_SYS
    #pragma message("\t    KXERelease_DefendEngineAPIMonitor")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Defend Engine API Mointer"
    #define VI_INTERNALNAME		"kavapim.sys"
    #define VI_ORIGINALFILENAME	"kavapim.sys"
#endif

#ifdef KXE_DE_SAFE_SYS
    #pragma message("\t    KXERelease_DefendEngineSafeModule")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Defend Engine Safe Module"
    #define VI_INTERNALNAME		"kavsafe2.sys"
    #define VI_ORIGINALFILENAME	"kavsafe2.sys"
#endif

#ifdef KXE_DE_WFS_DLL
    #pragma message("\t    KXERelease_LocalWhiteListSystemModule")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Local White List System Module"
    #define VI_INTERNALNAME		"whitefilesystem.dll"
    #define VI_ORIGINALFILENAME	"whitefilesystem.dll"
#endif

#ifdef KXE_SERV_EXE
    #pragma message("\t    KXERelease_ServiceManager")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Service Manager"
    #define VI_INTERNALNAME		"kxeserv.exe"
    #define VI_ORIGINALFILENAME	"kxeserv.exe"
#endif

#ifdef KXE_IPC_DLL
    #pragma message("\t    KXERelease_IPCWrapper")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus IPC Wrapper"
    #define VI_INTERNALNAME		"kavipc2.dll"
    #define VI_ORIGINALFILENAME	"kavipc2.dll"
#endif

#ifdef KXE_CFG_DLL
    #pragma message("\t    KXERelease_ConfigManager")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Config Manager"
    #define VI_INTERNALNAME		"kxecfg.dll"
    #define VI_ORIGINALFILENAME	"kxecfg.dll"
#endif

#ifdef KXE_EVENT_DLL
    #pragma message("\t    KXERelease_EventManager")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Event Manager"
    #define VI_INTERNALNAME		"kxeevent.dll"
    #define VI_ORIGINALFILENAME	"kxeevent.dll"
#endif

#ifdef KXE_HTTP_DLL
    #pragma message("\t    KXERelease_HttpManager")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus HTTP Manager"
    #define VI_INTERNALNAME		"kxehttp.dll"
    #define VI_ORIGINALFILENAME	"kxehttp.dll"
#endif

#ifdef KXE_LOG_DLL
    #pragma message("\t    KXERelease_DebugLogManager")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Debug Log Manager"
    #define VI_INTERNALNAME		"kxelog.dll"
    #define VI_ORIGINALFILENAME	"kxelog.dll"
#endif

#ifdef KXE_THREAD_DLL
    #pragma message("\t    KXERelease_ThreadManager")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Thread Manager"
    #define VI_INTERNALNAME		"kxethr.dll"
    #define VI_ORIGINALFILENAME	"kxethr.dll"
#endif

#ifdef KXE_BASE_DLL
    #pragma message("\t    KXERelease_BaseSDK")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Base SDK"
    #define VI_INTERNALNAME		"kxebase.dll"
    #define VI_ORIGINALFILENAME	"kxebase.dll"
#endif

#ifdef KXE_PASSPORT_SP_DLL
    #pragma message("\t    KXERelease_PassportSP")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Passport Service Provider"
    #define VI_INTERNALNAME		"kxeppsp.dll"
    #define VI_ORIGINALFILENAME	"kxeppsp.dll"
#endif

#ifdef KXE_PASSPORTEX_SP_DLL
	#pragma message("\t    KXERelease_PassportSPEx")
	#define VI_FILEDESCRIPTION	"Kingsoft Passport Service Provider Ex"
	#define VI_INTERNALNAME		"kxepassportspex"
	#define VI_ORIGINALFILENAME	"kxepassportspex.dll"
#endif

#ifdef KXE_SYSTEM_SP_DLL
    #pragma message("\t    KXERelease_SystemSP")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus System Service Provider"
    #define VI_INTERNALNAME		"kxesyssp.dll"
    #define VI_ORIGINALFILENAME	"kxesyssp.dll"
#endif

#ifdef KXE_UPDATE_SP_DLL
    #pragma message("\t    KXERelease_UpdateSP")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Update Service Provider"
    #define VI_INTERNALNAME		"kxeupdsp.dll"
    #define VI_ORIGINALFILENAME	"kxeupdsp.dll"
#endif

#ifdef KXE_EXPRESS_LAYER_DLL
    #pragma message("\t    KXERelease_ExpressLayer")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Express Layer"
    #define VI_INTERNALNAME		"kxeexp.dll"
    #define VI_ORIGINALFILENAME	"kxeexp.dll"
#endif

#ifdef SCOM_DLL
	#pragma message("\t    SCOM_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft Internet Security SCOM"
	#define VI_INTERNALNAME		"scom.dll"
	#define VI_ORIGINALFILENAME	"scom.dll"
#endif

#ifdef SCOMREGSVR_EXE
	#pragma message("\t    SCOMREGSVR_EXE")
	#define VI_FILEDESCRIPTION	"Kingsoft Internet Security SCOM Register Service"
	#define VI_INTERNALNAME		"scomregsvr.exe"
	#define VI_ORIGINALFILENAME	"scomregsvr.exe"
#endif

#ifdef KXE_DEVMGR_EXE
	#pragma message("\t    KXE_DEVMGR_EXE")
	#define VI_FILEDESCRIPTION	"Kingsoft Driver Install Manager"
	#define VI_INTERNALNAME		"kdevmgr.exe"
	#define VI_ORIGINALFILENAME	"kdevmgr.exe"
#endif

#ifdef KXE_DUMP_DLL
	#pragma message("\t    KXE_DUMP_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft Dump Generator"
	#define VI_INTERNALNAME		"kxedump.dll"
	#define VI_ORIGINALFILENAME	"kxedump.dll"
#endif

#ifdef KXE_COLLECT_EXE
	#pragma message("\t    KXE_COLLECT_EXE")
	#define VI_FILEDESCRIPTION	"Kingsoft Info Collecter"
	#define VI_INTERNALNAME		"kxecolct.exe"
	#define VI_ORIGINALFILENAME	"kxecolct.exe"
#endif

#ifdef KXE_UPDATE_SERVICE_EXE
	#pragma message("\t    KXE_UPDATE_SERVICE_EXE")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Update Service"
	#define VI_INTERNALNAME		"kxeupsrv.exe"
	#define VI_ORIGINALFILENAME	"kxeupsrv.exe"
#endif

#ifdef KXE_FILE_REGISTER_DLL
	#pragma message("\t    KXE_FILE_REGISTER_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft Component Register"
	#define VI_INTERNALNAME		"kxereg.dll"
	#define VI_ORIGINALFILENAME	"kxereg.dll"
#endif

#ifdef KXE_FILE_REGISTER_DLL
	#pragma message("\t    KXE_FILE_REGISTER_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft Component Register"
	#define VI_INTERNALNAME		"kxereg.dll"
	#define VI_ORIGINALFILENAME	"kxereg.dll"
#endif

#ifdef KXE_SERVICE_CALL_DLL
	#pragma message("\t   KXE_SERVICE_CALL_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft Service Call"
	#define VI_INTERNALNAME		"kxesvc.dll"
	#define VI_ORIGINALFILENAME	"kxesvc.dll"
#endif

#ifdef KXE_UPDATE_EXE
	#pragma message("\t   KXE_UPDATE_EXE")
	#define VI_FILEDESCRIPTION	"KXEngine Update Service"
	#define VI_INTERNALNAME		"kxeupd.exe"
	#define VI_ORIGINALFILENAME	"kxeupd.exe"
#endif

#ifdef KXE_PIPE_DLL
	#pragma message("\t   KXE_PIPE_DLL")
	#define VI_FILEDESCRIPTION	"KXEngine Pipe"
	#define VI_INTERNALNAME		"kxepipe.dll"
	#define VI_ORIGINALFILENAME	"kxepipe.dll"
#endif

#ifdef KXE_HOST_EXE
	#pragma message("\t   KXE_HOST_EXE")
	#define VI_FILEDESCRIPTION	"KXEngine Product Host"
	#define VI_INTERNALNAME		"kxehost.exe"
	#define VI_ORIGINALFILENAME	"kxehost.exe"
#endif

#ifdef KXE_UTIL_EXE
	#pragma message("\t   KXE_UTIL_EXE")
	#define VI_FILEDESCRIPTION	"KXEngine Utility"
	#define VI_INTERNALNAME		"kxeutil.exe"
	#define VI_ORIGINALFILENAME	"kxeutil.exe"
#endif

#ifdef KXE_XMLHTTP_DLL
	#pragma message("\t   KXE_XMLHTTP_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft XML Http Request"
	#define VI_INTERNALNAME		"kxmlhttp.dll"
	#define VI_ORIGINALFILENAME	"kxmlhttp.dll"
#endif

#ifdef KXE_KWatch64_SYS // size : 20000 工程名称的宏标记
	#pragma message("\t    KXE_KWatch64_SYS")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus KWatch Driver"	//该工程编译生成的文件的文件描述
	#define VI_INTERNALNAME		"kwatch64.sys"				//该工程编译生成的文件的内部名称
	#define VI_ORIGINALFILENAME	"kwatch64.sys"			//该工程编译生成的文件的原始DLL文件名
#endif

#ifdef KXE_KWatch32_SYS // size : 20000 工程名称的宏标记
	#pragma message("\t    KXE_KWatch32_SYS")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus KWatch Driver"	//该工程编译生成的文件的文件描述
	#define VI_INTERNALNAME		"kwatch32.sys"				//该工程编译生成的文件的内部名称
	#define VI_ORIGINALFILENAME	"kwatch32.sys"			//该工程编译生成的文件的原始DLL文件名
#endif

#ifdef KXE_Driver64_EXE // size : 20000 工程名称的宏标记
    #pragma message("\t    KXE_Driver64_EXE")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Driver Install"	//该工程编译生成的文件的文件描述
    #define VI_INTERNALNAME		"driver64.exe"				//该工程编译生成的文件的内部名称
    #define VI_ORIGINALFILENAME	"driver64.exe"			//该工程编译生成的文件的原始DLL文件名
#endif

#ifdef KXE_Driver32_EXE // size : 20000 工程名称的宏标记
    #pragma message("\t    KXE_Driver32_EXE")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Driver Install"	//该工程编译生成的文件的文件描述
    #define VI_INTERNALNAME		"driver32.exe"				//该工程编译生成的文件的内部名称
    #define VI_ORIGINALFILENAME	"driver32.exe"			//该工程编译生成的文件的原始DLL文件名
#endif

#ifdef KXE_FileMonSys_DLL // size : 20000 工程名称的宏标记 
	#pragma message("\t    KXE_FileMonSYS_Dll")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus FileMon Subsystem"	//该工程编译生成的文件的文件描述
	#define VI_INTERNALNAME		"kxefmsys.dll"				//该工程编译生成的文件的内部名称
	#define VI_ORIGINALFILENAME	"kxefmsys.dll"			//该工程编译生成的文件的原始DLL文件名
#endif

#ifdef KXE_FileMonSDK_DLL // size : 20000 工程名称的宏标记 
    #pragma message("\t    KXE_FileMonSDK_DLL")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus FileMon Pluign"	//该工程编译生成的文件的文件描述
    #define VI_INTERNALNAME		"kxefm.dll"				//该工程编译生成的文件的内部名称
    #define VI_ORIGINALFILENAME	"kxefm.dll"			//该工程编译生成的文件的原始DLL文件名
#endif

#ifdef KXE_FileMonSP_DLL // size : 20000 工程名称的宏标记 
    #pragma message("\t    KXE_FileMonSP_Dll")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus FileMon Serviceprovider"	//该工程编译生成的文件的文件描述
    #define VI_INTERNALNAME		"kxefmsp.dll"				//该工程编译生成的文件的内部名称
    #define VI_ORIGINALFILENAME	"kxefmsp.dll"			//该工程编译生成的文件的原始DLL文件名
#endif

#ifdef KXE_DE_WFS_SDK_DLL
	#pragma message("\t    KXERelease_LocalWhiteListSystemSDKModule")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Local White List System SDK Module"
	#define VI_INTERNALNAME		"kxewfssdk.dll"
	#define VI_ORIGINALFILENAME	"kxewfssdk.dll"
#endif

#ifdef KXE_DE_WFS_SP_DLL
	#pragma message("\t    KXERelease_LocalWhiteListSystemSPModule")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Local White List System SP Module"
	#define VI_INTERNALNAME		"kxewfssp.dll"
	#define VI_ORIGINALFILENAME	"kxewfssp.dll"
#endif

#ifdef KXE_DE_WFS_SYS_DLL
	#pragma message("\t    KXERelease_LocalWhiteListSystemModule")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Local White List System Module"
	#define VI_INTERNALNAME		"kxewfssys.dll"
	#define VI_ORIGINALFILENAME	"kxewfssys.dll"
#endif

#ifdef KISM_LOGVIEWER
	#pragma message("\t    KISM_LOGVIEWER")
	#define VI_FILEDESCRIPTION	"Kingsoft AntiVirus LogViewer"
	#define VI_INTERNALNAME		"kavlogviewer"
	#define VI_ORIGINALFILENAME	"kavlog2.exe"
#endif

#ifdef KISM_RECYCLE
	#pragma message("\t    KISM_RECYCLE")
	#define VI_FILEDESCRIPTION	"Kingsoft AntiVirus Recycle"
	#define VI_INTERNALNAME		"krecycle"
	#define VI_ORIGINALFILENAME	"krecycle.exe"
#endif

#ifdef KISM_OESETWIZ
	#pragma message("\t    KISM_OESETWIZ")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus OutlookExpress Spam SetupWizzard"
	#define VI_INTERNALNAME		"kmailmon"
	#define VI_ORIGINALFILENAME	"kmailmon.exe"
#endif

#ifdef KISM_MAILMON
	#pragma message("\t    KISM_MAILMON")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Email Scan"
	#define VI_INTERNALNAME		"kmailoeb"
	#define VI_ORIGINALFILENAME	"kmailoeb.dll"
#endif


#ifdef KISM_KMSETTING
	#pragma message("\t    KISM_KMSETTING")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus KMSetting"
	#define VI_INTERNALNAME		"kmsetting"
	#define VI_ORIGINALFILENAME	"kmfilter.exe"
#endif

#ifdef KISM_KAVSTART
	#pragma message("\t    KISM_KAVSTART")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Security Center"
	#define VI_INTERNALNAME		"kavstart"
	#define VI_ORIGINALFILENAME	"kavstart.exe"
#endif

#ifdef KISM_SETUPWIZARD
	#pragma message("\t    KISM_SETUPWIZARD")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus SetupWizard"
	#define VI_INTERNALNAME		"setupwiz"
	#define VI_ORIGINALFILENAME	"setupwiz.exe"
#endif

#ifdef KISM_KUPDATE
#pragma message("\t    KISM_KUPDATE")
#define VI_FILEDESCRIPTION	"Kingsoft KUpLive"
#define VI_INTERNALNAME		"kuplive"
#define VI_ORIGINALFILENAME	"kuplive.exe"
#endif

#ifdef KISM_KWATCH
	#pragma message("\t    KISM_KWATCH")
	#define VI_FILEDESCRIPTION	"Kingsoft AntiVirus KWatch Service"
	#define VI_INTERNALNAME		"KWatch"
	#define VI_ORIGINALFILENAME	"KWatch.exe"
#endif

#ifdef KISM_SERVICEMANAGER
	#pragma message("\t    KISM_SERVICEMANAGER")
	#define VI_FILEDESCRIPTION	"Kingsoft AntiVirus Service Manager"
	#define VI_INTERNALNAME		"kissvc"
	#define VI_ORIGINALFILENAME	"kissvc.exe"
#endif

#ifdef KISM_KISCALLER
	#pragma message("\t    KISM_KISCALLER")
	#define VI_FILEDESCRIPTION	"Kingsoft AntiVirus Module Caller"
	#define VI_INTERNALNAME		"kiscaller"
	#define VI_ORIGINALFILENAME	"kiscaller.exe"
#endif

#ifdef KISM_KISVSWTH
	#pragma message("\t    KISM_KISVSWTH")
	#define VI_FILEDESCRIPTION	"Kingsoft AntiVirus version switch"
	#define VI_INTERNALNAME		"kisvswth"
	#define VI_ORIGINALFILENAME	"kisvswth.exe"
#endif

#ifdef ETM_KISPOST 
	#pragma message("\t    ETM_KISPOST")
	#define VI_FILEDESCRIPTION	"KIS Operation Support"
	#define VI_INTERNALNAME		"kispost"
	#define VI_ORIGINALFILENAME	"kispost.bin"
#endif

#ifdef KISM_MAKESETUP
#pragma message("\t    KISM_MAKESETUP")
#define VI_FILEDESCRIPTION	"Kingsoft Make Setup"
#define VI_INTERNALNAME		"makesetupext"
#define VI_ORIGINALFILENAME	"makesetupext.exe"
#endif

#ifdef KISM_KAVPE
#pragma message("\t    KISM_KAVPE")
#define VI_FILEDESCRIPTION	"Kingsoft AntiVirus PE"
#define VI_INTERNALNAME		"kavpe"
#define VI_ORIGINALFILENAME	"kavpe.exe"
#endif

#ifdef KISM_UPDUTILITY
//	#pragma comment(linker, "/base:0x43000000")
#pragma message("\t    KISM_UPDUTILITY")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Updutility Module"
#define VI_INTERNALNAME		"updutility"
#define VI_ORIGINALFILENAME	"updutility.dll"
#endif

#ifdef KISM_SETUPMODECHANGE_EXE
//	#pragma comment(linker, "/base:0x43000000")
#pragma message("\t    KISM_SETUPMODECHANGE_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Setup Mode Changer"
#define VI_INTERNALNAME		"setupmodechange"
#define VI_ORIGINALFILENAME	"setupmodechange.exe"
#endif

/////////////////////////////////////////////////////////////
//dll

#ifdef KISM_KIS
//	#pragma comment(linker, "/base:0x41000000")
	#pragma message("\t    KISM_KIS")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Common Module"
	#define VI_INTERNALNAME		"kis"
	#define VI_ORIGINALFILENAME	"kis.dll"
#endif

#ifdef KISM_KAV32RES
//	#pragma comment(linker, "/base:0x41800000")	//因为纯资源DLL，所以写死在工程设置中，而且要留大一点
	#pragma message("\t    KISM_KAV32RES")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Main Program Resource"
	#define VI_INTERNALNAME		"kav32.res"
	#define VI_ORIGINALFILENAME	"kav32res.dll"
#endif

#ifdef KISM_KAVIPC
//	#pragma comment(linker, "/base:0x42000000")
	#pragma message("\t    KISM_KAVIPC")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Interprocess Communication"
	#define VI_INTERNALNAME		"kavipc"
	#define VI_ORIGINALFILENAME	"kavipc2.dll"
#endif

#ifdef KISM_PPWIZARD
//	#pragma comment(linker, "/base:0x43400000")
	#pragma message("\t    KISM_PPWIZARD")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus PPWizard Module"
	#define VI_INTERNALNAME		"ppwiz"
	#define VI_ORIGINALFILENAME	"kavppwiz.dll"
#endif

#ifdef KISM_KAVQUARA
//	#pragma comment(linker, "/base:0x43800000")
	#pragma message("\t    KISM_KAVQUARA")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Quarantine Module"
	#define VI_INTERNALNAME		"kavquara"
	#define VI_ORIGINALFILENAME	"kavquara.dll"
#endif

#ifdef KISM_OEPLUGIN
//	#pragma comment(linker, "/base:0x43B00000")
	#pragma message("\t    KISM_OEPLUGIN")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Outlook Express Plugin"
	#define VI_INTERNALNAME		"kmailoeb"
	#define VI_ORIGINALFILENAME	"kmailoeb.dll"
#endif

#ifdef KISM_KOLHTML
//	#pragma comment(linker, "/base:0x44400000")
	#pragma message("\t    KISM_KOLHTML")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Online Service Module"
	#define VI_INTERNALNAME		"kolhtml"
	#define VI_ORIGINALFILENAME	"kolhtml.dll"
#endif

#ifdef KISM_POPSUPPORT3
//	#pragma comment(linker, "/base:0x44B00000")
	#pragma message("\t    KISM_POPSUPPORT3")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Security Center Support Component"
	#define VI_INTERNALNAME		"popsprt3"
	#define VI_ORIGINALFILENAME	"popsprt3.dll"
#endif

#ifdef KISM_SERVICECALL
//	#pragma comment(linker, "/base:0x45000000")
	#pragma message("\t    KISM_SERVICECALL")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Vista Platform Helper Service"
	#define VI_INTERNALNAME		"kissvc"
	#define VI_ORIGINALFILENAME	"kissvc.dll"
#endif

#ifdef KISM_NDISFILTER
//	#pragma comment(linker, "/base:0x45800000")
	#pragma message("\t    KISM_NDISFILTER")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Ndis Filter Install Helper"
	#define VI_INTERNALNAME		"ndisfilter"
	#define VI_ORIGINALFILENAME	"ndisfilter.dll"
#endif

#ifdef KISM_IEBEXT
//	#pragma comment(linker, "/base:0x45B00000")
	#pragma message("\t    KISM_IEBEXT")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus IE Buddy Extend Module"
	#define VI_INTERNALNAME		"iebext"
	#define VI_ORIGINALFILENAME	"iebext.dll"
#endif

#ifdef KISM_KAVPASSPORT
//	#pragma comment(linker, "/base:0x46000000")
	#pragma message("\t    KISM_KAVPASSPORT")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Online Passport Support"
	#define VI_INTERNALNAME		"kavpassp"
	#define VI_ORIGINALFILENAME	"kavpassp.dll"
#endif

#ifdef KISM_SETUPHELPER
//	#pragma comment(linker, "/base:0x46400000")
	#pragma message("\t    KISM_SETUPHELPER")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Setup Helper"
	#define VI_INTERNALNAME		"setuphelper"
	#define VI_ORIGINALFILENAME	"setuphelper.dll"
#endif

#ifdef KISM_KISFREE
//	#pragma comment(linker, "/base:0x46800000")
	#pragma message("\t    KISM_KISFREE")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus free"
	#define VI_INTERNALNAME		"kisfree"
	#define VI_ORIGINALFILENAME	"kisfree.dll"
#endif

#ifdef KISM_KISFREEEX
//	#pragma comment(linker, "/base:0x46800000")
	#pragma message("\t    KISM_KISFREEEX")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus free"
	#define VI_INTERNALNAME		"kisfreeex"
	#define VI_ORIGINALFILENAME	"kisfreeex.dll"
#endif


#ifdef KISM_KBILLBOARD
//	#pragma comment(linker, "/base:0x46800000")
	#pragma message("\t    KISM_KISFREEEX")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus free"
	#define VI_INTERNALNAME		"kbillboard"
	#define VI_ORIGINALFILENAME	"kbillboard.ocx"
#endif

#ifdef KISM_PUSHINFO
//	#pragma comment(linker, "/base:0x46400000")
	#pragma message("\t    KISM_PUSHINFO")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Pushinfo"
	#define VI_INTERNALNAME		"pushinfo"
	#define VI_ORIGINALFILENAME	"pushinfo.dll"
#endif

#ifdef KISM_KAVKW
//	#pragma comment(linker, "/base:0x47000000")   
	#pragma message("\t    KISM_KAVKW")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus keyword Analyzer"
	#define VI_INTERNALNAME		"kavkw"
	#define VI_ORIGINALFILENAME	"kavkw.dll"
#endif

#ifdef KISM_KWHELPER
//	#pragma comment(linker, "/base:0x47400000")   
	#pragma message("\t    KISM_KWHELPER")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus keyword helper"
	#define VI_INTERNALNAME		"kwhelper"
	#define VI_ORIGINALFILENAME	"kwhelper.dll"
#endif

#ifdef KISM_UPLIVEEX
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Uplive Extend"
#define VI_INTERNALNAME		"updateex"
#define VI_ORIGINALFILENAME	"updateex.dll"
#endif

#ifdef KISM_HOTFIX 
	//#pragma comment(linker, "/base:0x4DA00000")   
	#pragma message("\t    KISM_HOTFIX")
	#define VI_FILEDESCRIPTION	"Kingsoft HotFix"
	#define VI_INTERNALNAME		"kahotfix"	
	#define VI_ORIGINALFILENAME	"kahotfix.khf"	
#endif

#ifdef KISM_ADUSERINFO 
//#pragma comment(linker, "/base:0x4DA00000")   
#pragma message("\t    KISM_ADUSERINFO")
#define VI_FILEDESCRIPTION	"Kingsoft ADUserInfo"
#define VI_INTERNALNAME		"aduserinfo"	
#define VI_ORIGINALFILENAME	"aduserinfo.DLL"	
#endif

#ifdef KISM_KISOP
//	#pragma comment(linker, "/base:0x47400000")   
	#pragma message("\t    KISM_KISOP")
	#define VI_FILEDESCRIPTION	"Kingsoft Install Information collector"
	#define VI_INTERNALNAME		"kisop"
	#define VI_ORIGINALFILENAME	"kisop.dll"
#endif

#ifdef KISM_AUTORUNMAN
//	#pragma comment(linker, "/base:0x47400000")   
#pragma message("\t    KISM_AUTORUNMAN")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy Autorunman"
#define VI_INTERNALNAME		"autorunman"
#define VI_ORIGINALFILENAME	"autorunman.dll"
#endif

#ifdef KISM_FILECRUSHER
//	#pragma comment(linker, "/base:0x47400000")   
#pragma message("\t    KISM_FILECRUSHER")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy Filecrusher"
#define VI_INTERNALNAME		"filecrusher"
#define VI_ORIGINALFILENAME	"filecrusher.dll"
#endif

#ifdef KISM_FILEDOWNLOADER
//	#pragma comment(linker, "/base:0x47400000")   
#pragma message("\t    KISM_FILEDOWNLOADER")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy Filedownloader"
#define VI_INTERNALNAME		"filedownloader"
#define VI_ORIGINALFILENAME	"filedownloader.dll"
#endif

#ifdef KISM_KASBROWSERSHIELD
//	#pragma comment(linker, "/base:0x47400000")   
#pragma message("\t    KISM_KASBROWSERSHIELD")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy Kasbrowsershield"
#define VI_INTERNALNAME		"kasbrowsershield"
#define VI_ORIGINALFILENAME	"kasbrowsershield.dll"
#endif

#ifdef KISM_KASCOMMON
//	#pragma comment(linker, "/base:0x47400000")   
#pragma message("\t    KISM_KASCOMMON")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy Kascommon"
#define VI_INTERNALNAME		"kascommon"
#define VI_ORIGINALFILENAME	"kascommon.dll"
#endif

#ifdef KISM_KASEARCH
//	#pragma comment(linker, "/base:0x47400000")   
#pragma message("\t    KISM_KASEARCH")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy Kasearch"
#define VI_INTERNALNAME		"kasearch"
#define VI_ORIGINALFILENAME	"kasearch.dll"
#endif

#ifdef KISM_KASSPYINFO
//	#pragma comment(linker, "/base:0x47400000")   
#pragma message("\t    KISM_KASSPYINFO")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy Kasspyinfo"
#define VI_INTERNALNAME		"kasspyinfo"
#define VI_ORIGINALFILENAME	"kasspyinfo.dll"
#endif

#ifdef KISM_LSPREPAIRER
//	#pragma comment(linker, "/base:0x47400000")   
#pragma message("\t    KISM_LSPREPAIRER")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy Lsprepairer"
#define VI_INTERNALNAME		"lsprepairer"
#define VI_ORIGINALFILENAME	"lsprepairer.dll"
#endif

#ifdef KISM_PROCESSMANAGER
//	#pragma comment(linker, "/base:0x47400000")   
#pragma message("\t    KISM_PROCESSMANAGER")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy Processmanager"
#define VI_INTERNALNAME		"processmanager"
#define VI_ORIGINALFILENAME	"processmanager.dll"
#endif

#ifdef KISM_SYSTEMREPAIR
//	#pragma comment(linker, "/base:0x47400000")   
#pragma message("\t    KISM_SYSTEMREPAIR")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy Systemrepair"
#define VI_INTERNALNAME		"systemrepair"
#define VI_ORIGINALFILENAME	"systemrepair.dll"
#endif

#ifdef KISM_TRACEERASER
//	#pragma comment(linker, "/base:0x47400000")   
#pragma message("\t    KISM_TRACEERASER")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy Traceeraser"
#define VI_INTERNALNAME		"traceeraser"
#define VI_ORIGINALFILENAME	"traceeraser.dll"
#endif

#ifdef KISM_TRACKERASER
//	#pragma comment(linker, "/base:0x47400000")   
#pragma message("\t    KISM_TRACKERASER")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy Trackeraser"
#define VI_INTERNALNAME		"trackeraser"
#define VI_ORIGINALFILENAME	"trackeraser.dll"
#endif

#ifdef KISM_VIEWDESCRIBE
//	#pragma comment(linker, "/base:0x47400000")   
#pragma message("\t    KISM_VIEWDESCRIBE")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy Viewdescribe"
#define VI_INTERNALNAME		"viewdescribe"
#define VI_ORIGINALFILENAME	"viewdescribe.dll"
#endif

#ifdef KISM_KANTRAY
//	#pragma comment(linker, "/base:0x47400000")   
#pragma message("\t    KISM_KANTRAY")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy Kantray"
#define VI_INTERNALNAME		"kantray"
#define VI_ORIGINALFILENAME	"kantray.dll"
#endif

#ifdef KISM_TRASHFILECLEAR
//	#pragma comment(linker, "/base:0x47400000")   
#pragma message("\t    KISM_TRASHFILECLEAR")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy Trashfileclear"
#define VI_INTERNALNAME		"trashfileclear"
#define VI_ORIGINALFILENAME	"trashfileclear.dll"
#endif

#ifdef KISM_KSAENGINEXT
//	#pragma comment(linker, "/base:0x47400000")   
#pragma message("\t    KISM_KSAENGINEXT")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy KSAext"
#define VI_INTERNALNAME		"ksaext"
#define VI_ORIGINALFILENAME	"ksaext.ext"
#endif

#ifdef KISM_KASMENUEXT 
//  #pragma comment(linker, "/base:0x4DA00000")   
#pragma message("\t    KISM_KASMENUEXT")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy MenuExt"	//文件描述
#define VI_INTERNALNAME		"kasmenuext"			//内部名称
#define VI_ORIGINALFILENAME	"kasmenuext.dll"		//原始DLL文件名
#endif


#ifdef KISM_KASTOOLBOX
//  #pragma comment(linker, "/base:0x4DA00000")   
#pragma message("\t    KISM_KASTOOLBOX")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy ToolBox"	//文件描述
#define VI_INTERNALNAME		"kastoolbox"			//内部名称
#define VI_ORIGINALFILENAME	"kastoolbox.exe"		//原始DLL文件名
#endif


#ifdef KISM_KASCNOBJ
//	#pragma comment(linker, "/base:0x4DA00000")   
#pragma message("\t    KISM_KASCNOBJ")
#define VI_FILEDESCRIPTION	"Kingsoft KAVPE Scan Module"
#define VI_INTERNALNAME		"kascnobj"
#define VI_ORIGINALFILENAME	"kascnobj.dll"
#endif

#ifdef KISM_KAVPEND
//	#pragma comment(linker, "/base:0x4DA00000")
#pragma message("\t    KISM_KAVPEND")
#define VI_FILEDESCRIPTION	"Kingsoft KAVPE Unload Module"
#define VI_INTERNALNAME		"kavpend"
#define VI_ORIGINALFILENAME	"kavpend.dll"
#endif

#ifdef KISM_KPESHELL
//	#pragma comment(linker, "/base:0x4DA00000")
#pragma message("\t    KISM_KPESHELL")
#define VI_FILEDESCRIPTION	"Kingsoft KAVPE SHELL Module"
#define VI_INTERNALNAME		"kpeshell"
#define VI_ORIGINALFILENAME	"kpeshell.dll"
#endif

#ifdef KISM_TASKBAR
//	#pragma comment(linker, "/base:0x43000000")
#pragma message("\t    KISM_TASKBAR")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Taskbar Extent Module"
#define VI_INTERNALNAME		"ktaskbar"
#define VI_ORIGINALFILENAME	"ktaskbar.dll"
#endif

// -------------------------------------------------------------------------
//sys
#ifdef KISM_NETWCHSYS
	#pragma message("\t    KISM_NETWCHSYS")
	#define VI_FILEDESCRIPTION	"Kingsoft Firewall Kernel Driver"
	#define VI_INTERNALNAME		"knetwch"
	#define VI_ORIGINALFILENAME	"knetwch.sys"
#endif
	
#ifdef KISM_KWATCHSYSV
	#pragma message("\t    KISM_KWATCHSYSV")
	#define VI_FILEDESCRIPTION	"Kingsoft Firewall Ndis Filter Driver"
	#define VI_INTERNALNAME		"knetwchv"
	#define VI_ORIGINALFILENAME	"knetwchv.sys"
#endif
// -------------------------------------------------------------------------

#ifdef KISM_KPECALL
	#pragma message("\t    KISM_UPLIVEFROMWIN")
	#define VI_FILEDESCRIPTION	"Kingsoft KPECall"
	#define VI_INTERNALNAME		"kpecall"
	#define VI_ORIGINALFILENAME	"kpecall.dll"
#endif

#ifdef KISM_LOADPACKAGE
	#pragma message("\t    KISM_UPLIVEFROMWIN")
	#define VI_FILEDESCRIPTION	"Kingsoft LoadPackage"
	#define VI_INTERNALNAME		"loadpackage"
	#define VI_ORIGINALFILENAME	"loadpackage.exe"
#endif

#ifdef KXE_UPDATE_CHECKER
	#pragma message("\t    KXE_UPDATE_CHECKER")
	#define VI_FILEDESCRIPTION	"Kingsoft Update Checker"
	#define VI_INTERNALNAME		"kxeupchk.dll"
	#define VI_ORIGINALFILENAME	"kxeupchk.dll"
#endif

#ifdef KXE_LEOS_UPDATE
	#pragma message("\t    KXE_LEOS_UPDATE")
	#define VI_FILEDESCRIPTION	"LEOS Update"
	#define VI_INTERNALNAME		"leosupdate.exe"
	#define VI_ORIGINALFILENAME	"leosupdate.exe"
#endif

#ifdef KXE_PASSPORT_SP_DLL
    #pragma message("\t    KXERelease_PassportSP")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Passport Service Provider"
    #define VI_INTERNALNAME		"kxeppsp.dll"
    #define VI_ORIGINALFILENAME	"kxeppsp.dll"
#endif

#ifdef KXE_PASSPORTEX_SP_DLL
	#pragma message("\t    KXERelease_PassportSPEx")
	#define VI_FILEDESCRIPTION	"Kingsoft Passport Service Provider Ex"
	#define VI_INTERNALNAME		"kxepassportspex"
	#define VI_ORIGINALFILENAME	"kxepassportspex.dll"
#endif

#ifdef KXE_KAVSTART_DLL
#pragma message("\t    KXEKAVSTART_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus kavstart security center"
#define VI_INTERNALNAME		"kavstart.dll"
#define VI_ORIGINALFILENAME	"kavstart.dll"
#endif


#ifdef KISM_EVENTLOG
    #pragma message("\t    KISM_EVENTLOG")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Event Manager"
    #define VI_INTERNALNAME		"kavevent.dll"
    #define VI_ORIGINALFILENAME	"kavevent.dll"
#endif

#ifdef KISM_UPDATE
    #pragma message("\t    KISM_UPDATE")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Uplive"
    #define VI_INTERNALNAME		"uplive.exe"
    #define VI_ORIGINALFILENAME	"uplive.exe"
#endif

#ifdef KXE_SCAN_SP_DLL
	#pragma message("\t    KXE_SCAN_SP_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft Scan Service Provider"
	#define VI_INTERNALNAME		"kxesansp.dll"
	#define VI_ORIGINALFILENAME	"kxesansp.dll"
#endif

#ifdef KXE_POPSDK_DLL
#pragma message("\t    KXE_POPSDK_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft KxePop SDKSystem"
#define VI_INTERNALNAME		"kxepop.dll"
#define VI_ORIGINALFILENAME	"kxepop.dll"
#endif

#ifdef KXE_POPSYS_DLL
#pragma message("\t    KXE_POPSYS_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft KxePop SubSystem"
#define VI_INTERNALNAME		"kxepopsys.dll"
#define VI_ORIGINALFILENAME	"kxepopsys.dll"
#endif

#ifdef KXE_POP_SP_DLL
#pragma message("\t    KXE_POP_SP_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft KxePop Service Provider"
#define VI_INTERNALNAME		"kxepopsp.dll"
#define VI_ORIGINALFILENAME	"kxepopsp.dll"
#endif

#ifdef KXE_KXEPOPTOOL_EXE
#pragma message("\t    KXE_KXEPOPTOOL_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft KxePop Tool"
#define VI_INTERNALNAME		"kxepoptool.exe"
#define VI_ORIGINALFILENAME	"kxepoptool.exe"
#endif

#ifdef KXE_KSTART64_EXE
#pragma message("\t    KXE_KSTART64_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Security Center"
#define VI_INTERNALNAME		"kstart64.exe"
#define VI_ORIGINALFILENAME	"kstart64.exe"
#endif

#ifdef KXE_KSTART32_EXE
#pragma message("\t    KXE_KSTART32_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Security Center"
#define VI_INTERNALNAME		"kstart32.exe"
#define VI_ORIGINALFILENAME	"kstart32.exe"
#endif

#ifdef KXE_KSTART_EXE
#pragma message("\t    KXE_KSTART_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Security Center"
#define VI_INTERNALNAME		"kstart.exe"
#define VI_ORIGINALFILENAME	"kstart.exe"
#endif

#ifdef KXE_KAV64_EXE
#pragma message("\t    KXE_KAV64_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Security"
#define VI_INTERNALNAME		"kav64.exe"
#define VI_ORIGINALFILENAME	"kav64.exe"
#endif

#ifdef KXE_KAV32_EXE
#pragma message("\t    KXE_KAV32_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Security"
#define VI_INTERNALNAME		"kav32.exe"
#define VI_ORIGINALFILENAME	"kav32.exe"
#endif

#ifdef KXE_KXESCGUI_EXE
#pragma message("\t    KXE_KXESCGUI_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Security"
#define VI_INTERNALNAME		"kgui.exe"
#define VI_ORIGINALFILENAME	"kgui.exe"
#endif

#ifdef KXE_KXESCDOWN_EXE
#pragma message("\t    KXE_KXESCDOWN_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Security"
#define VI_INTERNALNAME		"kxescdown.exe"
#define VI_ORIGINALFILENAME	"kxescdown.exe"
#endif

#ifdef KXE_KAVMAIN_EXE
#pragma message("\t    KXE_KAVMAIN_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Security"
#define VI_INTERNALNAME		"kavmain.exe"
#define VI_ORIGINALFILENAME	"kavmain.exe"
#endif

#ifdef KXE_KOLHTML_DLL
#pragma message("\t    KXE_KOLHTML_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Online Service Module"
#define VI_INTERNALNAME		"kolhtml.dll"
#define VI_ORIGINALFILENAME	"kolhtml.dll"
#endif

#ifdef KXE_KAVMENU_DLL
#pragma message("\t    KXE_KAVMENU_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Security"
#define VI_INTERNALNAME		"kavmenu.dll"
#define VI_ORIGINALFILENAME	"kavmenu.dll"
#endif

#ifdef KXE_KAVMENU64_DLL
#pragma message("\t    KXE_KAVMENU64_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Security"
#define VI_INTERNALNAME		"kavmenu64.dll"
#define VI_ORIGINALFILENAME	"kavmenu64.dll"
#endif

#ifdef KXE_SETUP64_EXE
#pragma message("\t    KXE_SETUP64_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Security"
#define VI_INTERNALNAME		"setup64.exe"
#define VI_ORIGINALFILENAME	"setup64.exe"
#endif

#ifdef KXE_SETUPSHELL_EXE
#pragma message("\t    KXE_SETUPSHELL_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Security"
#define VI_INTERNALNAME		"setupshell.exe"
#define VI_ORIGINALFILENAME	"setupshell.exe"
#endif

#ifdef KXE_SETUPWIZEXT_EXE
#pragma message("\t    KXE_SETUPWIZEXT_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Security"
#define VI_INTERNALNAME		"setupwizext.exe"
#define VI_ORIGINALFILENAME	"setupwizext.exe"
#endif

#ifdef KXE_SYSTEM_SP_DLL
#pragma message("\t    KXERelease_SystemSP")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus System Service Provider"
#define VI_INTERNALNAME		"kxesyssp.dll"
#define VI_ORIGINALFILENAME	"kxesyssp.dll"
#endif

#ifdef KXE_ERROR_DLL
#pragma message("\t    KXE_ERROR_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Security"
#define VI_INTERNALNAME		"kxeerror.dll"
#define VI_ORIGINALFILENAME	"kxeerror.dll"
#endif

#ifdef WSS_KSECOREX
	#pragma message("\t    WSS_KSECOREX")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Scan Engine Core Extended"
	#define VI_INTERNALNAME		"ksecorex"
	#define VI_ORIGINALFILENAME	"ksecorex.dll"
#endif

#ifdef WSS_KSECORE
	#pragma message("\t    WSS_KSECORE")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Scan Engine Core"
	#define VI_INTERNALNAME		"ksecore"
	#define VI_ORIGINALFILENAME	"ksecore.dll"
#endif

#ifdef WSS_KSEDB
	#pragma message("\t    WSS_KSEDB")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Data Persistence"
	#define VI_INTERNALNAME		"ksedb"
	#define VI_ORIGINALFILENAME	"ksedb.dll"
#endif

#ifdef WSS_KSPKAS
	#pragma message("\t    WSS_KSEDB")
	#define VI_FILEDESCRIPTION	"Kingsoft Antispy Scanner"
	#define VI_INTERNALNAME		"kspkas"
	#define VI_ORIGINALFILENAME	"kspkas.dll"
#endif

#ifdef WSS_KSEDT
	#pragma message("\t    WSS_KSEDT")
	#define VI_FILEDESCRIPTION	"Kingsoft Scan Engine Diagnostic Console Tool"
	#define VI_INTERNALNAME		"ksedt"
	#define VI_ORIGINALFILENAME	"ksedt.exe"
#endif

#ifdef WSS_KSEDTN
	#pragma message("\t    WSS_KSEDTN")
	#define VI_FILEDESCRIPTION	"Kingsoft Scan Engine Diagnostic Tool"
	#define VI_INTERNALNAME		"ksedtn"
	#define VI_ORIGINALFILENAME	"ksedtn.dll"
#endif

#ifdef WSS_KSPDEMO
	#pragma message("\t    WSS_KSPDEMO")
	#define VI_FILEDESCRIPTION	"Kingsoft Demo Scanner"
	#define VI_INTERNALNAME		"kspdemo"
	#define VI_ORIGINALFILENAME	"kspdemo.dll"
#endif

#ifdef WSS_KSE4KAS2
	#pragma message("\t    WSS_KSE4KAS2")
	#define VI_FILEDESCRIPTION	"Kingsoft Scan Engine For Antispy2x"
	#define VI_INTERNALNAME		"kse4kas2"
	#define VI_ORIGINALFILENAME	"kse4kas2.dll"
#endif

#ifdef WSS_KSE4DEF
	#pragma message("\t    WSS_KSE4DEF")
	#define VI_FILEDESCRIPTION	"Kingsoft Scan Engine For Defender"
	#define VI_INTERNALNAME		"kse4def"
	#define VI_ORIGINALFILENAME	"kse4def.dll"
#endif

#ifdef WSS_KSE4KIS8
	#pragma message("\t    WSS_KSE4KIS8")
	#define VI_FILEDESCRIPTION	"Kingsoft Scan Engine For Kingsoft Internet Security 8.x"
	#define VI_INTERNALNAME		"kse4kis8"
	#define VI_ORIGINALFILENAME	"kse4kis8.dll"
#endif

#ifdef WSS_KSEJOB // size : 20000 工程名称的宏标记
#pragma message("\t    WSS_KSEJOB")
#define VI_FILEDESCRIPTION	"KXEngine Plat KSEJob library"	//该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"ksejob"				//该工程编译生成的文件的内部名称
#define VI_ORIGINALFILENAME	"ksejob.dll"			//该工程编译生成的文件的原始DLL文件名
#endif

#ifdef  WSS_KSREPAIR
#pragma message("\t    WSS_KASSYSREPAIRSCAN")
#define VI_FILEDESCRIPTION	"KSE System Scan  Repair Plat  library"	//该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"ksrepair"				            //该工程编译生成的文件的内部名称
#define VI_ORIGINALFILENAME	"ksrepair.dll"			                //该工程编译生成的文件的原始DLL文件名
#endif

#ifdef WSS_KSEFOPER
#pragma message("\t    WSS_KSEFOPER")
#define VI_FILEDESCRIPTION	"KSE File Operate Plat  library"	//该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"ksefoper"				        //该工程编译生成的文件的内部名称
#define VI_ORIGINALFILENAME	"ksefoper.dll"			            //该工程编译生成的文件的原始DLL文件名
#endif

#ifdef WSS_KSEMSCAN
#pragma message("\t    WSS_KSEMSCAN")
#define VI_FILEDESCRIPTION	"KSE Memory Scan library"	        //该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"ksemscan"				        //该工程编译生成的文件的内部名称
#define VI_ORIGINALFILENAME	"ksemscan.dll"			            //该工程编译生成的文件的原始DLL文件名
#endif

#ifdef WSS_KSEMEMSC
#pragma message("\t    WSS_KSEMEMSC")
#define VI_FILEDESCRIPTION	"KSE Memory Scan Expand library"	//该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"ksememsc"				        //该工程编译生成的文件的内部名称
#define VI_ORIGINALFILENAME	"ksememsc.dll"			            //该工程编译生成的文件的原始DLL文件名
#endif

#ifdef WSS_KSEMEMSCSTUB
#pragma message("\t    WSS_KSEMEMSCSTUB")
#define VI_FILEDESCRIPTION	"KSE Memory ProcessorStubd library"	//该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"ksememscstub"				        //该工程编译生成的文件的内部名称
#define VI_ORIGINALFILENAME	"ksememscstub.dll"			            //该工程编译生成的文件的原始DLL文件名 
#endif

#ifdef WSS_KSPFENG
#pragma message("\t    WSS_KSPFENG")
#define VI_FILEDESCRIPTION	"KSE Scanner and Processor for File Engine library"	//该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"kspfeng"				        	//该工程编译生成的文件的内部名称
#define VI_ORIGINALFILENAME	"kspfeng.dll"			            		//该工程编译生成的文件的原始DLL文件名 
#endif

#ifdef WSS_KSSINGLE
#pragma message("\t    WSS_KSSINGLE")
#define VI_FILEDESCRIPTION	"KSE Single library"	            //该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"kssingle"				        //该工程编译生成的文件的内部名称
#define VI_ORIGINALFILENAME	"kssingle.dll"			            //该工程编译生成的文件的原始DLL文件名 
#endif

#ifdef WSS_KSESDK
#pragma message("\t    WSS_KSESDK")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Scan Engine Plugin SDK"	//该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"ksesdk"				        //该工程编译生成的文件的内部名称
#define VI_ORIGINALFILENAME	"ksesdk.dll"			            	//该工程编译生成的文件的原始DLL文件名 
#endif

#ifdef KXE_ARCHIVE // size : 20000 工程名称的宏标记
	#pragma message("\t    KXE_ARCHIVE_MANAGER")
	#define VI_FILEDESCRIPTION	"KXEngine Archive Manager"
	#define VI_INTERNALNAME		"karchive"
	#define VI_ORIGINALFILENAME	"karchive.dat"
#endif

#ifdef KXE_CORE // size : 20000 工程名称的宏标记
	#pragma message("\t    KXE_CORE")
	#define VI_FILEDESCRIPTION	"KXEngine Core"
	#define VI_INTERNALNAME		"kaecore"
	#define VI_ORIGINALFILENAME	"kaecore.dat"
#endif

#ifdef KXE_COREA // size : 20000 工程名称的宏标记
	#pragma message("\t    KXE_COREA")
	#define VI_FILEDESCRIPTION	"KXEngine Core A"
	#define VI_INTERNALNAME		"kaecorea"
	#define VI_ORIGINALFILENAME	"kaecorea.dat"
#endif

#ifdef KXE_EXTEND // size : 20000 工程名称的宏标记
	#pragma message("\t    KXE_EXTEND")
	#define VI_FILEDESCRIPTION	"KXEngine Extend"
	#define VI_INTERNALNAME		"kaextend"
	#define VI_ORIGINALFILENAME	"kaextend.dat"
#endif

#ifdef KXE_EXTEND2 // size : 20000 工程名称的宏标记
	#pragma message("\t    KXE_EXTEND2")
	#define VI_FILEDESCRIPTION	"KXEngine Extend 2"
	#define VI_INTERNALNAME		"kaextend2"
	#define VI_ORIGINALFILENAME	"kaext2.dat"
#endif

#ifdef KXE_UNPACK // size : 20000 工程名称的宏标记
	#pragma message("\t    KXE_UNPACK")
	#define VI_FILEDESCRIPTION	"KXEngine Unpacker"
	#define VI_INTERNALNAME		"kaeunpak"
	#define VI_ORIGINALFILENAME	"kaeunpak.dat"
#endif

#ifdef KXE_KAPOLICY // size : 20000 工程名称的宏标记
	#pragma message("\t    KXE_KAPOLICY")
	#define VI_FILEDESCRIPTION	"KXEngine Policy Maker"
	#define VI_INTERNALNAME		"kapolicy"
	#define VI_ORIGINALFILENAME	"kapolicy.exe"
#endif

#ifdef KXE_VIRUSNAME // size : 20000 工程名称的宏标记
	#pragma message("\t    KXE_VIRUSNAME")
	#define VI_FILEDESCRIPTION	"KXEngine Virus Name Accessor"
	#define VI_INTERNALNAME		"kaevname"
	#define VI_ORIGINALFILENAME	"kaevname.dat"
#endif

#ifdef KXE_OLEA // size : 20000 工程名称的宏标记
	#pragma message("\t    KXE_OLEA")
	#define VI_FILEDESCRIPTION	"KXEngine Archive OLE"
	#define VI_INTERNALNAME		"kaeolea"
	#define VI_ORIGINALFILENAME	"kaeolea.dat"
#endif

#ifdef KXE_KAEARCHA // size : 20000 工程名称的宏标记
	#pragma message("\t    KXE_KAEARCHA")
	#define VI_FILEDESCRIPTION	"KXEngine Archive Add-on"
	#define VI_INTERNALNAME		"kaearcha"
	#define VI_ORIGINALFILENAME	"kaearcha.dat"
#endif

#ifdef KXE_COREF // size : 20000 工程名称的宏标记
	#pragma message("\t    KXE_COREF")
	#define VI_FILEDESCRIPTION	"KXEngine Core F"
	#define VI_INTERNALNAME		"kaecoref"
	#define VI_ORIGINALFILENAME	"kaecoref.dat"
#endif

#ifdef KXE_KAEARCHB // size : 20000 工程名称的宏标记
	#pragma message("\t    KXE_KAEARCHB")
	#define VI_FILEDESCRIPTION	"KXEngine Archive Basic"
	#define VI_INTERNALNAME		"kaearchb"
	#define VI_ORIGINALFILENAME	"kaearchb.dat"
#endif

#ifdef KXE_WFS // size : 20000 工程名称的宏标记
	#pragma message("\t    KXE_WFS")
	#define VI_FILEDESCRIPTION	"kse wfs"
	#define VI_INTERNALNAME		"wfs"
	#define VI_ORIGINALFILENAME	"wfs.dll"
#endif

#ifdef KXE_PESIGNSINFOGATHER // size : 20000 工程名称的宏标记
    #pragma message("\t    KXE_PESiGNSINFOGATHER")
    #define VI_FILEDESCRIPTION	"PE file signs information gather"
    #define VI_INTERNALNAME		"pesignsinfogather"
    #define VI_ORIGINALFILENAME	"pesignsinfogather.exe"
#endif

#ifdef KXE_IPC_BASE
    #pragma message("\t    KXEIPCDLL")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus IPC"
    #define VI_INTERNALNAME		"kxeipc.dll"
    #define VI_ORIGINALFILENAME	"kxeipc.dll"
#endif

#ifdef KXE_EXT_PS_FOR_KPP
    #pragma message("\t    KXERelease_ExtPredefStringForKpp")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Ext Predefined String for Kpp"
    #define VI_INTERNALNAME		"kxeepskpp.dll"
    #define VI_ORIGINALFILENAME	"kxeepskpp.dll"
#endif

#ifdef _KXE_GAME_PROTECT
    #pragma message("\t    KXERelease_GameProtect")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Game Protect"
    #define VI_INTERNALNAME		"kxegprotect.dll"
    #define VI_ORIGINALFILENAME	"kxegprotect.dll"
#endif

#ifdef _KXE_VEH_PROCESS
    #pragma message("\t    KXERelease_VEHProcess")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus VEH Process"
    #define VI_INTERNALNAME		"kxeveh.dll"
    #define VI_ORIGINALFILENAME	"kxeveh.dll"
#endif

#ifdef KSSAFEBOX_KXESP
    #pragma message("\t    KSSAFEBOX_KXESP")
    #define VI_FILEDESCRIPTION	"Kingsoft Password Protector Module"
    #define VI_INTERNALNAME		"kxesp"
    #define VI_ORIGINALFILENAME	"kxesp.dll"
#endif

#ifdef KSSAFEBOX_REUPDATE
    #pragma message("\t    KSSAFEBOX_REUPDATE")
    #define VI_FILEDESCRIPTION	"Kingsoft Password Protector Module"
    #define VI_INTERNALNAME		"reupdate"
    #define VI_ORIGINALFILENAME	"reupdate.exe"
#endif

#ifdef KSSAFEBOX_UPDATE
    #pragma message("\t    KSSAFEBOX_UPDATE")
    #define VI_FILEDESCRIPTION	"Kingsoft Password Protector Module"
    #define VI_INTERNALNAME		"ksupdate"
    #define VI_ORIGINALFILENAME	"ksupdate.exe"
#endif

#ifdef KSSAFEBOX_UPLIVE
    #pragma message("\t    KSSAFEBOX_UPLIVE")
    #define VI_FILEDESCRIPTION	"Kingsoft Password Protector Module"
    #define VI_INTERNALNAME		"uplive"
    #define VI_ORIGINALFILENAME	"uplive.dll"
#endif

#ifdef KISUPTRAY_DLL
#pragma message("\t    KISUPTRAY_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Uplive Tray Icon Module"
#define VI_INTERNALNAME		"kisuptray"
#define VI_ORIGINALFILENAME	"kisuptray.dll"
#endif

#ifdef KSSAFEBOX_KPP2UPREPORT
    #pragma message("\t    KSSAFEBOX_KPP2UPREPORT")
    #define VI_FILEDESCRIPTION	"Kingsoft Password Protector Module"
    #define VI_INTERNALNAME		"kpp2upreport"
    #define VI_ORIGINALFILENAME	"kpp2upreport.exe"
#endif

#ifdef KPP_POP
    #pragma message("\t    KSSAFEBOX_KPP2UPREPORT")
    #define VI_FILEDESCRIPTION	"Kingsoft Password Protector Module"
    #define VI_INTERNALNAME		"kpppop"
    #define VI_ORIGINALFILENAME	"kpppop.dll"
#endif

#ifdef KPP_MAIN
    #pragma message("\t    KPP_MAIN")
    #define VI_FILEDESCRIPTION	"Kingsoft Password Protector Module"
    #define VI_INTERNALNAME		"KPPMain"
    #define VI_ORIGINALFILENAME	"kppmain.exe"
#endif

#ifdef KXE_DE_CLIENT_DLL
    #pragma message("\t    KXE_DE_CLIENT_DLL")
    #define VI_FILEDESCRIPTION	"Kingsoft Defend Engine Client"
    #define VI_INTERNALNAME		"kxedeclient"
    #define VI_ORIGINALFILENAME	"kxedeclient.dll"
#endif

#ifdef KXE_DE_SP_DLL
    #pragma message("\t    KXE_DE_SP_DLL")
    #define VI_FILEDESCRIPTION	"Kingsoft Defend Engine service provider"
    #define VI_INTERNALNAME		"kxedesp"
    #define VI_ORIGINALFILENAME	"kxedesp.dll"
#endif

#ifdef KXE_GAME_PROTECT_DLL
    #pragma message("\t    KXE_GAME_PROTECT_DLL")
    #define VI_FILEDESCRIPTION	"Kingsoft Defend Engine Game protect processor"
    #define VI_INTERNALNAME		"kxegprotect"
    #define VI_ORIGINALFILENAME	"kxegprotect.dll"
#endif

#ifdef KXE_DRIVER_DEFEND_DLL
    #pragma message("\t    KXE_DRIVER_DEFEND_DLL")
    #define VI_FILEDESCRIPTION	"Kingsoft Defend Engine Driver Defend processor"
    #define VI_INTERNALNAME		"driverprotect"
    #define VI_ORIGINALFILENAME	"driverprotect.dll"
#endif

#ifdef KXE_FILE_CHECK_DLL
    #pragma message("\t    KXE_FILE_CHECK_DLL")
    #define VI_FILEDESCRIPTION	"Kingsoft Defend Engine File Checker"
    #define VI_INTERNALNAME		"kaefilecheck"
    #define VI_ORIGINALFILENAME	"kaefilecheck.dll"
#endif

#ifdef KXE_FILE_CHECK_TOOL
    #pragma message("\t    KXE_FILE_CHECK_TOOL")
    #define VI_FILEDESCRIPTION	"Kingsoft Defend Engine File Check Tool"
    #define VI_INTERNALNAME		"kaefilechecktools"
    #define VI_ORIGINALFILENAME	"kaefilechecktools.dll"
#endif

#ifdef KPP_SCAN
    #pragma message("\t    KPP_SCAN")
    #define VI_FILEDESCRIPTION	"Kingsoft Scan Engine File Check Tool"
    #define VI_INTERNALNAME		"kppscan"
    #define VI_ORIGINALFILENAME	"kppscan.dll"
#endif


#ifdef _IKXE_REG_
#pragma message("\t    _IKXE_REG_")
#define VI_FILEDESCRIPTION	"Kingsoft Defend Engine Reg Tool"
#define VI_INTERNALNAME		"ikxereg"
#define VI_ORIGINALFILENAME	"ikxereg.exe"
#endif

#ifdef _TEST_KXE_DECS
#pragma message("\t    _TEST_KXE_DECS")
#define VI_FILEDESCRIPTION	"Kingsoft Defend Engine install Tool"
#define VI_INTERNALNAME		"deinstall"
#define VI_ORIGINALFILENAME	"deinstall.exe"
#endif

#ifdef KXESCAN_KASENGINE
#pragma message("\t    KXESCAN_KASENGINE")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Engine"
#define VI_INTERNALNAME		"kasengine"
#define VI_ORIGINALFILENAME	"kasengine.dll"
#endif

#ifdef KXESCAN_KASLUA
#pragma message("\t    KXESCAN_KASLUA")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Engine"
#define VI_INTERNALNAME		"kaslua"
#define VI_ORIGINALFILENAME	"kaslua.dll"
#endif

#ifdef KXEDEFEND_KAEAUTORUNEX
#pragma message("\t    KXEDEFEND_KAEAUTORUNEX")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Engine"
#define VI_INTERNALNAME		"kaeautorunex"
#define VI_ORIGINALFILENAME	"kaeautorunex.dll"
#endif

#ifdef KXE_DE_SAFEAPI_SYS
    #pragma message("\t    KXERelease_DefendEngineSafeApi")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Defend Engine Safe Module"
    #define VI_INTERNALNAME		"kavsafe"
    #define VI_ORIGINALFILENAME	"kavsafe.sys"
#endif

#ifdef KXE_DE_BOOTCLEAN_SYS
#pragma message("\t    KXERelease_DefendEngineBootClean")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Defend Engine Bootclean Module"
#define VI_INTERNALNAME		"kavbootc"
#define VI_ORIGINALFILENAME	"kavbootc.sys"
#endif

#ifdef KXE_DE_DEVCONTROL
#pragma message("\t    KXERelease_DefendEngineControl")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Defend Engine Control"
#define VI_INTERNALNAME		"kavdevc"
#define VI_ORIGINALFILENAME	"kavdevc.dll"
#endif

#ifdef KISM_KPOPCLIENT_DLL
	#pragma message("\t    KISM_KPOPCLIENT_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft Common Module"
	#define VI_INTERNALNAME		"kpopclient"
	#define VI_ORIGINALFILENAME	"kpopclient.dll"
#endif

#ifdef KISM_KPOPUICENTER_DLL
#pragma message("\t    KISM_KPOPUICENTER_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Common Module"
#define VI_INTERNALNAME		"kpopuicenter"
#define VI_ORIGINALFILENAME	"kpopuicenter.dll"
#endif

#ifdef KISM_KHTMLFRAME_DLL
	#pragma message("\t    KISM_KHTMLFRAME_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft Html Module"
	#define VI_INTERNALNAME		"khtmlframe"
	#define VI_ORIGINALFILENAME	"khtmlframe.dll"
#endif

#ifdef KISM_KPOPSERVER
	#pragma message("\t    KISM_KPOPSERVER")
	#define VI_FILEDESCRIPTION	"Kingsoft Pop Service Provider"
	#define VI_INTERNALNAME		"kpopserver"
	#define VI_ORIGINALFILENAME	"kpopserver.exe"
#endif

#ifdef KISM_KPOPUI
	#pragma message("\t    KISM_KPOPUI")
	#define VI_FILEDESCRIPTION	"Kingsoft Pop Module"
	#define VI_INTERNALNAME		"kpopui"
	#define VI_ORIGINALFILENAME	"kpopui.exe"
#endif

#ifdef KXE_KCOLLECT
	//#pragma comment(linker, "/base:0x4DA00000")   
	#pragma message("\t    KXE_KCOLLECT")
	#define VI_FILEDESCRIPTION	"KCollect Data"	//该工程编译生成的文件的文件描述
	#define VI_INTERNALNAME		"KCOLLECT"				//该工程编译生成的文件的内部名称
	#define VI_ORIGINALFILENAME	"kcollect.exe"			//该工程编译生成的文件的原始DLL文件名
#endif

#ifdef KXEFWSSERV_EXE
#pragma message("\t    KXEFWSSERV_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Filemon Service Along With Subsystems Of White List And Scanning"	//该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"KXEFWSSERV"
#define VI_ORIGINALFILENAME	"kxefwsserv.exe"
#endif

#ifdef KXE_ACTIVITY_DLL
#pragma message("\t    KXE_ACTIVITY_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus activity"	//该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"kxesta.dll"
#define VI_ORIGINALFILENAME	"kxesta.dll"
#endif

#ifdef KXE_EXTKBS_EXT
#pragma message("\t    KXE_EXTKBS_EXT")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Collection"	//该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"extkbs.ext"
#define VI_ORIGINALFILENAME	"extkbs.ext"
#endif

#ifdef KXE_EXTKFS_EXT
#pragma message("\t    KXE_EXTKFS_EXT")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Collection"	//该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"extkfs.ext"
#define VI_ORIGINALFILENAME	"extkfs.ext"
#endif

#ifdef KXE_EXTKIS_EXT
#pragma message("\t    KXE_EXTKIS_EXT")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Collection"	//该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"extkis.ext"
#define VI_ORIGINALFILENAME	"extkis.ext"
#endif

#ifdef KXE_EXTKLG_EXT
#pragma message("\t    KXE_EXTKLG_EXT")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Collection"	//该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"extklg.ext"
#define VI_ORIGINALFILENAME	"extklg.ext"
#endif

#ifdef KXE_EXTKUI_EXT
#pragma message("\t    KXE_EXTKUI_EXT")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Collection"	//该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"extkui.ext"
#define VI_ORIGINALFILENAME	"extkui.ext"
#endif

#ifdef KXE_KICENGIN_DLL
#pragma message("\t    KXE_KICENGIN_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Collection"	//该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"kicengin.dll"
#define VI_ORIGINALFILENAME	"kicengin.dll"
#endif

#ifdef KXE_KICTASK_DLL
#pragma message("\t    KXE_KICTASK_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Collection"	//该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"kictask.dll"
#define VI_ORIGINALFILENAME	"kictask.dll"
#endif

#ifdef KXE_PARSER_DLL
#pragma message("\t    KXE_PARSER_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Collection"	//该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"parser.dll"
#define VI_ORIGINALFILENAME	"parser.dll"
#endif

#ifdef KXE_KXECO_DLL
#pragma message("\t    KXE_KXECO_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Collection"	//该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"kxeco.dll"
#define VI_ORIGINALFILENAME	"kxeco.dll"
#endif

#ifdef KXE_KXECOSP_DLL
#pragma message("\t    KXE_KXECOSP_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Collection"	//该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"kxecosp.dll"
#define VI_ORIGINALFILENAME	"kxecosp.dll"
#endif

#ifdef KIS_KISEXPORT_DLL
#pragma message("\t    KIS_KISEXPORT_DLL
#define VI_FILEDESCRIPTION	"Kingsoft Export"
#define VI_INTERNALNAME		"KISEXPORT"
#define VI_ORIGINALFILENAME	"kisexport.dll"
#endif

#ifdef KIS_KSAPPFRM_DLL
#pragma message("\t    KIS_KSAPPFRM_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft AppHtml Frame"
#define VI_INTERNALNAME		"KSAPPFRM"
#define VI_ORIGINALFILENAME	"ksappfrm.dll"
#endif

#ifdef KIS_KSTOOLS_DLL
#pragma message("\t    KIS_KSTOOLS_DLL")
#define VI_FILEDESCRIPTION	"KXEngine KsTools"	   
#define VI_INTERNALNAME		"KsTools"		
#define VI_ORIGINALFILENAME	"KsTools.dll"			
#endif

#ifdef KIS_MAIN_EXE
#pragma message("\t    KIS_MAIN_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft KIS Main"
#define VI_INTERNALNAME		"KISMAIN"
#define VI_ORIGINALFILENAME	"kismain.exe"
#endif

#ifdef KIS_DEFENDMON_DLL
#pragma message("\t    KIS_DEFENDMON_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft KIS Main"
#define VI_INTERNALNAME		"DEFENDMON"
#define VI_ORIGINALFILENAME	"defendmon.dll"
#endif

#ifdef KIS_QUESTIONFEEDBACK_DLL
#pragma message("\t    KIS_QUESTIONFEEDBACK_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft KIS Main"
#define VI_INTERNALNAME		"QUESTIONFEEDBACK"
#define VI_ORIGINALFILENAME	"qsfb.dll"
#endif

#ifdef KSA_MAIN_EXE
#pragma message("\t    KSA_MAIN_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft KSA Main"
#define VI_INTERNALNAME		"KSAMAIN"
#define VI_ORIGINALFILENAME	"ksamain.exe"
#endif

#ifdef KXE_PlATMAIN 
	#pragma message("\t    KXE_PLAT_MAIN")
	#define VI_FILEDESCRIPTION	"KXEngine Plat Main"	//该工程编译生成的文件的文件描述
	#define VI_INTERNALNAME		"KXMAIN"				//该工程编译生成的文件的内部名称
	#define VI_ORIGINALFILENAME	"kxemain.exe"			//该工程编译生成的文件的原始DLL文件名
#endif

#ifdef KSA_KSAENGINE_DLL
#pragma message("\t    KSA_KSAENGINE_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Security Analysis Module"
#define VI_INTERNALNAME		"ksaengine"
#define VI_ORIGINALFILENAME	"ksaengine.dll"
#endif

#ifdef KSA_KSAMAIN_EXE
#pragma message("\t    KSA_KSAMAIN_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Security Analysis Module"
#define VI_INTERNALNAME		"ksaserver"
#define VI_ORIGINALFILENAME	"ksaserver.exe"
#endif

#ifdef KIS_UISP_EXE
#pragma message("\t    KIS_UISP_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Security UI Module"
#define VI_INTERNALNAME		"kisuisp"
#define VI_ORIGINALFILENAME	"kisuisp.exe"
#endif


#ifdef KIS_SQLITE3_DLL
#pragma message("\t    KIS_SQLITE3_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Security Sqlite"
#define VI_INTERNALNAME		"sqlite3"
#define VI_ORIGINALFILENAME	"sqlite3.dll"
#endif

#ifdef KXE_JSON_V6DLL
#pragma message("\t    KXE_JSON_V6DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Security Analysis Module"
#define VI_INTERNALNAME		"jsonv6"
#define VI_ORIGINALFILENAME	"jsonv6.dll"
#endif

#ifdef K_NAME_INFO_DLL
#pragma message("\t    K_NAME_INFO_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Virus Name Infomation Module"
#define VI_INTERNALNAME		"knameinfo"
#define VI_ORIGINALFILENAME	"knameinfo.dll"
#endif


#ifdef K_NAME_INFO_SDK_DLL
#pragma message("\t    K_NAME_INFO_SDK_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Virus Name-Translate SDK"
#define VI_INTERNALNAME		"knameinfosdk"
#define VI_ORIGINALFILENAME	"knameinfosdk.dll"
#endif

#ifdef K_NAME_INFO_SP_DLL
#pragma message("\t    K_NAME_INFO_SP_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Virus Name-Translate Service Provider"
#define VI_INTERNALNAME		"knameinfosp"
#define VI_ORIGINALFILENAME	"knameinfosp.dll"
#endif

#ifdef KISFRM_EXE
	#pragma message("\t    KISFRM_EXE")
	#define VI_FILEDESCRIPTION	"Kingsoft Internet Security Gui Frame"
	#define VI_INTERNALNAME		"kisfrm"
	#define VI_ORIGINALFILENAME	"kisfrm.exe"
#endif

#ifdef KISCENTER_EXE
	#pragma message("\t    KISCENTER_EXE")
	#define VI_FILEDESCRIPTION	"Kingsoft Internet Security Center"
	#define VI_INTERNALNAME		"kiscenter"
	#define VI_ORIGINALFILENAME	"kiscenter.exe"
#endif



#ifdef SYSCLEANER_TKFRM_EXE
	#pragma message("\t    SYSCLEANER_TKFRM_EXE")
	#define VI_FILEDESCRIPTION	"Kingsoft Internet Security Toolkit"
	#define VI_INTERNALNAME		"tkfrm"
	#define VI_ORIGINALFILENAME	"tkfrm.dll"
#endif



#ifdef KISM_DUMP
//	#pragma comment(linker, "/base:0x45400000")
#pragma message("\t    KISM_DUMP")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Dump Collect Library"
#define VI_INTERNALNAME		"dump"
#define VI_ORIGINALFILENAME	"dump.dll"
#endif

#ifdef KISM_KPFWFRM
//	#pragma comment(linker, "/base:0x45600000")
#pragma message("\t    KISM_KPFWFRM")
#define VI_FILEDESCRIPTION	"Kingsoft persenal firewal"
#define VI_INTERNALNAME		"kpfwfrm"
#define VI_ORIGINALFILENAME	"kpfwfrm.exe"
#endif

#ifdef KISM_KPFWMAIN
//	#pragma comment(linker, "/base:0x45600000")
#pragma message("\t    KISM_KPFWMAIN")
#define VI_FILEDESCRIPTION	"Kingsoft Persenal Firewall Main"
#define VI_INTERNALNAME		"kpfwmain"
#define VI_ORIGINALFILENAME	"kpfwmain.exe"
#endif

#ifdef KISM_KPFWTRAY
//	#pragma comment(linker, "/base:0x45600000")
#pragma message("\t    KISM_KPFWTRAY")
#define VI_FILEDESCRIPTION	"Kingsoft Persenal Firewall Tray"
#define VI_INTERNALNAME		"kpfwtray"
#define VI_ORIGINALFILENAME	"kpfwtray.exe"
#endif

#ifdef KPFW_SETUP
//	#pragma comment(linker, "/base:0x45800000")
#pragma message("\t    KPFW_SETUP")
#define VI_FILEDESCRIPTION	"Kingsoft Persenal Firewall Setup"
#define VI_INTERNALNAME		"setupkpfw"
#define VI_ORIGINALFILENAME	"setupkpfw.exe"
#endif

#ifdef KPFW_SWITCH
//	#pragma comment(linker, "/base:0x45800000")
#pragma message("\t    KPFW_SWITCH")
#define VI_FILEDESCRIPTION	"Kingsoft Persenal Firewall Switcher"
#define VI_INTERNALNAME		"switcher"
#define VI_ORIGINALFILENAME	"switcher.exe"
#endif

#ifdef KPFW_DRVINSTALL
//	#pragma comment(linker, "/base:0x43000000")
#pragma message("\t    KPFW_DRVINSTALL")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus FireWall DrvInstall"
#define VI_INTERNALNAME		"fwdrvinstall"
#define VI_ORIGINALFILENAME	"fwdrvinstall.exe"
#endif

#ifdef SERVICE_MANAGER
//	#pragma comment(linker, "/base:0x43000000")
#pragma message("\t    SERVICE_MANAGER")
#define VI_FILEDESCRIPTION	"Kingsoft Service Manager"
#define VI_INTERNALNAME		"kservicemgr"
#define VI_ORIGINALFILENAME	"kservicemgr.exe"
#endif


/////////////////////////////////////////////////////////////
//dll
#ifdef KISM_KISCOMM // size : 0x200000
	#pragma message("\t    KISM_KISCOMM")
	#define VI_FILEDESCRIPTION	"Kingsoft Internet Security Common Module"
	#define VI_INTERNALNAME		"kiscomm"
	#define VI_ORIGINALFILENAME	"kiscomm.dll"
#endif

#ifdef EVENTMGR_DLL
#pragma message("\t    EVENTMGR_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Internet Security Event Manager"
#define VI_INTERNALNAME		"eventmgr"
#define VI_ORIGINALFILENAME	"eventmgr.dll"
#endif

#ifdef EVENTREG_DLL
#pragma message("\t    EVENTREG_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Internet Security Event Register"
#define VI_INTERNALNAME		"eventreg"
#define VI_ORIGINALFILENAME	"eventreg.dll"
#endif

#ifdef KPFWSVR_EXE
#pragma message("\t    KPFWSVR_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Internet Security kpfw serivce"
#define VI_INTERNALNAME		"kpfwsvr"
#define VI_ORIGINALFILENAME	"kpfwsvr.exe"
#endif

#ifdef KISRPC_DLL
#pragma message("\t    KISRPC_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Internet Security RPC"
#define VI_INTERNALNAME		"kisrpc"
#define VI_ORIGINALFILENAME	"kisrpc.dll"
#endif

#ifdef KISSAFEMSN_DLL
#pragma message("\t    KISSAFEMSN_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Internet Security Safe Msn"
#define VI_INTERNALNAME		"kissafemsn"
#define VI_ORIGINALFILENAME	"kissafemsn.dll"
#endif

#ifdef MSIMG32_DLL_DLL
#pragma message("\t    MSIMG32_DLL_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Internet Security Safe Msn Assist"
#define VI_INTERNALNAME		"msimg32"
#define VI_ORIGINALFILENAME	"msimg32.dll"
#endif

// -------------------------------------------------------------------------
//sys
#ifdef FIREWALL_NDISFILT
	#pragma message("\t    FIREWALL_NDISFILT")
	#define VI_FILEDESCRIPTION	"Kingsoft Firewall NdisFilter Driver"
	#define VI_INTERNALNAME		"KNdisFilt"
	#define VI_ORIGINALFILENAME	"kndisfilt.sys"
#endif  // FIREWALL_NDISFILT


#ifdef FIREWALL_TDIFILT
	#pragma message("\t    FIREWALL_TDIFILT")
	#define VI_FILEDESCRIPTION	"Kingsoft Firewall TdiFilter Driver"
	#define VI_INTERNALNAME		"KTdiFilt"
	#define VI_ORIGINALFILENAME	"ktdifilt.sys"
#endif  // FIREWALL_TDIFILT

#ifdef FIREWALL_NDISHOOK
	#pragma message("\t    FIREWALL_NDISHOOK")
	#define VI_FILEDESCRIPTION	"Kingsoft Firewall NdisHook Driver"
	#define VI_INTERNALNAME		"KNdisHook"
	#define VI_ORIGINALFILENAME	"kndishook.sys"
#endif  // FIREWALL_NDISHOOK

#ifdef FIREWALL_ANTIARP
	#pragma message("\t    FIREWALL_ANTIARP")
	#define VI_FILEDESCRIPTION	"Kingsoft Firewall Antiarp Driver"
	#define VI_INTERNALNAME		"kantiarp"
	#define VI_ORIGINALFILENAME	"kantiarp.sys"
#endif  // FIREWALL_NDISHOOK

// -------------------------------------------------------------------------


#ifdef KISM_NETMON // size : 0x200000
//	#pragma comment(linker, "/base:0x41200000")   
	#pragma message("\t    KISM_NETMON")
	#define VI_FILEDESCRIPTION	"Kingsoft Firewall net moniter"
	#define VI_INTERNALNAME		"netmon"
	#define VI_ORIGINALFILENAME	"netmon.dll"
#endif

#ifdef KISM_NETFLUX // size : 0x200000
	// #pragma comment(linker, "/base:0x41200000")   
	#pragma message("\t    KISM_NETFLUX")
	#define VI_FILEDESCRIPTION	"Kingsoft Firewall net flux moniter"
	#define VI_INTERNALNAME		"netflux"
	#define VI_ORIGINALFILENAME	"netflux.dll"
#endif


#ifdef KISM_T3GTRAY // size : 0x200000  
	// #pragma comment(linker, "/base:0x41200000") 
	#pragma message("\t    KISM_T3GTRAY")
	#define VI_FILEDESCRIPTION	"Kingsoft Firewall NetFlux Tray"
	#define VI_INTERNALNAME		"t3gtray"
	#define VI_ORIGINALFILENAME	"t3gtray.dll"
#endif

#ifdef KISM_NETCORE // size : 0x200000
// #pragma comment(linker, "/base:0x41200000")   
	#pragma message("\t    KISM_NETCORE")
	#define VI_FILEDESCRIPTION	"Kingsoft Firewall net core component"
	#define VI_INTERNALNAME		"netcore"
	#define VI_ORIGINALFILENAME	"netcore.dll"
#endif

#ifdef KISM_AXHEARTBEAT // size : 0x200000
//	#pragma comment(linker, "/base:0x41200000")   
#pragma message("\t    KISM_ACHEARBEAT")
#define VI_FILEDESCRIPTION	"Kingsoft Firewall net heartbeat activex"
#define VI_INTERNALNAME		"axheartbeat"
#define VI_ORIGINALFILENAME	"axheartbeat.dll"
#endif

#ifdef KISM_AXSTATEBAR // size : 0x200000
//	#pragma comment(linker, "/base:0x41200000")   
#pragma message("\t    KISM_AXSTATEBAR")
#define VI_FILEDESCRIPTION	"Kingsoft Firewall net statebar activex"
#define VI_INTERNALNAME		"axstatebar"
#define VI_ORIGINALFILENAME	"axstatebar.dll"
#endif

#ifdef KISM_ALERTBAR // size : 0x200000
#pragma message("\t    KISM_ALERTBAR")
#define VI_FILEDESCRIPTION	"Kingsoft Net Flux Mgr Alertbar"
#define VI_INTERNALNAME		"alertbar"
#define VI_ORIGINALFILENAME	"alertbar.exe"
#endif

#ifdef FIREWALL_MAILMON
//	#pragma comment(linker, "/base:0x43B00000")
	#pragma message("\t    FIREWALL_MAILMON")
	#define VI_FILEDESCRIPTION	"Kingsoft Firewall Mail Monitor"
	#define VI_INTERNALNAME		"kmailmon"
	#define VI_ORIGINALFILENAME	"kmailmon.dll"
#endif

#ifdef FIREWALL_PROXY
//	#pragma comment(linker, "/base:0x44B00000")
	#pragma message("\t    FIREWALL_PROXY")
	#define VI_FILEDESCRIPTION	"Kingsoft Firewall Proxy"
	#define VI_INTERNALNAME		"fwproxy"
	#define VI_ORIGINALFILENAME	"fwproxy.dll"
#endif
	
#ifdef FIREWALL_DEMO_SVC
//	#pragma comment(linker, "/base:0x45B00000")
	#pragma message("\t    FIREWALL_DEMO_SVC")
	#define VI_FILEDESCRIPTION	"Kingsoft Firewall Demo Service"
	#define VI_INTERNALNAME		"fwsvc_demo"
	#define VI_ORIGINALFILENAME	"fwsvc_demo.dll"
#endif


#ifdef KISM_KPFWSHELL // size : 0x200000
//	#pragma comment(linker, "/base:0x46B00000")   
	#pragma message("\t    KISM_KPFWSHELL")
	#define VI_FILEDESCRIPTION	"Kingsoft Internet Security Personal Firewall Shell"
	#define VI_INTERNALNAME		"kpfwshell"
	#define VI_ORIGINALFILENAME	"kpfwshell.dll"
#endif

#ifdef SYSCLEANER_TOOLKIT_DLL
//	#pragma comment(linker, "/base:0x47B00000")
	#pragma message("\t    SYSCLEANER_TOOLKIT_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft SysCleaner Toolkit"
	#define VI_INTERNALNAME		"toolkit"
	#define VI_ORIGINALFILENAME	"toolkit.dll"
#endif


#ifdef SYSCLEANER_PROCESSMANAGER_DLL
//	#pragma comment(linker, "/base:0x48B00000")
	#pragma message("\t    SYSCLEANER_PROCESSMANAGER_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft SysCleaner ProcessManager"
	#define VI_INTERNALNAME		"ProcessManager"
	#define VI_ORIGINALFILENAME	"processmanager.dll"
#endif

#ifdef SYSCLEANER_VIEWDESCRIBE_DLL
//	#pragma comment(linker, "/base:0x49B00000")
	#pragma message("\t    SYSCLEANER_VIEWDESCRIBE_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft SysCleaner ViewDescribe"
	#define VI_INTERNALNAME		"ViewDescribe"
	#define VI_ORIGINALFILENAME	"viewdescribe.dll"
#endif


#ifdef KIS_TRAY_DLL
//	#pragma comment(linker, "/base:0x5AB00000")
	#pragma message("\t    KIS_TRAY_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft Internet Security TrayPopo Module"
	#define VI_INTERNALNAME		"kistray"
	#define VI_ORIGINALFILENAME	"kistray.dll"
#endif

#ifdef KIS_SCOREUI_DLL
//	#pragma comment(linker, "/base:0x5BB00000")
	#pragma message("\t    KIS_SCOREUI_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft Internet Security ScoreUI Module"
	#define VI_INTERNALNAME		"kisscoreui"
	#define VI_ORIGINALFILENAME	"kisscoreui.dll"
#endif

#ifdef KIS_SCORESVR_DLL
//	#pragma comment(linker, "/base:0x5CB00000")
	#pragma message("\t    KIS_SCORESVR_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft Internet Security Score Service"
	#define VI_INTERNALNAME		"scoresvr"
	#define VI_ORIGINALFILENAME	"scoresvr.dll"
#endif

#ifdef KIS_COMMON_EXTEND_DLL
//	#pragma comment(linker, "/base:0x5DB00000")
	#pragma message("\t    KIS_COMMON_EXTEND_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft Internet Security Command Extend"
	#define VI_INTERNALNAME		"kiscmnext"
	#define VI_ORIGINALFILENAME	"kiscmnext.dll"
#endif

#ifdef KIS_SYSCLEANER_COMMON_EXTEND_DLL
//	#pragma comment(linker, "/base:0x5EB00000")
	#pragma message("\t    KIS_SYSCLEANER_COMMON_EXTEND_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft SysCleaner Command Extend"
	#define VI_INTERNALNAME		"kscmnext"
	#define VI_ORIGINALFILENAME	"kscmnext.dll"
#endif

#ifdef KIS_SAFETYINFO_DLL
//	#pragma comment(linker, "/base:0x5FB00000")
	#pragma message("\t    KIS_SAFETYINFO_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft SysCleaner Safety Information"
	#define VI_INTERNALNAME		"safetyinfo"
	#define VI_ORIGINALFILENAME	"safetyinfo.dll"
#endif

#ifdef KIS_ONLINESCAN_DLL
//	#pragma comment(linker, "/base:0x60B00000")
	#pragma message("\t    KIS_ONLINESCAN_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft SysCleaner Online Scan"
	#define VI_INTERNALNAME		"onlinescan"
	#define VI_ORIGINALFILENAME	"onlinescan.dll"
#endif


#ifdef KIS_KANTIARPDEVC_DLL
//	#pragma comment(linker, "/base:0x61B00000")
	#pragma message("\t    KIS_KANTIARPDEVC_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft Internet Security AntiArp Driver Control"
	#define VI_INTERNALNAME		"kantiarpdevc"
	#define VI_ORIGINALFILENAME	"kantiarpdevc.dll"
#endif

#ifdef KIS_TOOLKIT_ANTORUNMAN_DLL
//	#pragma comment(linker, "/base:0x61B00000")
	#pragma message("\t    KIS_TOOLKIT_ANTORUNMAN_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft Internet Security Antorunman"
	#define VI_INTERNALNAME		"antorunman"
	#define VI_ORIGINALFILENAME	"antorunman.dll"
#endif

#ifdef KIS_TOOLKIT_SYSTEMREPAIR_DLL
//	#pragma comment(linker, "/base:0x61B00000")
	#pragma message("\t    KIS_TOOLKIT_SYSTEMREPAIR_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft Internet Security System Repair"
	#define VI_INTERNALNAME		"systemrepair"
	#define VI_ORIGINALFILENAME	"systemrepair.dll"
#endif

#ifdef KIS_TOOLKIT_LSPREPAIRER_DLL
//	#pragma comment(linker, "/base:0x61B00000")
	#pragma message("\t    KIS_TOOLKIT_LSPREPAIRER_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft Internet Security LSP Repair"
	#define VI_INTERNALNAME		"lsprepairer"
	#define VI_ORIGINALFILENAME	"lsprepairer.dll"
#endif

#ifdef KIS_TOOLKIT_TRACEERASER_DLL
//	#pragma comment(linker, "/base:0x47400000")   
#pragma message("\t    KISM_TRACKERASER")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy Trackeraser"
#define VI_INTERNALNAME		"trackeraser"
#define VI_ORIGINALFILENAME	"trackeraser.dll"
#endif

#ifdef KISM_KASEARCH
//	#pragma comment(linker, "/base:0x47400000")   
#pragma message("\t    KISM_KASEARCH")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy Kasearch"
#define VI_INTERNALNAME		"kasearch"
#define VI_ORIGINALFILENAME	"kasearch.dll"
#endif

#ifdef KIS_TOOLKIT_PTHDEFENSE_DLL
//	#pragma comment(linker, "/base:0x61B00000")
	#pragma message("\t    KIS_TOOLKIT_PTHDEFENSE_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft Internet Security T-Horse defense"
	#define VI_INTERNALNAME		"kpthdefense"
	#define VI_ORIGINALFILENAME	"kpthdefense.dll"
#endif

#ifdef KPFWSETUP_DLL
//#pragma comment(linker, "/base:0x61D00000")
#pragma message("\t    KPFWSETUP_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Internet Security Setup Component"
#define VI_INTERNALNAME		"kpfwsetup"
#define VI_ORIGINALFILENAME	"kpfwsetup.dll"
#endif

#ifdef KWEBSITEREP_DLL
//#pragma comment(linker, "/base:0x61F00000")
#pragma message("\t    KWEBSITEREP_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Internet Security WebSite Rep"
#define VI_INTERNALNAME		"kwebsiterep"
#define VI_ORIGINALFILENAME	"kwebsiterep.dll"
#endif

#ifdef KXE_SR_SP_DLL
//#pragma comment(linker, "/base:0x61F00000")
#pragma message("\t    KXE_SR_SP_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft System Repair Service Provider"
#define VI_INTERNALNAME		"kxesrsp"
#define VI_ORIGINALFILENAME	"kxesrsp.dll"
#endif

#ifdef KXE_COMPONENT_DLL
#pragma message("\t    KXE_COMPONENT_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Internet Security Component"
#define VI_INTERNALNAME		"component"
#define VI_ORIGINALFILENAME	"component.dll"
#endif

#ifdef KXE_COMPONENTMGR_DLL
#pragma message("\t    KXE_COMPONENTMGR_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Internet Security Component Manager"
#define VI_INTERNALNAME		"componentmgr"
#define VI_ORIGINALFILENAME	"componentmgr.dll"
#endif


#ifdef KXE_MISC_SP_DLL
#pragma message("\t    KXE_MISC_SP_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Misc Service Provider"
#define VI_INTERNALNAME		"kxmiscsp.dll"
#define VI_ORIGINALFILENAME	"kxmiscsp.dll"
#endif

#ifdef KPFWTRAY_DLL
#pragma message("\t    KXEKPFWTRAY_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Personal FireWall Tray"
#define VI_INTERNALNAME		"kpfwtray"
#define VI_ORIGINALFILENAME	"kpfwtray.dll"
#endif

#ifdef KXE_T3GMAIN_EXE
#pragma message("\t    KXE_T3GMAIN_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft 3G Traffice Manager"
#define VI_INTERNALNAME		"t3gmain"
#define VI_ORIGINALFILENAME	"t3gmain.exe"
#endif

#ifdef KXE_SC_TRAY_EXE
	#pragma message("\t   KXE_SC_TRAY_EXE")
	#define VI_FILEDESCRIPTION	"KXEngine Security Center Tray manager"
	#define VI_INTERNALNAME		"kxetray.exe"
	#define VI_ORIGINALFILENAME	"kxetray.exe"
#endif

#ifdef KXE_SC_SP_DLL
#pragma message("\t   KXE_SC_SP_DLL")
#define VI_FILEDESCRIPTION	"KXEngine Security Center Service Provider"
#define VI_INTERNALNAME		"kxescsp"
#define VI_ORIGINALFILENAME	"kxescsp.dll"
#endif

#ifdef KS_SOFT_MGR_DLL
#pragma message("\t   KS_SOFT_MGR_DLL")
#define VI_FILEDESCRIPTION	"KingSoft Soft Manager"
#define VI_INTERNALNAME		"kssmgr"
#define VI_ORIGINALFILENAME	"kssmgr.dll"
#endif
// -------------------------------------------------------------------------

#ifdef KISM_KSINFO
#pragma message("\t    KISM_KSINFO")
#define VI_FILEDESCRIPTION	"Kingsoft AntiVirus ksinfo"
#define VI_INTERNALNAME		"ksinfo"
#define VI_ORIGINALFILENAME	"ksinfo.exe"
#endif

#ifdef KISM_SETUPEXT
	#pragma message("\t    KISM_SETUPEXT")
	#define VI_FILEDESCRIPTION	"Kingsoft Install"
	#define VI_INTERNALNAME		"setupext"
	#define VI_ORIGINALFILENAME	"setupext.exe"
#endif

#ifdef KISM_INFOCC_DLL
//	#pragma comment(linker, "/base:0x4DA00000")
	#pragma message("\t    KISM_INFOCC")
	#define VI_FILEDESCRIPTION	"Kingsoft infoc"
	#define VI_INTERNALNAME		"infocc"
	#define VI_ORIGINALFILENAME	"infocc.dll"
#endif

#ifdef KISM_INFOCS_DLL
//	#pragma comment(linker, "/base:0x4DA00000")
	#pragma message("\t    KISM_INFOCS")
	#define VI_FILEDESCRIPTION	"Kingsoft infoc"
	#define VI_INTERNALNAME		"infocs"
	#define VI_ORIGINALFILENAME	"infocs.dll"
#endif

#ifdef KISM_INFOCP_DLL
//	#pragma comment(linker, "/base:0x4DA00000")
	#pragma message("\t    KISM_INFOCP")
	#define VI_FILEDESCRIPTION	"Kingsoft infocp"
	#define VI_INTERNALNAME		"infocp"
	#define VI_ORIGINALFILENAME	"infocp.dll"
#endif

#ifdef KISM_KINFOC_DLL
//	#pragma comment(linker, "/base:0x4DA00000")
#pragma message("\t    KISM_KINFOC")
	#define VI_FILEDESCRIPTION	"Kingsoft kinfoc"
	#define VI_INTERNALNAME		"kinfoc"
	#define VI_ORIGINALFILENAME	"kinfoc.dll"
#endif

#ifdef KISM_KUPDATEDATA
	#pragma message("\t    KISM_KUPDATEDATA")
	#define VI_FILEDESCRIPTION	"Kingsoft ksdinfo"	
	#define VI_INTERNALNAME		"ksdinfo"				
	#define VI_ORIGINALFILENAME	"ksdinfo.exe"			
#endif

#ifdef KISM_KINFOCUPDATA
	#pragma message("\t    KISM_KINFOCUPDATA")
	#define VI_FILEDESCRIPTION	"Kingsoft kupdata"	
	#define VI_INTERNALNAME		"kupdata"				
	#define VI_ORIGINALFILENAME	"kupdata.exe"			
#endif

#ifdef KXE_UPDATE_KISLIVX
	#pragma message("\t    KXE_UPDATE_KISLIVX")
	#define VI_FILEDESCRIPTION	"Kingsoft Security Online Upgrade"
	#define VI_INTERNALNAME		"kislivx.dll"
	#define VI_ORIGINALFILENAME	"kislivx.dll"
#endif

#ifdef KXE_UPDATE_COMPONENT
	#pragma message("\t    KXE_UPDATE_COMPONENT")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Update Component"
	#define VI_INTERNALNAME		"kisupco.dll"
	#define VI_ORIGINALFILENAME	"kisupco.dll"
#endif

#ifdef KXE_UPDATE_CS3
	#pragma message("\t    KXE_UPDATE_CS3")
	#define VI_FILEDESCRIPTION	"Kingsoft Security CS3 Controller"
	#define VI_INTERNALNAME		"kisupcs3.dll"
	#define VI_ORIGINALFILENAME	"kisupcs3.dll"
#endif

#ifdef KXE_UPDATE_KISUPFRM
	#pragma message("\t    KXE_UPDATE_KISUPFRM")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Update UI Frame"
	#define VI_INTERNALNAME		"kisupfrm.dll"
	#define VI_ORIGINALFILENAME	"kisupfrm.dll"
#endif

#ifdef KISM_KISLIVE
    #pragma message("\t    KISM_KISLIVE")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus Online Upgrade"
    #define VI_INTERNALNAME		"kislive.exe"
    #define VI_ORIGINALFILENAME	"kislive.exe"
#endif

#ifdef KISM_KISLIBUI
    #pragma message("\t    KISM_KISUPLIBUI")
    #define VI_FILEDESCRIPTION	"Kingsoft Kisuplib"
    #define VI_INTERNALNAME		"kisuplib.exe"
    #define VI_ORIGINALFILENAME	"kisuplib.exe"
#endif


#ifdef KISM_KISADDIN
	#pragma message("\t    KISM_KISADDIN")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus KisAddin"
	#define VI_INTERNALNAME		"KisAddin"
	#define VI_ORIGINALFILENAME	"KisAddin.exe"
#endif

#ifdef KISM_KPELIVE
	#pragma message("\t    KISM_KPELIVE")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus KPElive"
	#define VI_INTERNALNAME		"KPElive"
	#define VI_ORIGINALFILENAME	"kpelive.exe"
#endif

#ifdef KISM_KMINILIVE
	#pragma message("\t    KISM_KMINILIVE")
	#define VI_FILEDESCRIPTION	"Kingsoft Security Mini Uplive"
	#define VI_INTERNALNAME		"KMinilive"
	#define VI_ORIGINALFILENAME	"kminilive.exe"
#endif

#ifdef KISM_KISLIVE_LITE
	#pragma message("\t    KISM_KISLIVE_LITE")
	#define VI_FILEDESCRIPTION	"Kingsoft Security Online Upgrade"
	#define VI_INTERNALNAME		"kisupd"
	#define VI_ORIGINALFILENAME	"kisupd.exe"
#endif

#ifdef KISM_KISUPFIX
	#pragma message("\t    KISM_KISUPFIX")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Update Fix Helper"
	#define VI_INTERNALNAME		"kisupfix"
	#define VI_ORIGINALFILENAME	"kisupfix.exe"
#endif

#ifdef KISM_KISUPLIB
	#pragma message("\t    KISM_KISUPLIB")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Update Antivirus Library"
	#define VI_INTERNALNAME		"kisuplib"
	#define VI_ORIGINALFILENAME	"kisuplib.exe"
#endif

#ifdef KISM_KVALUEINST
	#pragma message("\t    KISM_KVALUEINST")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Value-added Installer"
	#define VI_INTERNALNAME		"kvalueinst.exe"
	#define VI_ORIGINALFILENAME	"kvalueinst.exe.exe"
#endif

#ifdef KISM_KLIVESETUP
	#pragma message("\t    KISM_KLIVESETUP")
	#define VI_FILEDESCRIPTION	"Kingsoft Security Online Installer"
	#define VI_INTERNALNAME		"klivesetup"
	#define VI_ORIGINALFILENAME	"klivesetup.exe"
#endif
#ifdef KISM_LEOSUPDATE
	#pragma message("\t    KISM_LEOSUPDATE")
	#define VI_FILEDESCRIPTION	"LEOSUpdate Application"
	#define VI_INTERNALNAME		"LEOSUpdate"
	#define VI_ORIGINALFILENAME	"LEOSUpdate.EXE"
#endif

#ifdef KISM_COOKIE
#pragma message("\t    KISM_COOKIE")
#define VI_FILEDESCRIPTION	"Kingsoft AntiVirus Cookie"
#define VI_INTERNALNAME		"kcookie"
#define VI_ORIGINALFILENAME	"kcookie.exe"
#endif

#ifdef KXE_COREB // size : 20000 工程名称的宏标记
	#pragma message("\t    KXE_COREB")
	#define VI_FILEDESCRIPTION	"KXEngine Core B"
	#define VI_INTERNALNAME		"kaecoreb"
	#define VI_ORIGINALFILENAME	"kaecoreb.dat"
#endif

#ifdef KXE_COREM // size : 20000 工程名称的宏标记
	#pragma message("\t    KXE_COREM")
	#define VI_FILEDESCRIPTION	"KXEngine Core M"
	#define VI_INTERNALNAME		"kaecorem"
	#define VI_ORIGINALFILENAME	"kaecorem.dat"
#endif

#ifdef KXE_HEURISTICS // size : 20000 工程名称的宏标记
	#pragma message("\t    KXE_HEURISTICS")
	#define VI_FILEDESCRIPTION	"KXEngine Heuristics"
	#define VI_INTERNALNAME		"kaecoreh"
	#define VI_ORIGINALFILENAME	"kaecoreh.dat"
#endif

#ifdef KBOOTCLEAN_INSTALL_32
#pragma message("\t    KBOOTCLEAN_INSTALL_32")
#define VI_FILEDESCRIPTION	"Kingsoft Boot Clean Engine install Tool"
#define VI_INTERNALNAME		"bcinstall32"
#define VI_ORIGINALFILENAME	"bcinstall32.exe"
#endif

#ifdef KBOOTCLEAN_INSTALL_64
#pragma message("\t    KBOOTCLEAN_INSTALL_64")
#define VI_FILEDESCRIPTION	"Kingsoft Boot Clean Engine install Tool"
#define VI_INTERNALNAME		"bcinstall64"
#define VI_ORIGINALFILENAME	"bcinstall64.exe"
#endif

#ifdef KXE_DE_BOOTCLEAN_64_SYS
#pragma message("\t    KXERelease_DefendEngineBootClean")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Defend Engine Bootclean Module"
#define VI_INTERNALNAME		"kavbootc64"
#define VI_ORIGINALFILENAME	"kavbootc64.sys"
#endif

#ifdef KPE_MAIN_EXE
#pragma message("\t    KPE_MAIN_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft KPE Main"
#define VI_INTERNALNAME		"KPEMAIN"
#define VI_ORIGINALFILENAME	"kpemain.exe"
#endif

#ifdef KXE_KISSCR 
//#pragma comment(linker, "/base:0x4DA00000")   
#pragma message("\t    KXE_KISSCR")
#define VI_FILEDESCRIPTION	"KIS Screen Saver"		//该工程编译生成的文件的文件描述
#define VI_INTERNALNAME		"kxescr"				//该工程编译生成的文件的内部名称
#define VI_ORIGINALFILENAME	"kxescr.scr"			//该工程编译生成的文件的原始DLL文件名
#endif

#ifdef K_KISLIVE_CFG_DLL
#pragma message("\t    K_KISLIVE_CFG_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Configuration DLL"
#define VI_INTERNALNAME		"kcfgdll"
#define VI_ORIGINALFILENAME	"kcfgdll.dll"
#endif


#ifdef KPIKFEX_EXE
#pragma message("\t    KPIKFEX_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Internet Security Center"
#define VI_INTERNALNAME		"kpikfex"
#define VI_ORIGINALFILENAME	"kpikfex.exe"
#endif


#ifdef KXE_UNIUPDATE_SP_DLL
    #pragma message("\t    KXERelease_UniUpdateSP")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus UniUpdate Service Provider"
    #define VI_INTERNALNAME		"kislivesp.dll"
    #define VI_ORIGINALFILENAME	"kislivesp.dll"
#endif

#ifdef KXE_UNIUPDATE_SVC_SP_DLL
	#pragma message("\t    KXERelease_UniUpdateSvcSP")
	#define VI_FILEDESCRIPTION	"Kingsoft Antivirus UniUpdate Service Provider"
	#define VI_INTERNALNAME		"kisupsvc.dll"
	#define VI_ORIGINALFILENAME	"kisupsvc.dll"
#endif

#ifdef KXE_KSGMERGE_DLL
    #pragma message("\t    KXE_KSGMERGE_DLL")
    #define VI_FILEDESCRIPTION	"Kingsoft Antivirus ksg Merge"
    #define VI_INTERNALNAME		"ksgmerge.dll"
    #define VI_ORIGINALFILENAME	"ksgmerge.dll"
#endif

#ifdef KXE_SC_SP_DLL
#pragma message("\t   KXE_SC_SP_DLL")
#define VI_FILEDESCRIPTION	"KXEngine Security Center Service Provider"
#define VI_INTERNALNAME		"kxescsp"
#define VI_ORIGINALFILENAME	"kxescsp.dll"
#endif

#ifdef KISM_FILEDESTROYDISK
#pragma message("\t    KISM_FILEDESTROYDISK")
#define VI_FILEDESCRIPTION	"Kingsoft File Destroy Disk Helper"
#define VI_INTERNALNAME		"diskaccess"
#define VI_ORIGINALFILENAME	"diskaccess.dll"
#endif

#ifdef KISM_FILEDESTROYDLL
#pragma message("\t    KISM_FILEDESTROYDLL")
#define VI_FILEDESCRIPTION	"Kingsoft File Destroy Dll"
#define VI_INTERNALNAME		"kfiledestroy"
#define VI_ORIGINALFILENAME	"kfiledestroy.dll"
#endif

#ifdef KISM_FILEDESTROY
#pragma message("\t    KISM_FILEDESTROY")
#define VI_FILEDESCRIPTION	"Kingsoft AntiVirus File Destroy Plus"
#define VI_INTERNALNAME		"kfiledestroy"
#define VI_ORIGINALFILENAME	"kfiledestroy.exe"
#endif

#ifdef KISM_FILEDESTORYSYS
#pragma message("\t    KISM_FILEDESTORYSYS")
#define VI_FILEDESCRIPTION	"Kingsoft File Destroy Driver"
#define VI_INTERNALNAME		"kcrushersys"
#define VI_ORIGINALFILENAME	"kcrushersys.sys"
#endif

#ifdef KXE_BRWHELPR_DLL
#pragma message("\t    KXE_BRWHELPR_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Browser Setting Helper"
#define VI_INTERNALNAME		"brwhelpr.dll"
#define VI_ORIGINALFILENAME	"brwhelpr.dll"
#endif

#ifdef KXE_PPWIZ_EXE
#pragma message("\t    KXE_PPWIZ_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Passport Wizard"
#define VI_INTERNALNAME		"kxeppwiz.exe"
#define VI_ORIGINALFILENAME	"kxeppwiz.exe"
#endif

#ifdef KXE_SIGN_SP_EXE
#pragma message("\t    KXE_SIGN_SP_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Sign Service Provider"
#define VI_INTERNALNAME		"ksignsp.exe"
#define VI_ORIGINALFILENAME	"ksignsp.exe"
#endif

#ifdef KISM_PROCMGREX
#pragma message("\t    KISM_PROCMGREX")
#define VI_FILEDESCRIPTION	"Kingsoft AntiVirus Process Manager Plus"
#define VI_INTERNALNAME		"procmgrex"
#define VI_ORIGINALFILENAME	"procmgrex.exe"
#endif

#ifdef KUTILITIES_KUTLT
#pragma message("\t    KUTILITIES_KUTLT")
#define VI_FILEDESCRIPTION	"Kingsoft AntiVirus Kbox"
#define VI_INTERNALNAME		"kbox"
#define VI_ORIGINALFILENAME	"kbox.exe"
#endif

#ifdef KXE_LIBPROTOBUF
#pragma message("\t    KXE_LIBPROTOBUF")
#define VI_FILEDESCRIPTION	"Kingsoft AntiVirus protobuf"
#define VI_INTERNALNAME		"libprotobuf"
#define VI_ORIGINALFILENAME	"libprotobuf.dll"
#endif

#ifdef KXE_NETREGCOMP
#pragma message("\t    KXE_NETREGCOMP")
#define VI_FILEDESCRIPTION	"Kingsoft AntiVirus netregcomp"
#define VI_INTERNALNAME		"netregcomp"
#define VI_ORIGINALFILENAME	"netregcomp.exe"
#endif

//-------Kas define------------------------------------------------------------------
#ifdef KIS_ACCOUNTCHECK
#pragma message("\t    KIS_ACCOUNTCHECK")
#define VI_FILEDESCRIPTION	"Kingsoft Account Safe Checker"
#define VI_INTERNALNAME		"accountcheck"
#define VI_ORIGINALFILENAME	"accountcheck.dll"
#endif // KIS_ACCOUNTCHECK

#ifdef KIS_AVPCHECK
#pragma message("\t    KIS_AVPCHECK")
#define VI_FILEDESCRIPTION	"Kingsoft AntiVirus Product Checker"
#define VI_INTERNALNAME		"avpcheck"
#define VI_ORIGINALFILENAME	"avpcheck.dll"
#endif // KIS_AVPCHECK

#ifdef KIS_KASREALTIMESP
#pragma message("\t    KIS_KASREALTIMESP")
#define VI_FILEDESCRIPTION	"Kingsoft Realtime Protect"
#define VI_INTERNALNAME		"kasrtsp"
#define VI_ORIGINALFILENAME	"kasrtsp.dll"
#endif // KIS_KASREALTIMESP

#ifdef KIS_KASMALWARESP
#pragma message("\t    KIS_KASMALWARESP")
#define VI_FILEDESCRIPTION	"Kingsoft Malware Sp"
#define VI_INTERNALNAME		"kasmwsp"
#define VI_ORIGINALFILENAME	"kasmwsp.dll"
#endif // KIS_KASMALWARESP

#ifdef KIS_KASRTSERVICE
#pragma message("\t    KIS_KASRTSERVICE")
#define VI_FILEDESCRIPTION	"Kingsoft kasrtservice Dll"
#define VI_INTERNALNAME		"kasrtservice"
#define VI_ORIGINALFILENAME	"kasrtservice.dll"
#endif // KIS_KASRTSERVICE

#ifdef KIS_KASSP
#pragma message("\t    KIS_KASSP")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy Service Provider"
#define VI_INTERNALNAME		"kasserviceprovider"
#define VI_ORIGINALFILENAME	"kasserviceprovider.dll"
#endif // KIS_KASSP

#ifdef KIS_KASTRAYDLL
#pragma message("\t    KIS_KASTRAYDLL")
#define VI_FILEDESCRIPTION	"Kingsoft kasray Dll"
#define VI_INTERNALNAME		"kastraydll"
#define VI_ORIGINALFILENAME	"kastraydll.dll"
#endif // KIS_KASTRAYDLL

#ifdef KIS_KRTMAIN
#pragma message("\t    KIS_KRTMAIN")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy krtmain"
#define VI_INTERNALNAME		"krtmain"
#define VI_ORIGINALFILENAME	"krtmain.exe"
#endif // KIS_KRTMAIN

#ifdef KIS_KSAMAIN
#pragma message("\t    KIS_KSAMAIN")
#define VI_FILEDESCRIPTION	"Kingsoft Hot Fix"
#define VI_INTERNALNAME		"ksamain"
#define VI_ORIGINALFILENAME	"ksamain.exe"
#endif // KIS_KSAMAIN

#ifdef KIS_KSRMAIN
#pragma message("\t    KIS_KSRMAIN")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy Ksrmain"
#define VI_INTERNALNAME		"ksrmain"
#define VI_ORIGINALFILENAME	"ksrmain.exe"
#endif	// KIS_KSRMAIN

#ifdef KIS_MARK
#pragma message("\t    KIS_MARK")
#define VI_FILEDESCRIPTION	"Kingsoft Anti-Spyware"
#define VI_INTERNALNAME		"kasmain"
#define VI_ORIGINALFILENAME	"kasmain.exe"
#endif // KIS_MARK

#ifdef KIS_MARKMGR
#pragma message("\t    KIS_MARKMGR")
#define VI_FILEDESCRIPTION	"Kingsoft Antispy Mark Manager"
#define VI_INTERNALNAME		"markmgr"
#define VI_ORIGINALFILENAME	"markmgr.dll"
#endif // KIS_MARKMGR

#ifdef KIS_PATCHCHECK
#pragma message("\t    KIS_PATCHCHECK")
#define VI_FILEDESCRIPTION	"Kingsoft Patch Checker"
#define VI_INTERNALNAME		"patchcheck"
#define VI_ORIGINALFILENAME	"patchcheck.dll"
#endif // KIS_PATCHCHECK

#ifdef KIS_REGMON
#pragma message("\t    KIS_REGMON")
#define VI_FILEDESCRIPTION	"Kingsoft Register Monitor"
#define VI_INTERNALNAME		"regmon"
#define VI_ORIGINALFILENAME	"regmonitorproxy.dll"
#endif // KIS_REGMON

#ifdef KIS_SRCHECK
#pragma message("\t    KIS_SRCHECK")
#define VI_FILEDESCRIPTION	"Kingsoft System Repair Checker"
#define VI_INTERNALNAME		"systemrepaircheck"
#define VI_ORIGINALFILENAME	"systemrepaircheck.dll"
#endif // KIS_SRCHECK

#ifdef KIS_UDISKMON
#pragma message("\t    KIS_UDISKMON")
#define VI_FILEDESCRIPTION	"Kingsoft Udisk Monitor"
#define VI_INTERNALNAME		"udiskmon"
#define VI_ORIGINALFILENAME	"udiskmon.dll"
#endif // KIS_KSAMAIN

#ifdef KIS_RTMCHECK
#pragma message("\t    KIS_RTMCHECK")
#define VI_FILEDESCRIPTION	"Kingsoft Realtime Checker"
#define VI_INTERNALNAME		"realtimemoncheck"
#define VI_ORIGINALFILENAME	"realtimemoncheck.dll"
#endif // KIS_RTMCHECK

#ifdef KIS_KXESASP
#pragma message("\t    KIS_KXESASP")
#define VI_FILEDESCRIPTION	"Kingsoft Kxesasp"
#define VI_INTERNALNAME		"kxesasp"
#define VI_ORIGINALFILENAME	"Kxesasp.dll"
#endif // KIS_KXESASP

#ifdef KSR_SCAN_DLL
#pragma message("\t    KSR_SCAN_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft System Repair Scan"	
#define VI_INTERNALNAME		"ksrscan.dll"				
#define VI_ORIGINALFILENAME	"ksrscan.dll"			
#endif

#ifdef KISM_KNITRPT_DLL
	#pragma message("\t    KISM_KNITRPT_DLL")
	#define VI_FILEDESCRIPTION	"Kingsoft Common Module"
	#define VI_INTERNALNAME		"knitrpt"
	#define VI_ORIGINALFILENAME	"knitrpt.dll"
#endif

#ifdef KSM_KSMMAIN
//	#pragma comment(linker, "/base:0x43000000")
#pragma message("\t    KSM_KSMMAIN")
#define VI_FILEDESCRIPTION	"Kingsoft Software Manager"
#define VI_INTERNALNAME		"ksmmain"
#define VI_ORIGINALFILENAME	"ksmmain.exe"
#endif

// -------------------------------------------------------------------------
#ifdef KXE_SC_KUNIST_EXE
	#pragma message("\t    KXE_SC_KUNIST_EXE")
	#define VI_FILEDESCRIPTION	"Kingsoft AntiVirus Uninstall Tool"
	#define VI_INTERNALNAME		"KUnist"
	#define VI_ORIGINALFILENAME	"kunist.exe"
#endif

#ifdef KXE_KSTARTRUNS_DLL
#pragma message("\t    KXE_KSTARTRUNS_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft AntiVirus Autorun Tool"
#define VI_INTERNALNAME		"KStartRuns"
#define VI_ORIGINALFILENAME	"kstartruns.dll"
#endif

#ifdef KXE_KREPAIR_EXE
#pragma message("\t    KXE_KREPAIR_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft AntiVirus Auto Repair Tool"
#define VI_INTERNALNAME		"KRepair"
#define VI_ORIGINALFILENAME	"krepair.exe"
#endif

#ifdef ETM_KARETR
#pragma message("\t    KISM_INSTALLLAUNCH")
#define VI_FILEDESCRIPTION  "Kingsoft AntiVirus KARetriever"
#define VI_INTERNALNAME		"karetriever"
#define VI_ORIGINALFILENAME	"karetr.dll"
#endif

#ifdef KIS_FRAME_CSAN
#pragma message("\t    KIS_FRAME_CSAN")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Module"
#define VI_INTERNALNAME		"kisfscan.dll"
#define VI_ORIGINALFILENAME	"kisfscan.dll"
#endif

#ifdef KSC_KSWSCX_DLL
#pragma message("\t    KSWSCX_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft System Scan Cleaner"
#define VI_INTERNALNAME		"kswscx.dll"
#define VI_ORIGINALFILENAME	"kswscx.dll"
#endif
#ifdef KXE_KXEFRAMESP_DLL
#pragma message("\t    KXE_KXEFRAMESP_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft Framework Functional Service Provider"
#define VI_INTERNALNAME		"kxebscsp.dll"
#define VI_ORIGINALFILENAME	"kxebscsp.dll"
#endif

#ifdef KXE_KAVUSBKILL
#pragma message("\t    KXE_KAVUSBKILL")
#define VI_FILEDESCRIPTION  "Kingsoft AntiVirus Kavusbkill Module"
#define VI_INTERNALNAME		"kavusbkill"
#define VI_ORIGINALFILENAME	"kavusbkill.dll"
#endif

#ifdef KISM_FOUNDERCALLEX
#pragma message("\t    KISM_FOUNDERCALLEX")
#define VI_FILEDESCRIPTION	"Kingsoft Foundercall Module Plus"
#define VI_INTERNALNAME		"foundercallex"
#define VI_ORIGINALFILENAME	"foundercallex.dll"
#endif

#ifdef KISM_HPASYNC
#pragma message("\t    KISM_FOUNDERCALLEX")
#define VI_FILEDESCRIPTION	"Kingsoft Foundercall Module Plus"
#define VI_INTERNALNAME		"foundercallex"
#define VI_ORIGINALFILENAME	"foundercallex.dll"
#endif

#ifdef KIS_PLUGIN_DLL
#pragma message("\t    KIS_PLUGIN_DLL")
#define VI_FILEDESCRIPTION	"Kingsoft KIS Plugin"
#define VI_INTERNALNAME		"kplugin"
#define VI_ORIGINALFILENAME	"kplugin.dll"
#endif

#ifdef KIS_KEVENTLOG3
#pragma message("\t    KIS_KEVENTLOG3")
#define VI_FILEDESCRIPTION	"KXEngine KeventLog3"	   
#define VI_INTERNALNAME		"KeventLog3"		
#define VI_ORIGINALFILENAME	"KeventLog3.dll"			
#endif

#ifdef KXE_SCAN_PROC_CTRL_SP
#pragma message("\t    KXE_SCAN_PROC_CTRL_SP")
#define VI_FILEDESCRIPTION	"Kingsoft Scan Process Controller Service Provider"
#define VI_INTERNALNAME		"kxespc"
#define VI_ORIGINALFILENAME	"kxespc.dll"
#endif

#ifdef KXE_SCAN_PROC_EXE
#pragma message("\t    KXE_SCAN_PROC_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Scan Process"
#define VI_INTERNALNAME		"kscan"
#define VI_ORIGINALFILENAME	"kscan.exe"
#endif

#ifdef K_PRIVACY_EXE
#pragma message("\t    K_PRIVACY_EXE")
#define VI_FILEDESCRIPTION	"Kingsoft Privacy Protection"
#define VI_INTERNALNAME		"KPrivacy"
#define VI_ORIGINALFILENAME	"kprivacy.exe"
#endif

#ifdef KIS_SETUP_BUILDER
#pragma message("\t    KSB_SETUP_BUILDER")
#define VI_FILEDESCRIPTION	"Kingsoft Antivirus Setup Builder"
#define VI_INTERNALNAME		"kissetupbuilder.exe"
#define VI_ORIGINALFILENAME	"kissetupbuilder.exe"
#endif

#endif /* __BUILDCFG_BUILDCFG_H__ */
