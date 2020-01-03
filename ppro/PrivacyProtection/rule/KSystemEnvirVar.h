#ifndef __KGetSystemEnvirVar_h__
#define __KGetSystemEnvirVar_h__

#include <shlobj.h>

class KSystemEnvirVar
{
public:
	CString GetValue(LPCTSTR szVariable)
	{
		TCHAR  szFolderPath[MAX_PATH + 1] = {0}; 
		if (0 == _tcsicmp(szVariable, _T("%Desktop%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_DESKTOP, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%Internet%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_INTERNET, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%Programs%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_PROGRAMS, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%Controls%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_CONTROLS, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%Printers%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_PRINTERS, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%Personal%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_PERSONAL, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%Favorites%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_FAVORITES, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%Startup%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_STARTUP, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%Recent%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_RECENT, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%Sendto%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_SENDTO, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%Bitbucket%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_BITBUCKET, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%StartMenu%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_STARTMENU, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%Mydocuments%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_MYDOCUMENTS, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%Mymusic%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_MYMUSIC, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%Myvideo%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_MYVIDEO, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%Desktopdirectory%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_DESKTOPDIRECTORY, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%Drives%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_DRIVES, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%Network%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_NETWORK, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%Nethood%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_NETHOOD, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%Fonts%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_FONTS, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%Templates%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_TEMPLATES, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%CommonStartMenu%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_COMMON_STARTMENU, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%CommonPrograms%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_COMMON_PROGRAMS, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%CommonStartup%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_COMMON_STARTUP, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%LocalAppdate%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_LOCAL_APPDATA, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%CommonAppdata%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_COMMON_APPDATA, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%Windows%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_WINDOWS, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%System32%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_SYSTEM, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%ProgramFilesComm%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_PROGRAM_FILES_COMMON, 0);
		} else if (0 == _tcsicmp(szVariable, _T("%CommonDocuments%"))) {
			::SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_COMMON_DOCUMENTS, 0);
		}
		else
		{
			CString strVariable(szVariable);
			strVariable.Remove(_T('%'));

			TCHAR* szTemp = _tgetenv(strVariable);
			if (szTemp)
			{
				if (_tcsstr(szTemp, L"~"))
				{
					::GetLongPathNameW(szTemp, szFolderPath, MAX_PATH);
				}
				else
				{
					_tcsncpy(szFolderPath, szTemp, MAX_PATH);
				}
				szFolderPath[MAX_PATH] = 0;
			}
		}

		return szFolderPath;
	}

protected:

};

/*
"%commonappdata%","c:\documents and settings\all users\application data"
"%commondocuments%","c:\documents and settings\all users\documents"
"%commonprograms%","c:\documents and settings\all users\「开始」菜单\程序"
"%commonstartmenu%","c:\documents and settings\all users\「开始」菜单"
"%commonstartup%","c:\documents and settings\all users\「开始」菜单\程序\启动"
"%desktop%","c:\documents and settings\administrator\桌面"
"%desktopdirectory%","c:\documents and settings\administrator\桌面"
"%favorites%","c:\documents and settings\administrator\favorites"
"%fonts%","c:\windows\fonts"
"%localappdate%","c:\documents and settings\administrator\local settings\application data"
"%mydocuments%","d:\my documents"
"%mymusic%","d:\my documents\my music"
"%nethood%","c:\documents and settings\administrator\nethood"
"%personal%","d:\my documents"
"%programfilescomm%","c:\program files\common files"
"%programs%","c:\documents and settings\administrator\「开始」菜单\程序"
"%recent%","c:\documents and settings\administrator\recent"
"%sendto%","c:\documents and settings\administrator\sendto"
"%startmenu%","c:\documents and settings\administrator\「开始」菜单"
"%startup%","c:\documents and settings\administrator\「开始」菜单\程序\启动"
"%system32%","c:\windows\system32"
"%templates%","c:\documents and settings\administrator\templates"
"%windows%","c:\windows"
*/

#endif