
//吕鸥 2003 4月15日修改
//梦工厂软件
//最后一次修改 2003 4月25日下午17:35
//说明：
//这是一个基于tile地图的路径搜寻的算法

//使用的时候需要一组外部变量
//ASHORT  xend;
//ASHORT  yend;
//来指定搜寻结束的终点。
//调用Create的时候需要指定两个外部的function，第一个是判断某个tile无阻挡的，另外一个是Judge函数
//地图的相关资料可以在判断阻挡的函数里面调用
//可以调用SetCloseLimited来指定搜索的最大深度。
//调用SetDirMask(0xFF)可以指定搜索的方向。
//调用FindPath开始寻路
//调用MakePath生成server端需要的路径,然后访问PathInfo可以得到需要的路径
//调用GetPathInfo可以取得ｘ，ｙ格式的坐标
//完成一次寻路后调用ClearModified清除寻路的记录
//地图坐标：
// o----------->x
// |
// |
// | 
// |
//\|/
// y

#include "stdafx.h"
#include "pathserver.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#define COST_MAX 30000
#define tile_pos(x,y) (((ADWORD)y<<16)|x)
#define tile_x(pos)   (ASHORT)(pos&0xffff)
#define tile_y(pos)   (ASHORT)(pos>>16)







// 待处理节点入队列, 依靠对目的地估价距离插入排序
char AstarPathFind::AddToOpenQueue(TAstarNode *node)
{
    ADWORD i;
    ASHORT f=node->EstimateCost;
    TAstarNode *p,*b;
    node->Modified|=2;	// 记录Open标志
    for (b=NULL,p=m_Open,i=0;p && i<=m_OpenCount;b=p,p=p->Next,i++) 
        if (f<=p->EstimateCost) 
            break;
    if (i>m_OpenCount) 
        return -1;
    //if( m_ > m_OpenLimited )
    //return -2;

#ifdef SHOW_PATH
    Nod  innod;
    ZeroMemory(&innod,sizeof(Nod));
    innod.x = tile_x(node->Pos);
    innod.y = tile_y(node->Pos);
    //assert(innod.x >= 0);
    //assert(innod.y >= 0);
    m_FullOpenList.push_back(innod);
#endif

    //if(p && m_Stright )
    //{
    //    if((abs(p->Direct-m_DirToEnd) < abs(node->Direct-m_DirToEnd)) /*&& p->Direct%2==0 */)
    //    {
    //        p->Next = node;
    //        if(b)
    //        {
    //            b->Next = p;
    //        }
    //        else
    //            m_Open = p;
    //    }
    //    else
    //    {
    //        node->Next = p;
    //        if (b) 
    //            b->Next=node;
    //        else 
    //            m_Open=node;
    //    }
    //}
    //else
    //Add To the Open Query

    node->Next=p;
    if (b)
    {
        b->Next=node;
    }
    else 
    {
        m_Open = node;
    }

    m_OpenCount++;
    return 0;
}
// 将离目的地估计最近的方案出队列
char AstarPathFind::GetFromOpenQueue()
{
    TAstarNode *BestChoice=m_Open;
    if (!m_Open) 
    {
        //m_LackMem = TRUE;
        //取不到Open队列
        return -1;
    }
    m_Open=m_Open->Next;
    if (BestChoice->Modified&4) 
        return -2;               //已经在m_Close中了
    BestChoice->Next=m_Close;
    BestChoice->Modified&=5;	 // 清除m_Open标志
    BestChoice->Modified|=4;	 // 设置m_Close标志
    m_Close=BestChoice;
    m_OpenCount--;
    m_CloseCount++;
    return 0;
}
// 释放栈顶节点
char AstarPathFind::PopCloseStack()
{
    if (m_Close) 
    { 
        m_Close->Modified&=3;		// 清除m_Close标志
        m_Close=m_Close->Next;
        m_CloseCount--; 
        return 0; 
    }
    return -1;
}
// 还原修改过的所有节点
void AstarPathFind::ClearModified()
{
    ADWORD i;
    for (i=0;i<m_ModifyPoint;i++) 
    {
        m_Modify[i]->Modified=0;
        m_Modify[i]->ActualCost=COST_MAX;
    }
    m_xend = -1;
    m_yend = -1;
    m_ModifyPoint=0;
    m_OpenCount=0;
    m_CloseCount=0;
    m_Open=NULL;
    m_Close=NULL;
    ZeroMemory(PathInfo,sizeof(PathNod)*MAX_SMOOTH_PATH);

}
// 尝试下一步移动到 x,y 可行否
char AstarPathFind::TryTile(short x,short y,TAstarNode *Father)
{
    int ActualCost;
    if (x<0||y<0||x>=m_width||y>=m_height) 
        return 1;
	if (!MoveAble(x,y,m_pRegion)) 
        return 1;			// 如果地图无法通过则退出
    if (m_Node[y][x].Modified & 6)
        return 1;	// 如果已经存在在m_Open,m_Close队列中则退出
    ActualCost=Father->ActualCost+1;		// 实际值计算
    if (ActualCost>=m_Node[y][x].ActualCost) 
        return 1;
    if (!m_Node[y][x].Modified) 
        m_Modify[m_ModifyPoint++]=&m_Node[y][x]; // 记录这个修改过的点以还原
    if(m_ModifyPoint > m_MaxSize)
        return 1;
    m_Node[y][x].ActualCost=ActualCost;
    m_Node[y][x].Modified=1;
    m_Node[y][x].Father=Father;
    m_Node[y][x].EstimateCost=ActualCost+JudgeCost(x,y,tile_x(Father->Pos),tile_y(Father->Pos));
    m_Node[y][x].Direct = GetDirection( x,y,tile_x(Father->Pos),tile_y(Father->Pos));

    AddToOpenQueue(&m_Node[y][x]);        // 将节点加入m_Open队列
    return 0;
}

