#include "StdAfx.h"
#include "iscriptframework.h"
#include "visualobject.h"

#include "SManager.h"
#include <cmath>
////////////////////////////////////////
// extra data
#include "ResManagerModule\ResManagerModule.h"
extern CResManagerModule *g_pResManager;

#include "ResManagerModule/DCanvasEx.h"
extern DCanvasEx *g_Canvas;

#include "ResManagerModule/DFont.h"
extern CDFont  g_CommonFont16;
extern CDFont  g_CommonFont12;
////////////////////////////////////////
unsigned __int32 GetCycleTime();

CVisualObject::CVisualObject(LPCSTR szKey) : 
IObject(szKey)
{
    m_x = 0;
    m_y = 0;

    m_Key = szKey;

    // init player data
    m_PlayerID = 0;
    m_WeaponID = 0;
    m_CurFrame = 0;

    dwPlay = 0;
    m_wSpace = 0;
    m_wFBegin = 0;
    m_wFEnd = 0;

    m_iSpeed = 0;
    m_dwSegTime = 0;

	Sin = 0;
	Cos = 0;
	SegX = SegY = 0;
	DestX = DestY = 0;
	m_dwMCurTime = 0;
    DistanceLength = 0;

    szAIFunc = NULL;

    m_TalkInfo.dwTime = 0;
    m_TalkInfo.wType = 0;

    m_pRelationObject = NULL; 
    iReferenceCount = 0; 
}

CVisualObject::~CVisualObject(void)
{
    if (iReferenceCount != 0)
        MessageBoxW(0, L"还被其他对象所引用", 0, 0);
}

IObject *CVisualObject::GetInterface(const __int32 IID)
{
    return (IID == IID_OBJECT_VISUAL) ? this : NULL;
}

HRESULT CVisualObject::Init(SCreateParameter *pParameter)
{
    if (pParameter == NULL)
        return S_FALSE;

    // 无效的参数，大小不同可能导致操作溢出
    if (pParameter->cbSize != sizeof(SVisualObjectParam))
        return S_FALSE;

    SVisualObjectParam *pParam = (SVisualObjectParam *)pParameter;

    // 启动初始化脚本函数
    Script::g_Script.CallFunc(pParam->InitFunction, this);

    return S_OK;
}

HRESULT CVisualObject::Exit(void)
{
    return S_OK;
}

void CVisualObject::UpdateAI(void)
{
    if (szAIFunc)
        Script::g_Script.CallFunc(szAIFunc, this);

    m_CurFrame = ((GetCycleTime()-m_dwSegTime)/m_wSpace)%(m_wFEnd - m_wFBegin) + m_wFBegin;

    Move();
}

void CVisualObject::UpdateShow(int x, int y)
{
    if (m_pRelationObject != 0)
    {
        x = x - m_pRelationObject->m_x;
        y = y - m_pRelationObject->m_y;
    }

    switch (m_ResourceType)
    {
    case VT_STRINGSPR:
        break;

    case VT_PLAYER:
        if (m_Png != NULL)
        {
            m_Png->DrawWithWeapon(m_x - x, m_y - y, m_Wng.get(), m_CurFrame, *(BYTE*)&dwPlay, NULL);
        }
        break;

    case VT_MONSTER:
        break;

    case VT_NPC:
        break;
    }

    PlayEffect(x, y);
    DWORD dwCur = GetCycleTime();
    if(dwCur < m_TalkInfo.dwTime)
        ShowTalk(x, y);

}

void CVisualObject::ShowBack(int x, int y)
{
    switch (m_ResourceType)
    {
    case VT_STRINGSPR:
        break;

    case VT_PLAYER:
        if (m_Png != NULL)
        {
            m_Png->DrawShadow(m_x - x, m_y - y, m_CurFrame);
        }
        break;

    case VT_MONSTER:
        break;

    case VT_NPC:
        break;
    }
}

LPCSTR CVisualObject::GetKey(void)
{
    return m_Key;
}

void CVisualObject::SetPosition(int x, int y)
{
    m_x = x;
    m_y = y;
}

void CVisualObject::GetPosition(int &x, int &y)
{
    x = m_x;
    y = m_y;
}

void CVisualObject::SetSpaceNumber(WORD wNumber, WORD wFBegin, WORD wFEnd)
{
    m_wSpace = wNumber;
    m_wFBegin = wFBegin;
    m_wFEnd = wFEnd;
}

