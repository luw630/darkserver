// ======== SObject.h ======================================================

#pragma once


////////////////////////////////////////////////////

namespace Script
{
    
	class CSRegionObject 
	{
	public:
		static int L_CreateOldMapRegion(lua_State* L);    
        static int L_loadRegion(lua_State* L);   
		static int L_setViewPos(lua_State* L);

	public:
		static void CreateShadow(void);


	};


	class CSVisualObject
	{
	protected:
		enum eOBJECT_ATTRIBUTE {
			EOA_POS = 1,
			EOA_PIC,
			EOA_ACTION,
			EOA_SPACE,
		};
	public:
		static int L_createVObject(lua_State *L);
		static int L_createVTObject(lua_State *L);

		static int L_getParam(lua_State *L);   
        static int L_getObject(lua_State *L);
        static int L_setVObjectAttribute(lua_State *L);
        

	public:
		static void CreateShadow(void);


	};


}