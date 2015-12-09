/****************************************************************
//  FileName:   DFace.cpp
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
****************************************************************/
#include "StdAfx.h"

#include "dface.h"
#include "Canvas.h"
#include <string>

#include "Pub\\dwt.h"

#include "Cursor.h"
extern  CCursor*     g_pCursor;

extern CDFont	g_CommonFont12;
extern CDFont	g_CommonFont16;

static CDFace *pCaptureFace = NULL;
static CDFace *m_pFaceTopMost = NULL;

POINT CDFace::curCursorPosition = { 0, 0 };

bool isAlphaEnable = false;  // 用于渐变的标记
BYTE byAlpha = 0;

BYTE CalcShadowAlpha( BYTE shadowSpeed )  // 计算渐变Alpha, shadowSpeed越小，速度越快
{
    if ( isAlphaEnable )
    {
        static int counter = 0;
        counter++;

        if ( counter % shadowSpeed == 0 )
            byAlpha++;

        if ( byAlpha >= 32 )
        {
            isAlphaEnable = false;
            byAlpha = 0;
            counter = 0;
        }
        else
            return byAlpha;
    }

    return 32;
}

CDFace::CDFace(__int32 iid, int fontsize) : 
    m_IID(iid), 
    m_Font((fontsize==16)?g_CommonFont16:g_CommonFont12)
{
	m_pRef = NULL;
	m_bCapture = false;
	m_bShow = true;
    m_actFlag = false;
    m_pParent = NULL;
    m_wCount = 0;

    m_Font.SetColor(222, 222, 222);

    m_nLeft = 0, m_nTop = 0, m_nWidth = 0, m_nHeight = 0;
}

CDFace::~CDFace(void)
{
    // 尝试取消捕获状态,内部有判断,不会造成其他负担
    ReleaseCapture();

	m_bShow = false;
	m_ChildsList.clear();
    m_pParent = NULL;
}

void CDFace::SetFontSize(int size)
{
    LPVOID p = &m_ChildsList;
    *(LPDWORD)((LPBYTE)p + sizeof(m_ChildsList)) = (DWORD)(ptrdiff_t)(&((size==16)?g_CommonFont16:g_CommonFont12));
}

bool CDFace::SetParameter(LPCSTR szParameterName, LPARAM lParam)
{
    if( strcmp(szParameterName, "Position") == 0 || strcmp(szParameterName, "pos") == 0 )
    {
        m_nLeft = GetParameterDWORD(lParam, 1);
        m_nTop  = GetParameterDWORD(lParam, 2);
        if ( m_nLeft < 0 ) m_nLeft = g_pCanvas->m_nWidth + m_nLeft;
        if ( m_nTop < 0 ) m_nTop = g_pCanvas->m_nHeight + m_nTop;
    }
    else if( strcmp(szParameterName, "Abspos") == 0 )
    {
        m_nLeft = GetParameterDWORD(lParam, 1);
        m_nTop  = GetParameterDWORD(lParam, 2);
    }
    else if(stricmp(szParameterName, "Size") == 0)
    {
        m_nWidth = GetParameterDWORD(lParam, 1);
        m_nHeight = GetParameterDWORD(lParam, 2);

        //if ( m_nWidth == 800 ) m_nWidth = g_pCanvas->m_nWidth;
        //if ( m_nWidth == 799 ) m_nWidth = g_pCanvas->m_nWidth - 1;
        //if ( m_nHeight == 600 ) m_nHeight = g_pCanvas->m_nHeight;
        //if ( m_nHeight == 599 ) m_nHeight = g_pCanvas->m_nHeight - 1;
    }
    else if(strcmp(szParameterName, "Name") == 0)
        dwt::strcpy(m_GuiName, (LPSTR)lParam, 256);
    
    else if ( ( strcmp( szParameterName, "VisibleAtStart" ) == 0 ) ||
        ( stricmp( szParameterName, "visiable" ) == 0 ) )
    {
        if ( stricmp( (LPSTR)lParam, "false" ) == 0 )
            m_bShow = false;
        else if ( stricmp( (LPSTR)lParam, "true" ) == 0 )
            m_bShow = true;
    }
    else if ( strcmp( szParameterName, "onkey" ) == 0 )
	{
		size_t len = strlen( m_GuiName );
		if ( len > 100 )
			return false;

		LPDWORD optional = ( LPDWORD )( m_GuiName + len + 2 );
        optional[0] = ( int )lParam;
	}
	else
        return false;

    return true;
}

