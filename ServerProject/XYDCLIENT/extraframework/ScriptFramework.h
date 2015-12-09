#pragma once

// 脚本框架的基本结构：
// 脚本引擎、场景、对象

#include "iregion.h"

class CScriptFramework :
    public IFramework
{
public:
    IFramework *GetInterface(const __int32 IID);                // 根据IID获取相应接口

private:
    // ****************************************
    // 接口定义
    HRESULT Init(SCreateParameter *pParam);                     // 初始化
    HRESULT Loop(void);                                         // 执行循环
    HRESULT Exit(void);                                         // 结束

    void UpdateAI(void);                                        // 由调用者控制节奏的AI更新入口
    void UpdateShow(void);                                      // 由调用者控制节奏的实现更新入口
    void InputHandle(int iKeyState, int vKey, int x, int y);    // 本地操作控制入口

public:
    HRESULT LoadEventFile(LPCSTR filename);                     // 使用脚本文件初始化事件

public:
    IRegion *GetCurRegion();

public:
    HRESULT CreateRegion(const __int32 iCreateIID, SCreateParameter *pParam);   // 创建脚本场景
    void ClearRegion();     // 清空当前脚本场景

public:
    CScriptFramework(void);
    ~CScriptFramework(void);

private:
    IRegion *m_pRegion;         // 当前用到的脚本场景
};

