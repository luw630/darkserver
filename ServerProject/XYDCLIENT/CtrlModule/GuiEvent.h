/****************************************************************
//  FileName:   GuiEvent
//  Author:		邓超
//  Create:		2010.2.28
//  Company:    DreamWork
****************************************************************/

#pragma once
#include <list>

#define INVOKE(THIS,FUN) (THIS->*FUN)
//事件参数接口
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


//事件接口
template<typename TSender>
class IGuiEvent
{
protected:
	IGuiEvent(){}
	virtual ~IGuiEvent(){}
public:
	virtual void Release( ){ delete this ;}
	//调用事件
	virtual int  Invoke( TSender* pSender, IGuiEventArgs* pArgs ) = 0;
};


//代理事件类,代理事件本身不处理事件,而是将事件先代理下来,再分发给被代理的对象. 可以起到接口转换作用.
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


//CEvent 函数负责创建一个代码事件对象.
//参数: pObj为被代理的对象, handle为被代理对象的处理函数.
template<typename TS,typename DC>
IGuiEvent<TS>* CEvent( DC* pObj, int (DC::*handle)(TS*, IGuiEventArgs*) )
{
	return new DelegateEvent<TS,DC>(pObj,handle);
}


//事件集合,用来对多个回调的支持
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

	//添加事件
	void	operator += ( Event* pHandle ){ if(pHandle)  mEventList.push_back(pHandle); }
	//回调事件
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
