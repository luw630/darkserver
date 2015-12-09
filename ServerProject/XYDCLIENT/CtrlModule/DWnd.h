/****************************************************************
//  FileName:   DWnd.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG 0926
****************************************************************/

#pragma once

#include "DFace.h"

#include "Colors.h"
#include "dButton.h"
#include "dSlideBar.h"
#include "dProgress.h"
#include "DCheckButton.h"
#include "DEdit.h"
#include "dsprbutton.h"
#include "dstatic.h"

#include "dsprcheckbutton.h"
#include "dsprprogress.h"
#include "dprogress.h"
#include "ddropBox.h"
#include "dlistbox.h"


//#ifdef _DEBUG
//#include <crtdbg.h>
//using namespace std;
//#undef _AFX_NO_DEBUG_CRT
//#define new new (_NORMAL_BLOCK, __FILE__, __LINE__)
//#endif _DEBUG



class CDWnd :
	public CDFace
{
private:
	int             m_xWidth;
	int             m_yHeight;

	int             m_nWndClr;

public:
	CDWnd(void); 
	~CDWnd(void);
	virtual	int		InitByBinary(LPARAM lParam);
	virtual	int		OnCreate(LPARAM lParam);								// 该函数在窗口被创建的时候自动调用
	virtual	int		KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);
	virtual	void	Update(/*const RECT *rcParentArea*/);
//	void			AddChild(CDWnd* pWnd);
	void			DrawRect(RECT rc, int nClrLT = g_RGB(222,222,222), int nClrRB = g_RGB(155, 155, 155));
	void			SetWndClr(int nClr);                                    // 设置窗口颜色
};