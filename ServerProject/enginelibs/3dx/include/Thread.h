//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	Thread.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef Thread_H
#define Thread_H
//---------------------------------------------------------------------------
typedef void (* TThreadFunc)(void* arg);
//---------------------------------------------------------------------------
class XENGINE_API Thread
{
private:
	HANDLE			m_ThreadHandle;
	DWORD			m_ThreadId;
	TThreadFunc 	m_ThreadFunc;
	LPVOID			m_ThreadParam;
public:
	Thread();
	~Thread();
	BOOL			Create(TThreadFunc lpFunc, void* lpParam);
	void			Destroy();
	void			Suspend();
	void			Resume();
	BOOL			IsRunning();
	void			WaitForExit();
	int				GetPriority();
	BOOL			SetPriority(int priority);
private:
	DWORD			ThreadFunction();
	static			DWORD WINAPI ThreadProc(LPVOID lpParam);
};
//---------------------------------------------------------------------------
#endif
