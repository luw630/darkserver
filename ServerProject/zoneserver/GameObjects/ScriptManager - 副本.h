// ----- ScriptManager.h -----
//
//   --  Author: RenYi
//   --  Date:   06/13/03
//   --  Desc:   �ű�������ͷ�ļ�
// --------------------------------------------------------------------
//   --  ���ｫ���й���������˵����нű�
//       ������ű������ݼӻ��Ĺ��ܺ�����ʵ��
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

	// ԭ��ͨ���޸Ľű�ϵͳ�����Ѿ�֧�ֿɿش�С�����ݱ������ָ�����ݶ��ǽű��������л���Ĵ洢�ռ�
	void *saveData;		// dbMgr[sid].data ���ڱ�������ݱ�  ��ʵ������ָ��������ݽṹ��Ԥ���õĻ���ռ�
	void *tempData;		// dbMgr[sid].temp ������ʱ�����ݱ�  �����ָ����������ʱ��������ģ�ֻ��Ϊ�˴������ݶ�����

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


	  BOOL TriggerEvent(const char* name, int nType);     // �����ű���ĳ���¼�
	  BOOL TriggerEvent(const stEvent *pTE);
	  int Restore( int flag = 1,const char* name = NULL );                       // �ָ���ʼ����״̬������ֵΪʧ�ܵĽű�����     

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
		  CFightObject	*m_OldpAttacker;//������
	  };

	  void PushFightObjectCondition( CScriptManager::TempFightObjectCondition &tmp );
	  void PopFightObjectCondition(CScriptManager::TempFightObjectCondition &tmp);
	  void PushCondition( CScriptManager::TempCondition &tmp );
	  void PopCondition( CScriptManager::TempCondition &tmp );
	  void GetCondition( CScriptManager::TempCondition &tmp );

	  int GetQuestSkill(const char* name, DWORD mAttackerGID, DWORD mDefenderGID, float mDefenderWorldPosX, float mDefenderWorldPosY, BYTE dwSkillIndex, struct SQuestSkill_C2S_MsgBody &sqQuestSkillMsg);
	  int GetDamageValue(const char* name, DWORD mAttackerGID, DWORD mDefenderGID, WORD dskillID, struct CFightObject::SDamage &sdamage);
	  int GetEquiptFightPower(const char* name, DWORD dEquiptUserGID, BYTE bType, BYTE bIndex, DWORD &dEquiptFightPower);//ȡ��װ��ս����
	  int GetWorldTalkLimit(CPlayer *pPlayer, const char* funcname,WORD &wresult);  //���纰�����ƣ������ƶ���lua���ж�
	  int GetMonsterVipExp(CPlayer *pPlayer, const char* funcname, DWORD dmonsterexp, DWORD &dmonsteraddexp);//��������VIP�ӳ�
	  int GetItemUseLimit(CPlayer *pPlayer, const char* funcname, DWORD ditemindex, WORD &buse);

	  
public:
	// ���빦�ܺ���  Ҳ����Ҫ����LUA��ִ�еĺ���
	// --- ����ֵ������int  
	static int L_DoFile(lua_State *L);

	static int L_rfalse(lua_State *L);
	static int L_rint(lua_State *L);
	static int L_RFalse(lua_State *L);
	static int L_GetStartServerTime(lua_State *L);
	
	CMonster		*m_pMonster;
	CPlayer			*m_pPlayer;
	CNpc			*m_pNpc;
	CBuilding		*m_pBuilding;
	SPackageItem *m_pUseItem; //��ǰʹ�õ���Ʒ
	SRawItemBuffer *m_pItemBuffer;//��ǰ�������ӿռ�

	// ս�����
	CFightObject	*m_pAttacker;//������
	CFightObject	*m_pDefencer;//������

	// ��ҳ�˵�������NPC����
	static bool IfSendTheMultiPageMsg;
private:

	BYTE       m_CopyItem[8][8];                // ������ҵ���Ʒ��

	// �ű����ɶ�����б�
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
	//20150122 wk ���������ű�ģ��
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
	void SaveTaskEx( LPCSTR key ); // ��� key == NULL ���ʾ�������еĻ�������
	STaskExBuffer* LoadTaskEx( LPCSTR key );

public:
	static bool  AddMountsProperty(int mountIndex, int porperty, int v, int max=-1);

public:
	///////////////////////////////////////////////////////////////////////////////////////////////
	// �ճ�ʱ�������
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