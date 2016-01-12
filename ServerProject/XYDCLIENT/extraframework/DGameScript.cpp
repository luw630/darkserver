//----- DGameScript.cpp ------
//
//   --  Author: RenYi
//   --  Date:   04/24/03
//   --  Desc:   脚本引擎类实现文件
//---------------------------------------------------------------------

#include "stdafx.h"

//#include <windows.h>
#include <assert.h>

#include "DGameScript.h"

extern "C" {
#include "lauxlib.h"
#include "lualib.h"
}
extern BOOL rfalse(char, char, LPCSTR, ...);
using namespace std;

//extern int rfalse(char, char, const char*, ...);

void CGameScript::OpenLua()
{
	ls = luaL_newstate();
	lua_gc( ls, LUA_GCSTOP, 0 );    /* stop collector during initialization */
	luaL_openlibs( ls );            /* open libraries */
	lua_gc( ls, LUA_GCRESTART, 0 );

	extern int L_dofile( lua_State *L );
	lua_register(ls, "dofile", L_dofile);

	//lua_baselibopen( ls );
	//lua_mathlibopen( ls );
	//lua_iolibopen( ls );
	//lua_strlibopen( ls );
	//lua_dblibopen( ls );
}

void CGameScript::CloseLua()
{
	lua_close( ls );
	ls = NULL;
}

CGameScript::CGameScript()
{
	OpenLua();
	path[0] = NULL;
}

CGameScript::~CGameScript()
{
	CloseLua();
}

void CGameScript::Reset()
{
	CloseLua();
	OpenLua();
}

void CGameScript::SetPath(const char *rootpath)
{
	if (strlen(rootpath) < 512)
	{
		strcpy(path, rootpath);
	}   
}

int CGameScript::DoFile(const char *luafile)
{
	char buf[1024];
	strcpy(buf, path);
	strcat(buf, luafile);

	PakFile file;
	if (!file.Open(buf)) 
	{
		char temp[1024];
		sprintf(temp, "%s\r\n", buf);
		OutputDebugString(temp);
		rfalse(1,1,temp);
		return -1;
	}

	char *data = new char[file.Size()+1];
	file.Read( data, file.Size() );
	data[file.Size()] = 0;

	int ret = DoBuffer(data, file.Size(), buf);

	//delete data;
	delete	[]data;
	data = 0;
	//SAFE_DELETE_ARRAY(data);

	return ret;
}

int CGameScript::DoString(const char *str)
{
	char temp[1024];
	int ret = luaL_loadstring( ls, str );
	if ( ret != 0 )
	{
		LPCSTR str = lua_tostring( ls, -1 );
		if ( str )
		{
			sprintf( temp, "%s\r\n", str );
			OutputDebugString( temp );
			rfalse(1,1,temp);
			lua_pop( ls, 1 );
		}
		return ret;
	}

	ret = lua_pcall( ls, 0, LUA_MULTRET, 0 );
	if ( ret != 0 )
	{
		LPCSTR str = lua_tostring( ls, -1 );
		if ( str )
		{
			sprintf( temp, "%s\r\n", str );
			OutputDebugString( temp );
			rfalse(1,1,temp);
			lua_pop( ls, 1 );
		}
	}

	return ret;
}

int L_dofile( lua_State *L ) {
	if ( lua_type( L, 1 ) != LUA_TSTRING ) {
		OutputDebugString( "dofile : error arg1\r\n" );
		return 0;
	}

	char temp[1024];
	LPCSTR luafile = lua_tostring( L, 1 );

	PakFile file;
	if ( !file.Open( ( LPSTR )luafile ) ) {
		sprintf( temp, "can't open %s\r\n", luafile );
		OutputDebugString( temp );
		rfalse(1,1,temp);
		return 0;
	}

	char *data = new char[file.Size()+1];
	file.Read( data, file.Size() );
	data[file.Size()] = 0;

	int top = lua_gettop( L );
	int ret = luaL_loadbuffer( L, data, file.Size(), luafile );
	if ( ret == 0 )
		ret = lua_pcall( L, 0, LUA_MULTRET, 0 );
	delete  []data;
	data = 0;
	if ( LPCSTR str = lua_tostring( L, -1 ) ) {
		sprintf( temp, "%s\r\n", str );
		OutputDebugString( temp );
		rfalse(1,1,temp);
		lua_pop( L, 1 );
	}
	lua_settop( L, top );
	lua_pushnumber( L, ret );
	return 1;
}

