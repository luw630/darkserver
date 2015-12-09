#pragma once
#include "ctrlmodule\ddialog.h"
//#define _DEBUG_FOR_LUOU_TEAM
class CDSprMenu :
    public CDDialog
{
private:
	//Sprite m_spr;
    CDSprButton  *m_pBtnInfo;
    bool    m_bLDown;
    int     m_nIDTo;
    char    m_szName[11];

    char    m_sSprFileName_Bk[128];
public:
    CDSprMenu(void);
    ~CDSprMenu(void);
	virtual	int		OnCreate(LPARAM lParam);								// �ú����ڴ��ڱ�������ʱ���Զ�����
	virtual	int		OnHide();												// �ú����ڴ��ڱ����ٵ�ʱ���Զ�����
	virtual	int		InitByBinary(LPARAM lParam);							// ʹ�ö�������������ʼ����������
	virtual	void	Update();												// ���ݸ��£��������û��Ķ���
	virtual	int		KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);	// ��������������Ϣ����
 
    virtual bool    AddChild( GuiObjConverter pGuiObject );                                    // ����Ӵ���
    virtual bool	SetParameter(LPCSTR szParameterName, LPARAM lParam);

    void            ShowIt(char* szName, bool bShow = true);
	DECLARE_MY_MESSAGE_MAP()
    int    OnInfo();
};
