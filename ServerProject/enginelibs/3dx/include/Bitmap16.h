//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	Bitmap16.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef Bitmap16_H
#define Bitmap16_H
//---------------------------------------------------------------------------
#include "MemClass.h"
//---------------------------------------------------------------------------
typedef struct {
	char		Id[4];	// "BM16"
	int			Width;
	int			Height;
	int			RGBMask;
} BMPHEADER16;
//---------------------------------------------------------------------------
class XENGINE_API Bitmap16
{
protected:
	MemClass	m_Bitmap;
	int			m_nWidth;
	int			m_nHeight;
public:
	Bitmap16();
	~Bitmap16();
	BOOL		Init(int nWidth, int nHeight);
	void		Free();
	BOOL		Load(LPSTR lpFileName);
	BOOL		Save(LPSTR lpFileName);
	void		Draw(int nX, int nY);
	void		DrawMask(int nX, int nY,int nMaskColor);
	void		DrawLum(int nX, int nY, int nLum);
	void		Clear();
	void		PutPixel(int nX, int nY, WORD wColor);
	WORD		GetPixel(int nX, int nY);
	LPWORD		GetPointer(int nX, int nY);

public:
	void*		GetBitmap()	{ return m_Bitmap.GetMemPtr(); };
	int			GetWidth()	{ return m_nWidth; };
	int			GetHeight()	{ return m_nHeight; };

    void        ReadOnly()  { m_Bitmap.ReadOnly(); }
    void        ConvertToTex();
};
//---------------------------------------------------------------------------
#endif
