/****************************************************************
//  FileName:   DDialog.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG 0925
****************************************************************/

#pragma once
#include "Dwnd.h"
#include "NetWorkModule\ItemTypedef.h"

class CDDialog :
	public CDWnd
{
private:
	int  m_x0, m_y0;
	int  m_xWidth;
	int  m_yHeight;

	bool m_bEnableMove;	
	int  m_nWndClr;
public:
	bool m_bStartMove;
	CDDialog(void);
	~CDDialog(void);
	virtual	int		InitByBinary(LPARAM lParam);							// ʹ�ö�������������ʼ����������
	virtual	int		OnCreate(LPARAM lParam);								// �ú����ڴ��ڱ�������ʱ���Զ�����
	virtual	int		KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);	// ��������������Ϣ����
	virtual	void	Update();												// ���ݸ��£��������û��Ķ���
	void			DrawRect(Canvas* pCanvas, RECT* rc, int nClrLT = g_RGB(222,222,222)/*����ϱ߿����ɫ*/, int nClrRB = g_RGB(155, 155, 155)/*�Һ��±߿����ɫ*/); // ��ָ����ɫ��3dЧ���Ŀ�
	void			SetEnableMove(bool bEnableMove = true) { m_bEnableMove = bEnableMove;}; // �����Ƿ������϶�
    void            ShowEffect(SItemBase *sItemBase, int x, int y, int iAlpla);   // �����ĳ�����Ʒ������Ч���緢����Щ  
public:
	GuiEvent		EventMouseMove;
	GuiEvent		EventLButtonDown;
	GuiEvent		EventLButtonUp;
};

