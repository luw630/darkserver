#pragma once
#include "iobject.h"

#include "ResManagerModule\ResManagerModule.h"

class CVisualObject :
    public IObject
{
public:
    IObject *GetInterface(const __int32 IID);   // ����IID��ȡ��Ӧ�ӿ�

private:
    // ****************************************
    // �ӿڶ���
    HRESULT Init(SCreateParameter *pParam);     // ��ʼ������
    HRESULT Exit(void);                         // �ͷ���Դ

    void UpdateAI(void);                        // �ɵ����߿��ƽ����AI�������
    void UpdateShow(int x, int y);              // �ɵ����߿��ƽ����ʵ�ָ�����ڣ�[x��y]�Ǹ��������ʾλ��

    LPCSTR GetKey(void);                        // �����Լ��Ĳ�ѯ�ؼ���

public:
    void ShowBack(int x, int y);

public:
    enum {VT_NULL, VT_STRINGSPR, VT_PLAYER, VT_MONSTER, VT_NPC};

public:
    void SetPosition(int x, int y);             // �����Լ��ĵ�ǰλ�ã����������������
    void GetPosition(int &x, int &y);           // �����Լ��ĵ�ǰλ�ã����������������

	void SetSpaceNumber(WORD wNumber, WORD wFBegin, WORD wFEnd); 
	void Moveto(int xPos, int yPos, int iSpeed);
    void SetEventFunc(LPCSTR szEventFunc);

    void SetTalkInfo(WORD wType, LPCSTR szTalk, DWORD dwTime);

public:
    void SetPlayerImage( EActionState ActType, WORD PlayerID = 0, WORD WeaponID = 0);           // �����Լ��ĵ�ǰλ�ã����������������
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
    int m_ResourceType; // ��ǰ����Դ����

    // ��ͨspr��Դ
    LPStringSpr m_StringSpr;

    // ���spr��Դ
    LPPng m_Png;
    LPWng m_Wng;
    WORD m_PlayerID;
    WORD m_WeaponID;
    DWORD m_CurFrame;

    // ����spr��Դ
    LPMog m_Mog;

    // NPCspr��Դ
    LPNpg m_Npg;

    DWORD dwPlay;

private:
	WORD m_wSpace;
	WORD m_wFBegin;
	WORD m_wFEnd;
	DWORD m_dwSegTime;

private:
	// �ƶ����
	double DistanceLength;
	double Sin;
	double Cos;
	int SegX, SegY;
	int DestX, DestY;
    int m_iSpeed;
	DWORD m_dwMCurTime;

private:
    // �¼��������
    dwt::stringkey<char[STRING_KEY_LEN]> szAIFunc;

private:
    // �Ի����
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

    // Ч�����
    struct EFFECT {
        bool bPlay;
        LPStringSpr lpSpr;

        DWORD dwTime;

    };

    std::list<EFFECT> m_EffectList;

};

// ���ڴ����ű�����Ĳ���
struct SVisualObjectParam : 
    public SObjectCreateParameter
{
};
