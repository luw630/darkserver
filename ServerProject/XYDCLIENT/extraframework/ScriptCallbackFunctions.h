#pragma once

// 脚本框架用的接口函数定义
#include "IscriptFramework.h"
#include "IFramework.h"
#include "IObject.h"
#include "IRegion.h"

// 创建脚本场景
IRegion *CreateOldMapRegion(IFramework *pFramework, long lMapIndex, LPCSTR szInitFunction); 

// 获取当前场景
IRegion *GetCurRegion(IFramework *pFramework); 

// 添加一个脚本对象
HScriptObject CreateVisualObject(IRegion *pRegion, LPCSTR szKey, LPCSTR szInitFunction); 

// 获取当前场景的显示位置
void GetShowPos(IRegion *pRegion, int &x, int &y);

// 设置当前场景的显示位置
void SetShowPos(IRegion *pRegion, int x, int y);

// 获取当前对象的显示位置
void GetShowPos(IObject *pObject, int &x, int &y);

// 设置当前对象的显示位置
void SetShowPos(IObject *pObject, int x, int y);

// 设置一个对象使用玩家的显示资源
void SetPlayerImage(IObject *pObject, WORD wActionType, int PlayerID, int WeaponID);
 
void SetSpaceNumber(IObject *pObject, WORD wNumber, WORD wFBegin, WORD wFEnd); 

void MoveTo(IObject *pObject, int nXPos, int nYPos, int nSpeed);

void SetEventFunc(IObject *pObject, LPCSTR szEventFunc);

void SetTalkInfo(IObject *pObject, WORD wType, LPCSTR szTalkInfo, DWORD dwTime);

void SetRelation(IObject *pSObject, IObject *pDObject);

HScriptObject GetObjectWithKey(LPCSTR szKey);

void AddEffect(IObject *pObject, LPCSTR szFileName, WORD wTime);

void MoveShowPos(int x, int y);