int CGameScript::DoBuffer(const char *buf, size_t size, const char *name)
{
	char temp[1024];
	int ret = luaL_loadbuffer( ls, buf, size, name );
	if ( ret != 0 )
	{
		LPCSTR str = lua_tostring( ls, -1 );
		if ( str )
		{
			sprintf( temp, "%s\r\n", str );
			OutputDebugString( temp );
			rfalse(1,1,temp);
			lua_pop( ls, 1 );
		}
		return ret;
	}

	ret = lua_pcall( ls, 0, LUA_MULTRET, 0 );
	if ( ret != 0 )
	{
		LPCSTR str = lua_tostring( ls, -1 );
		if ( str )
		{
			sprintf( temp, "%s\r\n", str );
			OutputDebugString( temp );
			rfalse(1,1,temp);
			lua_pop( ls, 1 );
		}
		 return ret;
	}

	return ret;
}

int CGameScript::DoBuffer(const char *buf, size_t size)
{
	return DoBuffer( buf, size, 0 );
}

void CGameScript::RegisterFunction(LPCSTR FuncName, lua_CFunction Func)
{
	lua_register(ls, FuncName, Func);
}

#define PREPARE_CALL                                            \
	if ( ( name == NULL ) || ( name[0] == 0 ) ) return false;   \
	int stackPos = lua_gettop( ls );                            \
	lua_getglobal( ls, name );                                  \
	if ( lua_isnil( ls, -1 ) ) { lua_pop( ls, 1 ); return false; }

#define DO_CALL                                                 \
	if ( !Call( stackPos ) ) return false; return true;

bool CGameScript::CallFunc( const char *name )
{
	PREPARE_CALL
		DO_CALL
}

bool CGameScript::CallFunc( const char *name, CScriptValue arg )
{	
	PREPARE_CALL
		Push( arg ); 
	DO_CALL
}

bool CGameScript::CallFunc( const char *name, CScriptValue arg1, CScriptValue arg2 )
{
	PREPARE_CALL
		Push( arg1 ); 
	Push( arg2 ); 
	DO_CALL
}

bool CGameScript::CallFunc( const char *name, CScriptValue arg1, CScriptValue arg2, CScriptValue arg3 )
{
	PREPARE_CALL
		Push( arg1 ); 
	Push( arg2 ); 
	Push( arg3 ); 
	DO_CALL
}

bool CGameScript::CallFunc( const char *name, CScriptValue arg1, 
	CScriptValue arg2, CScriptValue arg3, CScriptValue arg4 )
{
	PREPARE_CALL
		Push( arg1 ); 
	Push( arg2 ); 
	Push( arg3 ); 
	Push( arg4 ); 
	DO_CALL
}

bool CGameScript::CallFunc( const char *name, CScriptValue arg1, 
	CScriptValue arg2, CScriptValue arg3, CScriptValue arg4, CScriptValue arg5 )
{
	PREPARE_CALL
		Push( arg1 ); 
	Push( arg2 ); 
	Push( arg3 ); 
	Push( arg4 ); 
	Push( arg5 ); 
	DO_CALL
}

bool CGameScript::Call( int stackPos )
{
	int curPos = lua_gettop( ls );
	if ( stackPos >= curPos )
		return false;

	if ( lua_pcall( ls, curPos - stackPos - 1, LUA_MULTRET, 0 ) != 0 ) 
	{
		char temp[1024];
		sprintf( temp, "%s\r\n", lua_tostring( ls, -1 ) );
		OutputDebugString( temp );
		rfalse(1,1,temp);
		lua_settop( ls, stackPos );
		return false;
	}

	int nNum = lua_gettop( ls ) - stackPos;
	SetSize( nNum );
	for ( int i = 0; i < nNum; i ++ )
	{
		SetRet( i, GetTopValue() );
		lua_pop( ls, 1 );
	}

	return true;
}

