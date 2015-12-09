#include "StdAfx.h"
#include "iscriptframework.h"
#include "oldmapregion.h"

#include "visualobject.h"

#include "SManager.h"
#include "ScriptFramework.h"
////////////////////////////////////////
// extra data
#include "ResManagerModule\ResManagerModule.h"
extern CResManagerModule *g_pResManager;

#include "ResManagerModule/DCanvasEx.h"
extern DCanvasEx *g_Canvas;

extern IRegion *g_Region;
////////////////////////////////////////
unsigned __int32 GetCycleTime();

struct SSortPoint
{
    SSortPoint(const int x, const int y)
    {
        m_x = x;
        m_y = y;
    }

    bool operator < (const SSortPoint &other) const
    {
        if (m_y < other.m_y) 
            return true;

        else if (m_y > other.m_y) 
            return false;

        return m_x < other.m_x;
    }

    int m_x;
    int m_y;
};

#define _TILEW 64
#define _TILEH 32

COldMapRegion::COldMapRegion(void)
{
    m_xScreen = 0;
    m_yScreen = 0;
}

COldMapRegion::~COldMapRegion(void)
{
    m_xScreen = 0;
    m_yScreen = 0;
}

IRegion *COldMapRegion::GetInterface(const __int32 IID)
{
    return (IID == IID_REGION_OLD_MAP_REGION) ? this : NULL;
}

HRESULT COldMapRegion::Init(SCreateParameter *pParameter)
{
    if (pParameter == NULL)
        return S_FALSE;

    // 无效的参数，大小不同可能导致操作溢出
    if (pParameter->cbSize != sizeof(SOldMapRegionParam))
        return S_FALSE;

    SOldMapRegionParam *pParam = (SOldMapRegionParam *)pParameter;

    // 载入场景地图
    if (m_MapData.LoadMap(pParam->lMapIndex) == 0)
    {
        // 载入游戏用场景地图失败
        return S_FALSE;
    }

    // 启动初始化脚本函数
	Script::g_Script.CallFunc(pParam->InitFunction, this);

    return S_OK;
}

HRESULT COldMapRegion::Exit(void)
{
    std::map<dwt::stringkey<char[STRING_KEY_LEN]>, IObject *>::iterator it = m_ObjectMap.begin();
    while (it != m_ObjectMap.end())
    {
        try
        {
            it->second->Exit();
            delete it->second;
        }
        catch (...)
        {
            // trace exception infomation
            // ...
        }

        it++;
    }

    m_ObjectMap.clear();

    return S_OK;
}

void COldMapRegion::UpdateAI(void)
{
    std::map<dwt::stringkey<char[STRING_KEY_LEN]>, IObject *>::iterator it = m_ObjectMap.begin();
    std::list< dwt::stringkey<char[STRING_KEY_LEN]> > RemoveList;

    while (it != m_ObjectMap.end())
    {
        try
        {
            it->second->UpdateAI();
        }
        catch (...)
        {
            // set invalid object
            RemoveList.push_back(it->first);

            // trace exception infomation
            // ...
        }

        it++;
    }

    // 无效对象清除，因为是无效对象，就不再对其进行任何操作了，包括删除（Call-Exit）
    while (RemoveList.size())
    {
        it = m_ObjectMap.find(RemoveList.front());
        if (it != m_ObjectMap.end())
            m_ObjectMap.erase(it);

        RemoveList.pop_front();
    }
}