void  AstarPathFind::MoveToDir(ASHORT& xstart ,ASHORT& ystart ,short dir)
{
    switch(dir)
    {
    case  0:
        xstart = xstart ;
        ystart = ystart - 1;
        break;
    case  1:
        xstart = xstart + 1;
        ystart = ystart - 1;
        break;
    case  2:
        xstart = xstart + 1;
        ystart = ystart ;
        break;
    case  3:
        xstart = xstart + 1;
        ystart = ystart + 1;
        break;
    case  4:
        xstart = xstart;
        ystart = ystart + 1;
        break;
    case  5:
        xstart = xstart - 1;
        ystart = ystart +1;
        break;
    case  6:
        xstart = xstart - 1;
        ystart = ystart ;
        break;
    case  7:
        xstart = xstart - 1;
        ystart = ystart - 1;
        break;
    }
    if (xstart == 0xffff ) xstart = 0;
    if (ystart == 0xffff ) ystart = 0;

}

short AstarPathFind::GetProperEnd(ASHORT xstart,ASHORT ystart,ASHORT xend1,ASHORT yend1)
{
    short xbegin = xend1;
    short ybegin = yend1;
    short dir;
    ASHORT Ret = 0;
    //ASHORT step;
    //ASHORT mat[MAX_SMOOTH_PATH];
    //ASHORT Looper;
    //八方向矩阵
    static short incx[8]={  0,  1, 1, 1, 0, -1, -1, -1 };
    static short incy[8]={ -1, -1, 0, 1, 1,  1,  0, -1 };
    //short xloop;
    //short yloop;
    dir = GetDirection(xstart,ystart,xend1,yend1);

    while( 1 != MoveAble(xend1,yend1,m_pRegion))
    {
        if(Ret > 10)
        {
            Ret = 0;
            break;
        }
        MoveToDir(xend1,yend1,dir);
        if ( xend1 <= 0 || yend1 <= 0 || xend1 >= m_width-1 || yend1 >= m_height-1 )
        {
            dir = dir + 1;
            if(dir == 8) dir = 0;
            xend1 = xbegin;
            yend1 = ybegin;
        }
        Ret ++ ;
    }
    m_xbest = xend1;
    m_ybest = yend1;
    return Ret;
}
#ifdef _MYDEBUG
void AstarPathFind::ClearFullOpen()
{
    int iSize;
    deque<Nod*>::iterator iter;
    //Nod  pTemp;
    iSize = m_FullOpenList.size();
    //for (int iLooper = 0; iLooper < iSize ;iLooper ++)
    //{
    //     pTemp = (*m_FullOpenList[iLooper]);
    //}
    if (!m_FullOpenList.empty())
        m_FullOpenList.clear();
    //if(m_FullOpenList.size() > 0 )
    //{
    //    m_FullOpenList.
    //    for (iter = m_FullOpenList.begin();iter != m_FullOpenList.end();iter++)
    //    {
    //        if( NULL != (*iter))
    //        {
    //            delete (*iter);
    //            (*iter) = NULL;
    //        }

    //    }        
    //    while(!m_FullOpenList.empty())
    //        m_FullOpenList.pop_front();
    //    //iSize = m_FullOpenList.size();
    //}
}
#endif


