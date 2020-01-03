///////////////////////////////////////////////////////////////	
//	
// FileName  :  kseribuffer.h
// Creator   :  yangzhenhui@kingsoft.com
// Date    	 :  2009-12-18  11:28
// Comment   :  
//
///////////////////////////////////////////////////////////////	
#pragma once
#include <vector>
#include <set>
#include <map>
#include <list>
using namespace std;

namespace KIS
{

class KSeriBuffer
{
public:
	KSeriBuffer(void):m_bLoading(FALSE),m_uCurrPos(0)
	{

	}

	~KSeriBuffer(void)
	{
	}

	template <typename T>
	KSeriBuffer& operator<<( T& ogg )
	{
		if( !m_pBuffer.size() )
		{
			SetLoading( TRUE );
			ULONG uLength = GetSize( ogg );
			m_pBuffer.resize( (ULONG)uLength );
		}

		if( m_pBuffer.size() )
			Serialize( *this, ogg );

		return (*this);
	}

	template <typename T>
	KSeriBuffer& operator>>( T& ogg )
	{
		if( m_pBuffer.size() )
		{
			SetLoading( FALSE );
			Serialize( *this, ogg );
		}
		return (*this);
	}

	VOID SetLoading( BOOL bMode )
	{
		if( m_bLoading != bMode )
		{
			m_bLoading = bMode;
			m_uCurrPos = 0;
		}
	}

	BOOL Loading()
	{
		return m_bLoading;
	}

