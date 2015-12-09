/****************************************************************
//  FileName:   DSprCheckButton.cpp
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
****************************************************************/
#include "StdAfx.h"

#include "dsprcheckbutton.h"
#include <Colors.h>

extern BYTE byAlpha;
extern bool isAlphaEnable;

#define CLR_TIPTEXT g_RGB(222,222,222)
#define CLR_TIPBACK g_RGB(44,44,44)

CDSprCheckButton::CDSprCheckButton(void) : CDFace(IID_CDSPRCHECKBUTTON)
{
	m_bLDown = false;
	m_nState = 1;
    m_bShowToolTip = false;
    m_bInRect = false;
    m_nWidth = 70, m_nHeight = 20;
    m_bEnable = true;
    m_sBmpFile[0] = 0;
    m_sToolTip[0][0] = 0;
    m_bChecked = false;

	m_nTipWidth = 0;
	m_nTipHeight = 0;
	m_nLines = 0;
	m_bUseColor = false;
	m_dwColor = 0;
    SetMonseInTimeMax();
}

CDSprCheckButton::~CDSprCheckButton(void)
{
}

bool CDSprCheckButton::SetParameter(LPCSTR szParameterName, LPARAM lParam)
{
    if(strcmp(szParameterName, "SprFileName_Disable") == 0)
        dwt::strcpy(m_Param.sBmpFileName[0], (LPSTR)lParam, 256);
    else if(strcmp(szParameterName, "SprFileName_Normal") == 0)
        dwt::strcpy(m_Param.sBmpFileName[1], (LPSTR)lParam, 256);
    else if(strcmp(szParameterName, "SprFileName_Active") == 0)
        dwt::strcpy(m_Param.sBmpFileName[2], (LPSTR)lParam, 256);
    else if(strcmp(szParameterName, "SprFileName_Down") == 0)
        dwt::strcpy(m_Param.sBmpFileName[3], (LPSTR)lParam, 256);
    else if(strcmp(szParameterName, "SprFileName_CheckedNormal") == 0)
        dwt::strcpy(m_Param.sBmpFileName[4], (LPSTR)lParam, 256);
    else if(strcmp(szParameterName, "SprFileName_CheckedActive") == 0)
        dwt::strcpy(m_Param.sBmpFileName[5], (LPSTR)lParam, 256);
    else if(strcmp(szParameterName, "SprFileName_CheckedDown") == 0)
        dwt::strcpy(m_Param.sBmpFileName[6], (LPSTR)lParam, 256);

	else if (strcmp(szParameterName, "ToolTip") == 0)
		AddToolTip((LPSTR)lParam);

    return CDFace::SetParameter(szParameterName, lParam);
}
 
int  CDSprCheckButton::OnCreate(LPARAM lParam)
{
	for(int i = 0; i < 7; i ++)
	{
        if((m_Param.sBmpFileName[i][0] == 0) || (strlen(m_Param.sBmpFileName[i]) > 256))
            return 0;
		strcpy(m_sBmpFile, m_Param.sBmpFileName[i]);
		m_bmp[i].Load(m_sBmpFile);
		//m_bmp[i].MakePalette();
	}
	return 1;
}

void CDSprCheckButton::OnCheck(bool bChecked)
{
	GetParent()->OnMyMessage(this, bChecked);
}

int	CDSprCheckButton::InitByBinary(LPARAM lParam)
{
	return 1;
}

int		CDSprCheckButton::KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y)
{
	POINT pt;
	pt.x = x;
	pt.y = y;
	RECT m_rcWnd = GetRECT();
	ClientToScreen(&m_rcWnd);

    m_x = x, m_y = y;

	int iInRegion;

    m_bInRect = true;
	iInRegion = m_bmp[0].GetPixelAlpha(0, x - m_rcWnd.left, y - m_rcWnd.top);
	if(iInRegion)
	{
		if(!m_bEnable)
			return 1;

		if(iKeyState == MS_LDOWN)
		{
			m_nState = m_bChecked?6:3;
			m_bLDown = true;
		}
		else if((iKeyState == MS_LUP)&&(m_bLDown))
		{
			m_bLDown = false;


			m_nState = m_bChecked?2:5;
			m_bChecked = !m_bChecked;
			OnCheck(m_bChecked);
		}
		else
		{
			if(m_bLDown)
			{
				m_nState = m_bChecked?6:3;
			}
			else
			{
				m_nState = m_bChecked?5:2;
			}
		}
	}
	else
	{
		m_bInRect = false;

		if(!m_bEnable)
			return 1;
		m_nState = m_bChecked?4:1;
	}

	return 0;
}

void CDSprCheckButton::OnMouseLeave()
{
    m_bInRect = false;

	if (m_bEnable)
		m_nState = m_bChecked?4:1;

	m_nMouseInTime  = 0;
}

