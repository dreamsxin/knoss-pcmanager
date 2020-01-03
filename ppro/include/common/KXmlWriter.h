#ifndef __KXmlWriter_h__
#define __KXmlWriter_h__

#include "tinyxml.h"

class KXmlWriter
{
public:
	KXmlWriter();

	BOOL LoadFile(IN LPCSTR szXmlFile);
	BOOL WriteFile(IN LPCSTR szXmlFile);

	BOOL Parse(IN LPCSTR szXml);

	BOOL Open(IN LPCSTR szPath);
	BOOL Write(IN LPCSTR szKey, IN int nValue);
	BOOL Write(IN LPCSTR szKey, IN LPCSTR szValue);
	BOOL Write(IN LPCSTR szKey, IN LPCWSTR szValue);
	BOOL WriteText(IN LPCSTR szText, BOOL bCDATA = FALSE);
	BOOL WriteText(IN LPCWSTR szText, BOOL bCDATA = FALSE);

	BOOL AddNewElement(IN LPCSTR szKey);

	LPCSTR ToXml(void);

protected:
	TiXmlElement* _Open(LPCSTR szPath);
	TiXmlElement* _Create(TiXmlNode* pParentNode, LPCSTR szPath);
	TiXmlElement* _FindSiblingElement(TiXmlElement* pElement, LPCSTR szFolder);

private:
	TiXmlDocument m_xmlDoc;
	TiXmlPrinter m_xmlPrinter;
	TiXmlElement* m_pOpenElement;

	TiXmlElement* m_OpenParentElement;
};

#endif