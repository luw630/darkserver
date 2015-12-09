//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	Mp3Music.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef Mp3Music_H
#define Mp3Music_H
//---------------------------------------------------------------------------
#include "Music.h"
#include "MemClass.h"
#include "PakFile.h"
//---------------------------------------------------------------------------
class XENGINE_API Mp3Music : public Music
{
private:
	PakFile  m_Mp3File;
	MemClass m_Mp3Buffer;
protected:
	PBYTE	mp3_buffer;
	PBYTE	mp3_bufptr;
	int		mp3_bufbytes;
	int		mp3_frmbytes;
	virtual BOOL  Mp3Init();
	virtual BOOL  Mp3FillBuffer();
	virtual DWORD Mp3Decode(PBYTE lpPcmBuf, DWORD dwBufLen);
	virtual BOOL  Mp3FileOpen(LPSTR FileName);
	virtual DWORD Mp3FileRead(PBYTE pBuf, DWORD dwLen);
	virtual DWORD Mp3FileSeek(LONG lOffset);
	virtual void  InitSoundFormat();
	virtual DWORD ReadWaveData(LPBYTE lpBuf, DWORD dwLen);
public:
	virtual BOOL Open(LPSTR FileName);
	virtual void Seek(int nPercent);
	virtual void Rewind();
	virtual void Close();
};
//---------------------------------------------------------------------------
#endif