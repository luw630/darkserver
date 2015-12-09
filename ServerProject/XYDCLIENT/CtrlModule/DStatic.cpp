/****************************************************************
//  FileName:   DStatic.cpp
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG 0926
****************************************************************/

#include "StdAfx.h"
#include "dstatic.h"
#include "colors.h"
#include "DSlideBar.h"
#include "ResManagerModule\DCanvasEx.h"

extern DCanvasEx   *g_Canvas;

//BEGIN_MY_MESSAGE_MAP(CDStatic)
//MESSAGE_MAP(m_pSlideBar, OnPosChanged())
//END_MY_MESSAGE_MAP

int CDStatic::OnMyMessage(LPFace lpFace, LPARAM lParam)
{
	if( lpFace == (LPFace)m_pSlideBar )
	{
		return OnPosChanged();
	}
	return RET_NONE;
}

CDStatic::CDStatic(int fontsize) : CDFace(IID_CDSTATIC, fontsize)
{
    m_dwClr = g_RGB(222, 222, 222);

	m_nFontLineWidth = 400;
	m_nFontLineHeight = 16;
	m_nLineBegin = 0;
	m_bHaveSpecialChar = false;
	m_bHaveSlideBar = false;
	m_bHoldSlideBarPos = false;

    m_pSlideBar = NULL;
	m_textHeiht = 0;
	m_drawpos = 0;
}

CDStatic::~CDStatic(void)
{
	//if( m_pSlideBar != NULL )
	//{
	//	delete m_pSlideBar;
	//	m_pSlideBar = NULL;
	//}
}

bool CDStatic::SetParameter( LPCSTR szParameterName, LPARAM lParam )
{
	/// WndCustomize 是在设置完所有属性后,才调用OnCreate
	if ( stricmp( szParameterName, "Text" ) == 0 )
	{
		if( m_pSlideBar == 0 )
			m_strBuff = ( LPSTR )lParam;
		else
			SetText( ( LPSTR )lParam, false );
	}
	else if ( stricmp( szParameterName, "setsize" ) == 0 )
	{
		m_nWidth = GetParameterDWORD(lParam, 1);
        m_nHeight = GetParameterDWORD(lParam, 2);
	}
	else if(strcmp(szParameterName, "pos") == 0)
    {
        m_nLeft = GetParameterDWORD(lParam, 1);
        m_nTop  = GetParameterDWORD(lParam, 2);
    }
	else
		return CDFace::SetParameter( szParameterName, lParam );
	return true;
}

int CDStatic::InitByBinary(LPARAM lParam)
{
	return 1;
}

int CDStatic::OnCreate(LPARAM lParam)
{
	m_bMouseEnter = false;

	RECT rcWnd = GetRECT();
	if (m_nFontLineWidth > (rcWnd.right - rcWnd.left - 20))
		m_nFontLineWidth =  rcWnd.right - rcWnd.left - 20;

	if( m_pSlideBar == 0 )
	{
		RECT rcSlideBar;
		rcSlideBar.left = m_nWidth - 9;//rcWnd.right - 15;//因为使用滚动条不多，所以这里配合图档
		rcSlideBar.top =  1;
		rcSlideBar.right = m_nWidth;//rcWnd.right;
		rcSlideBar.bottom = rcWnd.bottom - rcWnd.top;
		m_pSlideBar = new CDSlideBar;
		CDSlideBar::SParameter param;
		param.nColorBar = g_RGB(222, 122, 77);
		param.nColorSlider = g_RGB(128, 128, 128);
		m_pSlideBar->Create(this, rcSlideBar, (LPARAM)&param);
	}

	if (m_pSlideBar)
	{
		m_pSlideBar->Show(false);
		m_pSlideBar->SetHorizon(false);

		m_pSlideBar->SetBePicture("", 
			"Interface/BigMap/拖动条1.spr",
			"Interface/BigMap/拖动条2.spr",
			"Interface/BigMap/拖动条3.spr");
	}

	if( m_strBuff.length() > 0 )
	{
		AddTex( m_strBuff );
		m_strBuff = "";
	}
    return 1;
}

int CDStatic::KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y)
{
	if( iKeyState== MS_MOVE )
	{	
		if( !m_bMouseEnter )
			m_MouseEnterEvent.Invoke(this, NULL);
		else
		{
			MouseEventArgs arg(x,y);
			m_MouseMoveEvent.Invoke(this,&arg);
		}
		m_bMouseEnter = true;
	}
    if ( iKeyState == MS_MOVE && m_pSlideBar && m_pSlideBar ->isVisible() )
    {
        RECT rcSlideBar = m_pSlideBar->GetRcBar();
        POINT pt;
        pt.x = x; pt.y = y;
        if ( PtInRect( &rcSlideBar, pt ) )
			m_pSlideBar->m_bShowTip = true;
        else
            m_pSlideBar->m_bShowTip = false;
    }
	return 1;
}

