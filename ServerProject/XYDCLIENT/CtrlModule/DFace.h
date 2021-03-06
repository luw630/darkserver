/****************************************************************
//  FileName:   DFace.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  Checked:    YG  0925
****************************************************************/


#ifndef		_DFACE_H_
#define		_DFACE_H_

#include "ObjectService\Include\IObject.h"
#include "Win32Wnd.h"
#include "ResManagerModule\DFont.h"
#include "ResManagerModule\SpriteEx.h"

#include "pub\dwt.h"

#include "guiframework.h"

//#ifdef _DEBUG
//#include <crtdbg.h>
//#include <list>
//using namespace std;
//#undef _AFX_NO_DEBUG_CRT
//#define new new (_NORMAL_BLOCK, __FILE__, __LINE__)
//#endif _DEBUG


#define DECLARE_MY_MESSAGE_MAP() virtual int OnMyMessage(LPFace lpFace, LPARAM lParam);
#define BEGIN_MY_MESSAGE_MAP(a) int  a::OnMyMessage(LPFace lpFace, LPARAM lParam) { 
#define MESSAGE_MAP(b, c)  if(lpFace == (LPFace)b) return c;
#define END_MY_MESSAGE_MAP return RET_NONE;}

const __int32 IID_CDEDIT = 0x112af926; 

enum	KEY_STATE					// 传递键盘鼠标消息的状态 参照TM_KEY_??? & TM_MOUSE_???
{
	KS_NONE=0,
	KEY_BEGIN=10,
	KEY_UP,
	KEY_DOWN,
	KEY_HOLD,
	KEY_END,
	KEY_CHAR,						// My Add  ->
	KEY_IMECHAR,				// 输入IME字符

	MS_BEGIN=20,
	MS_LUP,
	MS_LDOWN,						// 鼠标左键按下
	MS_LMOVE,
	MS_RUP,
	MS_RDOWN,
	MS_RMOVE,
	MS_MOVE,
	MS_STOP,
    MS_LBUTTONDBLCLK,
    MS_RBUTTONDBLCLK,
	MS_MOUSEWHEEL,			//中键	added by xj
	MS_END,
};

enum	E_RETKETMS			// 返回键盘鼠标消息执行的结果
{
	RET_NONE,						// 无特殊返回值
	RET_MSON,						// 鼠标在上面
	RET_MSUP,						// 鼠标松开
	RET_MSDOWN,					// 鼠标按下
	RET_OK,							// 点击确认按钮
	RET_CLOSE,
	// ...
};

typedef CMiniPtr<class CDFace> LPFace;

class CDFace
    : public IGuiObject
{
public:
	CDFace *GetPoint(__int32 IID) { if (this == NULL) return NULL; if (IID == m_IID) return this; return NULL; }
private:
	RECT					m_rcArea;			// 该窗口的有效区域（相对于父窗口）
	CDFace					*m_pParent;			// 该窗口的父对象
	bool					m_bShow;
    bool                    m_actFlag;
    void                    RealAct();
    
public:
	bool					m_bCapture;			// 该窗口是否处于消息捕获模式
	const void				*m_pRef; 
	const __int32			m_IID; 
	std::list < LPFace >	m_ChildsList;		// 该窗口的子对象列表，处于独占模式的子窗口和这个无关

    CDFont &m_Font;

    WORD                    m_wCount;
public:
	virtual	int		InitByBinary(LPARAM lParam);								// 使用二进制数据来初始化窗口数据
	virtual	int		OnCreate(LPARAM lParam) = 0;								// 该函数在窗口被创建的时候自动调用
    virtual int	    PtInFace(POINT &pt);
	virtual	int     OnClose();              									// 该函数在窗口被销毁的时候自动调用
	virtual	int     OnHide();              									// 该函数在窗口被销毁的时候自动调用
	virtual	int		KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);		// 键盘鼠标的输入消息传递
	virtual	void	Update();													// 数据更新，包括画该画的东西
	virtual int	    OnMessage(KEY_STATE iKeyState,int vKey,int x,int y);        // 消息处理
	virtual int		OnMyMessage(LPFace lpFace, LPARAM lParam);                  // 重载来添加自己的消息响应
    virtual void    OnMouseLeave();                                             // 鼠标移出窗口矩形时激发，必须先调用TrackMouseLeft才能被激发
    virtual void    RecvHotKey(int vKey);
    virtual void    OnHotKey(int vKey);

	virtual bool    AddChild( GuiObjConverter pGuiObject );                                    // 添加子窗口
    virtual bool    SetParameter(LPCSTR szParameterName, LPARAM lParam);

    DWORD           GetParameterDWORD(LPARAM lParam, WORD wIndex);
    char*           GetParameterStr(LPARAM lParam, WORD wIndex);
public:
    int             m_nLeft, m_nTop, m_nWidth, m_nHeight;/////////////////////////

    void            SetPos(int x, int y, int width, int height);
	void            SetPos( int x, int y );
	void			SetRECT(int left,int top,int right,int bottom);              // 设置矩形框
	RECT			GetRECT(void);                         // 获取矩形框
	int             GetHeight();
	int             GetWidth();

	bool			isVisible();
	bool			isActive();

	virtual bool    Create(CDFace* pParent, RECT rc, LPARAM lParam);            // 调用该函数创建相应的窗口
	bool			HaveChild() { return m_ChildsList.empty();};                // 返回是否有子窗口，暂时没什么用；），因为没有得到某个窗口下的另一个窗口的指针的函数
	void			ClientToScreen(RECT* rc);
	void			ClientToScreen(POINT *pt);
	void			ScreenToClient(POINT *pt);
	virtual void    Show(bool bShow = true);

	void			SetTopMost(CDFace* pFace);

    virtual void    Active();
	CDFace*		    GetParent();                                                // 得到父窗口
	void			OnMove(int x, int y);
    void			OnMoveTo(int x, int y);
	void			SetCursor(Sprite* pSpr);                                    // 设置鼠标，（没用）

	virtual void	Paint();                                                    // 画窗口内容
	virtual void	PaintLevelTop();											// 在Paint后再次绘制本级窗口的所有子窗口特殊上层效果。。
    void            PaintTip();
	void			SetCapture();                                               // 捕捉鼠标
	void			ReleaseCapture();                                           // 释放鼠标
    bool            isCaptured() { return m_bCapture;};

    CDFace*         GetFirstChild();                                            // 得到第一个子窗口
    CDFace*         GetNextChild(CDFace* pFace);                                // 得到第二个子窗口
    CDFace*         GetFirstFace();                                             // 得到第一个窗口
    void            DrawRect(RECT* rc, int nClrLT, int nClrRB);

    CDFace*         GetCapturedFace();

    void            GetCurrentPos(POINT& pt);
    virtual    void            UpdateTip();

	void            SetFontSize(int size = 12);

    bool            OnProccMessage( KEY_STATE iKeyState );

	static inline bool GetKeyState(int key)
	{
		return ((GetAsyncKeyState(key) & 0x8000) != 0);
	}
public:
	CDFace(__int32 iid, int fontsize = 12);
	virtual ~CDFace(void);

    bool isValid() { return true; }

    friend class CReference;

    static POINT curCursorPosition;

    std::list< LPFace >::iterator m_Child; // 该叠代器主要用于处理跨函数跨层进行的容器删除

    friend class WndCustomize;
};

void ClearAlphaWithBord(int nX, int nY, int nWidth, int nHeight, int nColor,int nAlpha, WORD wBordCornerColor = g_RGB( 222, 222, 222 ), WORD wBordColor = g_RGB(  139, 139, 139 ));

#endif		//_DFACE_H_