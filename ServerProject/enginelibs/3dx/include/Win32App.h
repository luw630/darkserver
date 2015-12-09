//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	Win32App.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Win32App Class
//---------------------------------------------------------------------------
#ifndef Win32App_H
#define Win32App_H
//---------------------------------------------------------------------------
#define ENGINE_ICON 101

//---------------------------------------------------------------------------
class ENGINE_API Win32App
{
protected:
	char	m_szClass[32];
	char	m_szTitle[32];
	BOOL	m_bShowMouse;
	BOOL	m_bActive;
	BOOL	m_bMultiGame;
	virtual	BOOL	InitClass(HINSTANCE hInstance);
	virtual	BOOL	InitWindow(HINSTANCE hInstance);
	virtual	BOOL	HideMouse();
	virtual	BOOL	GameInit();
	virtual BOOL	GameLoop();
	virtual BOOL	GameExit();
	virtual int		HandleInput(UINT uMsg, WPARAM wParam, LPARAM lParam) { return 0; }
public:
	Win32App();
	virtual BOOL	Init(HINSTANCE hInstance,char* AppName="EngineLib");
	virtual void	Run();
	virtual	void	ShowMouse(BOOL bShow);
	virtual void	SetMultiGame(BOOL bMulti);
	virtual LRESULT	MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	void			SetMouseHoverTime(unsigned int uHoverTime);
private:
	void			GenerateMsgHoverMsg();
	unsigned int	m_uMouseHoverTimeSetting;
	unsigned int	m_uMouseHoverStartTime;
	int				m_nLastMousePos;
	unsigned int	m_uLastMouseStatus;
};
//---------------------------------------------------------------------------
#endif
