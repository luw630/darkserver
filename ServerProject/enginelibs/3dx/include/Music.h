//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	Music.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef Music_H
#define Music_H
//---------------------------------------------------------------------------
#define NUMEVENTS	3
//---------------------------------------------------------------------------
#include "Thread.h"
//---------------------------------------------------------------------------
class XENGINE_API Music
{
protected:
	WAVEFORMATEX			m_WaveFormat;
	LPDIRECTSOUNDBUFFER 	m_pSoundBuffer;
	LPDIRECTSOUNDNOTIFY 	m_pSoundNotify;
	DSBPOSITIONNOTIFY		m_PosNotify[NUMEVENTS];
	HANDLE					m_hEvent[NUMEVENTS];
	DWORD					m_dwBufferSize;
	BOOL					m_bLoop;
	Thread					m_Thread;
public:
	Music();
	virtual ~Music();
	virtual BOOL			Open(LPSTR FileName);
	virtual void			Close();
	virtual void			Play(BOOL bLoop = FALSE);
	virtual void			Stop();
	virtual void			Rewind();
	virtual void			SetVolume(LONG lVolume);
	virtual BOOL			HandleNotify();
protected:
	virtual BOOL			Init();
	virtual void			InitSoundFormat();
	virtual BOOL			InitSoundBuffer();
	virtual void			FreeSoundBuffer();
	virtual BOOL			InitSoundNotify();
	virtual void			FreeSoundNotify();
	virtual BOOL			InitEventHandle();
	virtual void			FreeEventHandle();
	virtual BOOL			FillBufferWithSound(DWORD dwPos);
	virtual BOOL			FillBufferWithSilence();
	virtual DWORD			ReadWaveData(LPBYTE lpBuf, DWORD dwLen);
	static	void			ThreadFunction(void* lpParam);
};
//---------------------------------------------------------------------------
#endif