void CDStatic::Update()
{
	RECT rcWnd = GetRECT();
	ClientToScreen(&rcWnd);
    int nWidth = m_Font.GetLineWidth();
    int nHeight = m_Font.GetLineHeight();

	if (m_nFontLineWidth > 12)
		m_Font.SetLine(m_nFontLineWidth, m_nFontLineHeight);
	else
		m_nFontLineWidth = g_pCanvas->m_nWidth;

	RECT rcClipOld, rcClipNew;
	g_Canvas->GetClipRect(&rcClipOld);
	if ((rcWnd.right - rcWnd.left > 1)&&(rcWnd.bottom - rcWnd.top > 1))
	{
		rcClipNew.left = rcWnd.left;
		rcClipNew.right= rcWnd.right;
		rcClipNew.top  = rcWnd.top + 1;
		rcClipNew.bottom = rcWnd.bottom - 1;
		g_Canvas->SetClipRect(&rcClipNew);
	}
	
	if (m_bHoldSlideBarPos) m_bHaveSlideBar = true;
    DWORD oldcr = m_Font.GetColor();
    m_Font.SetColor( m_dwClr );
	int curHeight = 0;
	if( m_strBuff.length() > 0 )
	{
		int oldWidth = m_Font.GetLineWidth();
		int oldHeight = m_Font.GetLineHeight();
		m_Font.SetLine( m_nFontLineWidth, m_nFontLineHeight );
		m_Font.DrawColorText( rcWnd.left + 1/*+ (m_bHaveSlideBar?18:1)*/, 2 -m_drawpos/**m_Font.GetLineHeight()*/ + rcWnd.top , m_strBuff.c_str());//m_cBuff[i]);
		m_Font.SetLine( oldWidth, oldHeight );
		// curHeight += 
	}

	for( int i = 0; i < (int)m_strBufferList.size(); i ++ )
	{ 
		//m_drawpos = m_Font.GetHeight()+2;
		m_Font.DrawColorText( rcWnd.left + 4, curHeight + 3 + i * (m_Font.GetHeight()+2) + rcWnd.top - m_drawpos, m_strBufferList[i].c_str() );
		curHeight += ( ( (int)strlen( m_strBufferList[i].c_str() ) * 12 ) / ( rcWnd.right - rcWnd.left ) ) * (m_Font.GetHeight()+2);
	}
	m_Font.SetColor( oldcr );

	g_Canvas->SetClipRect(&rcClipOld);
	
	//该成后面偏移
    //m_Font.SetLine( nWidth - ( m_bHaveSlideBar ? 18 : 1 ), nHeight);
}

void CDStatic::AddTex( std::string str )
{
	m_drawpos = 0;
	if( m_strBufferList.size() > 50 )
		return;

	m_strBufferList.push_back( str );
	if( GetTexHeight() > m_nHeight/*300*/ )
	{
		m_bHaveSlideBar = true;
		m_pSlideBar->Show(true);
		m_pSlideBar->SetPosition( 0 );
	}
	else
	{
		m_bHaveSlideBar = false;
		m_pSlideBar->Show(false);
	}
}

int CDStatic::GetTexHeight()
{
	m_textHeiht = 0;
	for( int i = 0; i < (int)m_strBufferList.size(); i ++ )
	{
		int w = /*strlen( m_strBufferList[i].c_str() )*/m_Font.GetColorStringLength(m_strBufferList[i].c_str()) * 6;
		m_textHeiht += ( w / m_nFontLineWidth + ( ( w % m_nFontLineWidth > 0 ) ? 1 : 0 ) );
	}

	m_textHeiht *= ( m_Font.GetHeight() + 2 );

	return m_textHeiht;
}

void CDStatic:: ClearTex()
{
	m_strBufferList.clear();
}

int CDStatic::OnPosChanged()
{
	int nPos = m_pSlideBar->GetPosition();
	m_nLineBegin = nPos;

	//m_drawpos = nPos * ( m_textHeiht - m_nHeight/*300*/ ) / 20;
	m_drawpos = (m_textHeiht + 20 - m_nHeight)*nPos/100;
	m_drawpos = m_drawpos < 0 ? 0 : m_drawpos;

	return 1;
}