void CGameScript::Push( CScriptValue &value )
{
	switch(value.GetType())
	{
	case value.VT_NIL :
		lua_pushnil(ls);
		break;

	case value.VT_NUMBER:
		lua_pushnumber(ls, value);
		break;

	case value.VT_STRING:
		lua_pushstring(ls, value);
		break;

	case value.VT_FUNCTION:
		lua_pushcfunction(ls, value);
		break;

	case value.VT_POINTER:
		{
			VariablePointer p = (VariablePointer)value;
			lua_pushlstring(ls, (LPCSTR)&p, 4);
		}
		break;

	case value.VT_USERDATA:
		// lua_pushusertag(ls, (char*)value, value.GetType());
		{
			void *p = NULL;
			int size = 0;
			if (value.GetUserData(p, size))
			{
				lua_pushlstring(ls, (LPCSTR)p, size);
			}
		}
		break;
	}
}

CScriptValue CGameScript::GetTopValue(int nIndex)
{
	if(lua_isnil(ls, nIndex))
		return CScriptValue();

	if(lua_isnumber(ls, nIndex))
		return CScriptValue((float)lua_tonumber(ls, nIndex));

	if(lua_isstring(ls, nIndex))
		return CScriptValue(lua_tostring(ls, nIndex));

	if(lua_iscfunction(ls, nIndex))
		return CScriptValue(lua_tocfunction(ls, nIndex));

	if(lua_isuserdata(ls, nIndex))
	{
		assert(!"not avariable");
		return CScriptValue(lua_touserdata(ls, nIndex), 0);
	}

	return CScriptValue();
}


CScriptValue CGameScript::GetGlobalValue(const char *name)
{
	lua_getglobal(ls, name);
	CScriptValue ret = GetTopValue();
	lua_settop(ls, -2);

	return ret;
}

void CGameScript::SetGlobalValue(CScriptValue& val, const char* name)
{
	Push(val);
	lua_setglobal(ls, name);
}

bool CGameScript::SetTableValue(const char *sTableIndexName, CScriptValue& value)
{
	char tempbuf[256];
	strncpy(tempbuf, sTableIndexName, 255); tempbuf[255] = 0;

	char *pch = tempbuf;
	bool finddot = false;
	while (*pch++ != 0) if (*pch == '.') { finddot = true; *pch = 0; break; }

	if (!finddot) return false;

	if (isdigit(*tempbuf)) return false;

	// save current esp
	int t = lua_gettop(ls);

	// locate dest table
	if (!GetGlobalTable(tempbuf)) return false;

	// locate table item
	if (RecursionGetTable(++pch))
	{
		Push(value);
		lua_settable(ls, -3);
	}

	// pop all the located table/item
	lua_settop(ls, t);

	return true;
}

CScriptValue CGameScript::GetTableValue(const char *sTableIndexName)
{
	CScriptValue temp;
	char tempbuf[256];
	strncpy(tempbuf, sTableIndexName, 255); tempbuf[255] = 0;

	char *pch = tempbuf;
	bool finddot = false;
	while (*pch++ != 0) if (*pch == '.') { finddot = true; *pch = 0; break; }

	if (!finddot) return CScriptValue();

	if (isdigit(*tempbuf)) return CScriptValue();

	// save current esp
	int t = lua_gettop(ls);

	// locate dest table
	if (!GetGlobalTable(tempbuf)) return CScriptValue();

	// locate table item
	if (RecursionGetTable(++pch))
	{
		// push dest table item in lua_stack
		if (GetTableItem(-2))
		{
			// get value;
			temp = GetTopValue();
		}
	}

	// pop all the located table/item
	lua_settop(ls, t);

	return temp;
}

bool CGameScript::RecursionGetTable(char *szNameEntry)
{
	size_t count = 0;
	size_t size = strlen(szNameEntry)+1;
	bool isEnd = false;
	while (count < size)
	{
		if (szNameEntry[count] == '.')  
		{
			szNameEntry[count] = 0; 
			break; 

		}
		if (szNameEntry[count] == '\0')  
		{
			isEnd = true;
			break; 
		}
		count ++;
	}

	if (isdigit(*szNameEntry))
	{
		// is number
		int number = atoi(szNameEntry);
		Push(CScriptValue(number));
	}
	else
	{
		// is name
		Push(CScriptValue(szNameEntry));
	}

	if (isEnd) 
	{
		return true;
	}
	else
	{
		if (!GetTable(-2))
		{
			lua_pop(ls, 1);
			return false;
		}
	}

	return RecursionGetTable(&szNameEntry[count+1]);
}

int CGameScript::GetTableSize(const char *sTableName)
{ 
	lua_getglobal(ls, sTableName); 
	int size = lua_objlen(ls, -1);
	lua_settop(ls, -2); 
	return size; 
} 
