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
		// �����ṩ���ű��Ľӿں���
		static int L_init(lua_State *L);         // ������ĳ�ʼ������
		static int L_new(lua_State *L); 
		static int L_print(lua_State *L);

	private:

		void  CreateShadow(lua_State *L);  
		void  Test(void);

	};

	extern CSManager g_Script;
}