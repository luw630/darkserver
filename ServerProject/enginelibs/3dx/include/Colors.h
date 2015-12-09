//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	Colors.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef Colors_H
#define Colors_H
//---------------------------------------------------------------------------
extern ENGINE_API	BYTE	g_Red(WORD wColor);
extern ENGINE_API	BYTE	g_Green(WORD wColor);
extern ENGINE_API	BYTE	g_Blue(WORD wColor);
extern ENGINE_API	WORD	g_RGB555(int nRed, int nGreen, int nBlue);
extern ENGINE_API	WORD	g_RGB565(int nRed, int nGreen, int nBlue);
extern ENGINE_API	void	g_555To565(int nWidth, int nHeight, void* lpBitmap);
extern ENGINE_API	void	g_565To555(int nWidth, int nHeight, void* lpBitmap);
extern XENGINE_API	WORD	(*g_RGB)(int nRed, int nGreen, int nBlue);
//---------------------------------------------------------------------------
#endif