void CVisualObject::Moveto(int xPos, int yPos, int iSpeed)
{

    SegX = m_x;
    SegY = m_y;

    DestX = xPos;
    DestY = yPos;

    DistanceLength = hypot(DestX - SegX, DestY - SegY);

    Sin = (DestY - SegY) / DistanceLength;
    Cos = (DestX - SegX) / DistanceLength;

    m_iSpeed = iSpeed;
	m_dwMCurTime = GetCycleTime();

    int nNeedX = xPos - m_x;
    int nNeedY = yPos - m_y;

/*
    if(nNeedX == 0)
    {
        if(nNeedY < 0) {
            m_wFBegin = 0;
            m_wFEnd = 8;
        }else if(nNeedY > 0) {
            m_wFBegin = 32;
            m_wFEnd = 40;
        }else
            return;
    }
    if(nNeedY == 0)
    {
        if(nNeedX < 0) {
            m_wFBegin = 48;
            m_wFEnd = 56;
        }else if(nNeedX > 0) {
            m_wFBegin = 16;
            m_wFEnd = 24;
        }else
            return;
    }

    if(nNeedX != 0 && nNeedY != 0)
    {
        float lSlope  = (float)((float)nNeedY/(float)nNeedX);

        if(lSlope < 1)
        {
            if(nNeedX > 0) {
                m_wFBegin = 8;
                m_wFEnd = 16;
            }else if(nNeedX < 0) {
                m_wFBegin = 40;
                m_wFEnd = 48;
            }
        }else if(lSlope > 1) {
            if(nNeedX > 0) {
                m_wFBegin = 24;
                m_wFEnd = 32;
            }else if(nNeedX < 0) {
                m_wFBegin = 56;
                m_wFEnd = 64;
            }
        }else
            return;
    }
*/
}

void CVisualObject::Move()
{
    if (DistanceLength < 0.1)
        return;

    int d = static_cast<int>((GetCycleTime() - m_dwMCurTime) / 1000.0 * m_iSpeed);

    if (d <= static_cast<int>(DistanceLength))
    {
        m_x = int(d * Cos + SegX);
        m_y = int(d * Sin + SegY);
    }
    else
    {
        m_x = DestX;
        m_y = DestY;
        DistanceLength = 0;
    }
}

void CVisualObject::SetTalkInfo(WORD wType, LPCSTR szTalk, DWORD dwTime)
{
    m_TalkInfo.wType = wType;
    m_TalkInfo.dwTime = GetCycleTime() + dwTime * 1000;
    m_TalkInfo.m_szTalkInfo  = szTalk;
}

void CVisualObject::ShowTalk(int x, int y)
{
    if(m_TalkInfo.wType == TALK::ET_NULL)
        return;
     
    switch(m_TalkInfo.wType)
    {
    case TALK::ET_DRAWBACK: 
        {
            g_Canvas->DrawFade(m_x - x, m_y - y - 150, 100, 60,15);
            g_CommonFont12.SetLine(80, 20);
            g_CommonFont12.SetColor(RGB(214,234,99));
            g_CommonFont12.DrawText(m_x - x + 10, m_y - y - 140, m_TalkInfo.m_szTalkInfo, 0); 

        } break;

    case TALK::ET_NOBACK: 
        {
            g_CommonFont12.SetLine(80, 20);
            g_CommonFont12.SetColor(RGB(214,234,99));
            g_CommonFont12.DrawText(m_x - x + 10, m_y - y - 100, m_TalkInfo.m_szTalkInfo, 0);

        } break;

    default:
        return;
    }

}

void CVisualObject::SetEventFunc(LPCSTR szEventFunc)
{
    szAIFunc = szEventFunc;
}

void CVisualObject::SetPlayerImage(EActionState ActType, WORD PlayerID, WORD WeaponID)
{
    m_ResourceType = VT_PLAYER;
    m_dwSegTime = GetCycleTime();

    if (PlayerID != 0) m_PlayerID = PlayerID;
    if (WeaponID != 0) m_WeaponID = WeaponID;

    m_Png = g_pResManager->GetPng(ActType, m_PlayerID);
    m_Wng = g_pResManager->GetWng(ActType, (BYTE)m_PlayerID, WeaponID);

}

void CVisualObject::AddEffect(LPCSTR szFileName, WORD wTime, bool bAutoPlay )
{
    if(szFileName == NULL)
        return;

    if(szFileName[0] == 0)
        return;

    EFFECT effect;

    effect.bPlay = bAutoPlay;
    effect.lpSpr = g_pResManager->GetSpr(szFileName);
    effect.dwTime = GetCycleTime() + wTime * 1000;

    m_EffectList.push_back(effect);
}

