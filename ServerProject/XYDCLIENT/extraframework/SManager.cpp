// ========== SManager.cpp ================================================== 

#include "stdafx.h"
#include <sstream>
#include "SManager.h"
#include "SObject.h"
using namespace Script;
using namespace std;
#pragma comment (lib, "lualib.lib") 
//////////////////////////////////////////////////////////////
CSManager Script::g_Script;


void CSManager::RegFunc()
{
	Script::CSRegionObject::CreateShadow();
	Script::CSVisualObject::CreateShadow();
	Script::CSEvent::CreateShadow();
}

void CSManager::Test()
{
	MessageBox(0, 0, "helllo", 0);
}
// 脚本接口函数
int CSManager::L_init(lua_State *L)
{
	/*LPATTRIBUTE pAtt = NULL;
	ATTRIBUTE att;
	ZeroMemory(&att, sizeof(ATTRIBUTE));
	att.dwEventID = 10;
	att.nID = 1;
	att.nPosX = 10;
	att.nPosY = 20;
	att.szName = "norikada";
	pAtt = &att;

	lua_pushlstring(L, (LPCSTR)pAtt, sizeof(ATTRIBUTE));
   
	CSManager *csm = new CSManager;
	csm->Test();*/

	return 1;
}

int CSManager::L_new(lua_State *L)
{
	CSManager *csm = new CSManager;    
	if(!csm) { return 0; }

	const char *s = lua_tostring(L, 1);
	if(!s) {  return 0; }

	lua_settop(L, 0);
	if(strcmp("test", s) == 0) {
		csm->CreateShadow(L);
	}
	else if(strcmp("point",s) == 0) {
		//CSRegionObject *cso = new CSRegionObject;
		//cso->CreateShadow(L);

	}
	 
	return 1;
}

int CSManager::L_print(lua_State *L)
{
	char szName[30]  = { 0 }; 

	//LPATTRIBUTE apAtt = (LPATTRIBUTE)lua_tostring(L, 1);
	//if(apAtt) {
	//	MessageBox(0 , apAtt->szName, 0, 0);
	//}
	MessageBox(0, "hao", 0, 0);

    return 0;
}
// 映射一个数组，定义成员为函数的方法调用
void CSManager::CreateShadow(lua_State *L)
{
	//int tag;

	//lua_newtable(L);	

	//lua_pushstring(L,"this");						
	//lua_pushuserdata(L,(void*)this);
	//lua_settable(L,1);

	//lua_pushcfunction(L, L_init);
	//tag = lua_newtag(L);						
	//lua_settagmethod(L,tag,"function");			
	//lua_pushstring(L,"Init");			
	//lua_pushusertag(L,(void*)this,tag);			
	//lua_settable(L,1);	
}

// 脚本运行入口
void CSManager::Run()
{	
	RegFunc();
	DoFile("Init.lua");  // test
}