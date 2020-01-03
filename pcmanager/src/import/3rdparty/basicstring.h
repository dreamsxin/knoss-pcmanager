// BasicString.h 
//     Extended STL-based string template class designed for the full 
//	   interactivity between all classes based on it as well as _bstr_t,
//	   CString, and CStringT classes. 
//	   Includes implementation for almost all CString methods.
//
// Written by: Marochko Andrey Valentinovich, a_marochko@yahoo.com; (2001 - 2003)
//
// Documented at: http://www.codeguru.com/string/BasicString.html
//
//
// Version 2.
//
//         Now BasicString class can be based not only on the native 
//     STL's basic_string, but also on its modified version contained 
//     in the header "xstring.h". 
//         Note that VC++ 6.0sp5 does not support the modified version, 
//     since it generates incorrect code because the optimized version 
//     of basic_string class uses inheritance allowing EBO (empty base 
//     optimization). 
//         In VC++ 7.1 (NET 2003) the optimized version of basic_string 
//     is used by default. To force using native STL's implementation,
//     define the USE_CURRENT_STL_IMPL flag before including this header.
//
//         The main improvements of the new BasicString class provided 
//     by the optimized basic_string version:
//     - the compact object layout (the only DWORD vs. 4 DWORS in 
//       VC++ 6, and 7 DWORDS in VC++ 7.1 (!));
//     - optimized copy/assignment implementation;
//     - optimized GetBuffer/ReleaseBuffer performance;
//     - support for quick initialization of temporary string objects
//       with a string literal via the family of _R... macros (see
//       the short annotation at the end of the header).
//
//         Other improvements (not depending on the basic_string class):
//     - Template constructor that is used to construct a BasicString 
//       object from any type, which either can be converted to pointer
//       underlying string element type (const _E*), or to const char*,
//       or to const wchar_t*, or has the c_str() method. 
//           This constructor allows to avoid ambiguity when the source 
//       type can be implicitly converted to both const char* const 
//       wchar_t* (and in this case selects the most efficient way of 
//       initializing). 
//           Actually this allows to create BasicString objects from 
//       any extant string class and, potentially, from future string
//       classes without adding specialized constructors (e.g. the
//       first version of BasicString contained specialized conversion 
//       constructors for _bstr_t and CString, now they are unnecessary).
//           The only side-effect of using this constructor is bulky
//       compiler error messages when you try to create a string from
//       the object of improper type.
//     - Extensible ToStr/ToWstr/ToAstr macros semantics. You can make
//       any type convertible to String/WString/AString type by means
//       of a simple declaration using one of the DEFINE_TYPE_TO_STRING/
//       DEFINE_TYPE_TO_STRING_x macros. Look how this is done for 
//       RECT, POINT, SIZE, CRect, CPoint, and CSize types at the end 
//       of this header.
//
//         Incompatibilities with the first version:
//     - Definition of the BasicString class moved from the 'std' 
//       namespace to 'std_ex';
//     - Macros ToTstr, ResToTstr, and GuidToTstr renamed into ToStr, 
//       ResToStr, and GuidToStr correspondingly.
//     - Method set_bstr renamed into realloc_bstr.
//
//         Not tested functionality:
//     - Using BasicString objects based on optimized version of the 
//       basic_string class with iostream library.
//
///////////////////////////////////////////////////////////////////////

//     If your target OS is not Windows, define _NO_WINDOWS symbol  
// before the first inclusion of this header. This will exclude the 
// code using Windows-specific APIs. 
//     Additionally find calls to the functions WideCharToMultiByte and 
// MultiByteToWideChar and replace them with the currently commented 
// standard C-library function calls. But note that (at least with VC++)
// they does not use the current system locale, so if you are writing
// multilingual applications, you must manage locales manually using 
// proper C-library functions.
//     (I have not tested this class under OS'es other than Windows, so
// it is possible that some other incompatibilities will show. In this
// case drop me a line so that I could amend the code.)

#pragma once

#if !defined (_NO_WINDOWS)
	#include <windows.h>
#endif // _NO_WINDOWS

#if !defined(_MSC_VER)  ||  _MSC_VER < 1310
	//     VC++ 6 generates incorrect code for __bs_internal__::
	// basic_string class because the latter exploits EBO (empty base
	// optimization). So fall back to the native STL implementation.
	#define USE_CURRENT_STL_IMPL
#endif

#define USE_CURRENT_STL_IMPL
#if defined(USE_CURRENT_STL_IMPL)
	#include <string>
	#define  __bs_ns__  std
#else // !USE_CURRENT_STRING_IMPL
	//     "xstring.h" contains optimized version of the basic_string 
	// class based on that from VC++ 6 (see the short annotation at the
	// beginning of the header).
	#include "xstring.h"
	#define  __bs_ns__  __bs_internal__
#endif // !USE_CURRENT_STRING_IMPL

#if defined(__AFX_H__) || defined(__ATLSTR_H__)
	#if defined(_ATL_VER) && _ATL_VER >= 0x0700  ||  defined(_MFC_VER) && _MFC_VER >= 0x0700
		#define __CSTRING_AW_SUPPORT__
	#else
		#define __CSTRING_SUPPORT__
	#endif
#endif

#if defined(_INC_COMUTIL)
	#define __BSTR_T_SUPPORT__
#endif

#if defined(GUID_DEFINED)
	#define __GUIDs_SUPPORT__
#endif

#if defined(__wtypes_h__)
	#define __WTYPES_SUPPORT__
#endif

#if defined(__ATLTYPES_H__) || defined(__AFXWIN_H__)
	#define __ATLTYPES_SUPPORT__
#endif


#include <crtdbg.h>
//#include "TmplHlp.h"

///////////////////////////////////////////////////////////////////////
// struct char_traits_i
//
#if !defined(__CHAR_TRAITS_I__)
#define  __CHAR_TRAITS_I__

namespace  std_ex  {

template <class _E>
struct char_traits_i : std::char_traits<_E>
{
	static  bool __cdecl  eq ( const _E& _X, const _E& _Y )
	{
		int		iRes;
		if ( sizeof(_E) == sizeof(char) )
			iRes = strnicmp ((const char*)&_X, (const char*)&_Y, 1);
		else
			iRes = wcsnicmp ((const wchar_t*)&_X, (const wchar_t*)&_Y, 1);
		return  iRes == 0;
	}
	static  bool __cdecl  lt ( const _E& _X, const _E& _Y )
	{
		int		iRes;
		if ( sizeof(_E) == sizeof(char) )
			iRes = strnicmp ((const char*)&_X, (const char*)&_Y, 1);
		else
			iRes = wcsnicmp ((const wchar_t*)&_X, (const wchar_t*)&_Y, 1);
		return  iRes < 0;
	}
	static size_t __cdecl length ( const _E *_U )
	{
		if ( sizeof(_E) == sizeof(char) )
			return  !_U ? 0 : strlen((const char*)_U);
		else
			return  !_U ? 0 : wcslen((const wchar_t*)_U);
	}
	static  int __cdecl  compare ( const _E *_U, const _E *_V, size_t _N )
	{
		// In Microsoft's STL implementation if _U or _V is NULL, _N is 0.
		// In BasicString's implementation _U or _V can never be NULL.
		if ( sizeof(_E) == sizeof(char) )  {
			return  _N == 0 ? 0 : 
						strnicmp ((const char*)_U, (const char*)_V, _N);
		}
		else  {
			return  _N == 0 ? 0 : 
					wcsnicmp ((const wchar_t*)_U, (const wchar_t*)_V, _N);
		}
	}
	static  const _E *__cdecl  find ( const _E *_U, size_t _N, const _E& _C )
	{
		_E	_Ch = _C;
		const _E	*pcFoundL, *pcFoundU;
		if ( sizeof(_E) == sizeof(char) )  {
			pcFoundL = (const _E *)memchr ((const char*)_U, 
										   (_E)(tolower(_Ch)), _N);
			pcFoundU = (const _E *)memchr ((const char*)_U, 
										   (_E)(toupper(_Ch)), _N);
		}
		else  {
			pcFoundL = (const _E *)wmemchr ((const wchar_t*)_U, 
									 (_E)(towlower((wchar_t)_Ch)), _N);
			pcFoundU = (const _E *)wmemchr ((const wchar_t*)_U, 
									 (_E)(towupper((wchar_t)_Ch)), _N);
		}
		return  pcFoundL && pcFoundU ? min(pcFoundL, pcFoundU) : 
									   max(pcFoundL, pcFoundU);
	}
};

}	// namespace std_ex

#endif  // __CHAR_TRAITS_I__
//
// struct char_traits_i
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// class GMemAllocator
//
#if !defined(__G_MEM_ALLOCATOR__)
#define  __G_MEM_ALLOCATOR__

namespace  std_ex  {

template<class _Ty>
class GMemAllocator : public std::allocator<_Ty>
{
public:
	template<class _Other>
		GMemAllocator<_Ty>& operator=(const GMemAllocator<_Other>&)
		{	// assign from a related GMemAllocator (do nothing)
		return (*this);
		}

	template<class Other>
		struct rebind {
			typedef GMemAllocator<Other> other;
		};
	pointer allocate(size_type _N)
		{return ((pointer)GlobalAlloc(GMEM_FIXED, _N * sizeof(_Ty))); }
	pointer allocate(size_type _N, const void *)
		{return allocate(_N); }
	void deallocate(void _FARQ *_P)
		{GlobalFree(_P); }
	void deallocate(void _FARQ *_P, size_type)
		{deallocate(_P); }
};


template<class _Ty, class _U> inline
bool  operator == ( const GMemAllocator<_Ty>&, const GMemAllocator<_U>& )
{
	return  true;
}
template<class _Ty, class _U> inline
bool  operator == ( const GMemAllocator<_Ty>&, const std::allocator<_U>& )
{
	return  false;
}
template<class _Ty, class _U> inline
bool  operator == ( const std::allocator<_Ty>&, const GMemAllocator<_U>& )
{
	return  false;
}

template<class _Ty, class _U> inline
bool  operator != ( const GMemAllocator<_Ty>&, const GMemAllocator<_U>& )
{
	return  false;
}
template<class _Ty, class _U> inline
bool  operator != ( const GMemAllocator<_Ty>&, const std::allocator<_U>& )
{
	return  true;
}
template<class _Ty, class _U> inline
bool  operator != ( const std::allocator<_Ty>&, const GMemAllocator<_U>& )
{
	return  true;
}

}	// namespace std_ex

#endif // __G_MEM_ALLOCATOR__
//
// class GMemAllocator
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// class BasicString
//
#if !defined(__BASIC_STRING_EX__)
#define  __BASIC_STRING_EX__

//     In order for the code implementing interactivity with _bstr_t and 
// CString classes to compile the <comdef.h> and <afx.h> headers must be 
// included before this one.