void CVisualObject::PlayEffect(int x, int y)
{
    if(m_EffectList.size() == 0)
        return;

    DWORD dwCur = GetCycleTime();
    std::list<EFFECT>::iterator it = m_EffectList.begin();
    for(; it != m_EffectList.end();)
    {
        EFFECT *pEffect = &*it;
        if(pEffect->dwTime > dwCur) {
            pEffect->lpSpr->Draw(m_x - x, m_y - y, ((GetCycleTime()-m_dwSegTime)/m_wSpace)%(m_wFEnd - m_wFBegin) + m_wFBegin, *(BYTE*)&dwPlay);
        }/*else
            m_EffectList.erase(it);*/

        it++;
    }


}

void CVisualObject::SetRelation(CVisualObject *pSourceObject)
{
    if (iReferenceCount != 0)
        return;

    if (m_pRelationObject != NULL)
        m_pRelationObject->iReferenceCount --;

    m_pRelationObject = pSourceObject;
    if (m_pRelationObject != NULL)
        m_pRelationObject->iReferenceCount ++;
}


// 获取当前对象的显示位置
void GetShowPos(IObject *pObject, int &x, int &y)
{
    if (pObject == NULL)
        return;

    CVisualObject *pVisualObject = (CVisualObject *)pObject->GetInterface(IID_OBJECT_VISUAL);
    if (pVisualObject == NULL)
        return;

    pVisualObject->GetPosition(x, y);
}

// 设置当前对象的显示位置
void SetShowPos(IObject *pObject, int x, int y)
{
    if (pObject == NULL)
        return;

    CVisualObject *pVisualObject = (CVisualObject *)pObject->GetInterface(IID_OBJECT_VISUAL);
    if (pVisualObject == NULL)
        return;

    pVisualObject->SetPosition(x, y);
}

// 设置一个对象使用玩家的显示资源
void SetPlayerImage(IObject *pObject, WORD wActionType, int PlayerID, int WeaponID)
{
    if (pObject == NULL)
        return;

    CVisualObject *pVisualObject = (CVisualObject *)pObject->GetInterface(IID_OBJECT_VISUAL);
    if (pVisualObject == NULL)
        return;

    pVisualObject->SetPlayerImage((EActionState)wActionType , PlayerID, WeaponID);
}

void SetSpaceNumber(IObject *pObject, WORD wNumber, WORD wFBegin, WORD wFEnd)
{
    if (pObject == NULL)
        return;

    CVisualObject *pVisualObject = (CVisualObject *)pObject->GetInterface(IID_OBJECT_VISUAL);
    if (pVisualObject == NULL)
        return;

    pVisualObject->SetSpaceNumber(wNumber, wFBegin, wFEnd);

}

void SetRelation(IObject *pSObject, IObject *pDObject)
{
    if (pSObject == NULL || pDObject == NULL )
        return;

    CVisualObject *pVisualSObject = (CVisualObject *)pSObject->GetInterface(IID_OBJECT_VISUAL);
    if (pVisualSObject == NULL)
        return;

    CVisualObject *pVisualDObject = (CVisualObject *)pDObject->GetInterface(IID_OBJECT_VISUAL);
    if (pVisualDObject == NULL)
        return;

    pVisualDObject->SetRelation(pVisualSObject);
    
}

void MoveTo(IObject *pObject, int nXPos, int nYPos, int nSpeed)
{

    if (pObject == NULL)
        return;

    CVisualObject *pVisualObject = (CVisualObject *)pObject->GetInterface(IID_OBJECT_VISUAL);
    if (pVisualObject == NULL)
        return;

    pVisualObject->Moveto(nXPos, nYPos, nSpeed);

}

void SetEventFunc(IObject *pObject, LPCSTR szEventFunc)
{
    if (pObject == NULL)
        return;

    CVisualObject *pVisualObject = (CVisualObject *)pObject->GetInterface(IID_OBJECT_VISUAL);
    if (pVisualObject == NULL)
        return;

    pVisualObject->SetEventFunc(szEventFunc);

}

void SetTalkInfo(IObject *pObject, WORD wType, LPCSTR szTalkInfo, DWORD dwTime)
{
    if (pObject == NULL)
        return;

    CVisualObject *pVisualObject = (CVisualObject *)pObject->GetInterface(IID_OBJECT_VISUAL);
    if (pVisualObject == NULL)
        return;

    pVisualObject->SetTalkInfo(wType, szTalkInfo, dwTime);
}

void AddEffect(IObject *pObject, LPCSTR szFileName, WORD wTime)
{
    if (pObject == NULL)
        return;

    CVisualObject *pVisualObject = (CVisualObject *)pObject->GetInterface(IID_OBJECT_VISUAL);
    if (pVisualObject == NULL)
        return;

    pVisualObject->AddEffect(szFileName, wTime);

}
