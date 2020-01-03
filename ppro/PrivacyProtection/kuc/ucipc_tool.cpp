/************************************************************************
* @file      : ucipc_tool.h
* @author    : WangBin5 <WangBin5.com>
* @date      : 26/11/2009 AM 11:53:11
* @brief     : 通用跨平台的IPC所使用的各种工具函数
*
* $Id: $
/************************************************************************/

//////////////////////////////////////////////////////////////////////////

#include "ucipc_tool.h"

INT KWcslen(WCHAR* str)
{
    WCHAR *string = (WCHAR *) str;
	
    while( *string )
		string++;
	
    return (INT)(string - str);
}


INT KWcscpySafe( WCHAR* wcDest, INT numElement, WCHAR *wcSrc)
{
    INT size = 0;

    if(!wcDest)
        return IPC_COM_ERROR_POINT;

    if(!wcSrc)
    {
        wcDest[0] = 0;
        return IPC_COM_ERROR_POINT;
    }

    if(numElement <= 0)
    {
        wcDest[0] = 0;

        if(numElement < 0)
            return IPC_COM_BUF_TOO_SMALL;
        else
            return IPC_COM_BUF_TOO_SMALL;
    }

    size = (INT)wcslen(wcSrc) + 1;

    if(size > numElement)
    {
        wcDest[0] = 0;
        return IPC_COM_BUF_TOO_SMALL;
    }

    if(size > numElement)
        size = numElement;

    memcpy( wcDest, wcSrc, size*sizeof(WCHAR) );

    return IPC_COM_SUCCEED;
}

INT KWcsncpySafe(
	WCHAR* wcDest, 
	INT numElement,
	WCHAR *wcSrc,
	INT count
	)
{
	INT size = 0;
	
	if (!wcDest || !numElement)
		return IPC_COM_ERROR_POINT;

	wcDest[0] = 0;
	
	if (!wcSrc)
	{
		return IPC_COM_ERROR_POINT;
	}
	
	size = IPC_Min((INT)wcslen(wcSrc), count);
	
	if (size >= numElement)
	{
		return IPC_COM_BUF_TOO_SMALL;
	}
	
	memcpy( wcDest, wcSrc, size*sizeof(WCHAR) );
		wcDest[size] = '\0';
	
	return IPC_COM_SUCCEED;
}

INT KWcscatSafe( WCHAR* wcDest, INT numElement, WCHAR* wcSrc)
{
	INT nlenSrc = 0;
	INT nlenDest = 0;

	if(numElement == 0 || wcDest == NULL || wcSrc == NULL)
		return IPC_COM_ERROR_POINT;
	
	nlenDest = (INT)wcslen(wcDest);
	nlenSrc = (INT)wcslen(wcSrc);
	
	if(nlenDest + nlenSrc > numElement-1)
		return IPC_COM_ERROR_POINT;

	memcpy(&wcDest[nlenDest], wcSrc, nlenSrc*sizeof(WCHAR));

	wcDest[nlenDest + nlenSrc] = 0;

	return IPC_COM_SUCCEED;
}