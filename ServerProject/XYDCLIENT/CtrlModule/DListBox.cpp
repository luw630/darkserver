/****************************************************************
//  FileName:   DListBox.cpp
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
****************************************************************/
#include "StdAfx.h"

#include "dlistbox.h"
#include "DSlideBar.h"

extern CDFont	g_CommonFont12;
extern CDFont	g_CommonFont16;

#define ITEMSTRING_MAXLEN 100

BEGIN_MY_MESSAGE_MAP(CDListBox)
MESSAGE_MAP(m_pSlideBar, OnPosChanged())
END_MY_MESSAGE_MAP

CDListBox::CDListBox(void) : CDFace(IID_CDLISTBOX)
{
    for(int i = 0; i < 10; i ++)
        m_nItemWidth[i] = 108;
    for(i = 0; i < 10; i ++)
        for(int j = 0; j < 500; j ++)
        {
            m_sItem[i][j] = NULL;
            m_pSprItem[i][j] = NULL;
			m_sItemColor[i][j] = 0;
        }

    m_nIndexSelected = -1;
    m_nIndexFocused = -1;
    m_nItemHeight = 17;

	m_nLineBegin = 0;
	m_nMaxLineCount = 0;
	m_nLineCount = 0;
    m_bShowBoard = false;
    m_bShowSelBoard = false;
	m_bShowSlideBar = false;
	m_pSlideBar = NULL;
	m_dwTextColor = 52800;
	m_dwSelColor = 99999;

	m_bHoldSlideBarPos = false;
	m_nLineCountCanShow = 100;

	m_HideSlideBar = false;
	m_LineCountOnlyShow = 0;
	m_bTexCenter = false;
	m_bEnable = true;
}

CDListBox::~CDListBox(void)
{
    for(int i = 0; i < 10; i ++)
        for(int j = 0; j < m_nMaxLineCount; j ++)
		{
			if( m_sItem[i][j] != NULL )
			{
				delete [] m_sItem[i][j];
				m_sItem[i][j] = NULL;
			}

            if ( m_pSprItem[i][j] != NULL )
                m_pSprItem[i][j] = NULL;    
		}
}

int	CDListBox::OnCreate(LPARAM lParam)
{
	RECT rcWnd = GetRECT();
//	ClientToScreen(&rcWnd);
	RECT rcSlideBar;
	rcSlideBar.left = m_nWidth - 16;//rcWnd.right - 15;//因为使用滚动条不多，所以这里配合图档
	rcSlideBar.top =  1;
	rcSlideBar.right = m_nWidth;//rcWnd.right;
	rcSlideBar.bottom = rcWnd.bottom - rcWnd.top;

	m_pSlideBar = new CDSlideBar;
	CDSlideBar::SParameter param;
	param.nColorBar = g_RGB(222, 122, 77);
	param.nColorSlider = g_RGB(128, 128, 128);
	m_pSlideBar->Create(this, rcSlideBar, (LPARAM)&param);

	if (m_pSlideBar)
	{
		m_pSlideBar->Show(false);
		m_pSlideBar->SetHorizon(false);

		m_pSlideBar->SetBePicture("", 
			"Interface\\okcancel\\滑动条按扭.spr",
			"Interface\\okcancel\\滑动条按扭.spr",
			"Interface\\okcancel\\滑动条按扭.spr");
	}
    return 1;
}

void CDListBox::setSlideBar( char* b1, char* b2, char* b3, char* b4 )
{
	if (m_pSlideBar)
	{
		m_pSlideBar->SetBePicture(b1, b2, b3, b4 );
	}
}

bool CDListBox::AddChild( GuiObjConverter pGuiObject )
{
	//CDFace::AddChild(m_pSlideBar);
	return CDFace::AddChild(pGuiObject);
}

