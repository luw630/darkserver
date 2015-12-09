/****************************************************************
//  FileName:   DSlideBar.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG 0925
****************************************************************/

#pragma once
#include "dface.h"
#include "Colors.h"

const __int32 IID_CDSLIDEBAR = 0x1122ed9e; 

class CDSlideBar :
	public CDFace
{
private:
	int	 		m_nPos;
	int  		m_nMaxPos;
	int  		m_nMinPos;
	int  		m_xOld, m_yOld;
	int  		m_nBarColor, m_nSlideColor;

	bool 		m_bStartDrag;
	int  		m_x, m_y;
	int  		m_nBtnState; //��������״̬
	bool 		m_bShowBmp;  //�Ƿ���ʾ����ͼƬ
	bool 		m_bCapture;

	bool		m_bHorizon;	//�Ƿ��Ǻ��򻬶���.
	int			m_nBarWidthHeight;

	Sprite		m_sprBack, m_sprBar[4];//Normal, m_sprBarActive, m_sprBarDown;
	char		m_strBarImg[256];
public:
	struct SParameter 
	{
		int 	nColorBar;
		int 	nColorSlider;
	};

	CDSlideBar(void);
	~CDSlideBar(void);
	virtual	int	InitByBinary(LPARAM lParam);							// ʹ�ö�������������ʼ����������
	virtual	int	OnCreate(LPARAM lParam);								// �ú����ڴ��ڱ�������ʱ���Զ�����
	virtual	int	KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);	// ��������������Ϣ����
	virtual	void Update();							                    // ���ݸ��£��������û��Ķ���
    virtual void OnMouseLeave();
    virtual void PaintLevelTop();

	virtual bool SetParameter( LPCSTR szParameterName, LPARAM lParam );
	virtual int OnPosChanging(int nPos);                                // ������λ�øı�ʱ����
	void        SetPosition(int nPos, int nMaxPos = 100, int nMinPos = 0);     // ���û�����λ�ã����ֵ����Сֵ
	int         GetPosition();                                                 // ���ص�ǰ������λ��

	void		SetHorizon(bool bHorizon = true); // �����Ƿ�Ϊ����ģ���Ϊ������ʾ��

	void		SetBePicture(char* szFileName_Back, char* szFileName_Bar_Normal, 
		                     char* szFileName_Bar_Active, char* szFileName_Bar_Down);
	void		SetBePicture(char* szFileName_Back, char* szFileName);

    RECT        GetRcBar();
     bool       m_bShowTip;

};
