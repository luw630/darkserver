/****************************************************************
//  FileName:   DButton.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG 0925
****************************************************************/

#pragma once
#include <Bitmap.h>
#include <Engine.h>
#include <WavSound.h>
#include "dface.h"

const __int32 IID_CDCOMNMENU = 0x2233fcb0; 

class CDComnMenu :
	public CDFace
{
private:
	bool			m_bEnable;
	bool			m_bLDown;

    bool            m_bShowToolTip;
    bool            m_bInRect;
    int             m_nWidth, m_nHeight;

    char            m_sItemTextp[50][10];
    void            (*OnItem)();
public:
    static WavSound m_ClickEffect;

public:
	CDComnMenu(void);
	~CDComnMenu(void);
	virtual	int	 InitByBinary(LPARAM lParam);								// 使用二进制数据来初始化窗口数据
	virtual	int	 OnCreate(LPARAM lParam);								    // 该函数在窗口被创建的时候自动调用
	virtual	int	 KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);		// 键盘鼠标的输入消息传递
	virtual	void Update();													// 数据更新，包括画该画的东西
    virtual void UpdateTip();

    virtual int  OnClicked();
    virtual void OnMouseLeave();

    void         AddMenuItem(char* sMenuText, void (*OnItem)(), char* sToolTip = NULL);
    void         Disable(int nIndex);
};
