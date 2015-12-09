//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	Node.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef	Node_H
#define	Node_H
//---------------------------------------------------------------------------
class ENGINE_API Node
{
public:
	Node* m_pNext;
	Node* m_pPrev;

public:
	Node(void);
	virtual ~Node(){};
	Node* GetNext(void);
	Node* GetPrev(void);
	void InsertBefore(Node* pNode);
	void InsertAfter(Node* pNode);
	void Remove(void);
	BOOL IsLinked(void);
};
//---------------------------------------------------------------------------
#endif
