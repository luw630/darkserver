/****************************************************************
//  FileName:   DSliderBar.cpp
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
****************************************************************/
#include "StdAfx.h"

#include "Engine.h"
#include "Canvas.h"
#include "Colors.h"
#include "dslidebar.h"

#define BAR_HEIGHT_WIDTH	30
#define CLR_TIPBACK g_RGB(44,44,44)
#define CLR_TIPTEXT g_RGB(222,222,222)

CDSlideBar::CDSlideBar(void) : CDFace(IID_CDSLIDEBAR)
{
	m_nPos = 0;
	m_bShowBmp = false;
	m_nMaxPos = 100;
	m_nMinPos = 0;
	m_bCapture = false;
    m_nBtnState = 1;
	m_bHorizon = true;
    m_bShowTip = false;

	m_nBarWidthHeight = BAR_HEIGHT_WIDTH;
	m_strBarImg[0]='\0';
}

CDSlideBar::~CDSlideBar(void)
{
}

int  CDSlideBar::OnCreate(LPARAM lParam)
{
	SParameter* param = (SParameter*)lParam;
	m_nBarColor = param->nColorBar;
	m_nSlideColor = param->nColorSlider;

	if( m_strBarImg[0]!='\0' )
	{
		m_sprBar[0].Load(m_strBarImg);
	}
	RECT rcWnd = GetRECT();
	if ((rcWnd.right - rcWnd.left) < BAR_HEIGHT_WIDTH)
		m_nBarWidthHeight = rcWnd.right - rcWnd.left;
	return 1;
}

void CDSlideBar::SetPosition(int nPos, int nMaxPos, int nMinPos)
{
	if(nPos < 0)
		nPos = 0;
	if(nPos > 100)
		nPos = 100;
	m_nPos = nPos;
	m_nMaxPos = nMaxPos;
	m_nMinPos = nMinPos;
}

int  CDSlideBar::GetPosition()
{
	return m_nPos;
}

int	CDSlideBar::InitByBinary(LPARAM lParam)
{
	return 0;
}

void CDSlideBar::SetHorizon(bool bHorizon) 
{
	m_bHorizon = bHorizon;

	RECT rcWnd = GetRECT();
	if (bHorizon)
	{
		if ((rcWnd.right - rcWnd.left) < BAR_HEIGHT_WIDTH)
			m_nBarWidthHeight = rcWnd.right - rcWnd.left - 2;
	}
	else
	{
		if ((rcWnd.bottom - rcWnd.top) < BAR_HEIGHT_WIDTH)
			m_nBarWidthHeight = rcWnd.bottom - rcWnd.top - 2;
	}
}