int	CDListBox::InitByBinary(LPARAM lParam)
{
    return 1;
}
int CDListBox::GetSelectIndex( int x, int y )
{
	RECT rcWnd = GetRECT(); 
	ClientToScreen(&rcWnd);

	if ((y - rcWnd.top)/m_nItemHeight + m_nLineBegin < m_nMaxLineCount) 
		return (y - rcWnd.top)/m_nItemHeight + m_nLineBegin;
	else
		return -1;
}
int	CDListBox::KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y)
{
    RECT rcWnd = GetRECT(); 
    ClientToScreen(&rcWnd);


    m_x = x, m_y = y;

	if ((y - rcWnd.top)/m_nItemHeight + m_nLineBegin < m_nMaxLineCount) 
		m_nIndexFocused = (y - rcWnd.top)/m_nItemHeight + m_nLineBegin;
	else
		m_nIndexFocused = -1;
		
	switch( iKeyState )
	{
	case MS_LDOWN:
		{
			if ( !m_bEnable)
			{
				LDownPrepareArgs arg(m_bEnable);
				m_LDownPrepareEvent.Invoke(this,&arg);
				return RET_NONE;
			}
			if ((m_nIndexFocused >= 0) && (m_nIndexFocused < m_nMaxLineCount))
			{
				m_nIndexSelected = m_nIndexFocused;
				OnClicked();
			}
		}
		break;
	case MS_MOVE:
		{	
			MouseEventArgs arg(x,y);
			m_MouseMoveEvent.Invoke(this,&arg);
			if (m_pSlideBar && m_pSlideBar->isVisible() )
			{
				RECT rcSlideBar = m_pSlideBar->GetRcBar();
				POINT pt;
				pt.x = x; pt.y = y;
				if ( PtInRect( &rcSlideBar, pt ) )
					m_pSlideBar->m_bShowTip = true;
				else
					m_pSlideBar->m_bShowTip = false;
			}
		}
		break;
	case MS_LBUTTONDBLCLK:
		{
			if ( !m_bEnable)
			{
				LDownPrepareArgs arg(m_bEnable);
				m_LDownPrepareEvent.Invoke(this,&arg);
				return RET_NONE;
			}
			if ((m_nIndexFocused >= 0) && (m_nIndexFocused < m_nMaxLineCount))
			{
				m_nIndexSelected = m_nIndexFocused;
				m_DbSelectItemEvent.Invoke(this,NULL);
			}
		}
		break;
	case MS_MOUSEWHEEL:
		{
			//新增中键滑动 added by xj
			float nTmp;
			int nPos;
			//中键后滚 负数。  中键前滚，正数
			if (vKey > 0 && m_pSlideBar->isVisible())	//前滚
			{
				//nTmp = (float)(rcWnd.right - rcWnd.left - m_nBarWidthHeight)/(m_nMaxPos - m_nMinPos);
				//nPos = (x - rcWnd.left - nXtoBar)/nTmp;
				m_pSlideBar->SetPosition(m_pSlideBar->GetPosition()-5); 
				int nPos = m_pSlideBar->GetPosition(); 
				RECT rcWnd = GetRECT();
				m_nLineBegin = (m_nMaxLineCount - m_nLineCountCanShow) * nPos / 100  ;
				
				//m_nLineBegin = (m_nMaxLineCount - m_nLineCountCanShow) * nPos / 100  ;
				if (m_nLineBegin < 0)
					m_nLineBegin = 0;


			}
			else if(vKey < 0 && m_pSlideBar->isVisible())	//后滚
			{
				m_pSlideBar->SetPosition(m_pSlideBar->GetPosition()+5); 
				int nPos = m_pSlideBar->GetPosition(); 
				RECT rcWnd = GetRECT();
				m_nLineBegin = (m_nMaxLineCount - m_nLineCountCanShow) * nPos / 100  ;
				//m_pSlideBar->SetPosition(m_pSlideBar->GetPosition()+2); 
				//nTmp = (float)(rcWnd.bottom - rcWnd.top - m_nBarWidthHeight)/(m_nMaxPos - m_nMinPos);
				//nPos = (y - rcWnd.top - nYtoBar)/nTmp;
			}
		}
		break;
	default:
		break;
	}

	return CDFace::KeyMsAction(iKeyState, vKey, x, y);
}

