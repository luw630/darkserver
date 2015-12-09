//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	PakList.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef PakList_H
#define PakList_H
//---------------------------------------------------------------------------
#include "PakData.h"
#include "Mutex.h"
//---------------------------------------------------------------------------
#define MAX_PAK		32
//---------------------------------------------------------------------------
class XENGINE_API PakList
{
private:
	PakData	    *m_PakFile;
	long		m_nNumber;
	long		m_nActive;
	Mutex		m_Mutex;
public:
	PakList();
	~PakList();
	BOOL		Open(LPSTR FileName);
	BOOL        AddPakFile(LPSTR FileName, DWORD dwVersion = 0);
	void		Close();
	DWORD		Read(LPVOID lpBuffer, DWORD dwLen);
	DWORD		Seek(LONG lOffset, DWORD dwMethod);
	DWORD		Tell();
	long		Search(LPSTR pFileName, PDWORD pOffset, PDWORD pLen);
	BOOL		Decode(TCodeInfo* pCodeInfo);
	void		SetActivePak(int nActive){m_nActive = nActive;};
	long		GetActivePak(){return m_nActive;};
	void		Lock(){m_Mutex.Lock();};
	void		Unlock(){m_Mutex.Unlock();};

	PakList *ActiveMySelf();
};
extern ENGINE_API PakList* g_pPakList;
//---------------------------------------------------------------------------
#endif