	BOOL m_bLoading;
	ULONG m_uCurrPos;
	vector <char> m_pBuffer;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//buffer
static VOID Serialize(KSeriBuffer &seriBuffer, char* pBuffer, ULONG uSize )
{
	if( seriBuffer.Loading() )
	{
		memcpy( (char*)&seriBuffer.m_pBuffer[seriBuffer.m_uCurrPos], pBuffer, uSize );
		seriBuffer.m_uCurrPos += uSize;
	}
	else
	{
		memcpy( pBuffer, (char*)&seriBuffer.m_pBuffer[seriBuffer.m_uCurrPos], uSize );
		seriBuffer.m_uCurrPos += uSize;
	}
}

//struct
template <typename T>
static ULONG GetSize(T &t)
{
	return sizeof(t);
}

template <typename T>
static ULONG GetSize(T *t)
{
	return sizeof(T);
}

template <typename T>
static VOID Serialize(KSeriBuffer &seriBuffer, T &t )
{
	ULONG uSize = GetSize(t);
	if( seriBuffer.Loading() )
	{
		memcpy( (char*)&seriBuffer.m_pBuffer[seriBuffer.m_uCurrPos], (char*)&t, uSize );
		seriBuffer.m_uCurrPos += uSize;
	}
	else
	{
		memcpy( (char*)&t, (char*)&seriBuffer.m_pBuffer[seriBuffer.m_uCurrPos], uSize);
		seriBuffer.m_uCurrPos += uSize;
	}
}

//////////////////////////////////////////////////////////////////////////
//stl

//string
template <typename chT>
static ULONG GetSize(std::basic_string<chT, char_traits<chT>, allocator<chT> > &t)
{
	return (ULONG)t.size()*sizeof(chT) + sizeof(ULONG);
}

//////////////////////////////////////////////////////////////////////////
/// Put a string in the archive
template <class chT>
static VOID Serialize(KSeriBuffer &seriBuffer, std::basic_string<chT, std::char_traits<chT>, std::allocator<chT> > &str )
{
	ULONG uLen = 0;
	if( seriBuffer.Loading() )
	{
		uLen = (ULONG)str.size()*sizeof(chT);
		seriBuffer << uLen;
		if( uLen != 0 )
		{
			Serialize( seriBuffer, &str[0] , uLen );
		}
	}
	else
	{
		seriBuffer >> uLen;
		if( uLen != 0 )
		{
			str.resize( uLen );
			Serialize( seriBuffer, &str[0] , uLen );
		}
	}
}


//vector
template <class VT>
static ULONG GetSize(std::vector<VT> &container)
{
	ULONG uLen = 0;
	for( std::vector<VT>::iterator item = container.begin(); item != container.end(); item++ )
	{
		uLen += GetSize(*item);
	}
	return uLen + sizeof(ULONG);
}

template <class VT>
static ULONG GetSize(std::vector<VT*> &container)
{
	ULONG uLen = 0;
	for( std::vector<VT*>::iterator item = container.begin(); item != container.end(); item++ )
	{
		uLen += GetSize(**item);
	}
	return uLen + sizeof(ULONG);
}

template <class VT>
static VOID Serialize(KSeriBuffer &seriBuffer, std::vector<VT> &container )
{
	ULONG uLen = 0;
	if( seriBuffer.Loading() )
	{
		uLen = (ULONG)container.size();
		seriBuffer << uLen;
		if( uLen != 0 )
		{
			for( std::vector<VT>::iterator item = container.begin(); item != container.end(); item++ )
			{
				seriBuffer << (*item);
			}
		}
	}
	else
	{
		VT vTemp;
		seriBuffer >> uLen;
		container.clear();
		for( ULONG i=0; i<uLen; i++ )
		{
			seriBuffer >> vTemp;
			container.push_back( vTemp );
		}
	}
}

template <class VT>
static VOID Serialize(KSeriBuffer &seriBuffer, std::vector<VT*> &container )
{
	ULONG uLen = 0;
	if( seriBuffer.Loading() )
	{
		uLen = (ULONG)container.size();
		seriBuffer << uLen;
		if( uLen != 0 )
		{
			for( std::vector<VT*>::iterator item = container.begin(); item != container.end(); item++ )
			{
				seriBuffer << (**item);
			}
		}
	}
	else
	{
		VT *vTemp;
		seriBuffer >> uLen;
		container.clear();
		for( ULONG i=0; i<uLen; i++ )
		{
			vTemp = new VT;
			seriBuffer >> *vTemp;
			container.push_back( vTemp );
		}
	}
}

//list
template <class VT>
static ULONG GetSize(std::list<VT> &container)
{
	ULONG uLen = 0;
	for( std::list<VT>::iterator item = container.begin(); item != container.end(); item++ )
	{
		uLen += GetSize(*item);
	}
	return uLen + sizeof(ULONG);
}

template <class VT>
static ULONG GetSize(std::list<VT*> &container)
{
	ULONG uLen = 0;
	for( std::list<VT>::iterator item = container.begin(); item != container.end(); item++ )
	{
		uLen += GetSize(**item);
	}
	return uLen + sizeof(ULONG);
}


template <class VT>
static VOID Serialize(KSeriBuffer &seriBuffer, std::list<VT> &container )
{
	ULONG uLen = 0;
	if( seriBuffer.Loading() )
	{
		uLen = (ULONG)container.size();
		seriBuffer << uLen;
		if( uLen != 0 )
		{
			for( std::list<VT>::iterator item = container.begin(); item != container.end(); item++ )
			{
				seriBuffer << (*item);
			}
		}
	}
	else
	{
		VT vTemp;
		seriBuffer >> uLen;
		container.clear();
		for( ULONG i=0; i<uLen; i++ )
		{
			seriBuffer >> vTemp;
			container.push_back( vTemp );
		}	
	}
}

template <class VT>
static VOID Serialize(KSeriBuffer &seriBuffer, std::list<VT*> &container )
{
	ULONG uLen = 0;
	if( seriBuffer.Loading() )
	{
		uLen = (ULONG)container.size();
		seriBuffer << uLen;
		if( uLen != 0 )
		{
			for( std::list<VT>::iterator item = container.begin(); item != container.end(); item++ )
			{
				seriBuffer << (**item);
			}
		}
	}
	else
	{
		VT *vTemp;
		seriBuffer >> uLen;
		container.clear();
		for( ULONG i=0; i<uLen; i++ )
		{
			vTemp = new VT;
			seriBuffer >> *vTemp;
			container.push_back( vTemp );
		}	
	}
}


//set
template <class VT>
static ULONG GetSize(std::set<VT> &container)
{
	ULONG uLen = 0;
	for( std::set<VT>::iterator item = container.begin();
		item != container.end(); item++ )
	{
		uLen += GetSize(*item);
	}
	return uLen + sizeof(ULONG);
}

template <class VT>
static ULONG GetSize(std::set<VT*> &container)
{
	ULONG uLen = 0;
	for( std::set<VT*>::iterator item = container.begin();
		item != container.end(); item++ )
	{
		uLen += GetSize(**item);
	}
	return uLen + sizeof(ULONG);
}


template <class VT>
static VOID Serialize(KSeriBuffer &seriBuffer, std::set<VT> &container )
{
	ULONG uLen = 0;
	if( seriBuffer.Loading() )
	{
		uLen = (ULONG)container.size();
		seriBuffer << uLen;
		if( uLen != 0 )
		{
			for( std::set<VT>::iterator item = container.begin(); item != container.end(); item++ )
			{
				seriBuffer << (*item);
			}
		}
	}
	else
	{
		VT vTemp;
		seriBuffer >> uLen;
		container.clear();
		for( ULONG i=0; i<uLen; i++ )
		{
			seriBuffer >> vTemp;
			container.insert( vTemp );
		}	
	}
}

template <class VT>
static VOID Serialize(KSeriBuffer &seriBuffer, std::set<VT*> &container )
{
	ULONG uLen = 0;
	if( seriBuffer.Loading() )
	{
		uLen = (ULONG)container.size();
		seriBuffer << uLen;
		if( uLen != 0 )
		{
			for( std::set<VT*>::iterator item = container.begin(); item != container.end(); item++ )
			{
				seriBuffer << (**item);
			}
		}
	}
	else
	{
		VT *vTemp;
		seriBuffer >> uLen;
		container.clear();
		for( ULONG i=0; i<uLen; i++ )
		{
			vTemp = new VT;
			seriBuffer >> *vTemp;
			container.insert( vTemp );
		}	
	}
}

//map
template <
	class _Kty,
	class _Ty,
	class _Pr,
	class _Alloc
>
static ULONG GetSize(std::map< _Kty, _Ty, _Pr, _Alloc > &container)
{
	ULONG uLen = 0;
	for( std::map< _Kty, _Ty, _Pr, _Alloc >::iterator item = container.begin();
		item != container.end(); item++ )
	{
		uLen += GetSize(item->first);
		uLen += GetSize(item->second);
	}
	return uLen + sizeof(ULONG);
}

template <
class _Kty,
class _Ty,
class _Pr,
class _Alloc
>
static ULONG GetSize(std::map< _Kty, _Ty*, _Pr, _Alloc > &container)
{
	ULONG uLen = 0;
	for( std::map< _Kty, _Ty*, _Pr, _Alloc >::iterator item = container.begin();
		item != container.end(); item++ )
	{
		uLen += GetSize(item->first);
		uLen += GetSize(*item->second);
	}
	return uLen + sizeof(ULONG);
}


template <
	class _Kty,
	class _Ty,
	class _Pr,
	class _Alloc
>
static VOID Serialize(KSeriBuffer &seriBuffer, std::map< _Kty, _Ty, _Pr, _Alloc > &container )
{
	ULONG uLen = 0;
	if( seriBuffer.Loading() )
	{
		uLen = (ULONG)container.size();
		seriBuffer << uLen;
		if( uLen != 0 )
		{
			for( std::map< _Kty, _Ty, _Pr, _Alloc >::iterator item = container.begin(); 
				item != container.end(); item++ )
			{
				seriBuffer << item->first;
				seriBuffer << item->second;
			}
		}
	}
	else
	{
		_Kty _k;
		_Ty _v;
		seriBuffer >> uLen;
		container.clear();
		for( ULONG i=0; i<uLen; i++ )
		{
			seriBuffer >> _k;
			seriBuffer >> _v;
			container[_k] = _v;
		}	
	}
}

template <
class _Kty,
class _Ty,
class _Pr,
class _Alloc
>
static VOID Serialize(KSeriBuffer &seriBuffer, std::map< _Kty, _Ty*, _Pr, _Alloc > &container )
{
	ULONG uLen = 0;
	if( seriBuffer.Loading() )
	{
		uLen = (ULONG)container.size();
		seriBuffer << uLen;
		if( uLen != 0 )
		{
			for( std::map< _Kty, _Ty*, _Pr, _Alloc >::iterator item = container.begin(); 
				item != container.end(); item++ )
			{
				seriBuffer << item->first;
				seriBuffer << *item->second;
			}
		}
	}
	else
	{
		_Kty _k;
		_Ty *_v;
		seriBuffer >> uLen;
		container.clear();
		for( ULONG i=0; i<uLen; i++ )
		{
			_v = new _Ty;
			seriBuffer >> _k;
			seriBuffer >> *_v;
			container[_k] = _v;
		}	
	}
}

//////////////////////////////////////////////////////////////////////////

};


