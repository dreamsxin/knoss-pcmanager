#include "stdafx.h"
#include "KFunction.h"
#include "KProcFileRule.h"

BOOL KProcFileRule::Match(KAccessData* pData)
{
	if (pData->nProcDirLen <= pData->nFileDirLen &&
		memcmp(pData->szFilePath, pData->szProcPath, pData->nProcDirLen * sizeof(TCHAR)) == 0)
	{
		return TRUE;
	}

	return FALSE;
}

void KProcFileRule::DbgPrint()
{
	DPrintA("KProcFileRule::DbgPrint\n\n");
}