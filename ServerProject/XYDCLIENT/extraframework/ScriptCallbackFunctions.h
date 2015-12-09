#pragma once

// �ű�����õĽӿں�������
#include "IscriptFramework.h"
#include "IFramework.h"
#include "IObject.h"
#include "IRegion.h"

// �����ű�����
IRegion *CreateOldMapRegion(IFramework *pFramework, long lMapIndex, LPCSTR szInitFunction); 

// ��ȡ��ǰ����
IRegion *GetCurRegion(IFramework *pFramework); 

// ���һ���ű�����
HScriptObject CreateVisualObject(IRegion *pRegion, LPCSTR szKey, LPCSTR szInitFunction); 

// ��ȡ��ǰ��������ʾλ��
void GetShowPos(IRegion *pRegion, int &x, int &y);

// ���õ�ǰ��������ʾλ��
void SetShowPos(IRegion *pRegion, int x, int y);

// ��ȡ��ǰ�������ʾλ��
void GetShowPos(IObject *pObject, int &x, int &y);

// ���õ�ǰ�������ʾλ��
void SetShowPos(IObject *pObject, int x, int y);

// ����һ������ʹ����ҵ���ʾ��Դ
void SetPlayerImage(IObject *pObject, WORD wActionType, int PlayerID, int WeaponID);
 
void SetSpaceNumber(IObject *pObject, WORD wNumber, WORD wFBegin, WORD wFEnd); 

void MoveTo(IObject *pObject, int nXPos, int nYPos, int nSpeed);

void SetEventFunc(IObject *pObject, LPCSTR szEventFunc);

void SetTalkInfo(IObject *pObject, WORD wType, LPCSTR szTalkInfo, DWORD dwTime);

void SetRelation(IObject *pSObject, IObject *pDObject);

HScriptObject GetObjectWithKey(LPCSTR szKey);

void AddEffect(IObject *pObject, LPCSTR szFileName, WORD wTime);

void MoveShowPos(int x, int y);


