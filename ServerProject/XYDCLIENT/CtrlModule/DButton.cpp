/****************************************************************
//  FileName:   DButton.cpp
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  说明：普通按钮类，需先new一个该类的指针，然后在Create()中创建和再入资源
//　　　　参数通过SParameter结构传入，为４个图片文件名，Bitmap格式
****************************************************************/
#include "StdAfx.h"


#include "Engine.h"
#include "Canvas.h"
#include "dbutton.h"
#include "Colors.h"

WavSound CDButton::m_ClickEffect;

CDButton::CDButton(void) : CDFace(IID_CDBUTTON)
{
	m_bText = true;
	m_nBtnState = 1; 
	m_bEnable = true;
	m_bLDown = false;
	m_bCapture = false;
	m_x = 0;
	m_y = 0;
    m_bShowToolTip = false;
    m_bInRect = false;
    m_nWidth = 120, m_nHeight = 70;
    m_pToolTip[0] = 0;
}

CDButton::~CDButton(void)
{
}

int	 CDButton::InitByBinary(LPARAM lParam)
{

	return 0;
}

bool CDButton::SetParameter(LPCSTR szParameterName, LPARAM lParam)
{
    if(strcmp(szParameterName, "FileName_Disable") == 0)
        dwt::strcpy(m_Param.sBmpFileName[0], (LPSTR)lParam, 128);
    else if(strcmp(szParameterName, "FileName_Normal") == 0)
        dwt::strcpy(m_Param.sBmpFileName[1], (LPSTR)lParam, 128);
    else if(strcmp(szParameterName, "FileName_Active") == 0)
        dwt::strcpy(m_Param.sBmpFileName[2], (LPSTR)lParam, 128);
    else if(strcmp(szParameterName, "FileName_Down") == 0)
        dwt::strcpy(m_Param.sBmpFileName[3], (LPSTR)lParam, 128);

    return CDFace::SetParameter(szParameterName, lParam);
}

int  CDButton::OnCreate(LPARAM lParam)
{
	if(!lParam)
		return 0;
	//SParameter *param = (SParameter *)lParam;
	//if (m_Param.iType == 1)
	{
		for(int i = 0; i < 4; i ++)
		{
			if(m_Param.sBmpFileName[i][0] != 0)
			{
				if(!m_bmp[i].Load(m_Param.sBmpFileName[i]))
				{
					//MessageBox(NULL, "Bmp Load Failed", "Error", MB_OK);
					//return 0;
				}
			}
			else
				return 0;
		}
		DrawBmp(1);
	}
	return 1;
}

int CDButton::KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y)
{
	int nRet = RET_NONE;

	RECT rcWnd = GetRECT();
	ClientToScreen(&rcWnd);
	POINT pt;
	pt.x = x;
	pt.y = y;
	//0: disable, 1: normal, 2:up, 3:down, 4:down_down, 5:down_up  //, 6:down_focus,
	if(!m_bEnable)
		return nRet;

	if(PtInRect(&rcWnd,pt))
	{
		if(m_bCapture && m_bLDown)
		{
			m_nBtnState = 3;
		}
	
		if(iKeyState == MS_LDOWN)
		{
            SetCapture();
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
			if(m_nBtnState != 2) 
			{
				m_nBtnState = 2;
			}
			if(m_bLDown)
			{
                static DWORD dwPrevClickTime = 0;

                if (timeGetTime() < dwPrevClickTime + g_pCanvas->m_nHeight)
	                return RET_NONE;

                dwPrevClickTime = timeGetTime();

                ReleaseCapture();
				m_nBtnState = 1;
  				nRet = RET_OK;
				m_bLDown = false;
                m_bInRect = false;

                // 播放点击音效
                extern bool bUseSound; 
                if ( bUseSound ) m_ClickEffect.Play(0, 0, 0);

                return OnClicked();
			}
		}
		if(iKeyState == MS_MOVE)
		{
			if(m_bLDown == true)
			{
				m_nBtnState = 3;
			}
			else
			{
				m_nBtnState = 2;
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
				DrawBmp(m_nBtnState);
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

//按钮点击的时候触发此函数
int CDButton::OnClicked()
{
	m_ClickEvent.Invoke(this,0);
	return GetParent()->OnMyMessage(this, 0);
}

//鼠标移出窗口（控件）矩形框（在Create中设置的）时触发
void  CDButton::OnMouseLeave()
{
    m_bInRect = false;
    m_nBtnState = 1;
}

void CDButton::Update()
{
	DrawBmp(m_nBtnState);

}

void CDButton::UpdateTip()
{
    RECT rcToolTip;
    rcToolTip.left = m_x - m_nWidth/2, rcToolTip.right = m_x + m_nWidth/2, rcToolTip.top = m_y - m_nHeight, rcToolTip.bottom = m_y;
    if(m_bShowToolTip && m_bInRect)
    {
        ClearAlphaWithBord(m_x - m_nWidth/2, m_y - m_nHeight, m_nWidth, m_nHeight, g_RGB(255,255,255), 16);
        m_Font.SetLine(m_nWidth - 10, 15);
        int a =m_Font.GetColor();
        m_Font.SetColor(g_RGB(222,2,2));
        m_Font.DrawText(m_x - m_nWidth/2, m_y - m_nHeight, m_pToolTip);
        DrawRect(&rcToolTip, 3333, 6666);
        m_Font.SetColor(a);
    }
}

void CDButton::AddToolTip(char* sToolTip)
{
    if(strlen(sToolTip) >= 30)
    {
        MessageBox(NULL, "Error", "string tool long", MB_OK);
        return;
    }
    strcpy(m_pToolTip, sToolTip);
    if(sToolTip && sToolTip[0] != 0)
    m_bShowToolTip = true;
}

void CDButton::DrawBmp(int nBmpIndex)
{
    RECT rcWnd = GetRECT();
    ClientToScreen(&rcWnd);
	if(nBmpIndex > 3)
		nBmpIndex = 3;
	//m_bmp[nBmpIndex].MakePalette();
	m_bmp[nBmpIndex].Draw(rcWnd.left, rcWnd.top);
    if (!m_bEnable)
    {
        g_pCanvas->ClearAlpha(rcWnd.left, rcWnd.top, 
            m_bmp[nBmpIndex].GetWidth(), m_bmp[nBmpIndex].GetHeight(), 
            g_RGB(0, 0, 0), 16);
    }
}

void CDButton::Enable(bool bEnable)
{
    m_bEnable = bEnable;
	if(bEnable)
    {
		m_nBtnState = 1;
    }
    else
    {
        m_nBtnState = 0;
    }
}

