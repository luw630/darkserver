#include "StdAfx.h"
#include ".\diebutton.h"

#include <ShellApi.h>

#include "Engine.h"
#include "Canvas.h"
#include "Colors.h"
#include "DButton.h"



#define CLR_TIPTEXT g_RGB(222,222,222)
#define CLR_TIPBACK g_RGB(44,44,44)

CDIEButton::CDIEButton(void) : CDFace(IID_CDIEBUTTON)
{
	m_bText = true;
	m_nBtnState = 1;
	m_bEnable = true;
	m_bEnableMove = false;
	m_bDrage = false;
	m_bCapture = false;
	m_x = 0;
	m_y = 0;
    m_sToolTip[0][0] = 0;
    m_sBmpFile[0] = 0;
    m_bShowToolTip = false;
    m_bInRect = false;

	m_nTipWidth = 0;
	m_nTipHeight = 0;
	m_nLines = 0;
	m_bFlash = false;
	m_nBmpIndex = 1;
    m_nMouseInTime = 0;
    SetMonseInTimeMax();

    memset( &m_Param, 0, sizeof( m_Param ) );
}

CDIEButton::~CDIEButton(void)
{
}

bool CDIEButton::SetParameter(LPCSTR szParameterName, LPARAM lParam)
{
    if (strcmp(szParameterName, "SprFileName_Disable") == 0)
        dwt::strcpy(m_Param.sBmpFileName[0], (LPSTR)lParam, 128);
    else if (strcmp(szParameterName, "SprFileName_Normal") == 0)
    {
        dwt::strcpy(m_Param.sBmpFileName[1], (LPSTR)lParam, 128);
        //Sprite sprTmp;
        //sprTmp.Load((LPSTR)lParam);
        //m_nWidth = sprTmp.GetWidth();
        //m_nHeight = sprTmp.GetHeight();
    }
    else if (strcmp(szParameterName, "SprFileName_Active") == 0)
        dwt::strcpy(m_Param.sBmpFileName[2], (LPSTR)lParam, 128);
    else if (strcmp(szParameterName, "SprFileName_Down") == 0)
        dwt::strcpy(m_Param.sBmpFileName[3], (LPSTR)lParam, 128);
	else if (strcmp(szParameterName, "ToolTip") == 0)
		AddToolTip((LPSTR)lParam);
    else if ( strcmp( szParameterName, "image" ) == 0 )
        dwt::strcpy( m_Param.sBmpFileName[1], ( LPSTR )lParam, 128 );
	else if( strcmp(szParameterName, "Tip") == 0 )
		AddToolTip((LPSTR)lParam);
	else if( strcmp( szParameterName, "URL" ) == 0  )
		m_URL = ( LPSTR )lParam;
    else
        return CDFace::SetParameter(szParameterName, lParam);

    return true;
}

int	 CDIEButton::InitByBinary(LPARAM lParam)
{
	return 0;
}

int  CDIEButton::OnCreate(LPARAM lParam)
{
	if(!lParam)
		return 0;

	for(int i = 0; i < 4; i ++)
	{
        if(strlen(m_Param.sBmpFileName[i]) > 128) 
            return 0;

		strcpy(m_sBmpFile, m_Param.sBmpFileName[i]);
		m_bmp[i].Load(m_sBmpFile);
	}

    // 增加一个特殊处理，如果m_bmp[1]为无效spr，同时m_bmp[0]为有效spr，则该spr必定是4帧
    /*
    if ( m_bmp[1].GetFrames() == 0 )
    {
        // 如果没有4帧，则尝试使用 m_Param.sBmpFileName[0]加后缀0~3再次创建！
        char tempString[256];
	    for ( int i = 0; i < 4; i ++ )
	    {
		    sprintf( tempString, "%s%d.spr", m_Param.sBmpFileName[1], i );
			m_bmp[i].Load( tempString );
	    }
    }*/

	return 1;
}

