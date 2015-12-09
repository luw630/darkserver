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
#include "dcomnmenu.h"
#include "Colors.h"

WavSound CDComnMenu::m_ClickEffect;

CDComnMenu::CDComnMenu(void) : CDFace(IID_CDCOMNMENU)
{
	m_bEnable = true;
	m_bLDown = false;
	m_bCapture = false;
    m_bShowToolTip = false;
    m_bInRect = false;
    m_nWidth = 120, m_nHeight = 70;
}

CDComnMenu::~CDComnMenu(void)
{
}

int	 CDComnMenu::InitByBinary(LPARAM lParam)
{

	return 0;
}

int  CDComnMenu::OnCreate(LPARAM lParam)
{
	if(!lParam)
		return 0;
	return 1;
}

int CDComnMenu::KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y)
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

	return nRet;
}

//按钮点击的时候触发此函数
int CDComnMenu::OnClicked()
{
	return GetParent()->OnMyMessage(this, 0);
}

//鼠标移出窗口（控件）矩形框（在Create中设置的）时触发
void  CDComnMenu::OnMouseLeave()
{
    m_bInRect = false;
}

void CDComnMenu::Update()
{
}

void CDComnMenu::UpdateTip()
{
    //RECT rcToolTip;
    //rcToolTip.left = m_x - m_nWidth/2, rcToolTip.right = m_x + m_nWidth/2, rcToolTip.top = m_y - m_nHeight, rcToolTip.bottom = m_y;
    //if(m_bShowToolTip && m_bInRect)
    //{
    //    g_pCanvas->ClearAlpha(m_x - m_nWidth/2, m_y - m_nHeight, m_nWidth, m_nHeight, g_RGB(255,255,255), 16);
    //    m_Font.SetLine(m_nWidth - 10, 15);
    //    int a =m_Font.GetColor();
    //    m_Font.SetColor(g_RGB(222,2,2));
    //    m_Font.DrawText(m_x - m_nWidth/2, m_y - m_nHeight, m_pToolTip);
    //    DrawRect(&rcToolTip, 3333, 6666);
    //    m_Font.SetColor(a);
    //}
}

void CDComnMenu::AddMenuItem(char* sMenuText, void (*OnItem)(), char* sToolTip)
{
}

void CDComnMenu::Disable(int nIndex)
{
}

