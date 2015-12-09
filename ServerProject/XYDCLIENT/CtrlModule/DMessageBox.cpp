/****************************************************************
//  FileName:   DMessageBox.cpp
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
****************************************************************/
#include "StdAfx.h"

#include "dmessagebox.h"

BEGIN_MY_MESSAGE_MAP(CDMessageBox)
MESSAGE_MAP(m_pBtn1, OnOK())
END_MY_MESSAGE_MAP

CDMessageBox::CDMessageBox(void) 
{
	m_nWndClr = g_RGB(172,172, 272);
    // m_InfoText[0] = '\0';
    m_sSprFileName_Bk[0] = 0;

	m_bSetZ = false;
}

CDMessageBox::~CDMessageBox(void)
{
}

bool CDMessageBox::SetParameter(LPCSTR szParameterName, LPARAM lParam)
{    
    if(strcmp(szParameterName, "SprFileName_Bk") == 0)
        dwt::strcpy(m_sSprFileName_Bk, (LPSTR)lParam, 128);

    CDFace::SetParameter(szParameterName, lParam);
    return false;
}

bool CDMessageBox::AddChild( GuiObjConverter pGuiObject )
{
    if(strcmp(pGuiObject->m_GuiName, "关闭") == 0)
        m_pBtn1 = (CDSprButton*)pGuiObject;

    return CDFace::AddChild(pGuiObject);
}

int	CDMessageBox::OnCreate(LPARAM lParam)
{
    m_spr.Load(m_sSprFileName_Bk);

	return 1;
}

int CDMessageBox::KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y)
{
    if((iKeyState == KEY_CHAR) && (vKey == VK_ESCAPE))
    {
        m_MessageQueue.pop();
        // Show(false);
    }

	return RET_NONE;
}

void CDMessageBox::Update()
{
    if (m_MessageQueue.size() == 0)
    {
        Show(false);
        return;
    }
	if ( m_bSetZ)		//在创建人物弹出对话框时，提升Z坐标
	{
		extern void Set3DZ( DWORD base, DWORD flag );
		extern DWORD Get3DZ();
		DWORD __z = Get3DZ();
		Set3DZ( 1000 , 0 );

		RECT m_rcWnd = GetRECT();
		ClientToScreen(&m_rcWnd);
		//g_pCanvas->ClearAlpha(300, 230, 230, 130, 444, 16);//m_rc.left, m_rc.top, m_rc.right - m_rc.left, m_rc.bottom - m_rc.top, m_nWndClr);
		extern int Center( int, BOOL );
		int ox = Center( m_spr.GetWidth(), true );
		int oy = Center( m_spr.GetHeight(), false );
		m_pBtn1->m_nLeft = Center( m_pBtn1->m_nWidth, true );
		m_pBtn1->m_nTop = Center( m_pBtn1->m_nHeight, false ) + m_spr.GetHeight() / 2 - m_pBtn1->m_nHeight + 3;
		m_spr.DrawAlpha(ox, oy, 0);
		//DrawRect(g_pCanvas, &m_rcWnd);
		m_Font.SetLine(250, 20);
		DWORD dwClrOld = m_Font.GetColor();
		//m_Font.SetColor(55, 55, 55);
		//m_Font.DrawText(ox+24, oy+30, m_MessageQueue.front().c_str());
		m_Font.SetColor(222, 22, 22);
		m_Font.DrawText(ox+25, oy+31, m_MessageQueue.front().c_str());
		m_Font.SetColor(dwClrOld);

		Set3DZ( __z, 0);
	}
	else
	{
		RECT m_rcWnd = GetRECT();
		ClientToScreen(&m_rcWnd);
		//g_pCanvas->ClearAlpha(300, 230, 230, 130, 444, 16);//m_rc.left, m_rc.top, m_rc.right - m_rc.left, m_rc.bottom - m_rc.top, m_nWndClr);
		extern int Center( int, BOOL );
		int ox = Center( m_spr.GetWidth(), true );
		int oy = Center( m_spr.GetHeight(), false );
		m_pBtn1->m_nLeft = Center( m_pBtn1->m_nWidth, true );
		m_pBtn1->m_nTop = Center( m_pBtn1->m_nHeight, false ) + m_spr.GetHeight() / 2 - m_pBtn1->m_nHeight + 3;
		m_spr.DrawAlpha(ox, oy, 0);
		//DrawRect(g_pCanvas, &m_rcWnd);
		m_Font.SetLine(250, 20);
		DWORD dwClrOld = m_Font.GetColor();
		//m_Font.SetColor(55, 55, 55);
		//m_Font.DrawText(ox+24, oy+30, m_MessageQueue.front().c_str());
		m_Font.SetColor(222, 22, 22);
		m_Font.DrawText(ox+25, oy+31, m_MessageQueue.front().c_str());
		m_Font.SetColor(dwClrOld);
	}
}

void CDMessageBox::SetWndClr(int nClr)
{
	m_nWndClr = nClr;
}

int  CDMessageBox::MessageBox(LPCTSTR sMsg, int flag)
{
    if (flag == -1)
    {
        if (!m_MessageQueue.empty())
        {
            m_MessageQueue.front() = sMsg;
            return 0;
        }
    }
	if ( sMsg == NULL)
		return 0;
    m_MessageQueue.push(sMsg);
    // strcpy(m_InfoText, sMsg);

    Show();

	return 0;
}

int CDMessageBox::OnOK()
{
    m_MessageQueue.pop();
	SetCoordZ( false);
    return 0;
}

int CDMessageBox::OnExit()
{
    m_MessageQueue.pop();
	SetCoordZ( false);
    return 0;
}

void CDMessageBox::DrawRect(Canvas* pCanvas, RECT* rc, int nClrLT, int nClrRB)
{
	pCanvas->DrawLine(rc->left, rc->top, rc->right, rc->top, nClrLT);
	pCanvas->DrawLine(rc->left, rc->top, rc->left, rc->bottom, nClrLT);
	pCanvas->DrawLine(rc->right, rc->top, rc->right, rc->bottom , nClrRB);
	pCanvas->DrawLine(rc->left, rc->bottom, rc->right, rc->bottom, nClrRB);	
}
