// kavmain.h
#ifndef	_PRIVACY_PROTECTION_GUI_H_
#define _PRIVACY_PROTECTION_GUI_H_
#include <atlfile.h>
#include "KLogic.h"


class KAppModule
	: public CAppModule
{
public:
	KAppModule()
		: m_hModRichEdit2(NULL)
	{
		m_bQuit = false;
	}


	HRESULT		Init(HINSTANCE hInstance);
	void		Uninit();
	HRESULT		Run();
	BOOL		CheckInstance();
	void		SetActiveWindow(HWND hWndActive);
	BOOL		ProcessCommandLine(void);

	


protected:
	void		_InitUIResource();


private:
	HMODULE				m_hModRichEdit2;
	bool				m_bQuit;

	typedef struct _K_Instance_Data
	{
		HWND hActiveWnd;
	} K_Instance_Data;

	CAtlFileMapping<K_Instance_Data> m_mapping;



};

extern KAppModule _Module;


#endif	// #ifndef	_PRIVACY_PROTECTION_GUI_H_
