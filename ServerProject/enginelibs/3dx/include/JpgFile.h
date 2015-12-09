//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	JpgFile.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef JpgFile_H
#define JpgFile_H
//---------------------------------------------------------------------------
#include "MemClass.h"
#include "Bitmap16.h"
//---------------------------------------------------------------------------
class XENGINE_API JpgFile
{
private:
	MemClass	m_Buffer;
public:
	BOOL		Load(LPSTR lpFileName, Bitmap16* lpBitmap);
	BOOL		LoadToBitmap( Bitmap16 &bitmap, LPVOID buffer, size_t size );
};
//---------------------------------------------------------------------------
#endif