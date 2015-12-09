/****************************************************************
//  FileName:   DRichEdit.h
//  Author:		Wonly
//  Create:		2003.4.2
****************************************************************/

#ifndef		_DRICHEDIT_H_
#define		_DRICHEDIT_H_

#include "dface.h"
#include "Bitmap.h"

const		int		MAX_CHATDATALEN	    = 256;		// ���˵�����ݴ�С
const		int		MAX_CHATMSGLEN	    = 128;		// ���˵����С
const		int		MAX_BMPCOUNT		= 30;		// Bmp ͼƬ����
const		int		MAX_DEFCHARNUM		= 8;		// ȱʡ�������
const		int		CURSOR_FLASHCOUNT	= 10;		// �༭�����ٸ�ѭ����һ��
const		int     MAXNAMEPASSWORD     = 16;       // name and password chars max num

class CDRichEdit :	public CDFace
{
	Bitmap		m_Bitmap[MAX_BMPCOUNT];																	// �������ͼƬ��Դ
	char		m_szChatBuff[MAX_CHATMSGLEN];														// ��ǰ��������ı�
	int			m_iCurChatWord;																					// ��ǰ��������ı���С
	int         m_nCursorPos;
	char		m_szDefaultChar[MAX_DEFCHARNUM][MAX_CHATMSGLEN];				// ��ҿ��Ա����������
	bool		m_bIn;
public:
	CDRichEdit(void);
	~CDRichEdit(void);

	virtual	int		OnCreate(LPARAM lParam);								// �ú����ڴ��ڱ�������ʱ���Զ�����
	virtual void	Update();																										// ���ݸ��£��������û��Ķ���
	virtual int		KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);			// ��������������Ϣ����

	void	InitData();																									// ��ʼ����������

	int		DrawRichText( int x, int y, char	*pShowMsg, int iStrLen );	// ������������ı�
	int		DrawText();

	int		InsertChar( char   cChar );																	// ��������������ı�����һ����ַ�
	int		InsertWord( char *szChar );																	// ��������������ı�����һȫ���ַ�
	int		SetString ( char *szStr	 );																	// ���õ�ǰ���������ı�
	char	*GetString(void)		{	 return	m_szChatBuff;		};						// ��ȡ���������ı�
	void	DelChatBuff(void);																					// ɾ��������������ı�

	void	SaveDefChar(char *szFileName);															// ��������趨������
	int		LoadDefChar(char *szFileName);															// ��������趨������
	void	WriteDefChar(int index);																		// ����ǰ�����ı�д������趨������
	void	ReadDefChar(int index);																		// ������趨�����ж�ȡ����ǰ�����ı�
	void	DrawRect(Canvas* pCanvas, RECT* rc, int nClrLT, int nClrRB);
};

class		DEdit : public CDFace
{
	char		m_szText[MAX_CHATMSGLEN];
public:
	BOOL		m_bShow;

	DEdit(void);
	~DEdit(void);

	virtual	int	OnCreate(LPARAM lParam);								// �ú����ڴ��ڱ�������ʱ���Զ�����

	void		SetString(char *szStr );																	// �����ı����е��ı�
	char		*GetString(void)			{		return	m_szText;		};					// ��ȡ�ı����е��ı�
};

#endif		//_DRICHEDIT_H_