void CDStatic::SetText(DWORD dNum)
{
	//memset(m_cBuff, 0, sizeof(m_cBuff));

	char sTxt[50];
    DWORD d = dNum;       
    _ui64toa(d, sTxt, 10);
	m_strBuff = sTxt;
    int a = 0;
    while(d = d/10)
        a ++;

	if (a > 2)
	for (int i = a; i >= 0; i --)
	{
		if (((a - i)%3 == 0) && (a != i))
		{
			m_strBuff[i + a/3 - (a-i)/3 + 1] = ',';
			m_strBuff[i + a/3 - (a-i)/3] = m_strBuff[i];
		}
		else
			m_strBuff[i + a/3 - (a-i)/3] = m_strBuff[i];
	}
    m_strBuff[a + a/3 + 1] = '\0';
}

void CDStatic::SetText(int   nNum)
{
	int ii = nNum;
	char cTxt[50];
    itoa(ii, cTxt, 10);
	m_strBuff = cTxt;
}

void  CDStatic::SetText(LPSTR sText, bool bHaveSpecialChar)
{
	m_bHaveSpecialChar = bHaveSpecialChar; 
	m_strBuff = sText;
	if( bHaveSpecialChar )
	{
		int nStrLen = static_cast<int>(strlen( sText ));
		/// 转换以前的格式: 数字% ????
		for (int i=1; i<nStrLen; i++) 
		{
			if (sText[i] == '%')
			{
				if ( (sText[i-1] >='0' ) && (sText[i-1] <='9' ) ) 
				{
					m_strBuff[i-1] = sText[i];
					m_strBuff[i] = sText[i-1];	
				}
			}
		}
	}
	int nlen =  /*static_cast<int>(strlen( sText ));*/m_Font.GetColorStringLength(m_strBuff.c_str());
	//sText="1、如对方与您处于2%同一立场0%或中立立场，按下键盘上的1%ctrl0%并对其按鼠标左键或右键即";
	m_nLineBegin = 0;
	int nTextWidth = nlen * m_Font.GetWidth();
	int nContainerWidth = ( m_nWidth - 18 ) * ( m_nHeight / m_Font.GetHeight() );
	m_textHeiht = 0;
	int charCount = 0;
	int rowCount = 0;
	for( int i=0; i<(int)m_strBuff.length(); ++i )
	{
		if ( m_strBuff.at(i) ==  '%' )
			++i;
		else if ( m_strBuff.at(i) != '\r'&& m_strBuff.at(i) != '\n'&& m_strBuff.at(i) != '\t' )
		{
			int code = ( BYTE )m_strBuff.at(i);
			if ( code >= 0x80 )
			{
				++i;
				charCount += 2;
			}
			else
				++charCount;
			if( charCount*6 >= m_nFontLineWidth )
			{
				++rowCount;
				charCount = 0;
			}
		}
		else
		{
			/// 换行
			++rowCount;
			charCount = 0;
		}
	}
	if( charCount > 0 )
		++rowCount;
	m_textHeiht = rowCount*m_Font.GetHeight();
 
	if ( ( nTextWidth > nContainerWidth ) && ( nContainerWidth > 12 ) )
	{
		m_bHaveSlideBar = true;
		m_pSlideBar->SetPosition( 0 );
		m_pSlideBar->Show(true);
	}
	else
	{
		m_bHaveSlideBar = false;
		m_pSlideBar->SetPosition( 0 );
		m_pSlideBar->Show(false);
	}		
}

void CDStatic::SetTextColor(int nR, int nG, int nB) 
{ 
    m_dwClr = g_RGB(nR, nG, nB);
	// m_dwClrTxt[0] = m_dwClr;
}

void CDStatic::SetTextColor(DWORD color)
{
	m_dwClr = color;
}

void CDStatic::SetTextLine(int nWidth, int nHeight) 
{
	m_nFontLineWidth = nWidth;
	m_nFontLineHeight = nHeight;

	RECT rcWnd = GetRECT();
	if (rcWnd.right - rcWnd.left - 20 > 0)
		if (m_nFontLineWidth > (rcWnd.right - rcWnd.left - 20))
			m_nFontLineWidth =  rcWnd.right - rcWnd.left - 20;

    m_Font.SetLine(m_nFontLineWidth, m_nFontLineHeight);
}

char* CDStatic::GetText() 
{ 
	return (char*)m_strBuff.c_str();
}

void CDStatic::OnMouseLeave()
{
	if(m_bMouseEnter)
		m_MouseLeaveEvent.Invoke(this, NULL);
	m_bMouseEnter = false;
    if ( m_pSlideBar )
        m_pSlideBar->m_bShowTip = false;	
}

// void CDStatic::SetSliderBarSpr(char* fileback, char* filenormal, char* fileactive, char* filedown)
// {
// 	if (m_pSlideBar)
// 		m_pSlideBar->SetBePicture(fileback,filenormal, fileactive, filedown);
// }