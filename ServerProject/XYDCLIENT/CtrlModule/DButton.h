/****************************************************************
//  FileName:   DButton.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG 0925
****************************************************************/

#pragma once
#include <Bitmap.h>
#include <Engine.h>
#include <WavSound.h>
#include "dface.h"

const __int32 IID_CDBUTTON = 0x1122ebaf; 

class CDButton :
	public CDFace
{
private:
    Bitmap			m_bmp[4]; 	
	int				m_x;
	int				m_y;
	bool			m_bCapture;

    char            m_pToolTip[30];
	int				m_nStateCount;

	int				m_nBtnState;//0: disable, 1: normal, 2:up, 3:down, 4:down_down, 5:down_up  //, 6:down_focus,
	BOOL			m_bText;//have text
	bool			m_bEnable;
	bool			m_bLDown;

    bool            m_bShowToolTip;
    bool            m_bInRect;
    int             m_nWidth, m_nHeight;

	void			DrawBmp(int nBmpIndex);
public:
    static WavSound m_ClickEffect;
	GuiEvent		m_ClickEvent;
public:
	struct SParameter
	{
		int iType;                                                          // ��û��
		LPSTR lpszBmpFileName[4];                                           // ͼƬ�ļ������飬Bitmap��ʽ
        char sBmpFileName[4][256];

		LPSTR lpszBtnCaption;                                               // ��û��
		int nBordClrLT;                                                     // ��û��
		int nBordClrRB;                                                     // ��û��
	} m_Param;

	CDButton(void);
	~CDButton(void);
	virtual	int	 InitByBinary(LPARAM lParam);								// ʹ�ö�������������ʼ����������
	virtual	int	 OnCreate(LPARAM lParam);								    // �ú����ڴ��ڱ�������ʱ���Զ�����
	virtual	int	 KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);		// ��������������Ϣ����
	virtual	void Update();													// ���ݸ��£��������û��Ķ���
    virtual void UpdateTip();
	void		 Enable(bool bEnable = true);
    bool		 isEnable() { return m_bEnable; }
	virtual int  OnClicked();
    virtual void OnMouseLeave();

    virtual bool    SetParameter(LPCSTR szParameterName, LPARAM lParam);

    void         AddToolTip(char* sToolTip);                               // �����Ϣ��ʾ
    void         SetToolTipWH(int nWidth, int nHeight) {m_nWidth = nWidth, m_nHeight = nHeight;}; //������ʾ����

};
