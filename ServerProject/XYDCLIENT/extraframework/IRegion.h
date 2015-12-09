#pragma once

DECLARE_HANDLE(HScriptObject);

interface IRegion
{
    virtual ~IRegion() {}
    virtual IRegion *GetInterface(const __int32 IID) = 0;           // 根据IID获取相应接口

    // ****************************************
    // 接口定义
    virtual HRESULT Init(SCreateParameter *pParam) = 0;             // 初始化场景
    virtual HRESULT Exit(void) = 0;                                 // 释放资源

    virtual void UpdateAI(void) = 0;                                // 由调用者控制节奏的AI更新入口
    virtual void UpdateShow(void) = 0;                              // 由调用者控制节奏的实现更新入口，[x,y]是场景的显示起始位置

    virtual HScriptObject CreateObject(const __int32 iCreateIID, LPCSTR szKey, SCreateParameter *pParam) = 0;   // 添加一个脚本对象
    virtual HScriptObject LocateObject(LPCSTR szKey) = 0;           // 通过关键字获取一个脚本对象
    virtual HRESULT       DeleteObject(HScriptObject hObj) = 0;     // 释放一个脚本对象（可能会导致对象被删除）

    // ****************************************
    // 创建对象实体
    static IRegion *CreateInstance(const __int32 IID, SCreateParameter *pParam);
};
