#ifndef _ASTAR_H_
#define _ASTAR_H_


#ifdef _MYDEBUG
#undef _MYDEBUG
#endif


#define MAPINFO mapinfo
struct PathNod 
{
    BYTE dir : 3;
    BYTE len : 5;
};

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
    ADWORD Pos;			// 该点的坐标(16,16)的模式保存(y,x)
    ASHORT ActualCost;	// 保存从起点到该节点的实际开销
    ASHORT EstimateCost;	// 保存此点的估价开销
    _TAstarNode *Father;	// 此点的父节点
    _TAstarNode *Next;	// 在Open或者Next链表中的下一个节点
    char Modified;		// 该节点是否被修改过，记录而备清除1空,2 Open,4 Close
    //Luou Added 
    short  Direct;      //到终点的方向，如果EstimateCost相同的话就用这个比较

}TAstarNode;

class AstarPathFind
{
protected:

    TAstarNode **m_Node;		// 对应地图中每个节点
    TAstarNode *m_Open;		// 保存没有处理的按估计值排序的节点
    TAstarNode *m_Close;		// 保存处理过的节点
    // Luou added Apr 14th 2003 start
    short m_SmoothCount;
    // Luou added Apr 14th 2003 end
    TAstarNode **m_Modify;	// 保存修改过的节点
    TAstarNode *m_ShowOpen;
    int m_height;				// 地图的高度
    int m_width;				// 地图的宽度
    void** m_mapdata;
    BOOL   m_Stright;

    int    m_MakeCount;

    ADWORD m_MaxSize;			// 最大面积即height*width
    ADWORD m_ModifyPoint;		// Modify数组的指针
    ADWORD m_OpenCount;		// Open队列中的节点数目
    ASHORT m_CloseCount;		// Close队列里面的节点数目
    ADWORD m_OpenLimited;		// Open队列最多节点数目
    ADWORD m_CloseLimited;	// Close队列最多节点数目
    ASHORT m_DirMask;			// 要搜索方向的标志，0-7位为从上开始顺时针七个方向
    ADWORD m_MinPos;			// 终点或最接近终点的节点
    ASHORT m_DirToEnd;          // 到终点的方向
    ASHORT m_xend;              //
    ASHORT m_yend;              //
    ASHORT m_xstart;
    ASHORT m_ystart;

    ASHORT m_xbest;
    ASHORT m_ybest;
    ASHORT m_BestCost;

    ASHORT GetDistance(ASHORT xstart,ASHORT ystart,ASHORT xend,ASHORT yend);

    char (*NoBlock)(ASHORT xstart,ASHORT ystart,ASHORT xend,ASHORT yend);
    // 检查地图是否可以移动函数指针
    char (*MoveAble)(ASHORT x,ASHORT y);	
    // 取得估计值的函数指针
    ASHORT (*JudgeCost)(ASHORT x,ASHORT y,ASHORT xend,ASHORT yend);	
    // 将节点排序加入Open队列
    char AddToOpenQueue(TAstarNode *node);	
    // 从Open队列取出最小的并放入Close队列
    char GetFromOpenQueue();				
    // 取出Close队列中的节点
    char PopCloseStack();	
    //传入的值可能是负值，在函数体内判断				
    char TryTile(short x,short y,TAstarNode *Father);
#ifdef _MYDEBUG
    list<Nod> m_FullOpenList;
#endif


public:
    ~AstarPathFind() { Release(); }
    //Luou Added Apr 15th 2003 start
    PathNod PathInfo[64];      //平滑后的节点信息
    //Luou Added Apr 15th 2003 end

//	list<PathNod> PathInfoEx;	//平滑后的节点信息(地图，非区域)

    // 清除所有搜索记录
    void ClearModified();					
    int Create(int mapw,int maph,char (*MoveCheck)(ASHORT x,ASHORT y),ASHORT (*JudgeFun)(ASHORT x,ASHORT y,ASHORT xend,ASHORT yend));
    int Release();
    int FindPath(ASHORT startx,ASHORT starty,ASHORT endx,ASHORT endy);
    int GetPosPath(ASHORT *PosXY,int maxpos);
    int GetDirPath(char *Dirs,int maxpos);
    void MakePath(void);


    int GetClose();
    ASHORT GetCloseCount()
    {
        return m_CloseCount;
    };

    //取得两点间的相对方向
    short GetDirection(ASHORT xstart,ASHORT ystart,ASHORT xend1,ASHORT yend1);

    short GetProperEnd(ASHORT xstart,ASHORT ystart,ASHORT xend1,ASHORT yend1);//如果终点有阻挡，取得离终点最近的无阻挡的点

    void MoveToDir(ASHORT& xstart ,ASHORT& ystart ,short dir);

    int GetPathInfo(Nod*Path,int iMaxCount);
	int	GetPathInfoCount(void);													//取得路径数组的总数
    int GetMakePath( BYTE*&pPathInfo);

	int GetMakePathEx(list<PathNod> *pPath);

    int GetOpenNode(Nod *Path,int iMaxCount);
    int GetOpenList(Nod *Path,int iMaxCount);

    TAstarNode * FindNearstNode();

#ifdef _MYDEBUG
    void ClearFullOpen();
#endif

    void SetJudgeFun(ASHORT (*JudgeFun)(ASHORT,ASHORT,ASHORT,ASHORT));
    void SetMapCheck(char (*MapCheck)(ASHORT,ASHORT));
    void SetOpenLimited(long OpenLimited);
    void SetCloseLimited(long CloseLimited);
    void SetDirMask(long DirMask);

	int GetSamplingPath( Nod *pPath, int iSmpleNum, int iMaxCount );
};

#endif
