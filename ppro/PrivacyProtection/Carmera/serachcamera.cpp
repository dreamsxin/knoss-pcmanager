/************************************************************************
* @file      : serachcamera.cpp
* @author    : ChenZhiQiang <chenzhiqiang@kingsoft.com>
* @date      : 2010/10/14 9:41:05
* @brief     : 
*
* $Id: $
/************************************************************************/

#include "stdafx.h"
#include <stdio.h>
#include <devguid.h>
#include <regstr.h>
#include "serachcamera.h"
#pragma comment(lib,"Setupapi.lib")

// -------------------------------------------------------------------------
BOOL GetPhysicalDeviceObjectNameBySearchCarmera(
	std::wstring&	strPhysiceDeviceObjcetName, 
	std::wstring&	strDeviceDesc, 
	KActor&			Actor)
{
    BOOL bRet = FALSE;
    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DeviceInfoData = {0};
    std::wstring strDeviceObjcetName;

    // Create a HDEVINFO with all present devices.
    hDevInfo = SetupDiGetClassDevs(
        (LPGUID)&GUID_DEVCLASS_IMAGE, 0, 0,DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        // Insert error handling here.
        return bRet;
    }

    // Enumerate through all devices in Set.
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
    {
		if (Actor.WaitForStop(1))
			return FALSE;

       if(GetPhysicalDeviceObjectNameByProperty(
           hDevInfo, 
           &DeviceInfoData, 
           strDeviceObjcetName,
		   Actor))
       {
           if (!strDeviceObjcetName.empty())
           {
               strPhysiceDeviceObjcetName = strDeviceObjcetName;
               bRet = TRUE;
               break;
           }
       }
    }

    //  Cleanup
    SetupDiDestroyDeviceInfoList(hDevInfo);
    return bRet;
}

BOOL GetPhysicalDeviceObjectNameByProperty(
	HDEVINFO			hDevInfo, 
	PSP_DEVINFO_DATA	pDeviceInfoData, 
	std::wstring&		strPhysiceDeviceObjcetName, 
	KActor&				Actor)
{
    BOOL bRet = TRUE;
    DWORD DataT;
    LPTSTR lpBuffer = NULL;
    DWORD dwBufSize = 0;
    // 
    // Call function with null to begin with, 
    // then use the returned buffer size 
    // to Alloc the buffer. Keep calling until
    // success or an unknown failure.
    // 
    while (!SetupDiGetDeviceRegistryProperty(
        hDevInfo,
        pDeviceInfoData,
        SPDRP_PHYSICAL_DEVICE_OBJECT_NAME,
        &DataT, (PBYTE)lpBuffer, dwBufSize, &dwBufSize))
    {
        DWORD dwLastErr = GetLastError();
		if (Actor.WaitForStop(1))
			break;

        if (ERROR_INSUFFICIENT_BUFFER == dwLastErr)
        {
            // Change the buffer size.
            if (lpBuffer) LocalFree(lpBuffer);
            lpBuffer = (LPTSTR)LocalAlloc(LPTR, dwBufSize + 1);
        }
        else
        {
            // Insert error handling here.
            bRet = FALSE;
            break;
        }
    }
    if (bRet && lpBuffer)
    {
        strPhysiceDeviceObjcetName = lpBuffer;
    }
    if (lpBuffer) LocalFree(lpBuffer);

    return bRet;
}
// -------------------------------------------------------------------------
// $Log: $
