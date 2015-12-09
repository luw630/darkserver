/****************************************************************
//  FileName:   DSprCheckButton.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG 0926
****************************************************************/

#pragma once
#include "dface.h"

const __int32 IID_CDSPRCHECKBUTTON = 0x1152a982; 

class CDSprCheckButton :
	public CDFace
{
private:
	bool            m_bChecked;
	bool            m_bLDown;
	int	            m_nState;   //0: disable, 1: normal, 2:normal_up, 3:normal_down, 4:checked, 5:checked_up, 6:checked_down
//	int	            m_bdisable; //0: disable, 1: normal, 2:normal_up, 3:normal_down, 4:checked, 5:checked_up, 6:checked_down
	Sprite          m_bmp[7];
	char            m_sBmpFile[128];


    int             m_x, m_y;
    bool            m_bShowToolTip;

	char            m_sToolTip[5][30];
	int				m_nTipWidth, m_nTipHeight;
	int				m_nLines;

    bool            m_bInRect;
    int             m_nWidth, m_nHeight;
    bool            m_bEnable;
	DWORD			m_dwColor;
	bool			m_bUseColor;
    int             m_nMouseInTime;         // 鼠标停留时间
    WORD            m_nMonseInTimeMax;      // 停留最大值

public:
	struct SParameter 
	{
		LPSTR lpszBmpFileName[7];
        char sBmpFileName[7][256];
	} m_Param;

	CDSprCheckButton(void);
	~CDSprCheckButton(void);

	virtual	int		OnCreate(LPARAM lParam);								// 该函数在窗口被创建的时候自动调用
	bool			GetChecked(){return m_bChecked;}
	virtual int		InitByBinary(LPARAM lParam);                            // 初始化画布画笔,  资源的打开
	virtual	int		KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);  // 键盘鼠标的输入消息传递
	virtual	void	Update();                                               // 数据更新，包括画该画的东西
    virtual void    OnMouseLeave();                                         // 鼠标移出触发    
    virtual	void    UpdateTip();	

    virtual bool    SetParameter(LPCSTR szParameterName, LPARAM lParam);

	bool			IsEnable();
    void            Enable(bool bEnable = true);    //
    void            SetDisable() { m_bEnable = false; m_nState = 0;};
	void			SetCheck(bool bCheck);                                  // 设置检查框是否为按下状态
	void			OnCheck(bool bChecked);                                 // 点击后被触发
    void            AddToolTip(LPSTR sToolTip);                             // 添加消息提示
    void            SetToolTipWH(int nWidth, int nHeight) {m_nTipWidth = nWidth, m_nTipHeight = nHeight;}; // 设置提示框宽高
	void			SetCheckWithColor(bool bCheck = false, DWORD color = 0);
    void            SetMonseInTimeMax( WORD nMonseInTimeMax = 25 ){ m_nMonseInTimeMax = nMonseInTimeMax; };// 默认50次==1秒
    WORD            GetMonseInTimeMax(){ return m_nMonseInTimeMax; };
	void			GetToolTip(char * tipstr);
};
