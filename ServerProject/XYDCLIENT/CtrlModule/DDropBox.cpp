/****************************************************************
//  FileName:   ddropbox.cpp
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
****************************************************************/
#include "StdAfx.h"

#include "ddropbox.h"
#include <Colors.h>

#include "pub\dwt.h"
extern CDFont	g_CommonFont12;

#define CLR_TIPTEXT g_RGB(222,222,222)
#define CLR_TIPBACK g_RGB(44,44,44)
//#define CLR_TIPTEXT g_RGB(22,22,222)
//#define CLR_TIPBACK g_RGB(205,205,0)
#define CLR_TIPTEXT_SHORTCUT g_RGB(205,205,0)

#ifdef _DEBUG

// follow codes is copy at crt/src/dbgint.h

/*
 * For diagnostic purpose, blocks are allocated with extra information and
 * stored in a doubly-linked list.  This makes all blocks registered with
 * how big they are, when they were allocated, and what they are used for.
 */

#define nNoMansLandSize 4

typedef struct _CrtMemBlockHeader
{
        struct _CrtMemBlockHeader * pBlockHeaderNext;
        struct _CrtMemBlockHeader * pBlockHeaderPrev;
        char *                      szFileName;
        int                         nLine;
#ifdef _WIN64
        /* These items are reversed on Win64 to eliminate gaps in the struct
         * and ensure that sizeof(struct)%16 == 0, so 16-byte alignment is
         * maintained in the debug heap.
         */
        int                         nBlockUse;
        size_t                      nDataSize;
#else  /* _WIN64 */
        size_t                      nDataSize;
        int                         nBlockUse;
#endif  /* _WIN64 */
        long                        lRequest;
        unsigned char               gap[nNoMansLandSize];
        /* followed by:
         *  unsigned char           data[nDataSize];
         *  unsigned char           anotherGap[nNoMansLandSize];
         */
} _CrtMemBlockHeader;

/*
bool cmpgap(void *p)
{
    __asm 
    {
        mov ecx, p
        mov eax, [ecx]
        cmp eax, 0xfdfdfdfd
        jne _rfalse

        mov eax, 1
        jmp _end

_rfalse:
        int 3
        mov eax, 0

_end:
    }
}

int _CrtCheckMem(const void *_memory, int _size)
{
    if (_memory == NULL)
        return 0;

    try
    {
        _CrtMemBlockHeader *pHeader = (_CrtMemBlockHeader *) 
            & ((const __int8 *)_memory) [- (__int32) sizeof(_CrtMemBlockHeader)];

        if (_size != 0)
        {
            if (_size != pHeader->nDataSize)
            {
                assert(!"分配块大小不匹配");
                return 0;
            }
        }

        unsigned char *gap = pHeader->gap;
        if (!cmpgap(gap))
        {
            assert(!"前边界溢出");
            return 0;
        }

        gap += pHeader->nDataSize + nNoMansLandSize;

        if (!cmpgap(gap))
        {
            assert(!"后边界溢出");
            return 0;
        }
    }
    catch (...)
    {
        assert(!"内存检测异常！！！");
    }

    return 1;
}
int _CrtCheckMemArray(const void *_memory, int _size)
{
    return _CrtCheckMem(((const __int8 *)_memory) - 4, _size + 4);
}
*/

#endif _DEBUG

CDDropBox::CDDropBox(void):CDFace(IID_CDDROPBOX)
{
    m_bUp = false;
    m_nItemCount = 0;
    m_bDropDown = false;
    m_nItemMouseOn = -1;
    m_nItemSel = 0;
    m_bDown = true;
    m_bPic = false;
    m_bPtInRect = false;
    for(int i = 0; i < 20; i ++)
    {
        m_cHotKey[i] = ' ';
        //m_sToolTip[i] = new char[100];
        m_sToolTip[i][0] = 0;
        //m_sItem[i] = new char[100];
        m_sItem[i][0] = 0;

        m_rcItem[i].left = 0;m_rcItem[i].right = 0;m_rcItem[i].top = 0;m_rcItem[i].bottom = 0;
        m_rcItems.left = 0;m_rcItems.right = 0;m_rcItems.top = 0;m_rcItems.bottom = 0;
    }
    m_bDrawFocusRect = false;

	m_bDrawNormalRect = false;

	m_bEnableHotKey = true;
}

