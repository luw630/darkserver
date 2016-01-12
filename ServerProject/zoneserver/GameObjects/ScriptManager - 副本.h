// ----- ScriptManager.h -----
//
//   --  Author: RenYi
//   --  Date:   06/13/03
//   --  Desc:   脚本管理类头文件
// --------------------------------------------------------------------
//   --  这里将集中管理服务器端的所有脚本
//       程序与脚本的数据加换的功能函数的实现
//---------------------------------------------------------------------    
#pragma once

#include <time.h>
#include <cassert>
#include <xydclient\extraframework\dgamescript.h>
#include <liteserializer/variant.h>
#include "Networkmodule\ItemTypedef.h"
#include "FightObject.h"
//#include <pub/waitqueue.h>

//#pragma comment (lib, "DGameScript.lib")
#pragma comment (lib, "lualib.lib")


struct stEvent {

	enum {
		TE_MONSTER_DEAD = 1,
		TE_MONSTER_REFRESH,
		TE_MONSTER_AI,
		TE_NPC_TALK,
	}TE_Type;
}; 

struct stLuaData
{
	char	cstrDataKey[30];	//table--name
	WORD	wDataLen;	//table--len
	stLuaData::stLuaData():wDataLen(0){memset(cstrDataKey,0,30);}
};

enum ObjectType {  

	OT_MONSTER = 1, 
	OT_PLAYER, 
	OT_NPC 
};

struct STaskExBuffer
{
	STaskExBuffer() : saveData(0), tempData(0), saveSize(0), tempSize(0), flags(0) {}
	~STaskExBuffer() 
	{ 
		if (saveSize && saveData && ((flags & SIZE1_FIXED) == 0)) 
			delete saveData; 
		if (tempSize && tempData) 
			delete tempData; 
	}

	// 原理，通过修改脚本系统，现已经支持可控大小的数据表，这里的指针数据都是脚本数据序列化后的存储空间
	void *saveData;		// dbMgr[sid].data 用于保存的数据表！  这实际上是指向玩家数据结构中预备好的缓存空间
	void *tempData;		// dbMgr[sid].temp 用于临时性数据表！  这里的指针数据是临时分配出来的，只是为了传递数据而存在

	int saveSize;
	int tempSize;

	enum { SIZE1_FIXED = 1, SIZE2_FIXED = 2, ERROR_IN_SERIAL = 16 };
	DWORD flags;
};

///////////////////////////////////////////////////////////////////////////////////
class CScriptManager : public CGameScript
{
protected:  
	BOOL  AddGoods(void);

public:
	CScriptManager() : 
	  m_pMonster( NULL ),
		  m_pPlayer( NULL ),
		  m_pNpc( NULL ),
		  m_dwLoopTime(0), 
		  m_pAttacker(NULL),
		  m_pDefencer(NULL),
		  prepared_paramnum( -1 ), 
		  load_errs( 0 ) 
	  {
		  _time64(&m_EveryDayManagerTime);
		  m_MonsterList.resize(65536);
		  m_NpcList.resize(65536);
	  };

	  void SetScriptPath(const char *rootpath);          
	  BOOL RegisterFunc(void);
	  void LuaRegisterFunc();
	  void SetCondition(class CMonster *pMonster, class CPlayer *pPlayer, class CNpc *pNpc, class CBuilding *pBuilding = NULL );
	  void CleanCondition(void); 

	  void SetFightCondition(class CFightObject *pAttacker);
	  void CleanFightCondition();

	  void SetFightDefencer(class CFightObject *pDefencer);
	  void CleanFightDefencer();

	  void SetInUseItemPackage(SPackageItem *pitem);
	  void CleanItemPackage();

	  void SetRawItemBuffer(SRawItemBuffer *ItemBuffer);
	  void CleanRawItemBuffer();

	  void SetPlayer(class CPlayer *pPlayer);
	  void CleanPlayer();

	  void SetMonster(class CMonster *pmonster);
	  void CleanMonster();


	  BOOL TriggerEvent(const char* name, int nType);     // 触发脚本的某个事件
	  BOOL TriggerEvent(const stEvent *pTE);
	  int Restore( int flag = 1,const char* name = NULL );                       // 恢复初始化的状态，返回值为失败的脚本数量     

	  BOOL DoFunc(const char *filename, DWORD wScriptID, DWORD dwChoseID = 0);    
	  //BOOL LostItems(ObjectType Who, WORD wItemID, WORD wLife, WORD wRange, DWORD dwMoney);

	  DWORD  GetMonsterID(WORD wIndex) const  { return ( wIndex < m_MonsterList.size() ) ? m_MonsterList[wIndex] : 0; } 
	  DWORD  GetNpcID(WORD wIndex) const  { return ( wIndex < m_NpcList.size() ) ? m_NpcList[wIndex] : 0; } 
	  void SetMonsterID(WORD wMonsterID, DWORD dwGID) { if ( wMonsterID < m_MonsterList.size() ) m_MonsterList[wMonsterID] = dwGID; }
	  void SetNpcID(WORD wNpcID, DWORD dwGID) { if ( wNpcID < m_NpcList.size() ) m_NpcList[wNpcID] = dwGID; }
	  void SetMListSize(int nSize) { m_MonsterList.resize(nSize); }
	  void SetNListSize(int nSize) { m_NpcList.resize(nSize); }

	  void SetLoop(void);

	  struct TempCondition {
		  CMonster   *m_pOldMonster;
		  CPlayer    *m_pOldPlayer;
		  CNpc       *m_pOldNpc;
		  CBuilding  *m_pOldBuilding;
	  };
	  struct TempFightObjectCondition
	  {
		  CFightObject	*m_OldpAttacker;//攻击者
	  };

