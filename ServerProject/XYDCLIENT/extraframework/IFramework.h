#pragma once
#include <UNKNWN.H>

struct SCreateParameter
{
    DWORD cbSize;
};

interface IFramework
{
    virtual ~IFramework() {}
    virtual IFramework *GetInterface(const __int32 IID) = 0;                // 根据IID获取相应接口

    // ****************************************
    // 接口定义
    virtual HRESULT Init(SCreateParameter *pParam) = 0;                     // 初始化
    virtual HRESULT Loop(void) = 0;                                         // 执行循环
    virtual HRESULT Exit(void) = 0;                                         // 结束

    virtual void UpdateAI(void) = 0;                                        // 由调用者控制节奏的AI更新入口
    virtual void UpdateShow(void) = 0;                                      // 由调用者控制节奏的实现更新入口
    virtual void InputHandle(int iKeyState, int vKey, int x, int y) = 0;    // 本地操作控制入口

    // ****************************************
    // 创建对象实体
    static IFramework *CreateInstance(const __int32 IID, SCreateParameter *pParam);
};
