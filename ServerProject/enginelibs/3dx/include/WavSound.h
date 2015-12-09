//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	WavSound.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef WavSound_H
#define WavSound_H

#include <list>

//---------------------------------------------------------------------------
#define BUFFER_COUNT		3	// sound buffer count
//---------------------------------------------------------------------------
class XENGINE_API WavSound
{
private:
	LPDIRECTSOUNDBUFFER 	m_Buffer[BUFFER_COUNT];
	int						GetFreeBuffer();
	int						GetPlayBuffer();
public:
	WavSound();
	~WavSound();
	BOOL					Load(LPSTR FileName);
	void					Free();
	void					Play(int nPan, int nVol, BOOL bLoop);
	void					Stop();
	void					SetPan(int nPan);
	void					SetVolume(int nVolume);
	BOOL					IsPlaying();
};
//---------------------------------------------------------------------------

class ENGINE_API WavSoundMultiple
{
public:
	bool Load(const char *filename);
	void Free();

	int	 Play(int nPan, int nVol, bool bLoop);
	void Stop(int index);

	void SetPan(int index, int nPan);
	void SetVolume(int index, int nVolume);

public:
	WavSoundMultiple();
	~WavSoundMultiple();

private:
	LPDIRECTSOUNDBUFFER GetFreeBuffer();
	LPDIRECTSOUNDBUFFER GetPlayBuffer(int index);

private:
	LPDIRECTSOUNDBUFFER 	        m_RawBuffer;
    LPDIRECTSOUNDBUFFER             m_FindCache;
    std::list<LPDIRECTSOUNDBUFFER>  m_PlayingBuffer;
};

#endif