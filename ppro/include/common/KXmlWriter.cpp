#include "stdafx.h"
#include <atlstr.h>
#include "KXmlWriter.h"
#include "KPathParser.h"
#include "kutfconv.h"

KXmlWriter::KXmlWriter()
{
	m_pOpenElement = NULL;
}

BOOL KXmlWriter::LoadFile(IN LPCSTR szXmlFile)
{
	m_xmlDoc.LoadFile(szXmlFile);
	if (m_xmlDoc.Error()) return FALSE;

	return TRUE;
}

BOOL KXmlWriter::WriteFile(IN LPCSTR szXmlFile)
{
	return m_xmlDoc.SaveFile(szXmlFile);
}

BOOL KXmlWriter::Parse(IN LPCSTR szXml)
{
	m_xmlDoc.Parse(szXml);
	if (m_xmlDoc.Error()) return FALSE;

	return TRUE;
}

TiXmlElement* KXmlWriter::_Open(IN LPCSTR szPath)
{
	KPathParserA path;
	BOOL bRetCode = FALSE;
	TiXmlDocument* pDocument = NULL;
	TiXmlElement* pParentElement = NULL;
	TiXmlElement* pTempElement = NULL;

	path.Parse(szPath);
	if (!path.Size()) goto Exit0;

	pDocument = m_xmlDoc.GetDocument();
	pParentElement = m_xmlDoc.RootElement();
	if (!pParentElement)
	{
		TiXmlDeclaration* pDeclar = new TiXmlDeclaration("1.0", "UTF-8", "no");
		pDocument->LinkEndChild(pDeclar);
		pTempElement = _Create(pDocument, szPath);
		goto Exit0;
	}

	pTempElement = _FindSiblingElement(pParentElement, path.Get(0));
	if (!pTempElement)
	{
		pTempElement = _Create(pDocument, szPath);
		goto Exit0;
	}

	pParentElement = pTempElement;
	pTempElement = pParentElement->FirstChildElement();
	for (size_t i = 1; i < path.Size(); i++)
	{
		pTempElement = _FindSiblingElement(pTempElement, path.Get(i));
		if (!pTempElement)
		{
			CStringA strPath;
			for (size_t j = i; j < path.Size(); j++)
			{
				strPath += path.Get(j);
				strPath += "\\";
			}
			pTempElement = _Create(pParentElement, strPath);
			goto Exit0;
		}
		if (i != path.Size() - 1)
		{
			pParentElement = pTempElement;
			pTempElement = pParentElement->FirstChildElement();
		}
	}

Exit0:
	return pTempElement;
}

TiXmlElement* KXmlWriter::_FindSiblingElement(TiXmlElement* pElement, LPCSTR szFolder)
{	
	while (pElement)
	{		
		if (lstrcmpA(pElement->Value(), szFolder) == 0)
			break;

		pElement = pElement->NextSiblingElement();
	}
	return pElement;
}

TiXmlElement* KXmlWriter::_Create(TiXmlNode* pParentNode, LPCSTR szPath)
{
	KPathParserA path;
	TiXmlElement* pRetElement = NULL;

	path.Parse(szPath);
	if (!path.Size()) goto Exit0;

	for (size_t i = 0; i < path.Size(); i++)
	{
		pRetElement = new TiXmlElement(path.Get(i));
		pParentNode->LinkEndChild(pRetElement);
		pParentNode = pRetElement;
	}

Exit0:
	return pRetElement;
}

BOOL KXmlWriter::Open(IN LPCSTR szPath)
{
	if (!szPath)
	{
		m_pOpenElement = m_xmlDoc.RootElement();
	}
	else
	{
		m_pOpenElement = _Open(szPath);
	}

	if(m_pOpenElement)
		m_OpenParentElement = m_pOpenElement;

	return m_pOpenElement != NULL;
}

BOOL KXmlWriter::Write(IN LPCSTR szKey, IN int nValue)
{
	if (!m_pOpenElement) return FALSE;

	m_pOpenElement->SetAttribute(szKey, nValue);

	return TRUE;
}

BOOL KXmlWriter::Write(IN LPCSTR szKey, IN LPCSTR szValue)
{
	if (!m_pOpenElement || !szValue) return FALSE;

	m_pOpenElement->SetAttribute(szKey, szValue);

	return TRUE;
}

BOOL KXmlWriter::Write(IN LPCSTR szKey, IN LPCWSTR szValue)
{
	if (!m_pOpenElement || !szValue) return FALSE;

	m_pOpenElement->SetAttribute(szKey, TCharToUTF8(szValue));

	return TRUE;
}

BOOL KXmlWriter::WriteText(IN LPCSTR szText, BOOL bCDATA)
{
	if (!m_pOpenElement || !szText) return FALSE;

	TiXmlText *pXmlText = new TiXmlText(szText);
	pXmlText->SetCDATA(bCDATA);
	m_pOpenElement->LinkEndChild(pXmlText);

	return TRUE;
}

BOOL KXmlWriter::WriteText(IN LPCWSTR szText, BOOL bCDATA)
{
	if (!m_pOpenElement || !szText) return FALSE;

	CStringA strText = TCharToUTF8(szText);
	TiXmlText *pXmlText = new TiXmlText((LPCSTR)strText);
	pXmlText->SetCDATA(bCDATA);
	m_pOpenElement->LinkEndChild(pXmlText);

	return TRUE;
}

LPCSTR KXmlWriter::ToXml(void)
{
	m_xmlDoc.Accept(&m_xmlPrinter);
	return m_xmlPrinter.CStr();
}

BOOL KXmlWriter::AddNewElement(IN LPCSTR szKey)
{

	if (!m_OpenParentElement || !szKey) return FALSE;

	TiXmlElement* pItemElement = new TiXmlElement(szKey);
	m_OpenParentElement->LinkEndChild(pItemElement);

	m_pOpenElement = pItemElement;

	return m_pOpenElement != NULL;
}
