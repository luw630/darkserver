/****************************************************************
//  FileName:   DRichEdit.h
//  Author:		Wonly
//  Create:		2003.4.2
****************************************************************/

#ifndef		_DRICHEDIT_H_
#define		_DRICHEDIT_H_

#include "dface.h"
#include "Bitmap.h"

const		int		MAX_CHATDATALEN	    = 256;		// 玩家说话数据大小
const		int		MAX_CHATMSGLEN	    = 128;		// 玩家说话大小
const		int		MAX_BMPCOUNT		= 30;		// Bmp 图片数量
const		int		MAX_DEFCHARNUM		= 8;		// 缺省语句数量
const		int		CURSOR_FLASHCOUNT	= 10;		// 编辑光标多少个循环闪一次
const		int     MAXNAMEPASSWORD     = 16;       // name and password chars max num

class CDRichEdit :	public CDFace
{
	Bitmap		m_Bitmap[MAX_BMPCOUNT];																	// 表情符号图片资源
	char		m_szChatBuff[MAX_CHATMSGLEN];														// 当前玩家输入文本
	int			m_iCurChatWord;																					// 当前玩家输入文本大小
	int         m_nCursorPos;
	char		m_szDefaultChar[MAX_DEFCHARNUM][MAX_CHATMSGLEN];				// 玩家可以保存的语句队列
	bool		m_bIn;
public:
	CDRichEdit(void);
	~CDRichEdit(void);

	virtual	int		OnCreate(LPARAM lParam);								// 该函数在窗口被创建的时候自动调用
	virtual void	Update();																										// 数据更新，包括画该画的东西
	virtual int		KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);			// 键盘鼠标的输入消息传递

	void	InitData();																									// 初始化画布画笔

	int		DrawRichText( int x, int y, char	*pShowMsg, int iStrLen );	// 画包含表情的文本
	int		DrawText();

	int		InsertChar( char   cChar );																	// 在玩家输入聊天文本插入一半角字符
	int		InsertWord( char *szChar );																	// 在玩家输入聊天文本插入一全角字符
	int		SetString ( char *szStr	 );																	// 设置当前玩家输入的文本
	char	*GetString(void)		{	 return	m_szChatBuff;		};						// 获取玩家输入的文本
	void	DelChatBuff(void);																					// 删除玩家输入聊天文本

	void	SaveDefChar(char *szFileName);															// 保存玩家设定语句队列
	int		LoadDefChar(char *szFileName);															// 载入玩家设定语句队列
	void	WriteDefChar(int index);																		// 将当前输入文本写入玩家设定语句队列
	void	ReadDefChar(int index);																		// 从玩家设定语句队列读取到当前输入文本
	void	DrawRect(Canvas* pCanvas, RECT* rc, int nClrLT, int nClrRB);
};

class		DEdit : public CDFace
{
	char		m_szText[MAX_CHATMSGLEN];
public:
	BOOL		m_bShow;

	DEdit(void);
	~DEdit(void);

	virtual	int	OnCreate(LPARAM lParam);								// 该函数在窗口被创建的时候自动调用

	void		SetString(char *szStr );																	// 设置文本框中的文本
	char		*GetString(void)			{		return	m_szText;		};					// 获取文本框中的文本
};

#endif		//_DRICHEDIT_H_