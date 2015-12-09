/****************************************************************
//  FileName:   DProgress.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG 0924
****************************************************************/

#pragma once
#include <Bitmap.h>
#include "dface.h"
#include <Colors.h>

const __int32 IID_CDPROGRESS = 0x11232edb; 


class CDProgress :
	public CDFace
{
private:
	Bitmap  m_bmp;
	bool    m_bShowBmp;
	int		m_nStep;
	int		m_nProgress;
	bool	m_bHorizon;
	int		m_nColorBk, m_nColorStart, m_nColorEnd, m_nColorMask;

    char    m_sToolTip[30];
    bool    m_bShowToolTip;
    bool    m_bInRect;
    int     m_nWidth, m_nHeight;
    int     m_x, m_y;

public:
	CDProgress(void);
	~CDProgress(void);
	virtual	int	    OnCreate(LPARAM lParam);								// �ú����ڴ��ڱ�������ʱ���Զ�����
	virtual int		InitByBinary(LPARAM lParam);
	virtual	int		KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);
	virtual	void	Update();
    virtual void    OnMouseLeave();
    virtual void    UpdateTip();

    virtual bool    SetParameter(LPCSTR szParameterName, LPARAM lParam);

	void	SetStep(int nStep = 1);                                     // 
	void	SetChangeColor(int nClrStart, int nClrEnd) { m_nColorStart = nClrStart, m_nColorEnd = nClrEnd;}; // ���ý�����ɫ
	void	SetProgress(int nProgress);                                 // ���ý��ȣ�������Ϊ100
	void	SetBkBmp(LPSTR sFileName);                                  // ���ñ���ͼƬ����δ��
	void    SetMaskColor(int nR, int nG, int nB);                       // 
	void	SetBkColor(int nR, int nG, int nB) { m_nColorBk = g_RGB(nR, nG, nB);}; // ���ñ�����ɫ
	void	SetHorizon(bool bHorizon = true) { m_bHorizon = bHorizon;}; // �����Ƿ�Ϊ����ģ���Ϊ������ʾ��

    void    AddToolTip(LPSTR sToolTip);                            // �����Ϣ��ʾ
    void    SetToolTipWH(int nWidth, int nHeight) {m_nWidth = nWidth, m_nHeight = nHeight;}; // ������Ϣ����
};
 