int	CDSlideBar::KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y)
{
    RECT rcWnd = GetRECT();
    ClientToScreen(&rcWnd);

	RECT rcBar = GetRcBar();

	POINT pt;
	pt.x = x;
	pt.y = y;
	if(iKeyState == MS_MOVE)
		g_pCanvas->DrawLine(1,1,x,y, 11);

	static int nXtoBar = 0;
	static int nYtoBar = 0;

	m_nBtnState = 1;
#pragma warning( disable : 4244 )
	if(PtInRect(&rcBar,pt))
	{
        m_bShowTip = true;
		m_nBtnState = 2;
		if(iKeyState == MS_LDOWN)
		{
			m_x = x;
			m_y = y;
			if(m_nBtnState != 3)
			{
				m_nBtnState = 3;
				m_bCapture = true;
				SetCapture();
			}
			nXtoBar = m_x - rcBar.left;
			nYtoBar = m_y - rcBar.top;
		}
		else if(iKeyState == MS_LUP)
		{
			if(m_bCapture)
			{
				ReleaseCapture();
				m_bCapture = false;
				if(m_nBtnState != 2) 
				{
					m_nBtnState = 2;
				}
			}
		}
		if((iKeyState == MS_MOVE) && m_bCapture)
		{
			float nTmp;
			int nPos;
			if (m_bHorizon)
			{
				nTmp = (float)(rcWnd.right - rcWnd.left - m_nBarWidthHeight)/(m_nMaxPos - m_nMinPos);
				nPos = (x - rcWnd.left - nXtoBar)/nTmp;
			}
			else
			{
				nTmp = (float)(rcWnd.bottom - rcWnd.top - m_nBarWidthHeight)/(m_nMaxPos - m_nMinPos);
				nPos = (y - rcWnd.top - nYtoBar)/nTmp;
			}
			SetPosition(nPos, m_nMaxPos, m_nMinPos);
			OnPosChanging(nPos);
		}
	}
	else if(PtInRect(&rcWnd, pt))
	{
		if(iKeyState == MS_LDOWN)
		{
			m_bCapture = true;
			m_x = x;
			m_y = y;
			int nTmp;
			if (m_bHorizon)
				nTmp = (x - rcWnd.left - nXtoBar)* m_nMaxPos/(rcWnd.right - rcWnd.left - m_nBarWidthHeight);
			else
				nTmp = (y - rcWnd.top - nYtoBar)* m_nMaxPos/(rcWnd.bottom - rcWnd.top - m_nBarWidthHeight);
			SetPosition(nTmp, m_nMaxPos, m_nMinPos);
			OnPosChanging(nTmp);
 			m_x = x;
			
			if(m_nBtnState != 3)
			{
				m_nBtnState = 3;
			}
		}
	}

	if(!PtInRect(&rcBar,pt)) 
	{
        m_bShowTip = false;
		if((iKeyState == MS_MOVE) && m_bCapture && m_nBtnState != 2)
		{
			float nTmp;
			int nPos;
			if (m_bHorizon)
			{
				nTmp = (float)(rcWnd.right - rcWnd.left - m_nBarWidthHeight)/(m_nMaxPos - m_nMinPos);
				nPos = (x - rcWnd.left - nXtoBar)/nTmp;
			}
			else
			{
				nTmp = (float)(rcWnd.bottom - rcWnd.top - m_nBarWidthHeight)/(m_nMaxPos - m_nMinPos);
				nPos = (y - rcWnd.top - nYtoBar)/nTmp;
			}

			if(m_bCapture)
			{
				SetPosition(nPos, m_nMaxPos, m_nMinPos);
				OnPosChanging(nPos);
			}
			else if(m_nBtnState != 3) 
			{
				m_nBtnState = 2;
			}
		}

		if(m_nBtnState != 1)
		{
			m_nBtnState = 1;
		}
		if(iKeyState == MS_LUP)
		{
			if(m_bCapture)
			{
				m_bCapture = false;
				ReleaseCapture();
				if(m_nBtnState != 2) 
				{
					m_nBtnState = 2;
				}
			}
		}	
	}
	return 0;
}

int CDSlideBar::OnPosChanging(int nPos)
{
	if(nPos > 100) nPos = 100;
	if(nPos < 0) nPos = 0;
	return GetParent()->OnMyMessage(this, (LPARAM)nPos);
}
bool	CDSlideBar::SetParameter( LPCSTR szParameterName, LPARAM lParam )
{
	if ( lParam && strcmp( szParameterName, "image" ) == 0 ) {
		dwt::strcpy(m_strBarImg, (LPSTR)lParam, 128);
	}
	else
		return CDFace::SetParameter(szParameterName, lParam);
	return true;
}
void	CDSlideBar::Update()
{
    RECT rcWnd = GetRECT();
    ClientToScreen(&rcWnd);
	int nBordClrLT = m_nSlideColor + RGB(333,33,33);
	int nBordClrRB = m_nSlideColor - RGB(333,41,41);

	if(!m_bShowBmp)
		g_pCanvas->ClearAlpha(rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top + 1, m_nSlideColor, 8);
	else
		m_sprBack.DrawAlpha(rcWnd.left, rcWnd.top, 0);

	RECT rcBar;
	int a = (rcWnd.right - rcWnd.left - m_nBarWidthHeight)/(m_nMaxPos - m_nMinPos);
	float b = m_nPos /(m_nMaxPos - m_nMinPos);

	if (m_bHorizon)
	{
		rcBar.left = (rcWnd.right - rcWnd.left - m_nBarWidthHeight) * m_nPos / (m_nMaxPos - m_nMinPos) + rcWnd.left;
		rcBar.top = rcWnd.top + 1;
		rcBar.right = rcBar.left + m_nBarWidthHeight;
		rcBar.bottom = rcWnd.bottom - 1;
	}
	else
	{
		rcBar.left = rcWnd.left + 1;
		rcBar.top = (rcWnd.bottom - rcWnd.top - m_nBarWidthHeight) * m_nPos / (m_nMaxPos - m_nMinPos) + rcWnd.top;
		rcBar.right = rcWnd.right - 1;
		rcBar.bottom = rcBar.top + m_nBarWidthHeight;
	}
	if(!m_bShowBmp)
		g_pCanvas->ClearAlpha(rcBar.left, rcBar.top, rcBar.right - rcBar.left, rcBar.bottom - rcBar.top, m_nBarColor, 8);
	else
	{
		int nFrames = m_sprBar[0].GetFrames();
		if( nFrames>=1 )
		{
			if( nFrames==4 )
				m_sprBar[0].DrawAlpha(rcBar.left, rcBar.top, m_nBtnState);
			else
				m_sprBar[0].DrawAlpha(rcBar.left, rcBar.top, m_nBtnState-1);
		}
		else
		{
			if (m_nBtnState == 2 || m_nBtnState == 3) 
				m_sprBar[m_nBtnState].DrawAlpha(rcBar.left, rcBar.top, 0);
			else
				m_sprBar[1].DrawAlpha(rcBar.left, rcBar.top, 0);
		}
		//if (m_nBtnState == 2) 
		//	m_sprBarActive.DrawAlpha(rcBar.left, rcBar.top, 0);
		//else if (m_nBtnState == 3)
		//	m_sprBarDown.DrawAlpha(rcBar.left, rcBar.top, 0);
		//else
		//	m_sprBarNormal.DrawAlpha(rcBar.left, rcBar.top, 0);
	}

	if(!m_bShowBmp)
		if (m_bCapture) 
			DrawRect(&rcBar, 5555, 5555);
}

