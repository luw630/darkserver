//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	EDSound.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef EDSound_H
#define EDSound_H
//---------------------------------------------------------------------------
class XENGINE_API EDirectSound
{
private:
	LPDIRECTSOUND			m_pDirectSound;
	LPDIRECTSOUNDBUFFER		m_pPrimarySoundBuffer;
public:
	EDirectSound();
	~EDirectSound();
	BOOL					Init();
	void					Exit();
	LPDIRECTSOUND			GetDirectSound(){return m_pDirectSound;};
private:
	BOOL					CreateDirectSound();
	BOOL					CreatePrimarySoundBuffer();
};
extern ENGINE_API EDirectSound* g_pDirectSound;
//---------------------------------------------------------------------------
#endif