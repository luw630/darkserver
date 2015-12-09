/****************************************************************
//  FileName:   DStatic.h
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
****************************************************************/

#pragma once
#include "dedit.h"

const __int32 IID_CDSTATIC = 0x112cb0eb; 


//#ifdef _DEBUG
//#include <crtdbg.h>
//using namespace std;
//#undef _AFX_NO_DEBUG_CRT
//#define new new (_NORMAL_BLOCK, __FILE__, __LINE__)
//#endif _DEBUG



class CDStatic :
	public CDFace
{
#define MAX_TEXTCOLOR 10

private:
    //char            m_cBuff[MAX_TEXTCOLOR][512];
	std::string		m_strBuff;
	std::vector< std::string > m_strBufferList;
    DWORD           m_dwClr;

	int				m_nFontLineWidth, m_nFontLineHeight;
	int             m_textHeiht,m_drawpos;


	class CDSlideBar		*m_pSlideBar;
	int				m_nLineBegin;
	bool			m_bHaveSpecialChar;
	bool			m_bHaveSlideBar;
	bool			m_bHoldSlideBarPos;

public:
	GuiEvent		m_MouseMoveEvent;
	GuiEvent		m_MouseEnterEvent;
	GuiEvent		m_MouseLeaveEvent;
private:
	bool			m_bMouseEnter;
public:
	CDStatic(int fontsize = 12);
	~CDStatic(void);

	virtual bool    SetParameter( LPCSTR szParameterName, LPARAM lParam );

	int             GetTexHeight();
	virtual	int		InitByBinary(LPARAM lParam);								// ʹ�ö�������������ʼ����������
	virtual	int		OnCreate(LPARAM lParam);								    // �ú����ڴ��ڱ�������ʱ���Զ�����
	virtual	int		KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y);		// ��������������Ϣ����
	virtual	void	Update();													// ���ݸ��£��������û��Ķ���
    virtual void    OnMouseLeave();

	void            AddTex( std::string );
	void            ClearTex();
	void			SetText(LPSTR sText, bool bHaveSpecialChar = false);                // ���þ�̬�ı�
	void			SetText(DWORD dNum);                                        // ���þ�̬�ı�������Ϊ���������
    void            SetText(int   nNum);
	char*			GetText();//m_cBuff[0];};
	void			SetTextColor(int nR, int nG, int nB);                       // ����������ɫ
	void			SetTextColor( DWORD color);									// ����������ɫ
    void            SetTextLine(int nWidth, int nHeight);                       // ���û��и߶ȺͿ��
	void			SetHoldSlideBarPos() { m_bHoldSlideBarPos = true;};
	/*void			SetSliderBarSpr(char* fileback, char* filenormal, char* fileactive, char* filedown);*/

	DECLARE_MY_MESSAGE_MAP()
	int				OnPosChanged();
};
