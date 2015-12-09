//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	Engine.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef Engine_H
#define Engine_H
//---------------------------------------------------------------------------
#include "Win32.h"
//#include "Debug.h"
#include "EDDraw.h"
#include "EDSound.h"
//#include "EDInput.h"

#include "File.h"
#include "FilePath.h"
//#include "FileDialog.h"
//#include "Timer.h"
//#include "MemBase.h"
//#include "Random.h"
#include "PakList.h"
#include "PaKFile.h"
#include "IniFile.h"
#include "Canvas.h"
#include "Sprite.h"
//#include "Font.h"
#include "Win32App.h"
#include "colors.h"

// 3DÀ©Õ¹
#include "3dx_Mgr.h"

//---------------------------------------------------------------------------
XENGINE_API BOOL	g_InitEngine();
XENGINE_API void	g_ExitEngine();
//---------------------------------------------------------------------------

#endif
