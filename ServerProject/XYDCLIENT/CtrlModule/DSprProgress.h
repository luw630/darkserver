/****************************************************************
//  FileName:   DSprProgress.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG 0926 
****************************************************************/

#pragma once
#include "dface.h"

const __int32 IID_CDSPRPROGRESS = 0x116b0c83; 

class CDSprProgress :
    public CDFace
{
public:
	// 显示模式
	enum DRAW_MODE
	{
		DM_CUR_MAX,			// curProgress/maxProgress
		DM_PERCENT,				// progress%
		DM_LABEL,					// label
	};

private:
    Sprite          m_spr;
    bool            m_bLeft;
    int             m_bClr;
    int             m_nProgress, m_nMaxProgress;
    int             m_nDia;
    int             m_nCurTime;
	bool	        m_bHorizon;

    int             m_x, m_y;
    bool            m_bShowToolTip;
    bool            m_bInRect;
    int             m_nWidth, m_nHeight;

    bool            m_bShowText; 
    char            m_sToolTip[30];
    char            m_sBarFile[128];
    Sprite          m_sprBar, m_sprMask;

    char            m_sSprFileName_Progress[128], m_sSprFileName_Mask[128];

    bool            m_bTrans;
	int				m_nFramesIndex, m_nIndexTmp;
    bool            m_bConverse; // 是否逆向增加
	bool            m_bClipMode;

	DRAW_MODE			m_eDrawMode;

	// 标签文字
	std::string	m_strLabel;

	// 显示文字的时候，是否需要显示底图
	bool		m_bShowTextBG;

public:
    CDSprProgress(void);
    ~CDSprProgress(void);
    virtual bool SetParameter(LPCSTR szParameterName, LPARAM lParam);
	virtual	int OnCreate(LPARAM lParam);								// 该函数在窗口被创建的时候自动调用
	virtual int InitByBinary(LPARAM lParam);
	virtual	int KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);
	virtual	void Update();
    virtual void OnMouseLeave();                                     // 鼠标移出触发  
    virtual void UpdateTip();
    void SetProgress(int nProgress, int nMaxProgress = 100); // 设置进度条进度，和最大值
    void AddToolTip(LPSTR sToolTip);                         // 添加提示框
    void SetToolTipWH(int nWidth, int nHeight) {m_nWidth = nWidth, m_nHeight = nHeight;}; // 设置提示框宽高
    bool isToolTipShow() { return m_bInRect;};               // 提示框是否可见
    void SetShowTip(bool bShowIt) { m_bInRect = bShowIt;};   // 设置是否显示提示框
    void SetShowText(bool bShowText) { m_bShowText = bShowText;};
   	void SetHorizon(bool bHorizon = true) { m_bHorizon = bHorizon;}; // 设置是否为横向的，否为纵向显示的
    void SetBar(LPSTR sBarFile);

    void SetMaskBmp(LPSTR sMaskFile); //主要是为了适应显示进度条上的高光效果
    void SetBeTrans(bool bTrans) { m_bTrans = bTrans;};
    void SetConverse(bool converse = true) { m_bConverse = converse; }

	void	SetDrawMode(DRAW_MODE eDrawMode) { m_eDrawMode = eDrawMode;};
	void	EnableShowTextBG(bool bEnable) { m_bShowTextBG = bEnable; }

	void	SetLabel(const char* label) { m_strLabel = label; }
};