int CDFace::InitByBinary(LPARAM lParam)
{
	return 1;
}

int	CDFace::KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y)
{
	return	RET_NONE;
}

int	CDFace::PtInFace(POINT &pt)
{
    RECT rc = GetRECT();    
    ClientToScreen(&rc);
    return PtInRect(&rc, pt);
}

static int nCurrent_x = 0;
static int nCurrent_y = 0;

int CDFace::OnMessage(KEY_STATE iKeyState,int vKey,int x,int y)
{
	if (!m_bShow)
		return 0;

	RECT rcWnd = GetRECT();
	ClientToScreen(&rcWnd);

    RECT rc;

    POINT pt;
	pt.x = x; 
    pt.y = y;
    nCurrent_x = x;
    nCurrent_y = y;
    int rcode = 0;

	if (iKeyState < MS_BEGIN)
	{
		if (m_ChildsList.size() != 0)
		{
			LPFace pFace = *m_ChildsList.begin();
            if(pFace->isVisible())
			    pFace->OnMessage(iKeyState, vKey, x, y);
		}
		else
		{
		}
	}
	else
    {
//      bool bChildCaptured = false;

		//if (m_ChildsList.size() != 0)
  //      {
		//    for (std::list< LPFace >::iterator m_Child = m_ChildsList.begin(); m_Child != m_ChildsList.end(); m_Child ++)
		//    {
		//	    LPFace pFace = *m_Child;
		//	    if (pFace->m_bCapture && pFace->isVisible())
		//	    {
  //                  bChildCaptured = true;
  //                  break;
  //              }
  //          }
  //      }

        //if (isCaptured() && !bChildCaptured)
        //{
        //     KeyMsAction(iKeyState, vKey, x, y);
        //}

        if (pCaptureFace != NULL)
            return pCaptureFace->KeyMsAction(iKeyState, vKey, x, y);

        BOOL hit = false;
        int result = 0;

        CDFace *doAct = NULL;

		for ( m_Child = m_ChildsList.begin(); m_Child != m_ChildsList.end(); m_Child ++ )
		{
			LPFace pFace = *m_Child;
            rc = pFace->GetRECT();  
            pFace->ClientToScreen(&rc);

            if ( hit || !PtInRect( &rc, pt ) )
            {
                pFace->OnMouseLeave();
                continue;
            }

			//if (m_pFaceTopMost)
			//	if (m_pFaceTopMost->isVisible()&& m_pFaceTopMost->PtInFace(pt))
			//	{
			//		return m_pFaceTopMost->OnMessage(iKeyState, vKey, x, y);
			//	}

            if ( pFace->isVisible() && !hit )
            {
                if ((pCaptureFace == NULL) && pFace->PtInFace(pt))
                {
                    OnProccMessage( iKeyState );
				    result = pFace->OnMessage(iKeyState, vKey, x, y);
                    if ( result == -1 )
                        doAct = pFace;
                    hit = true;
                }
            }
		}

        if ( doAct ) doAct->RealAct();

        if ( hit ) return result;

		if (PtInRect(&rcWnd, pt))
		{
			if(iKeyState == MS_LDOWN || iKeyState == MS_RDOWN)
			{
                CDFace* pParent = GetParent();
                if(pParent)
                    pParent->Active();

				Active();
				//Paint();
                rcode = -1;
			}
		}		
    }

    //if ((isCaptured()) &&(iKeyState > MS_BEGIN))
    //    return RET_NONE;

	return KeyMsAction(iKeyState, vKey, x, y), rcode;
}

int CDFace::OnMyMessage(LPFace lpFace, LPARAM lParam)
{
	return RET_NONE;
}

void CDFace::RecvHotKey(int vKey)
{
    if( (vKey ==  ' ') || (vKey == VK_ESCAPE))
        return;

    CDFace* pFace = GetFirstFace();

    if(pFace != NULL)				//处理快捷键
        if(( pFace->GetPoint(IID_CDEDIT) == NULL ) || (!pFace->isVisible()) || ((vKey >= VK_F1) && (vKey <= VK_F12)))
        {
            OnHotKey(vKey);
	        if(m_ChildsList.size() != 0)
		        for(std::list< LPFace >::iterator child = m_ChildsList.begin(); child != m_ChildsList.end(); child ++)
		        {
                    LPFace pFace = *child;
                    if(pFace->isVisible())
                        pFace->RecvHotKey(vKey);
                }
        }
}