ASHORT AstarPathFind::GetDistance(ASHORT xstart,ASHORT ystart,ASHORT xend,ASHORT yend)
{
    ASHORT ret;
    ret = (ASHORT)sqrtf((float)((xstart-xend)*(xstart-xend) + (ystart-yend)*(ystart-yend)));
    return ret;

}


// 路径寻找主函数
int AstarPathFind::FindPath(ASHORT startx,ASHORT starty,ASHORT endx,ASHORT endy)
{
    TAstarNode *root;
    int i,j,x,y,ok,dirs=0;
    ASHORT max = 0;
    ASHORT MinJudge;
    //八方向矩阵
    static short incx[8]={  0,  1, 1, 1, 0, -1, -1, -1 };
    static short incy[8]={ -1, -1, 0, 1, 1,  1,  0, -1 };

    //如果起点不可走
//    if((!MoveAble(startx,starty,m_pRegion))/*&&(!MoveAble(endx,endy))*/)
//        return -1;
    //如果终点不可走
    if((!MoveAble(endx,endy,m_pRegion))/*&&(!MoveAble(endx,endy))*/)
        return -1;

    m_xstart = startx;
    m_ystart = starty;

    m_xend = endx;
    m_yend = endy;

    //if ( 0 < GetProperEnd(startx ,starty,endx,endy))
    //{
    //    m_xend = m_xbest;
    //    m_yend = m_ybest;
    //    endx = m_xend;
    //    endy = m_yend;
    //}
    if (!m_Node||!m_Modify) 
        return -1;
    ClearModified();
    root=&m_Node[starty][startx];
    root->ActualCost=0;
    m_BestCost = JudgeCost(startx,starty,endx,endy);
    root->EstimateCost=m_BestCost;
    root->Father=NULL; 
    root->Modified=1;
    //是否作直线路径的优化
    //if( (startx == endx || starty == endy) && !((startx == endx)&&(starty == endy)) /*&& (1 == NoBlock(startx,starty,endx,endy))*/)
    //{
    //    m_Stright = FALSE;
    //}
    //else
    //{
    //    m_Stright = FALSE;
    //}

    root->Direct = GetDirection(endx,endy,startx,starty);
    m_DirToEnd = root->Direct;
    m_Modify[m_ModifyPoint++]=root;
    AddToOpenQueue(root);
    m_MinPos=tile_pos(startx,starty);
    //最小的Judge值
    MinJudge=JudgeCost(startx,starty,endx,endy);

    // 对一个点操作
    for (ok=0;ok==0;)
    {
        ok=GetFromOpenQueue();		// 取出Open队列估价值最小的节点放入m_Close中
        // Open队列已经按估价值的大小排序
        root=m_Close;					// 得到刚才取出的节点
        if (ok||root==NULL) 
        { 
            ok=-1; 
            break; 
        }
        x=tile_x(root->Pos);
        y=tile_y(root->Pos);
        if ( root->EstimateCost - root->ActualCost < MinJudge )	// 找到一个估计离终点最近的点
        {
            MinJudge = root->EstimateCost - root->ActualCost;
            m_MinPos = root->Pos;
        }
        if (m_CloseCount > max) 
        {
            max = m_CloseCount;
        }
        if (x == endx && y == endy) 
        {
            m_MinPos=root->Pos,ok=1;	// 如果走到终点了
        }
        else 
        {
            for (i=0,dirs=1,j=1;i<8;i++,dirs<<=1)		// 如果没有走到终点
                if (m_DirMask & dirs) 
                {
                    char ret;
                    ret = TryTile(x+incx[i],y+incy[i],root);
                    //if ( ret == -2 )
                    //    //goto _outofmem;
                    //else
                    j &= ret;
                }
                if (j) //如果j无法行走，则将其从close队列中pop出来
                    if (PopCloseStack()) 
                    { 
                        ok=-2; 
                        break; 
                    }	// 如果不是通路则从m_Close中取出
        }
        if (m_OpenCount>=m_OpenLimited||m_CloseCount>=m_CloseLimited) // 如果超过最大的路径数目
            ok=2;
    }
    if (ok<0) 
    {
        //m_xend = endx;
        //m_yend = endy;

        //pBestNode = FindNearstNode();
        //m_Close = pBestNode;
        //return 0 ;
        return ok;
    }


    //_outofmem:
    //pBestNode = FindNearstNode();
    //pBestNode->Father = m_Close;
    //m_Close = pBestNode;

    return 0;
}
TAstarNode * AstarPathFind::FindNearstNode() 
{
    TAstarNode *pBest = NULL;


    //ASHORT BestCost;
    ASHORT Dif;
    ASHORT DifX;
    ASHORT DifY;
    ASHORT DifLast;
    //BestCost = GetDistance(m_xstart,m_ystart,m_xend,m_yend);
    pBest = m_Modify[0];
    DifLast = abs(m_xend-tile_x(pBest->Pos)) + abs(m_yend-tile_y(pBest->Pos));

    for (unsigned i = 1; i < m_ModifyPoint; i++) 
    {
        TAstarNode *pNode = m_Modify[i];

        DifX = abs(m_xend-tile_x(pNode->Pos));
        DifY = abs(m_yend-tile_y(pNode->Pos));
        Dif = DifX  + DifY;


        if (pBest == NULL || Dif <= DifLast)
        {
            if (Dif == DifLast /*||((DifX == 1)&&(DifY == 1))*/ )
            {
                if(pNode->ActualCost < pBest->ActualCost)
                {
                    pBest = pNode;
                    DifLast = Dif;
                }
            }
            else
            {
                pBest = pNode;
                DifLast = Dif;
            }
        }
    }

    return pBest;
}

