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

enum	KEY_STATE					// ���ݼ��������Ϣ��״̬ ����TM_KEY_??? & TM_MOUSE_???
{
	KS_NONE=0,
	KEY_BEGIN=10,
	KEY_UP,
	KEY_DOWN,
	KEY_HOLD,
	KEY_END,
	KEY_CHAR,						// My Add  ->
	KEY_IMECHAR,				// ����IME�ַ�

	MS_BEGIN=20,
	MS_LUP,
	MS_LDOWN,						// ����������
	MS_LMOVE,
	MS_RUP,
	MS_RDOWN,
	MS_RMOVE,
	MS_MOVE,
	MS_STOP,
    MS_LBUTTONDBLCLK,
    MS_RBUTTONDBLCLK,
	MS_MOUSEWHEEL,			//�м�	added by xj
	MS_END,
};

enum	E_RETKETMS			// ���ؼ��������Ϣִ�еĽ��
{
	RET_NONE,						// �����ⷵ��ֵ
	RET_MSON,						// ���������
	RET_MSUP,						// ����ɿ�
	RET_MSDOWN,					// ��갴��
	RET_OK,							// ���ȷ�ϰ�ť
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
	RECT					m_rcArea;			// �ô��ڵ���Ч��������ڸ����ڣ�
	CDFace					*m_pParent;			// �ô��ڵĸ�����
	bool					m_bShow;
    bool                    m_actFlag;
    void                    RealAct();
    
public:
	bool					m_bCapture;			// �ô����Ƿ�����Ϣ����ģʽ
	const void				*m_pRef; 
	const __int32			m_IID; 
	std::list < LPFace >	m_ChildsList;		// �ô��ڵ��Ӷ����б������ڶ�ռģʽ���Ӵ��ں�����޹�

    CDFont &m_Font;

    WORD                    m_wCount;
public:
	virtual	int		InitByBinary(LPARAM lParam);								// ʹ�ö�������������ʼ����������
	virtual	int		OnCreate(LPARAM lParam) = 0;								// �ú����ڴ��ڱ�������ʱ���Զ�����
    virtual int	    PtInFace(POINT &pt);
	virtual	int     OnClose();              									// �ú����ڴ��ڱ����ٵ�ʱ���Զ�����
	virtual	int     OnHide();              									// �ú����ڴ��ڱ����ٵ�ʱ���Զ�����
	virtual	int		KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);		// ��������������Ϣ����
	virtual	void	Update();													// ���ݸ��£��������û��Ķ���
	virtual int	    OnMessage(KEY_STATE iKeyState,int vKey,int x,int y);        // ��Ϣ����
	virtual int		OnMyMessage(LPFace lpFace, LPARAM lParam);                  // �����������Լ�����Ϣ��Ӧ
    virtual void    OnMouseLeave();                                             // ����Ƴ����ھ���ʱ�����������ȵ���TrackMouseLeft���ܱ�����
    virtual void    RecvHotKey(int vKey);
    virtual void    OnHotKey(int vKey);

	virtual bool    AddChild( GuiObjConverter pGuiObject );                                    // �����Ӵ���
    virtual bool    SetParameter(LPCSTR szParameterName, LPARAM lParam);

    DWORD           GetParameterDWORD(LPARAM lParam, WORD wIndex);
    char*           GetParameterStr(LPARAM lParam, WORD wIndex);
public:
    int             m_nLeft, m_nTop, m_nWidth, m_nHeight;/////////////////////////

    void            SetPos(int x, int y, int width, int height);
	void            SetPos( int x, int y );
	void			SetRECT(int left,int top,int right,int bottom);              // ���þ��ο�
	RECT			GetRECT(void);                         // ��ȡ���ο�
	int             GetHeight();
	int             GetWidth();

	bool			isVisible();
	bool			isActive();

	virtual bool    Create(CDFace* pParent, RECT rc, LPARAM lParam);            // ���øú���������Ӧ�Ĵ���
	bool			HaveChild() { return m_ChildsList.empty();};                // �����Ƿ����Ӵ��ڣ���ʱûʲô�ã�������Ϊû�еõ�ĳ�������µ���һ�����ڵ�ָ��ĺ���
	void			ClientToScreen(RECT* rc);
	void			ClientToScreen(POINT *pt);
	void			ScreenToClient(POINT *pt);
	virtual void    Show(bool bShow = true);

	void			SetTopMost(CDFace* pFace);

    virtual void    Active();
	CDFace*		    GetParent();                                                // �õ�������
	void			OnMove(int x, int y);
    void			OnMoveTo(int x, int y);
	void			SetCursor(Sprite* pSpr);                                    // ������꣬��û�ã�

	virtual void	Paint();                                                    // ����������
	virtual void	PaintLevelTop();											// ��Paint���ٴλ��Ʊ������ڵ������Ӵ��������ϲ�Ч������
    void            PaintTip();
	void			SetCapture();                                               // ��׽���
	void			ReleaseCapture();                                           // �ͷ����
    bool            isCaptured() { return m_bCapture;};

    CDFace*         GetFirstChild();                                            // �õ���һ���Ӵ���
    CDFace*         GetNextChild(CDFace* pFace);                                // �õ��ڶ����Ӵ���
    CDFace*         GetFirstFace();                                             // �õ���һ������
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

    std::list< LPFace >::iterator m_Child; // �õ�������Ҫ���ڴ����纯�������е�����ɾ��

    friend class WndCustomize;
};

void ClearAlphaWithBord(int nX, int nY, int nWidth, int nHeight, int nColor,int nAlpha, WORD wBordCornerColor = g_RGB( 222, 222, 222 ), WORD wBordColor = g_RGB(  139, 139, 139 ));

#endif		//_DFACE_H_