void CDFace::OnHotKey(int vKey)
{
}

void CDFace::SetCursor(Sprite* pSpr)
{
//	pSpr->Draw(,,0);
}

bool CDFace::Create(CDFace* pParent, RECT rc, LPARAM lParam)
{
	m_pParent = pParent;
	m_rcArea = rc;
    m_nHeight = rc.bottom - rc.top;
    m_nWidth = rc.right - rc.left;

	m_nLeft = rc.left;
	m_nTop = rc.top;

	if(!OnCreate(lParam))
		return false;
	if(pParent)
	{
		pParent->AddChild(this);       
	}
	return true;
}

void CDFace::Paint()
{
    if ( ( pCaptureFace != NULL ) && ( pCaptureFace == this ) )
        return;

    //extern BOOL paintDebug;
    //LARGE_INTEGER lb, le;
    //if ( paintDebug )
    //    QueryPerformanceCounter( &lb );

	if(m_bShow)
	{
        // 用于检测该虚函数调用的具体目标地址！
        void *checkPtr = ( *( void*** )this )[9];
        _asm {
            mov ecx, this
            call checkPtr
        }
	    //Update();

		for ( std::list< LPFace >::reverse_iterator it = m_ChildsList.rbegin();
			it != m_ChildsList.rend(); it ++ )
		{
			if ( (*it)->m_actFlag && (*it)->m_bShow )
			{
				(*it)->RealAct();
				it = m_ChildsList.rbegin();
			}
		}


        //std::list< LPFace >::reverse_iterator tmpit = m_ChildsList.rend();
        for ( std::list< LPFace >::reverse_iterator it = m_ChildsList.rbegin();
            it != m_ChildsList.rend(); it ++ )
        {
            (*it)->Paint();
            (*it)->PaintLevelTop();
            //if ( (*it)->m_actFlag )
            //    tmpit = it;
        }

        //if ( tmpit != m_ChildsList.rend() )
        //    ( *tmpit )->RealAct();

		//std::list< LPFace >::iterator m_Child = m_ChildsList.end();
		//if(m_ChildsList.size() !=0)
		//do 
		//{
		//	m_Child--;
		//	(*m_Child)->Paint();
		//}
		//while (m_Child != m_ChildsList.begin());

		//if (m_pFaceTopMost)
		//	m_pFaceTopMost->Paint();
	}

    //if ( paintDebug && ( std::string( "登陆窗口" ) == m_GuiName ) ) {
    //    QueryPerformanceCounter( &le );
    //    LARGE_INTEGER lf;
    //    QueryPerformanceFrequency( &lf );
    //    static char _str[1024];
    //    sprintf( _str, "%s %.08f\r\n", m_GuiName, (float)(( (double)le.QuadPart - lb.QuadPart ) / lf.QuadPart ) );
    //    OutputDebugString( _str );
    //}
}

void PaintCaptureFace()
{
    if ( pCaptureFace == NULL ) 
        return;

	for ( CDFace *iter = pCaptureFace; iter != NULL; iter = iter->GetParent() )
	{
		if ( iter->isVisible() )
			continue;

		ReleaseCapture();
		return;
	}

    if ( pCaptureFace->isVisible() )
	{
	    pCaptureFace->Update();

		for ( std::list< LPFace >::reverse_iterator it = pCaptureFace->m_ChildsList.rbegin();
            it != pCaptureFace->m_ChildsList.rend(); it ++ )
		{
			(*it)->Paint();
		}
	}
}

void CDFace::PaintTip()
{
	if(m_bShow)
	{
        UpdateTip();

		for ( std::list< LPFace >::reverse_iterator it = m_ChildsList.rbegin();
            it != m_ChildsList.rend(); it ++ )
		{
			(*it)->PaintTip();
		}
	}
}

void CDFace::Active()
{
	if(!m_pParent)
		return;

    m_actFlag = true;

    return;
}

