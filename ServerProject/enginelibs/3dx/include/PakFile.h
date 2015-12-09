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
	File		m_File;			// ��ʵ�ļ�(���ڰ���)
	MemClass	m_MemFile;		// file buffer
	MemClass	m_MemRead;		// read buffer
	MemClass	m_MemBlock;		// block buffer
	DWORD		m_dwFileOfs;	// ��һ���ƫ����,0 ��ʾ�ǵ����ļ�
	DWORD		m_dwFileLen;	// �ļ�����
	DWORD		m_dwFilePtr;	// ��ָ���λ��(������λ��)
	DWORD		m_dwDataPtr;	// ��ѹ�������λ��
	PBYTE		m_pBuffer;		// ������ָ��(����̶�Ϊ64K)
	int			m_nBlocks;		// �ļ���Ϊ64�������
	PWORD		m_pBlocks;		// ÿ��ĳ���
	BOOL		m_nPackage;		// �ļ�������
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
