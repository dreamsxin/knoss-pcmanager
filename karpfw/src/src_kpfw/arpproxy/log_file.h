#ifndef __log_file_h__
#define __log_file_h__

#define __log_file__

#ifdef __log_file__

int __cdecl __log_to_fileA(const char* format, ...);
int __cdecl __log_to_fileW(const wchar_t* format, ...);

#define DPrintA(__format, ...) __log_to_fileA(__format, __VA_ARGS__)
#define DPrintW(__format, ...) __log_to_fileW(__format, __VA_ARGS__)

#ifdef _UNICODE
#define DPrintT(__format, ...) __log_to_fileW
#else
#define DPrintT(__format, ...) __log_to_fileA
#endif

#else	//#ifdef __log_file__

#define DPrintA(__format, ...)
#define DPrintW(__format, ...)
#define DPrintT(__format, ...)

#endif	//#ifdef __log_file__

#endif