void CDFace::RealAct()
{
    if ( !m_actFlag )
        return;

	if(!m_pParent)
		return;

	LPFace temp(this);
    if(m_bShow)
    {
        m_actFlag = false;
		if ( (CDFace *)m_pParent->m_ChildsList.front() != this )
		{
            std::list< LPFace >::iterator it = 
                std::find( m_pParent->m_ChildsList.begin(), m_pParent->m_ChildsList.end(), temp );
            if ( it != m_pParent->m_ChildsList.end() )
            {
			    m_pParent->m_Child = m_pParent->m_ChildsList.erase( it );
			    m_pParent->m_ChildsList.push_front(this);
            }
		}
	    // Update();
    }

	if (m_pParent)
	{
		m_pParent->Active();
	}
}

//bool CDFace::AddChild(LPFace pGuiObject)
//{
//	m_ChildsList.push_back(pGuiObject); 
//	return true;
//}

bool CDFace::AddChild( GuiObjConverter pGuiObject )///////////////
{
    //m_pParent = ?????
    ((CDFace*)pGuiObject)->m_pParent = this;
    m_ChildsList.push_back((CDFace*)pGuiObject); 
    return true;
}

void CDFace::PaintLevelTop()
{
}
void CDFace::Update()
{
}

void CDFace::UpdateTip()
{
}

char* CDFace::GetParameterStr(LPARAM lParam, WORD wIndex)// wIndex从1开始
{
    static char sParam[256] = {0};
    WORD j = 1;
    short nCut[20] = {-1};
    char* Param = (char*)lParam;
    int nLen = dwt::strlen(Param, 256);
    for(int i = 0; i < nLen; i ++)
        if(Param[i] == ',')
            nCut[j++] = i;

    nCut[j] = nLen;

    if(wIndex > j) return 0;

    memcpy(sParam, Param + nCut[wIndex - 1] + 1, nCut[wIndex] - nCut[wIndex - 1]);
    sParam[ nCut[wIndex] - nCut[wIndex - 1] - 1 ] = 0;


    /*
    static char sParam[256] = {0};
    WORD j = 1;
    WORD nCut[20] = {0};
    char* Param = (char*)lParam;
    for(int i = 0; i < dwt::strlen(sParam, 256); i ++)
        if(Param[i] == ',')
            nCut[j++] = i;

    if(wIndex > j) return NULL;

    memcpy(sParam, Param + nCut[wIndex - 1], nCut[wIndex] - nCut[wIndex - 1]);
    sParam[nCut[wIndex] - nCut[wIndex - 1]] = 0;
    */

    return sParam;
}

DWORD CDFace::GetParameterDWORD(LPARAM lParam, WORD wIndex)// wIndex从1开始
{
    char sParam[256] = {0};
    WORD j = 1;
    short nCut[20] = {-1};
    char* Param = (char*)lParam;
    int nLen = dwt::strlen(Param, 256);
    for(int i = 0; i < nLen; i ++)
        if(Param[i] == ',')
            nCut[j++] = i;

    nCut[j] = nLen;

    if(wIndex > j) return 0;

    memcpy(sParam, Param + nCut[wIndex - 1] + 1, nCut[wIndex] - nCut[wIndex - 1]);
    sParam[ nCut[wIndex] - nCut[wIndex - 1] - 1 ] = 0;

    return (DWORD)_atoi64(sParam);
}

int SplitParameters( LPCSTR str, std::vector< std::string > &args )
{
    args.clear();
    if ( ( str == NULL ) || ( str[0] == 0 ) )
        return 0;

    int nLen = dwt::strlen( str, 512 );
    char tempstr[1024];
    memcpy( tempstr, str, nLen + 1 );

    WORD j = 1;
    short nCut[100] = {0};

    for ( int i = 0; i < nLen; i ++ )
    {
        if ( tempstr[i] == ',' )
        {
            tempstr[i] = 0;
            nCut[j++] = i + 1;
        }
    }

    for ( int i = 0; i < j; i ++ )
        args.push_back( tempstr + nCut[i] );

    return (int)args.size();
}

void CDFace::SetCapture()
{
	if ((!m_bCapture) && (pCaptureFace == NULL))
	{
		m_bCapture = true;
        pCaptureFace = this;
		//if (m_pParent)
		//{
		//	LPFace temp(this);
		//	m_pParent->m_ChildsList.remove(this);
		//	m_pParent->m_ChildsList.push_front(this);
		//	m_pParent->SetCapture();
		//}
	}
}

CDFace*  CDFace::GetCapturedFace()
{
    return pCaptureFace;
}

