#include "windows.h"

#include "StdAfx.h"
#include "iscriptframework.h"
#include "newmapdata.h"

////////////////////////////////////////
// extra data
#include "ResManagerModule\ResManagerModule.h"
extern CResManagerModule *g_pResManager;
////////////////////////////////////////


CTileSpr::CTileSpr(void) : 
    IObject(NULL),
    STimeFrame()
{
    wSprIndex = 0;
}

CTileSpr::~CTileSpr(void)
{
}

IObject *CTileSpr::GetInterface(const __int32 IID)
{
    return NULL;
}

HRESULT CTileSpr::Init(SCreateParameter *pParam)
{
    return S_OK;
}

HRESULT CTileSpr::Exit(void)
{
    return S_OK;
}

void CTileSpr::UpdateAI(void)
{
}

void CTileSpr::UpdateShow(int x, int y)
{
    LPSpr spr = g_pResManager->GetSpr(wSprIndex);
    if (spr == NULL)
        return;

    if (wFrameInterval == 0) 
        wFrameInterval = spr->m_Spr.GetInterval() * 25;

    spr->Draw(m_x - x, m_y - y, GetCurFrame(), dwAudioFlag);
}

LPCSTR CTileSpr::GetKey(void)
{
    return NULL;
}

CNewMapData::CNewMapData(void) : 
    CMapData()
{
    pTileSprs = NULL;
}

CNewMapData::~CNewMapData(void)
{
    if (pTileSprs != NULL)
        delete [] pTileSprs;
}

int CNewMapData::LoadMap(long MapIndex)
{
    if (CMapData::LoadMap(MapIndex) == 0)
        return 0;

    if (pTileSprs != NULL)
        delete [] pTileSprs;

    pTileSprs = new CTileSpr[GetRegionW()*GetRegionH()];

    for (int y = 0; y < GetRegionH(); y++)
    {
        for (int x = 0; x < GetRegionW(); x++)
        {
            int index = y * GetRegionW() + x;
            CTileSpr *pTileSpr = &pTileSprs[index];
            SSprTile *pTile = &m_Index2[index];
            pTileSpr->wSprIndex = pTile->SprIndex;
            pTileSpr->wBaseFrame = pTile->Rand;
            pTileSpr->m_x = x * 64;
            pTileSpr->m_y = y * 32;
        }
    }

    return 1;
}
