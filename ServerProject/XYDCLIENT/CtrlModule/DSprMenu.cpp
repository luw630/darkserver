#include "StdAfx.h"
#include "dsprmenu.h"

CDSprMenu::CDSprMenu(void)
{
    m_sSprFileName_Bk[0] = 0;
}

CDSprMenu::~CDSprMenu(void)
{
}

BEGIN_MY_MESSAGE_MAP(CDSprMenu)
MESSAGE_MAP(m_pBtnInfo, OnInfo())  
END_MY_MESSAGE_MAP

bool CDSprMenu::SetParameter(LPCSTR szParameterName, LPARAM lParam)
{    
    if(strcmp(szParameterName, "SprFileName_Bk") == 0)
        dwt::strcpy(m_sSprFileName_Bk, (LPSTR)lParam, 128);

    CDFace::SetParameter(szParameterName, lParam);
    return false;
}

bool CDSprMenu::AddChild( GuiObjConverter pGuiObject )
{
    if(strcmp(pGuiObject->m_GuiName, "Íæ¼ÒÐÅÏ¢") == 0)
        m_pBtnInfo = (CDSprButton*)pGuiObject;

    return CDFace::AddChild(pGuiObject);
}

int		CDSprMenu::OnCreate(LPARAM lParam)
{
	//m_spr.Load(m_sSprFileName_Bk);
	return 1;
}

int		CDSprMenu::OnHide()
{
	return 1;
}

int		CDSprMenu::InitByBinary(LPARAM lParam)
{
	return 1;
}

void	CDSprMenu::Update()
{	
	RECT rc = GetRECT();
	ClientToScreen(&rc);
	//m_spr.Draw(320,250,0);
}

void CDSprMenu::ShowIt(char* szName, bool bShow)
{ 
	dwt::strcpy(m_szName, szName, 11); 
	szName[10] = 0;

	Show(bShow);
}

int CDSprMenu::OnInfo()
{
    Show(false);
    return 0;
}

int		CDSprMenu::KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y)
{
	if (iKeyState == MS_LDOWN)
	{
        if ((x < 300) || (x > 370) || (y < 300) || (y > 470)) 
		    Show(false); 
	}

    CDDialog::KeyMsAction(iKeyState, vKey, x, y);

	return RET_NONE;
}
