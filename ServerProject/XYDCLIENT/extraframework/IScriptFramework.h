#pragma once

// 脚本框架用的接口数据定义

#include "iframework.h"



#define STRING_KEY_LEN      40
#define FILENAME_LEN        200



const __int32 IID_FRAMEWORK_SCRIPT = 0x131315e0;        // 脚本框架类型ID
const __int32 IID_REGION_OLD_MAP_REGION = 0x131319b1;   // 可重用map文件的脚本场景类型ID

void StartEventWithScriptFilename(IFramework *pFramework, LPCSTR szFilename);
void ShowPicture(int  nID, int nPosX = 0, int nPosY = 0);


// 用于创建脚本框架的参数，主要是可用于创建脚本框架的同时初始化脚本场景
struct SScriptFrameworkParam : 
    public SCreateParameter
{
    __int32 iCreateIID;                 // 脚本场景的类型ID
    SCreateParameter *pCreateMapParam;  // 脚本场景的创建参数
};

// 用于创建可以重用现在场景文件类型的场景的参数
struct SOldMapRegionParam : 
    public SCreateParameter
{
    long lMapIndex;     // 现在游戏用地图的索引编号
    dwt::stringkey<char[FILENAME_LEN]> InitFunction;   // 场景初始化函数名
};
