//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	MemStack.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef MemStack_H
#define MemStack_H
//---------------------------------------------------------------------------
#define MAX_CHUNK	10
//---------------------------------------------------------------------------
class XENGINE_API MemStack
{
private:
	PBYTE		m_pStack[MAX_CHUNK];
	int			m_nStackTop;
	int 		m_nStackEnd;
	int			m_nChunkTop;
	int			m_nChunkSize;
public:
	MemStack();
	~MemStack();
	void		Init(int nChunkSize = 65536);
	PVOID		Push(int nSize);
	void		Free(PVOID pMem);
	BOOL		AllocNewChunk();
	void		FreeAllChunks();
	int			GetChunkSize();
	int			GetStackSize();
};
//---------------------------------------------------------------------------
#endif