void CDSlideBar::SetBePicture(char* szFileName_Back, char* szFileName_Bar_Normal, 
		char* szFileName_Bar_Active, char* szFileName_Bar_Down)
{
	m_bShowBmp = true;
	m_sprBack.Load(szFileName_Back);
	BOOL b1 = m_sprBar[1].Load(szFileName_Bar_Normal,1);
	BOOL b2 = m_sprBar[2].Load(szFileName_Bar_Active);
	m_sprBar[3].Load(szFileName_Bar_Down);

	RECT rcWnd = GetRECT();
	ClientToScreen(&rcWnd);

	if (m_sprBar[1].GetHeight() > m_nBarWidthHeight)
		m_nBarWidthHeight = m_sprBar[1].GetHeight();

	if (m_sprBar[1].GetWidth() > m_nBarWidthHeight)
		m_nBarWidthHeight = m_sprBar[1].GetWidth();
}
void CDSlideBar::SetBePicture(char* szFileName_Back, char* szFileName)
{
	m_bShowBmp = true;
	m_sprBack.Load(szFileName_Back);

	m_sprBar[0].Load(szFileName);

	if (m_sprBar[0].GetHeight() > m_nBarWidthHeight)
		m_nBarWidthHeight = m_sprBar[0].GetHeight();

	if (m_sprBar[0].GetWidth() > m_nBarWidthHeight)
		m_nBarWidthHeight = m_sprBar[0].GetWidth();
}
void CDSlideBar::PaintLevelTop()
{
    if ( (!m_bShowTip || m_bCapture)  )
        return;

    int nWidth = 40;
    int nHeight = 20;

    POINT pt;
    GetCurrentPos(pt);

    ClearAlphaWithBord(pt.x - nWidth/2, pt.y - nHeight, nWidth, nHeight, CLR_TIPBACK, 5);
    int oldw = m_Font.GetLineWidth();
    int oldh = m_Font.GetLineHeight();
    int olda =m_Font.GetColor();

    m_Font.SetLine(200, 20);
    m_Font.SetColor(CLR_TIPTEXT);

    m_Font.DrawText(pt.x - nWidth/2 + 4, pt.y  - nHeight + 4, "ÍÏ¶¯ÎÒ");

    m_Font.SetColor(olda);
    m_Font.SetLine(oldw, oldh);
}

void CDSlideBar::OnMouseLeave()
{
    m_bShowTip = false;
}

RECT CDSlideBar::GetRcBar()
{
    RECT rcWnd = GetRECT();
    ClientToScreen(&rcWnd);

    RECT rcBar;
    if (m_bHorizon)
    {
        rcBar.left = (rcWnd.right - rcWnd.left - m_nBarWidthHeight) * m_nPos / (m_nMaxPos - m_nMinPos) + rcWnd.left;
        rcBar.top = rcWnd.top + 1;
        rcBar.right = rcBar.left + BAR_HEIGHT_WIDTH;
        rcBar.bottom = rcWnd.bottom - 1;
    }
    else
    {
        rcBar.left = rcWnd.left + 1;
        rcBar.top = (rcWnd.bottom - rcWnd.top - m_nBarWidthHeight) * m_nPos / (m_nMaxPos - m_nMinPos) + rcWnd.top;
        rcBar.right = rcWnd.right - 1;
        rcBar.bottom = rcBar.top + m_nBarWidthHeight;
    }

    return rcBar;
}
