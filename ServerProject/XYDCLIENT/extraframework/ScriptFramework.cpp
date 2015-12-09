#include "StdAfx.h"
#include "iscriptframework.h"
#include "scriptframework.h"
#include "SManager.h"
#include "NewMapData.h"

#include "Engine.h"
#include "JpgFile.h"


#include "OldMapRegion.h"

IRegion  *g_Region = NULL;

CScriptFramework::CScriptFramework(void)
{
    m_pRegion = NULL;
}

CScriptFramework::~CScriptFramework(void)
{
    ClearRegion();
}

IFramework *CScriptFramework::GetInterface(const __int32 IID)
{
    return (IID == IID_FRAMEWORK_SCRIPT) ? this : NULL;
}

HRESULT CScriptFramework::Init(SCreateParameter *pParameter)
{
    if (pParameter == NULL)
        return S_FALSE;

    // 无效的参数，大小不同可能导致操作溢出
    if (pParameter->cbSize != sizeof(SScriptFrameworkParam))
        return S_FALSE;

    SScriptFrameworkParam *pParam = (SScriptFrameworkParam *)pParameter;

    // 不需要同时创建场景
    if (pParam->iCreateIID == 0)
        return S_OK;

    // 根据初始化数据同时创建场景
    return CreateRegion(pParam->iCreateIID, pParam->pCreateMapParam);
}

HRESULT CScriptFramework::LoadEventFile(LPCSTR filename)
{
	ClearRegion();

	int nRet = Script::g_Script.DoFile(filename);
    if(nRet != 0) 
		return S_FALSE;

	Script::g_Script.CallFunc("InitEvent", this);

    return S_FALSE;
}

HRESULT CScriptFramework::Loop(void)
{
    // 暂时还用不到这个主循环
    return S_OK;
}

HRESULT CScriptFramework::Exit(void)
{
    if (m_pRegion == NULL)
        return S_FALSE;

    ClearRegion();

    return S_OK;
}

void CScriptFramework::UpdateAI(void)
{
    if (m_pRegion == NULL)
        return;

    m_pRegion->UpdateAI();
}

void CScriptFramework::UpdateShow(void)
{
    if (m_pRegion == NULL)
        return;

    m_pRegion->UpdateShow();
}

void CScriptFramework::InputHandle(int iKeyState, int vKey, int x, int y)
{
}

HRESULT CScriptFramework::CreateRegion(const __int32 iCreateIID, SCreateParameter *pParam)
{
    // 如果以前的场景还有效，那么先清空之
    if (m_pRegion != NULL)
        ClearRegion();

    m_pRegion = IRegion::CreateInstance(iCreateIID, pParam);
    g_Region = m_pRegion;
    // 无效的对象编号，导致无法获得正确对象实体
    if (m_pRegion == NULL)
        return S_FALSE;

    return S_OK;
}

void CScriptFramework::ClearRegion()
{
    if (m_pRegion != NULL)
    {
        m_pRegion->Exit();

        delete m_pRegion;
        m_pRegion = NULL;
    }

	Script::g_Script.Reset();
	Script::g_Script.RegFunc();
}

IRegion *CScriptFramework::GetCurRegion()
{
    return m_pRegion;
}

IRegion *CreateOldMapRegion(IFramework *pFramework, long lMapIndex, LPCSTR szInitFunction)
{
    if (pFramework == NULL)
        return NULL;

    if (szInitFunction == NULL)
        return NULL;

    if (szInitFunction[0] == 0)
        return NULL;

    CScriptFramework *pScriptFramework = (CScriptFramework *)pFramework->GetInterface(IID_FRAMEWORK_SCRIPT);
    if (pScriptFramework == NULL)
        return NULL;

    SOldMapRegionParam Param;
    Param.cbSize = sizeof(SOldMapRegionParam);
    Param.lMapIndex = lMapIndex;
    Param.InitFunction = szInitFunction;

    // 不论这里的CreateRegion成功与否，pScriptFramework->GetCurRegion()可以直接反映操作结果
    pScriptFramework->CreateRegion(IID_REGION_OLD_MAP_REGION, &Param);

    return pScriptFramework->GetCurRegion();
}

IRegion *GetCurRegion(IFramework *pFramework)
{
    if (pFramework == NULL)
        return NULL;

    CScriptFramework *pScriptFramework = (CScriptFramework *)pFramework->GetInterface(IID_FRAMEWORK_SCRIPT);
    if (pScriptFramework == NULL)
        return NULL;

    return pScriptFramework->GetCurRegion();
}

void StartEventWithScriptFilename(IFramework *pFramework, LPCSTR szFilename)
{
    if (pFramework == NULL)
        return;

    if (szFilename == NULL)
        return;

    if (szFilename[0] == 0)
        return;

    CScriptFramework *pScriptFramework = (CScriptFramework *)pFramework->GetInterface(IID_FRAMEWORK_SCRIPT);
    if (pScriptFramework == NULL)
        return;

    pScriptFramework->LoadEventFile(szFilename);
}