void CDIEButton::SetNewParam(LPARAM lNewParam)
{
	if((!lNewParam) || this == NULL)
		return;
	SParameter *param = (SParameter *)lNewParam;
	if (param->iType == 1)
	{
		for(int i = 0; i < 4; i ++)
		{
			if(param->sBmpFileName[i][0] != 0)
			{
                if(strlen(param->sBmpFileName[i]) > 128) 
                    return;
				strcpy(m_sBmpFile, param->sBmpFileName[i]);
				if(!m_bmp[i].Load(m_sBmpFile))
				{
				}
			}
			else
				return;
		}

		if (m_bmp[1].GetWidth() > 0)
		{
			RECT rcWnd = GetRECT();
			rcWnd.right = rcWnd.left + m_bmp[1].GetWidth();
			rcWnd.bottom = rcWnd.top + m_bmp[1].GetHeight();
			SetRECT(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom);
		}
	}
}

void CDIEButton::AdjustRect()
{
	RECT rcWnd = GetRECT();
    rcWnd.right = rcWnd.left + ( m_nWidth = m_bmp[1].GetWidth() );
	rcWnd.bottom = rcWnd.top + ( m_nHeight = m_bmp[1].GetHeight() );
	SetRECT(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom);
}

int CDIEButton::KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y)
{
	int nRet = RET_NONE;

	RECT rcWnd = GetRECT();
	ClientToScreen(&rcWnd);
	POINT pt;
	pt.x = x;
	pt.y = y;

	if (iKeyState < MS_BEGIN)
		return 0;

	int iInRegion;

	if(PtInRect(&rcWnd,pt)) 
	{
        //Active();
		iInRegion = m_bmp[1].GetPixelAlpha(0, x - rcWnd.left, y - rcWnd.top);
		if(iInRegion)
		{

			if(m_bCapture && m_bDrage)
			{
				m_nBtnState = 3;
			}
			
			if(iKeyState == MS_LDOWN)
			{
				if(!m_bEnable)
					return nRet;

				SetCapture();
				m_bCapture = true;

			    m_bLDown = true;
			    m_x = x;
			    m_y = y;
			    if(m_nBtnState != 3)
			    {
				    m_nBtnState = 3;
			    }
			}
			else if(iKeyState == MS_LUP)
			{
				if(!m_bEnable)
					return nRet;

			    if(m_nBtnState != 2) 
			    {
				    m_nBtnState = 2;
			    }

			    if(m_bLDown)
			    {
                    static DWORD dwPrevClickTime = 0;

                    if (timeGetTime() < dwPrevClickTime + 100)
	                    return RET_NONE;

                    dwPrevClickTime = timeGetTime();

                    ReleaseCapture();
					m_bCapture = false;

				    m_nBtnState = 1;
  				    nRet = RET_OK;
				    m_bLDown = false;
                    m_bInRect = false;

                    // 播放点击音效
                    extern bool bUseSound;
                    if ( bUseSound ) CDButton::m_ClickEffect.Play(0, 0, 0);

                    if ( !m_URL.empty() )
                        ShellExecute( GetDesktopWindow(), _T( "open" ), m_URL.c_str(), NULL, NULL, SW_NORMAL );

                    return OnClicked();
                }
			}
			else{}

			if(iKeyState == MS_MOVE)
			{
				if(!m_bEnable)
					m_nBtnState = 0;
				else
				{
					if(m_bLDown == true)
					{
						m_nBtnState = 3;
					}
					else
					{
						m_nBtnState = 2;
					}
				}
                m_bInRect = true;
                m_x = x, m_y = y;			
            }
            m_bInRect = true;
            m_x = x, m_y = y;
		}
	}
	else
	{
		if(iKeyState == MS_LUP)
		{
			ReleaseCapture();
			m_bCapture = false;
			m_bLDown = false;
			if(m_nBtnState != 1)
			{
				m_nBtnState = 1;
			}
		}
		else
		{
			m_nBtnState = 2;
		}
        m_bInRect = false;
	}

	return nRet;
}

int CDIEButton::OnClicked()
{
    CDFace *pParent = GetParent();
    if(pParent != NULL)
	    return pParent->OnMyMessage(this, 0);
    else
    {
        return 0;
    }
}

void CDIEButton::OnMouseLeave()
{
    m_bInRect = false;
    if (!m_bCapture)// = false;
	{
		if (m_bEnable)
			m_nBtnState = 1;
		else
			m_nBtnState = 0;
	}

	m_nMouseInTime  = 0;
}

