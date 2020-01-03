
#ifndef CCLOSEWHQL_H_08_07_03
#define CCLOSEWHQL_H_08_07_03

//在 vista 下需要有 管理员权限才能正常使用

typedef struct tagEnumReturn
{
	TCHAR * tszName;
	BOOL   bRetCode;
}EnumReturn;

class CCloseWHQL
{

public:

	CCloseWHQL();
	~CCloseWHQL();

	//外部只需调用下面两个函数
	BOOL StartCloseThread();
	void EndCloseThread();

	void SetDlgName(LPTSTR lpDlgName);
	static DWORD WINAPI _CloseThread( LPVOID lpParameter );

protected:

	HANDLE m_hThread;

};

#endif //CCLOSEWHQL_H_08_07_03