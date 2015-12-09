/****************************************************************
//  FileName:   DSprProgress.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG 0926 
****************************************************************/

#pragma once
#include "dface.h"

const __int32 IID_CDSPRPROGRESS = 0x116b0c83; 

class CDSprProgress :
    public CDFace
{
public:
	// ��ʾģʽ
	enum DRAW_MODE
	{
		DM_CUR_MAX,			// curProgress/maxProgress
		DM_PERCENT,				// progress%
		DM_LABEL,					// label
	};

private:
    Sprite          m_spr;
    bool            m_bLeft;
    int             m_bClr;
    int             m_nProgress, m_nMaxProgress;
    int             m_nDia;
    int             m_nCurTime;
	bool	        m_bHorizon;

    int             m_x, m_y;
    bool            m_bShowToolTip;
    bool            m_bInRect;
    int             m_nWidth, m_nHeight;

    bool            m_bShowText; 
    char            m_sToolTip[30];
    char            m_sBarFile[128];
    Sprite          m_sprBar, m_sprMask;

    char            m_sSprFileName_Progress[128], m_sSprFileName_Mask[128];

    bool            m_bTrans;
	int				m_nFramesIndex, m_nIndexTmp;
    bool            m_bConverse; // �Ƿ���������
	bool            m_bClipMode;

	DRAW_MODE			m_eDrawMode;

	// ��ǩ����
	std::string	m_strLabel;

	// ��ʾ���ֵ�ʱ���Ƿ���Ҫ��ʾ��ͼ
	bool		m_bShowTextBG;

public:
    CDSprProgress(void);
    ~CDSprProgress(void);
    virtual bool SetParameter(LPCSTR szParameterName, LPARAM lParam);
	virtual	int OnCreate(LPARAM lParam);								// �ú����ڴ��ڱ�������ʱ���Զ�����
	virtual int InitByBinary(LPARAM lParam);
	virtual	int KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);
	virtual	void Update();
    virtual void OnMouseLeave();                                     // ����Ƴ�����  
    virtual void UpdateTip();
    void SetProgress(int nProgress, int nMaxProgress = 100); // ���ý��������ȣ������ֵ
    void AddToolTip(LPSTR sToolTip);                         // �����ʾ��
    void SetToolTipWH(int nWidth, int nHeight) {m_nWidth = nWidth, m_nHeight = nHeight;}; // ������ʾ����
    bool isToolTipShow() { return m_bInRect;};               // ��ʾ���Ƿ�ɼ�
    void SetShowTip(bool bShowIt) { m_bInRect = bShowIt;};   // �����Ƿ���ʾ��ʾ��
    void SetShowText(bool bShowText) { m_bShowText = bShowText;};
   	void SetHorizon(bool bHorizon = true) { m_bHorizon = bHorizon;}; // �����Ƿ�Ϊ����ģ���Ϊ������ʾ��
    void SetBar(LPSTR sBarFile);

    void SetMaskBmp(LPSTR sMaskFile); //��Ҫ��Ϊ����Ӧ��ʾ�������ϵĸ߹�Ч��
    void SetBeTrans(bool bTrans) { m_bTrans = bTrans;};
    void SetConverse(bool converse = true) { m_bConverse = converse; }

	void	SetDrawMode(DRAW_MODE eDrawMode) { m_eDrawMode = eDrawMode;};
	void	EnableShowTextBG(bool bEnable) { m_bShowTextBG = bEnable; }

	void	SetLabel(const char* label) { m_strLabel = label; }
};
