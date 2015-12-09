// =======  SManager.h ======================================================== 


#include "DGameScript.h"
#include "SEvent.h"


/////////////////////////////////////////////////////////////

namespace Script  
{
	class CSManager : public CGameScript
	{
	public:
		void  Run(void);
		void  RegFunc(void);
		// 所有提供给脚本的接口函数
		static int L_init(lua_State *L);         // 做最初的初始化工作
		static int L_new(lua_State *L); 
		static int L_print(lua_State *L);

	private:

		void  CreateShadow(lua_State *L);  
		void  Test(void);

	};

	extern CSManager g_Script;
}