CDDropBox::~CDDropBox(void)
{
 /*   for(int i = 0; i < 20; i ++)
    {
        delete[] m_sToolTip[i];
        delete[] m_sItem[i];
    }*/
}
bool CDDropBox::SetParameter(LPCSTR szParameterName, LPARAM lParam)
{
    char cItem[9] = {0};
    char cTmp = szParameterName[8];
    memcpy(cItem, szParameterName, 5);
    if(strcmp(cItem, "Item_") == 0)
        AddItem((LPSTR)lParam);

    memcpy(cItem, szParameterName, 8);
    if(strcmp(cItem, "ToolTip_") == 0)
        AddToolTip(atoi(&cTmp) - 1, (LPSTR)lParam);
    if(strcmp(szParameterName, "BePicture") == 0)
        if(strcmp((LPSTR)lParam, "true") == 0)
            SetBePic();
    return CDFace::SetParameter(szParameterName, lParam);
}

int	CDDropBox::OnCreate(LPARAM lParam)
{
    return 1;
}

int	CDDropBox::InitByBinary(LPARAM lParam)
{
    return 1;
}

int	CDDropBox::KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y)
{
    if(m_nItemCount == 0)
        return 0;
    RECT rcWnd = GetRECT();
    ClientToScreen(&rcWnd);

    POINT pt;
    pt.x = x, pt.y = y;

    if(iKeyState == KEY_UP)
    {
        if((m_nItemMouseOn >= 0) && m_bDropDown)
        {
            if((vKey == VK_ESCAPE) || (vKey == '+'))
            {
                ReleaseCapture();
                m_bDropDown = false;
                m_bPtInRect = false;
            }
            else
			{
				if (!(((vKey >= 0x30) && (vKey <= 0x39)) || ((vKey >= 0x41) && (vKey <= 0x5B)) || vKey == 32 || ((vKey >= 96) && (vKey <= 105))))
				{
					return 1;
				}
				for (int i = 0; i < 20; i ++)
				{
					if (vKey == GetHotKey(i))
						SetHotKey(i , ' ');
				}
                SetHotKey(m_nItemMouseOn, vKey);
			}
        }
    }
 

    if(iKeyState == MS_LDOWN)
    {
        //在此处计算各个矩形的位置是因为矩形数据只受鼠标单击影响
        m_rcItems.left = rcWnd.left;
        m_rcItems.right = rcWnd.right;
        
		m_bPtInRect = true;

        if(((rcWnd.bottom + (rcWnd.bottom - rcWnd.top) * m_nItemCount)  > g_pCanvas->m_nHeight) || (m_bUp))
        {
            m_bDown = false;
            m_rcItems.top = rcWnd.top - (m_bDrawFocusRect?m_nItemCount:m_nItemCount/2)*(rcWnd.bottom - rcWnd.top);
            m_rcItems.bottom = rcWnd.top;
            for(int i = 0; i < m_nItemCount; i ++)
            {
                m_rcItem[i].left = rcWnd.left;
                m_rcItem[i].right = rcWnd.right;
                m_rcItem[i].top = rcWnd.top - (i +1) * (rcWnd.bottom - rcWnd.top);
                m_rcItem[i].bottom = rcWnd.top - i*(rcWnd.bottom - rcWnd.top);  
            }
        }
        else
        {
            m_bDown = true;
            m_rcItems.top = rcWnd.bottom;
            m_rcItems.bottom = rcWnd.bottom + (m_bDrawFocusRect?m_nItemCount:m_nItemCount/2)*(rcWnd.bottom - rcWnd.top);
            for(int i = 0; i < m_nItemCount; i ++)
            {
                m_rcItem[i].left = rcWnd.left;
                m_rcItem[i].right = rcWnd.right;
                m_rcItem[i].top = rcWnd.bottom + i * (rcWnd.bottom - rcWnd.top);
                m_rcItem[i].bottom = rcWnd.bottom + (i + 1)*(rcWnd.bottom - rcWnd.top);  
            }
        }
  
        //
        SetCapture();

        if(PtInRect(&rcWnd, pt))
        {
            m_bDropDown = !m_bDropDown;
            //if(m_bDropDown)
        }
        else//  if(!PtInRect(&rcItems, pt))
        {
         //   ReleaseCapture();
         //   m_bDropDown = false;
        }
    }
    if(iKeyState == MS_LUP)
    {
        if(PtInRect(&m_rcItems, pt) &&(m_nItemMouseOn >= 0))
        {
            m_nItemSel = m_nItemMouseOn;
            OnClicked(m_nItemSel);
            m_bDropDown = false;
            ReleaseCapture();
        }
        else if((!PtInRect(&m_rcItems, pt)) &&(!PtInRect(&rcWnd, pt)))
        {
            ReleaseCapture();
            m_bDropDown = false;
            m_bPtInRect = false;
        }
        else if(!m_bDropDown)
                    ReleaseCapture();

        m_nItemMouseOn = -1;
        m_bPtInRect = false;
    }
    if(iKeyState == MS_MOVE)
    {
        //Active();
        m_bPtInRect = true;
        m_nItemMouseOn = -1;
        for(int i = 0; i < m_nItemCount; i ++)
        {
            if(PtInRect(&m_rcItem[i], pt))
            {
                m_nItemMouseOn = i;
                break;
            }
        }
        m_x = x;
        m_y = y;
    }

   return 1;
}
 
