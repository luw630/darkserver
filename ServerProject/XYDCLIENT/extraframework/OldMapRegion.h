#pragma once
#include "iregion.h"

#include "iobject.h"

#include "NewMapData.h"

class COldMapRegion :
    public IRegion
{
public:
    IRegion *GetInterface(const __int32 IID);           // ����IID��ȡ��Ӧ�ӿ�

private:
    // ****************************************
    // �ӿڶ���
    HRESULT Init(SCreateParameter *pParam);             // ��ʼ������
    HRESULT Exit(void);                                 // �ͷ���Դ

    void UpdateAI(void);                                // �ɵ����߿��ƽ����AI�������
    void UpdateShow();                      // �ɵ����߿��ƽ����ʵ�ָ�����ڣ�[x,y]�ǳ�������ʾ��ʼλ��

    HScriptObject CreateObject(const __int32 iCreateIID, LPCSTR szKey, SCreateParameter *pParam);         // ���һ���ű�����
    HScriptObject LocateObject(LPCSTR szKey);           // ͨ���ؼ��ֻ�ȡһ���ű�����
    HRESULT       DeleteObject(HScriptObject hObj);     // �ͷ�һ���ű����󣨿��ܻᵼ�¶���ɾ����

public:
    void GetShowPos(int &x, int &y);
    void SetShowPos(int x, int y);

    void MoveRegion(int x1, int y1, int x2, int y2);

public:
    HScriptObject GetObjectWithKey(LPCSTR szKey);

public:
    COldMapRegion(void);
    ~COldMapRegion(void);

private:
    std::map<dwt::stringkey<char[STRING_KEY_LEN]>, IObject *> m_ObjectMap;      //�����ϵĶ���ӳ���

    CNewMapData m_MapData; // ��Ϸ�ó�����ͼ����

    int m_xScreen, m_yScreen;   // ����ָ����ǰ��������ʾ��ʼλ��


};
