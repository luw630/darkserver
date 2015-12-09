//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	File.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef File_H
#define File_H
//---------------------------------------------------------------------------
#define SEEK_ERROR		0xFFFFFFFF
//---------------------------------------------------------------------------
class XENGINE_API File
{
private:
	HANDLE		m_hFile;	// File Handle
	DWORD		m_dwLen;	// File Size
	DWORD		m_dwPos;	// File Pointer
public:
	File();
	~File();
	BOOL		Open(LPSTR FileName);//for read
	BOOL		Create(LPSTR FileName);//for write
	BOOL		Append(LPSTR FileName);//for append write
	void		Close();
	DWORD		Read(LPVOID lpBuffer, DWORD dwReadBytes);
	DWORD		Write(LPVOID lpBuffer, DWORD dwWriteBytes);
	DWORD		Seek(LONG lDistance, DWORD dwMoveMethod);
	DWORD		Tell();
	DWORD		Size();
};
//---------------------------------------------------------------------------
#endif