void CDDropBox::OnHotKey(int vKey)
{
    if((vKey == ' ') || !m_bEnableHotKey) 
        return;
    for(int i = 0; i < ((!m_bDrawFocusRect||!m_bPic)?m_nItemCount/2:m_nItemCount); i ++)
        if(vKey == GetHotKey(i))
        {
            SetItemSelect(i);
            break;
        }
}

void CDDropBox::Update()
{
    if(m_nItemCount == 0)
        return;
    DWORD dwClrOld = m_Font.GetColor();
    m_Font.SetColor(222,222,122);
    RECT rcWnd = GetRECT();
    ClientToScreen(&rcWnd);
    m_rcItems.left = rcWnd.left;
    m_rcItems.right = rcWnd.right;
    if(((rcWnd.bottom + (rcWnd.bottom - rcWnd.top) * m_nItemCount)  > g_pCanvas->m_nHeight) || (m_bUp))
    {
        m_bDown = false;
        m_rcItems.top = rcWnd.top - (m_bDrawFocusRect?m_nItemCount:m_nItemCount/2)*(rcWnd.bottom - rcWnd.top);
        m_rcItems.bottom = rcWnd.top;
        for(int i = 0; i < m_nItemCount; i ++)
        {
            m_rcItem[i].left = rcWnd.left;
            m_rcItem[i].right = rcWnd.right;
            m_rcItem[i].top = rcWnd.top - (i +1) * (rcWnd.bottom - rcWnd.top);
            m_rcItem[i].bottom = rcWnd.top - i*(rcWnd.bottom - rcWnd.top);  
        }
    }
    else
    {
        m_bDown = true;
        m_rcItems.top = rcWnd.bottom;
        m_rcItems.bottom = rcWnd.bottom + (m_bDrawFocusRect?m_nItemCount:m_nItemCount/2)*(rcWnd.bottom - rcWnd.top);
        for(int i = 0; i < m_nItemCount; i ++)
        {
            m_rcItem[i].left = rcWnd.left;
            m_rcItem[i].right = rcWnd.right;
            m_rcItem[i].top = rcWnd.bottom + i * (rcWnd.bottom - rcWnd.top);
            m_rcItem[i].bottom = rcWnd.bottom + (i + 1)*(rcWnd.bottom - rcWnd.top);  
        }
    }

    if(!m_bPic)
    {
        g_pCanvas->ClearAlpha(rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, g_RGB(255,255,0), 22);
        DrawRect(&rcWnd, 333, 3333);
        m_Font.SetLine(100, (rcWnd.bottom - rcWnd.top));
    }
    if(m_bDropDown)
    {
        if(!m_bPic)
            g_pCanvas->ClearAlpha(m_rcItems.left, m_rcItems.top, m_rcItems.right - m_rcItems.left, m_rcItems.bottom - m_rcItems.top, g_RGB(255,0,255), 11);
        for(int i = 0; i < (m_bDrawFocusRect?m_nItemCount:(m_nItemCount/2)); i ++)
        {
            if(!m_bPic)
                m_Font.DrawText(m_rcItem[i].left, m_rcItem[i].top, m_sItem[i]);
            else
			{
                m_spr[i].DrawAlpha(m_rcItem[i].left, m_rcItem[i].top, 0);

				if (m_bDrawNormalRect)
					DrawRect(&m_rcItem[i], g_RGB(222, 222, 22), g_RGB(222, 222, 22));
			}

        }
        if(!m_bPic)
            DrawRect(&m_rcItems, 5555, 5555);
    }
    if((m_nItemMouseOn >= 0) && m_bDropDown)
    {
        if(!m_bPic ||m_bDrawFocusRect)
            DrawRect(&m_rcItem[m_nItemMouseOn], 63333, 66666);
        else
        {
            if(!m_bDrawFocusRect)
                m_spr[m_nItemCount/2 + m_nItemMouseOn].DrawAlpha(m_rcItem[m_nItemMouseOn].left, m_rcItem[m_nItemMouseOn].top, 0);
            else
                m_spr[m_nItemMouseOn].DrawAlpha(m_rcItem[m_nItemMouseOn].left, m_rcItem[m_nItemMouseOn].top, 0);
        }
    }

    if(!m_bPic)
        m_Font.DrawText(rcWnd.left, rcWnd.top, m_sItem[m_nItemSel]);
    else if(m_nItemSel >= 0)	//画图片
        m_spr[m_nItemSel].DrawAlpha(rcWnd.left, rcWnd.top, 0);


    m_Font.SetColor(dwClrOld);
}

