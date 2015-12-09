#pragma once

// �ű�����õĽӿ����ݶ���

#include "iframework.h"



#define STRING_KEY_LEN      40
#define FILENAME_LEN        200



const __int32 IID_FRAMEWORK_SCRIPT = 0x131315e0;        // �ű��������ID
const __int32 IID_REGION_OLD_MAP_REGION = 0x131319b1;   // ������map�ļ��Ľű���������ID

void StartEventWithScriptFilename(IFramework *pFramework, LPCSTR szFilename);
void ShowPicture(int  nID, int nPosX = 0, int nPosY = 0);


// ���ڴ����ű���ܵĲ�������Ҫ�ǿ����ڴ����ű���ܵ�ͬʱ��ʼ���ű�����
struct SScriptFrameworkParam : 
    public SCreateParameter
{
    __int32 iCreateIID;                 // �ű�����������ID
    SCreateParameter *pCreateMapParam;  // �ű������Ĵ�������
};

// ���ڴ��������������ڳ����ļ����͵ĳ����Ĳ���
struct SOldMapRegionParam : 
    public SCreateParameter
{
    long lMapIndex;     // ������Ϸ�õ�ͼ���������
    dwt::stringkey<char[FILENAME_LEN]> InitFunction;   // ������ʼ��������
};
