//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	Canvas.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef Canvas_H
#define Canvas_H
//---------------------------------------------------------------------------
#include "MemClass.h"
#include "Node.h"
//---------------------------------------------------------------------------
//typedef void (* DRAW_FUNC)(void* pNode, void* pCanvas);
//---------------------------------------------------------------------------
class DrawNode : public Node
{
public:
//	DRAW_FUNC	m_pDrawFunc;
	BOOL		m_bChanged;
	long		m_nX;
	long		m_nY;
	long		m_nWidth;
	long		m_nHeight;
	long		m_nColor;
	long		m_nAlpha;
	void*		m_pBitmap;
	void*		m_pPalette;
};
//---------------------------------------------------------------------------
typedef struct {
	long		x;			// 裁减后的X坐标
	long		y;			// 裁减后的Y坐标
	long		width;		// 裁减后的宽度
	long		height;		// 裁减后的高度
	long		left;		// 上边界裁剪量
	long		top;		// 左边界裁剪量
	long		right;		// 右边界裁剪量
} Clipper;
/*/---------------------------------------------------------------------------
typedef struct {
	long		num;		// 当前的对象数
	long		changed;	// 重绘标志
	DrawNode*	node[10];	// 绘制对象指针
} KSubCanvas;*/
//---------------------------------------------------------------------------
class XENGINE_API Canvas
{
private:
//	List		m_DrawList;// 绘制列表
	DrawNode*	pNode;	
	MemClass	m_Canvas;// 画布内存对象
//	MemClass	m_SubCanvas;//画布子区域
public:
	void*		m_pCanvas;// 画布内存
	long		m_nWidth;// 画布宽度
	long		m_nHeight;// 画布高度
	long		m_nPitch;// 画布的行跨距
	long		m_nMask16;// 16bit RGB Mask
	long		m_nMask32;// 32bit RGB Mask
	long		m_nScreenWidth;// 屏幕宽度
	long		m_nScreenHeight;// 屏幕高度
	long		m_nScreenPitch;// 屏幕跨度
//	long		m_nSubRows;// 行数目
//	long		m_nSubCols;// 列数目
	RECT		m_ClipRect;// 裁减区域
	BOOL		m_bChanged;// 改变标志
	BOOL		m_bDisabled;// 改变标志

private:
//	void		SubAddNode(int nX,int nY,int nW,int nH,DrawNode* pNode);
//	void		SubAdd(int nCol, int nRow, DrawNode* pNode);
//	void		SubChangedNode(int nX,int nY,int nW,int nH,DrawNode* pNode);
//	void		SubChanged(int nCol, int nRow);
//	void		SubMarkChanged();
	void		UpdateBackBuffer(LPRECT lpRect);
	void		UpdateFrontBuffer(LPRECT lpRect);
public:
	Canvas();
	~Canvas();
	void		Init(int nWidth, int nHeight);
	void		SetActiveCanvas(void);

	void		Changed(BOOL bChanged){m_bChanged = bChanged;};
	void		DisableDraw(BOOL bDisabled){m_bDisabled = bDisabled;};
	BOOL		isDisabled(){return m_bDisabled;};
	void		FillCanvas(WORD wColor);
	void		UpdateCanvas();
	void		UpdateScreen();
	void		UpdateScreenWindowedZoom();
//	void		UpdateScreen(LPRECT lpRect);
	BOOL		RectClip(LPRECT pRect);
	BOOL		MakeClip(long nX, long nY, long nWidth, long nHeight, Clipper* pClipper);
	void		SetClipRect(LPRECT pRect);
	void		GetClipRect(LPRECT pRect);
	void		DrawPixel(int nX, int nY, int nColor);
	void		DrawPixelAlpha(int nX, int nY, int nColor, int nAlpha);
	void		DrawLine(int nX1, int nY1, int nX2, int nY2, int nColor);
	void		DrawLineAlpha(int nX1, int nY1, int nX2, int nY2, int nColor, int nAlpha);
	void		DrawSprite(int nX, int nY, int nWidth, int nHeight, void* lpSprite, void* lpPalette);
    void		DrawSpriteMixColor(int nX, int nY, int nWidth, int nHeight, void* lpSprite, void* lpPalette,int nColor,int nAlpha);
    void		DrawSpriteWithColor(int nX, int nY, int nWidth, int nHeight, void* lpSprite, void* lpPalette,int nColor,int nAlpha);
	void		DrawSpriteAlpha(int nX, int nY, int nWidth, int nHeight, void* lpSprite, void* lpPalette, int nExAlpha);
	void		DrawSpriteBorder(int nX, int nY, int nWidth, int nHeight, int nColor, void* lpSprite);
	void		DrawSprite3LevelAlpha(int nX, int nY, int nWidth, int nHeight, void* lpSprite, void* lpPalette);
	void		DrawFont(int nX, int nY, int nWidth, int nHeight, int nColor, int nAlpha,void* lpFont);
	void		DrawFontSolid(int nX, int nY, int nWidth, int nHeight, int nColor, int nAlpha,void* lpFont);
	void		DrawBitmap(int nX, int nY, int nWidth, int nHeight, void* lpBitmap, void* lpPalette);
	void		DrawBitmapMask(int nX, int nY, int nWidth, int nHeight, void* lpBitmap, void* lpPalette, int nMaskColor);
	void		DrawBitmap16(int nX, int nY, int nWidth, int nHeight, void* lpBitmap);
	void		DrawBitmap16Mask(int nX, int nY, int nWidth, int nHeight, void* lpBitmap,int nMaskColor);
	void		DrawBitmap16Lum(int nX, int nY, int nWidth, int nHeight, void* lpBitmap,int nAlpha);
	void		DrawBitmap16mmx(int nX, int nY, int nWidth, int nHeight, void* lpBitmap);
	void		DrawBitmap16win(int nX, int nY, int nWidth, int nHeight, void* lpBitmap);
	void		DrawFade(int nX, int nY, int nWidth, int nHeight, int nAlpha);
    void        Clear(int nX, int nY, int nWidth, int nHeight, int nColor);
    void        DotClear(int nX, int nY, int nWidth, int nHeight, int nColor);
    void        ClearAlpha(int nX, int nY, int nWidth, int nHeight, int nColor,int nAlpha);
    bool        GetImage(int nX, int nY, int nWidth, int nHeight, void *lpBitmap);
    void        DrawBox(int x, int y, int w, int h, WORD edgARGB, WORD nttARGB = 0);
};
extern XENGINE_API Canvas* g_pCanvas;
//---------------------------------------------------------------------------
#endif
