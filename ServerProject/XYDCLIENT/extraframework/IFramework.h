#pragma once
#include <UNKNWN.H>

struct SCreateParameter
{
    DWORD cbSize;
};

interface IFramework
{
    virtual ~IFramework() {}
    virtual IFramework *GetInterface(const __int32 IID) = 0;                // ����IID��ȡ��Ӧ�ӿ�

    // ****************************************
    // �ӿڶ���
    virtual HRESULT Init(SCreateParameter *pParam) = 0;                     // ��ʼ��
    virtual HRESULT Loop(void) = 0;                                         // ִ��ѭ��
    virtual HRESULT Exit(void) = 0;                                         // ����

    virtual void UpdateAI(void) = 0;                                        // �ɵ����߿��ƽ����AI�������
    virtual void UpdateShow(void) = 0;                                      // �ɵ����߿��ƽ����ʵ�ָ������
    virtual void InputHandle(int iKeyState, int vKey, int x, int y) = 0;    // ���ز����������

    // ****************************************
    // ��������ʵ��
    static IFramework *CreateInstance(const __int32 IID, SCreateParameter *pParam);
};
