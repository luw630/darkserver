#pragma once
#include "mainworld\mapdata.h"

#include "iobject.h"

class CTileSpr : 
    public IObject,
    public STimeFrame
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
    CTileSpr(void);
    ~CTileSpr(void);

public:
    WORD wSprIndex;
    DWORD dwAudioFlag;

    int m_x;
    int m_y;
};

class CNewMapData :
    public CMapData
{
public:
	int LoadMap(long MapIndex);

public:
    CNewMapData(void);
    ~CNewMapData(void);

public:
    CTileSpr *pTileSprs;
};
