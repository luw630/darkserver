//---------------------------------------------------------------------------
// Game Engine (c) 1999-2003 by Dreamwork
//
// File:	Win32.h
// Date:	2001.10.10
// Code:	Dreamwork
// Desc:	Percompiled header files of Win32 Platform
//---------------------------------------------------------------------------
#ifndef Win32_H
#define Win32_H
#define DIRECTINPUT_VERSION 0x800
//---------------------------------------------------------------------------
#define ENGINE_API
#ifdef ENGINE_EXPORT
#   define XENGINE_API __declspec(dllexport) 
#else
#   ifdef ENGINE_IMPORT
#       define XENGINE_API __declspec(dllimport) 
#   else
#       define XENGINE_API
#   endif
#endif
//---------------------------------------------------------------------------
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//---------------------------------------------------------------------------
// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
//---------------------------------------------------------------------------
#include <stdio.h>
//#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <dinput.h>
#include <ddraw.h>
//---------------------------------------------------------------------------
#endif
