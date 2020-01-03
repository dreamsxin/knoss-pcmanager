#ifndef _K_DECRYPT_FILE_H_
#define _K_DECRYPT_FILE_H_

#include <Windows.h>

int EncryptFileData(BYTE* pbyData, DWORD dwDataLen);
HRESULT Decrypt2File(OUT char* pszSaveFileName, IN int nFileNameSize);
HRESULT Decrypt2File_Oem(IN char szSaveFileDir[], IN int nResourceStart);
HRESULT Decrypt2File_One(IN OUT char szSaveFileDir[], IN int nResourceStart);
DWORD GetWinTempPath(DWORD nBufferLength, LPTSTR lpBuffer);

#endif

