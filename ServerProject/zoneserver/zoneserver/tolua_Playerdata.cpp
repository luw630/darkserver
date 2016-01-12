/*
** Lua binding: playerdata
** Generated automatically by tolua++-1.0.92 on 12/15/15 17:42:41.
*/
#include "stdafx.h"
#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"
#include <string>
/* Exported function */
TOLUA_API int  tolua_playerdata_open (lua_State* tolua_S);
#include "playerdata.pb.h"

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_int32 (lua_State* tolua_S)
{
 int* self = (int*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_PlayerData (lua_State* tolua_S)
{
 PlayerData* self = (PlayerData*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"int");
 tolua_usertype(tolua_S,"::google::protobuf::Message");
 tolua_usertype(tolua_S,"PlayerData");
 tolua_usertype(tolua_S,"size_t");
}

/* method: new of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_new00
static int tolua_playerdata_PlayerData_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"PlayerData",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   PlayerData* tolua_ret = (PlayerData*)  Mtolua_new((PlayerData)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"PlayerData");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_new00_local
static int tolua_playerdata_PlayerData_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"PlayerData",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   PlayerData* tolua_ret = (PlayerData*)  Mtolua_new((PlayerData)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"PlayerData");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: delete of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_delete00
static int tolua_playerdata_PlayerData_delete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PlayerData",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PlayerData* self = (PlayerData*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'delete'", NULL);
#endif
  Mtolua_delete(self);
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_new01
static int tolua_playerdata_PlayerData_new01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"PlayerData",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const PlayerData",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  const PlayerData* from = ((const PlayerData*)  tolua_tousertype(tolua_S,2,0));
  {
   PlayerData* tolua_ret = (PlayerData*)  Mtolua_new((PlayerData)(*from));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"PlayerData");
  }
 }
 return 1;
tolua_lerror:
 return tolua_playerdata_PlayerData_new00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_new01_local
static int tolua_playerdata_PlayerData_new01_local(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"PlayerData",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const PlayerData",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  const PlayerData* from = ((const PlayerData*)  tolua_tousertype(tolua_S,2,0));
  {
   PlayerData* tolua_ret = (PlayerData*)  Mtolua_new((PlayerData)(*from));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"PlayerData");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
tolua_lerror:
 return tolua_playerdata_PlayerData_new00_local(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: default_instance of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_default_instance00
static int tolua_playerdata_PlayerData_default_instance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"PlayerData",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   const PlayerData& tolua_ret = (const PlayerData&)  PlayerData::default_instance();
    tolua_pushusertype(tolua_S,(void*)&tolua_ret,"const PlayerData");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'default_instance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: New of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_New00
static int tolua_playerdata_PlayerData_New00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const PlayerData",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const PlayerData* self = (const PlayerData*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'New'", NULL);
#endif
  {
   PlayerData* tolua_ret = (PlayerData*)  self->New();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"PlayerData");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'New'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateByString of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_CreateByString00
static int tolua_playerdata_PlayerData_CreateByString00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
	tolua_Error tolua_err;
	if (
		!tolua_isusertype(tolua_S, 1, "const PlayerData", 0, &tolua_err) ||
		!tolua_isstring(tolua_S, 2, 0, &tolua_err) ||
		!tolua_isnoobj(tolua_S, 3, &tolua_err)
		)
		goto tolua_lerror;
	else
#endif
	{
		const PlayerData* self = (const PlayerData*)tolua_tousertype(tolua_S, 1, 0);
		const char* SerializeStr = ((const char*)tolua_tostring(tolua_S, 2, 0));
#ifndef TOLUA_RELEASE
		if (!self) tolua_error(tolua_S, "invalid 'self' in function 'CreateByString'", NULL);
#endif
		{
			PlayerData* tolua_ret = (PlayerData*)self->CreateByString(SerializeStr);
			tolua_pushusertype(tolua_S, (void*)tolua_ret, "PlayerData");
		}
	}
	return 1;
#ifndef TOLUA_RELEASE
tolua_lerror :
	tolua_error(tolua_S, "#ferror in function 'CreateByString'.", &tolua_err);
	return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CopyFrom of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_CopyFrom00
static int tolua_playerdata_PlayerData_CopyFrom00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PlayerData",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const PlayerData",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PlayerData* self = (PlayerData*)  tolua_tousertype(tolua_S,1,0);
  const PlayerData* from = ((const PlayerData*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CopyFrom'", NULL);
#endif
  {
   self->CopyFrom(*from);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CopyFrom'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: MergeFrom of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_MergeFrom00
static int tolua_playerdata_PlayerData_MergeFrom00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PlayerData",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const PlayerData",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PlayerData* self = (PlayerData*)  tolua_tousertype(tolua_S,1,0);
  const PlayerData* from = ((const PlayerData*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'MergeFrom'", NULL);
#endif
  {
   self->MergeFrom(*from);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'MergeFrom'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Clear of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_Clear00
static int tolua_playerdata_PlayerData_Clear00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PlayerData",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PlayerData* self = (PlayerData*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Clear'", NULL);
#endif
  {
   self->Clear();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Clear'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsInitialized of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_IsInitialized00
static int tolua_playerdata_PlayerData_IsInitialized00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const PlayerData",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const PlayerData* self = (const PlayerData*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsInitialized'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsInitialized();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsInitialized'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ByteSize of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_ByteSize00
static int tolua_playerdata_PlayerData_ByteSize00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const PlayerData",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const PlayerData* self = (const PlayerData*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ByteSize'", NULL);
#endif
  {
   int tolua_ret = (int)  self->ByteSize();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ByteSize'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: has_sid of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_has_sid00
static int tolua_playerdata_PlayerData_has_sid00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const PlayerData",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const PlayerData* self = (const PlayerData*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'has_sid'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->has_sid();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'has_sid'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: clear_sid of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_clear_sid00
static int tolua_playerdata_PlayerData_clear_sid00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PlayerData",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PlayerData* self = (PlayerData*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'clear_sid'", NULL);
#endif
  {
   self->clear_sid();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'clear_sid'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: sid of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_sid00
static int tolua_playerdata_PlayerData_sid00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const PlayerData",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const PlayerData* self = (const PlayerData*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'sid'", NULL);
#endif
  {
   int tolua_ret = (int)  self->sid();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'sid'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_sid of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_set_sid00
static int tolua_playerdata_PlayerData_set_sid00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PlayerData",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PlayerData* self = (PlayerData*)  tolua_tousertype(tolua_S,1,0);
  int value = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_sid'", NULL);
#endif
  {
   self->set_sid(value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_sid'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: has_playername of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_has_playername00
static int tolua_playerdata_PlayerData_has_playername00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const PlayerData",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const PlayerData* self = (const PlayerData*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'has_playername'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->has_playername();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'has_playername'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: clear_playername of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_clear_playername00
static int tolua_playerdata_PlayerData_clear_playername00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PlayerData",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PlayerData* self = (PlayerData*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'clear_playername'", NULL);
#endif
  {
   self->clear_playername();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'clear_playername'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: playername of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_playername00
static int tolua_playerdata_PlayerData_playername00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const PlayerData",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const PlayerData* self = (const PlayerData*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'playername'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->playername().c_str();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'playername'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_playername of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_set_playername00
static int tolua_playerdata_PlayerData_set_playername00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PlayerData",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PlayerData* self = (PlayerData*)  tolua_tousertype(tolua_S,1,0);
  const char* value = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_playername'", NULL);
#endif
  {
   self->set_playername(value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_playername'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_playername of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_set_playername01
static int tolua_playerdata_PlayerData_set_playername01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PlayerData",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"size_t",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  PlayerData* self = (PlayerData*)  tolua_tousertype(tolua_S,1,0);
  const char* value = ((const char*)  tolua_tostring(tolua_S,2,0));
  size_t size = *((size_t*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_playername'", NULL);
#endif
  {
   self->set_playername(value,size);
  }
 }
 return 0;
tolua_lerror:
 return tolua_playerdata_PlayerData_set_playername00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: has_bisnewplayer of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_has_bisnewplayer00
static int tolua_playerdata_PlayerData_has_bisnewplayer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const PlayerData",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const PlayerData* self = (const PlayerData*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'has_bisnewplayer'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->has_bisnewplayer();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'has_bisnewplayer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: clear_bisnewplayer of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_clear_bisnewplayer00
static int tolua_playerdata_PlayerData_clear_bisnewplayer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PlayerData",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PlayerData* self = (PlayerData*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'clear_bisnewplayer'", NULL);
#endif
  {
   self->clear_bisnewplayer();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'clear_bisnewplayer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: bisnewplayer of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_bisnewplayer00
static int tolua_playerdata_PlayerData_bisnewplayer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const PlayerData",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const PlayerData* self = (const PlayerData*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'bisnewplayer'", NULL);
#endif
  {
   int tolua_ret = (int)  self->bisnewplayer();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'bisnewplayer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_bisnewplayer of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_set_bisnewplayer00
static int tolua_playerdata_PlayerData_set_bisnewplayer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PlayerData",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PlayerData* self = (PlayerData*)  tolua_tousertype(tolua_S,1,0);
  int value = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_bisnewplayer'", NULL);
#endif
  {
   self->set_bisnewplayer(value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_bisnewplayer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: skillindex_size of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_skillindex_size00
static int tolua_playerdata_PlayerData_skillindex_size00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const PlayerData",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const PlayerData* self = (const PlayerData*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'skillindex_size'", NULL);
#endif
  {
   int tolua_ret = (int)  self->skillindex_size();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'skillindex_size'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: clear_skillindex of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_clear_skillindex00
static int tolua_playerdata_PlayerData_clear_skillindex00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PlayerData",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PlayerData* self = (PlayerData*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'clear_skillindex'", NULL);
#endif
  {
   self->clear_skillindex();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'clear_skillindex'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: skillindex of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_skillindex00
static int tolua_playerdata_PlayerData_skillindex00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const PlayerData",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const PlayerData* self = (const PlayerData*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'skillindex'", NULL);
#endif
  {
   int tolua_ret = (int)  self->skillindex(index);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((int)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"int");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(int32));
     tolua_pushusertype(tolua_S,tolua_obj,"int32");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'skillindex'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_skillindex of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_set_skillindex00
static int tolua_playerdata_PlayerData_set_skillindex00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PlayerData",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"int32",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PlayerData* self = (PlayerData*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
  int value = *((int*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_skillindex'", NULL);
#endif
  {
   self->set_skillindex(index,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_skillindex'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: add_skillindex of class  PlayerData */
#ifndef TOLUA_DISABLE_tolua_playerdata_PlayerData_add_skillindex00
static int tolua_playerdata_PlayerData_add_skillindex00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PlayerData",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"int32",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PlayerData* self = (PlayerData*)  tolua_tousertype(tolua_S,1,0);
  int value = *((int*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'add_skillindex'", NULL);
#endif
  {
   self->add_skillindex(value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'add_skillindex'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_playerdata_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"PlayerData","PlayerData","::google::protobuf::Message",tolua_collect_PlayerData);
  #else
  tolua_cclass(tolua_S,"PlayerData","PlayerData","::google::protobuf::Message",NULL);
  #endif
  tolua_beginmodule(tolua_S,"PlayerData");
   tolua_function(tolua_S,"new",tolua_playerdata_PlayerData_new00);
   tolua_function(tolua_S,"new_local",tolua_playerdata_PlayerData_new00_local);
   tolua_function(tolua_S,".call",tolua_playerdata_PlayerData_new00_local);
   tolua_function(tolua_S,"delete",tolua_playerdata_PlayerData_delete00);
   tolua_function(tolua_S,"new",tolua_playerdata_PlayerData_new01);
   tolua_function(tolua_S,"new_local",tolua_playerdata_PlayerData_new01_local);
   tolua_function(tolua_S,".call",tolua_playerdata_PlayerData_new01_local);
   tolua_function(tolua_S,"default_instance",tolua_playerdata_PlayerData_default_instance00);
   tolua_function(tolua_S,"New",tolua_playerdata_PlayerData_New00);
   tolua_function(tolua_S, "CreateByString", tolua_playerdata_PlayerData_CreateByString00);
   tolua_function(tolua_S,"CopyFrom",tolua_playerdata_PlayerData_CopyFrom00);
   tolua_function(tolua_S,"MergeFrom",tolua_playerdata_PlayerData_MergeFrom00);
   tolua_function(tolua_S,"Clear",tolua_playerdata_PlayerData_Clear00);
   tolua_function(tolua_S,"IsInitialized",tolua_playerdata_PlayerData_IsInitialized00);
   tolua_function(tolua_S,"ByteSize",tolua_playerdata_PlayerData_ByteSize00);
   tolua_function(tolua_S,"has_sid",tolua_playerdata_PlayerData_has_sid00);
   tolua_function(tolua_S,"clear_sid",tolua_playerdata_PlayerData_clear_sid00);
   tolua_function(tolua_S,"sid",tolua_playerdata_PlayerData_sid00);
   tolua_function(tolua_S,"set_sid",tolua_playerdata_PlayerData_set_sid00);
   tolua_function(tolua_S,"has_playername",tolua_playerdata_PlayerData_has_playername00);
   tolua_function(tolua_S,"clear_playername",tolua_playerdata_PlayerData_clear_playername00);
   tolua_function(tolua_S,"playername",tolua_playerdata_PlayerData_playername00);
   tolua_function(tolua_S,"set_playername",tolua_playerdata_PlayerData_set_playername00);
   tolua_function(tolua_S,"set_playername",tolua_playerdata_PlayerData_set_playername01);
   tolua_function(tolua_S,"has_bisnewplayer",tolua_playerdata_PlayerData_has_bisnewplayer00);
   tolua_function(tolua_S,"clear_bisnewplayer",tolua_playerdata_PlayerData_clear_bisnewplayer00);
   tolua_function(tolua_S,"bisnewplayer",tolua_playerdata_PlayerData_bisnewplayer00);
   tolua_function(tolua_S,"set_bisnewplayer",tolua_playerdata_PlayerData_set_bisnewplayer00);
   tolua_function(tolua_S,"skillindex_size",tolua_playerdata_PlayerData_skillindex_size00);
   tolua_function(tolua_S,"clear_skillindex",tolua_playerdata_PlayerData_clear_skillindex00);
   tolua_function(tolua_S,"skillindex",tolua_playerdata_PlayerData_skillindex00);
   tolua_function(tolua_S,"set_skillindex",tolua_playerdata_PlayerData_set_skillindex00);
   tolua_function(tolua_S,"add_skillindex",tolua_playerdata_PlayerData_add_skillindex00);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_playerdata (lua_State* tolua_S) {
 return tolua_playerdata_open(tolua_S);
};
#endif

