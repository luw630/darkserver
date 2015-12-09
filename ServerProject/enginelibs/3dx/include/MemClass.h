//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	MemClass.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef MemClass_H
#define MemClass_H
//---------------------------------------------------------------------------
class XENGINE_API MemClass
{
private:
	PVOID		m_lpMemPtr;
	DWORD		m_lpMemLen;
public:
	MemClass();
	~MemClass();
	PVOID		Alloc(DWORD dwSize);
	void		Free();
	void		Zero();
	LPVOID		ReadOnly();
	void		Fill(BYTE byFill);
	void		Fill(WORD wFill);
	void		Fill(DWORD dwFill);
	PVOID		GetMemPtr() { return m_lpMemPtr; };
	DWORD		GetMemLen() { return m_lpMemLen & 0x7fffffff; };
};
//---------------------------------------------------------------------------
#endif
