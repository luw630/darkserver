#ifndef _ASTAR_H_
#define _ASTAR_H_

#ifdef _MYDEBUG
#undef _MYDEBUG
#endif
#define MAPINFO mapinfo2

struct PathNod 
{
    BYTE dir : 3;
    BYTE len : 5;
};
class CRegion;

#define  MAX_SMOOTH_PATH 64
#include <assert.h>

#include <list>
#include <deque>
using namespace std;

typedef unsigned long ADWORD;

typedef unsigned short ASHORT;

typedef struct _Nod
{
    ASHORT x;
    ASHORT y;

}Nod;    

typedef struct _TAstarNode
{
    ADWORD Pos;			// �õ������(16,16)��ģʽ����(y,x)
    ASHORT ActualCost;	// �������㵽�ýڵ��ʵ�ʿ���
    ASHORT EstimateCost;	// ����˵�Ĺ��ۿ���
    _TAstarNode *Father;	// �˵�ĸ��ڵ�
    _TAstarNode *Next;	// ��Open����Next�����е���һ���ڵ�
    char Modified;		// �ýڵ��Ƿ��޸Ĺ�����¼�������1��,2 Open,4 Close
    short  Direct;      //���յ�ķ������EstimateCost��ͬ�Ļ���������Ƚ�
}TAstarNode;

class AstarPathFind
{
protected:

    class CRegion * m_pRegion;
    TAstarNode **m_Node;		// ��Ӧ��ͼ��ÿ���ڵ�
    TAstarNode *m_Open;		// ����û�д���İ�����ֵ����Ľڵ�
    TAstarNode *m_Close;		// ���洦����Ľڵ�
    // Luou added Apr 14th 2003 start
    short m_SmoothCount;
    // Luou added Apr 14th 2003 end
    TAstarNode **m_Modify;	// �����޸Ĺ��Ľڵ�
    TAstarNode *m_ShowOpen;

    int m_height;				// ��ͼ�ĸ߶�
    int m_width;				// ��ͼ�Ŀ��

    void** m_mapdata;

    BOOL   m_Stright;

    int    m_MakeCount;

    ADWORD m_MaxSize;			// ��������height*width
    ADWORD m_ModifyPoint;		// Modify�����ָ��
    ADWORD m_OpenCount;		// Open�����еĽڵ���Ŀ
    ASHORT m_CloseCount;		// Close��������Ľڵ���Ŀ
    ADWORD m_OpenLimited;		// Open�������ڵ���Ŀ
    ADWORD m_CloseLimited;	// Close�������ڵ���Ŀ
    ASHORT m_DirMask;			// Ҫ��������ı�־��0-7λΪ���Ͽ�ʼ˳ʱ���߸�����
    ADWORD m_MinPos;			// �յ����ӽ��յ�Ľڵ�
    ASHORT m_DirToEnd;          // ���յ�ķ���
    ASHORT m_xend;              //
    ASHORT m_yend;              //
    ASHORT m_xstart;
    ASHORT m_ystart;

    ASHORT m_xbest;
    ASHORT m_ybest;
    ASHORT m_BestCost;

    ASHORT GetDistance(ASHORT xstart,ASHORT ystart,ASHORT xend,ASHORT yend);

    char (*NoBlock)(ASHORT xstart,ASHORT ystart,ASHORT xend,ASHORT yend);
    // ����ͼ�Ƿ�����ƶ�����ָ��
    char (*MoveAble)(ASHORT x,ASHORT y,CRegion * pSetRegion);	
    // ȡ�ù���ֵ�ĺ���ָ��
    ASHORT (*JudgeCost)(ASHORT x,ASHORT y,ASHORT xend,ASHORT yend);	
    // ���ڵ��������Open����
    char AddToOpenQueue(TAstarNode *node);	
    // ��Open����ȡ����С�Ĳ�����Close����
    char GetFromOpenQueue();				
    // ȡ��Close�����еĽڵ�
    char PopCloseStack();	
    //�����ֵ�����Ǹ�ֵ���ں��������ж�				
    char TryTile(short x,short y,TAstarNode *Father);
#ifdef _MYDEBUG
    list<Nod> m_FullOpenList;
#endif


public:
    //Luou Added Apr 15th 2003 start
    PathNod PathInfo[64];      //ƽ����Ľڵ���Ϣ
    //Luou Added Apr 15th 2003 end
    // �������������¼
    void ClearModified();					
    int Create(int mapw,int maph,/*char (*BlockFun)(ASHORT,ASHORT,ASHORT,ASHORT),*/char (*MoveCheck)(ASHORT x,ASHORT y,CRegion *),ASHORT (*JudgeFun)(ASHORT x,ASHORT y,ASHORT xend,ASHORT yend),CRegion * pSetRegion);
    int Release();
    int FindPath(ASHORT startx,ASHORT starty,ASHORT endx,ASHORT endy);
    int GetPosPath(ASHORT *PosXY,int maxpos);
    int GetDirPath(char *Dirs,int maxpos);
    void SmoothPath(void);
    void MakePath(void);

    int GetClose();

    ASHORT GetCloseCount()
    {
        return m_CloseCount;
    }

    //ȡ����������Է���
    short GetDirection(ASHORT xstart,ASHORT ystart,ASHORT xend1,ASHORT yend1);

    short GetProperEnd(ASHORT xstart,ASHORT ystart,ASHORT xend1,ASHORT yend1);//����յ����赲��ȡ�����յ���������赲�ĵ�

    void MoveToDir(ASHORT& xstart ,ASHORT& ystart ,short dir);

    int GetPathInfo(Nod*Path,int iMaxCount);
    int GetMakePath( BYTE*&pPathInfo);

    int GetOpenNode(Nod *Path,int iMaxCount);
    int GetOpenList(Nod *Path,int iMaxCount);

    TAstarNode * FindNearstNode();


#ifdef _MYDEBUG
    void ClearFullOpen();
#endif

    void SetRegion(CRegion* pSetRegion);
    void SetJudgeFun(ASHORT (*JudgeFun)(ASHORT,ASHORT,ASHORT,ASHORT));
    void SetMapCheck(char (*MapCheck)(ASHORT,ASHORT,CRegion *));
    void SetOpenLimited(long OpenLimited);
    void SetCloseLimited(long CloseLimited);
    void SetDirMask(long DirMask);
};

#endif
