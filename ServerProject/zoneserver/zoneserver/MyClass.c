/*
** Lua binding: MyClass
** Generated automatically by tolua 5.2.4 on 12/02/15 20:26:15.
*/
#include "stdafx.h"
#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string"

#include "tolua.h"

/* Exported function */
TOLUA_API int  tolua_MyClass_open (lua_State* tolua_S);
LUALIB_API int  luaopen_MyClass (lua_State* tolua_S);

#include "MyClass.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
}

/* Open lib function */
LUALIB_API int  luaopen_MyClass (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
 tolua_endmodule(tolua_S);
 return 1;
}
/* Open tolua function */
TOLUA_API int tolua_MyClass_open (lua_State* tolua_S)
{
  lua_pushcfunction(tolua_S, luaopen_MyClass);
  lua_pushstring(tolua_S, "MyClass");
  lua_call(tolua_S, 1, 0);
  return 1;
}
