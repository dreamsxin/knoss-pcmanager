#include "stdafx.h"
#include "PrivacyProtectionGUI.h"
#include "KAddSoftwareDlg.h"
#include <Psapi.h>
#include "Common\KXmlReader.h"
#include "KSignCache.h"


KAddSoftwareDlg::~KAddSoftwareDlg()
{
	m_ActorInit.Stop();
}

BOOL KAddSoftwareDlg::OnInitDialog(CWindow /*wndFocus*/, LPARAM /*lInitParam*/)
{
	m_ResultList.Create( 
		GetViewHWND(), NULL, NULL, 
		WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL , 
		0, 3001, NULL);
	{
		INT nRow = 0;
		m_ResultList.InsertColumn(nRow++, BkString::Get(135), LVCFMT_CENTER, 350);
		m_ResultList.InsertColumn(nRow++, BkString::Get(136), LVCFMT_CENTER, 280);
	}
	m_ResultList.SetObserverWindow( m_hWnd );


	m_ActorInit.Startup(this);


	return TRUE;
}

void KAddSoftwareDlg::OnBtnClose()
{
	EndDialog(IDCANCEL);
}

void KAddSoftwareDlg::OnBtnCustom()
{
	EndDialog(IDABORT);
}

void KAddSoftwareDlg::OnBtnOk()
{
	for (int i = 0; i < m_ResultList.GetItemCount(); i++)
	{
		if (!m_ResultList.GetCheckState(i))
			continue;

		CListViewCtrlEx::TListItem* pItem = m_ResultList._GetItemData(i);
		m_IndexArray.Add(pItem->dwMapId);
	}

	EndDialog(IDOK);
}

void KAddSoftwareDlg::OnSelectAllCheck()
{
	if (GetItemCheck(109))
	{
		m_ResultList.CheckAll();
	}
	else
		m_ResultList.CleanCheck();
}

void KAddSoftwareDlg::OnSelectAll()
{
	SetItemCheck(109, !GetItemCheck(109));
	if (GetItemCheck(109))
	{
		m_ResultList.CheckAll();
	}
	else
		m_ResultList.CleanCheck();
}

void KAddSoftwareDlg::EndDialog(UINT uRetCode)
{

	__super::EndDialog(uRetCode);
}

void KAddSoftwareDlg::OnSysCommand(UINT nID, CPoint point)
{
	SetMsgHandled(FALSE);

	switch (nID & 0xFFF0)
	{
	case SC_CLOSE:
		SetMsgHandled(TRUE);
		OnBtnClose();
		break;
	}
}

void KAddSoftwareDlg::OnActivate(KActor* pActor)
{
	if (m_ActorInit.IsActive())
	{
		// Get the list of process identifiers.
		DWORD aProcesses[1024], cbNeeded, cProcesses;
		unsigned int i;

		if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
			return;

		// Calculate how many process identifiers were returned.
		cProcesses = cbNeeded / sizeof(DWORD);

		if (!InitSoftwareData())
			return;

		// Print the name and process identifier for each process.
		for ( i = 0; i < cProcesses; i++ )
		{
			if (m_ActorInit.WaitForStop(1))
				return;

			if (aProcesses[i] == 0)
				continue;


			// Get a handle to the process.
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, aProcesses[i]);
			if (NULL == hProcess)
				continue;

			// Get the process name.
			HMODULE hModule					= NULL;
			DWORD	cbNeeded				= 0L;
			TCHAR	szProcessName[MAX_PATH] = _T("");
			TCHAR	szPath[MAX_PATH]		= _T(""); 

			if (!EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded) )
				goto CONTINUE;

			if (0 == GetModuleBaseName(hProcess, hModule, szProcessName, sizeof(szProcessName) / sizeof(TCHAR)))
				goto CONTINUE;

			int nIndex = m_SoftwareMap.FindKey(CString(szProcessName).MakeLower());
			if (-1 == nIndex)
				goto CONTINUE;

			if (0 == GetModuleFileNameEx(hProcess, hModule, szPath, sizeof(szPath) / sizeof(TCHAR)))
				goto CONTINUE;

			{
				LPCSTR pSign = g_signCache.GetFileSign(szPath, _tcslen(szPath));
				CStringA strSign;
				if (pSign)
					strSign = pSign;

				if (strSign != m_SoftwareMap.GetValueAt(nIndex).strDataSign.MakeLower())
					goto CONTINUE;
			}


			if (m_SoftwareMap.GetValueAt(nIndex).bExist)
				goto CONTINUE;

			SendMessage(MSG_UPDATE_RESULT, (WPARAM)szPath, (LPARAM)nIndex);
			m_SoftwareMap.GetValueAt(nIndex).bExist = true;
			

		CONTINUE:
			CloseHandle( hProcess );
		}
	}

}

LRESULT KAddSoftwareDlg::OnUpdateResult(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
	HICON hIcon = NULL;
	KLogic Logic;
	if (!Logic.GetFileIcon((TCHAR*)wParam, hIcon))
		hIcon = NULL;

	m_SoftwareMap.GetValueAt(lParam).hIcon = hIcon;
	m_SoftwareMap.GetValueAt(lParam).strProcessFile = (TCHAR*)wParam;
	int nItem = m_ResultList.Append(m_SoftwareMap.GetValueAt(lParam).strName, hIcon, lParam, LISTITEM_CHECKBOX);
	m_ResultList.AppendSubItem(nItem, m_SoftwareMap.GetValueAt(lParam).strCompany);

	return 0;
}

bool KAddSoftwareDlg::InitSoftwareData()
{
	BOOL		bRet		= FALSE;
	CStringA	strXml;


	bRet = BkResUtil::LoadBkXmlResource(IDR_SOFTWARE_DATA, strXml);
	if (!bRet)
		return false;

	KXmlReader XmlReader;
	XmlReader.Parse(strXml);

	bRet = XmlReader.Open("root\\software");
	if (!bRet)
		return false;

	do 
	{
		SOFTWARE_INFO	Info;
		CString			strProcess;


		Info.bExist = false;
		XmlReader.Read("Name", Info.strName);
		XmlReader.Read("Process", strProcess);
		XmlReader.Read("DataSign", Info.strDataSign);
		XmlReader.Read("Company", Info.strCompany);

		strProcess.MakeLower();
		int nIndex = m_SoftwareMap.FindKey(strProcess);
		if (-1 == nIndex)
		{	
			m_SoftwareMap.Add(strProcess, Info);
		}


		bRet = XmlReader.NextSibling();
		if (!bRet) 
			break;

	} while (true);

	return true;
}
