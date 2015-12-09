/****************************************************************
//  FileName:   DStatic.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
****************************************************************/

#pragma once
#include "dedit.h"

const __int32 IID_CDSTATIC = 0x112cb0eb; 


//#ifdef _DEBUG
//#include <crtdbg.h>
//using namespace std;
//#undef _AFX_NO_DEBUG_CRT
//#define new new (_NORMAL_BLOCK, __FILE__, __LINE__)
//#endif _DEBUG



class CDStatic :
	public CDFace
{
#define MAX_TEXTCOLOR 10

private:
    //char            m_cBuff[MAX_TEXTCOLOR][512];
	std::string		m_strBuff;
	std::vector< std::string > m_strBufferList;
    DWORD           m_dwClr;

	int				m_nFontLineWidth, m_nFontLineHeight;
	int             m_textHeiht,m_drawpos;


	class CDSlideBar		*m_pSlideBar;
	int				m_nLineBegin;
	bool			m_bHaveSpecialChar;
	bool			m_bHaveSlideBar;
	bool			m_bHoldSlideBarPos;

public:
	GuiEvent		m_MouseMoveEvent;
	GuiEvent		m_MouseEnterEvent;
	GuiEvent		m_MouseLeaveEvent;
private:
	bool			m_bMouseEnter;
public:
	CDStatic(int fontsize = 12);
	~CDStatic(void);

	virtual bool    SetParameter( LPCSTR szParameterName, LPARAM lParam );

	int             GetTexHeight();
	virtual	int		InitByBinary(LPARAM lParam);								// 使用二进制数据来初始化窗口数据
	virtual	int		OnCreate(LPARAM lParam);								    // 该函数在窗口被创建的时候自动调用
	virtual	int		KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);		// 键盘鼠标的输入消息传递
	virtual	void	Update();													// 数据更新，包括画该画的东西
    virtual void    OnMouseLeave();

	void            AddTex( std::string );
	void            ClearTex();
	void			SetText(LPSTR sText, bool bHaveSpecialChar = false);                // 设置静态文本
	void			SetText(DWORD dNum);                                        // 设置静态文本，内容为传入的数字
    void            SetText(int   nNum);
	char*			GetText();//m_cBuff[0];};
	void			SetTextColor(int nR, int nG, int nB);                       // 设置字体颜色
	void			SetTextColor( DWORD color);									// 设置字体颜色
    void            SetTextLine(int nWidth, int nHeight);                       // 设置换行高度和宽度
	void			SetHoldSlideBarPos() { m_bHoldSlideBarPos = true;};
	/*void			SetSliderBarSpr(char* fileback, char* filenormal, char* fileactive, char* filedown);*/

	DECLARE_MY_MESSAGE_MAP()
	int				OnPosChanged();
};
