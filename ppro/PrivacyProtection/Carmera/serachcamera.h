/************************************************************************
* @file      : serachcamera.h
* @author    : ChenZhiQiang <chenzhiqiang@kingsoft.com>
* @date      : 2010/10/14 9:41:02
* @brief     : 
*
* $Id: $
/************************************************************************/
#ifndef __SERACHCAMERA_H__
#define __SERACHCAMERA_H__

#include <string>
#include <setupapi.h>
#include "../KActor.h"

// -------------------------------------------------------------------------

BOOL GetPhysicalDeviceObjectNameBySearchCarmera(
    std::wstring& strPhysiceDeviceObjcetName,
    std::wstring& strDeviceDesc,
	KActor& Actor);

BOOL GetPhysicalDeviceObjectNameByProperty(
     HDEVINFO hDevInfo,
     PSP_DEVINFO_DATA pDeviceInfoData,
    std::wstring& strPhysiceDeviceObjcetName,
	KActor& Actor);


// -------------------------------------------------------------------------
// $Log: $

#endif /* __SERACHCAMERA_H__ */
