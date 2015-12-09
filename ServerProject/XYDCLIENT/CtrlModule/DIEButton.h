#pragma once
#include "dface.h"

const __int32 IID_CDIEBUTTON = 0x113b225e;

class CDIEButton :
	public CDFace
{


private:
    Sprite          m_bmp[4]; 
	
	int				m_x;
	int				m_y;
	bool			m_bCapture;
    bool            m_bLDown;

	char			m_cKey;
	int				m_nStateCount;

	int				m_nBtnState;	//0: disable, 1: normal, 2:up, 3:down, 4:down_down, 5:down_up  //, 6:down_focus,
	BOOL			m_bText;		//have text
	bool			m_bEnable;
	bool			m_bEnableMove;
	bool			m_bDrage;

	char			m_sBmpFile[128];
    char            m_sToolTip[5][30];
    bool            m_bShowToolTip;
	int				m_nTipWidth, m_nTipHeight;
	int				m_nLines;
    bool            m_bInRect;

	bool			m_bFlash;
	int				m_nBmpIndex;

    LPSTR           m_SprName;

	void			DrawBmp(int nBmpIndex);
    int             m_nMouseInTime;
    WORD            m_nMonseInTimeMax;      // ͣ�����ֵ

public:
    std::string	    m_URL;											//IE��ַ

	struct SParameter
	{
		int iType;
		char sBmpFileName[4][256];
        //LPSTR lpszBmpFileName[4];

		LPSTR lpszBtnCaption;
		int nBordClrLT;
		int nBordClrRB;
	} m_Param;

	CDIEButton(void);
	~CDIEButton(void);

	virtual	int	 InitByBinary(LPARAM lParam);								// ʹ�ö�������������ʼ����������
	virtual	int	 OnCreate(LPARAM lParam);								    // �ú����ڴ��ڱ�������ʱ���Զ�����
	virtual	int	 KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);		// ��������������Ϣ����
	virtual	void Update();													// ���ݸ��£��������û��Ķ���
    virtual	void UpdateTip();	

    virtual bool SetParameter(LPCSTR szParameterName, LPARAM lParam);

    void        SetNewParam(LPARAM lNewParam);
	void		GetParam(SParameter* param) { memcpy(param, &m_Param, sizeof (SParameter));};
    void        AdjustRect();

	void         SetHotKey(char cKey);                                      // ��δ��
	void         SetDragMove(bool bEnableMove = false);                     // �����Ƿ������϶�
	void		 SetFlash(bool bFlash = false) { m_bFlash = bFlash;};
	void         Enable(bool bEnable = true);                               // �ܷ���
    bool         isEnable() { return m_bEnable; }                           // �ܷ���
	virtual int  OnClicked();                                               // ���������
    virtual void OnMouseLeave();                                            // ����Ƴ�����

    void         AddToolTip(LPSTR sToolTip);
    void         SetToolTipWH(int nWidth, int nHeight) {};
    void         SetMonseInTimeMax( WORD nMonseInTimeMax = 25 ){ m_nMonseInTimeMax = nMonseInTimeMax; };
    WORD         GetMonseInTimeMax(){ return m_nMonseInTimeMax; };
};
