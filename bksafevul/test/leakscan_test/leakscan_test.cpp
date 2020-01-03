// leakscan_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <vulfix/Vulfix.h>
#include <libheader/libupdhelper.h>

#define  DLLFILE_VULFIX		"ksafevul.dll"

struct TDllProxy
{
	TDllProxy(LPCSTR szDllPath) : m_hModule(NULL)
	{
		m_hModule = LoadLibraryA(szDllPath);
	}
	~TDllProxy()
	{
		if(m_hModule)
			FreeLibrary(m_hModule);
		m_hModule = NULL;
	}

	FARPROC Get(LPCSTR szFuncName)
	{
		if(m_hModule)
		{
			return GetProcAddress(m_hModule, szFuncName);
		}
		return NULL;
	}
	HMODULE m_hModule;
};

void PressAnyKeyToContinue()
{
	_tprintf(_T("press any key to continue ... \n"));
	getchar();
}

void DumpIVulCollection( IVulCollection * pCollection )
{
	LONG nStatus = 0;
	if(SUCCEEDED(pCollection->get_Status(&nStatus)))
	{
		_tcprintf(_T("USE Status(FOR QuickScan AND Not Use IUpdate) %d\r\n"), nStatus);
	}
	else
	{
		INT nHigh, nOption, nSP;
		ATLVERIFY(SUCCEEDED(pCollection->get_VulCount(&nHigh, &nOption, &nSP)));
		_tcprintf(_T("COUNT High:%d  Option:%d  SP:%d\r\n"), nHigh, nOption, nSP);
		
		ULONG count = 0;
		pCollection->get_Count(&count);
		for(size_t i=0; i<count; ++i)
		{
			// et_Item2(ULONG n, int *pKBID, ULONG *pLevel, int *pNType, DWORD *pVFlags, BSTR* pTitle, BSTR* pDescription) = 0
			int kbID=0, nLevel=0, nType=0;
			DWORD vFlags=0;
			BSTR title, desc, pubdate;
			
			pCollection->get_Item3(i, &kbID, &nLevel, &nType, &vFlags, &title, &desc, &pubdate);
			_tcprintf(_T("%d/%d[Lvl:%d Type:%d VFlag:%04x] [%s]%s\t%s\r\n"), i, count, nLevel, nType, vFlags, pubdate, title, desc);
			SysFreeString(title);
			SysFreeString(desc);
		}
	}
}

void TestFullScan(DWORD dwFlags=0)
{
	TDllProxy dll(DLLFILE_VULFIX);
	FnCreateObject fn = (FnCreateObject) dll.Get(FUNCNAME_CreateObject);

	IVulScan *pscan = NULL;	
	if(SUCCEEDED(fn(__uuidof(IVulScan), (VOID**)&pscan)))
	{
		do
		{
			//PressAnyKeyToContinue();

			IVulCollection *pCollection = NULL;

			HRESULT hr = S_OK;
			{
				// T_TimeEvaluate __time_check(_T("Vul_Scan"));
				hr = pscan->ScanLeakEx(&pCollection, dwFlags);
			}
			if(SUCCEEDED(hr))
			{
				DumpIVulCollection(pCollection);
				pCollection->Release();
			}		

		}while(FALSE);

		pscan->Release();
	}
}

void TestQuickScan()
{
	TDllProxy dll(DLLFILE_VULFIX);
	FnExpressScanLeak fn = (FnExpressScanLeak) dll.Get(FUNCNAME_EXPRESSSCANLEAK);

	IVulCollection *pCollection = NULL;
	if(SUCCEEDED(fn(&pCollection, 0, TRUE)))
	{
		DumpIVulCollection(pCollection);
		pCollection->Release();
	}
}

HRESULT TestCombine(LPCTSTR szFileBase, LPCTSTR szFileInc)
{
	TDllProxy dll(DLLFILE_VULFIX);
	FnCreateUpdateHelper fn = (FnCreateUpdateHelper) dll.Get(FUNCNAME_CreateUpdateHelper);

	HRESULT hr = E_FAIL;
	IUpdateHelper *pidb = NULL;
	if( SUCCEEDED( hr = fn(__uuidof(IUpdateHelper), (void**)&pidb) ) )
	{		
		IUpdateHelper2 *pidb2 = NULL;
		if( SUCCEEDED(pidb->QueryInterface(__uuidof(IUpdateHelper2), (void**)&pidb2)) )
		{
			//LPCTSTR szSysIncPack = _T("D:\\system.dat\\system_2010.4.27.1_2010.4.27.2_inc.dat");
			//LPCTSTR szIncPack = _T("D:\\office.dat\\office_2010.4.20.1_2010.4.27.1_inc.dat");
			hr = pidb2->Combine2(szFileBase, szFileInc);
			//pidb->Combine(szIncPack);
		}		
	}
	return hr;
}


class TVulfixObserver : public IVulfixObserver
{
public:
	TVulfixObserver(IRepairVul *pRepairVul) : m_pRepairVul(pRepairVul)
	{
	}
	