int CDListBox::OnClicked()
{
	m_SelectItemEvent.Invoke(this,NULL);
    CDFace *pParent = GetParent();
    if(pParent != NULL)
	    return pParent->OnMyMessage(this, 0);
    else
    {
        return 0;
    }
}

void CDListBox::Update()
{
    RECT rcWnd = GetRECT();
    ClientToScreen(&rcWnd);

	m_nLineCountCanShow = (rcWnd.bottom - rcWnd.top)/m_nItemHeight;

	if( m_LineCountOnlyShow > 0 && m_LineCountOnlyShow < m_nLineCountCanShow )
		m_nLineCountCanShow = m_LineCountOnlyShow;

    POINT pt;
    pt.x = m_x, pt.y = m_y;

    for(int i = 0; i < 10; i ++)
    {
        for(int j = m_nLineBegin; j < m_nMaxLineCount; j ++)
        {
			if (m_sItem[i][j] == NULL && m_pSprItem[i][j] == NULL )
				continue;

			//if ((j - m_nLineBegin + 1) * m_nItemHeight > (rcWnd.bottom - rcWnd.top))
			if ( (j - m_nLineBegin + 1) > m_nLineCountCanShow )
				break;

            if( ( m_sItem[i][j] != NULL && m_sItem[i][j][0] != 0 ) || m_pSprItem[i][j] != NULL )
            {
				//滚动条。右边偏移
                int w = m_Font.GetLineWidth();
                int h = m_Font.GetLineHeight();
                DWORD nClr = m_Font.GetColor();

				m_Font.SetLine(m_nItemWidth[i] - ( ( m_bShowSlideBar | m_bHoldSlideBarPos )? 15 : 0 ), 18); 
                
                if(j == m_nIndexSelected)
				{
                    m_Font.SetColor(m_dwSelColor);

					RECT rcBoard;
					//滚动条移动到了右边不需要左边偏移了
					rcBoard.left = rcWnd.left + GetPosOffestX(i) + 2 + ((m_bShowSlideBar|m_bHoldSlideBarPos)?15:0);
					rcBoard.top  = rcWnd.top + (j - m_nLineBegin) * m_nItemHeight;
					rcBoard.right = rcBoard.left + m_nItemWidth[i];
					rcBoard.bottom = rcBoard.top + m_nItemHeight;
					if (m_bShowSelBoard)
					{
						DrawRect(&rcBoard, 333333, 333333);
					}

                    if ( i == 0 )
					    m_sprSelBack.DrawAlpha(rcWnd.left + GetPosOffestX(i) + ((m_bShowSlideBar|m_bHoldSlideBarPos)?15:0), rcWnd.top + (j - m_nLineBegin) * m_nItemHeight + 4, 0);
				}
                else if(j == m_nIndexFocused)
                    m_Font.SetColor(22, 244, 22);
				else
					m_Font.SetColor(m_dwTextColor);
                if ( m_sItem[i][j] != NULL && m_sItem[i][j][0] != 0 )
				{
					int add = 0;
					if( m_bTexCenter )
					{
						int size = (int)strlen( m_sItem[i][j] ) * 6;
						add = ( m_nWidth - size ) / 2;
					}
					if( m_sItemColor[i][j] != 0 )
						m_Font.SetColor( m_sItemColor[i][j] );

					int ileft = rcWnd.left + add + GetPosOffestX(i) + 5;
					int itop = rcWnd.top + (j - m_nLineBegin) * m_nItemHeight + 5;

					m_Font.DrawColorText( ileft /*+ ((m_bShowSlideBar|m_bHoldSlideBarPos)?15:0)*/, itop, m_sItem[i][j]);
				}
                
                m_Font.SetLine( w,h );
                m_Font.SetColor(nClr);

                if ( m_pSprItem[i][j] != NULL )
                    m_pSprItem[i][j]->DrawAlpha( rcWnd.left + GetPosOffestX(i) + 5, rcWnd.top + (j - m_nLineBegin) * m_nItemHeight + 5, 0 );
            }
        }
    }

    if(m_bShowBoard)
        DrawRect(&rcWnd, 33333,44444);
}

