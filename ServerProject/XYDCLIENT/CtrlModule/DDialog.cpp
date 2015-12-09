/****************************************************************
//  FileName:   DDialog.cpp
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
****************************************************************/
#include "StdAfx.h"

#include "ddialog.h"
#include "ResManagerModule\resmanagermodule.h"
#include "ResManagerModule\resmanagermodule.h"

CDDialog::CDDialog(void)
{
	m_nWndClr = g_RGB(172,172, 272);
	m_bEnableMove = true;
	m_bStartMove = false;
}

CDDialog::~CDDialog(void)
{
}

int	CDDialog::InitByBinary(LPARAM lParam)
{
	return 0;
}

int		CDDialog::OnCreate(LPARAM lParam)
{
	return 1;
}

int CDDialog::KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y)
{
	RECT rc = GetRECT();
	ClientToScreen(&rc);
	POINT pt;
	pt.x = x;
	pt.y = y;

	if( iKeyState== MS_MOVE )
	{	
		if( m_bStartMove ){
			OnMoveTo(x - m_x0, y - m_y0);
		}
		else {
			MouseEventArgs arg(x,y);
			EventMouseMove.Invoke(this,&arg);
		}
	}
	else if(iKeyState == MS_LDOWN)
	{
		if(PtInRect(&rc, pt))
		{ 
			MouseEventArgs arg(x,y);
			if(!EventLButtonDown.Invoke(this,&arg) && m_bEnableMove){
				SetCapture();
				m_x0 = x - rc.left;
				m_y0 = y - rc.top;
				m_bStartMove = true;
			}
		}
	}
	else if(iKeyState == MS_LUP)
	{
		if(m_bStartMove)
		{
			ReleaseCapture();
			m_bStartMove = false;
		}
		else
		{
			MouseEventArgs arg(x,y);
			EventLButtonUp.Invoke(this,&arg);
		}
	}

	return RET_NONE;
}

void CDDialog::Update()
{
	RECT rc = GetRECT();
	ClientToScreen(&rc);
	g_pCanvas->Clear(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, m_nWndClr);
	DrawRect(g_pCanvas, &rc);
}

void CDDialog::DrawRect(Canvas* pCanvas, RECT* rc, int nClrLT, int nClrRB)
{
	g_pCanvas->DrawLine(rc->left, rc->top, rc->right, rc->top, nClrLT);
	g_pCanvas->DrawLine(rc->left, rc->top, rc->left, rc->bottom, nClrLT);
	g_pCanvas->DrawLine(rc->right, rc->top, rc->right, rc->bottom , nClrRB);
	g_pCanvas->DrawLine(rc->left, rc->bottom, rc->right, rc->bottom, nClrRB);	
}
