/**
* @file    sqlite3encrypt.c
* @brief   ...
* @author  zhangrui
* @date    2009-02-18  11:49
*/

#ifndef SQLITE_HAS_CODEC
#define SQLITE_HAS_CODEC    1
#endif//SQLITE_HAS_CODEC

#pragma warning(disable: 4018)  // 'expression' : signed/unsigned mismatch
#pragma warning(disable: 4100)  // 'identifier' : unreferenced formal parameter
#pragma warning(disable: 4127)  // conditional expression is constant
#pragma warning(disable: 4210)  // nonstandard extension used : function given file scope
#pragma warning(disable: 4244)  // conversion from 'type1' to 'type2', possible loss of data
#pragma warning(disable: 4267)  // conversion from 'size_t' to 'type', possible loss of data
#pragma warning(disable: 4701)  // potentially uninitialized local variable 'iCursor' used
#pragma warning(disable: 4706)  // assignment within conditional expression

#include "sqlite3.c"

#include "rijndael.c"
#include "enc_md5.c"
#include "codec.c"
#include "codecext.c"
#include "pager_secure.c"