void CDListBox::SetShowSelSpr(char *szSprFileName)
{
	m_sprSelBack.Load(szSprFileName);
}

void CDListBox::OnMouseLeave()
{
    m_nIndexFocused = -1;
    //m_nIndexSelected = -1;
    if ( m_pSlideBar )
        m_pSlideBar->m_bShowTip = false;
}

void CDListBox::AddItem(int nX, int nY, LPCSTR sItem)
{
    if ( nX >= 10 || nY >= 500 || nX < 0 || nY < 0 )
        return;

    RECT rcWnd = GetRECT();
    ClientToScreen(&rcWnd);
	m_nLineCountCanShow = (rcWnd.bottom - rcWnd.top)/m_nItemHeight;


    if(strlen(sItem) >= ITEMSTRING_MAXLEN) 
    {
#ifdef _DEBUG
        MessageBox(NULL, "err", "String too long!", MB_OK);
#endif
        return;
    }
	if ((nY+1) > m_nMaxLineCount)
		m_nMaxLineCount = nY+1;

    if ( IsLineEmpty(nY) )
        m_nLineCount++;

	//for (int i=0; i<10; i++)
	//	for (int j=0; j < m_nMaxLineCount; j ++)
	//	{
	//		if (m_sItem[i][j] == NULL)
	//			m_sItem[i][j] = new char[ITEMSTRING_MAXLEN];
	//	}
	if (m_sItem[nX][nY] == NULL)
	{
		m_sItem[nX][nY] = new char[ITEMSTRING_MAXLEN];
	}
	//memset (m_sItem[nX][nY], 0, ITEMSTRING_MAXLEN);

	if (m_sItem[nX][nY])
	{
		dwt::strcpy(m_sItem[nX][nY], sItem, ITEMSTRING_MAXLEN-1);
	}


	m_bShowSlideBar = m_nMaxLineCount * m_nItemHeight > (rcWnd.bottom - rcWnd.top);
	m_pSlideBar->Show(m_bShowSlideBar && !m_HideSlideBar);
}

void CDListBox::SetItemWidth( int nWidth, int nX)
{
    m_nItemWidth[nX] = nWidth;
}

int CDListBox::GetSelItemIndex(int nRowIndex)
{
    return m_nIndexSelected;
}

void CDListBox::SetSelItemIndex(int nLine, bool bSendClickMsg) 
{ 
	m_nIndexFocused = m_nIndexSelected = nLine; 

	if (m_nMaxLineCount > 0)
	{
		m_pSlideBar->SetPosition((nLine+1) * 100 / m_nMaxLineCount);

		OnPosChanged();
	}
	else
		m_pSlideBar->SetPosition(0);

	if (bSendClickMsg)
		OnClicked();
}

char* CDListBox::GetSelItemString( int i )
{
    if ( i < 0 || i >= 10 )
        return NULL;

    return m_sItem[i][m_nIndexSelected];
}

void CDListBox::DeleteItem(int nIndex)
{
        for(int j = 0; j < 10; j ++)
        {
			if( m_sItem[j][nIndex] != NULL )
			{
				delete[] m_sItem[j][nIndex];
				m_sItem[j][nIndex] = NULL;
			}
            if ( m_pSprItem[j][nIndex] != NULL )
                m_pSprItem[j][nIndex] = NULL;     
        }
		m_nLineCount--;
}

void CDListBox::Clear()
{
    for(int i = 0; i < 10; i ++)
        for(int j = 0; j < m_nMaxLineCount; j ++)
		{
			if( m_sItem[i][j] != NULL )
			{
				delete[] m_sItem[i][j];
				m_sItem[i][j] = NULL;
			}
            if ( m_pSprItem[i][j] != NULL )
                m_pSprItem[i][j] = NULL;   
		}

	m_nLineCount = 0;
	m_nMaxLineCount = 0;
	m_nLineBegin = 0;
	m_bShowSlideBar = false;
	m_pSlideBar->Show(m_bShowSlideBar);
	m_pSlideBar->SetPosition(0);
}

