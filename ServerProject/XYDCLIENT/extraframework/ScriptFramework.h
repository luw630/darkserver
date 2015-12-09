#pragma once

// �ű���ܵĻ����ṹ��
// �ű����桢����������

#include "iregion.h"

class CScriptFramework :
    public IFramework
{
public:
    IFramework *GetInterface(const __int32 IID);                // ����IID��ȡ��Ӧ�ӿ�

private:
    // ****************************************
    // �ӿڶ���
    HRESULT Init(SCreateParameter *pParam);                     // ��ʼ��
    HRESULT Loop(void);                                         // ִ��ѭ��
    HRESULT Exit(void);                                         // ����

    void UpdateAI(void);                                        // �ɵ����߿��ƽ����AI�������
    void UpdateShow(void);                                      // �ɵ����߿��ƽ����ʵ�ָ������
    void InputHandle(int iKeyState, int vKey, int x, int y);    // ���ز����������

public:
    HRESULT LoadEventFile(LPCSTR filename);                     // ʹ�ýű��ļ���ʼ���¼�

public:
    IRegion *GetCurRegion();

public:
    HRESULT CreateRegion(const __int32 iCreateIID, SCreateParameter *pParam);   // �����ű�����
    void ClearRegion();     // ��յ�ǰ�ű�����

public:
    CScriptFramework(void);
    ~CScriptFramework(void);

private:
    IRegion *m_pRegion;         // ��ǰ�õ��Ľű�����
};

