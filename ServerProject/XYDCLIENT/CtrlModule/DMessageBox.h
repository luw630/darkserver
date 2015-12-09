/****************************************************************
//  FileName:   DMessageBox.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG 0924
****************************************************************/

#pragma once
#include "DWnd.h"

#include <queue>
#include "pub/dwt.h"

class CDMessageBox :
	public CDWnd
{
private:
	int             m_nWndClr;
	LPFace			m_pBtn1;
    Sprite          m_spr;

    char            m_sSprFileName_Bk[128];
	bool			m_bSetZ;								//是否调整Z坐标
public:
	bool            m_bStartMove;
	CDMessageBox(void);
	~CDMessageBox(void);
	virtual	int	    OnCreate(LPARAM lParam);								// 该函数在窗口被创建的时候自动调用
	virtual	int 	KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);
	virtual	void	Update();
 
    virtual bool    AddChild( GuiObjConverter pGuiObject );                                    // 添加子窗口
    virtual bool    SetParameter(LPCSTR szParameterName, LPARAM lParam);//////////////

    void			DrawRect(Canvas* pCanvas, RECT* rc, int nClrLT = g_RGB(222,222,222), int nClrRB = g_RGB(155, 155, 155));
	void			SetWndClr(int nClr);
	int				MessageBox(LPCTSTR sMsg, int flag = 0);                                 // 显示消息框，参数为显示内容
	void			SetCoordZ( bool setZ) { m_bSetZ = setZ;}								//是否调整Z坐标
	bool			GetCoordZ() { return m_bSetZ;}

	DECLARE_MY_MESSAGE_MAP()
	int OnOK();
	int OnExit();

    std::queue< std::string > m_MessageQueue;
};

