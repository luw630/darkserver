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

	bool			m_HideSlideBar;			/// 不显示滚动条
	int				m_LineCountOnlyShow;	/// 这显示这么多行

    Sprite          *m_pSprItem[10][500];   // 图片

	bool            m_bTexCenter; // 字体居中
	bool			m_bEnable;				// 是否可用

public:
	GuiEvent		m_MouseMoveEvent;
	GuiEvent		m_SelectItemEvent;
	GuiEvent		m_DbSelectItemEvent;
	GuiEvent		m_LDownPrepareEvent;
public:
    CDListBox(void);
    ~CDListBox(void);
	virtual bool    AddChild( GuiObjConverter pGuiObject );                       // 添加子窗口
	virtual	int		OnCreate(LPARAM lParam);								// 该函数在窗口被创建的时候自动调用
	virtual int		InitByBinary(LPARAM lParam);
	virtual	int		KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);
	virtual	void	Update();
    virtual void    OnMouseLeave();
	virtual int     OnClicked();

    void            AddItem(int nX, int nY, LPCSTR sItem);               // 在指定位置添加项
	void            setItem( int x, int y, LPCSTR value );
    void            SetItemWidth( int nWidth, int nX = 0);              // 设置第nX列项的宽度
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

    void            AddItem(int nX, int nY, DWORD dwNumber, bool singed = false );     // 在指定位置添加项
    void            AddItem(int nX, int nY, Sprite *pSprItem);                         // 在制定位置添加图片项
    BOOL            IsLineEmpty(int nY);                                               // 判断该行是否为空
    int             GetPosOffestX( int nX );                                           // 得到nX列的坐标偏移
	/*void			SetSliderBarSpr(char* fileback, char* filenormal, char* fileactive, char* filedown);*/
    class           CDSlideBar		*m_pSlideBar;
	void            setItemColor( int i, int j, DWORD color );
	void            setSlideBar( char* b1, char* b2, char* b3, char* b4 );
	void			Enable(bool bEnable = true){ m_bEnable = bEnable;}                              // 是否可用

	DECLARE_MY_MESSAGE_MAP()
	int				OnPosChanged();
	int				GetSelectIndex( int x, int y );
};
