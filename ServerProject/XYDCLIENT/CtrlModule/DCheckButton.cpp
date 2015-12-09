/****************************************************************
//  FileName:   DCheckButton.cpp
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  说明：　检查框类，需先new一个该类的指针，然后在Create()中创建和再入资源
//          参数通过SParameter结构传入，为7个图片文件名，Bitmap格式
****************************************************************/
#include "StdAfx.h"

#include "dcheckbutton.h"
#include <Colors.h>

CDCheckButton::CDCheckButton(void) : CDFace(IID_CDCHECKBUTTON)
{
	m_bCapture = false;
	m_nState = 1;
    m_sToolTip[0] = 0;
    m_sBmpFile[0] = 0;
    m_bLDown = false;
    m_bShowToolTip = false;
    m_bInRect = false;
    m_bEnable = true;
    m_nWidth = 120, m_nHeight = 70;
}

CDCheckButton::~CDCheckButton(void)
{
}


int  CDCheckButton::OnCreate(LPARAM lParam)
{
    RECT rcTmp = GetRECT();
	ClientToScreen(&rcTmp);
	SParameter *param = (SParameter *)lParam;
	for(int i = 0; i < 7; i ++)
	{
        if(strlen(param->lpszBmpFileName[i])>=128)
        {
            MessageBox(NULL, "Error", "file name too long", MB_OK);
            return 0;
        }
		strcpy(m_sBmpFile, param->lpszBmpFileName[i]);
		m_bmp[i].Init(rcTmp.right - rcTmp.left,rcTmp.bottom - rcTmp.top,16);
		bool b = m_bmp[i].Load(m_sBmpFile) == TRUE;
		//m_bmp[i].MakePalette();
	}
	return 1;
}

void CDCheckButton::OnCheck(bool bChecked)
{
	GetParent()->OnMyMessage(this, bChecked);
}

int	CDCheckButton::InitByBinary(LPARAM lParam)
{
	return 1;
}

int		CDCheckButton::KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y)
{
    if(!m_bEnable)
        return 1;
	POINT pt;
	pt.x = x;
	pt.y = y;
	RECT m_rcWnd = GetRECT();
	ClientToScreen(&m_rcWnd);

    m_bInRect = true;

    Active();
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
    m_x = x, m_y = y;


	return 0;
}

void CDCheckButton::AddToolTip(LPSTR sToolTip)
{
    if(strlen(sToolTip) >= 30) 
    {
        MessageBox(NULL, "Error", "string too long", MB_OK);
        _asm int 3
        return ;
    }
    strcpy(m_sToolTip, sToolTip);
    if(sToolTip && sToolTip[0] != 0)
    m_bShowToolTip = true;
}

void	CDCheckButton::Update()
{
    RECT rcTmp = GetRECT();
	ClientToScreen(&rcTmp);
    if(!m_bInRect)
        m_nState = m_bChecked?4:1;

    m_bmp[m_nState].Draw(rcTmp.left,rcTmp.top);

    RECT rcToolTip;
    rcToolTip.left = m_x - m_nWidth/2, rcToolTip.right = m_x + m_nWidth/2, rcToolTip.top = m_y - m_nHeight, rcToolTip.bottom = m_y;
    if(m_bShowToolTip && m_bInRect)
    {
        g_pCanvas->ClearAlpha(m_x - m_nWidth/2, m_y - m_nHeight, m_nWidth, m_nHeight, g_RGB(255,255,255), 16);
        m_Font.SetLine(m_nWidth - 10, 15);
        int a =m_Font.GetColor();
        m_Font.SetColor(g_RGB(222,2,2));
        m_Font.DrawText(m_x - m_nWidth/2, m_y - m_nHeight, m_sToolTip);
        DrawRect(&rcToolTip, 3333, 6666);
        m_Font.SetColor(a);
    }
}
