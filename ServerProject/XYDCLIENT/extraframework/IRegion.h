#pragma once

DECLARE_HANDLE(HScriptObject);

interface IRegion
{
    virtual ~IRegion() {}
    virtual IRegion *GetInterface(const __int32 IID) = 0;           // ����IID��ȡ��Ӧ�ӿ�

    // ****************************************
    // �ӿڶ���
    virtual HRESULT Init(SCreateParameter *pParam) = 0;             // ��ʼ������
    virtual HRESULT Exit(void) = 0;                                 // �ͷ���Դ

    virtual void UpdateAI(void) = 0;                                // �ɵ����߿��ƽ����AI�������
    virtual void UpdateShow(void) = 0;                              // �ɵ����߿��ƽ����ʵ�ָ�����ڣ�[x,y]�ǳ�������ʾ��ʼλ��

    virtual HScriptObject CreateObject(const __int32 iCreateIID, LPCSTR szKey, SCreateParameter *pParam) = 0;   // ���һ���ű�����
    virtual HScriptObject LocateObject(LPCSTR szKey) = 0;           // ͨ���ؼ��ֻ�ȡһ���ű�����
    virtual HRESULT       DeleteObject(HScriptObject hObj) = 0;     // �ͷ�һ���ű����󣨿��ܻᵼ�¶���ɾ����

    // ****************************************
    // ��������ʵ��
    static IRegion *CreateInstance(const __int32 IID, SCreateParameter *pParam);
};