void CDFace::ReleaseCapture()
{
    if (this == pCaptureFace)
    {
        pCaptureFace = NULL;

	    m_bCapture = false;
    }
	//if (m_pParent)
	//{
	//	m_pParent->ReleaseCapture();
	//}
}

CDFace* CDFace::GetParent()
{
	return m_pParent;
}

CDFace* CDFace::GetFirstChild()
{
	if ( m_ChildsList.size() == 0 )
        return NULL;

    return *m_ChildsList.begin();
}

CDFace* CDFace::GetNextChild(CDFace* pFace)
{
	if(m_ChildsList.size() == 0)
	{
        return NULL;
    }
    std::list < LPFace >::iterator it = m_ChildsList.begin();
    for(; it != m_ChildsList.end(); it ++)
    {
        CDFace* pChild = *it;
        if(pChild == pFace)
        {
            it ++;
            return (it != m_ChildsList.end())?(LPFace)*it:NULL;
        }
    }
    return NULL;
}

CDFace* CDFace::GetFirstFace()
{
    CDFace* pFace = this;
    while(pFace->GetParent())
        pFace = pFace->GetParent();

    if(!pFace->isVisible())   //
        return NULL;

    CDFace* pChild = pFace->GetFirstChild(); 

    while(pChild) 
    {
        pFace = pChild;
        if(!pChild->isVisible())
            pChild = GetNextChild(pChild);
        else
            pChild = pChild->GetFirstChild();
    }

    return pFace;
}

int	CDFace::OnHide()
{
	return 1;
}

int	CDFace::OnClose()
{
	return 1;
}

void CDFace::Show(bool bShow)
{
	m_bShow = bShow;
    if(!bShow)
        OnHide();
}

void CDFace::OnMouseLeave()
{
	for( m_Child = m_ChildsList.begin(); m_Child != m_ChildsList.end(); m_Child ++ )
	{
        LPFace pFace = *m_Child;
        pFace->OnMouseLeave();
    }
}

void CDFace::ClientToScreen(RECT* rc)
{
	CDFace* pTmp = GetParent();

    while (pTmp != NULL)
	{
        //RECT r = pTmp->GetRECT();
        //pTmp->ClientToScreen(&r);
		rc->left += pTmp->m_nLeft;
		rc->right += pTmp->m_nLeft;
		rc->top += pTmp->m_nTop;
		rc->bottom += pTmp->m_nTop;
		pTmp = pTmp->GetParent();
	}
} 

void CDFace::ClientToScreen(POINT *pt)
{
	CDFace* pTmp = GetParent();
	while(pTmp)
	{
		pt->x += pTmp->GetRECT().left;
		pt->y += pTmp->GetRECT().top;
		pTmp = pTmp->GetParent();
	}
}
void CDFace::ScreenToClient(POINT *pt)
{
	CDFace* pTmp = GetParent();
	while(pTmp)
	{
		pt->x -= pTmp->GetRECT().left;
		pt->y -= pTmp->GetRECT().top;
		pTmp = pTmp->GetParent();
	}
}

void CDFace::OnMove(int x, int y)
{
	m_nLeft += x;
	m_nTop += y;
    if(m_nLeft < 0) 
    {
        m_nLeft = 0;
    }
    if(m_nLeft + m_nWidth > g_pCanvas->m_nWidth)
    {
        m_nLeft = g_pCanvas->m_nWidth - m_nWidth;
    }
    if(m_nTop < 0) 
    {
        m_nTop = 0;
    }
    if(m_nTop + m_nHeight > g_pCanvas->m_nHeight) 
    {
        m_nTop = g_pCanvas->m_nHeight - m_nHeight;
    }
    
	Paint();
}

void CDFace::OnMoveTo(int x, int y)
{
	m_nLeft = x;
	m_nTop = y;
    if(m_nLeft < 0) 
    {
        m_nLeft = 0;
    }
    if(m_nLeft + m_nWidth > g_pCanvas->m_nWidth)
    {
        m_nLeft = g_pCanvas->m_nWidth - m_nWidth;
    }
    if(m_nTop < 0) 
    {
        m_nTop = 0;
    }
    if(m_nTop + m_nHeight > g_pCanvas->m_nHeight) 
    {
        m_nTop = g_pCanvas->m_nHeight - m_nHeight;
    }

	Paint();
}
void CDFace::SetPos(int x, int y, int width, int height)
{
    m_nLeft = x; 
    m_nTop = y;
    m_nWidth = width;
    m_nHeight = height; 
}

