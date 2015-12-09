//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	Bitmap.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef Bitmap_H
#define Bitmap_H
//---------------------------------------------------------------------------
#include "MemClass.h"
#include "Palette.h"
//---------------------------------------------------------------------------
typedef struct {
	char	Id[4];
	LONG	Width;
	LONG	Height;
	LONG	Colors;
} BMPHEADER;
//---------------------------------------------------------------------------
class XENGINE_API Bitmap
{
private:
	MemClass	m_Bitmap;
	PAL32		m_Pal32[256];
	PAL16		m_Pal16[256];
	int			m_nWidth;
	int			m_nHeight;
	int			m_nColors;

public:
	int			m_nColorKey;

public:
	Bitmap();
	BOOL		Init( int nWidth, int nHeight, int nColors );
	BOOL		Load(LPSTR lpFileName);
	BOOL		Save(LPSTR lpFileName);
	void		Draw( int nX, int nY );
	void		Clear(BYTE byColor);
	void		MakePalette( int colors = -1 );
	void		PutPixel(int nX, int nY, BYTE byColor);
	BYTE		GetPixel(int nX, int nY);

public:
	void*		GetBitmap()	{ return m_Bitmap.GetMemPtr(); };
	PAL32*		GetPal32()	{ return m_Pal32; };
	PAL16*		GetPal16()	{ return m_Pal16; };
	int			GetWidth()	{ return m_nWidth; };
	int			GetHeight()	{ return m_nHeight; };
	int			GetColors()	{ return m_nColors; };
};
//---------------------------------------------------------------------------
#endif
