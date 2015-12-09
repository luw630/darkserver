/****************************************************************
//  FileName:   DWnd.cpp
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
****************************************************************/

#include "StdAfx.h"
#include "dwnd.h"

CDWnd::CDWnd(void) : CDFace(0)
{
	m_nWndClr = g_RGB(172,172, 272);
}

CDWnd::~CDWnd(void)
{
}

int CDWnd::InitByBinary(LPARAM lParam)
{
	return 0;
}

int CDWnd::KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y)
{
	return RET_NONE;
}

void CDWnd::Update()
{
	RECT rcWnd = GetRECT();
	ClientToScreen(&rcWnd);
	g_pCanvas->Clear(rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, m_nWndClr);
}


void CDWnd::SetWndClr(int nClr)
{
	m_nWndClr = nClr;
}

void CDWnd::DrawRect(RECT rc, int nClrLT, int nClrRB)
{
	assert(g_pCanvas != NULL);
	{
		g_pCanvas->DrawLine(rc.left, rc.top, rc.right, rc.top, nClrLT);
		g_pCanvas->DrawLine(rc.left, rc.top, rc.left, rc.bottom, nClrLT);
		g_pCanvas->DrawLine(rc.right, rc.top, rc.right, rc.bottom , nClrRB);
		g_pCanvas->DrawLine(rc.left, rc.bottom, rc.right, rc.bottom, nClrRB);	
	}
}

int  CDWnd::OnCreate(LPARAM lParam)
{
	return 1;
}