void CDDropBox::UpdateTip()
{
    RECT rcWnd = GetRECT();
    ClientToScreen(&rcWnd); 

    DWORD dwClrOld = m_Font.GetColor();
    int nHeightOld = m_Font.GetLineHeight();
    int nWidthOld  = m_Font.GetLineWidth(); 
    m_Font.SetLine(240, 18); 

    m_Font.SetColor(CLR_TIPTEXT);

    int nWidth = dwt::strlen(m_sToolTip[m_nItemMouseOn], 100) * m_Font.GetWidth()/2 + 5;
    int nHeight =  m_Font.GetHeight() + 4; 

    if (m_x + 20 + nWidth > g_pCanvas->m_nWidth) 
        m_x = ( g_pCanvas->m_nWidth - 20 )-nWidth;
	else if (m_x - nWidth/2 < 0)
		m_x = nWidth/2;

    RECT rcToolTip; 
    rcToolTip.left = m_x + 20; rcToolTip.top = m_y - nHeight - 2; rcToolTip.right = m_x + 20 + nWidth; rcToolTip.bottom = m_y;

    if(!m_bPtInRect)
        return;

    if((m_nItemMouseOn >= 0) && m_bDropDown && (strlen(m_sToolTip[m_nItemMouseOn]) > 0)) 
    {
        if((m_cHotKey[m_nItemMouseOn] != ' ') && (m_cHotKey[m_nItemMouseOn] != '+'))
        {
            rcToolTip.top = m_y - 38; 
            if(nWidth < (9*m_Font.GetWidth()/2+5)) 
            {
                nWidth = 9*m_Font.GetWidth()/2+5;
                rcToolTip.right = rcToolTip.left + nWidth;
            }
            ClearAlphaWithBord(m_x + 18, m_y - 39 + 11, nWidth, 42, CLR_TIPBACK, 8);
        }
        else
		{
            ClearAlphaWithBord(m_x + 18, m_y - nHeight - 1 + 11, nWidth, nHeight + 4, CLR_TIPBACK, 8);
		}
    }
    else if((m_nItemSel>=0) && (m_sToolTip[m_nItemSel][0] != 0 ) && !m_bDropDown)
    { 
        nWidth = dwt::strlen(m_sToolTip[m_nItemSel], 100) * m_Font.GetWidth()/2 + 5;

        if (m_x + nWidth/2 > g_pCanvas->m_nWidth)
            m_x = g_pCanvas->m_nWidth - nWidth/2;
		else if (m_x - nWidth/2 < 0)
			m_x = nWidth/2;

        ClearAlphaWithBord(m_x - nWidth/2 - 3, m_y - nHeight - 2, nWidth, nHeight + 4, CLR_TIPBACK, 8);
    }


    if((m_nItemMouseOn >= 0) && m_bDropDown) 
        if(strlen(m_sToolTip[m_nItemMouseOn]) > 0)
        {
            m_Font.DrawText(m_x + 21, m_y - nHeight + 3 + 11, m_sToolTip[m_nItemMouseOn]);

            if((m_cHotKey[m_nItemMouseOn] != ' ') && (m_cHotKey[m_nItemMouseOn] != '+'))
            {
                char sHotKey[13];

				if(m_cHotKey[m_nItemMouseOn] >= 96 && m_cHotKey[m_nItemMouseOn] <= 105) //小键盘数字
				{
					_snprintf( sHotKey, 13, "快捷键(Num%c)", GetHotKey(m_nItemMouseOn) - 48 );
				}
				else
				{
					_snprintf( sHotKey, 13, "快捷键(%c)", GetHotKey(m_nItemMouseOn) );
				}

				sHotKey[12] = 0;
                int nLen = dwt::strlen(m_sToolTip[m_nItemMouseOn], 100)*g_CommonFont12.GetWidth();
                m_Font.DrawText(m_x + 23, m_y - 36 + 11, sHotKey);
            }

            //DrawRect(&rcToolTip, 3333, 6666);
        }

    if(m_nItemSel>=0)
    if(m_bPtInRect && m_sToolTip[m_nItemSel][0] != 0 && !m_bDropDown) 
    {
        nWidth = dwt::strlen(m_sToolTip[m_nItemSel], 100) * m_Font.GetWidth()/2 + 5;
        m_Font.SetColor(CLR_TIPTEXT_SHORTCUT);

        if (m_x + nWidth/2 > g_pCanvas->m_nWidth)
            m_x = g_pCanvas->m_nWidth - nWidth/2;
		else if (m_x - nWidth/2 < 0)
			m_x = nWidth/2;
		//技能提示
        m_Font.DrawText(m_x - nWidth/2 , m_y - nHeight + 2, m_sToolTip[m_nItemSel]);

        rcToolTip.top = m_y - nHeight - 2;
        rcToolTip.left = m_x - nWidth/2 - 2;
        rcToolTip.right = m_x + nWidth/2 + 2;
        //DrawRect(&rcToolTip, 3333, 6666);
    }
    m_Font.SetColor(dwClrOld);
    m_Font.SetLine(nWidthOld, nHeightOld);
}

