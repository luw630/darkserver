/****************************************************************
//  FileName:   DSprCheckButton.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG 0926
****************************************************************/

#pragma once
#include "dface.h"

const __int32 IID_CDSPRCHECKBUTTON = 0x1152a982; 

class CDSprCheckButton :
	public CDFace
{
private:
	bool            m_bChecked;
	bool            m_bLDown;
	int	            m_nState;   //0: disable, 1: normal, 2:normal_up, 3:normal_down, 4:checked, 5:checked_up, 6:checked_down
//	int	            m_bdisable; //0: disable, 1: normal, 2:normal_up, 3:normal_down, 4:checked, 5:checked_up, 6:checked_down
	Sprite          m_bmp[7];
	char            m_sBmpFile[128];


    int             m_x, m_y;
    bool            m_bShowToolTip;

	char            m_sToolTip[5][30];
	int				m_nTipWidth, m_nTipHeight;
	int				m_nLines;

    bool            m_bInRect;
    int             m_nWidth, m_nHeight;
    bool            m_bEnable;
	DWORD			m_dwColor;
	bool			m_bUseColor;
    int             m_nMouseInTime;         // ���ͣ��ʱ��
    WORD            m_nMonseInTimeMax;      // ͣ�����ֵ

public:
	struct SParameter 
	{
		LPSTR lpszBmpFileName[7];
        char sBmpFileName[7][256];
	} m_Param;

	CDSprCheckButton(void);
	~CDSprCheckButton(void);

	virtual	int		OnCreate(LPARAM lParam);								// �ú����ڴ��ڱ�������ʱ���Զ�����
	bool			GetChecked(){return m_bChecked;}
	virtual int		InitByBinary(LPARAM lParam);                            // ��ʼ����������,  ��Դ�Ĵ�
	virtual	int		KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);  // ��������������Ϣ����
	virtual	void	Update();                                               // ���ݸ��£��������û��Ķ���
    virtual void    OnMouseLeave();                                         // ����Ƴ�����    
    virtual	void    UpdateTip();	

    virtual bool    SetParameter(LPCSTR szParameterName, LPARAM lParam);

	bool			IsEnable();
    void            Enable(bool bEnable = true);    //
    void            SetDisable() { m_bEnable = false; m_nState = 0;};
	void			SetCheck(bool bCheck);                                  // ���ü����Ƿ�Ϊ����״̬
	void			OnCheck(bool bChecked);                                 // ����󱻴���
    void            AddToolTip(LPSTR sToolTip);                             // �����Ϣ��ʾ
    void            SetToolTipWH(int nWidth, int nHeight) {m_nTipWidth = nWidth, m_nTipHeight = nHeight;}; // ������ʾ����
	void			SetCheckWithColor(bool bCheck = false, DWORD color = 0);
    void            SetMonseInTimeMax( WORD nMonseInTimeMax = 25 ){ m_nMonseInTimeMax = nMonseInTimeMax; };// Ĭ��50��==1��
    WORD            GetMonseInTimeMax(){ return m_nMonseInTimeMax; };
	void			GetToolTip(char * tipstr);
};
