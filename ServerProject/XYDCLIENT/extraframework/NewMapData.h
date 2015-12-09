#pragma once
#include "mainworld\mapdata.h"

#include "iobject.h"

class CTileSpr : 
    public IObject,
    public STimeFrame
{
public:
    IObject *GetInterface(const __int32 IID);   // 根据IID获取相应接口

private:
    // ****************************************
    // 接口定义
    HRESULT Init(SCreateParameter *pParam);     // 初始化对象
    HRESULT Exit(void);                         // 释放资源

    void UpdateAI(void);                        // 由调用者控制节奏的AI更新入口
    void UpdateShow(int x, int y);              // 由调用者控制节奏的实现更新入口，[x，y]是父对象的显示位置

    LPCSTR GetKey(void);                        // 返回自己的查询关键字

public:
    CTileSpr(void);
    ~CTileSpr(void);

public:
    WORD wSprIndex;
    DWORD dwAudioFlag;

    int m_x;
    int m_y;
};

class CNewMapData :
    public CMapData
{
public:
	int LoadMap(long MapIndex);

public:
    CNewMapData(void);
    ~CNewMapData(void);

public:
    CTileSpr *pTileSprs;
};
