//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	Mutex.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef Mutex_H
#define Mutex_H
//---------------------------------------------------------------------------
#define SINGLE_PROCESS
//---------------------------------------------------------------------------
class XENGINE_API Mutex
{
private:
#ifdef SINGLE_PROCESS
	CRITICAL_SECTION m_CriticalSection;//用于单进程的线程同步
#else
	HANDLE m_hMutex;//用于多进程的线程同步
#endif
public:
    Mutex();
    ~Mutex();
    void Lock(void);
    void Unlock(void);
};
//---------------------------------------------------------------------------
#endif