void AstarPathFind::SmoothPath(void)
{


}

void AstarPathFind::MakePath(void)
{
    ASHORT x;
    ASHORT y;
    ASHORT xlast;
    ASHORT ylast;
    short lastdir;
    short dirnow;
    TAstarNode * pTempClose;
    pTempClose = m_Close;
    ZeroMemory(PathInfo,sizeof(PathNod)*MAX_SMOOTH_PATH);
    if(pTempClose == NULL )
        return ;
    xlast = tile_x(pTempClose->Pos);            //取得上一个走过的点
    ylast = tile_y(pTempClose->Pos);      

    pTempClose = pTempClose->Father;            //取下一个点的资料
    if( pTempClose == NULL )
        return; 
    lastdir = GetDirection(xlast,ylast,tile_x(pTempClose->Pos),tile_y(pTempClose->Pos));//取得两点间的方向差距

    int   iPathCount = 0;
    int   iMoveTile = 0;
    //BYTE  xbyte,ybyte;
    while(pTempClose!=NULL)
    {
        if(iPathCount>=15)                      //判断是否超过最大路径
            break;
        x = tile_x(pTempClose->Pos);            //取得这次的点坐标
        y = tile_y(pTempClose->Pos);    
        dirnow = GetDirection(xlast,ylast,x,y); //判断两个点间的相对方向
        if (lastdir != dirnow /*|| pTempClose->Father == NULL*/ )
        {
            PathInfo[iPathCount].dir = lastdir;
            PathInfo[iPathCount].len = iMoveTile;
            iMoveTile = 1;
            iPathCount++;
        }
        else
        {
            iMoveTile++ ;                       //路径的个数累加
        }
        pTempClose = pTempClose->Father;
        if(pTempClose == NULL)
        {
            if(lastdir != dirnow)
            {
                PathInfo[iPathCount].dir = dirnow;
                PathInfo[iPathCount].len = iMoveTile;
            }
            else
            {
                PathInfo[iPathCount].dir = lastdir;
                PathInfo[iPathCount].len = iMoveTile;
            }
            iPathCount ++;
        }
        xlast = x;                              //移到下一个点
        ylast = y;
        lastdir = dirnow;
    }
    PathNod PathInfoTemp[MAX_SMOOTH_PATH];
    for(int iLooper = 0 ;iLooper< iPathCount;iLooper++)
    {
        PathInfoTemp[iLooper] = PathInfo[iPathCount-1-iLooper];
    }
    memcpy(PathInfo,PathInfoTemp,sizeof(PathNod)*(iPathCount));
    m_SmoothCount = iPathCount;
}


