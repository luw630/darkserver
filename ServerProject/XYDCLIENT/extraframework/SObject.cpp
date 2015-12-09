// =======  SManager.cpp ======================================================== 

#include "stdafx.h"
#include "SManager.h"
#include "SObject.h"
#include "ScriptCallbackFunctions.h"

using namespace Script;

////////////////////////////////////////////////////////

// 场景
int CSRegionObject::L_CreateOldMapRegion(lua_State *L)
{
	IFramework **pFramework = (IFramework **)lua_tostring(L, 1);
	long lIndex = static_cast<long>(lua_tonumber(L, 2));
	LPCSTR lpStr = lua_tostring(L, 3);

	lua_settop(L ,0);
	CreateOldMapRegion(*pFramework, lIndex, lpStr);
    
	return 0;
}

int CSRegionObject::L_setViewPos(lua_State *L)
{
	IRegion **pIRegion = (IRegion **)lua_tostring(L, 1);
	int xPos = static_cast<int>(lua_tonumber(L, 2));
	int yPos = static_cast<int>(lua_tonumber(L, 3));

	SetShowPos(*pIRegion, xPos, yPos);

	return 0;
}

int CSRegionObject::L_loadRegion(lua_State *L)
{

	return 0;
}

// 注册
void CSRegionObject::CreateShadow()
{
	// 映射场景对象函数
	//int tag;

	//lua_newtable(L);	

	//lua_pushstring(L,"this");						
	//lua_pushuserdata(L,(void*)this);
	//lua_settable(L,1);

	//lua_pushcfunction(L, L_CreateOldMapRegion);
	//tag = lua_newtag(L);						
	//lua_settagmethod(L,tag,"function");			
	//lua_pushstring(L,"CreateOldMapRegion");			
	//lua_pushusertag(L,(void*)this,tag);			
	//lua_settable(L,1);	

	//lua_pushcfunction(L, L_loadRegion);
	//tag = lua_newtag(L);						
	//lua_settagmethod(L,tag,"function");			
	//lua_pushstring(L,"loadRegion");			
	//lua_pushusertag(L,(void*)this,tag);			
	//lua_settable(L,1);

	g_Script.RegisterFunction("CreateOldMapRegion", L_CreateOldMapRegion);
	g_Script.RegisterFunction("SetViewPosition", L_setViewPos);
}

// 场景上对象
int CSVisualObject::L_createVObject(lua_State *L)
{

    IRegion **pIRegion = (IRegion **)lua_tostring(L, 1);
	LPCSTR  lpszKey = lua_tostring(L, 2);
	LPCSTR  lpszInitFunc = lua_tostring(L, 3);

	lua_settop(L ,0);
	CreateVisualObject(*pIRegion, lpszKey, lpszInitFunc);

	return 0;
}

int CSVisualObject::L_getParam(lua_State *L)
{


    return 1;
}

int CSVisualObject::L_getObject(lua_State *L)
{
    LPCSTR szKey = static_cast<const char*>(lua_tostring(L, 1));
    HScriptObject pObject = GetObjectWithKey(szKey);

    
    if(pObject)
    {
        lua_pushlstring(L, (LPCSTR)&pObject, sizeof(HScriptObject));
    }else
        lua_pushnumber(L, 0);

    return 1;
}

int CSVisualObject::L_setVObjectAttribute(lua_State *L)
{
	IObject **pIObject = (IObject **)lua_tostring(L, 1);
	WORD wIndex = static_cast<WORD>(lua_tonumber(L, 2));

	switch(wIndex)	
	{
	case EOA_POS:   {
			int xPos = static_cast<int>(lua_tonumber(L, 3));
			int yPos = static_cast<int>(lua_tonumber(L, 4));
			SetShowPos(*pIObject, xPos, yPos);
		}break;

	case EOA_PIC:   {
			WORD wActionType = static_cast<WORD>(lua_tonumber(L, 3));
			int nPlayerID = static_cast<int>(lua_tonumber(L, 4));
			int nWeaponID = static_cast<int>(lua_tonumber(L, 5));
			SetPlayerImage(*pIObject, wActionType, nPlayerID, nWeaponID);
		}break;

	case EOA_ACTION: {
			WORD wAction = static_cast<WORD>(lua_tonumber(L, 3));
			SetPlayerImage(*pIObject, wAction, 0, 0);
		}break;

	case EOA_SPACE:	 {
			WORD wNumber = static_cast<WORD>(lua_tonumber(L, 3));
			WORD wFBegin = static_cast<WORD>(lua_tonumber(L, 4));
			WORD wFEnd = static_cast<WORD>(lua_tonumber(L, 5));
            SetSpaceNumber(*pIObject, wNumber,wFBegin, wFEnd );
		}break;

    //case EOA_ROBJECT: {
    //        LPCSTR szfilename = static_cast<const char*>(lua_tostring(L, 3));
    //        AddEffect(*pIObject, szfilename, 0);
    //    }break;

	default:
		return 0;
	}
  
	return 0;
}

// 注册
void CSVisualObject::CreateShadow()
{
	//int tag;

	//lua_newtable(L);	

	//lua_pushstring(L,"this");						
	//lua_pushuserdata(L,(void*)this);
	//lua_settable(L,1);

	//lua_pushcfunction(L, L_getParam);
	//tag = lua_newtag(L);						
	//lua_settagmethod(L,tag,"function");			
	//lua_pushstring(L,"getParam");			
	//lua_pushusertag(L,(void*)this,tag);			
	//lua_settable(L,1);	

	//lua_pushcfunction(L, L_setVObject);
	//tag = lua_newtag(L);						
	//lua_settagmethod(L,tag,"function");			
	//lua_pushstring(L,"setVObject");			
	//lua_pushusertag(L,(void*)this,tag);			
	//lua_settable(L,1);

	g_Script.RegisterFunction("CreateVObject", L_createVObject);
	g_Script.RegisterFunction("SetVObjectAttribute", L_setVObjectAttribute);
    g_Script.RegisterFunction("GetObject", L_getObject);
 
}
