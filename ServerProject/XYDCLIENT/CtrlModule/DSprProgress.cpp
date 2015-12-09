/****************************************************************
//  FileName:   DSprProgress.cpp
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork 
****************************************************************/
#include "StdAfx.h"
#include "ResManagerModule\DCanvasEx.h"
#include "dsprprogress.h"
#include <Colors.h>
#include <math.h>
#include <string>
extern DCanvasEx        *g_Canvas;          // 公用画布
#define CLR_TIPTEXT g_RGB(222,0,0)
#define CLR_TIPBACK g_RGB(255,255,0)

CDSprProgress::CDSprProgress(void):CDFace(IID_CDSPRPROGRESS)
{
    m_bShowToolTip = false;
    m_bInRect = false;
    m_nWidth = 120, m_nHeight = 70;
    m_bShowText = true;
    m_bHorizon = true;
    m_sBarFile[0] = 0;
    m_sToolTip[0] = 0;

    m_bTrans = false;

    m_sSprFileName_Progress[0] = 0;
    m_sSprFileName_Mask[0] = 0;

	m_nFramesIndex = 1;
	m_nIndexTmp = 0;
    m_nProgress = 0;
    m_nMaxProgress = 1;
    m_bConverse = false; // 默认是正向增加
	m_bClipMode = false; // 默认缩放模式
	m_eDrawMode = DM_CUR_MAX;
	m_bShowTextBG = false;
}

CDSprProgress::~CDSprProgress(void)
{
}

bool CDSprProgress::SetParameter(LPCSTR szParameterName, LPARAM lParam)
{
    if(strcmp(szParameterName, "SprFileName_Progress") == 0)
        dwt::strcpy(m_sSprFileName_Progress, (LPSTR)lParam, 128);
	else if(strcmp(szParameterName, "SprFileName_Mask") == 0)
		dwt::strcpy(m_sSprFileName_Mask, (LPSTR)lParam, 128);
	else if(strcmp(szParameterName, "ClipMode") == 0)
		m_bClipMode = (atoi((LPSTR)lParam) == 1);
    return CDFace::SetParameter(szParameterName, lParam);
}

int	CDSprProgress::OnCreate(LPARAM lParam)
{
    m_spr.Load(m_sSprFileName_Progress);
	m_nFramesIndex = m_spr.GetFrames();
	
    m_sprMask.Load(m_sSprFileName_Mask);
    return 1;
}

int CDSprProgress::InitByBinary(LPARAM lParam)
{
    return 1;
}

int CDSprProgress::KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y)
{
	POINT pt;
	pt.x = x;
	pt.y = y;
	RECT rcWnd = GetRECT();
	ClientToScreen(&rcWnd);
	int iInRegion;

	iInRegion = m_spr.GetPixelAlpha(0, x - rcWnd.left, y - rcWnd.top);
	if(iInRegion)
	{
        //Active();
        m_bInRect = true;
        m_x = x, m_y = y;
    }
    else
    {
        m_bInRect = false;
    }
    return 1;
}

void CDSprProgress::OnMouseLeave()
{
    m_bInRect = false;
}


#define CHAGNESPEED 3

