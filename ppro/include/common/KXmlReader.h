#ifndef __KXmlReader_h__
#define __KXmlReader_h__

#include "tinyxml.h"

/*
@ ClassName : KXmlReader
@ Brief		: Xml解析，仅解析utf8编码的XML
*/
class KXmlReader
{
public:
	KXmlReader();
	
	BOOL LoadFile(IN LPCSTR szXmlFile);
	BOOL Parse(IN LPCSTR szXml);

	BOOL Open(IN LPCSTR szPath);
	BOOL Read(IN LPCSTR szKey, OUT int& bValue);
	BOOL Read(IN LPCSTR szKey, OUT bool& nValue);
#if _MSC_VER > 1200 // vc6.0
	void Read(IN LPCSTR szKey, OUT CStringA& strValue);
	void ReadText(OUT CStringA& strText);
#endif

	void Read(IN LPCSTR szKey, OUT CStringW& strValue);
	void ReadText(OUT CStringW& strText);

	BOOL NextSibling();
	BOOL ReadOneElement();

protected:
	TiXmlElement* _Open(LPCSTR szPath);
	TiXmlElement* _FindSiblingElement(TiXmlElement* pElement, LPCSTR szFolder);

private:
	TiXmlDocument m_xmlDoc;
	TiXmlElement* m_pOpenElement;

	BOOL          m_bFirstElement;
};

#endif