	  void PushFightObjectCondition( CScriptManager::TempFightObjectCondition &tmp );
	  void PopFightObjectCondition(CScriptManager::TempFightObjectCondition &tmp);
	  void PushCondition( CScriptManager::TempCondition &tmp );
	  void PopCondition( CScriptManager::TempCondition &tmp );
	  void GetCondition( CScriptManager::TempCondition &tmp );

	  int GetQuestSkill(const char* name, DWORD mAttackerGID, DWORD mDefenderGID, float mDefenderWorldPosX, float mDefenderWorldPosY, BYTE dwSkillIndex, struct SQuestSkill_C2S_MsgBody &sqQuestSkillMsg);
	  int GetDamageValue(const char* name, DWORD mAttackerGID, DWORD mDefenderGID, WORD dskillID, struct CFightObject::SDamage &sdamage);
	  int GetEquiptFightPower(const char* name, DWORD dEquiptUserGID, BYTE bType, BYTE bIndex, DWORD &dEquiptFightPower);//取得装备战斗力
	  int GetWorldTalkLimit(CPlayer *pPlayer, const char* funcname,WORD &wresult);  //世界喊话限制，现在移动到lua中判断
	  int GetMonsterVipExp(CPlayer *pPlayer, const char* funcname, DWORD dmonsterexp, DWORD &dmonsteraddexp);//怪物死亡VIP加成
	  int GetItemUseLimit(CPlayer *pPlayer, const char* funcname, DWORD ditemindex, WORD &buse);

	  
public:
	// 加入功能函数  也就是要放入LUA中执行的函数
	// --- 返回值必须是int  
	static int L_DoFile(lua_State *L);

	static int L_rfalse(lua_State *L);
	static int L_rint(lua_State *L);
	static int L_RFalse(lua_State *L);
	static int L_GetStartServerTime(lua_State *L);
	
	CMonster		*m_pMonster;
	CPlayer			*m_pPlayer;
	CNpc			*m_pNpc;
	CBuilding		*m_pBuilding;
	SPackageItem *m_pUseItem; //当前使用的物品
	SRawItemBuffer *m_pItemBuffer;//当前背包格子空间

	// 战斗相关
	CFightObject	*m_pAttacker;//攻击者
	CFightObject	*m_pDefencer;//被攻击

	// 分页菜单，控制NPC买卖
	static bool IfSendTheMultiPageMsg;
private:

	BYTE       m_CopyItem[8][8];                // 镜像玩家的物品栏

	// 脚本生成对象的列表
	std::vector<DWORD>  m_MonsterList;
	std::vector<DWORD>  m_NpcList;    

	friend class CSMonsterFunc;
	friend class CSNPCFunc;
	friend class CSPlayerFunc;
	friend class CSOtherFunc;
	friend class LuaFunctor;
	friend int NotifyMaster(CPlayer *);

	DWORD m_dwLoopTime;

	friend struct ExtraLuaFunctions;
	//20150122 wk 三国独立脚本模块
	friend class CSSanGuoFunc;
public:
	BOOL PrepareFunction( LPCSTR funcname );
	void PrepareParameters( lite::Serialreader &slr );
	void PushParameter( lite::Variant const &lvt );
	void PushVariantArray( lite::Serialreader &slr );
	void PushVariantTable( lite::Serialreader &slr, int tableType );
	void PushDWORDArray( std::list< DWORD > &lst );

	BOOL Execute( lite::Variant *result = NULL );
	void PrepareBreak();

private:
	int prepared_paramnum;

public:
	int load_errs;

public:
	std::map< std::string, STaskExBuffer > taskExMgr;
	void SaveTaskEx( LPCSTR key ); // 如果 key == NULL 则表示保存所有的缓存数据
	STaskExBuffer* LoadTaskEx( LPCSTR key );

public:
	static bool  AddMountsProperty(int mountIndex, int porperty, int v, int max=-1);

public:
	///////////////////////////////////////////////////////////////////////////////////////////////
	// 日常时间管理器
	INT64		m_EveryDayManagerTime;

	int OnEveryDayManagerRun();
	static int L_ReadEveryDayManagerTime(lua_State *L);
	static int L_WriteEveryDayManagerTime(lua_State *L);

	///////////////////////////////////////////////////////////////////////////////////////////////
};

class LuaFunctor
{
public:
	LuaFunctor( CScriptManager &script, LPCSTR funcName );
	LuaFunctor& operator [] ( lite::Variant const &lvt );
	BOOL operator () ( lite::Variant *result = NULL );

private:
	BOOL            initFailre;
	lua_State       *callStack;
	const LPCSTR    funcName;
	const int       stackTop;
};

extern CScriptManager g_Script; 

class CScriptState
{
public:
	CScriptState(CMonster* pMonster, CPlayer* pPlayer, CNpc* pNpc, CBuilding* pBuilding)
	{
		m_pMonster = g_Script.m_pMonster;
		m_pPlayer = g_Script.m_pPlayer;
		m_pNpc = g_Script.m_pNpc;
		m_pBuilding = g_Script.m_pBuilding;
		g_Script.SetCondition(pMonster, pPlayer, pNpc, pBuilding);
	}

	~CScriptState()
	{
		g_Script.SetCondition(m_pMonster, m_pPlayer, m_pNpc, m_pBuilding);
	}

private:
	CMonster* m_pMonster;
	CPlayer* m_pPlayer;
	CNpc* m_pNpc;
	CBuilding* m_pBuilding;
};