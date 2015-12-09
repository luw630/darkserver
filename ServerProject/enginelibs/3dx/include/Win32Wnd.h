//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	Win32Wnd.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef Win32Wnd_H
#define Win32Wnd_H
//---------------------------------------------------------------------------
XENGINE_API HWND	g_GetMainHWnd();
XENGINE_API void	g_SetMainHWnd(HWND hWnd);
XENGINE_API HWND	g_GetDrawHWnd();
XENGINE_API void	g_SetDrawHWnd(HWND hWnd);
//---------------------------------------------------------------------------
ENGINE_API void	g_GetClientRect(LPRECT lpRect);
ENGINE_API void	g_ClientToScreen(LPRECT lpRect);
ENGINE_API void	g_ScreenToClient(LPRECT lpRect);
ENGINE_API void g_ScreenToClient(LPPOINT lpPoint);
ENGINE_API void	g_ChangeWindowStyle();
//---------------------------------------------------------------------------
#endif
