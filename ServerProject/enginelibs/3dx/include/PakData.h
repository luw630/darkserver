//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	PakData.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef PakData_H
#define PakData_H
//---------------------------------------------------------------------------
#include <map>

#include "MemClass.h"
#include "File.h"
#include "Codec.h"
//---------------------------------------------------------------------------
// pack file header
typedef struct {
	BYTE		Signature[8];		// �ļ���ʶ ("DATAPACK")
	DWORD		FilesInPack;		// ���е��ļ�����
	DWORD		CompressMethod;		// ʹ�õ�ѹ���㷨
} TPakFileHeader;

// pack file index 
typedef struct {
	DWORD		FileId;				// 32bit Idֵ
	DWORD		FileOffset;			// �ļ��ڰ��е�ƫ��
	DWORD		FileLength;			// �ļ���ԭʼ����
} TPakFileIndex;
//---------------------------------------------------------------------------
//class ENGINE_API PakData
//{
//private:
//	File		m_File;				// PAK�ļ�
//	MemClass	m_MemIndex;			// �����ڴ�
//	Codec*		m_pCodec;			// ������
//	DWORD		m_nFilesInPack;		// ���е��ļ�����
//	DWORD		m_nCompressMethod;	// ѹ���㷨
//
//public:
//	PakData();
//	~PakData();
//	BOOL		Open(LPSTR FileName);
//	DWORD		Read(LPVOID lpBuffer, DWORD dwLength);
//	DWORD		Seek(LONG lOffset, DWORD dwMethod);
//	DWORD		Tell();
//	void		Close();
//	BOOL		Search(LPSTR pFileName, PDWORD pOffset, PDWORD pLen);
//	BOOL		Decode(TCodeInfo* pCodeInfo);
//};

class ENGINE_API PakData
{
public:
	PakData();
	~PakData();
	BOOL		Open(LPSTR FileName);
	DWORD		Read(LPVOID lpBuffer, DWORD dwLength);
	DWORD		Seek(LONG lOffset, DWORD dwMethod);
	DWORD		Tell();
	void		Close();
	BOOL		Search(LPSTR pFileName, PDWORD pOffset, PDWORD pLen);
	BOOL		Decode(TCodeInfo* pCodeInfo);

protected:
	DWORD		m_nFilesInPack;		// ���е��ļ�����
	DWORD		m_nCompressMethod;	// ѹ���㷨
	File		m_File;				// PAK�ļ�
	MemClass	m_MemIndex;			// �����ڴ�
	Codec*		m_pCodec;			// ������

    std::map<DWORD, TPakFileIndex *> m_IndexTree; // ������
};

//---------------------------------------------------------------------------
#endif
