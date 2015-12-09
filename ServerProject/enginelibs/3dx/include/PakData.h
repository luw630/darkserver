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
	BYTE		Signature[8];		// 文件标识 ("DATAPACK")
	DWORD		FilesInPack;		// 包中的文件总数
	DWORD		CompressMethod;		// 使用的压缩算法
} TPakFileHeader;

// pack file index 
typedef struct {
	DWORD		FileId;				// 32bit Id值
	DWORD		FileOffset;			// 文件在包中的偏移
	DWORD		FileLength;			// 文件的原始长度
} TPakFileIndex;
//---------------------------------------------------------------------------
//class ENGINE_API PakData
//{
//private:
//	File		m_File;				// PAK文件
//	MemClass	m_MemIndex;			// 索引内存
//	Codec*		m_pCodec;			// 解码器
//	DWORD		m_nFilesInPack;		// 包中的文件总数
//	DWORD		m_nCompressMethod;	// 压缩算法
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
	DWORD		m_nFilesInPack;		// 包中的文件总数
	DWORD		m_nCompressMethod;	// 压缩算法
	File		m_File;				// PAK文件
	MemClass	m_MemIndex;			// 索引内存
	Codec*		m_pCodec;			// 解码器

    std::map<DWORD, TPakFileIndex *> m_IndexTree; // 索引树
};

//---------------------------------------------------------------------------
#endif