void COldMapRegion::UpdateShow()
{
    // 显示场景以及场景上的所有对象
    int x = m_xScreen;
    int y = m_yScreen;

    if (m_MapData.m_BgImage.GetWidth() == 0)
        g_Canvas->FillCanvas(0);
    else
        m_MapData.m_BgImage.Draw(0, 0);

    // [x,y]是以像素为坐标的场景显示起始位置

    // 根据起始像素位置算出来起始的Tile位置

    int xSegTile = x / _TILEW;
    int ySegTile = y / _TILEH;

    int xOffset = x % _TILEW;
    int yOffset = y % _TILEH;

    // 显示地表
    {
        for (int yIndex = 0; yIndex < (600 / _TILEH + 2); yIndex++)
        {
            int yTileIndex = yIndex + ySegTile; // m_MapData.m_TileTypes[]
            if (yTileIndex < 0)
                continue;

            if (yTileIndex >= m_MapData.m_RegionH)
                break;

            for (int xIndex = 0; xIndex < (800 / _TILEW + 2); xIndex++)
            {
                int xTileIndex = xIndex + xSegTile; 
                if (xTileIndex < 0)
                    continue;

                if (xTileIndex >= m_MapData.m_RegionW)
                    break;

                WORD BlkIndexID = m_MapData.m_Index0[yTileIndex * m_MapData.m_RegionW + xTileIndex];
			    if (BlkIndexID == 0xFFFF)
                    continue;

                LPBlk blk = g_pResManager->GetBlk((WORD)(BlkIndexID >> 8));
                if (blk == NULL)
                    continue;

                blk->Draw((BYTE)(BlkIndexID & 0xff), xTileIndex * _TILEW - x, yTileIndex * _TILEH - y);
            }
        }
    }

    // 显示地毯
    {
        #define GetCurArea(pos, len) (((pos+1)/len) + (((pos+1)%len)?1:0) - 1)

        int xArea = GetCurArea(xSegTile, _AreaW);
        int yArea = GetCurArea(ySegTile, _AreaH);

        std::map<SSprSortTile, int> SortMap;
        for (int yIndex = yArea-1; yIndex <= yArea+2; yIndex++)
        {
            for (int xIndex = xArea-1; xIndex <= xArea+2; xIndex++)
            {
                SMapArea *pArea = m_MapData._GetAreaTable(xIndex, yIndex);
                if (pArea == NULL)
                    continue;

                std::list<SSprSortTile>::iterator it = pArea->m_Layer1.begin();
                while (it != pArea->m_Layer1.end())
                {
                    SortMap[*it] = 0;
                    it++;
                }
            }
        }

        std::map<SSprSortTile, int>::iterator it = SortMap.begin();
        while (it != SortMap.end())
        {
            SSprSortTile *pData = (SSprSortTile *)&it->first;
            it++;

            LPSpr spr = g_pResManager->GetSpr(pData->SprIndex);
            if (spr == NULL)
                continue;

            spr->Draw(pData->PutX - x, pData->PutY - y, pData->GetCurFrame(), (DWORD &)pData->dwAudioFlag);
        }
    }

    // 显示物件层
    {
        std::map<SSortPoint, IObject *> SortMap;

        for (int yIndex = 0; yIndex < (600 / _TILEH + 2) + 8; yIndex++)
        {
            int yTileIndex = yIndex + ySegTile;
            if (yTileIndex < 0)
                continue;

            if (yTileIndex >= m_MapData.m_RegionH)
                break;

            for (int xIndex = 0; xIndex < (800 / _TILEW + 2); xIndex++)
            {
                int xTileIndex = xIndex + xSegTile; 
                if (xTileIndex < 0)
                    continue;

                if (xTileIndex >= m_MapData.m_RegionW)
                    break;

                int index = yTileIndex * m_MapData.m_RegionW + xTileIndex;
                if (m_MapData.pTileSprs[index].wSprIndex == 0xffff)
                    continue;

                SortMap[SSortPoint(xTileIndex * _TILEW - x, yTileIndex * _TILEH - y)] = &m_MapData.pTileSprs[yTileIndex * m_MapData.m_RegionW + xTileIndex];
            }
        }

        // 排序场景上的对象
        {
            std::map<dwt::stringkey<char[STRING_KEY_LEN]>, IObject *>::iterator it = m_ObjectMap.begin();
            while (it != m_ObjectMap.end())
            {
                CVisualObject *pVisualObject = (CVisualObject *)it->second->GetInterface(IID_OBJECT_VISUAL);
                it++;

                if (pVisualObject == NULL)
                    continue;

                int xPos, yPos;
                pVisualObject->GetPosition(xPos, yPos);

                SortMap[SSortPoint(xPos, yPos)] = pVisualObject;
            }
        }

        // 根据最终排序结果显示所有对象的影子
        std::map<SSortPoint, IObject *>::iterator it = SortMap.begin();
        while (it != SortMap.end())
        {
            CVisualObject *pObj = (CVisualObject *)it->second->GetInterface(IID_OBJECT_VISUAL);
            it++;

            if (pObj)
            {
                pObj->ShowBack(x, y);
            }
        }

        // 根据最终排序结果显示所有对象
        it = SortMap.begin();
        while (it != SortMap.end())
        {
            it->second->UpdateShow(x, y);
            it++;
        }
    }

    // 显示遮盖
    {
        #define GetCurArea(pos, len) (((pos+1)/len) + (((pos+1)%len)?1:0) - 1)

        int xArea = GetCurArea(xSegTile, _AreaW);
        int yArea = GetCurArea(ySegTile, _AreaH);

        std::map<SSprSortTile, int> SortMap;
        for (int yIndex = yArea-1; yIndex <= yArea+2; yIndex++)
        {
            for (int xIndex = xArea-1; xIndex <= xArea+2; xIndex++)
            {
                SMapArea *pArea = m_MapData._GetAreaTable(xIndex, yIndex);
                if (pArea == NULL)
                    continue;

                std::list<SSprSortTile>::iterator it = pArea->m_Layer3.begin();
                while (it != pArea->m_Layer3.end())
                {
                    SortMap[*it] = 0;
                    it++;
                }
            }
        }

        std::map<SSprSortTile, int>::iterator it = SortMap.begin();
        while (it != SortMap.end())
        {
            SSprSortTile *pData = (SSprSortTile *)&it->first;
            it++;

            LPSpr spr = g_pResManager->GetSpr(pData->SprIndex);
            if (spr == NULL)
                continue;

            spr->Draw(pData->PutX - x, pData->PutY - y, pData->GetCurFrame(), (DWORD &)pData->dwAudioFlag);
        }
    }

}

