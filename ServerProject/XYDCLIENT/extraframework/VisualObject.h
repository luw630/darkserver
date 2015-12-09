#pragma once
#include "iobject.h"

#include "ResManagerModule\ResManagerModule.h"

class CVisualObject :
    public IObject
{
public:
    IObject *GetInterface(const __int32 IID);   // 根据IID获取相应接口

private:
    // ****************************************
    // 接口定义
    HRESULT Init(SCreateParameter *pParam);     // 初始化对象
    HRESULT Exit(void);                         // 释放资源

    void UpdateAI(void);                        // 由调用者控制节奏的AI更新入口
    void UpdateShow(int x, int y);              // 由调用者控制节奏的实现更新入口，[x，y]是父对象的显示位置

    LPCSTR GetKey(void);                        // 返回自己的查询关键字

public:
    void ShowBack(int x, int y);

public:
    enum {VT_NULL, VT_STRINGSPR, VT_PLAYER, VT_MONSTER, VT_NPC};

public:
    void SetPosition(int x, int y);             // 设置自己的当前位置（针对于整个场景）
    void GetPosition(int &x, int &y);           // 返回自己的当前位置（针对于整个场景）

	void SetSpaceNumber(WORD wNumber, WORD wFBegin, WORD wFEnd); 
	void Moveto(int xPos, int yPos, int iSpeed);
    void SetEventFunc(LPCSTR szEventFunc);

    void SetTalkInfo(WORD wType, LPCSTR szTalk, DWORD dwTime);

public:
    void SetPlayerImage( EActionState ActType, WORD PlayerID = 0, WORD WeaponID = 0);           // 返回自己的当前位置（针对于整个场景）
    void AddEffect(LPCSTR szFileName, WORD wTime, bool bAutoPlay = true);

public:
    void SetRelation(CVisualObject *pSourceObject);

public:
    CVisualObject(LPCSTR szKey);
    ~CVisualObject(void);

private:
    dwt::stringkey<char[STRING_KEY_LEN]> m_Key;
    
    CVisualObject *m_pRelationObject; 
    int iReferenceCount; 

private:
	void Move(void);
    void ShowTalk(int x, int y);
    void PlayEffect(int x, int y);

private:
    int m_x;
    int m_y;

private:
    int m_ResourceType; // 当前的资源类型

    // 普通spr资源
    LPStringSpr m_StringSpr;

    // 玩家spr资源
    LPPng m_Png;
    LPWng m_Wng;
    WORD m_PlayerID;
    WORD m_WeaponID;
    DWORD m_CurFrame;

    // 怪物spr资源
    LPMog m_Mog;

    // NPCspr资源
    LPNpg m_Npg;

    DWORD dwPlay;

private:
	WORD m_wSpace;
	WORD m_wFBegin;
	WORD m_wFEnd;
	DWORD m_dwSegTime;

private:
	// 移动相关
	double DistanceLength;
	double Sin;
	double Cos;
	int SegX, SegY;
	int DestX, DestY;
    int m_iSpeed;
	DWORD m_dwMCurTime;

private:
    // 事件相关数据
    dwt::stringkey<char[STRING_KEY_LEN]> szAIFunc;

private:
    // 对话相关
    struct TALK {
        enum {
            ET_NULL,
            ET_DRAWBACK,
            ET_NOBACK
        };

        WORD wType;  

        DWORD dwTime;
        dwt::stringkey<char[255]> m_szTalkInfo;
    };

    TALK m_TalkInfo;

    // 效果相关
    struct EFFECT {
        bool bPlay;
        LPStringSpr lpSpr;

        DWORD dwTime;

    };

    std::list<EFFECT> m_EffectList;

};

// 用于创建脚本对象的参数
struct SVisualObjectParam : 
    public SObjectCreateParameter
{
};