void CDIEButton::Update()
{
    AdjustRect();
	if (m_bFlash)
	{
		if ((m_nBtnState == 1) && (m_nBmpIndex != 2))
			m_nBmpIndex = m_nBtnState;

		static int nFlash = 0;
		nFlash ++;
		if (nFlash > 40)
		{
			nFlash = 0;

			if (m_nBmpIndex == 1)
				m_nBmpIndex = 2;
			else if (m_nBmpIndex == 2)
				m_nBmpIndex = 1;
		}
	}
	else
		m_nBmpIndex = m_nBtnState;


	DrawBmp(m_nBmpIndex);
}

void CDIEButton::UpdateTip()
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
	if (m_y - nHeight - 13 < 0)
	{
		m_y = nHeight + 13;
	}

    RECT rcToolTip; 
    rcToolTip.left = m_x - nWidth/2 + 2; 
	rcToolTip.top = m_y - nHeight + 1; 
	rcToolTip.right = m_x + nWidth/2; 
	rcToolTip.bottom = m_y;

    RECT rcWnd = GetRECT();
    ClientToScreen(&rcWnd);
    POINT pt;
    GetCurrentPos(pt);
    if (PtInRect(&rcWnd, pt))
	{
		if(m_bShowToolTip && m_bInRect && (m_nBtnState != 1))
		{
			ClearAlphaWithBord(m_x - nWidth/2, m_y - nHeight - 13, nWidth, nHeight, CLR_TIPBACK, 5);
			int w = m_Font.GetLineWidth();
			int h = m_Font.GetLineHeight();

			int a =m_Font.GetColor();
			m_Font.SetLine(200, 20);
			m_Font.SetColor(CLR_TIPTEXT);

			m_Font.DrawText(m_x - nWidth/2 + 4, m_y - nHeight + 4 - 13, m_sToolTip[0]);
			if (m_nMouseInTime > m_nMonseInTimeMax)
				for (int i=1; i<=m_nLines; i++)
					m_Font.DrawText(m_x - nWidth/2 + 4, m_y - nHeight + 4 - 13 + i* m_Font.GetHeight(), m_sToolTip[i]);

			m_Font.SetColor(a);
			m_Font.SetLine(w, h);
		}
	}
	else
	{
		if (!m_bCapture)
		{
			if (m_bEnable)
				m_nBtnState = 1;
			else
				m_nBtnState = 0;
		}
	}
}

void CDIEButton::SetHotKey(char cKey)
{
	m_cKey = cKey;
}

void CDIEButton::AddToolTip(LPSTR sToolTip)
{
	for (int i=0; i<5; i++)
		memset(m_sToolTip[i], 0, 30);
	m_nLines = 0;
	int nMaxLen = 0;
	for (int i=0, j=0; i<(int)strlen(sToolTip); i++)
	{
		if ((sToolTip[i] == 0) || (sToolTip[i] == '#') ||(j > 28))
		{
			m_sToolTip[m_nLines][j] = 0;

			if (j > 28) 
				i --;

			j = 0;
			if ((m_nLines > 3) || (sToolTip[i] == 0))
				break;
			m_nLines ++;

			continue;
		}

		m_sToolTip[m_nLines][j++] = sToolTip[i];

		if (j > nMaxLen)
			nMaxLen = j;
	}

	m_nTipWidth = nMaxLen * m_Font.GetWidth()/2 + 8;
	m_nTipHeight= (m_nLines+1) *  m_Font.GetHeight() + 8;

    if(sToolTip[0] != 0)
		m_bShowToolTip = true;
}

void CDIEButton::SetDragMove(bool bEnableMove)
{
	m_bEnableMove = bEnableMove;
}

void CDIEButton::DrawBmp(int nBmpIndex)
{
    RECT rcWnd = GetRECT();
    ClientToScreen(&rcWnd);
	if(nBmpIndex > 3)
		nBmpIndex = 3;

    if ( m_bmp[1].GetFrames() == 1 )
    {
    	m_bmp[nBmpIndex].DrawAlpha(rcWnd.left, rcWnd.top, 0);
    }
    else
    {
    	m_bmp[1].DrawAlpha(rcWnd.left, rcWnd.top, nBmpIndex);
    }
}

void CDIEButton::Enable(bool bEnable)
{
    m_bEnable = bEnable;
	if(bEnable)
		//DrawBmp(1);
		m_nBtnState = 1;
	else
    {
        m_nBtnState = 0;
		//DrawBmp(0);
		m_nBtnState = 0;
    }
}

