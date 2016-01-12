#include "stdafx.h"
#include "Oldfunction.h"
#include "../GameObjects/ScriptManager.h"

extern CScriptManager g_Script;

typedef lua_State *(*oluaL_newstate)(void);
oluaL_newstate tluaL_newstate;

typedef void (*oluaL_openlibs)(lua_State *L);
oluaL_openlibs tluaL_openlibs;

typedef int(*oluaEx_serialize)(lua_State *L, int idx, void *buf, int size);
oluaEx_serialize tluaEx_serialize;

typedef int (*oluaEx_unserialize)(lua_State *L, const void *buf, int size);
oluaEx_unserialize tluaEx_unserialize;


void luaEx_pushlightuserdata(lua_State *L, void *p, int size)
{
	lua_pushlightuserdata(L, p);
}

void* luaEx_touserdata(lua_State *L, int idx, int *size)
{
	return nullptr;
}

int luaEx_isint63(lua_State *L, int idx)
{
	return lua_isnumber(L, idx);
}

void luaEx_pushint63(lua_State *L, unsigned __int64 int63)
{
	lua_pushnumber(L, int63);
}

unsigned __int64 luaEx_toint63(lua_State *L, int idx)
{
	return lua_tonumber(L, idx);
}

int luaEx_serialize(lua_State *L, int idx, void *buf, int size)
{
	return tluaEx_serialize(L, idx, buf, size);
}

int luaEx_unserialize(lua_State *L, const void *buf, int size)
{
	return tluaEx_unserialize(L, buf, size);
}

int luaL_len(lua_State *L, int idx)
{
	return lua_objlen(L, idx);
}

COldfunction::COldfunction() :m_ls(nullptr)
{

}

COldfunction::~COldfunction()
{

}

void COldfunction::Initlib()
{
	HMODULE	hmod = LoadLibrary("lualib.dll");
	if (hmod)
	{
		tluaL_newstate = (oluaL_newstate)GetProcAddress(hmod, "luaL_newstate");
		if (tluaL_newstate)
		{
			m_ls = tluaL_newstate();
		}

		tluaL_openlibs = (oluaL_openlibs)GetProcAddress(hmod, "luaL_openlibs");
		if (tluaL_openlibs&&m_ls)
		{
			tluaL_openlibs(m_ls);
		}
		tluaEx_serialize = (oluaEx_serialize)GetProcAddress(hmod, "luaEx_serialize");
		tluaEx_unserialize = (oluaEx_unserialize)GetProcAddress(hmod, "luaEx_unserialize");

	}
}
