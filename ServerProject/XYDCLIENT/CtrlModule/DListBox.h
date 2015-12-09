/****************************************************************
//  FileName:   DListBox.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG 0924
****************************************************************/

#pragma once
#include "dface.h"
#include "DSlideBar.h"

const __int32 IID_CDLISTBOX = 0x1178fe6c; 

class CDListBox :
    public CDFace
{
	friend class CSimpleTree;
private:
    char* 			m_sItem[10][500];
	DWORD           m_sItemColor[10][500];
    int   			m_nItemWidth[10];
    int   			m_x, m_y;

    int   			m_nItemHeight;
    int   			m_nIndexFocused;
    int   			m_nIndexSelected;
    bool  			m_bShowBoard, m_bShowSelBoard;
	Sprite			m_sprSelBack;

	int				m_nMaxLineCount;
	int				m_nLineCount;
	int				m_nLineBegin;
	bool			m_bShowSlideBar;
	DWORD			m_dwTextColor, m_dwSelColor;

	bool			m_bHoldSlideBarPos;
	int				m_nLineCountCanShow;

	bool			m_HideSlideBar;			/// ����ʾ������
	int				m_LineCountOnlyShow;	/// ����ʾ��ô����

    Sprite          *m_pSprItem[10][500];   // ͼƬ

	bool            m_bTexCenter; // �������
	bool			m_bEnable;				// �Ƿ����

public:
	GuiEvent		m_MouseMoveEvent;
	GuiEvent		m_SelectItemEvent;
	GuiEvent		m_DbSelectItemEvent;
	GuiEvent		m_LDownPrepareEvent;
public:
    CDListBox(void);
    ~CDListBox(void);
	virtual bool    AddChild( GuiObjConverter pGuiObject );                       // ����Ӵ���
	virtual	int		OnCreate(LPARAM lParam);								// �ú����ڴ��ڱ�������ʱ���Զ�����
	virtual int		InitByBinary(LPARAM lParam);
	virtual	int		KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);
	virtual	void	Update();
    virtual void    OnMouseLeave();
	virtual int     OnClicked();

    void            AddItem(int nX, int nY, LPCSTR sItem);               // ��ָ��λ�������
	void            setItem( int x, int y, LPCSTR value );
    void            SetItemWidth( int nWidth, int nX = 0);              // ���õ�nX����Ŀ��
    void            SetItemHeight(int nItemHeight) { m_nItemHeight = nItemHeight;};
    char*           GetSelItemString( int i = 0 );
	int				GetSelItemIndex(int nRowIndex = 0);
	void			SetSelItemIndex(int nLine, bool bSendClickMsg = false);
	int				GetItemWidth(int nCow) { return m_nItemWidth[nCow];};
	int				GetItemHeight() { return m_nItemHeight; }
    void            DeleteItem(int nIndex);
	int				GetItemCount() { return m_nMaxLineCount;};
	int				GetCurItemCount(){ return m_nLineCount;};
	int				GetSlideBarPos() { return m_pSlideBar->GetPosition(); };
	int				GetLineBegin() {return m_nLineBegin;};
	void            setTexCenter( bool value ) { m_bTexCenter = value; }

    void            Clear();
    void            SetShowBoard(bool bShowBoard){m_bShowBoard = bShowBoard;};
	void            SetShowSelRect(bool bShowSelBoard){m_bShowSelBoard = bShowSelBoard;};
	void            SetShowSelSpr(char *szSprFileName);
	void			SetColor(DWORD dwNormalColor, DWORD dwSelColor = 99999);
	void			SetHoldSlideBarPos() { m_bHoldSlideBarPos = true;};
	void			SetSlideBarPos( int pos ) { m_pSlideBar->SetPosition( pos ); m_pSlideBar->OnPosChanging(0);};

    void            AddItem(int nX, int nY, DWORD dwNumber, bool singed = false );     // ��ָ��λ�������
    void            AddItem(int nX, int nY, Sprite *pSprItem);                         // ���ƶ�λ�����ͼƬ��
    BOOL            IsLineEmpty(int nY);                                               // �жϸ����Ƿ�Ϊ��
    int             GetPosOffestX( int nX );                                           // �õ�nX�е�����ƫ��
	/*void			SetSliderBarSpr(char* fileback, char* filenormal, char* fileactive, char* filedown);*/
    class           CDSlideBar		*m_pSlideBar;
	void            setItemColor( int i, int j, DWORD color );
	void            setSlideBar( char* b1, char* b2, char* b3, char* b4 );
	void			Enable(bool bEnable = true){ m_bEnable = bEnable;}                              // �Ƿ����

	DECLARE_MY_MESSAGE_MAP()
	int				OnPosChanged();
	int				GetSelectIndex( int x, int y );
};