	virtual void OnVulfixEvent( TVulFixEventType evt, int nKbId, DWORD dwParam1, DWORD dwParam2 )
	{
		//DEBUG_TRACE(_T("LeakEvent %d, %d (%d, %d)\n"), evt, nKbId, dwParam1, dwParam2);
		
		/*
		EVulfix_Error,				// 错误				
		EVulfix_ScanBegin,			// Scan 开始		nType, Num 
		EVulfix_ScanProgress,		// Scan 进度		nType, nCurrentItem 

		EVulfix_DownloadBegin,		// 下载开始			id  currentnum,	totalnum  
		EVulfix_DownloadProcess,	// 下载进度			id, downloadedsize, fullsize
		EVulfix_DownloadEnd,		// 下载结束			id, currentnum, down.result

		EVulfix_InstallBegin,		// 安装开始			id, currentnum,	totalnum 
		EVulfix_InstallEnd,			// 安装结束			id, currentnum, install.result 

		EVulfix_Task_Complete,		// 任务完全完成 
		EVulfix_Task_Error,			// 任务有错, 可能部分完成 
		*/
		if(EVulfix_InstallBegin==evt)
			m_bSmartInstallBegined = FALSE;
		else if(EVulfix_DownloadError==evt || EVulfix_DownloadEnd==evt || EVulfix_InstallError==evt || EVulfix_InstallBeginUser==evt || EVulfix_InstallEnd==evt || EVulfix_Download_Check_Error==evt )
		{
			BSTR bstr = NULL;
			if( m_pRepairVul && SUCCEEDED(m_pRepairVul->GetItemFilePath( nKbId, &bstr )))
			{
				CString strFilePath = bstr;
				SysFreeString(bstr);
			}
		}
	}

protected:
	IRepairVul *m_pRepairVul;
	BOOL m_bSmartInstallBegined;
};


HRESULT TestRepair()
{
	TDllProxy dll(DLLFILE_VULFIX);

	FnCreateVulFix fnScan = (FnCreateVulFix) dll.Get(FUNCNAME_CREATEVULFIX);
	FnCreateRepairVul fnRepair = (FnCreateRepairVul) dll.Get(FUNCNAME_CREATEREPAIRVUL);
	
	IVulfix *piScan = NULL;
	IRepairVul *piRepair = NULL;
	HRESULT hr = E_FAIL;

	if( fnScan && fnRepair && (piScan=fnScan(BEIKESAFE_VULSCAN_DLL_VERSION)) && (piRepair=fnRepair(BEIKESAFE_VULSCAN_DLL_VERSION)) )
	{
		// scan 
		piScan->Scan(0);
	
		const CSimpleArray<LPTUpdateItem>& arrSysVul = piScan->GetResults();
		const CSimpleArray<LPTVulSoft>& arrSoftVul = piScan->GetSoftVuls();
		
		IntArray arrSysIds, arrSoftIds;

		for(int i=0; i<arrSysVul.GetSize(); ++i)
		{
			LPTUpdateItem pvul = arrSysVul[i];
			_tprintf(_T("sysvul : id=%d, type=%d, name=%s\n"), pvul->nID, pvul->m_nType, pvul->strName);

            if ( pvul > 0 )
            {
                arrSysIds.Add(pvul->nID);
            }
		}

		for(int i=0; i<arrSoftVul.GetSize(); ++i)
		{
			LPTVulSoft pvul = arrSoftVul[i];
			_tprintf(_T("soft : id=%d, type=%d, name=%s\n"), pvul->nID, VTYPE_SOFTLEAK, pvul->strName);

			arrSoftIds.Add(pvul->nID);
		}	
		

		_tprintf(_T("开始修复漏洞\n"));
		PressAnyKeyToContinue();

		// repair 
		CString strPath, strImportPath;
		DWORD dwFlags = VULFLAG_REPAIR_IMPORT_LOCAL | VULFLAG_REPAIR_DOWNLOAD_IF_FAIL_IMPORT;
		strPath = _T("c:\\");
		strImportPath = _T("c:\\import");
		
		TVulfixObserver observer(piRepair);
		piRepair->SetObserver( &observer );
		piRepair->Reset();

		// 添加要修复的漏洞的ID 列表
		//piRepair->AddSoftVul(piScan, arrSoftIds);
		piRepair->AddVul(piScan, arrSysIds);

		// 开始修复 
		hr = piRepair->Repair(strPath, strImportPath, dwFlags);
		piRepair->WaitForRepairDone();
		piRepair->SetObserver(NULL);
	}	
	return hr;
}

int _tmain(int argc, _TCHAR* argv[])
{

    _tsetlocale(0, _T("chs"));

    if ( argc < 2 )
    {
        _tprintf( _T("参数:\n") );
        _tprintf( _T("1: 测试快速扫描\n") );
        _tprintf( _T("2: 测试全面扫描\n") );
        _tprintf( _T("3: 测试修复功能\n") );
        return 0;
    }

    if ( 0 ==  _tcsicmp( argv[1], _T("1") ) )
    {

        _tprintf(_T("测试快速扫描\n"));
        PressAnyKeyToContinue();
        TestQuickScan();
        goto Exit0;
    }

    if ( 0 == _tcsicmp( argv[1], _T("2") ) )
    {
        _tprintf(_T("测试全面扫描\n"));
        PressAnyKeyToContinue();
        TestFullScan();
        goto Exit0;
    }

	if ( 0 == _tcsicmp( argv[1], _T("3") ) )
    {
        _tprintf(_T("测试修复功能，为避免风险，可选和软件漏洞不做修复\n"));
        PressAnyKeyToContinue();
        TestRepair();
    }

Exit0:
	PressAnyKeyToContinue();
	return 0;
}

