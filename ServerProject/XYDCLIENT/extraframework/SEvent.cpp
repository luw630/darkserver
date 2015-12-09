// ======= Script.cpp =========================================

#include "stdafx.h"
#include "SManager.h"
#include "SEvent.h"
#include "ScriptCallbackFunctions.h"

using namespace Script;
//////////////////////////////////////////////////////////

int CSEvent::L_moveto(lua_State *L)
{

    IObject **pIObject = (IObject **)lua_tostring(L, 1);
	int nPosX = static_cast<int>(lua_tonumber(L, 2));
	int nPosY = static_cast<int>(lua_tonumber(L, 3));
    int nSpeed = static_cast<int>(lua_tonumber(L, 4));

	MoveTo(*pIObject, nPosX, nPosY, nSpeed);

	return 0;
}

int CSEvent::L_getshowpos(lua_State *L)
{
	int x = 0;
	int y = 0;

	IObject **pIObject = (IObject **)lua_tostring(L, 1);

	GetShowPos(*pIObject, x, y);
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);

	return 2;
}

int CSEvent::L_setevent(lua_State *L)
{
    IObject **pIObject = (IObject **)lua_tostring(L, 1);
   /* DWORD dwEventID = static_cast<DWORD>(lua_tonumber(L, 2));*/
    LPCSTR szEventFunc = static_cast<const char*>(lua_tostring(L, 2));

    //switch(dwEventID)
    //{
    //case 1: {
    //    int x = static_cast<int>(lua_tonumber(L, 4));
    //    int y = static_cast<int>(lua_tonumber(L, 5));
        SetEventFunc(*pIObject,szEventFunc);
    //        }break;

    //default:
    //    return 0;
    //}

    return 0;
}

int CSEvent::L_settalkinfo(lua_State *L)
{
    IObject **pIObject = (IObject **)lua_tostring(L, 1);
    WORD wType = static_cast<WORD>(lua_tonumber(L, 2));
    LPCSTR szTalkInfo = static_cast<const char*>(lua_tostring(L, 3));
    DWORD dwTime = static_cast<DWORD>(lua_tonumber(L, 4));

    SetTalkInfo(*pIObject, wType, szTalkInfo, dwTime);

    return 0;
}

int CSEvent::L_addeffect(lua_State *L)
{
    IObject **pIObject = (IObject **)lua_tostring(L, 1);
    LPCSTR szName = static_cast<const char*>(lua_tostring(L, 2));
    WORD wTime = static_cast<WORD>(lua_tonumber(L, 3));

    AddEffect(*pIObject, szName, wTime);  
    
    return 0;

}

int CSEvent::L_moveshowpos(lua_State *L)
{
    int x = static_cast<int>(lua_tonumber(L, 1));
    int y = static_cast<int>(lua_tonumber(L, 2));

    MoveShowPos(x, y);

    return 0;
}

// зЂВс
void CSEvent::CreateShadow()
{
	g_Script.RegisterFunction("MoveTo", L_moveto);
    g_Script.RegisterFunction("GetVObjectPos", L_getshowpos);
    g_Script.RegisterFunction("SetEvent", L_setevent);
    g_Script.RegisterFunction("Talk", L_settalkinfo);
    g_Script.RegisterFunction("PlayEffect", L_addeffect);
    g_Script.RegisterFunction("MoveMapPos", L_moveshowpos);
}