int CDListBox::OnPosChanged()
{
	int nPos = m_pSlideBar->GetPosition(); 

	RECT rcWnd = GetRECT();
	m_nLineBegin = (m_nMaxLineCount - m_nLineCountCanShow) * nPos / 100  ;
	if (m_nLineBegin < 0)
		m_nLineBegin = 0;
	return 1;
}

void CDListBox::SetColor(DWORD dwNormalColor, DWORD dwSelColor) 
{ 
	m_dwTextColor = dwNormalColor;
	m_dwSelColor = dwSelColor;
}


void CDListBox::AddItem(int nX, int nY, DWORD dwNumber, bool singed)
{
    if ( nX >= 10 || nY >= 500 || nX < 0 || nY < 0 )
        return;

    RECT rcWnd = GetRECT();
    ClientToScreen(&rcWnd);
    m_nLineCountCanShow = (rcWnd.bottom - rcWnd.top)/m_nItemHeight;

    if ((nY+1) > m_nMaxLineCount)
        m_nMaxLineCount = nY+1;

    if ( IsLineEmpty(nY) )
        m_nLineCount++;

    if (m_sItem[nX][nY] == NULL)
    {
		m_sItem[nX][nY] = new char[ITEMSTRING_MAXLEN];
    }

    if (m_sItem[nX][nY])
    {
        _snprintf( m_sItem[nX][nY], ITEMSTRING_MAXLEN-1, ( singed ? "%d" : "%u" ), dwNumber );
    }

    m_bShowSlideBar = m_nMaxLineCount * m_nItemHeight > (rcWnd.bottom - rcWnd.top);
    m_pSlideBar->Show(m_bShowSlideBar);
}

void CDListBox::setItem( int x, int y, LPCSTR value )
{
    if ( x >= 10 || y >= 500 || x < 0 || y < 0 )
        return;
	
	if( m_sItem[x][y] == NULL )
		m_sItem[x][y] = new char[ITEMSTRING_MAXLEN];

	dwt::strcpy( m_sItem[x][y], value, ITEMSTRING_MAXLEN-1 );
}

void CDListBox::AddItem(int nX, int nY, Sprite *pSprItem)
{
    if ( nX >= 10 || nY >= 500 || nX < 0 || nY < 0 )
        return;

    if ( pSprItem == NULL )
        return;

    RECT rcWnd = GetRECT();
    ClientToScreen(&rcWnd);
    m_nLineCountCanShow = (rcWnd.bottom - rcWnd.top)/m_nItemHeight;

    if ((nY+1) > m_nMaxLineCount)
        m_nMaxLineCount = nY+1;

    if ( IsLineEmpty(nY) )
        m_nLineCount++;

    m_pSprItem[nX][nY] = pSprItem;

    m_bShowSlideBar = m_nMaxLineCount * m_nItemHeight > (rcWnd.bottom - rcWnd.top);
    m_pSlideBar->Show(m_bShowSlideBar);
}

BOOL CDListBox::IsLineEmpty(int nY)
{
    if ( nY >= 500 || nY < 0 )
        return FALSE;

    for ( int i = 0; i < 10 ; i++ )
    {
        if ( m_sItem[i][nY] != NULL || m_pSprItem[i][nY] != NULL )
            return FALSE;
    }

    return TRUE;
}

int CDListBox::GetPosOffestX( int nX )
{
    if ( nX <= 0 || nX >= 10 )
        return 0;

    int nOffestX = 0;
    for ( int i = 0; i < nX; i++ )
        nOffestX += m_nItemWidth[i];

    return nOffestX;
}

void CDListBox::setItemColor( int i, int j, DWORD color )
{
	if( i < 0 || i >= 10 ) return;
	if( j < 0 || j >= 500 ) return;

	m_sItemColor[i][j] = color;
}
// void CDListBox::SetSliderBarSpr(char* fileback, char* filenormal, char* fileactive, char* filedown)
// {
// 	if (m_pSlideBar)
// 		m_pSlideBar->SetBePicture(fileback,filenormal, fileactive, filedown);
// }