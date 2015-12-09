#include "StdAfx.h"
#include "Engine.h"
#include "guiframework.h"

//#include "xmlite.h"
//#include "xmlite.cpp"
#include "pub/fgexml.h"
#include "comutil.h"
#include "Psapi.h"
#include "assert.h"

// ******************************************************************************************
// ֧�ź������֣�����һ�����öΣ�
// ==========================================================================================
using namespace xml;

IGuiObject *ParseXML(LPCSTR szXmlFramework, CGuiFramework *pGuiFramework);
IGuiObject *ResolutionElement(Element &Node, CGuiFramework *pGuiFramework);

void SetAttribute(Element &Node, IGuiObject *pGuiObject) {
    assert( pGuiObject );
    for ( Element::attrib_iterator it = Node.attrib.begin(); it != Node.attrib.end(); it++ )
        pGuiObject->SetParameter( it->GetName().c_str(), (LPARAM)(it->GetValue().c_str()) );
}

void SetSubElement(Element &Node, IGuiObject *pGuiParent, CGuiFramework *pGuiFramework) {
    assert( pGuiParent && pGuiFramework );
    IGuiObject *pGuiObject = NULL;
    for ( Element::element_iterator it = Node.subElement.begin(); it != Node.subElement.end(); it++ ) {
        if ( pGuiObject = ResolutionElement(**it, pGuiFramework) )
            pGuiParent->AddChild(pGuiObject);
    }
}

IGuiObject *ResolutionElement(Element &Node, CGuiFramework *pGuiFramework)
{
    if (pGuiFramework == NULL)
        return NULL;

    // ���ȴ����Ԫ�ص����ͣ�MainFrame��Include��Window��Button��ListBox������
    std::string &NordName = Node.name;

    if ( NordName == "Include" )    // ���XML�ļ����ݹ����ParseXML
        return ParseXML(Node.GetValue(), pGuiFramework);

    // ����Ԫ�����ʹ����ڵ�
    IGuiObject *pGuiObject = pGuiFramework->GeneratGuiObject(NordName.c_str());
    if (pGuiObject == NULL)
        return NULL;

    // ��¼Ԫ������
    strncpy( pGuiObject->m_GuiType, NordName.c_str(), sizeof( pGuiObject->m_GuiType ) );
    pGuiObject->m_GuiType[ sizeof(pGuiObject->m_GuiType)-1 ] = 0;

    // ����Ԫ���������ýڵ�
    SetAttribute(Node, pGuiObject);

    // �����ӽڵ�
    SetSubElement(Node, pGuiObject, pGuiFramework);

    // ��ʼ���ýڵ�
    if (!pGuiObject->OnCreate((LPARAM)Node.GetValue()))
    {
        delete pGuiObject;
        return NULL;
    }

    return pGuiObject;
}

IGuiObject *ParseXML(LPCSTR szXmlData, DWORD size, CGuiFramework *pGuiFramework) 
{ 
    CXML xml;
    try {
        xml.Parse( szXmlData );
    } catch ( std::exception &e ) {
        MessageBoxW( 0, _bstr_t( e.what() ), 0, 0 );
        return NULL;
    }
    return ResolutionElement(*xml.GetRootElement(), pGuiFramework);
}

IGuiObject *ParseXML(LPCSTR szXmlFramework, CGuiFramework *pGuiFramework) 
{ 
    if (szXmlFramework == NULL || pGuiFramework == NULL)
        return NULL;

	PakFile pf;
	if (!pf.Open((LPSTR)szXmlFramework))
		return NULL;

	DWORD size = pf.Size();
	char *buffer = new char [size+1];
	pf.Read(buffer, size);
    buffer[size] = 0;
    IGuiObject *ret = ParseXML( buffer, size, pGuiFramework );
    delete []buffer;
    return ret;
}

// ******************************************************************************************

CGuiFramework::CGuiFramework(void)
{
}

CGuiFramework::~CGuiFramework(void)
{
}

IGuiObject *CGuiFramework::GeneratGuiObject(LPCSTR szRegisteredName)
{
    return NULL;
}

IGuiObject *CGuiFramework::GuiInit(LPCSTR szXmlFramework)
{
    // ����Xml����ļ�����
    return ParseXML(szXmlFramework, this);
}

void GuiObjConverter::TypeNameAssert( LPCSTR type )
{
    std::string sp( type );
    if ( sp == "class CDFace" )
        return;
    int i = (int)sp.find( ptr->m_GuiType );
    if ( i == std::string::npos )
	{
		MessageBox( 0, type, ptr->m_GuiType, 0 );
		//char str[256];
		//sprintf( str, "%s <=> %s\r\n", type, ptr->m_GuiType );
		//OutputDebugString( str );
	}
}
