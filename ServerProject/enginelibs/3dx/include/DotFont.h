//---------------------------------------------------------------------------
// Engine (c) 1999-2003 by Dreamwork
// File:	DotFont.h
// Date:	2003.06.25
// Code:	qiuxin
// Desc:	���Ƶ����ֿ�GB2132�ַ���
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
	char		Id[4];				// ��ʶ'FONT'
	int		    Type;				// 0:gb2312 1:gbk
	DWORD		Count;				// ����
	DWORD		Width;				// ���
} TDotFontHead;

typedef struct _HotPoint
{
	RECT	rcRect;
    int		nIndex;
} THotPoint;


#define	DEFAULTCOLOR g_RGB(0xc0,0xc0,0xc0);//����

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
    // ���µ��߼���֧�����ֵİ�͸�����ƺ���ɫ�����Լ�������ƶ������
    // %����ʽ��ɫ����  ��Ҫ��Ϊ�˼����ϰ汾���߼����ַ������ޱ仯
    // #����ʽ��ɫ����  ��ҪΪ���ܹ����ӷ���Ľ�����ɫ���ƣ�������� #<<RRGGBB[AA]>[<RRGGBB[AA]>]>
    // $��Ч��ɫ����    ʵ����ɫ���� $<1<RRGGBB[AA]><RRGGBB[AA]>>
    // @�����л�        ����Ͳ���˵���ˣ�������� @<����>
    // &�������        ���������Ҫ��Ϊ��֧����չ���ֻ����߼����������ӱ��飬���Ӵ��»��߲��ҿ��������ӵ����֣�������� &<������Ϣ>
    // �������ص����� DWORD ( ole* )( int x, int y, int w, int h, LPCSTR ole, LPARAM arg )
    // x,y      �����롿��ǰ���Ƶ�Ŀ�����꣨���Ͻǣ�
    // w,h      ���������ǰ���ƶ���������ܻ��Ƴ����Ŀ��
    // ole      �����롿&<>��ʽ���е���Ϣ����<���ź��������
    // arg      �����롿���ڻص�����������Ϣ�ַ���ʱ����չ������Ϣ������
    // ����ֵ   �������ָ���������Ѿ������˶��ٸ��ַ����ⲿ��Ҫ�������ٸ��ַ�������
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

