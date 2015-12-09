#pragma once
#include "unknwn.h" 
#include "guievent.h" 

interface IGuiObject;
struct GuiObjConverter {
    IGuiObject *ptr;
    GuiObjConverter( IGuiObject *ptr ) : ptr(ptr) {}
    IGuiObject* operator -> () { return ptr; }
    void TypeNameAssert( LPCSTR type );
    template < typename _T >
    operator _T* () {
        TypeNameAssert( typeid(_T).name() ); // 这个地方由于没有用到多态，所以typeid可以在不加-GR的情况加使用
        return (_T*)ptr;
    }
};

interface IGuiObject
{
    IGuiObject() { m_GuiType[0] = 0; m_GuiName[0] = 0; m_GuiId = 0; }
    virtual ~IGuiObject() {}

    virtual BOOL OnCreate(LPARAM lParam) = 0;
    virtual bool AddChild( GuiObjConverter pGuiObject ) = 0;
    virtual bool SetParameter(LPCSTR szParameterName, LPARAM lParam) = 0;

    char    m_GuiType[128];
    char    m_GuiName[128];
    int     m_GuiId;
};
typedef EventSet<IGuiObject> GuiEvent;

class CGuiFramework
{
public:
    IGuiObject *GuiInit(LPCSTR szXmlFramework);                             // 初始化GUI框架

public:
    virtual IGuiObject *GeneratGuiObject(LPCSTR szRegisteredName);          // 创建一个GUI对象

public:
    CGuiFramework(void);
    virtual ~CGuiFramework(void);
};
