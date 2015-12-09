#pragma once

const __int32 IID_OBJECT_VISUAL = 0x13131af6; 

interface IObject
{
    IObject(const LPCSTR szKey) {}
    virtual ~IObject() {}
    virtual IObject *GetInterface(const __int32 IID) = 0;   // ����IID��ȡ��Ӧ�ӿ�

    // ****************************************
    // �ӿڶ���
    virtual HRESULT Init(SCreateParameter *pParam) = 0;     // ��ʼ������
    virtual HRESULT Exit(void) = 0;                         // �ͷ���Դ

    virtual void UpdateAI(void) = 0;                        // �ɵ����߿��ƽ����AI�������
    virtual void UpdateShow(int x, int y) = 0;              // �ɵ����߿��ƽ����ʵ�ָ�����ڣ�[x��y]�Ǹ��������ʾλ��

    virtual LPCSTR GetKey(void) = 0;                        // �����Լ��Ĳ�ѯ�ؼ���

    // ****************************************
    // ��������ʵ��
    static IObject *CreateInstance(const __int32 IID, const LPCSTR szKey, SCreateParameter *pParam);
};

struct SObjectCreateParameter :
    public SCreateParameter
{
    dwt::stringkey<char[STRING_KEY_LEN]> InitFunction;
};