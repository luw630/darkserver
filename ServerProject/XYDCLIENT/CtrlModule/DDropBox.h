/****************************************************************
//  FileName:   DDropBox.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG 0925
****************************************************************/

#pragma once
#include "dface.h"
#include "DEdit.h"
const __int32 IID_CDDROPBOX = 0x1174e02a; 
#define		SKILLLIST	20
class CDDropBox :
    public CDFace
{
private:
    char    m_sItem[20][100];
    char    m_cHotKey[20];
    int     m_nItemCount;
    bool    m_bDropDown;
    int     m_nItemSel;
    int     m_nItemMouseOn;
    RECT    m_rcItem[20];
    RECT    m_rcItems;
    bool    m_bDown; //�Ƿ��ڵ�����״̬

    bool    m_bPic;
    Sprite  m_spr[SKILLLIST];
    bool    m_bPtInRect;
    char    m_sToolTip[20][100];

    bool    m_bDrawFocusRect;//�Ƿ񻭽������ͼƬ�Ļ�һ�㲻����������
	bool	m_bDrawNormalRect;
    bool    m_bUp;//true:���ϵ��� false�����µ���

    int     m_x, m_y;

	bool	m_bEnableHotKey;

public:
    CDDropBox(void);
    ~CDDropBox(void);   
	virtual	int	    OnCreate(LPARAM lParam);								// �ú����ڴ��ڱ�������ʱ���Զ�����
	virtual int		InitByBinary(LPARAM lParam);
	virtual	int		KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);
	virtual	void	Update();
    virtual	void	UpdateTip();
    virtual void    OnMouseLeave();
    virtual void    OnHotKey(int vKey);

    virtual bool    SetParameter(LPCSTR szParameterName, LPARAM lParam);

    void            SetBePic() { m_bPic = true;};
    int             AddItem(LPSTR sItem);                               //����������ΪҪ��ӵ��ַ���
    void            SetItem(int nIndex, LPSTR sItem);
    int             GetSelItem();                                       // ����ѡ����������
	LPSTR			GetSelItemString(); 
    void            DropDown() { m_bDropDown = !m_bDropDown;};          // ����
    void            AddToolTip(int nIndex, char* sToolTip );
    LPSTR           GetToolTip(int nIndex) { if(nIndex > 9) return "nIndex too big"; return m_sToolTip[nIndex];};
    void            OnClicked(int nSel);
    void            SetDrawFocuseRect(bool bDrawFocusRect) { m_bDrawFocusRect = bDrawFocusRect;};
	void			SetDrawNormalRect(bool bDrawNormalRect) { m_bDrawNormalRect = bDrawNormalRect;};
    void            RemoveAll();
    void            RemoveByIndex(int nIndex);// 0----size-1
    void            SetUp(bool bUp = true) { m_bUp = bUp;};   //���������ϻ����µ���
    void            SetItemSelect(int nSel, bool bOnClick = true);

    void            SetHotKey(int nIndex, char cHotKey);
	void			SetEnableHotKey(bool bEnableHotKey = false) { m_bEnableHotKey = bEnableHotKey;};
    char            GetHotKey(int nIndex);

	int				GetItemCount() { return m_nItemCount; };
	void			ForceClearHotKey();//ǿ��������п�ݼ�����Ϊ�հף��¼Ӻ����������޸���ǰ�ĺ�����
	//////////////////////////////////////////////////////////////////////////
	void			 GetSkillIDList (int iID,int iX,int iY);			//�õ�����ID�б�
};
