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
	bool			m_bSetZ;								//�Ƿ����Z����
public:
	bool            m_bStartMove;
	CDMessageBox(void);
	~CDMessageBox(void);
	virtual	int	    OnCreate(LPARAM lParam);								// �ú����ڴ��ڱ�������ʱ���Զ�����
	virtual	int 	KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);
	virtual	void	Update();
 
    virtual bool    AddChild( GuiObjConverter pGuiObject );                                    // ����Ӵ���
    virtual bool    SetParameter(LPCSTR szParameterName, LPARAM lParam);//////////////

    void			DrawRect(Canvas* pCanvas, RECT* rc, int nClrLT = g_RGB(222,222,222), int nClrRB = g_RGB(155, 155, 155));
	void			SetWndClr(int nClr);
	int				MessageBox(LPCTSTR sMsg, int flag = 0);                                 // ��ʾ��Ϣ�򣬲���Ϊ��ʾ����
	void			SetCoordZ( bool setZ) { m_bSetZ = setZ;}								//�Ƿ����Z����
	bool			GetCoordZ() { return m_bSetZ;}

	DECLARE_MY_MESSAGE_MAP()
	int OnOK();
	int OnExit();

    std::queue< std::string > m_MessageQueue;
};

