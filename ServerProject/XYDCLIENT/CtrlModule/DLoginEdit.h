/****************************************************************
//  FileName:   DEdit.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG 0925
****************************************************************/

#pragma once
#include "dface.h"

//const int CURSOR_FLASHCOUNT	= 10;		// �༭�����ٸ�ѭ����һ��
#define MAX_LEN 1024
#define IID_CDLoginEdit 0x1234abcd

class CDLoginEdit :
	public CDFace
{
private:
	char  m_cBuff[MAX_LEN];
	unsigned int m_nCurPos;
	unsigned int m_nLen;
	unsigned int m_nMaxLen;
    unsigned int m_InputState;

	// ��չ��Ķ���
	unsigned int m_nMaxWriteLen;
	unsigned int m_nShowCur;
	unsigned int m_nNowPos;

	bool  m_bShowCursor;
	bool  m_bShiftDown;
	bool  m_bMask;

	int   m_nEnd, m_nFirst, m_nLast;
	bool  m_bShowBord;
//	int	  m_nBkColor;
    int   m_nCursorClr;
    bool  m_bNumOnly;
	DWORD m_dwNumMax;

    int   m_nR, m_nG, m_nB;
    int   m_iFlashCount;

    bool  m_bEnable;    

    bool  m_bCapture;

	char  m_sToolTip[31];
	bool  m_bShowToolTip, m_bInRect;
	int   m_x, m_y;

public:
	CDLoginEdit(int fontsize = 16);
	~CDLoginEdit(void);  

	virtual	int		InitByBinary(LPARAM lParam);								// ʹ�ö�������������ʼ����������
	virtual	int		OnCreate(LPARAM lParam);								// �ú����ڴ��ڱ�������ʱ���Զ�����
	virtual	int		KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);		// ��������������Ϣ����
	virtual	void	Update();													// ���ݸ��£��������û��Ķ���
    virtual	void	UpdateTip();	
	void			AddToolTip(LPSTR sToolTip);
    virtual void    OnMouseLeave();                                     // ����Ƴ�����  

    virtual bool    SetParameter(LPCSTR szParameterName, LPARAM lParam);
    
    void			DrawRect(RECT* rc, int nClrLT/*����ϱ߿����ɫ*/, int nClrRB/*�Һ��±߿����ɫ*/); // ��ָ����ɫ��3dЧ���Ŀ�
	void			Select(int nFirst, int nLast);                              // ѡ���λ��nFirst��nLast���ַ���
	void			Delete(int nFirst, int nLast);                              // ɾ����λ��nFirst��nLast���ַ���
	void			SetMask(bool bMask = true) { m_bMask = bMask;};             // �����Ƿ���������ʽ��ʾ
	char*			GetString() { return m_cBuff;};                             // ����������ַ���
	void			SetTextColor(int nR, int nG, int nB);                       // ����������ɫ
    unsigned int    GetStrLen() { return m_nLen + 1;};
//	void			SetBkColor(int nClr) { m_nBkColor = nClr;};                 // ���ñ�����ɫ
	void			SetShowBord(bool bShowBord = true) {m_bShowBord = bShowBord;}; // �Ƿ���ʾ�߿�
	void			Clear() { m_nLen = 0;m_nCurPos = 0; memset(m_cBuff, 0, MAX_LEN);};//���
	bool			AddChar(int nChar);                                         // ����ַ�
    void            SetCursorClr(int nClr) { m_nCursorClr = nClr; };            // ���ù����ɫ
    void            SetNumOnly(bool bNum = false);								// �����Ƿ�ֻ��������
    void            SetString(char *pBuff, long size);
    void            SetEnable(bool bEnable) {m_bEnable = bEnable;};
    void            SetMaxLen(int nLen) { if(nLen < MAX_LEN) m_nMaxLen = nLen;};

	void			SetRECT(int left, int top, int right, int bottom);
	void			SetString(DWORD dwNum);
	void			SetNumMax(DWORD dwNumMax);
	void			ShowPosmark();

    void SetFontSize(int size = 12);

private:
    int             m_FirstChar;
};
