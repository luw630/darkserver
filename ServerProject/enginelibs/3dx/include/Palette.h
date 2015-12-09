//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	Palette.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef Palette_H
#define Palette_H
//---------------------------------------------------------------------------
typedef struct {
	BYTE		Blue;		// ��ɫ����
	BYTE		Green;		// ��ɫ����
	BYTE		Red;		// ��ɫ����
	BYTE		Alpha;		// ͸������
} PAL32;					// 32λɫ��
//---------------------------------------------------------------------------
typedef struct {
	BYTE		Red;		// ��ɫ����
	BYTE		Green;		// ��ɫ����
	BYTE		Blue;		// ��ɫ����
} PAL24;					// 24λɫ��
//---------------------------------------------------------------------------
typedef WORD	PAL16;		// 16λɫ��
//---------------------------------------------------------------------------
void ENGINE_API	g_Pal32ToPal16(PAL32* pPal32, PAL16* pPal16, int nColors);
void ENGINE_API	g_Pal32ToPal24(PAL32* pPal32, PAL24* pPal24, int nColors);
void ENGINE_API	g_Pal32ToPal16Style(PAL32* pPal32, PAL16* pPal16, int nColors);
void ENGINE_API	g_Pal32ToPal16Lum(PAL32* pPal32, PAL16* pPal16, int nColors);
void ENGINE_API	g_Pal32ToPal16Gray(PAL32* pPal32, PAL16* pPal16, int nColors);
//---------------------------------------------------------------------------
void XENGINE_API g_Pal24ToPal16(PAL24* pPal24, PAL16* pPal16, int nColors);
void ENGINE_API	g_Pal24ToPal32(PAL24* pPal24, PAL32* pPal32, int nColors);
void ENGINE_API	g_Pal24ToPal16Style(PAL24* pPal24, PAL16* pPal16, int nColors);
void ENGINE_API	g_Pal24ToPal16Lum(PAL24* pPal24, PAL16* pPal16, int nColors);
void ENGINE_API	g_Pal24ToPal16Red(PAL24* pPal24, PAL16* pPal16, int nColors);
//---------------------------------------------------------------------------
void ENGINE_API	g_Pal16ToPal24(PAL16* pPal16, PAL24* pPal24, int nColors);
void ENGINE_API	g_Pal16ToPal32(PAL16* pPal16, PAL32* pPal32, int nColors);
//---------------------------------------------------------------------------
void ENGINE_API	g_SetColorStyle(BYTE Red, BYTE Green, BYTE Blue);
//---------------------------------------------------------------------------
#endif