void CDSprProgress::Update()
{
	m_nIndexTmp ++;
	if (m_nIndexTmp >= m_nFramesIndex*CHAGNESPEED)
		m_nIndexTmp = 0;

    RECT rcWnd = GetRECT();
    ClientToScreen(&rcWnd);

//     char sProgress1[32];
//     sprintf(sProgress1, "%d%%", m_nProgress);

	if (m_nMaxProgress <= 0)
		m_nMaxProgress = 1;
	if (m_nProgress < 0)
		m_nProgress = 0;

    RECT rc1, rc2;
    if(!m_bHorizon)
    {
        rc2.left = rcWnd.left, rc2.top = rcWnd.bottom - (rcWnd.bottom - rcWnd.top)*((float)m_nProgress/(float)m_nMaxProgress), rc2.right = rcWnd.right, rc2.bottom = rcWnd.bottom;
    }
    else if(!m_bConverse)
    {
        rc2.left = rcWnd.left, rc2.top = rcWnd.top, rc2.right = rcWnd.left + (rcWnd.right - rcWnd.left)*((float)m_nProgress/(float)m_nMaxProgress), rc2.bottom = rcWnd.bottom;
    }
    else
    {
        rc2.left = rcWnd.right - (rcWnd.right - rcWnd.left)*((float)m_nProgress/(float)m_nMaxProgress), rc2.top = rcWnd.top, rc2.right = rcWnd.right, rc2.bottom = rcWnd.bottom;
    }
	if ( m_bClipMode )
	{
		//g_pCanvas->GetClipRect(&rc1);
		//g_pCanvas->SetClipRect(&rc2);
		//m_spr.DrawAlpha(rcWnd.left, rcWnd.top , (m_nFramesIndex>1)?m_nIndexTmp/CHAGNESPEED:0, m_bTrans?20:32);
		//g_pCanvas->SetClipRect(&rc1);
		//m_spr.Draw(rcWnd.left, rcWnd.top, (m_nFramesIndex>1)?m_nIndexTmp/CHAGNESPEED:0);
		m_spr.DrawProgress(rcWnd.left, rcWnd.top, rc2.right-rc2.left, (m_nFramesIndex>1)?m_nIndexTmp/CHAGNESPEED:0, m_bTrans?0xc0808080:0xff808080);
	}
	else
	{
		m_spr.DrawZoom( &rc2, (m_nFramesIndex>1)?m_nIndexTmp/CHAGNESPEED:0, 0xff7f7f7f );
	}

    if(rc2.right - rcWnd.left > 3)
        m_sprBar.DrawAlpha(rc2.right, rcWnd.top, 0, m_bTrans?20:32);

    m_sprMask.DrawAlpha(rcWnd.left, rcWnd.top, 0, 20);

	if(m_bShowText)
	{
		char szProgress[32];
		m_Font.SetLine(g_pCanvas->m_nWidth, 20);
		void XENGINE_API	g_SetDefaultColor( int i, WORD color );
		g_SetDefaultColor(11, g_RGB(222, 222, 251));

		int nFontWidth = m_Font.GetWidth() / 2;
		int nFontHeight = m_Font.GetHeight();

		int nProLen = 0;

		if (m_eDrawMode == DM_CUR_MAX)
		{
			_snprintf(szProgress, 32, "%d/%d", m_nProgress, m_nMaxProgress);
			if (m_nProgress <= 0) 
				nProLen = 1; 
			else
				nProLen = (int)log10((float)m_nProgress) + 1;

			int textX = (rcWnd.left+rcWnd.right ) / 2 - nFontWidth * nProLen;
			int textY = (rcWnd.top + rcWnd.bottom) / 2 - nFontHeight / 2;
			if (m_bShowTextBG)
				g_Canvas->ClearAlpha(textX - 2, textY - 2, strlen(szProgress) * nFontWidth + 4, nFontHeight + 4, 64, 12);

			std::string fmtText = "%\xb9" + std::string(szProgress);
			m_Font.DrawColorText(textX, textY, fmtText.c_str());
		}
		else if (m_eDrawMode == DM_PERCENT)
		{
			_snprintf(szProgress, 32, "%d%%", m_nProgress);
			nProLen = strlen(szProgress);

			int textX = (rcWnd.left+rcWnd.right ) / 2 - nFontWidth * nProLen;
			int textY = (rcWnd.top + rcWnd.bottom) / 2 - nFontHeight / 2;
			if (m_bShowTextBG)
				g_Canvas->ClearAlpha(textX - 2, textY - 2, nProLen * nFontWidth + 4, nFontHeight + 4, 64, 12);

			std::string fmtText = "%\xb9" + std::string(szProgress);
			m_Font.DrawColorText(textX, textY, fmtText.c_str());
		}
		else if (m_eDrawMode == DM_LABEL)
		{
			nProLen = m_strLabel.length();
			int textX = (rcWnd.left+rcWnd.right ) / 2 - nFontWidth * nProLen / 2;
			int textY = (rcWnd.top + rcWnd.bottom) / 2 - nFontHeight / 2;
			if (m_bShowTextBG)
				g_Canvas->ClearAlpha(textX - 2, textY - 2, nProLen * nFontWidth + 4, nFontHeight + 4, 64, 12);

			std::string fmtText = "%\xb9" + m_strLabel;
			m_Font.DrawColorText(textX, textY, fmtText.c_str());
		}
	}
}

