/****************************************************************
//  FileName:   DSlideBar.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG 0925
****************************************************************/

#pragma once
#include "dface.h"
#include "Colors.h"

const __int32 IID_CDSLIDEBAR = 0x1122ed9e; 

class CDSlideBar :
	public CDFace
{
private:
	int	 		m_nPos;
	int  		m_nMaxPos;
	int  		m_nMinPos;
	int  		m_xOld, m_yOld;
	int  		m_nBarColor, m_nSlideColor;

	bool 		m_bStartDrag;
	int  		m_x, m_y;
	int  		m_nBtnState; //滑动条的状态
	bool 		m_bShowBmp;  //是否显示的是图片
	bool 		m_bCapture;

	bool		m_bHorizon;	//是否是横向滑动条.
	int			m_nBarWidthHeight;

	Sprite		m_sprBack, m_sprBar[4];//Normal, m_sprBarActive, m_sprBarDown;
	char		m_strBarImg[256];
public:
	struct SParameter 
	{
		int 	nColorBar;
		int 	nColorSlider;
	};

	CDSlideBar(void);
	~CDSlideBar(void);
	virtual	int	InitByBinary(LPARAM lParam);							// 使用二进制数据来初始化窗口数据
	virtual	int	OnCreate(LPARAM lParam);								// 该函数在窗口被创建的时候自动调用
	virtual	int	KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);	// 键盘鼠标的输入消息传递
	virtual	void Update();							                    // 数据更新，包括画该画的东西
    virtual void OnMouseLeave();
    virtual void PaintLevelTop();

	virtual bool SetParameter( LPCSTR szParameterName, LPARAM lParam );
	virtual int OnPosChanging(int nPos);                                // 滑动条位置改变时触发
	void        SetPosition(int nPos, int nMaxPos = 100, int nMinPos = 0);     // 设置滑动条位置，最大值和最小值
	int         GetPosition();                                                 // 返回当前滑动条位置

	void		SetHorizon(bool bHorizon = true); // 设置是否为横向的，否为纵向显示的

	void		SetBePicture(char* szFileName_Back, char* szFileName_Bar_Normal, 
		                     char* szFileName_Bar_Active, char* szFileName_Bar_Down);
	void		SetBePicture(char* szFileName_Back, char* szFileName);

    RECT        GetRcBar();
     bool       m_bShowTip;

};