void CDFace::SetPos( int x, int y )
{
	m_nLeft = x; 
	m_nTop = y;
}

int CDFace::GetHeight()
{
	return m_nHeight;
}

int CDFace::GetWidth()
{
	return m_nWidth;
}

void CDFace::SetRECT(int left,int top,int right,int bottom)
{
    m_nLeft = left; 
    m_nTop = top;
    m_nWidth = right - left;
    m_nHeight = bottom - top; 
}

RECT CDFace::GetRECT()
{
    RECT rc;
    rc.left = m_nLeft; 
    rc.top = m_nTop;
    rc.right = m_nLeft + m_nWidth; 
    rc.bottom = m_nTop + m_nHeight;
    return rc;
}

void CDFace::DrawRect(RECT* rc, int nClrLT, int nClrRB)
{
	g_pCanvas->DrawLine(rc->left, rc->top, rc->right-1, rc->top, nClrLT);
	g_pCanvas->DrawLine(rc->left, rc->top, rc->left, rc->bottom-1, nClrLT);
	g_pCanvas->DrawLine(rc->right-1, rc->top, rc->right-1, rc->bottom-1, nClrRB);
	g_pCanvas->DrawLine(rc->left, rc->bottom-1, rc->right-1, rc->bottom-1, nClrRB);	
}

bool CDFace::isVisible()
{
    return m_bShow;
}

bool CDFace::isActive()
{
	//if (m_pParent)
	//	return (m_pParent->m_ChildsList.front() == LPFace(this));
    CDFace *it = this;
    while ( it->m_pParent ) {
        if ( it->m_pParent->m_ChildsList.front() != it )
            return false;
        it = it->m_pParent;
    }
	return true;
}

void CDFace::GetCurrentPos(POINT& pt)
{
    pt.x = nCurrent_x;
    pt.y = nCurrent_y;
}

void CDFace::SetTopMost(CDFace* pFace)
{ 
	m_pFaceTopMost = pFace; 
}


bool CDFace::OnProccMessage( KEY_STATE iKeyState )
{
    if( g_pCursor && iKeyState==MS_MOVE ) 
    {
        g_pCursor->SetMyCursor( /*CURSORTYPE_COMMON*/);
    }
    return  false;
}

void ClearAlphaWithBord(int nX, int nY, int nWidth, int nHeight, int nColor,int nAlpha, WORD wBordCornerColor, WORD wBordColor)
{
    if ( g_pCanvas == NULL )
        return;

    g_pCanvas->ClearAlpha( nX, nY, nWidth, nHeight, nColor, nAlpha );

    if ( nWidth < 12 || nHeight < 12 )
        return;

    g_pCanvas->DrawLine( nX, nY, nX + 4, nY, wBordCornerColor ); // 左上
    g_pCanvas->DrawLine( nX, nY, nX, nY + 4, wBordCornerColor );			

    g_pCanvas->DrawLine( nX + nWidth - 4, nY, nX + nWidth, nY, wBordCornerColor ); // 右上
    g_pCanvas->DrawLine( nX + nWidth, nY, nX + nWidth, nY + 4, wBordCornerColor );

    g_pCanvas->DrawLine( nX + nWidth - 4, nY + nHeight, nX + nWidth, nY + nHeight, wBordCornerColor ); // 右下
    g_pCanvas->DrawLine( nX + nWidth, nY + nHeight - 4, nX + nWidth, nY + nHeight, wBordCornerColor );

    g_pCanvas->DrawLine( nX, nY + nHeight , nX + 4, nY + nHeight, wBordCornerColor ); // 左下
    g_pCanvas->DrawLine( nX, nY + nHeight - 4 , nX, nY + nHeight, wBordCornerColor );

    g_pCanvas->DrawLine( nX + 6, nY, nX + nWidth - 6, nY, wBordColor ); // 上
    g_pCanvas->DrawLine( nX + 6, nY + nHeight, nX + nWidth - 6, nY + nHeight, wBordColor ); // 下
    g_pCanvas->DrawLine( nX, nY + 6, nX, nY + nHeight - 6 , wBordColor ); // 左
    g_pCanvas->DrawLine( nX + nWidth, nY + 6, nX + nWidth, nY + nHeight - 6, wBordColor ); // 右
}