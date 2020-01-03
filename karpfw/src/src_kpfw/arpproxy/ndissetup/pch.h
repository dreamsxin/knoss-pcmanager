//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1997.
//
//  File:       P C H . H
//
//  Contents:   Pre-compiled header file
//
//  Notes:
//
//  Author:     kumarp 28-September-98
//
//----------------------------------------------------------------------------

#pragma once

// Turns off "string too long - truncated to 255 characters in the debug
// information, debugger cannot evaluate symbol."
//
#pragma warning (disable: 4786)
//
#define WINVER		    0x0501
#define _WIN32_WINNT	0x0501
//#define _WIN32_IE	    0x0501
#define _RICHEDIT_VER	0x0200
#define __RPC__out 
#include <atlbase.h>
#include <objbase.h>
#include "netcfgx.h"
#include "netcfgn.h"

#include <setupapi.h>
#include <devguid.h>
#include <tchar.h>
#include <stdio.h>

