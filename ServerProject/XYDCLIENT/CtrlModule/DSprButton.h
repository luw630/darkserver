/****************************************************************
//  FileName:   DSprButton.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG 0925
****************************************************************/

#pragma once
#include "dface.h"

const __int32 IID_CDSPRBUTTON = 0x113b225d; 

class CDSprButton :
	public CDFace
{
private:
    Sprite          m_bmp[4]; 
	
	int				m_x;
	int				m_y;
	bool			m_bCapture;
    bool            m_bLDown;

	char			m_cKey;
	int				m_nStateCount;

	int				m_nBtnState;//0: disable, 1: normal, 2:up, 3:down, 4:down_down, 5:down_up  //, 6:down_focus,
	BOOL			m_bText;//have text
	bool			m_bEnable;
	bool			m_bEnableMove;
	bool			m_bDrage;


	char			m_sBmpFile[128];
    char            m_sToolTip[5][30];
    bool            m_bShowToolTip;
	int				m_nTipWidth, m_nTipHeight;
	int				m_nLines;
    bool            m_bInRect;

	bool			m_bFlash;
	int				m_nBmpIndex;

    LPSTR           m_SprName;

	void			DrawBmp( int nBmpIndex, BYTE alpha = 32 );
    DWORD           nameFontColor;
    char            m_szBtnName[256];
    WORD            btnNameX;
    WORD            btnNameY;
    int             m_nMouseInTime;
    WORD            m_nMonseInTimeMax;      // 停留最大值

public:
	GuiEvent		m_LDownPrepareEvent;
	struct SParameter
	{
		int iType;
		char sBmpFileName[4][256];
        //LPSTR lpszBmpFileName[4];

		LPSTR lpszBtnCaption;
		int nBordClrLT;
		int nBordClrRB;
	} m_Param;

	CDSprButton(void);
	~CDSprButton(void);
	virtual	int	 InitByBinary(LPARAM lParam);								// 使用二进制数据来初始化窗口数据
	virtual	int	 OnCreate(LPARAM lParam);								    // 该函数在窗口被创建的时候自动调用
	virtual	int	 KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);		// 键盘鼠标的输入消息传递
	virtual	void Update();													// 数据更新，包括画该画的东西
    virtual	void UpdateTip();	

    virtual bool    SetParameter(LPCSTR szParameterName, LPARAM lParam);

    void        SetNewParam(LPARAM lNewParam);
	void		GetParam(SParameter* param) { memcpy(param, &m_Param, sizeof (SParameter));};
    void        AdjustRect();

    void         SetBtnName( LPCSTR name );
    void         SetBtnNameColor( BYTE r, BYTE g, BYTE b ) { nameFontColor = ( r << 16 | g << 8 | b ); };
    void         SetBtnNamePos( WORD x, WORD y ) { btnNameX = x; btnNameY = y; }
	void         SetHotKey(char cKey);                                      // 暂未用
	void         SetDragMove(bool bEnableMove = false);                     // 设置是否允许拖动
	void		 SetFlash(bool bFlash = false) { m_bFlash = bFlash;};
	void         Enable(bool bEnable = true);                               // 能否用
    bool         isEnable() { return m_bEnable; }                           // 能否用
	virtual int  OnClicked();                                               // 鼠标点击触发
    virtual void OnMouseLeave();                                            // 鼠标移出触发

    void         AddToolTip(LPSTR sToolTip);
    void         SetToolTipWH(int nWidth, int nHeight) {};
    void         SetMonseInTimeMax( WORD nMonseInTimeMax = 25 ){ m_nMonseInTimeMax = nMonseInTimeMax; };
    WORD         GetMonseInTimeMax(){ return m_nMonseInTimeMax; };
    void         PaintButtonTopSpr();
	int			 GetBtnState(){return m_nBtnState;};

	GuiEvent	 m_ClickEvent;
};
