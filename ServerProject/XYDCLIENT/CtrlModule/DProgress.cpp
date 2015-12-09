/****************************************************************
//  FileName:   DProgress.cpp
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
****************************************************************/
#include "StdAfx.h"

#include "Engine.h"
#include "Canvas.h"
#include "dprogress.h"
#include "pub\dwt.h"

#define CLR_TIPTEXT g_RGB(190,0,0)
#define CLR_TIPBACK g_RGB(205,205,205)

CDProgress::CDProgress(void) : CDFace(IID_CDPROGRESS)
{
	m_nProgress = 50;
	m_nStep = 1; 
	m_bShowBmp = false;
	m_nColorBk = 0;
	m_nColorStart = g_RGB(255, 11, 11);
	m_nColorEnd = g_RGB(11, 255, 11);
    m_sToolTip[0] = 0;
    m_bShowToolTip = false;
    m_bInRect = false;
    m_nWidth = 120, m_nHeight = 70;
    m_bHorizon = true;
}

CDProgress::~CDProgress(void)
{
}

int  CDProgress::OnCreate(LPARAM lParam)
{
	return 1;
}

int CDProgress::InitByBinary(LPARAM lParam)
{
	return 1;
}

bool CDProgress::SetParameter(LPCSTR szParameterName, LPARAM lParam)
{
    return CDFace::SetParameter(szParameterName, lParam);
}

int	CDProgress::KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y)
{
	RECT rcTmp = GetRECT();
	ClientToScreen(&rcTmp);
    POINT pt;
    pt.x = x; pt.y = y;
    if(PtInRect(&rcTmp, pt))
    {
        //Active();
        m_bInRect = true;
        m_x = x, m_y = y;
    }
    else
    {
        m_bInRect = false;
    }
	return 0;
}

void CDProgress::Update()
{
	RECT rcTmp = GetRECT();
	ClientToScreen(&rcTmp);
	if(m_bShowBmp)
		m_bmp.Draw(rcTmp.left, rcTmp.top);
	else
	g_pCanvas->Clear(rcTmp.left, rcTmp.top, rcTmp.right - rcTmp.left,  rcTmp.bottom - rcTmp.top , m_nColorBk);

    int xWidth = (rcTmp.right - rcTmp.left) * m_nProgress / 100;
	int yHeight = (rcTmp.bottom - rcTmp.top)* m_nProgress/100;
	if(m_bHorizon)
	{
		if(m_bShowBmp)
			g_pCanvas->Clear(rcTmp.left + xWidth, rcTmp.top, rcTmp.right - rcTmp.left - xWidth, rcTmp.bottom - rcTmp.top, m_nColorMask);
		else
			g_pCanvas->Clear(rcTmp.left, rcTmp.top, xWidth, rcTmp.bottom - rcTmp.top, g_RGB(2*(100-m_nProgress), 111,111));			
	}
	else
	{
		if(m_bShowBmp)
			g_pCanvas->Clear(rcTmp.left, rcTmp.top, rcTmp.right - rcTmp.left, yHeight, m_nColorMask);
		else
			g_pCanvas->Clear(rcTmp.left, rcTmp.bottom - yHeight, rcTmp.right - rcTmp.left, yHeight, g_RGB(2*(100-m_nProgress), 111, 111));
		g_pCanvas->DrawLine(rcTmp.left,rcTmp.bottom - yHeight,rcTmp.right - 1, rcTmp.bottom - yHeight, g_RGB(255,0,0));
	}

}

void CDProgress::UpdateTip()
{
    int nWidth = dwt::strlen(m_sToolTip, 100) * m_Font.GetWidth()/2 + 4; 
    int nHeight =  m_Font.GetHeight() + 4; 

    RECT rcToolTip;
    rcToolTip.left = m_x - nWidth/2, rcToolTip.right = m_x + nWidth/2, rcToolTip.top = m_y - nHeight, rcToolTip.bottom = m_y;
    if(m_bShowToolTip && m_bInRect)
    {
        //g_pCanvas->ClearAlpha(m_x - nWidth/2, m_y - nHeight, nWidth, nHeight, CLR_TIPBACK, 16);
        g_pCanvas->Clear(m_x - nWidth/2, m_y - nHeight, nWidth, nHeight, CLR_TIPBACK);

        int a =m_Font.GetColor();
        m_Font.SetColor(CLR_TIPTEXT);
        m_Font.DrawText(m_x - nWidth/2 + 2, m_y - nHeight + 2, m_sToolTip);
        DrawRect(&rcToolTip, 3333, 6666);

        m_Font.SetColor(a); 
    }
}

void CDProgress::AddToolTip(LPSTR sToolTip)
{
    if(strlen(sToolTip) >= 30)
    {
        MessageBox(NULL, "Error", "String too long", MB_OK);
        return;
    }
    strcpy(m_sToolTip, sToolTip);

    if(sToolTip && sToolTip[0] != 0)
    m_bShowToolTip = true;
}

void CDProgress::OnMouseLeave()
{
    m_bInRect = false;
}

void CDProgress::SetProgress(int nProgress)
{
	if(nProgress > 100)
		nProgress = 100;
	if(nProgress < 0)
		nProgress = 0;
	m_nProgress = nProgress;
}

void CDProgress::SetBkBmp(LPSTR sFileName)
{
	m_bShowBmp = m_bmp.Load(sFileName) == TRUE;
}

void CDProgress::SetMaskColor(int nR, int nG, int nB)
{
	m_nColorMask = g_RGB(nR, nG, nB);
}

void CDProgress::SetStep(int nStep)
{
	m_nStep = nStep;
}
