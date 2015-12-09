// ======= Script.h =========================================================

#pragma once

#include <vector>
#include <string>
#include <cassert>

///////////////////////////////////////////////////////////////////////////
namespace Script
{
 
	class  CSEvent 
	{	
	public:

		static int L_moveto(lua_State *L);
		static int L_getshowpos(lua_State *L);
        static int L_setevent(lua_State *L);
        static int L_settalkinfo(lua_State *L);
        static int L_addeffect(lua_State *L);
        static int L_moveshowpos(lua_State *L);
		
	public:
		// º¯ÊýÓ³Éä
	    static void CreateShadow(void);	    

	};
}