void CDSprProgress::UpdateTip()
{
	if (m_nMaxProgress <= 0)
		m_nMaxProgress = 1;

    int nWidth = dwt::strlen(m_sToolTip, 100) * m_Font.GetWidth()/2 + 4;
    int nHeight =  m_Font.GetHeight() + 4;

    RECT rcToolTip;
    rcToolTip.left = m_x - nWidth/2; rcToolTip.top = ((m_y - nHeight) < 3?3:(m_y - nHeight)); rcToolTip.right = m_x + nWidth/2; rcToolTip.bottom = rcToolTip.top + nHeight;

    RECT rcWnd = GetRECT();
    ClientToScreen(&rcWnd);
    POINT pt;
    GetCurrentPos(pt);
    if (PtInRect(&rcWnd, pt))
    if(m_bShowToolTip && m_bInRect)
    {
        //g_pCanvas->ClearAlpha(m_x - nWidth/2, (m_y - nHeight) < 3?3:(m_y - nHeight), nWidth, nHeight, CLR_TIPBACK, 8);
        g_pCanvas->Clear(m_x - nWidth/2, (m_y - nHeight) < 3?3:(m_y - nHeight), nWidth, nHeight, CLR_TIPBACK);
        int w = m_Font.GetLineWidth();
        int h = m_Font.GetLineHeight();

        int a =m_Font.GetColor();
        m_Font.SetLine(200, 20);
        m_Font.SetColor(CLR_TIPTEXT);
        m_Font.DrawText(m_x - nWidth/2 + 2, (m_y - nHeight)<3?3:(m_y - nHeight) + 2, m_sToolTip);
        DrawRect(&rcToolTip, 3333, 6666);

        m_Font.SetColor(a);
        m_Font.SetLine(w, h);
    }


    //RECT rcToolTip;
    //rcToolTip.left = m_x + 15, rcToolTip.right = m_x + m_nWidth + 15, rcToolTip.top = m_y + 15 + m_nHeight, rcToolTip.bottom = m_y  + 15 + m_nHeight;
    //if(m_bShowToolTip && m_bInRect)
    //{
    //    g_pCanvas->ClearAlpha(m_x + 15, m_y + 15, m_nWidth, m_nHeight, g_RGB(255,255,255), 16);
    //    m_Font.SetLine(m_nWidth - 10, 15);
    //    int a =m_Font.GetColor();
    //    m_Font.SetColor(g_RGB(222,2,2));
    //    m_Font.DrawText(m_x + 15, m_y + 16, m_sToolTip);
    //    DrawRect(&rcToolTip, 3333, 6666);
    //    m_Font.SetColor(a);
    //}
}

void CDSprProgress::AddToolTip(LPSTR sToolTip)
{
    if(strlen(sToolTip) >= 30) return;
    strcpy(m_sToolTip, sToolTip);
    if(sToolTip && sToolTip[0] != 0)
    m_bShowToolTip = true;
}

void CDSprProgress::SetBar(LPSTR sBarFile)
{
    if(strlen(sBarFile) >= 128) return;
    strcpy(m_sBarFile, sBarFile);
    m_sprBar.Load(sBarFile);
}

void CDSprProgress::SetProgress(int nProgress, int nMaxProgress)
{
    m_nProgress  = nProgress;
    if(nProgress > nMaxProgress)
        m_nProgress = nMaxProgress;
    if(nProgress < 0)
        m_nProgress = 0;

    m_nMaxProgress= nMaxProgress;

	if (m_nMaxProgress <= 0)
		m_nMaxProgress = 1;
}

void CDSprProgress::SetMaskBmp(LPSTR sMaskFile)
{
    m_sprMask.Load(sMaskFile);
}
