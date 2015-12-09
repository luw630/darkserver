//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	Sprite.h
// Date:	2000.09.18
// Code:	Dreamwork,Wooy
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef Sprite_H
#define Sprite_H
//---------------------------------------------------------------------------
#include "MemClass.h"
#include "Palette.h"
//---------------------------------------------------------------------------
typedef struct
{
	BYTE	Comment[4];	// ע������(SPR\0)
	WORD	Width;		// ͼƬ���
	WORD	Height;		// ͼƬ�߶�
	WORD	CenterX;	// ���ĵ�ˮƽλ��
	WORD	CenterY;	// ���ĵĴ�ֱλ��
	WORD	Frames;		// ��֡��
	WORD	Colors;		// ��ɫ��
	WORD	Directions;	// ������
	WORD	Interval;	// ÿ֡���������Ϸ֡Ϊ��λ��
	WORD	Reserved[6];// �����ֶΣ����Ժ�ʹ�ã�
} SPRHEAD;
//---------------------------------------------------------------------------
typedef struct
{
	DWORD	Offset;		// ÿһ֡��ƫ��
	DWORD	Length;		// ÿһ֡�ĳ���
} SPROFFS;
//---------------------------------------------------------------------------
typedef struct
{
	WORD	Width;		// ֡��С���
	WORD	Height;		// ֡��С�߶�
	WORD	OffsetX;	// ˮƽλ�ƣ������ԭͼ���Ͻǣ�
	WORD	OffsetY;	// ��ֱλ�ƣ������ԭͼ���Ͻǣ�
	BYTE	Sprite[1];	// RLEѹ��ͼ������
} SPRFRAME;
//---------------------------------------------------------------------------
class XENGINE_API Sprite
{
protected:
	MemClass 	m_Buffer;
	MemClass 	m_Palette;
	PAL24*		m_pPal24;
	PAL16*		m_pPal16;
	SPROFFS* 	m_pOffset;
	PBYTE		m_pSprite;
	int			m_nWidth;
	int			m_nHeight;
	int			m_nCenterX;
	int			m_nCenterY;
	int			m_nFrames;
	int			m_nColors;
	int			m_nDirections;
	int			m_nInterval;
	int			m_nColorStyle;

public:
	int			m_nLum;

public:
	Sprite();
	~Sprite();
	BOOL		Load(LPSTR FileName);
	BOOL		LoadFor3D(LPSTR FileName);
	void		Free();
	void		MakePalette();
	void		Make4444Palette();
	void		MakePaletteLum32();
	void		SetColorStyle(int nStyle, int nLum);
	void		SetStyle(int nStyle);
	void		Draw(int nX, int nY, int nFrame);
	void        DrawCenter(int nX, int nY, int nFrame); 
	void		DrawMixColor(int nX, int nY, int nFrame,int nColor,int nAlpha);
	void		DrawWithColor(int nX, int nY, int nFrame,int nColor,int nAlpha);
	void		DrawAlpha(int nX, int nY, int nFrame, int nExAlpha = 32);
    void		DrawAlphaCenter(int nX, int nY,int nFrame, int nExAlpha = 32);
	void		DrawTrans(int nX, int nY, int nFrame);
	void		DrawBorder(int nX, int nY, int nFrame, int nColor);
	int			NextFrame(int nFrame);
	int			GetWidth(){ return m_nWidth; };
	int			GetHeight(){ return m_nHeight; };
	int			GetCenterX(){ return m_nCenterX; };
	int			GetCenterY(){ return m_nCenterY; };
	void		SetCenterX(int X){ m_nCenterX = X; };
	void		SetCenterY( int Y){ m_nCenterY= Y; };
	int			GetFrames(){ return m_nFrames; };
	int			GetColors(){ return m_nColors; };
	PVOID		GetFrame(int nFrame);
	int			GetDirections(){ return m_nDirections; };
	int			GetInterval(){ return m_nInterval; };
	int			GetPixelAlpha(int nFrame, int x, int y);
	PVOID		GetPalette(){return m_Palette.GetMemPtr();}	;
	PVOID		Get24Palette(){return this->m_pPal24;};
	BOOL        GetImageInfo(void *pImageBuf, void *pAlphaBuf, DWORD cbSize, DWORD nFrame, bool useClear = true);
        BOOL        DrawAlphaInfo(int nX,int nY,int nFrame,int nIdCode,void *pImage);
	void        DrawAlphaInfoCenter(int nX, int nY, int nFrame,int nIdCode,void *pImage);
	void        DrawZoom( RECT *rect, int frame, int color );
	void        DrawProgress( int x, int y, int len, int frame, int color );
};
//---------------------------------------------------------------------------
#endif