short AstarPathFind::GetDirection(ASHORT xstart,ASHORT ystart,ASHORT xend1,ASHORT yend1)
{
    short tempx;
    short tempy;

    tempx = xstart - xend1;
    tempy = ystart - yend1;

    if ((tempx == 0)&&(tempy == 0))
        return -1;

    if( tempx == 0 ) 
    {
        if ( tempy >= 1 )
            return 4;
        else if ( tempy <= -1)
            return 0;
    }
    else if( tempx >= 1 )
    {
        if ( tempy >= 1 )
            return 3;
        else if (tempy == 0 )
            return 2;
        else 
            return 1;
    }
    else if( tempx <= -1 )
    {
        if ( tempy >= 1 )
            return 5;
        else if ( tempy == 0 )
            return 6;
        else 
            return 7;
    }
    return -2;
}


int AstarPathFind::GetPosPath(ASHORT *PosXY,int maxpos)
{
    ASHORT x,y;
    ADWORD j;
    TAstarNode *p,*s;
    int i;
    if (maxpos>1) 
        maxpos--;
    for (p=&m_Node[tile_y(m_MinPos)][tile_x(m_MinPos)],s=p,j=0;p&&j<m_MaxSize;p=p->Father,j++)
    {
        x=tile_x(p->Pos);
        y=tile_y(p->Pos);
        i=(p->ActualCost<maxpos)?p->ActualCost:maxpos;
        PosXY[i<<1]=x;
        PosXY[(i<<1)+1]=y;
    }
    i=(s->ActualCost+1<maxpos)?(s->ActualCost+1):maxpos;
    PosXY[i*2]=-1;
    PosXY[i*2+1]=-1;
    return 0;
}
int AstarPathFind::GetDirPath(char *PosDir,int maxpos)
{
    static short incx[8]={ 0, 1, 1, 1, 0, -1, -1, -1 };
    static short incy[8]={ -1, -1, 0, 1, 1, 1, 0, -1 };
    static char inc2r[10]={ 0,0,0,0 };
    ASHORT i,x,y,nx,ny;
    ADWORD j;
    TAstarNode *p,*s;
    if (!inc2r[0])
    {
        for (i=0;i<8;i++) inc2r[(incy[i]+1)*3+incx[i]+1]=(char)i;
        inc2r[4]=8;
    }
    x=tile_x(m_MinPos);
    y=tile_y(m_MinPos);
    if (maxpos>1) maxpos--;
    for (p=&m_Node[y][x],s=p,j=0;p&&j<m_MaxSize;p=p->Father,j++)
    {
        nx=tile_x(p->Pos);
        ny=tile_y(p->Pos);
        i=(p->ActualCost<maxpos)?(p->ActualCost):maxpos;
        PosDir[i]=inc2r[(y-ny+1)*3+x-nx+1];
        x=nx;
        y=ny;
    }
    i=(s->ActualCost+1<maxpos)?(s->ActualCost+1):maxpos;
    PosDir[i]=8;
    return 0;
}

int AstarPathFind::Create(int mapw,int maph,/*char (*BlockFun)(ASHORT,ASHORT,ASHORT,ASHORT),*/char (*MoveCheck)(ASHORT x,ASHORT y,CRegion * pSetRegion),ASHORT (*JudgeFun)(ASHORT x,ASHORT y,ASHORT xend,ASHORT yend),CRegion * pSetRegion)
{
    int i,j;



    m_SmoothCount = 0;
    m_height=maph;	m_width=mapw;
    m_MaxSize=maph;	m_MaxSize*=mapw;
    //新建是否更改的标识
    m_Modify=new TAstarNode*[m_MaxSize];
    if (!m_Modify) 
        return -1;
    // 新建地图资料的数据
    m_Node=new TAstarNode*[maph];
    if (!m_Node) 
    { 
        delete m_Modify; 
        m_Modify=NULL; 
        return -1; 
    }

    for (i=0;i<maph;i++) 
        m_Node[i]=new TAstarNode[mapw];
    for (i=0,j=1;i<maph;i++) 
        if (!m_Node[i]) 
            j=0;


    if (!j)
    {
        for (i=0;i<maph;i++) 
            if (m_Node[i]) 
                delete m_Node[i];
        delete m_Node;
        delete m_Modify;
        m_Node=NULL;
        m_Modify=NULL;
        return -2;
    }
    //初始化搜索资料的信息
    for (j=0;j<maph;j++) 
        for (i=0;i<mapw;i++) 
        {
            m_Node[j][i].Pos=tile_pos(i,j);
            m_Node[j][i].Modified=0;
            m_Node[j][i].ActualCost=COST_MAX;
        }
        //m_LackMem = FALSE;
        m_ModifyPoint=0;
        //设定检查阻挡函数
        SetMapCheck(MoveCheck);

        SetRegion(pSetRegion);
        //设定估价函数
        SetJudgeFun(JudgeFun);
        //NoBlock = BlockFun;
        //设定搜寻方向
        SetDirMask(0xFF);
        //设置open表的最大值
        SetOpenLimited(400);
        //SetOpenLimited(50);
        //设置close表的最大值
        SetCloseLimited(400);
        //SetCloseLimited(20);
#ifdef _MYDEBUG
        m_FullOpenList.clear();
#endif
        return 0;
}

