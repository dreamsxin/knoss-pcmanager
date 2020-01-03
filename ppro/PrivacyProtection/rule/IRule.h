#ifndef __IRule_h__
#define __IRule_h__

#define PRINT_RULE_RESULT

enum KEMRuleResult
{
	emRuleResult_Async = -1,
	emRuleResult_Error = 0,				//出错了
	emRuleResult_Show = 1,				//违规访问，要显示的
	emRuleResult_Distrustful = 2,		//可疑的
	emRuleResult_Unknown = 3,			//未知的
	emRuleResult_Disscard = 4,			//忽略
	emRuleResult_Trust = 5,				//信任的
};

struct KAccessData
{
	TCHAR szProcPath[MAX_PATH];
	int nProcPathLen;
	int nProcDirLen;

	TCHAR szFilePath[MAX_PATH];
	int nFilePathLen;
	int nFileDirLen;
	int nFileExtLen;

	int nResult;
	TCHAR szInfo[100];
	void* pExtendData;

	KAccessData()
	{
		szProcPath[0] = 0;
		nProcPathLen = 0;
		nProcDirLen = -1;
		szFilePath[0] = 0;
		nFilePathLen = 0;
		nFileDirLen = -1;
		nFileExtLen = -1;
		nResult = -1;
		pExtendData = NULL;
	}
};

//#define PRINT_RULE_RESULT
#define DECLARE_RULE_NAME(name) virtual LPCTSTR Name(void){ return L#name; }

/*
ClassName	: IRule
Creator		: zhongzhendong
Brief		: 规则的接口
*/

class IRule
{
public:
	virtual LPCTSTR Name(void) = 0;
	virtual void Enable(BOOL bEnable){}
	virtual BOOL Match(KAccessData* pData) = 0;
	virtual void DbgPrint(void) = 0;
};

/*
规则列表：
	1.扩展名规则
	2.进程与文件同目录规则
	3.文件路径规则
	4.文件目录规则
	5.进程路径规则
	6.进程目录规则
	7.扩展名关联进程与自定义扩展名关联进程
	8.自定义目录关联进程
	9.数字签名规则
	10.自定义文件关联进程规则
*/

struct KProcName
{
	CString strProcName;
	CStringA strSign;
	CString strFileName;
};

#endif