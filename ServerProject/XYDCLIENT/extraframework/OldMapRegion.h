#pragma once
#include "iregion.h"

#include "iobject.h"

#include "NewMapData.h"

class COldMapRegion :
    public IRegion
{
public:
    IRegion *GetInterface(const __int32 IID);           // 根据IID获取相应接口

private:
    // ****************************************
    // 接口定义
    HRESULT Init(SCreateParameter *pParam);             // 初始化场景
    HRESULT Exit(void);                                 // 释放资源

    void UpdateAI(void);                                // 由调用者控制节奏的AI更新入口
    void UpdateShow();                      // 由调用者控制节奏的实现更新入口，[x,y]是场景的显示起始位置

    HScriptObject CreateObject(const __int32 iCreateIID, LPCSTR szKey, SCreateParameter *pParam);         // 添加一个脚本对象
    HScriptObject LocateObject(LPCSTR szKey);           // 通过关键字获取一个脚本对象
    HRESULT       DeleteObject(HScriptObject hObj);     // 释放一个脚本对象（可能会导致对象被删除）

public:
    void GetShowPos(int &x, int &y);
    void SetShowPos(int x, int y);

    void MoveRegion(int x1, int y1, int x2, int y2);

public:
    HScriptObject GetObjectWithKey(LPCSTR szKey);

public:
    COldMapRegion(void);
    ~COldMapRegion(void);

private:
    std::map<dwt::stringkey<char[STRING_KEY_LEN]>, IObject *> m_ObjectMap;      //场景上的对象映射表

    CNewMapData m_MapData; // 游戏用场景地图数据

    int m_xScreen, m_yScreen;   // 用于指定当前场景的显示起始位置


};
