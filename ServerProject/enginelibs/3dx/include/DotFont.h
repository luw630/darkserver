//---------------------------------------------------------------------------
// Engine (c) 1999-2003 by Dreamwork
// File:	DotFont.h
// Date:	2003.06.25
// Code:	qiuxin
// Desc:	绘制点阵字库GB2132字符集
//---------------------------------------------------------------------------
#ifndef DotFont_H
#define DotFont_H
//---------------------------------------------------------------------------
#include "MemClass.h"
#include "Bitmap16.h"
#include <vector>
#include <utility>
#include <algorithm>

using std::vector;
using std::pair;
using std::make_pair;
using std::reverse;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef struct _DotFontHead {
	char		Id[4];				// 标识'FONT'
	int		    Type;				// 0:gb2312 1:gbk
	DWORD		Count;				// 数量
	DWORD		Width;				// 宽度
} TDotFontHead;

typedef struct _HotPoint
{
	RECT	rcRect;
    int		nIndex;
} THotPoint;


#define	DEFAULTCOLOR g_RGB(0xc0,0xc0,0xc0);//银白

#define DOTFONTMASK   0x00
#define DOTFONTSOLID  0x01
#define CHARASET_GB2312        0
#define CHARASET_GBK           1

//---------------------------------------------------------------------------
class XENGINE_API DotFont
{
protected:
	MemClass	m_DotFont;			// font data
    Bitmap16    m_Bitmap16;         // font image buffer
	DWORD		m_dwSize;			// size of font data
	int			m_nCount;			// count of word
	int			m_nFontW;			// width of font
	int			m_nFontH;			// height of font
	int			m_nLineW;			// width of line
	int			m_nLineH;			// height of line
	DWORD		m_dwColor;			// color of text
	DWORD		m_dwMaskColor;		// MaskColor of text
    int         m_TextStyle;        // TextStyle
	int         m_CodeType;         // CodeType

    void	    **masksTable;	    // pre-computed masks of font, ATTENTION! this is not a palette

protected:
	void	DrawFont(int nCode, int nX, int nY);
	void	GBKDrawFont(int nCode, int nX, int nY);
public:
	DotFont();
	~DotFont();
	void	Close(void);
	BOOL	Load(LPSTR FileName);
	DWORD	DrawText( int x, int y, LPCSTR str );
    // 最新的逻辑，支持文字的半透明绘制和颜色控制以及特殊绘制对象插入
    // %索引式颜色控制  主要是为了兼容老版本的逻辑，字符规则无变化
    // #编码式颜色控制  主要为了能够更加方便的进行颜色调制，编码规则 #<<RRGGBB[AA]>[<RRGGBB[AA]>]>
    // $特效颜色控制    实现颜色渐变 $<1<RRGGBB[AA]><RRGGBB[AA]>>
    // @字体切换        这个就不用说明了，编码规则 @<宋体>
    // &对象插入        这个功能主要是为了支持扩展文字绘制逻辑，比如增加表情，增加带下划线并且可做超链接的文字，编码规则 &<任意信息>
    // 对象插入回调函数 DWORD ( ole* )( int x, int y, int w, int h, LPCSTR ole, LPARAM arg )
    // x,y      【输入】当前绘制的目标坐标（左上角）
    // w,h      【输出】当前控制对象处理后，所能绘制出来的宽高
    // ole      【输入】&<>格式串中的信息，从<符号后进行输入
    // arg      【输入】用于回调函数处理信息字符串时的扩展配置信息。。。
    // 返回值   【输出】指明函数内已经处理了多少个字符，外部需要跳过多少个字符。。。
	//void	DrawTextEx( int x, int y, LPCSTR str, DWORD ( ole* )( int x, int y, int &w, int &h, LPCSTR ole, LPARAM arg ), LPARAM arg ); 
	DWORD	DrawColorText( int x, int y, LPCSTR str, THotPoint* pHotPoints=0, int nMaxRects=0, int* pRectNum =0);
	void	DrawSafeText(int nX, int nY, LPSTR lpText, unsigned int len);
	void	SetColor(BYTE Red, BYTE Green, BYTE Blue);
	void	SetMaskColor(BYTE Red, BYTE Green, BYTE Blue);
	void	SetLine(int nWidth, int nHeight = -1);
	int     GetColorStringLen(LPSTR lpText);
	int		GetWidth(){ return m_nFontW; };
	int		GetHeight(){ return m_nFontH; };
    void	SetTextStyle(int nTextStyle) { m_TextStyle=nTextStyle; };	
};
//---------------------------------------------------------------------------
#endif

