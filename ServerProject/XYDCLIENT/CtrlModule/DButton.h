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

const __int32 IID_CDBUTTON = 0x1122ebaf; 

class CDButton :
	public CDFace
{
private:
    Bitmap			m_bmp[4]; 	
	int				m_x;
	int				m_y;
	bool			m_bCapture;

    char            m_pToolTip[30];
	int				m_nStateCount;

	int				m_nBtnState;//0: disable, 1: normal, 2:up, 3:down, 4:down_down, 5:down_up  //, 6:down_focus,
	BOOL			m_bText;//have text
	bool			m_bEnable;
	bool			m_bLDown;

    bool            m_bShowToolTip;
    bool            m_bInRect;
    int             m_nWidth, m_nHeight;

	void			DrawBmp(int nBmpIndex);
public:
    static WavSound m_ClickEffect;
	GuiEvent		m_ClickEvent;
public:
	struct SParameter
	{
		int iType;                                                          // 暂没用
		LPSTR lpszBmpFileName[4];                                           // 图片文件名数组，Bitmap格式
        char sBmpFileName[4][256];

		LPSTR lpszBtnCaption;                                               // 暂没用
		int nBordClrLT;                                                     // 暂没用
		int nBordClrRB;                                                     // 暂没用
	} m_Param;

	CDButton(void);
	~CDButton(void);
	virtual	int	 InitByBinary(LPARAM lParam);								// 使用二进制数据来初始化窗口数据
	virtual	int	 OnCreate(LPARAM lParam);								    // 该函数在窗口被创建的时候自动调用
	virtual	int	 KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);		// 键盘鼠标的输入消息传递
	virtual	void Update();													// 数据更新，包括画该画的东西
    virtual void UpdateTip();
	void		 Enable(bool bEnable = true);
    bool		 isEnable() { return m_bEnable; }
	virtual int  OnClicked();
    virtual void OnMouseLeave();

    virtual bool    SetParameter(LPCSTR szParameterName, LPARAM lParam);

    void         AddToolTip(char* sToolTip);                               // 添加消息提示
    void         SetToolTipWH(int nWidth, int nHeight) {m_nWidth = nWidth, m_nHeight = nHeight;}; //设置提示框宽高

};
