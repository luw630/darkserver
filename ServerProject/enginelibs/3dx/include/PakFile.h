//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	PakFile.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef PakFile_H
#define PakFile_H
//---------------------------------------------------------------------------
#include "File.h"
#include "MemClass.h"

enum EREADMODE
{
    DISKFILE_PRECEDENCE,
    PACKFILE_PRECEDENCE,
    DISKFILE_ONLY,
    PACKFILE_ONLY
};

//---------------------------------------------------------------------------
XENGINE_API void g_SetPakFileMode(int nFileMode);
//---------------------------------------------------------------------------
class XENGINE_API PakFile
{
private:
	File		m_File;			// 真实文件(不在包中)
	MemClass	m_MemFile;		// file buffer
	MemClass	m_MemRead;		// read buffer
	MemClass	m_MemBlock;		// block buffer
	DWORD		m_dwFileOfs;	// 第一块的偏移量,0 表示是单独文件
	DWORD		m_dwFileLen;	// 文件长度
	DWORD		m_dwFilePtr;	// 读指针的位置(解码后的位置)
	DWORD		m_dwDataPtr;	// 在压缩包里的位置
	PBYTE		m_pBuffer;		// 读缓冲指针(缓冲固定为64K)
	int			m_nBlocks;		// 文件分为64块个个数
	PWORD		m_pBlocks;		// 每块的长度
	BOOL		m_nPackage;		// 文件包索引
	BOOL		ReadBlock(PBYTE pBuf, int nBlock);
	BOOL		OpenPak(LPSTR FileName);
	DWORD		ReadPak(PVOID buffer, DWORD size);
	DWORD		SeekPak(LONG offset, DWORD method);
	
public:
	PakFile();
	~PakFile();
	BOOL		Open(LPSTR FileName);
	void		Close();
	DWORD		Read(PVOID buffer, DWORD size);
	DWORD		Seek(LONG offset, DWORD method);
	DWORD		Tell();
	DWORD		Size();
	BOOL		Save(LPSTR FileName);
};
//---------------------------------------------------------------------------
#endif
