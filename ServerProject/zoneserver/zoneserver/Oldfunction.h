#pragma once
#include "stdafx.h"
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
void  luaEx_pushlightuserdata(lua_State *L, void *p, int size);

  void* luaEx_touserdata(lua_State *L, int idx, int *size);
  int luaEx_isint63(lua_State *L, int idx);
  void luaEx_pushint63(lua_State *L, unsigned __int64 int63);
  unsigned __int64 luaEx_toint63(lua_State *L, int idx);
  int luaEx_serialize(lua_State *L, int idx, void *buf, int size);
  int luaEx_unserialize(lua_State *L, const void *buf, int size);
  int luaL_len(lua_State *L, int idx);


  class COldfunction
  {
  public:
	  COldfunction();
	  ~COldfunction();
	  void Initlib();
  private:
	  lua_State  *m_ls;
  };