namespace  std_ex  {

///////////////////////////////////////////////////////////////////////
// std::basic_string based types
//
typedef  std::basic_string<char>								AStdString;
typedef  std::basic_string<wchar_t>								WStdString;
typedef  std::basic_string<char, char_traits_i<char> >			IAStdString;
typedef  std::basic_string<wchar_t, char_traits_i<wchar_t> >	IWStdString;

#if !defined (_NO_WINDOWS)
typedef  std::basic_string<char, 
			std::char_traits<char>, GMemAllocator<char> >		GAStdString;
typedef  std::basic_string<wchar_t, 
			std::char_traits<wchar_t>, GMemAllocator<wchar_t> >	GWStdString;
typedef  std::basic_string<char, 
			char_traits_i<char>, GMemAllocator<char> >			GIAStdString;
typedef  std::basic_string<wchar_t, 
			char_traits_i<wchar_t>, GMemAllocator<wchar_t> >	GIWStdString;
#endif  // !_NO_WINDOWS

///////////////////////////////////////////////////////////////////////
// BasicString based types
//
template<class _E,
		class _Tr = std::char_traits<_E>,
		class _A = std::allocator<_E> >
class BasicString;
 
typedef  BasicString<char>										AString;
typedef  BasicString<char, char_traits_i<char> >				IAString;
typedef  BasicString<wchar_t>									WString;
typedef  BasicString<wchar_t, char_traits_i<wchar_t> >			IWString;

#if !defined (_NO_WINDOWS)
typedef  BasicString<char, 
			std::char_traits<char>, GMemAllocator<char> >		GAString;
typedef  BasicString<wchar_t, 
			std::char_traits<wchar_t>, GMemAllocator<wchar_t> >	GWString;
typedef  BasicString<char, 
			char_traits_i<char>, GMemAllocator<char> >			GIAString;
typedef  BasicString<wchar_t, 
			char_traits_i<wchar_t>, GMemAllocator<wchar_t> >	GIWString;
#endif  // !_NO_WINDOWS

#if defined(_UNICODE) || defined(UNICODE)
	typedef  WStdString		StdString;
	typedef  IWStdString	IStdString;
	typedef  WString		String;
	typedef  IWString		IString;
	#if !defined (_NO_WINDOWS)
	typedef  GWStdString	GStdString;
	typedef  GIWStdString	GIStdString;
	typedef  GWString		GString;
	typedef  GIWString		GIString;
	#endif  // !_NO_WINDOWS
#else
	typedef  AStdString		StdString;
	typedef  IAStdString	IStdString;
	typedef  AString		String;
	typedef  IAString		IString;
	#if !defined (_NO_WINDOWS)
	typedef  GAStdString	GStdString;
	typedef  GIAStdString	GIStdString;
	typedef  GAString		GString;
	typedef  GIAString		GIString;
	#endif  // !_NO_WINDOWS
#endif

///////////////////////////////////////////////////////////////////////
// BasicString helpers
//

///////////////////////////////////////////////////////////////////////
// _ForType<T>::Select(C1, C2)
//
#if !defined(__CONSTANT_SELECTOR__)
#define  __CONSTANT_SELECTOR__

template<class _E>  struct  _ForType;

template<>
struct  _ForType<char>
{
	static const char* Select ( const char* pcsz, const wchar_t* pcwsz )
	{
		return  pcsz;
	}
};
template<>
struct  _ForType<wchar_t>
{
	static const wchar_t* Select ( const char* pcsz, const wchar_t* pcwsz )
	{
		return  pcwsz;
	}
};

#endif	// !__CONSTANT_SELECTOR__
//
// _ForType<T>::Select(C1, C2)
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// template IF (compile time type selection) 
//
// Authors: Mat Marcus and Jesse Jones (Simulating Partial Specialization)
//
#if !defined(__METACTRL__IF__)
#define  __METACTRL__IF__

namespace  meta  {

	struct  SelectThen
	{
		template<class Then, class Else>
		struct Result
		{
			typedef Then RET;
		};
	};
	struct  SelectElse
	{
		template<class Then, class Else>
		struct Result
		{
			typedef Else RET;
		};
	};
	
	template<bool Condition>
	struct  Selector
	{
		typedef SelectThen RET;
	};
	template<>  
	struct  Selector<false>
	{
		typedef SelectElse RET;
	};

	template<bool Condition, class Then, class Else>
	struct  IF
	{
		typedef typename Selector<Condition>::RET  select;
		typedef typename select::Result<Then,Else>::RET  RET;
	};
	
} // end namespace meta

#endif // !__METACTRL__IF__
//
// IF
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// templates SWITCH / CASE (compile time type selection) 
//
// Authors: Mat Marcus and Jesse Jones (Simulating Partial Specialization)
//
#if !defined(__METACTRL__SWITCH_CASE__)
#define  __METACTRL__SWITCH_CASE__

const int DEFAULT_CASE = -32767;

namespace  meta
{
    const int NilValue = -32768;

    struct NilCase
	{
		enum  { tag = NilValue };
        typedef  NilCase	RET;
    };

	template<int Tag, class Statement, class Next = NilCase>
	struct CASE
	{
		enum  { tag = Tag };
		typedef  Statement	statement;
		typedef  Next		next;
	};
	