void CDDropBox::SetItemSelect(int nSel, bool bOnClick) 
{ 
	m_nItemSel = nSel; 
	if (bOnClick) 
		OnClicked(m_nItemSel);
}

void CDDropBox::OnClicked(int nSel)
{
    m_bPtInRect = false;
    GetParent()->OnMyMessage(this, nSel);
}

int CDDropBox::AddItem(LPSTR sItem)
{
    if(dwt::strlen(sItem, 100) >= 99)
    {
        MessageBox(NULL, "Error", "string too long", MB_OK);
        return -1;
    }

    if (!m_bPic)
    {
            dwt::strcpy(m_sItem[m_nItemCount], sItem, 100);
    }
    else
        m_spr[m_nItemCount].Load(sItem);

    return m_nItemCount ++;
}

void CDDropBox::SetItem(int nIndex, LPSTR sItem)
{
    if (nIndex < 0)
        return;

    if (nIndex >= m_nItemCount)
        return;

    if(!m_bPic)
    {
        if(dwt::strlen(sItem, 100) >= 100)
        {
            MessageBox(NULL, "Error", "string too long", MB_OK);
        _asm int 3
            return;
        }
        else
            dwt::strcpy(m_sItem[nIndex], sItem, 100);
    }
    else
        m_spr[nIndex].Load(sItem);
}

