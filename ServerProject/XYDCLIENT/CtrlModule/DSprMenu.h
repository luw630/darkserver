#pragma once
#include "ctrlmodule\ddialog.h"
//#define _DEBUG_FOR_LUOU_TEAM
class CDSprMenu :
    public CDDialog
{
private:
	//Sprite m_spr;
    CDSprButton  *m_pBtnInfo;
    bool    m_bLDown;
    int     m_nIDTo;
    char    m_szName[11];

    char    m_sSprFileName_Bk[128];
public:
    CDSprMenu(void);
    ~CDSprMenu(void);
	virtual	int		OnCreate(LPARAM lParam);								// 该函数在窗口被创建的时候自动调用
	virtual	int		OnHide();												// 该函数在窗口被销毁的时候自动调用
	virtual	int		InitByBinary(LPARAM lParam);							// 使用二进制数据来初始化窗口数据
	virtual	void	Update();												// 数据更新，包括画该画的东西
	virtual	int		KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);	// 键盘鼠标的输入消息传递
 
    virtual bool    AddChild( GuiObjConverter pGuiObject );                                    // 添加子窗口
    virtual bool	SetParameter(LPCSTR szParameterName, LPARAM lParam);

    void            ShowIt(char* szName, bool bShow = true);
	DECLARE_MY_MESSAGE_MAP()
    int    OnInfo();
};