HScriptObject COldMapRegion::CreateObject(const __int32 iCreateIID, LPCSTR szKey, SCreateParameter *pParameter)
{
    dwt::stringkey<char[STRING_KEY_LEN]> Key = szKey;

    std::map<dwt::stringkey<char[STRING_KEY_LEN]>, IObject *>::iterator it = m_ObjectMap.find(Key);
    if (it != m_ObjectMap.end())
        // 已经有重复关键字的对象了
        return NULL;

    IObject *pObject = IObject::CreateInstance(iCreateIID, Key, pParameter);
    if (pObject == NULL)
        return NULL;

    m_ObjectMap[Key] = pObject;

    return (HScriptObject)pObject;
}

HScriptObject COldMapRegion::LocateObject(LPCSTR szKey)
{
    std::map<dwt::stringkey<char[STRING_KEY_LEN]>, IObject *>::iterator it = m_ObjectMap.find(szKey);
    if (it == m_ObjectMap.end())
        return NULL;

    return (HScriptObject)it->second;
}

HRESULT COldMapRegion::DeleteObject(HScriptObject hObj)
{
    IObject *pObject = (IObject *)hObj;
    if (pObject == NULL)
        return S_FALSE;

    LPCSTR szKey = pObject->GetKey();
    if (szKey == NULL)
        return S_FALSE;

    std::map<dwt::stringkey<char[STRING_KEY_LEN]>, IObject *>::iterator it = m_ObjectMap.find(szKey);
    if (it == m_ObjectMap.end())
        return NULL;

    m_ObjectMap.erase(it);

    pObject->Exit();
    delete pObject; // 现在的设计是直接让这里调用虚析构

    return S_OK;
}


void COldMapRegion::GetShowPos(int &x, int &y)
{
    x = m_xScreen;
    y = m_yScreen;
}

void COldMapRegion::SetShowPos(int x, int y)
{
    m_xScreen = x;
    m_yScreen = y;
}

void COldMapRegion::MoveRegion(int x1, int y1, int x2, int y2)
{
    SetShowPos(x1, y1);
}

HScriptObject COldMapRegion::GetObjectWithKey(LPCSTR szKey)
{
     HScriptObject pObject = NULL;
    if(szKey)
         pObject = LocateObject(szKey);
    
    return pObject;

}

void GetShowPos(IRegion *pRegion, int &x, int &y)
{
    if (pRegion == NULL)
        return;

    COldMapRegion *pOldMapRegion = (COldMapRegion *)pRegion->GetInterface(IID_REGION_OLD_MAP_REGION);
    if (pOldMapRegion == NULL)
        return;

    pOldMapRegion->GetShowPos(x, y);
}

void SetShowPos(IRegion *pRegion, int x, int y)
{
    if (pRegion == NULL)
        return;

    COldMapRegion *pOldMapRegion = (COldMapRegion *)pRegion->GetInterface(IID_REGION_OLD_MAP_REGION);
    if (pOldMapRegion == NULL)
        return;

    pOldMapRegion->SetShowPos(x, y);
}

HScriptObject CreateVisualObject(IRegion *pRegion, LPCSTR szKey, LPCSTR szInitFunction)
{
    if (pRegion == NULL)
        return NULL;

    if (szKey == NULL)
        return NULL;

    if (szKey[0] == 0)
        return NULL;

    if (szInitFunction == NULL)
        return NULL;

    if (szInitFunction[0] == 0)
        return NULL;

    SVisualObjectParam Param;
    Param.cbSize = sizeof(SVisualObjectParam);
    Param.InitFunction = szInitFunction;

    return pRegion->CreateObject(IID_OBJECT_VISUAL, szKey, &Param);
}

HScriptObject GetObjectWithKey(LPCSTR szKey)
{
    if(szKey == NULL)
        return NULL;

    if(szKey[0] == 0)
        return NULL;

    if(g_Region == NULL)
        return NULL;

    COldMapRegion *pOldMapRegion = (COldMapRegion *)g_Region->GetInterface(IID_REGION_OLD_MAP_REGION);
    if (pOldMapRegion == NULL)
        return NULL;
     
    return pOldMapRegion->GetObjectWithKey(szKey);

}

void MoveShowPos(int x, int y)
{
    COldMapRegion *pOldMapRegion = (COldMapRegion *)g_Region->GetInterface(IID_REGION_OLD_MAP_REGION);
    if (pOldMapRegion == NULL)
        return;

    pOldMapRegion->SetShowPos(x, y);
}