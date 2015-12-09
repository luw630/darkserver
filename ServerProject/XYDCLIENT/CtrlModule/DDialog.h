/****************************************************************
//  FileName:   DDialog.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG 0925
****************************************************************/

#pragma once
#include "Dwnd.h"
#include "NetWorkModule\ItemTypedef.h"

class CDDialog :
	public CDWnd
{
private:
	int  m_x0, m_y0;
	int  m_xWidth;
	int  m_yHeight;

	bool m_bEnableMove;	
	int  m_nWndClr;
public:
	bool m_bStartMove;
	CDDialog(void);
	~CDDialog(void);
	virtual	int		InitByBinary(LPARAM lParam);							// 使用二进制数据来初始化窗口数据
	virtual	int		OnCreate(LPARAM lParam);								// 该函数在窗口被创建的时候自动调用
	virtual	int		KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);	// 键盘鼠标的输入消息传递
	virtual	void	Update();												// 数据更新，包括画该画的东西
	void			DrawRect(Canvas* pCanvas, RECT* rc, int nClrLT = g_RGB(222,222,222)/*左和上边框的颜色*/, int nClrRB = g_RGB(155, 155, 155)/*右和下边框的颜色*/); // 以指定颜色画3d效果的框
	void			SetEnableMove(bool bEnableMove = true) { m_bEnableMove = bEnableMove;}; // 设置是否允许拖动
    void            ShowEffect(SItemBase *sItemBase, int x, int y, int iAlpla);   // 面板中某点的物品产生特效，如发光这些  
public:
	GuiEvent		EventMouseMove;
	GuiEvent		EventLButtonDown;
	GuiEvent		EventLButtonUp;
};

