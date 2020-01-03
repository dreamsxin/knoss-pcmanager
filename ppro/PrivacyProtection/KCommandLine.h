#ifndef __KCommandLine_h__
#define __KCommandLine_h__

#include <atlstr.h>
#include <vector>

typedef struct tagCmdParamData
{
	CString strCmd;
	std::vector<CString> vecParam;
} KCmdParamData;

class KCommandLine
{
public:
	KCommandLine()
	{
		int _argc = __argc;
		TCHAR** _argv = __targv;
		KCmdParamData data;

		data.strCmd = _argv[0];

		for (int i = 1; i < _argc; i++)
		{
			if (_argv[i][0] == '\\' || _argv[i][0] == '-')
			{
				m_lstCmdParamData.push_back(data);

				data.strCmd = _argv[i] + 1;
				data.vecParam.clear();
			}
			else
				data.vecParam.push_back(_argv[i]);
		}
		m_lstCmdParamData.push_back(data);
	}

	~KCommandLine(){}

	std::vector<CString> GetCommand(void)
	{
		std::vector<KCmdParamData>::iterator iter;
		std::vector<CString> vecCmd;
		for ( iter = m_lstCmdParamData.begin(); iter != m_lstCmdParamData.end(); ++iter)
		{
			vecCmd.push_back((*iter).strCmd);
		}
		return vecCmd;
	}

	std::vector<CString> GetCommandParam(CString strCmd)
	{
		std::vector<KCmdParamData>::iterator iter;
		for (iter = m_lstCmdParamData.begin(); iter != m_lstCmdParamData.end(); ++iter)
		{
			if ((*iter).strCmd == strCmd)
			{
				return (*iter).vecParam;
			}
		}

		return std::vector<CString>();
	}

	std::vector<CString> GetDefaultParam(void)
	{		
		return m_lstCmdParamData[0].vecParam;
	}
	
private:
	std::vector<KCmdParamData> m_lstCmdParamData;
};

#endif