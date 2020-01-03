#include "stdafx.h"
#include "KXmlReader.h"
#include "KPathParser.h"
#include "kutfconv.h"

KXmlReader::KXmlReader()
{
	m_pOpenElement = NULL;
	m_bFirstElement  = TRUE;
}

BOOL KXmlReader::LoadFile(IN LPCSTR szXmlFile)
{
	m_xmlDoc.LoadFile(szXmlFile);
	if (m_xmlDoc.Error()) return FALSE;
	
	return TRUE;
}

BOOL KXmlReader::Parse(IN LPCSTR szXml)
{	
	m_xmlDoc.Parse(szXml);
	if (m_xmlDoc.Error()) return FALSE;
		
	return TRUE;
}

BOOL KXmlReader::Open(IN LPCSTR szPath)
{	
	if (!szPath)
	{
		m_pOpenElement = m_xmlDoc.RootElement();
	}
	else
	{
		m_pOpenElement = _Open(szPath);
	}

	return m_pOpenElement != NULL;
}

TiXmlElement* KXmlReader::_Open(IN LPCSTR szPath)
{
	KPathParserA path;
	BOOL bRetCode = FALSE;
	TiXmlElement* pTempElement = NULL;
	size_t i = 0, max_size = 0;
	
	bRetCode = path.Parse(szPath);
	if (!bRetCode) goto Exit0;

	
	max_size = path.Size();
	pTempElement = m_xmlDoc.RootElement();
	
	for (i = 0; i < max_size; i++)
	{
		pTempElement = _FindSiblingElement(pTempElement, path.Get(i));
		if (!pTempElement) break;
		if (i != max_size - 1)
		{
			pTempElement = pTempElement->FirstChildElement();
		}
	}

Exit0:
	return pTempElement;
}

TiXmlElement* KXmlReader::_FindSiblingElement(TiXmlElement* pElement, LPCSTR szFolder)
{	
	while (pElement)
	{		
		if (lstrcmpA(pElement->Value(), szFolder) == 0)
			break;

		pElement = pElement->NextSiblingElement();
	}
	return pElement;
}

BOOL KXmlReader::Read(IN LPCSTR szKey, OUT bool& bValue)
{
	int nValue	=	0;
	if (Read(szKey, nValue))
	{
		if (nValue)
			bValue = true;
		else
			bValue = false;

		return TRUE;

	}
	else
		return FALSE;
}

BOOL KXmlReader::Read(LPCSTR szKey, int& nValue)
{
	if (!m_pOpenElement) return FALSE;

	int nRetCode = m_pOpenElement->QueryIntAttribute(szKey, &nValue);
	if (nRetCode != TIXML_SUCCESS) return FALSE;

	return TRUE;
}

#if _MSC_VER > 1200 // vc6.0
void KXmlReader::Read(LPCSTR szKey, CStringA& strValue)
{
	strValue.Empty();
	
	if (m_pOpenElement)
		strValue = m_pOpenElement->Attribute(szKey);
}

void KXmlReader::ReadText(OUT CStringA& strText)
{
	TiXmlNode* pFirstChild = NULL;
	
	strText.Empty();
	if (m_pOpenElement)
	{
		strText = m_pOpenElement->GetText();
	}
}
#endif


void KXmlReader::Read(LPCSTR szKey, CStringW& strValue)
{
	strValue.Empty();

	if (m_pOpenElement) 
		strValue = UTF8ToTChar(m_pOpenElement->Attribute(szKey));
}



void KXmlReader::ReadText(OUT CStringW& strText)
{
	TiXmlNode* pFirstChild = NULL;

	strText.Empty();
	if (m_pOpenElement)
	{
		strText = UTF8ToTChar(m_pOpenElement->GetText());
	}
}

BOOL KXmlReader::NextSibling()
{
	if ( m_pOpenElement )
		m_pOpenElement = m_pOpenElement->NextSiblingElement();
	return (m_pOpenElement != NULL);
}

BOOL KXmlReader::ReadOneElement()
{

	if (!m_pOpenElement) return FALSE;

	TiXmlElement* pItemElement = NULL;

	if (m_bFirstElement)
	{
		pItemElement = m_pOpenElement->FirstChildElement();
		if(pItemElement == NULL)
			return FALSE;

		m_bFirstElement = FALSE;
	}
	else
	{
		pItemElement = m_pOpenElement->NextSiblingElement();
		if(pItemElement == NULL)
			return FALSE;
	}

	m_pOpenElement = pItemElement;

	return TRUE;
}