int AstarPathFind::Release()
{
    int i,j;
    i = 0;
    j = 0;
    if (m_Node) 
    {
        //while ((m_Node[i]->Father != NULL ) &&　(m_Node[i]->Next != NULL))
        //{
        //    delete m_Node[i]->Father;
        //    delete m_Node[i]->Next;
        //    i++:
        //}
        for (j= 0; j<m_height;j++) 
            if (m_Node[j] /*&& m_Node[j]->Father && m_Node[j]->Next*/) 
            {
                delete m_Node[j];
            }
    }

    if (m_Node) 
        delete m_Node;
    if (m_Modify) 
        delete m_Modify;
    m_Node=NULL;
    m_Modify=NULL;
    return 0;
}

//TAstarNode * AstarPathFind::GetClose(void)
//{
//    return m_Close;
//}

#ifdef   _MYDEBUG
int  AstarPathFind::GetOpenNode(Nod *Path,int iMaxCount)
{
    short count = 0;
    Nod  OpenNod;
    list<Nod>::iterator iter;
    //for(int iLooper = 0 ; iLooper < count ;iLooper ++)
    //{
    for(iter = m_FullOpenList.begin();iter != m_FullOpenList.end();iter++)
    {
        OpenNod = (*iter);
        Path[count].x = OpenNod.x;
        Path[count].y = OpenNod.y;
        //count ++;
        //if(count >= iMaxCount -1)
        //    break;
        count ++;
        if(count >= iMaxCount -1)
            break;
    }
    return count;
}

int AstarPathFind::GetOpenList(Nod *Path,int iMaxCount)
{
    short count = 0;
    ASHORT x;
    ASHORT y;
    TAstarNode* tempOpen;
    tempOpen = m_Open;
    while(tempOpen!=NULL)
    {
        if(count >= iMaxCount)
            break;
        x = tile_x(tempOpen->Pos);
        y = tile_y(tempOpen->Pos);
        Path[count].x = x;
        Path[count].y = y;
        tempOpen = tempOpen->Next;
        count ++ ;
    }
    return count;
}


#endif
int  AstarPathFind::GetMakePath(BYTE *& pPathInfo)
{
    pPathInfo = (BYTE*)PathInfo;
    return m_SmoothCount;
}
int  AstarPathFind::GetPathInfo(Nod *Path,int iMaxCount)
{
    short count = 0;
    TAstarNode * tempClose;
    tempClose = m_Close;
    while( tempClose != NULL)
    {
        if(count >= iMaxCount)
            break;
        Path[count].x = tile_x(tempClose->Pos);
        Path[count].y = tile_y(tempClose->Pos);
        tempClose = tempClose->Father;
        count ++;
    }
    return count;
}

class CRegion;
void AstarPathFind::SetRegion(CRegion* pSetRegion){ m_pRegion = pSetRegion;}
void AstarPathFind::SetJudgeFun(ASHORT (*JudgeFun)(ASHORT,ASHORT,ASHORT,ASHORT)) { JudgeCost=JudgeFun; }
void AstarPathFind::SetMapCheck(char (*MapCheck)(ASHORT,ASHORT,CRegion *)) { MoveAble=MapCheck; }
void AstarPathFind::SetOpenLimited(long OL) { m_OpenLimited=OL; }
void AstarPathFind::SetCloseLimited(long CL) { m_CloseLimited=CL; }
void AstarPathFind::SetDirMask(long DM) { m_DirMask=(ASHORT)DM; }