	template<int Tag, class aCase> 
	struct  SWITCH
	{
		typedef  typename aCase::next	nextCase;
		//typedef  typename nextSwitch::RET
		enum  { tag = aCase::tag,	// VC++ 5.0 doesn't operate directly
									// on aCase::value in IF<>.
									// Thus we need a little cheat
				nextTag = nextCase::tag,	
				found = (tag == Tag || tag == DEFAULT_CASE) 
		}; 
		typedef typename IF<(nextTag == NilValue), 
							NilCase, 
							SWITCH<Tag, nextCase> >::RET  nextSwitch; 
		typedef typename IF<(found != 0), 
							typename aCase::statement, 
							typename nextSwitch::RET>::RET  RET;
	};

}

#endif // !__METACTRL__SWITCH_CASE__
//
// SWITCH / CASE
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// ConversionFrom<T1>::To<T2>::Exists (compile time convertibility check)
//
// Based on the idea from Andrei Alexandrescu, Modern C++ Design, 2001
//
#if !defined(__CONVERSION_FROM_EXISTS__)
#define __CONVERSION_FROM_EXISTS__

template <class T>
struct  ConversionFrom
{
	template <class U>
	class  To
	{
		typedef char Small;
		struct  Big  {  char dummy[2];  };
		static  Small  Test(U);
		static  Big  Test(...);
	public:
			enum { Exists = sizeof(Test(*(T*)NULL)) == sizeof(Small) };
	};
};

#endif // __CONVERSION_FROM_EXISTS__
//
// ConversionFrom<T1>::To<T2>::Exists
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Types<T1>::And<T2>::AreSame (compile time types identity check)
//
// Based on the idea from Andrei Alexandrescu, Modern C++ Design, 2001
//
#if !defined(__TYPES_IDENTITY_CHECKER__)
#define  __TYPES_IDENTITY_CHECKER__

template <class T>
struct  Types
{
	template <class U>
	struct  And
	{
		enum  {  AreSame = false  };
	};
	template<>
	struct  And<T>
	{
		enum  {  AreSame = true  };
	};
};

#endif // !__TYPES_IDENTITY_CHECKER__
//
// Types<T1>::And<T2>::AreSame
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// IS_EXPR_OF_TYPE (Compile-time expression type check)
//
//     Checks whether expression's result can be converted to the given
// type. No real evaluation of the expression is performed.
//
#if !defined(__IS_EXPR_OF_TYPE__)
#define  __IS_EXPR_OF_TYPE__

template<class T>
struct  ExprTypeCheckHelper
{
	typedef char Small;
	struct  Big  {  char dummy[2];  };
	static  Small  Test(T);
	static  Big  Test(...);
};
#define  IS_EXPR_OF_TYPE(expr, Type)	\
	(sizeof(ExprTypeCheckHelper<Type>::Test(expr)) ==	\
	 sizeof(ExprTypeCheckHelper<Type>::Small))

#endif // !__IS_EXPR_OF_TYPE__
//
// IS_EXPR_OF_TYPE
///////////////////////////////////////////////////////////////////////

#if !defined (_NO_WINDOWS)
#define  _load_string(_Len, hInst, nID, _Buf, _Size)				\
	if ( sizeof(_E) == sizeof(wchar_t) )							\
		_Len = ::LoadStringW (hInst, nID, (wchar_t*)_Buf, _Size);	\
	else															\
		_Len = ::LoadStringA (hInst, nID, (char*)_Buf, _Size);
#endif  // !_NO_WINDOWS

// Forward declaration
template<class _E>  struct  Str;

//     StrFromEltPtr, StrFromCharPtr, StrFromWCharPtr:
//     This structures incapsulate initialization helpers, which are
// selected by the template conversion constructor of the BasicString 
// class depending on its argument type.

//     Static helper methods can be DEFINED only AFTER the BasicString
// class declaration. But they still must be DECLARED BEFORE it.
template<class _E, class _Tr, class _A>
struct StrFromEltPtr
{
	static void Init ( BasicString<_E, _Tr, _A>* pStr, const _E* pszSrc );
};
template<class _E, class _Tr, class _A>
struct StrFromCharPtr
{
	static void Init ( BasicString<_E, _Tr, _A>* pStr, const char* pszSrc );
};
template<class _E, class _Tr, class _A>
struct StrFromWCharPtr
{
	static void Init ( BasicString<_E, _Tr, _A>* pStr, const wchar_t* pwszSrc );
};
template<class _E, class _Tr, class _A, class T>
struct StrFrom_c_str_Holder
{
	static void Init ( BasicString<_E, _Tr, _A>* pStr, const T& src );
};

#ifdef  _DEBUG
	#define  sec_belt_len  (sizeof(long)/sizeof(_E))
#else
	#define  sec_belt_len  0
#endif

template<class _E, 
		class _Tr, 
		class _A >
class  BasicString : 
		public __bs_ns__::basic_string<_E, _Tr, _A>
{
	typedef BasicString<_E, _Tr, _A>	_This;
	typedef __bs_ns__::basic_string<_E, _Tr, _A>	_Myt;
	typedef const_iterator _It;

	static HINSTANCE	sm_hInst;
	static const _This	sm_SpaceSym;

	friend  struct StrFromEltPtr<_E, _Tr, _A>;
	friend  struct StrFromCharPtr<_E, _Tr, _A>;
	friend  struct StrFromWCharPtr<_E, _Tr, _A>;

	bool _init_with_non_string_ptr ( const void* _S )
	{
		if ( !_S )  {
			_Myt::assign((const _E*)L"");
			return  true;
		}
		if ( !HIWORD(_S) )  {
			load ((UINT)_S);
			return  true;
		}
		return  false;
	}

	void  _pinitNA ( const char* _S, size_type n )
	{
		if ( !_init_with_non_string_ptr(_S) )
		{
			if ( sizeof(_E) == sizeof(char) )
				_initN((const _E*)_S, n);
			else
				_initA((const char*)_S, n);
		}
	}

	void  _pinitNW ( const wchar_t* _S, size_type n )
	{
		if ( !_init_with_non_string_ptr(_S) )
		{
			if ( sizeof(_E) == sizeof(wchar_t) )
				_initN((const _E*)_S, n);
			else
				_initW((const wchar_t*)_S, n);
		}
	}

	void  _initW ( const wchar_t* pwszSrc, int nLen )
	{
		INT nResult = 0;
		BOOL bRetCode = FALSE;

		bRetCode = IsTextUnicode(pwszSrc, nLen, NULL);
		if (bRetCode == TRUE)
		{
			int nSize = WideCharToMultiByte (CP_ACP, 0, pwszSrc, nLen, NULL, 0, NULL, NULL);
			WideCharToMultiByte (CP_ACP, 0, pwszSrc, nLen, (char*)get_buf(nSize), nSize, NULL, NULL);
			set_buf (nSize);
		}else
		{
			int nSize = WideCharToMultiByte (CP_UTF8, 0, pwszSrc, nLen, NULL, 0, NULL, NULL);
			WideCharToMultiByte (CP_UTF8, 0, pwszSrc, nLen, (char*)get_buf(nSize), nSize, NULL, NULL);
			set_buf (nSize);
		}
	}
	void  _initA ( const char* pszSrc, int nLen )
	{
		int nSize = MultiByteToWideChar (CP_UTF8, 0, pszSrc, nLen, NULL, 0);
		MultiByteToWideChar (CP_UTF8, 0, pszSrc, nLen, (wchar_t*)get_buf(nSize), nSize);
		set_buf (nSize);
	}

	void  _init ( const _E *_Src )
	{
		_Myt::assign(_Src);
	}
	void  _initN ( const _E *_Src, size_type n )
	{
		_Myt::assign(_Src, n);
	}

	bool  _is_hex_seq ( size_type& _Pos, size_type _N ) const
	{
		if ( sizeof(_E) == sizeof(char) )  {
			for ( UINT i = 0; i < _N; i++, _Pos++ )  {
				if ( !::isxdigit((char)at(_Pos)) )
					return  false;
			}
		}
		else  {
			for ( UINT i = 0; i < _N; i++, _Pos++ )  {
				if ( !::iswxdigit((wchar_t)at(_Pos)) )
					return  false;
			}
		}
		return  true;
	}
	bool  _is_hyphen ( size_type& _Pos ) const
	{
		if ( sizeof(_E) == sizeof(char) )
			return (char)at(_Pos++) == '-';
		else
			return (wchar_t)at(_Pos++) == L'-';
	}
	bool  _is_guid ( size_type _Pos ) const
	{
		if ( _is_hex_seq(_Pos, 8)  &&  _is_hyphen(_Pos)  &&
			 _is_hex_seq(_Pos, 4)  &&  _is_hyphen(_Pos)  &&
			 _is_hex_seq(_Pos, 4)  &&  _is_hyphen(_Pos)  &&
			 _is_hex_seq(_Pos, 4)  &&  _is_hyphen(_Pos)  && 
			 _is_hex_seq(_Pos, 12) )
		{
			return  true;
		}
		return  false;
	}

	void  _trim_from ( size_type _Pos )
	{
		if ( _Pos != npos )
			resize(_Pos + 1);
	}
	void  _trim_upto ( size_type _Pos )
	{
		if ( !_Pos )
			return;
		if ( _Pos == npos )
			clear();
		else  {
			_This	_str(c_str() + _Pos);
			assign (_str);
		}
	}
	#if !defined (_NO_WINDOWS)
	void  format_msg ( DWORD dwFlags, LPCVOID _Src, DWORD dwMsgID, va_list* _pArgs = NULL )
	{
		_E		*_Buf = NULL;
		DWORD	_Res;
		if ( sizeof(_E) == sizeof(char) )  {
			_Res = ::FormatMessageA (dwFlags | FORMAT_MESSAGE_ALLOCATE_BUFFER,
									 _Src, dwMsgID, 0, (LPSTR)&_Buf, 0, _pArgs);
		}
		else  {
			_Res = ::FormatMessageW (dwFlags | FORMAT_MESSAGE_ALLOCATE_BUFFER,
									 _Src, dwMsgID, 0, (LPWSTR)&_Buf, 0, _pArgs);
		}
		if ( _Res  &&  _Buf )  {
			_Myt::assign(_Buf);
			LocalFree(_Buf);
		}
	}
	#endif  // !_NO_WINDOWS

public:

	// Default constructor
	explicit BasicString ( const _A& _Al = _A() )
	: _Myt(_Al)
	{}

	//     Template conversion constructor:
	//     Constructs the BasicString object from an object of any type
	// T which has one of the following capabilities:
	// - conversion operator to char*;
	// - conversion operator to wchar_t*;
	// - method c_str returning either char* or wchar_t*;
	//     If the type T doesn't have any of such capabilities, compile
	// time error occurs.
	template<class T>
	BasicString ( const T& Src )
	{
		enum  { ElementPtr, CharPtr, WCharPtr, Default,
				Selector = 
					(ConversionFrom<T>::To<const _E*>::Exists ? ElementPtr :
					(ConversionFrom<T>::To<const char*>::Exists ? CharPtr :
					(ConversionFrom<T>::To<const wchar_t*>::Exists ? WCharPtr : 
					Default)))
			  };
		meta::SWITCH<Selector, 
				meta::CASE<ElementPtr, StrFromEltPtr<_E, _Tr, _A>, 
				meta::CASE<CharPtr, StrFromCharPtr<_E, _Tr, _A>, 
				meta::CASE<WCharPtr, StrFromWCharPtr<_E, _Tr, _A>,
				meta::CASE<DEFAULT_CASE, StrFrom_c_str_Holder<_E, _Tr, _A, T>
				> > > >
			>::RET::Init (this, Src);
	}

	// Copy constructor
	BasicString ( const BasicString& _Src )
	{
		assign(_Src);
	}

	// Overloaded conversion constructors
	BasicString ( const _Myt& _X, size_type _P, 
				  size_type _M, const _A& _Al = _A() )
	: _Myt(_X, _P, _M, _Al)
	{}

	#if !defined(USE_CURRENT_STL_IMPL)
	BasicString ( const __bs_internal__::FastRawStringInitializer<_E>& _R )
	{
		//     In case of using base class initialization expression
		// _Myt(reinterpret_cast<const _Myt&>(_R)) performance decreases
		// as much as 4 times! (Incredibly, but it is the fact.)
		*(DWORD*)(this) = (DWORD)_R._Ptr;
	}
	#endif // USE_CURRENT_STL_IMPL

	BasicString ( const char *_S, size_type _N, const _A& _Al = _A() )
	: _Myt(_Al)
	{
		_pinitNA(_S, _N);
	}
	BasicString ( const wchar_t *_S, size_type _N, const _A& _Al = _A() )
	: _Myt(_Al)
	{
		_pinitNW(_S, _N);
	}
	BasicString ( size_type _N, _E _C, const _A& _Al = _A() )
	: _Myt(_N, _C, _Al)
	{}
	BasicString ( _E _C, const _A& _Al = _A() )
	: _Myt(1, _C, _Al)
	{}
	BasicString ( _It _F, _It _L, const _A& _Al = _A() )
	: _Myt(_F, _L, _Al)
	{}

	BasicString ( HINSTANCE hInst, UINT nID, const _A& _Al = _A() )
	: _Myt(_Al)
	{
		load (hInst, nID);
	}

	#ifdef __GUIDs_SUPPORT__
	BasicString ( const GUID& guid )
	{
		printf (_ForType<_E>::Select(
				  "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			      L"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"),
				guid.Data1, guid.Data2, guid.Data3,
				guid.Data4[0], guid.Data4[1],
				guid.Data4[2], guid.Data4[3], guid.Data4[4],
				guid.Data4[5], guid.Data4[6], guid.Data4[7] );
	}
	#endif // __GUIDs_SUPPORT__

	~BasicString ()  {}

	void  clear ()
	{
		assign((const _E*)L"");
	}

	//     operator =
	//     No overloaded operators for __bs_ns__::basic_string, _bstr_t 
	// or CString required because they can be converted to _This by 
	// means of the conversion constructor.
	template<class T>
	_This& operator = ( const T& _Src )
	{
		return  assign(_This(_Src));
	}
	_This& operator = ( const _This& _Src )
	{
		return  assign(_Src);
	}

	//     operator +=
	//     No individual operators for __bs_ns__::basic_string, _bstr_t 
	// or CString required because they can be converted to _This by 
	// means of the conversion constructor.
	template<class T>
    _This& operator += ( const T& _R )
	{
		// Conversion to _Myt& is necessary to prevent the infinite recursion.
		return  (_This&)((_Myt&)*this += _This(_R));
	}

	operator  const char* ()  const
	{
		return  const_cast<_This*>(this)->c_strA();
	}
	operator  const wchar_t* ()  const
	{
		return  const_cast<_This*>(this)->c_strW();
	}
	#if defined(__BSTR_T_SUPPORT__)
	operator _bstr_t () const
	{
		return  _bstr_t((const wchar_t*)*this);
	}
	#endif  // __BSTR_T_SUPPORT__

	#if defined(__CSTRING_AW_SUPPORT__)
	operator CStringA () const
	{
		return  CStringA(c_strA());
	}
	operator CStringW () const
	{
		return  CStringW(c_strW());
	}
	#elif defined(__CSTRING_SUPPORT__)
	operator CString () const
	{
		return  CString(c_str());
	}
	#endif // __CSTRING_SUPPORT__

	#if _MSC_VER > 1300
	template<class _E1, class _Tr1, class _A1>
	operator std::basic_string<_E1, _Tr1, _A1> () const
	{
		return  basic_string<_E1, _Tr1, _A1>((const _E1*)*this);
	}
	#endif // VC++ 7.1 or above

	template<class T>
	reference  operator[] ( T _P0 )
	{
		return  ((_Myt&)*this)[(size_type)_P0];
	}
	template<class T>
	const_reference  operator[] ( T _P0 )  const
	{
		return  ((_Myt&)*this)[(size_type)_P0];
	}

	const char*  c_strA ()  const
	{
		_This	*pThis = const_cast<_This*>(this);
		if ( sizeof(_E) == sizeof(char) )
			//     Explicit cast is necessary to make the template 
			// compilable when _E is wchar_t.
			return  (const char*)c_str();
		int		nFullLen = length() + 1;
		pThis->reserve (nFullLen + (nFullLen + 1)/2);
		char	*ptr = (char*)(c_str() + nFullLen);
		if ( length() )  {
			//     Explicit cast is necessary to make the template 
			// compilable when _E is char.
			//wcstombs (ptr, (const wchar_t*)c_str(), nFullLen);
			WideCharToMultiByte (CP_ACP, 0, (const wchar_t*)c_str(), 
								 nFullLen, ptr, nFullLen, NULL, NULL);
		}
		else
			*ptr = 0;
		return  ptr;
	}
	const wchar_t*  c_strW ()  const
	{
		_This	*pThis = const_cast<_This*>(this);
		if ( sizeof(_E) == sizeof(wchar_t) )
			//     Explicit cast is necessary to make the template 
			// compilable when _E is char.
			return  (const wchar_t*)c_str();
		int		nFullLen = length() + 1;
		pThis->reserve (nFullLen + (nFullLen + 1) * 2);
		wchar_t	*ptr = (wchar_t*)(c_str() + nFullLen);
		if ( length() )  {
			//     Explicit cast is necessary to make the template 
			// compilable when _E is wchar_t.
			//_ASSERTE ("BasicString::to_strW: MultiByteToWideChar failed" && nRes);
			//mbstowcs (ptr, (const char*)c_str(), nFullLen);
			MultiByteToWideChar (CP_ACP, 0, (const char*)c_str(), 
								 nFullLen, ptr, nFullLen);
		}
		else
			*ptr = 0;
		return  ptr;
	}
	LPCTSTR  c_strT ()  const
	{
		if ( sizeof(_E) == sizeof(TCHAR) )
			// Explicit cast is necessary to make the template compilable when _E differs from TCHAR.
			return  (const TCHAR*)c_str();
		return  LPCTSTR(*this);		// invoke object's own cast operator
	}

	#ifdef  _OLEAUTO_H_
	const BSTR  bstr ()  const
	{
		_This	*pThis = const_cast<_This*>(this);
		int		nFullLen = length() + 1;
		pThis->reserve (nFullLen * (1 + sizeof(wchar_t)/sizeof(_E)) +
					    sizeof(DWORD)/sizeof(_E));
		wchar_t	*ptr = (wchar_t*)(c_str() + nFullLen);
		*(DWORD*)ptr = length() * sizeof(wchar_t);
		ptr += 2;	// skip bstr length
		if ( length() )  {
			//     Explicit cast is necessary to make the template 
			// compilable when _E is wchar_t.
			//_ASSERTE ("BasicString::to_strW: MultiByteToWideChar failed" && nRes);
			if ( sizeof(_E) == sizeof(wchar_t) )
				memcpy (ptr, c_str(), nFullLen * sizeof(wchar_t));
			else  {
				//mbstowcs (ptr, (const char*)c_str(), nFullLen);
				MultiByteToWideChar (CP_ACP, 0, (const char*)c_str(), 
									 nFullLen, ptr, nFullLen);
			}
		}
		else
			*ptr = 0;
		return  ptr;
	}
	#endif  // _OLEAUTO_H_

	static  size_type  guid_len ()  {  return  36;  }

	#ifdef __GUIDs_SUPPORT__
	GUID  guid ( size_type _Pos = 0 ) const
	{
		GUID	_Guid;
		int		c0, c1, c2, c3, c4, c5, c6, c7;
		int		N, n1, n2, n3, n4, n5, 
				n6, n7, n8, n9, n10, n11;
		if ( sizeof(_E) == sizeof(char) )  {
			N = sscanf ((const char*)c_str() + _Pos, 
						"%8x%n-%4hx%n-%4hx%n-%2x%n%2x%n-%2x%n%2x%n%2x%n%2x%n%2x%n%2x%n",
						&_Guid.Data1, &n1, &_Guid.Data2, &n2, &_Guid.Data3, &n3, 
						&c0, &n4, &c1, &n5, &c2, &n6, &c3, &n7, &c4, &n8, &c5, &n9, &c6, &n10, &c7, &n11);
		}
		else  {
			N = swscanf ((const wchar_t*)c_str() + _Pos, 
						L"%8x%n-%4hx%n-%4hx%n-%2x%n%2x%n-%2x%n%2x%n%2x%n%2x%n%2x%n%2x%n",
						&_Guid.Data1, &n1, &_Guid.Data2, &n2, &_Guid.Data3, &n3, 
						&c0, &n4, &c1, &n5, &c2, &n6, &c3, &n7, &c4, &n8, &c5, &n9, &c6, &n10, &c7, &n11);
		}
		if ( N == 11  &&  n1 == 8  &&  n2 == 13  &&  n3 == 18  &&  n4 == 21  &&  n5 == 23  &&
			 n6 == 26  &&  n7 == 28  &&  n8 == 30  &&  n9 == 32  &&  n10 == 34  &&  n11 == 36 )
		{
			_Guid.Data4[0] = c0;  _Guid.Data4[1] = c1;  _Guid.Data4[2] = c2;  _Guid.Data4[3] = c3;
			_Guid.Data4[4] = c4;  _Guid.Data4[5] = c5;  _Guid.Data4[6] = c6;  _Guid.Data4[7] = c7;
			return  _Guid;
		}
		return  GUID_NULL;
	}
	size_type  find_guid ( size_type _Pos = 0 ) const
	{
		while ( (long)_Pos <= (long)length() - (long)guid_len() ) {
			if ( _is_guid(_Pos) )
				return  _Pos;
			_Pos++;
		}
		return  npos;	// -1
	}

	GUID  GetGuid ( size_type _Pos = 0 ) const
	{
		return  guid(_Pos);
	}
	size_type  FindGuid ( size_type _Pos = 0 ) const
	{
		return  find_guid(_Pos);
	}
	#endif  // __GUIDs_SUPPORT__

	#if !defined (_NO_WINDOWS)
	bool  load ( HINSTANCE _Inst, UINT _Rid )
	{
		const  int	_InitSize = 256;
		_E		_Buf[_InitSize];
		int		_Len;
		_load_string(_Len, _Inst, _Rid, _Buf, _InitSize)
		if ( _InitSize - _Len > 1 )  {
			if ( _Len > 0 )
				assign(_Buf, _Len);
			else
				clear();
			return  _Len > 0;
		}
		int		_Size = _InitSize;
		do  {
			_Size += _Size;
			// The 'reserve' method cannot be used here because the final 'resize' in this
			// case will prevent only as many initial characters of the loaded string as
			// was the length of this object's string when this method was invoked:
			resize(_Size);
			_load_string(_Len, _Inst, _Rid, (_E*)c_str(), _Size)
		}  while ( _Size - _Len <= 1 );
		if ( _Len > 0 )
			resize(_Len);
		else
			clear();
		return  _Len > 0;
	}
	bool  load ( UINT _Rid )
	{
		return  load(sm_hInst, _Rid);
	}

	BOOL  LoadString ( HINSTANCE hInst, UINT nID )  
	{
		return  load(hInst, nID) ? TRUE : FALSE;
	}
	BOOL  LoadString ( UINT nID )
	{
		return  LoadString(sm_hInst, nID);
	}

	static  HINSTANCE  SetResourceHandle ( HINSTANCE hInst )
	{
		HINSTANCE	hInstPrev = sm_hInst;
		sm_hInst = hInst;
		return  hInstPrev;
	}
	#endif  // !_NO_WINDOWS

	_E* get_buf ( size_type _Len = -1, bool bPreserveContents = true )
	{
		_Len = (_Len == size_type(-1) ? length() : _Len) + 1;
		if ( !bPreserveContents )
			resize(0);
		//     The 'reserve' method cannot be used here because 'resize' 
		// called in the set_buf method will prevent in this case only 
		// as many initial characters of the new string as was the 
		// length of this string when 'get_buf' was invoked.
		resize (_Len + sec_belt_len);
		begin();	// freeze the buffer
		_E* _Buf = (_E*)c_str();
		#ifdef  _DEBUG
			*(long*)(_Buf + _Len) = -1;
		#endif
		return  _Buf;
	}
	void  set_buf ( size_type _Len = -1 )
	{
		_Len = _Len >= LONG_MAX ? _Tr::length(c_str()) : _Len;
		#ifdef  _DEBUG
			size_type _AllocLen = size();
			const _E* _Buf = c_str();
			_ASSERTE ("Allocated buffer length was exceeded" && 
					  *(long*)(_Buf + _AllocLen - sec_belt_len) == -1);
		#endif
		resize (_Len);
	}

	_E* GetBuffer ( int nBufLen = -1, bool bPreserveContents = true )
	{
		return  get_buf(nBufLen, bPreserveContents);
	}
	void  ReleaseBuffer ( int nNewLen = -1 )
	{
		set_buf(nNewLen);
	}

	#if !defined(va_start_ref)
		#define va_start_ref(ap,v)		\
		{	int var= sizeof(&v);		\
			__asm lea eax,v				\
			__asm add eax,var			\
			__asm mov ap,eax			\
		}
	#endif

	_This&  append ( const _This& _X )  
		{  return  static_cast<_This&>(_Myt::append(_X));  }
	_This&  append ( const _This& _X, size_type _P, size_type _M )
		{  return  static_cast<_This&>(_Myt::append(_X, _P, _M));  }
	_This&  append ( const _This& _X, size_type _M )
		{  return  append(_X, 0, _M);  }
	_This&  append ( size_type _M, _E _C )
		{  return  static_cast<_This&>(_Myt::append(_M, _C));  }
	_This&  append (_It _F, _It _L)
		{  return  static_cast<_This&>(_Myt::append(_F, _L));  }

	_This&  assign (const _E* _S )  
		{  return  static_cast<_This&>(_Myt::assign(_S));  }
	_This&  assign (const _E* _S, size_type _M )  
		{  return  static_cast<_This&>(_Myt::assign(_S, _M));  }
	_This&  assign ( const _This& _X )
		{  return  static_cast<_This&>(_Myt::assign(_X));  }
	_This&  assign ( const _This& _X, size_type _P, size_type _M )
		{  return  static_cast<_This&>(_Myt::assign(_X, _P, _M));  }
	_This&  assign ( const _This& _X, size_type _M )
		{  return  assign(_X, 0, _M);  }
	_This&  assign ( size_type _N, _E _C )
		{  return  static_cast<_This&>(_Myt::assign(_N, _C));  }
	_This&  assign ( _It _F, _It _L )
		{  return  static_cast<_This&>(_Myt::assign(_F, _L));  }

	_This&  insert ( size_type _P0, const _This& _X )
		{  return  static_cast<_This&>(_Myt::insert(_P0, _X));  }
	_This&  insert ( size_type _P0, const _This& _X, size_type _P, size_type _M )
		{  return  static_cast<_This&>(_Myt::insert(_P0, _X, _P, _M));  }
	_This&  insert ( size_type _P0, const _This& _X, size_type _M )
		{  return  static_cast<_This&>(_Myt::insert(_P0, _X, 0, _M));  }
	_This&  insert ( size_type _P0, size_type _M, _E _C )
		{  return  static_cast<_This&>(_Myt::insert(_P0, _M, _C));  }
	iterator insert ( iterator  _P, _E _C )
		{  return  _Myt::insert(_P, _C);  }
	void insert ( iterator  _P, size_type _M, _E _C )
		{  _Myt::insert(_P, _M, _C);  }
	void insert ( iterator  _P, _It _F, _It _L )
		{  _Myt::insert(_P, _F, _L);  }

	_This&  replace ( size_type _P0, size_type _N0, const _This& _X )
		{  return  static_cast<_This&>(_Myt::replace(_P0, _N0, _X));  }
	_This&  replace ( size_type _P0, size_type _N0, const _This& _X, 
					  size_type _P, size_type _M )
		{  return  static_cast<_This&>(_Myt::replace(_P0, _N0, _X, _P, _M));  }
	_This&  replace ( size_type _P0, size_type _N0, const _This& _X, size_type _M )
		{  return  static_cast<_This&>(_Myt::replace(_P0, _N0, _X, 0, _M));  }
	_This&  replace ( size_type _P0, size_type _N0, size_type _M, _E _C )
		{  return  static_cast<_This&>(_Myt::replace(_P0, _N0, _M, _C));  }
	_This&  replace ( iterator _F, iterator _L, const _This& _X )
		{  return  static_cast<_This&>(_Myt::replace(_F, _L, _X));  }
	_This&  replace ( iterator _F, iterator _L, const _This& _X, size_type _M )
		{  return  static_cast<_This&>(_Myt::replace(_F, _L, _X, _M));  }
	_This&  replace ( iterator _F, iterator _L,	size_type _M, _E _C )
		{  return  static_cast<_This&>(_Myt::replace(_F, _L, _M, _C));  }
	_This&  replace ( iterator _F1, iterator _L1, _It _F2, _It _L2 )
		{  return  static_cast<_This&>(_Myt::replace(_F1, _L1, _F2, _L2));  }

	//     find, rfind, find_xxx:
	//     Agument _P specifies the starting position in this string, 
	// while the _N argument pertains to the _X string parameter.
	size_type  find ( const _This& _X, size_type _P = 0 ) const
		{  return  _Myt::find(_X, _P);  }
	size_type  find ( const _This& _X, size_type _P, size_type _N ) const
		{  return  _Myt::find(_X, _P, _N);  }
	size_type  find ( _E _C, size_type _P = 0 ) const
		{  return  _Myt::find(_C, _P);  }

	size_type  rfind ( const _This& _X, size_type _P = npos ) const
		{  return  _Myt::rfind(_X, _P);  }
	size_type  rfind ( const _This& _X, size_type _P, size_type _N ) const
		{  return  _Myt::rfind(_X, _P, _N);  }
	size_type  rfind ( _E _C, size_type _P = npos ) const
		{  return  _Myt::rfind(_C, _P);  }

	size_type  find_first_of ( const _This& _X, size_type _P = 0 ) const
		{  return  _Myt::find_first_of(_X, _P);  }
	size_type  find_first_of ( const _This& _X, size_type _P, size_type _N ) const
		{  return  _Myt::find_first_of(_X, _P, _N);  }
	size_type  find_first_of ( _E _C, size_type _P = 0 ) const
		{  return  _Myt::find_first_of(_C, _P);  }

	size_type  find_last_of ( const _This& _X, size_type _P = npos ) const
		{  return  _Myt::find_last_of(_X, _P);  }
	size_type  find_last_of ( const _This& _X, size_type _P, size_type _N ) const
		{  return  _Myt::find_last_of(_X, _P, _N);  }
	size_type  find_last_of ( _E _C, size_type _P = npos ) const
		{  return  _Myt::find_last_of(_C, _P);  }

	size_type  find_first_not_of ( const _This& _X, size_type _P = 0 ) const
		{  return  _Myt::find_first_not_of(_X, _P);  }
	size_type  find_first_not_of ( const _This& _X, size_type _P, size_type _N ) const
		{  return  _Myt::find_first_not_of(_X, _P, _N);  }
	size_type  find_first_not_of ( _E _C, size_type _P = 0 ) const
		{  return  _Myt::find_first_not_of(_C, _P);  }

	size_type  find_last_not_of ( const _This& _X, size_type _P = npos ) const
		{  return  _Myt::find_last_not_of(_X, _P);  }
	size_type  find_last_not_of ( const _This& _X, size_type _P, size_type _N ) const
		{  return  _Myt::find_last_not_of(_X, _P, _N);  }
	size_type  find_last_not_of ( _E _C, size_type _P = npos ) const
		{  return  _Myt::find_last_not_of(_C, _P);  }

	int  compare ( const _This& _X ) const
		{  return  _Myt::compare(_X);  }
	int  compare ( size_type _P0, size_type _N0, const _This& _X ) const
		{  return  _Myt::compare(_P0, _N0, _X);  }
	int  compare ( size_type _P0, size_type _N0, const _This& _X, 
				   size_type _P, size_type _M ) const 
		{  return  _Myt::compare(_P0, _N0, _X, _P, _M);  }
	int  compare ( size_type _P0, size_type _N0, const _This& _X, size_type _M ) const
		{  return  _Myt::compare(_P0, _N0, _X, 0, _M);  }
	//int  compare ( const _E *_S ) const
	//	{  return  _Myt::compare();  }
	//int  compare ( size_type _P0, size_type _N0, const _E *_S ) const
	//	{  return  _Myt::compare();  }
	//int  compare ( size_type _P0, size_type _N0, const _E *_S, size_type _M ) const
	//	{  return  _Myt::compare();  }

	template<class _E2, class _Tr2, class _A2>
	void  swap ( __bs_ns__::basic_string<_E2, _Tr2, _A2>& _X )
	{
		_This _Ts = *this; 
		*this = _X;
		_X = _Ts;
	}

	void  printf ( const _E* _Fmt, ... )
	{
		va_list _Args;
		va_start (_Args, _Fmt);
		vprintf (_Fmt, _Args);
		va_end (_Args);
	}
	void  printf ( UINT _FmtID, ... )
	{
		_This	_Fmt((const _E*)_FmtID);
		va_list _Args;
		va_start (_Args, _FmtID);
		vprintf (_Fmt, _Args);
		va_end (_Args);
	}
	void  vprintf ( UINT _FmtID, va_list _Args )
	{
		_This	_Fmt((const _E*)_FmtID);
		vprintf(_Fmt, _Args);
	}
	void  vprintf ( const _E* _Fmt, va_list _Args )
	{
		int		_Res;
		
		#if defined (_NO_WINDOWS)

		long	_Size = 256;
		do  {
			if ( sizeof(_E) == sizeof(char) )  {
				_Res = _vsnprintf ((char*)get_buf(_Size), _Size, 
									(const char*)_Fmt, _Args);
			}
			else  {
				_Res = _vsnwprintf ((wchar_t*)get_buf(_Size), _Size, 
									(const wchar_t*)_Fmt, _Args);
			}
			_Size += _Size;
		} while ( _Res == -1 );
		if ( _Res > 0 )
			set_buf (_Res);
		else
			clear();

		#else // !_NO_WINDOWS

		long	_Size = 1024;
		_E		*_Buf = NULL;
		do  {
			_Buf = (_E*)_alloca(_Size * sizeof(_E));
			if ( sizeof(_E) == sizeof(char) )  {
				_Res = _vsnprintf ((char*)_Buf, _Size, 
									(const char*)_Fmt, _Args);
			}
			else  {
				_Res = _vsnwprintf ((wchar_t*)_Buf, _Size, 
									(const wchar_t*)_Fmt, _Args);
			}
			_Size += _Size;
		} while ( _Res == -1 );
		assign (_Buf, _Res);

		#endif // !_NO_WINDOWS
	}

	//void __cdecl  Format ( const _This& strFormat, ... )
	void __cdecl  Format ( const _E* strFormat, ... )
	{
		va_list _Args;
		va_start (_Args, strFormat);
		vprintf (strFormat, _Args);
		va_end (_Args);
	}
	void __cdecl  Format ( UINT nFormatID, ... )
	{
		_This	_Fmt((const _E*)nFormatID);
		va_list _Args;
		va_start (_Args, nFormatID);
		vprintf (_Fmt, _Args);
		va_end (_Args);
	}
	void  FormatV ( const _This& strFormat, va_list _Args )
	{
		vprintf (strFormat, _Args);
	}
	void  FormatV ( UINT nFormatID, va_list _Args )
	{
		_This	_Fmt((const _E*)nFormatID);
		vprintf (_Fmt, _Args);
	}

	#if !defined (_NO_WINDOWS)
	void __cdecl format_msg ( UINT _FmtID, ... )
	{
		_This	_Fmt((const _E*)_FmtID);
		va_list _Args;
		va_start (_Args, _FmtID);
		vformat_msg (_Fmt, _Args);
		va_end (_Args);
	}
	void __cdecl format_msg ( const _E* _Fmt, ... )
	{
		va_list _Args;
		va_start (_Args, _Fmt);
		vformat_msg (_Fmt, _Args);
		va_end (_Args);
	}
	void  vformat_msg ( UINT _FmtID, va_list _Args )
	{
		_This	_Fmt((const _E*)_FmtID);
		vformat_msg (_Fmt, _Args);
	}
	void  vformat_msg ( const _E* _Fmt, va_list _Args )
	{
		format_msg (FORMAT_MESSAGE_FROM_STRING, _Fmt, 0, &_Args);
	}

	void __cdecl FormatMessage ( UINT _FmtID, ... )
	{
		_This	_Fmt((const _E*)_FmtID);
		va_list _Args;
		va_start (_Args, _FmtID);
		vformat_msg (_Fmt, _Args);
		va_end (_Args);
	}
	void __cdecl FormatMessage ( const _E* _Fmt, ... )
	{
		va_list _Args;
		va_start (_Args, _Fmt);
		vformat_msg (_Fmt, _Args);
		va_end (_Args);
	}
	void __cdecl FormatMessageV ( UINT _FmtID, va_list _Args )
	{
		_This	_Fmt((const _E*)_FmtID);
		vformat_msg (_Fmt, _Args);
	}
	void __cdecl FormatMessageV ( const _E* _Fmt, va_list _Args )
	{
		vformat_msg (_Fmt, _Args);
	}

	void  format_sys_msg ( DWORD _MsgCode, ... )
	{
		va_list _Args;
		va_start (_Args, _MsgCode);
		vformat_sys_msg (_MsgCode, _Args);
		va_end (_Args);
	}
	void  vformat_sys_msg ( DWORD _MsgCode, va_list _Args )
	{
		format_msg (FORMAT_MESSAGE_FROM_SYSTEM, NULL, _MsgCode, &_Args);
	}
	void  format_mod_msg ( HMODULE hInst, DWORD _MsgCode, ... )
	{
		va_list _Args;
		va_start (_Args, _MsgCode);
		vformat_mod_msg (hInst, _MsgCode, _Args);
		va_end (_Args);
	}
	void  vformat_mod_msg ( HMODULE hInst, DWORD _MsgCode, va_list _Args )
	{
		// Adding FORMAT_MESSAGE_FROM_SYSTEM flag has no effect (in contrast to what MSDN says)
		format_msg (FORMAT_MESSAGE_FROM_HMODULE, hInst, _MsgCode, &_Args);
	}

	void  FormatSystemMessage ( DWORD _MsgCode, ... )
	{
		va_list _Args;
		va_start (_Args, _MsgCode);
		vformat_sys_msg (_MsgCode, _Args);
		va_end (_Args);
	}
	void  FormatSystemMessageV ( DWORD _MsgCode, va_list _Args )
	{
		vformat_sys_msg (_MsgCode, _Args);
	}
	void  FormatModuleMessage ( HMODULE hInst, DWORD _MsgCode, ... )
	{
		va_list _Args;
		va_start (_Args, _MsgCode);
		vformat_mod_msg (hInst, _MsgCode, _Args);
		va_end (_Args);
	}
	void  FormatModuleMessageV ( HMODULE hInst, DWORD _MsgCode, va_list _Args )
	{
		vformat_mod_msg (hInst, _MsgCode, _Args);
	}
	#endif // !_NO_WINDOWS

	#ifdef  _OLEAUTO_H_
	//     alloc_bstr:
	//     Returns a BSTR initialized with this string's data
	BSTR  alloc_bstr () const
	{
		return  SysAllocStringLen (c_strW(), length());
	}
	//     realloc_bstr:
	//     Reallocates the passed BSTR and copies the content of 
	// this string to it.
	
	BSTR  realloc_bstr ( BSTR* pbstr ) const
	{
		return  SysReAllocStringLen (pbstr, c_strW(), length());
	}
	BSTR  AllocSysString () const
	{
		return  bstr();
	}
	BSTR  SetSysString ( BSTR* pbstr ) const
	{
		return  realloc_bstr(pbstr);
	}
	#endif	// _OLEAUTO_H_

	int  GetLength () const  {  return  length();  }
	bool  IsEmpty () const  {  return  empty();  }

	int  cmp_case ( const _This& _S ) const
	{
		return  reinterpret_cast<const BasicString<_E, std::char_traits<_E>, _A>*>(this)->compare(_S);
	}
	int  cmp_no_case ( const _This& _S ) const
	{
		return  reinterpret_cast<const BasicString<_E, char_traits_i<_E>, _A>*>(this)->compare(_S);
	}
	int  Compare ( const _This& _S ) const  {  return  compare (_S);  }
	int  CompareCase ( const _This& _S ) const  {  return  cmp_case(_S);  }
	int  CompareNoCase ( const _This& _S ) const  {  return  cmp_no_case(_S);  }

	_This  Mid ( size_type _Pos, size_type _N )  const
	{
		return  substr(_Pos, _N);
	}
	_This  Right ( size_type _N )  const
	{
		return  substr(max(0, (long)length() - (long)_N), _N);
	}
	_This  Left ( size_type _N )  const
	{
		return  substr(0, _N);
	}

	// The second parameter of the Find, ReverseFind and FindOneOf is not implemented in CString
	size_type Find ( _E _C, size_type _Pos = 0 ) const
	{
		return  find(_C, _Pos);
	}
	size_type Find ( const _This& _S, size_type _Pos = 0 ) const
	{
		return  find(_S, _Pos);
	}
	size_type ReverseFind ( _E _C, size_type _Pos = npos ) const
	{
		return  rfind(_C, npos);
	}
	// This variant of ReverseFind is not implemented in CString
	size_type ReverseFind ( const _This& _S, size_type _Pos = npos ) const
	{
		return  rfind(_S, npos);
	}
	size_type FindOneOf ( const _This& _CharSet, size_type _Pos = 0 ) const
	{
		return find_first_of(_CharSet, _Pos);
	}
	// FindOneNotOf, FindLastOf, FindLastNotOf are not implemented in CString
	size_type FindOneNotOf ( const _This& _CharSet, size_type _Pos = 0 ) const
	{
		return find_first_not_of(_CharSet, _Pos);
	}
	size_type FindLastOf ( const _This& _CharSet, size_type _Pos = npos ) const
	{
		return find_last_of(_CharSet, _Pos);
	}
	size_type FindLastNotOf ( const _This& _CharSet, size_type _Pos = npos ) const
	{
		return find_last_not_of(_CharSet, _Pos);
	}

	_This substr_of ( const _This& _CharSet )  const
	{
		size_type	_FirstNotOf = find_first_not_of(_CharSet);
		return  _This(c_str(), (_FirstNotOf == npos ? size() : _FirstNotOf));
	}
	_This substr_not_of ( const _This& _CharSet )  const
	{
		size_type	_FirstOf = find_first_of(_CharSet);
		return  _This(c_str(), (_FirstOf == npos ? size() : _FirstOf));
	}
	_This SpanIncluding ( const _This& _CharSet )  const  {  return  substr_of(_CharSet);  }
	_This SpanExcluding ( const _This& _CharSet )  const  {  return  substr_not_of(_CharSet);  }

	void  trim ( _E _C )
	{
		size_type	_FirstNotOf = find_first_not_of(_C);
		_trim_upto(_FirstNotOf);
	}
	//#define  _SpaceSymbol _ForType<_E>::Select(" \t\n\r\v", L" \t\n\r\v")
	void  trim ( const _This& _CharSet = _SpaceSymbol )
	{
		size_type	_FirstNotOf = find_first_not_of(_CharSet);
		_trim_upto(_FirstNotOf);
	}
	void  rtrim ( _E _C )
	{
		size_type	_LastNotOf = find_last_not_of(_C);
		_trim_from(_LastNotOf);
	}
	void  rtrim ( const _This& _CharSet = sm_SpaceSym )
	{
		size_type	_LastNotOf = find_last_not_of(_CharSet);
		_trim_from(_LastNotOf);
	}
	void  TrimLeft ( _E _C )  {  trim(_C);  }
	void  TrimLeft ( const _This& _CharSet = sm_SpaceSym )  {  trim(_CharSet);  }
	void  TrimRight ( _E _C )  {  trim(_C);  }
	void  TrimRight ( const _This& _CharSet = sm_SpaceSym )  {  rtrim(_CharSet);  }

	void  to_upper ()
	{
		begin();	// freeze this string
		if ( sizeof(_E) == sizeof(wchar_t) )
			wcsupr ((wchar_t*)c_str());
		else
			strupr ((char*)c_str());
	}
	void  to_lower ()
	{
		begin();	// freeze this string
		if ( sizeof(_E) == sizeof(wchar_t) )
			wcslwr ((wchar_t*)c_str());
		else
			strlwr ((char*)c_str());
	}

	void  MakeUpper ()  {  to_upper();  }
	void  MakeLower ()  {  to_lower();  }

	// Insert character at zero-based index; concatenates if index is past end of string
	_This&  Insert ( size_type _Pos, _E _C )
	{
		insert ((_Pos > length() ? length() : _Pos), 1, _C);
		return  *this;
	}
	// Insert substring at zero-based index; concatenates if index is past end of string
	_This&  Insert ( size_type _Pos, const _This& _S )
	{
		insert ((_Pos > length() ? length() : _Pos), _S);
		return  *this;
	}
	_This&  Delete ( size_type _Pos, size_type _N = 1 )
	{
		erase(_Pos, _N);
		return  *this;
	}

	_This&  Replace ( _E chOld, _E chNew )
	{
		_E	*_Str = (_E*)c_str();
		while ( *_Str )  {
			if ( *_Str == chOld )
				*_Str = chNew;
			_Str++;
		}
		return  *this;
	}
	_This&  Replace ( const _This& strOld, const _This& strNew )
	{
		long	_DLen = strNew.length() - strOld.length();
		size_type	_Pos = 0;
		while ( (_Pos = find(strOld, _Pos)) != npos )  {
			if ( _DLen < 0 )
				erase (_Pos, -_DLen);
			else if ( _DLen > 0 )
				insert (_Pos, _DLen, _E(32));
			replace(_Pos, strNew.length(), strNew);
			_Pos += strNew.length();
		}
		return  *this;
	}
};	// BasicString

template<class _E, class _Tr, class _A>
HINSTANCE	BasicString<_E, _Tr, _A>::sm_hInst = GetModuleHandle (NULL);

template<class _E, class _Tr, class _A>
const  BasicString<_E, _Tr, _A>	
		BasicString<_E, _Tr, _A>::sm_SpaceSym = 
		_ForType<_E>::Select(" \t\n\r\v", L" \t\n\r\v");

//
//     The following operators provide interoperability with the classes
// _bstr_t, CString, and std::basic_string.
//

//////////////////////////////////////////////////////////////////////////
// operator +
//

//     In all these overloaded '+' operators the left operand of the 
// internally used '+=' operator must be a temporary variable constructed 
// from the left argument to avoid modification of the latter.

template<class _E, class _E2, class _Tr, class _Tr2, class _A, class _A2> 
inline
	BasicString<_E, _Tr, _A>  operator + ( 
							const BasicString<_E, _Tr, _A>& _L, 
							const BasicString<_E2, _Tr2, _A2>& _R )
{
	return  BasicString<_E, _Tr, _A>(_L) += _R;
}
template<class _E, class _E2, class _Tr, class _Tr2, class _A, class _A2> 
inline 
	BasicString<_E, _Tr, _A>  operator + ( 
							const std::basic_string<_E, _Tr, _A>& _L, 
							const BasicString<_E2, _Tr2, _A2>& _R )
{
	return  BasicString<_E, _Tr, _A>(_L) += _R;
}
template<class _E, class _E2, class _Tr, class _Tr2, class _A, class _A2> 
inline
    BasicString<_E, _Tr, _A>  operator + ( 
							const BasicString<_E, _Tr, _A>& _L, 
							const std::basic_string<_E2, _Tr2, _A2>& _R )
{
	return  BasicString<_E, _Tr, _A>(_L) += BasicString<_E, _Tr, _A>(_R);
}

//////////////////////////////////////////////////////////////////////////
// operator ==
//
template<class _E, class _E2, class _Tr, class _Tr2, class _A, class _A2> 
inline
    bool  operator == ( const BasicString<_E, _Tr, _A>& _L, 
					   const BasicString<_E2, _Tr2, _A2>& _R )
{
	return  _L.compare(_R) == 0;
}
template<class _E, class _E2, class _Tr, class _Tr2, class _A, class _A2>
inline
    bool  operator == ( const std::basic_string<_E, _Tr, _A>& _L, 
						const BasicString<_E2, _Tr2, _A2>& _R )
{
	return  BasicString<_E, _Tr, _A>(_L).compare(_R) == 0;
}
template<class _E, class _E2, class _Tr, class _Tr2, class _A, class _A2> 
inline
    bool  operator == ( const BasicString<_E, _Tr, _A>& _L, 
						const std::basic_string<_E2, _Tr2, _A2>& _R )
{
	return  _L.compare(BasicString<_E2, _Tr2, _A2>(_R)) == 0;
}

//////////////////////////////////////////////////////////////////////////
// operator !=
//
template<class _E, class _E2, class _Tr, class _Tr2, class _A, class _A2> 
inline
    bool  operator != ( const BasicString<_E, _Tr, _A>& _L, 
						const BasicString<_E2, _Tr2, _A2>& _R )
{
	return  !(_L == _R);
}
template<class _E, class _E2, class _Tr, class _Tr2, class _A, class _A2> 
inline
    bool  operator != ( const std::basic_string<_E, _Tr, _A>& _L, 
						const BasicString<_E2, _Tr2, _A2>& _R )
{
	return  !(_L == _R);
}
template<class _E, class _E2, class _Tr, class _Tr2, class _A, class _A2> 
inline
    bool  operator != ( const BasicString<_E, _Tr, _A>& _L, 
						const std::basic_string<_E2, _Tr2, _A2>& _R )
{
	return  !(_L == _R);
}

//////////////////////////////////////////////////////////////////////////
// operator <
//
template<class _E, class _E2, class _Tr, class _Tr2, class _A, class _A2> 
inline
    bool  operator < ( const BasicString<_E, _Tr, _A>& _L, 
					   const BasicString<_E2, _Tr2, _A2>& _R )
{
	return  _L.compare(_R) < 0;
}
template<class _E, class _E2, class _Tr, class _Tr2, class _A, class _A2> 
inline
    bool  operator < ( const std::basic_string<_E, _Tr, _A>& _L, 
					   const BasicString<_E2, _Tr2, _A2>& _R )
{
	return  BasicString<_E, _Tr, _A>(_L).compare(_R) < 0;
}
template<class _E, class _E2, class _Tr, class _Tr2, class _A, class _A2> 
inline
    bool  operator < ( const BasicString<_E, _Tr, _A>& _L, 
					   const std::basic_string<_E2, _Tr2, _A2>& _R )
{
	return  _L.compare(BasicString<_E2, _Tr2, _A2>(_R)) < 0;
}

//////////////////////////////////////////////////////////////////////////
// operator >
//
template<class _E, class _E2, class _Tr, class _Tr2, class _A, class _A2> 
inline
    bool  operator > ( const BasicString<_E, _Tr, _A>& _L, 
					   const BasicString<_E2, _Tr2, _A2>& _R )
{
	return  _L.compare(_R) > 0;
}
template<class _E, class _E2, class _Tr, class _Tr2, class _A, class _A2> 
inline
    bool  operator > ( const std::basic_string<_E, _Tr, _A>& _L, 
					   const BasicString<_E2, _Tr2, _A2>& _R )
{
	return  BasicString<_E, _Tr, _A>(_L).compare(_R) > 0;
}
template<class _E, class _E2, class _Tr, class _Tr2, class _A, class _A2> 
inline
    bool  operator > ( const BasicString<_E, _Tr, _A>& _L, 
					   const std::basic_string<_E2, _Tr2, _A2>& _R )
{
	return  _L.compare(BasicString<_E2, _Tr2, _A2>(_R)) > 0;
}

//////////////////////////////////////////////////////////////////////////
// operator <=
//
template<class _E, class _E2, class _Tr, class _Tr2, class _A, class _A2> 
inline
    bool  operator <= ( const BasicString<_E, _Tr, _A>& _L, 
						const BasicString<_E2, _Tr2, _A2>& _R )
{
	return  _L.compare(_R) <= 0;
}
template<class _E, class _E2, class _Tr, class _Tr2, class _A, class _A2> 
inline
    bool  operator <= ( const std::basic_string<_E, _Tr, _A>& _L, 
					    const BasicString<_E2, _Tr2, _A2>& _R )
{
	return  BasicString<_E, _Tr, _A>(_L).compare(_R) <= 0;
}
template<class _E, class _E2, class _Tr, class _Tr2, class _A, class _A2> 
inline
    bool  operator <= ( const BasicString<_E, _Tr, _A>& _L, 
						const std::basic_string<_E2, _Tr2, _A2>& _R )
{
	return  _L.compare(BasicString<_E2, _Tr2, _A2>(_R)) <= 0;
}

//////////////////////////////////////////////////////////////////////////
// operator >=
//
template<class _E, class _E2, class _Tr, class _Tr2, class _A, class _A2> 
inline
    bool  operator >= ( const BasicString<_E, _Tr, _A>& _L, 
						const BasicString<_E2, _Tr2, _A2>& _R )
{
	return  _L.compare(_R) >= 0;
}
template<class _E, class _E2, class _Tr, class _Tr2, class _A, class _A2> 
inline
    bool  operator >= ( const std::basic_string<_E, _Tr, _A>& _L, 
						const BasicString<_E2, _Tr2, _A2>& _R )
{
	return  BasicString<_E, _Tr, _A>(_L).compare(_R) >= 0;
}
template<class _E, class _E2, class _Tr, class _Tr2, class _A, class _A2> 
inline
    bool  operator >= ( const BasicString<_E, _Tr, _A>& _L, 
						const std::basic_string<_E2, _Tr2, _A2>& _R )
{
	return  _L.compare(BasicString<_E2, _Tr2, _A2>(_R)) >= 0;
}

//////////////////////////////////////////////////////////////////////////
// operator += (CString support)
//
#if defined(__CSTRING_AW_SUPPORT__)
template<class _E, class _Tr, class _A> inline
    CStringA __cdecl operator += ( CStringA& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _L += (LPCSTR)_R;
}
template<class _E, class _Tr, class _A> inline
    CStringW __cdecl operator += ( CStringW& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _L += (LPCWSTR)_R;
}
#elif defined(__CSTRING_SUPPORT__)
template<class _E, class _Tr, class _A> inline
    CString __cdecl operator += ( CString& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _L += (LPCTSTR)_R;
}
#endif

//////////////////////////////////////////////////////////////////////////
// operator + (raw character pointers, _bstr_t and CString support)
//
template<class _E, class _E2, class _Tr, class _A> inline
    BasicString<_E, _Tr, _A> __cdecl operator + ( const BasicString<_E, _Tr, _A>& _L, const _E2* _R )
{
	return  BasicString<_E, _Tr, _A>(_L) += _R;
}
template<class _E, class _E2, class _Tr, class _A> inline
    BasicString<_E, _Tr, _A>  operator + ( const _E2* _L, 
								const BasicString<_E, _Tr, _A>& _R )
{
	return  BasicString<_E, _Tr, _A>(_L) += _R;
}

#if defined(__BSTR_T_SUPPORT__)
template<class _E, class _Tr, class _A> inline
    BasicString<_E, _Tr, _A>  operator + ( 
								const BasicString<_E, _Tr, _A>& _L, 
								const _bstr_t& _R )
{
	return  BasicString<_E, _Tr, _A>(_L) += (const _E*)_R;
}
#endif

#if defined(__CSTRING_AW_SUPPORT__)
template<class _E, class _Tr, class _A> inline
    CStringA  operator + ( const BasicString<_E, _Tr, _A>& _L, 
						  const CStringA& _R )
{
	return  CStringA((LPCSTR)_L) += _R;
}

template<class _E, class _Tr, class _A> inline
    CStringA  operator + ( const CStringA& _L, 
						  const BasicString<_E, _Tr, _A>& _R )
{
	return  CStringA(_L) += CStringA((LPCSTR)_R);
}

template<class _E, class _Tr, class _A> inline
    CStringW  operator + ( const BasicString<_E, _Tr, _A>& _L, 
						  const CStringW& _R )
{
	return  CStringW((LPCWSTR)_L) += _R;
}

template<class _E, class _Tr, class _A> inline
    CStringW  operator + ( const CStringW& _L, 
						  const BasicString<_E, _Tr, _A>& _R )
{
	return  CStringW(_L) += CStringW((LPCWSTR)_R);
}
#elif defined(__CSTRING_SUPPORT__)
template<class _E, class _Tr, class _A> inline
    CString  operator + ( const BasicString<_E, _Tr, _A>& _L, 
						  const CString& _R )
{
	return  CString((LPCWSTR)_L) += _R;
}

template<class _E, class _Tr, class _A> inline
    CString  operator + ( const CString& _L, 
						  const BasicString<_E, _Tr, _A>& _R )
{
	return  CString(_L) += CString((LPCTSTR)_R);
}
#endif

//////////////////////////////////////////////////////////////////////////
// operator == (raw character pointers, _bstr_t and CString support)
//
template<class _E, class _E2, class _Tr, class _A> inline
bool __cdecl  operator == ( const _E *_L, const BasicString<_E2, _Tr, _A>& _R)
{
	return  _R.compare(BasicString<_E2>(_L)) == 0;
}
template<class _E, class _E2, class _Tr, class _A> inline
bool __cdecl  operator == ( const BasicString<_E, _Tr, _A>& _L, const _E2 *_R)
{
	return  _L.compare((const _E*)BasicString<_E2>(_R)) == 0;
}

#if defined(__BSTR_T_SUPPORT__)
template<class _E, class _Tr, class _A> inline
bool __cdecl operator == ( const BasicString<_E, _Tr, _A>& _L, const _bstr_t& _R )
{
	return  _L.compare((const _E*)_R) == 0;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator == ( const _bstr_t& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R == (const _E*)_L;
}
#endif

#if defined(__CSTRING_AW_SUPPORT__)
template<class _E, class _Tr, class _A> inline
    bool __cdecl operator == ( const BasicString<_E, _Tr, _A>& _L, const CStringA& _R )
{
	return  _L == (LPCSTR)_R;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator == ( const CStringA& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R == (LPCSTR)_L;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator == ( const BasicString<_E, _Tr, _A>& _L, const CStringW& _R )
{
	return  _L == (LPCWSTR)_R;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator == ( const CStringW& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R == (LPCWSTR)_L;
}
#elif defined(__CSTRING_SUPPORT__)
template<class _E, class _Tr, class _A> inline
    bool __cdecl operator == ( const BasicString<_E, _Tr, _A>& _L, const CString& _R )
{
	return  _L == (LPCTSTR)_R;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator == ( const CString& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R == (LPCTSTR)_L;
}
#endif


//////////////////////////////////////////////////////////////////////////
// operator != (raw character pointers, _bstr_t and CString support)
//
template<class _E, class _E2, class _Tr, class _A> inline
bool __cdecl  operator != ( const _E *_L, const BasicString<_E2, _Tr, _A>& _R)
{
	return  !(_L == _R);
}
template<class _E, class _E2, class _Tr, class _A> inline
bool __cdecl  operator != ( const BasicString<_E, _Tr, _A>& _L, const _E2 *_R)
{
	return  !(_L == _R);
}

#if defined(__BSTR_T_SUPPORT__)
template<class _E, class _Tr, class _A> inline
bool __cdecl operator != ( const BasicString<_E, _Tr, _A>& _L, const _bstr_t& _R )
{
	return  _L.compare((const _E*)_R) != 0;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator != ( const _bstr_t& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R != (const _E*)_L;
}
#endif

#if defined(__CSTRING_AW_SUPPORT__)
template<class _E, class _Tr, class _A> inline
    bool __cdecl operator != ( const BasicString<_E, _Tr, _A>& _L, const CStringA& _R )
{
	return  _L != (LPCSTR)_R;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator != ( const CStringA& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R != (LPCSTR)_L;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator != ( const BasicString<_E, _Tr, _A>& _L, const CStringW& _R )
{
	return  _L != (LPCWSTR)_R;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator != ( const CStringW& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R != (LPCWSTR)_L;
}
#elif defined(__CSTRING_SUPPORT__)
template<class _E, class _Tr, class _A> inline
    bool __cdecl operator != ( const BasicString<_E, _Tr, _A>& _L, const CString& _R )
{
	return  _L != (LPCTSTR)_R;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator != ( const CString& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R != (LPCTSTR)_L;
}
#endif

//////////////////////////////////////////////////////////////////////////
// operator < (raw character pointers, _bstr_t and CString support)
//
template<class _E, class _E2, class _Tr, class _A> inline
bool __cdecl  operator < ( const _E *_L, const BasicString<_E2, _Tr, _A>& _R)
{
	return  _R.compare(BasicString<_E2>(_L)) > 0;
}
template<class _E, class _E2, class _Tr, class _A> inline
bool __cdecl  operator < ( const BasicString<_E, _Tr, _A>& _L, const _E2 *_R)
{
	return  _L.compare((const _E*)BasicString<_E2>(_R)) < 0;
}

#if defined(__BSTR_T_SUPPORT__)
template<class _E, class _Tr, class _A> inline
    bool __cdecl operator < ( const BasicString<_E, _Tr, _A>& _L, const _bstr_t& _R )
{
	return  _L.compare((const _E*)_R) < 0;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator < ( const _bstr_t& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R > (const _E*)_L;
}
#endif

#if defined(__CSTRING_AW_SUPPORT__)
template<class _E, class _Tr, class _A> inline
    bool __cdecl operator < ( const BasicString<_E, _Tr, _A>& _L, const CStringA& _R )
{
	return  _L < (LPCSTR)_R;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator < ( const CStringA& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R > (LPCSTR)_L;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator < ( const BasicString<_E, _Tr, _A>& _L, const CStringW& _R )
{
	return  _L < (LPCWSTR)_R;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator < ( const CStringW& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R > (LPCWSTR)_L;
}
#elif defined(__CSTRING_SUPPORT__)
template<class _E, class _Tr, class _A> inline
    bool __cdecl operator < ( const BasicString<_E, _Tr, _A>& _L, const CString& _R )
{
	return  _L < (LPCTSTR)_R;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator < ( const CString& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R > (LPCTSTR)_L;
}
#endif

//////////////////////////////////////////////////////////////////////////
// operator > (raw character pointers, _bstr_t and CString support)
//
template<class _E, class _E2, class _Tr, class _A> inline
bool __cdecl  operator > ( const _E *_L, const BasicString<_E2, _Tr, _A>& _R)
{
	return  _R.compare(BasicString<_E2>(_L)) < 0;
}
template<class _E, class _E2, class _Tr, class _A> inline
bool __cdecl  operator > ( const BasicString<_E, _Tr, _A>& _L, const _E2 *_R)
{
	return  _L.compare((const _E*)BasicString<_E2>(_R)) > 0;
}

#if defined(__BSTR_T_SUPPORT__)
template<class _E, class _Tr, class _A> inline
    bool __cdecl operator > ( const BasicString<_E, _Tr, _A>& _L, const _bstr_t& _R )
{
	return  _L.compare((const _E*)_R) > 0;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator > ( const _bstr_t& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R < (const _E*)_L;
}
#endif

#if defined(__CSTRING_AW_SUPPORT__)
template<class _E, class _Tr, class _A> inline
    bool __cdecl operator > ( const BasicString<_E, _Tr, _A>& _L, const CStringA& _R )
{
	return  _L > (LPCSTR)_R;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator > ( const CStringA& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R < (LPCSTR)_L;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator > ( const BasicString<_E, _Tr, _A>& _L, const CStringW& _R )
{
	return  _L > (LPCWSTR)_R;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator > ( const CStringW& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R < (LPCWSTR)_L;
}
#elif defined(__CSTRING_SUPPORT__)
template<class _E, class _Tr, class _A> inline
    bool __cdecl operator > ( const BasicString<_E, _Tr, _A>& _L, const CString& _R )
{
	return  _L > (LPCTSTR)_R;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator > ( const CString& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R < (LPCTSTR)_L;
}
#endif

//////////////////////////////////////////////////////////////////////////
// operator <= (raw character pointers, _bstr_t and CString support)
//
template<class _E, class _E2, class _Tr, class _A> inline
bool __cdecl  operator <= ( const _E *_L, const BasicString<_E2, _Tr, _A>& _R)
{
	return  _R.compare(BasicString<_E2>(_L)) >= 0;
}
template<class _E, class _E2, class _Tr, class _A> inline
bool __cdecl  operator <= ( const BasicString<_E, _Tr, _A>& _L, const _E2 *_R)
{
	return  _L.compare((const _E*)BasicString<_E2>(_R)) <= 0;
}

#if defined(__BSTR_T_SUPPORT__)
template<class _E, class _Tr, class _A> inline
    bool __cdecl operator <= ( const BasicString<_E, _Tr, _A>& _L, const _bstr_t& _R )
{
	return  _L.compare((const _E*)_R) <= 0;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator <= ( const _bstr_t& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R >= (const _E*)_L;
}
#endif

#if defined(__CSTRING_AW_SUPPORT__)
template<class _E, class _Tr, class _A> inline
    bool __cdecl operator <= ( const BasicString<_E, _Tr, _A>& _L, const CStringA& _R )
{
	return  _L <= (LPCSTR)_R;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator <= ( const CStringA& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R >= (LPCSTR)_L;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator <= ( const BasicString<_E, _Tr, _A>& _L, const CStringW& _R )
{
	return  _L <= (LPCWSTR)_R;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator <= ( const CStringW& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R >= (LPCWSTR)_L;
}
#elif defined(__CSTRING_SUPPORT__)
template<class _E, class _Tr, class _A> inline
    bool __cdecl operator <= ( const BasicString<_E, _Tr, _A>& _L, const CString& _R )
{
	return  _L <= (LPCTSTR)_R;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator <= ( const CString& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R >= (LPCTSTR)_L;
}
#endif

//////////////////////////////////////////////////////////////////////////
// operator >= (raw character pointers, _bstr_t and CString support)
//
template<class _E, class _E2, class _Tr, class _A> inline
bool __cdecl  operator >= ( const _E *_L, const BasicString<_E2, _Tr, _A>& _R)
{
	return  _R.compare(BasicString<_E2>(_L)) <= 0;
}
template<class _E, class _E2, class _Tr, class _A> inline
bool __cdecl  operator >= ( const BasicString<_E, _Tr, _A>& _L, const _E2 *_R)
{
	return  _L.compare((const _E*)BasicString<_E2>(_R)) >= 0;
}

#if defined(__BSTR_T_SUPPORT__)
template<class _E, class _Tr, class _A> inline
    bool __cdecl operator >= ( const BasicString<_E, _Tr, _A>& _L, const _bstr_t& _R )
{
	return  _L >= (const _E*)_R;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator >= ( const _bstr_t& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R <= (const _E*)_L;
}
#endif

#if defined(__CSTRING_AW_SUPPORT__)
template<class _E, class _Tr, class _A> inline
    bool __cdecl operator >= ( const BasicString<_E, _Tr, _A>& _L, const CStringA& _R )
{
	return  _L >= (LPCSTR)_R;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator >= ( const CStringA& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R <= (LPCSTR)_L;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator >= ( const BasicString<_E, _Tr, _A>& _L, const CStringW& _R )
{
	return  _L >= (LPCWSTR)_R;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator >= ( const CStringW& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R <= (LPCWSTR)_L;
}
#elif defined(__CSTRING_SUPPORT__)
template<class _E, class _Tr, class _A> inline
    bool __cdecl operator >= ( const BasicString<_E, _Tr, _A>& _L, const CString& _R )
{
	return  _L >= (LPCTSTR)_R;
}

template<class _E, class _Tr, class _A> inline
    bool __cdecl operator >= ( const CString& _L, const BasicString<_E, _Tr, _A>& _R )
{
	return  _R <= (LPCTSTR)_L;
}
#endif


template<class _E, class _Tr, class _A>
void  StrFromEltPtr<_E, _Tr, _A>::Init ( BasicString<_E, _Tr, _A>* pStr, 
										 const _E* pszSrc )
{
	if ( !pStr->_init_with_non_string_ptr(pszSrc) )
		pStr->assign (pszSrc);
}

template<class _E, class _Tr, class _A>
void StrFromCharPtr<_E, _Tr, _A>::Init ( BasicString<_E, _Tr, _A>* pStr, 
										 const char* pszSrc )
{
	_ASSERTE (Types<_E>::And<wchar_t>::AreSame);
	if ( !pStr->_init_with_non_string_ptr(pszSrc) )
		pStr->_initA (pszSrc, strlen(pszSrc));
}

template<class _E, class _Tr, class _A>
void  StrFromWCharPtr<_E, _Tr, _A>::Init ( 
										BasicString<_E, _Tr, _A>* pStr, 
										const wchar_t* pwszSrc )
{
	_ASSERTE (Types<_E>::And<char>::AreSame);
	if ( !pStr->_init_with_non_string_ptr(pwszSrc) )
		pStr->_initW (pwszSrc, wcslen(pwszSrc));
}

//     The following class without declaration 
// is used for compile-time error detection.
class  c_str_Method_Must_Return_Pointer_To_Ordinary_Or_Wide_Character_String;

typedef c_str_Method_Must_Return_Pointer_To_Ordinary_Or_Wide_Character_String
		Wrong_c_str_Method;

template<class _E, class _Tr, class _A, class T>
void  StrFrom_c_str_Holder<_E, _Tr, _A, T>::Init ( 
										BasicString<_E, _Tr, _A>* pStr, 
										const T& Src )
{
	enum  { ElementPtr, CharPtr, WCharPtr, Default,
			Selector = 
				(IS_EXPR_OF_TYPE(Src.c_str(), const _E*) ? ElementPtr :
				(IS_EXPR_OF_TYPE(Src.c_str(), const char*) ? CharPtr :
				(IS_EXPR_OF_TYPE(Src.c_str(), const wchar_t*) ? WCharPtr : 
				 Default)))
			  };
	meta::SWITCH<Selector, 
				meta::CASE<ElementPtr, StrFromEltPtr<_E, _Tr, _A>, 
				meta::CASE<CharPtr, StrFromCharPtr<_E, _Tr, _A>, 
				meta::CASE<WCharPtr, StrFromWCharPtr<_E, _Tr, _A>,
				meta::CASE<DEFAULT_CASE, Wrong_c_str_Method
				> > > >
			  >::RET::Init (pStr, Src.c_str());
}

}	// end of namespace std extension

#define _initWithNum(charType, n, ntosFn)	\
	charType##String	str;				\
	ntosFn (n, str.get_buf(34), 10);	\
	str.set_buf();							\
	return  str

#define DEFINE_INTEGRAL_TO_STRING(T, ntosFnA, ntosFnW)		\
	namespace std_ex {										\
	template<> inline AString ToAString<T> ( const T& n )	\
	{ _initWithNum(A, n, ntosFnA); }						\
	template<> inline WString ToWString<T> ( const T& n )	\
	{ _initWithNum(W, n, ntosFnW); } }

#define DEFINE_TYPE_TO_STRING_1(T, obj, fmt, arg1)			\
	namespace std_ex {										\
	template<> inline AString ToAString<T> ( const T& obj )	\
	{ AString str;  str.printf(fmt, arg1);  return str; }	\
	template<> inline WString ToWString<T> ( const T& obj )	\
	{ WString str;  str.printf(L##fmt, arg1);  return str; } }

#define DEFINE_TYPE_TO_STRING_2(T, obj, fmt, arg1, arg2)		\
	namespace std_ex {											\
	template<> inline AString ToAString<T> ( const T& obj )		\
	{ AString str;  str.printf(fmt, arg1, arg2);  return str; }	\
	template<> inline WString ToWString<T> ( const T& obj )		\
	{ WString str;  str.printf(L##fmt, arg1, arg2);  return str; } }

#define DEFINE_TYPE_TO_STRING_3(T, obj, fmt, arg1, arg2, arg3)			\
	namespace std_ex {													\
	template<> inline AString ToAString<T> ( const T& obj )				\
	{ AString str;  str.printf(fmt, arg1, arg2, arg3);  return str; }	\
	template<> inline WString ToWString<T> ( const T& obj )				\
	{ WString str;  str.printf(L##fmt, arg1, arg2, arg3);  return str; } }

#define DEFINE_TYPE_TO_STRING_4(T, obj, fmt, arg1, arg2, arg3, arg4)		\
	namespace std_ex {														\
	template<> inline AString ToAString<T> ( const T& obj )					\
	{ AString str;  str.printf(fmt, arg1, arg2, arg3, arg4);  return str; }	\
	template<> inline WString ToWString<T> ( const T& obj )					\
	{ WString str;  str.printf(L##fmt, arg1, arg2, arg3, arg4);  return str; } }

namespace std_ex  {
template<typename T> inline
WString ToWString ( const T& t )
{
	return  WString(t);
}
template<typename T> inline
AString ToAString ( const T& t )
{
	return  AString(t);
}

} // namespace std_ex

DEFINE_INTEGRAL_TO_STRING(int, _itoa, _itow)
DEFINE_INTEGRAL_TO_STRING(UINT, _ultoa, _ultow)
DEFINE_INTEGRAL_TO_STRING(long, _ltoa, _ltow)
DEFINE_INTEGRAL_TO_STRING(ULONG, _ultoa, _ultow)
DEFINE_INTEGRAL_TO_STRING(__int64, _i64toa, _i64tow)
DEFINE_INTEGRAL_TO_STRING(unsigned __int64, _ui64toa, _ui64tow)
DEFINE_TYPE_TO_STRING_1(float, n, "%g", n)
DEFINE_TYPE_TO_STRING_1(double, n, "%g", n)

#if defined(__WTYPES_SUPPORT__)
DEFINE_TYPE_TO_STRING_4(RECT, rc, "{%d, %d, %d, %d}", 
						rc.left, rc.top, rc.right, rc.bottom)
DEFINE_TYPE_TO_STRING_2(POINT, pt, "{%d, %d}", pt.x, pt.y)
DEFINE_TYPE_TO_STRING_2(SIZE, size, "{%d, %d}", size.cx, size.cy)
#endif // __WTYPES_SUPPORT__

#if defined(__ATLTYPES_SUPPORT__)
DEFINE_TYPE_TO_STRING_4(CRect, rc, "{%d, %d, %d, %d}", 
						rc.left, rc.top, rc.right, rc.bottom)
DEFINE_TYPE_TO_STRING_2(CPoint, pt, "{%d, %d}", pt.x, pt.y)
DEFINE_TYPE_TO_STRING_2(CSize, size, "{%d, %d}", size.cx, size.cy)
#endif // __ATLTYPES_SUPPORT__

#define  ToWstr(str)	(LPCWSTR(std_ex::ToWString(str)))
#define  ToAstr(str)	(LPCSTR(std_ex::ToAString(str)))

#define  ResToWstr(nID)	((LPCWSTR)std_ex::WString((LPCWSTR)(nID)))
#define  ResToAstr(nID)	((LPCSTR)std_ex::AString((LPCSTR)(nID)))

#define  GuidToWstr(guid)	((LPCWSTR)std_ex::WString(guid))
#define  GuidToAstr(guid)	((LPCSTR)std_ex::AString(guid))

#define  WstrToGuid(str)	(std_ex::WString(str).guid())
#define  AstrToGuid(str)	(std_ex::AString(str).guid())


#if !defined(USE_CURRENT_STL_IMPL)

//     Macros from the _R family are intended to be used for optimized
// (maximally quick) initialization of the BasicString objects with
// string literals. _R is used instead of _T macro, and _RA/_RW macros 
// are used to wrap the ANSI and Unicode string literals correspondingly.
//     Caution! Use _R, and _R... macros only to initialize local
// BasicString objects or to pass a raw string to a function accepting 
// BasicString object as its argument.
//     Note that _Str argument of a _R, _RA, or _RW macro MUST be a 
// string literal (i.e. a quoted string). This is because _RW (and, 
// consequently, sometimes _R) macro prepends the 'L' letter to its 
// argument, and, moreover, uses sizeof(_Str) to determine its character
// length.

#define _RA(_Str)	_RA_EX(_Str, sizeof(_Str) - 1)
#define _RW(_Str)	_RW_EX(L##_Str, sizeof(_Str) - 1)

#define _RA_EX(_P, _Len)	\
		__bs_internal__::FastRawStringInitializer<char>(				\
				(char*)_alloca(((_Len) + 2								\
					+ __bs_internal__::StringBase<char>::_PrefixArea	\
					+ 3) & ~3),											\
				(_P), (_Len))

#define _RW_EX(_P, _Len)	\
		__bs_internal__::FastRawStringInitializer<wchar_t>(				\
			(wchar_t*)_alloca((((_Len) + 2 								\
				+ __bs_internal__::StringBase<wchar_t>::_PrefixArea)	\
					* sizeof(wchar_t) + 3) & ~3),						\
				(_P), (_Len))

#else  // USE_CURRENT_STL_IMPL

#define _RA(_Str)	_Str
#define _RW(_Str)	L##_Str

#define _RA_EX(_Str, _Len)	(_Str)
#define _RW_EX(_Str, _Len)	(_Str)

#endif // USE_CURRENT_STL_IMPL

#if defined(UNICODE) || defined(_UNICODE)
	#define  ToStr		ToWstr
	#define  ResToStr	ResToWstr
	#define  GuidToStr	GuidToWstr
	#define  StrToGuid	WstrToGuid
	#define  _R			_RW
	#define  _R_EX		_RW_EX
#else // ASCII
	#define  ToStr		ToAstr
	#define  ResToStr	ResToAstr
	#define  GuidToStr	GuidToAstr
	#define  StrToGuid	AstrToGuid
	#define  _R			_RA
	#define  _R_EX		_RA_EX
#endif // ASCII

#endif  // __BASIC_STRING_EX__