void CDSprCheckButton::AddToolTip(LPSTR sToolTip)
{
	for (int i=0; i<5; i++)
		memset(m_sToolTip[i], 0, 30);
	m_nLines = 0;
	int nMaxLen = 0;
    bool bIsNewLine = false;
	for (int i=0, j=0; i<(int)strlen(sToolTip); i++)
	{
		if ((sToolTip[i] == 0) || (sToolTip[i] == '#') ||(j > 28))
		{
            bIsNewLine = j > 28 ? true : false;

			m_sToolTip[m_nLines][j] = 0;
			j = 0;
			if ((m_nLines > 3) || (sToolTip[i] == 0))
				break;
			m_nLines ++;
            
            if ( !bIsNewLine )
                continue;
		}

		m_sToolTip[m_nLines][j++] = sToolTip[i];

		if (j > nMaxLen)
			nMaxLen = j;
	}

	m_nTipWidth = nMaxLen * m_Font.GetWidth()/2 + 4;
	m_nTipHeight= (m_nLines+1) *  m_Font.GetHeight() + 4;

    if(sToolTip && sToolTip[0] != 0)
		m_bShowToolTip = true;
}

void CDSprCheckButton::SetCheck(bool bCheck)
{ 
	m_bChecked = bCheck;
	if(m_bChecked)
		m_nState = 4;
	else
    {
        m_nState = 1;
    }
}

void    CDSprCheckButton::Enable(bool bEnable )
{
    m_bEnable = bEnable; 
    m_nState = m_bChecked?4:1;
}

void	CDSprCheckButton::Update()
{
	RECT m_rcWnd = GetRECT();
	ClientToScreen(&m_rcWnd);
    BYTE alpha = isAlphaEnable ? byAlpha : 32;
	if (m_bUseColor && m_bChecked)
		m_bmp[ m_nState ].DrawAlphaColor( m_rcWnd.left, m_rcWnd.top, 0, m_dwColor, alpha );
	else
		m_bmp[ m_nState ].DrawAlpha( m_rcWnd.left, m_rcWnd.top, 0, alpha );
}

void CDSprCheckButton::UpdateTip()
{
	int nHeight = m_nTipHeight;
	int nWidth = m_nTipWidth;
	m_nMouseInTime ++; 

	if (m_nMouseInTime < m_nMonseInTimeMax) 
	{
		nHeight = m_Font.GetHeight() + 8;
		nWidth  = (int)strlen(m_sToolTip[0]) * m_Font.GetWidth()/2 + 8;
	}

	if (m_x - nWidth/2 + 2 < 0)
	{
		m_x = nWidth/2 - 2;
	}
	if (m_x + nWidth/2 > g_pCanvas->m_nWidth)
	{
		m_x = g_pCanvas->m_nWidth - nWidth/2;
	}
	if (m_y - nHeight + 1 < 0)
	{
		m_y = nHeight - 1;
	}

    RECT rcToolTip; 
    rcToolTip.left = m_x - nWidth/2 + 2; rcToolTip.top = m_y - nHeight + 1; rcToolTip.right = m_x + nWidth/2; rcToolTip.bottom = m_y;

    RECT rcWnd = GetRECT();
    ClientToScreen(&rcWnd);
    POINT pt;
    GetCurrentPos(pt);
    if (PtInRect(&rcWnd, pt))
    if(m_bShowToolTip && m_bInRect)
    {
        ClearAlphaWithBord(m_x - nWidth/2, m_y - nHeight, nWidth, nHeight, CLR_TIPBACK, 5);
        int w = m_Font.GetLineWidth();
        int h = m_Font.GetLineHeight();

        int a =m_Font.GetColor();
        m_Font.SetLine(200, 20);
        m_Font.SetColor(CLR_TIPTEXT);

		m_Font.DrawText(m_x - nWidth/2 + 4, m_y - nHeight + 4, m_sToolTip[0]);
		if (m_nMouseInTime > m_nMonseInTimeMax)
			for (int i=1; i<=m_nLines; i++)
				m_Font.DrawText(m_x - nWidth/2 + 4, m_y - nHeight + 4 + i* m_Font.GetHeight(), m_sToolTip[i]);

        //DrawRect(&rcToolTip, 3333, 6666);

        m_Font.SetColor(a);

        m_Font.SetLine(w, h);
    }
}

void CDSprCheckButton::SetCheckWithColor(bool bCheck, DWORD color)
{
	m_bChecked = bCheck;
	if(m_bChecked)
	{
		m_nState = 4;
		m_dwColor = color;
		m_bUseColor = true;
	}
	else
	{
		m_nState = 1;
		m_dwColor = 0;
		m_bUseColor = false;
	}
}

void CDSprCheckButton::GetToolTip(char* tipstr)
{
	if (tipstr == NULL)
	{
		return;
	}
	strncpy(tipstr, m_sToolTip[0], 30);
    strncat(tipstr, ":", 2);
	for (int i=1; i<=m_nLines; i++)
	{
		strncat(tipstr, m_sToolTip[i], 30);
	}
	return ;
}

bool CDSprCheckButton::IsEnable()
{
	return m_bEnable;
}