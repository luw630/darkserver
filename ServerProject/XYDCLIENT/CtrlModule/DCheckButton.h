/****************************************************************
//  FileName:   DCheckButton.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG 0925
****************************************************************/

#pragma once

#include "dface.h"
#include <bitmap.h>

const __int32 IID_CDCHECKBUTTON = 0x112339b0; 

class CDCheckButton :

	public CDFace
{
private:
	bool            m_bChecked;
	bool            m_bCapture;
	int	            m_nState;//0: disable, 1: normal, 2:normal_up, 3:normal_down, 4:checked, 5:checked_up, 6:checked_down

    bool            m_bLDown;
    Bitmap          m_bmp[7];
    bool            m_bEnable;
    int             m_x, m_y;
	char            m_sBmpFile[128];
    char            m_sToolTip[30];
    bool            m_bShowToolTip;
    bool            m_bInRect;
    int             m_nWidth, m_nHeight; //of tooltip

public:
	struct SParameter 
	{
		LPSTR lpszBmpFileName[7];
	};

	CDCheckButton(void);
	~CDCheckButton(void);

	virtual	int		OnCreate(LPARAM lParam);								// �ú����ڴ��ڱ�������ʱ���Զ�����
	bool			GetChecked(){return m_bChecked;}                        // ���ؼ����Ƿ񱻰���
	virtual int		InitByBinary(LPARAM lParam);                            // ��ʼ����������,  ��Դ�Ĵ�
	virtual	int		KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);  // ��������������Ϣ����
	virtual	void	Update();                                               // ���ݸ��£��������û��Ķ���

	void			OnCheck(bool bChecked);                                 // ���ü����Ƿ���
    void            AddToolTip(LPSTR sToolTip);                             // �����Ϣ����ʾ
    void            SetToolTipWH(int nWidth, int nHeight) {m_nWidth = nWidth, m_nHeight = nHeight;}; // ������ʾ���͸�
};
