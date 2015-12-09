
//��Ÿ 2003 4��15���޸�
//�ι������
//���һ���޸� 2003 4��25������17:35
//˵����
//����һ������tile��ͼ��·����Ѱ���㷨

//ʹ�õ�ʱ����Ҫһ���ⲿ����
//ASHORT  xend;
//ASHORT  yend;
//��ָ����Ѱ�������յ㡣
//����Create��ʱ����Ҫָ�������ⲿ��function����һ�����ж�ĳ��tile���赲�ģ�����һ����Judge����
//��ͼ��������Ͽ������ж��赲�ĺ����������
//���Ե���SetCloseLimited��ָ�������������ȡ�
//����SetDirMask(0xFF)����ָ�������ķ���
//����FindPath��ʼѰ·
//����MakePath����server����Ҫ��·��,Ȼ�����PathInfo���Եõ���Ҫ��·��
//����GetPathInfo����ȡ�ã�������ʽ������
//���һ��Ѱ·�����ClearModified���Ѱ·�ļ�¼
//��ͼ���꣺
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







// ������ڵ������, ������Ŀ�ĵع��۾����������
char AstarPathFind::AddToOpenQueue(TAstarNode *node)
{
    ADWORD i;
    ASHORT f=node->EstimateCost;
    TAstarNode *p,*b;
    node->Modified|=2;	// ��¼Open��־
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
// ����Ŀ�ĵع�������ķ���������
char AstarPathFind::GetFromOpenQueue()
{
    TAstarNode *BestChoice=m_Open;
    if (!m_Open) 
    {
        //m_LackMem = TRUE;
        //ȡ����Open����
        return -1;
    }
    m_Open=m_Open->Next;
    if (BestChoice->Modified&4) 
        return -2;               //�Ѿ���m_Close����
    BestChoice->Next=m_Close;
    BestChoice->Modified&=5;	 // ���m_Open��־
    BestChoice->Modified|=4;	 // ����m_Close��־
    m_Close=BestChoice;
    m_OpenCount--;
    m_CloseCount++;
    return 0;
}
// �ͷ�ջ���ڵ�
char AstarPathFind::PopCloseStack()
{
    if (m_Close) 
    { 
        m_Close->Modified&=3;		// ���m_Close��־
        m_Close=m_Close->Next;
        m_CloseCount--; 
        return 0; 
    }
    return -1;
}
// ��ԭ�޸Ĺ������нڵ�
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
// ������һ���ƶ��� x,y ���з�
char AstarPathFind::TryTile(short x,short y,TAstarNode *Father)
{
    int ActualCost;
    if (x<0||y<0||x>=m_width||y>=m_height) 
        return 1;
	if (!MoveAble(x,y,m_pRegion)) 
        return 1;			// �����ͼ�޷�ͨ�����˳�
    if (m_Node[y][x].Modified & 6)
        return 1;	// ����Ѿ�������m_Open,m_Close���������˳�
    ActualCost=Father->ActualCost+1;		// ʵ��ֵ����
    if (ActualCost>=m_Node[y][x].ActualCost) 
        return 1;
    if (!m_Node[y][x].Modified) 
        m_Modify[m_ModifyPoint++]=&m_Node[y][x]; // ��¼����޸Ĺ��ĵ��Ի�ԭ
    if(m_ModifyPoint > m_MaxSize)
        return 1;
    m_Node[y][x].ActualCost=ActualCost;
    m_Node[y][x].Modified=1;
    m_Node[y][x].Father=Father;
    m_Node[y][x].EstimateCost=ActualCost+JudgeCost(x,y,tile_x(Father->Pos),tile_y(Father->Pos));
    m_Node[y][x].Direct = GetDirection( x,y,tile_x(Father->Pos),tile_y(Father->Pos));

    AddToOpenQueue(&m_Node[y][x]);        // ���ڵ����m_Open����
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
    //�˷������
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


// ·��Ѱ��������
int AstarPathFind::FindPath(ASHORT startx,ASHORT starty,ASHORT endx,ASHORT endy)
{
    TAstarNode *root;
    int i,j,x,y,ok,dirs=0;
    ASHORT max = 0;
    ASHORT MinJudge;
    //�˷������
    static short incx[8]={  0,  1, 1, 1, 0, -1, -1, -1 };
    static short incy[8]={ -1, -1, 0, 1, 1,  1,  0, -1 };

    //�����㲻����
//    if((!MoveAble(startx,starty,m_pRegion))/*&&(!MoveAble(endx,endy))*/)
//        return -1;
    //����յ㲻����
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
    //�Ƿ���ֱ��·�����Ż�
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
    //��С��Judgeֵ
    MinJudge=JudgeCost(startx,starty,endx,endy);

    // ��һ�������
    for (ok=0;ok==0;)
    {
        ok=GetFromOpenQueue();		// ȡ��Open���й���ֵ��С�Ľڵ����m_Close��
        // Open�����Ѿ�������ֵ�Ĵ�С����
        root=m_Close;					// �õ��ղ�ȡ���Ľڵ�
        if (ok||root==NULL) 
        { 
            ok=-1; 
            break; 
        }
        x=tile_x(root->Pos);
        y=tile_y(root->Pos);
        if ( root->EstimateCost - root->ActualCost < MinJudge )	// �ҵ�һ���������յ�����ĵ�
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
            m_MinPos=root->Pos,ok=1;	// ����ߵ��յ���
        }
        else 
        {
            for (i=0,dirs=1,j=1;i<8;i++,dirs<<=1)		// ���û���ߵ��յ�
                if (m_DirMask & dirs) 
                {
                    char ret;
                    ret = TryTile(x+incx[i],y+incy[i],root);
                    //if ( ret == -2 )
                    //    //goto _outofmem;
                    //else
                    j &= ret;
                }
                if (j) //���j�޷����ߣ������close������pop����
                    if (PopCloseStack()) 
                    { 
                        ok=-2; 
                        break; 
                    }	// �������ͨ·���m_Close��ȡ��
        }
        if (m_OpenCount>=m_OpenLimited||m_CloseCount>=m_CloseLimited) // �����������·����Ŀ
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
    xlast = tile_x(pTempClose->Pos);            //ȡ����һ���߹��ĵ�
    ylast = tile_y(pTempClose->Pos);      

    pTempClose = pTempClose->Father;            //ȡ��һ���������
    if( pTempClose == NULL )
        return; 
    lastdir = GetDirection(xlast,ylast,tile_x(pTempClose->Pos),tile_y(pTempClose->Pos));//ȡ�������ķ�����

    int   iPathCount = 0;
    int   iMoveTile = 0;
    //BYTE  xbyte,ybyte;
    while(pTempClose!=NULL)
    {
        if(iPathCount>=15)                      //�ж��Ƿ񳬹����·��
            break;
        x = tile_x(pTempClose->Pos);            //ȡ����εĵ�����
        y = tile_y(pTempClose->Pos);    
        dirnow = GetDirection(xlast,ylast,x,y); //�ж�����������Է���
        if (lastdir != dirnow /*|| pTempClose->Father == NULL*/ )
        {
            PathInfo[iPathCount].dir = lastdir;
            PathInfo[iPathCount].len = iMoveTile;
            iMoveTile = 1;
            iPathCount++;
        }
        else
        {
            iMoveTile++ ;                       //·���ĸ����ۼ�
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
        xlast = x;                              //�Ƶ���һ����
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
    //�½��Ƿ���ĵı�ʶ
    m_Modify=new TAstarNode*[m_MaxSize];
    if (!m_Modify) 
        return -1;
    // �½���ͼ���ϵ�����
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
    //��ʼ���������ϵ���Ϣ
    for (j=0;j<maph;j++) 
        for (i=0;i<mapw;i++) 
        {
            m_Node[j][i].Pos=tile_pos(i,j);
            m_Node[j][i].Modified=0;
            m_Node[j][i].ActualCost=COST_MAX;
        }
        //m_LackMem = FALSE;
        m_ModifyPoint=0;
        //�趨����赲����
        SetMapCheck(MoveCheck);

        SetRegion(pSetRegion);
        //�趨���ۺ���
        SetJudgeFun(JudgeFun);
        //NoBlock = BlockFun;
        //�趨��Ѱ����
        SetDirMask(0xFF);
        //����open������ֵ
        SetOpenLimited(400);
        //SetOpenLimited(50);
        //����close������ֵ
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
        //while ((m_Node[i]->Father != NULL ) &&��(m_Node[i]->Next != NULL))
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
