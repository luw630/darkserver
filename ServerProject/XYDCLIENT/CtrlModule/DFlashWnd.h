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
#include <FlashPlayer.h>
#include "dface.h"

const __int32 IID_CDFLASHWND = 0x1123ebaf; 

class CDFlashWnd :
	public CDFace
{
public:
    IFlashPlayer    *flash;
    std::string     movie;
    BOOL            rebuild;
    BOOL            trans;
    BOOL            ctrl;
    BOOL            border;
    DWORD           tickSkip;
    DWORD           curSkip;

public:
    static CDFlashWnd *activeFlash; // 当前已经激活的flash，主要用于处理消息事件！
    static CDFlashWnd *prevFlash; // 当前已经激活的flash，主要用于处理消息事件！

public:
	struct SParameter {
        std::string moviefile;
	} m_Param;

	CDFlashWnd(void);
	~CDFlashWnd(void);
	virtual	int	 InitByBinary(LPARAM lParam);								// 使用二进制数据来初始化窗口数据
	virtual	int	 OnCreate(LPARAM lParam);								    // 该函数在窗口被创建的时候自动调用
	virtual	int	 KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);		// 键盘鼠标的输入消息传递
	virtual	void Update();													// 数据更新，包括画该画的东西
    virtual void UpdateTip();
    virtual void OnMouseLeave();
    virtual int	 PtInFace(POINT &pt);
	virtual void Show(bool bShow);

    virtual bool SetParameter(LPCSTR szParameterName, LPARAM lParam);

    void         AddToolTip(char* sToolTip);                               // 添加消息提示
    void         SetToolTipWH(int nWidth, int nHeight) {m_nWidth = nWidth, m_nHeight = nHeight;}; //设置提示框宽高

};