void CDDropBox::RemoveAll()
{
    for(int i = 0; i < m_nItemCount; i ++)
    {
        if(!m_bPic)
            memset(m_sItem[i], 0, 100);
        else
            m_spr[i].Load("");
        memset(m_sToolTip[i], 0, 100);
    }
    m_nItemCount = 0;
    m_nItemSel = 0;
}

void  CDDropBox::RemoveByIndex(int nIndex)
{
    if ( nIndex < 0 || nIndex >= m_nItemCount )
        return;

    if ( nIndex + 1 == m_nItemCount )
    {
        m_cHotKey[19] = ' ';
        m_sToolTip[19][0] = 0;
        m_sItem[19][0] = 0;
        m_spr[19].Load("");
    }
    else
    {       
        memcpy( m_sItem[nIndex] , m_sItem[nIndex + 1], (20 - nIndex - 1) * 100 );
        memcpy( &(m_cHotKey[nIndex]) , &(m_cHotKey[nIndex + 1]), (20 - nIndex - 1) );
        memcpy( &(m_spr[nIndex]) , &(m_spr[nIndex + 1]), (20 - nIndex - 1) * sizeof( Sprite ) );
        memcpy( m_sToolTip[nIndex] , m_sToolTip[nIndex + 1], (20 - nIndex - 1) * 100 );

        m_cHotKey[19] = ' ';
        m_sToolTip[19][0] = 0;
        m_sItem[19][0] = 0;
        m_spr[19].Load("");
    }   
    m_nItemCount--;
    m_nItemSel = 0;
}

void CDDropBox::OnMouseLeave()
{
    m_bPtInRect = false;
}

void CDDropBox::SetHotKey(int nIndex, char cHotKey) 
{ 
	if (!m_bEnableHotKey)
		return;

    if((nIndex > ((!m_bDrawFocusRect||!m_bPic)?m_nItemCount/2:m_nItemCount)) || (nIndex < 0) || (cHotKey <= 0)) return;
    m_cHotKey[nIndex] = cHotKey;
}

char CDDropBox::GetHotKey(int nIndex)
{
    if((nIndex > ((!m_bDrawFocusRect||!m_bPic)?m_nItemCount/2:m_nItemCount)) || (nIndex < 0)) return ' '; 
    return m_cHotKey[nIndex];
}

void CDDropBox::AddToolTip(int nIndex, char* sToolTip)
{
    if (nIndex < 0)
        return;

    if (nIndex >= m_nItemCount)
        return;

    if((dwt::strlen(sToolTip, 100) >= 99))
    {
        MessageBox(NULL, "Error", "string too long", MB_OK);
        _asm int 3
        return;
    }

    dwt::strcpy(m_sToolTip[nIndex], sToolTip, 100);
}

int CDDropBox::GetSelItem()
{
    return m_nItemSel;
}
LPSTR CDDropBox::GetSelItemString()
{
	if ((m_nItemSel >= 0) && (m_nItemSel < m_nItemCount))
		return m_sItem[m_nItemSel];
	return "";
}

void CDDropBox::ForceClearHotKey()
{
	memset(m_cHotKey,0x20,20);
	return;
}

void CDDropBox::GetSkillIDList(int iID,int iX,int iY)
{
// 	std::list<Sprite>	pSkillList;
// 	
// 	for(int i = 0;i < 20; ++i)
// 	{
// 		pSkillList.push_back(m_spr[i]);
// 	}
	  m_spr[iID].DrawAlpha(iX, iY, 0);
	//return pSkillList;
}
