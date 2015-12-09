#include "stdafx.h"
#include "MyClass.h"


CMyClass::CMyClass() :m_nsid(0)
{
}


CMyClass::~CMyClass()
{
}

void CMyClass::SetSid(int nid)
{
	m_nsid = nid;
}

void CMyClass::SetName(char *strname)
{
	m_strname = strname;
}

const int CMyClass::GetSid()
{
	return m_nsid;
}

const char* CMyClass::GetName()
{
	return m_strname.c_str();
}
