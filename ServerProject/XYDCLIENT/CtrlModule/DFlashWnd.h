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
#include <FlashPlayer.h>
#include "dface.h"

const __int32 IID_CDFLASHWND = 0x1123ebaf; 

class CDFlashWnd :
	public CDFace
{
public:
    IFlashPlayer    *flash;
    std::string     movie;
    BOOL            rebuild;
    BOOL            trans;
    BOOL            ctrl;
    BOOL            border;
    DWORD           tickSkip;
    DWORD           curSkip;

public:
    static CDFlashWnd *activeFlash; // ��ǰ�Ѿ������flash����Ҫ���ڴ�����Ϣ�¼���
    static CDFlashWnd *prevFlash; // ��ǰ�Ѿ������flash����Ҫ���ڴ�����Ϣ�¼���

public:
	struct SParameter {
        std::string moviefile;
	} m_Param;

	CDFlashWnd(void);
	~CDFlashWnd(void);
	virtual	int	 InitByBinary(LPARAM lParam);								// ʹ�ö�������������ʼ����������
	virtual	int	 OnCreate(LPARAM lParam);								    // �ú����ڴ��ڱ�������ʱ���Զ�����
	virtual	int	 KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);		// ��������������Ϣ����
	virtual	void Update();													// ���ݸ��£��������û��Ķ���
    virtual void UpdateTip();
    virtual void OnMouseLeave();
    virtual int	 PtInFace(POINT &pt);
	virtual void Show(bool bShow);

    virtual bool SetParameter(LPCSTR szParameterName, LPARAM lParam);

    void         AddToolTip(char* sToolTip);                               // �����Ϣ��ʾ
    void         SetToolTipWH(int nWidth, int nHeight) {m_nWidth = nWidth, m_nHeight = nHeight;}; //������ʾ����

};
