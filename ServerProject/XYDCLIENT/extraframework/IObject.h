#pragma once

const __int32 IID_OBJECT_VISUAL = 0x13131af6; 

interface IObject
{
    IObject(const LPCSTR szKey) {}
    virtual ~IObject() {}
    virtual IObject *GetInterface(const __int32 IID) = 0;   // 根据IID获取相应接口

    // ****************************************
    // 接口定义
    virtual HRESULT Init(SCreateParameter *pParam) = 0;     // 初始化对象
    virtual HRESULT Exit(void) = 0;                         // 释放资源

    virtual void UpdateAI(void) = 0;                        // 由调用者控制节奏的AI更新入口
    virtual void UpdateShow(int x, int y) = 0;              // 由调用者控制节奏的实现更新入口，[x，y]是父对象的显示位置

    virtual LPCSTR GetKey(void) = 0;                        // 返回自己的查询关键字

    // ****************************************
    // 创建对象实体
    static IObject *CreateInstance(const __int32 IID, const LPCSTR szKey, SCreateParameter *pParam);
};

struct SObjectCreateParameter :
    public SCreateParameter
{
    dwt::stringkey<char[STRING_KEY_LEN]> InitFunction;
};