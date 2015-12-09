/****************************************************************
//  FileName:   DCheckButton.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG 0925
****************************************************************/

#pragma once

#include "dface.h"
#include <bitmap.h>

const __int32 IID_CDCHECKBUTTON = 0x112339b0; 

class CDCheckButton :

	public CDFace
{
private:
	bool            m_bChecked;
	bool            m_bCapture;
	int	            m_nState;//0: disable, 1: normal, 2:normal_up, 3:normal_down, 4:checked, 5:checked_up, 6:checked_down

    bool            m_bLDown;
    Bitmap          m_bmp[7];
    bool            m_bEnable;
    int             m_x, m_y;
	char            m_sBmpFile[128];
    char            m_sToolTip[30];
    bool            m_bShowToolTip;
    bool            m_bInRect;
    int             m_nWidth, m_nHeight; //of tooltip

public:
	struct SParameter 
	{
		LPSTR lpszBmpFileName[7];
	};

	CDCheckButton(void);
	~CDCheckButton(void);

	virtual	int		OnCreate(LPARAM lParam);								// 该函数在窗口被创建的时候自动调用
	bool			GetChecked(){return m_bChecked;}                        // 返回检查框是否被按下
	virtual int		InitByBinary(LPARAM lParam);                            // 初始化画布画笔,  资源的打开
	virtual	int		KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);  // 键盘鼠标的输入消息传递
	virtual	void	Update();                                               // 数据更新，包括画该画的东西

	void			OnCheck(bool bChecked);                                 // 设置检查框是否按下
    void            AddToolTip(LPSTR sToolTip);                             // 添加消息框提示
    void            SetToolTipWH(int nWidth, int nHeight) {m_nWidth = nWidth, m_nHeight = nHeight;}; // 设置提示框宽和高
};
