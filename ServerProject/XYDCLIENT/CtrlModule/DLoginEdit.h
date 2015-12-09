/****************************************************************
//  FileName:   DEdit.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG 0925
****************************************************************/

#pragma once
#include "dface.h"

//const int CURSOR_FLASHCOUNT	= 10;		// 编辑光标多少个循环闪一次
#define MAX_LEN 1024
#define IID_CDLoginEdit 0x1234abcd

class CDLoginEdit :
	public CDFace
{
private:
	char  m_cBuff[MAX_LEN];
	unsigned int m_nCurPos;
	unsigned int m_nLen;
	unsigned int m_nMaxLen;
    unsigned int m_InputState;

	// 扩展后的东西
	unsigned int m_nMaxWriteLen;
	unsigned int m_nShowCur;
	unsigned int m_nNowPos;

	bool  m_bShowCursor;
	bool  m_bShiftDown;
	bool  m_bMask;

	int   m_nEnd, m_nFirst, m_nLast;
	bool  m_bShowBord;
//	int	  m_nBkColor;
    int   m_nCursorClr;
    bool  m_bNumOnly;
	DWORD m_dwNumMax;

    int   m_nR, m_nG, m_nB;
    int   m_iFlashCount;

    bool  m_bEnable;    

    bool  m_bCapture;

	char  m_sToolTip[31];
	bool  m_bShowToolTip, m_bInRect;
	int   m_x, m_y;

public:
	CDLoginEdit(int fontsize = 16);
	~CDLoginEdit(void);  

	virtual	int		InitByBinary(LPARAM lParam);								// 使用二进制数据来初始化窗口数据
	virtual	int		OnCreate(LPARAM lParam);								// 该函数在窗口被创建的时候自动调用
	virtual	int		KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);		// 键盘鼠标的输入消息传递
	virtual	void	Update();													// 数据更新，包括画该画的东西
    virtual	void	UpdateTip();	
	void			AddToolTip(LPSTR sToolTip);
    virtual void    OnMouseLeave();                                     // 鼠标移出触发  

    virtual bool    SetParameter(LPCSTR szParameterName, LPARAM lParam);
    
    void			DrawRect(RECT* rc, int nClrLT/*左和上边框的颜色*/, int nClrRB/*右和下边框的颜色*/); // 以指定颜色画3d效果的框
	void			Select(int nFirst, int nLast);                              // 选择从位置nFirst到nLast的字符串
	void			Delete(int nFirst, int nLast);                              // 删除从位置nFirst到nLast的字符串
	void			SetMask(bool bMask = true) { m_bMask = bMask;};             // 设置是否用密码形式显示
	char*			GetString() { return m_cBuff;};                             // 返回输入的字符串
	void			SetTextColor(int nR, int nG, int nB);                       // 设置字体颜色
    unsigned int    GetStrLen() { return m_nLen + 1;};
//	void			SetBkColor(int nClr) { m_nBkColor = nClr;};                 // 设置背景颜色
	void			SetShowBord(bool bShowBord = true) {m_bShowBord = bShowBord;}; // 是否显示边框
	void			Clear() { m_nLen = 0;m_nCurPos = 0; memset(m_cBuff, 0, MAX_LEN);};//清除
	bool			AddChar(int nChar);                                         // 添加字符
    void            SetCursorClr(int nClr) { m_nCursorClr = nClr; };            // 设置光标颜色
    void            SetNumOnly(bool bNum = false);								// 设置是否只输入数字
    void            SetString(char *pBuff, long size);
    void            SetEnable(bool bEnable) {m_bEnable = bEnable;};
    void            SetMaxLen(int nLen) { if(nLen < MAX_LEN) m_nMaxLen = nLen;};

	void			SetRECT(int left, int top, int right, int bottom);
	void			SetString(DWORD dwNum);
	void			SetNumMax(DWORD dwNumMax);
	void			ShowPosmark();

    void SetFontSize(int size = 12);

private:
    int             m_FirstChar;
};
