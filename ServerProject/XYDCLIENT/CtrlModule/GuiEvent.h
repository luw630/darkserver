/****************************************************************
//  FileName:   GuiEvent
//  Author:		�˳�
//  Create:		2010.2.28
//  Company:    DreamWork
****************************************************************/

#pragma once
#include <list>

#define INVOKE(THIS,FUN) (THIS->*FUN)
//�¼������ӿ�
struct IGuiEventArgs{ };
struct MouseEventArgs: public IGuiEventArgs
{
	MouseEventArgs(int _x, int _y) { x=_x; y=_y; }
	int x; int y;
};

struct LDownPrepareArgs: public IGuiEventArgs
{
	LDownPrepareArgs(bool enable) { bEnable = enable; }
	bool bEnable;
};


//�¼��ӿ�
template<typename TSender>
class IGuiEvent
{
protected:
	IGuiEvent(){}
	virtual ~IGuiEvent(){}
public:
	virtual void Release( ){ delete this ;}
	//�����¼�
	virtual int  Invoke( TSender* pSender, IGuiEventArgs* pArgs ) = 0;
};


//�����¼���,�����¼����������¼�,���ǽ��¼��ȴ�������,�ٷַ���������Ķ���. �����𵽽ӿ�ת������.
template<typename TS,typename DC>
class DelegateEvent : public IGuiEvent<TS>
{
public:
	typedef int (DC::*Handle)( TS* pSender, IGuiEventArgs* pArgs );
private:
	DC*	mThis;
	Handle mHandle;

public:
	DelegateEvent(DC* refThis, Handle handle)
	{ 
		mThis = refThis;
		mHandle = handle;
	}
	virtual void Release( ){ delete this ;}

	virtual int  Invoke( TS* pSender, IGuiEventArgs* pArgs )
	{
		return INVOKE(mThis,mHandle)(pSender,pArgs);
	}
};


//CEvent �������𴴽�һ�������¼�����.
//����: pObjΪ������Ķ���, handleΪ���������Ĵ�����.
template<typename TS,typename DC>
IGuiEvent<TS>* CEvent( DC* pObj, int (DC::*handle)(TS*, IGuiEventArgs*) )
{
	return new DelegateEvent<TS,DC>(pObj,handle);
}


//�¼�����,�����Զ���ص���֧��
template<typename TSender>
class EventSet
{
public:
	typedef IGuiEvent<TSender> Event;
public:
	EventSet(){};
	~EventSet()
	{ 
		std::list<Event*>::iterator i=mEventList.begin();
		for( i; i!=mEventList.end(); i++ )
		{
			//TSender::ReleaseEvent(*i);
			(*i)->Release(); *i=0;
		}
		mEventList.clear();
	}

	//����¼�
	void	operator += ( Event* pHandle ){ if(pHandle)  mEventList.push_back(pHandle); }
	//�ص��¼�
	int		Invoke( TSender* pSender, IGuiEventArgs* pArgs )
	{
		int n=0;
		std::list<Event*>::iterator i=mEventList.begin();
		for( i; i!=mEventList.end(); i++ )
		{
			n = (*i)->Invoke(pSender,pArgs);
		}
		return n;
	}
private:
	std::list<Event*> mEventList;
};
