/****************************************************************
//  FileName:   DButton.cpp
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
****************************************************************/
#include "StdAfx.h"
#include "interface/dinterfacemanager.h"
#include "MainWorld\DMapArea.h"
#include "MainWorld\LifeManager.h"
#include "MainWorld\DFightDateC.h"
#include "MainWorld\DItemList.h"
#include "MainWorld\DGMManager.h"

#include "dedit.h"
#include <Colors.h>

#define CURSOR_FLASHCOUNT 10
#define CLR_TIPTEXT g_RGB(222,222,222)
#define CLR_TIPBACK g_RGB(44,44,44)

#include "pub/simpleedit.cpp"
#include "pub/dwt.h"
#include <list>

extern HWND g_hMainWnd;

extern bool useBIG5;

extern BOOL GBKtoBIG5( std::string &result, LPCSTR str );
extern BOOL BIG5toGBK( std::string &result, LPCSTR str );

extern BOOL UNICODEtoGBK( std::string &result, LPCWSTR wstr );
extern BOOL GBKtoUNICODE( std::basic_string< wchar_t > &result, LPCWSTR wstr );

extern BOOL GBK_simple_to_traditional( std::string &result, LPCSTR str, BOOL useColor = false);

CDEdit::CDEdit(int fontsize) : 
    CDFace(IID_CDEDIT, fontsize)
{
    m_FirstChar = 0;
    m_bNumOnly = false;
    m_nLen = 0;
    m_nCurPos = 0;
    m_bShowCursor = false;
    m_nEnd = 0;
    m_nFirst = m_nLast = 0;
    m_bShiftDown = false;
    m_bMask = false;
    m_nCursorClr = 0;
    m_bShowBord = true;
    m_iFlashCount = 0;

    ZeroMemory(m_cBuff, MAX_LEN);

    m_nMaxLen = 0;
    edit_init(m_nLen, m_nCurPos, m_InputState, m_nMaxLen, m_cBuff);

    m_bEnable = true;

    m_bCapture = false;

	m_nR = m_nG = m_nB = 222;

	m_dwNumMax = 0xffffffff;

	m_bInRect = m_bShowToolTip = false;
	memset(m_sToolTip, 0, 31);

	m_bMultiline = true;
	m_nShowCur = 0;
    it = bufex.end();
}

CDEdit::~CDEdit(void)
{
}

int	CDEdit::InitByBinary(LPARAM lParam)
{
    return 1;
}
   
bool CDEdit::SetParameter(LPCSTR szParameterName, LPARAM lParam)
{
	if (strcmp(szParameterName, "ToolTip") == 0)
		AddToolTip((LPSTR)lParam);
    // 设置编辑筐的类型 缺省时候为多行
	else if( strcmp(szParameterName, "Multiline") == 0 )
	{
		if ( stricmp( (LPSTR)lParam, "false" ) == 0 )
            m_bMultiline = false;
        else if ( stricmp( (LPSTR)lParam, "true" ) == 0 )
            m_bMultiline = true;
	}
	// 允许输入的字符串长度
	else if( strcmp(szParameterName, "MaxLine") == 0 )
    {
		m_nMaxLen = atoi( ( LPSTR )lParam );
        m_nMaxLen = m_nMaxLen < MAX_LEN ? m_nMaxLen : 0;
    }
	
    return CDFace::SetParameter(szParameterName, lParam);
}

int	CDEdit::OnCreate(LPARAM lParam)
{
    if (lParam == 1)
        EDIT_SET_STATE(m_InputState, EDIT_FLAG_SUPPORT_ENTER);

    RECT rc = GetRECT();

    int nLinecount = 1;
    if ( m_bMultiline )
       nLinecount = (rc.bottom - rc.top) / m_Font.GetHeight();

    if ( m_nMaxLen == 0 )
    {
        m_nMaxLen = ((rc.right - rc.left) / (m_Font.GetWidth()/2)) * nLinecount + 1;
        if (m_nMaxLen > MAX_LEN) 
            m_nMaxLen = 0;
    }

    return 1;
}

void CDEdit::SetRECT(int left, int top, int right, int bottom)
{
    int nLinecount = 1;
    if ( m_bMultiline )
	    nLinecount = (bottom - top) / m_Font.GetHeight();
	if (nLinecount < 1)
		nLinecount = 1;

    m_nMaxLen = ((right - left) / (m_Font.GetWidth()/2)) * nLinecount + 1;
    if (m_nMaxLen > MAX_LEN) 
        m_nMaxLen = 0;

	CDFace::SetRECT(left, top, right, bottom);
}

void CDEdit::SetFontSize(int size)
{
    CDFace::SetFontSize(size);

    RECT rc = GetRECT();

    if ( m_bMultiline )
        m_nMaxLen = ((rc.right - rc.left) / (m_Font.GetWidth()/2)) * ((rc.bottom - rc.top) / m_Font.GetHeight()) + 1;
    if (m_nMaxLen > MAX_LEN) 
        m_nMaxLen = 0;
}

int CDEdit::KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y)
{
    if ( !m_bEnable )
        return 1;

    RECT rc = GetRECT();
    ClientToScreen(&rc); 
	
	m_x = x;
	m_y = y;

    unsigned int row = 0, col = 0;
    unsigned int maxrow = 0, maxcol = 0;
    maxcol = (rc.right - rc.left)/(m_Font.GetWidth()/2);
    maxrow = (rc.bottom - rc.top)/m_Font.GetHeight();

    if(iKeyState >= MS_BEGIN)
    {
        if(iKeyState == MS_LBUTTONDBLCLK)
        {
            m_nCurPos = m_nLen; 
            Select(0, m_nCurPos); 
        //    Clear();
        }
        else if (iKeyState == MS_LDOWN)
        {
            Select(0, 0); 

            bool bAddOne = (2*(x - rc.left)%m_Font.GetWidth()) > (m_Font.GetWidth()/2);
            int nPosTo = 2*(x - rc.left)/m_Font.GetWidth() + (bAddOne?1:0) + (int)m_nShowCur;
            if (edit_iswordcode(m_nLen, nPosTo-1, m_nMaxLen, m_cBuff) == EDIT_ISWORDCODE)
            {
                nPosTo += 1;
            }

            nPosTo = nPosTo < 0 ? 0 : nPosTo;
            m_nCurPos = nPosTo> (int)m_nLen ? m_nLen : nPosTo;
            m_nEnd = m_nCurPos;

            SetCapture();
            m_bCapture = true;
        }
        else if (iKeyState == MS_MOVE)
        {
            if (m_bCapture)
            {
                bool bAddOne = (2*(x - rc.left)%m_Font.GetWidth()) > (m_Font.GetWidth()/2);
                int nPosTo = 2*(x - rc.left)/m_Font.GetWidth() + (bAddOne?1:0) + (int)m_nShowCur;
                if (edit_iswordcode(m_nLen, nPosTo-1, m_nMaxLen, m_cBuff) == EDIT_ISWORDCODE)
                {
                    nPosTo += 1;
                }
#pragma warning( disable : 4018 )
                nPosTo = nPosTo < 0 ? 0 : nPosTo;
                if ( m_nShowCur > 0 && nPosTo < m_nShowCur)
                    m_nShowCur--;
                if ( (nPosTo > m_nShowCur + maxcol) && (m_nLen > maxcol) && (m_nShowCur < m_nLen - maxcol) )
                    m_nShowCur++;

                m_nCurPos = (nPosTo > m_nLen) ? m_nLen : nPosTo;
                
                if (m_nCurPos > m_nEnd)
                    Select(m_nEnd, m_nCurPos);
                else
                    Select(m_nCurPos, m_nEnd);
            }

			m_bInRect = true;
			m_bShowToolTip = true;
        }
        else if (iKeyState == MS_LUP)
        {
            if (m_bCapture)
            {
                bool bAddOne = (2*(x - rc.left)%m_Font.GetWidth()) > (m_Font.GetWidth()/2);
                int nPosTo = 2*(x - rc.left)/m_Font.GetWidth() + (bAddOne?1:0) + (int)m_nShowCur;
                if (edit_iswordcode(m_nLen, nPosTo-1, m_nMaxLen, m_cBuff) == EDIT_ISWORDCODE)
                {
                    nPosTo += 1;
                }

                nPosTo = nPosTo < 0 ? 0 : nPosTo;
                m_nCurPos = (nPosTo > m_nLen) ? m_nLen : nPosTo;

                if (m_nCurPos > m_nEnd)
                    Select(m_nEnd, m_nCurPos);
                else
                    Select(m_nCurPos, m_nEnd);

                m_bCapture = false;
                ReleaseCapture();
            }     
        }
        return 0;
    }

    switch(iKeyState)
    {
    case KEY_CHAR:
        {
            switch (vKey)
            {
            case VK_CANCEL: // ctrl + c
            case VK_FINAL: // ctrl + x
                if (!m_bMask)
                    if (m_nFirst != m_nLast)
                    {
                        HGLOBAL hData;
                        char* pData;
                        long len = m_nLast - m_nFirst;
                        if (len == 0) 
                            return TRUE;

                        OpenClipboard(g_hMainWnd);
                        EmptyClipboard();
                        hData = GlobalAlloc(GMEM_FIXED, len+10);
                        pData = (char *)GlobalLock(hData);

                        memcpy(pData, &m_cBuff[m_nFirst], len);
                        pData[len] = 0;

                        // 这里做检测，如果当前状态为繁体，则将内部的简体码做一次转换
                        if ( useBIG5 )
                        {
                            std::string result;
                            GBKtoBIG5( result, pData );
                            memcpy( pData, result.c_str(), result.size() + 1 );
                        }

                        GlobalUnlock(hData);
                        SetClipboardData(CF_TEXT, hData);
                        CloseClipboard();

                        if ( vKey == VK_FINAL )
                        {
                            Delete(m_nFirst, m_nLast);
                            m_nFirst = m_nLast = 0;
                        }
                    }
                    return 1;

            case 0x16: // ctrl + v
                if (IsClipboardFormatAvailable(CF_UNICODETEXT))
                {
                    OpenClipboard(g_hMainWnd);
                    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
                    int len = (int)GlobalSize(hData);

                    wchar_t *pBuf = (wchar_t*)GlobalLock(hData);
                    // len = dwt::strlen(pBuf, len) + 1;

                    std::string result;
                    UNICODEtoGBK( result, pBuf );

                    len = (int)result.length() + 1;

                    ConverPos2RC(m_nLen, m_nLen, m_nMaxLen, m_cBuff, col, row, maxcol, maxrow);

                    unsigned int maxlen = m_nLen + (maxrow - (row+1)) * maxcol + (maxcol - col) + 1;
                    if (maxlen > m_nMaxLen) 
                        maxlen = m_nMaxLen;

                    if ((m_nFirst == m_nLast) && (m_nFirst != m_nCurPos))
                        m_nFirst = m_nLast = m_nCurPos;

                    // 这里需要在粘贴前判断：
                    // 如果当前状态为繁体，则将繁体数据转为简体后再粘贴
                    //if ( useBIG5 )
                    //{
                    //    GBK_simple_to_traditional( &result, result.c_str() );

                    //    edit_paste(m_nLen, m_nCurPos, maxlen, m_cBuff, m_nFirst, m_nLast, result.size() + 1, (LPSTR)result.c_str() );
                    //}
                    //else
                    if ( !m_bMultiline )
                        edit_paste(m_nLen, m_nCurPos, m_nMaxLen, m_cBuff, m_nFirst, m_nLast, len, (LPSTR)result.c_str() );
                    else
                        edit_paste(m_nLen, m_nCurPos, maxlen, m_cBuff, m_nFirst, m_nLast, len, (LPSTR)result.c_str() );

                    if ( m_nLen > maxcol && !m_bMultiline )
                        m_nShowCur = m_nLen - maxcol;
                    m_nFirst = m_nLast = 0;

                    GlobalUnlock(hData);
                    CloseClipboard();
                }
                return 1;
            }

            bool bSingleDel = true;   // 单个删除
            if (m_nFirst != m_nLast)
            {
                bSingleDel = false;
                Delete(m_nFirst, m_nLast);
                m_nFirst = m_nLast = 0;
            }

            if (!bSingleDel && vKey == 8)        //非单个删除跳过下面 vKey == 8 删除键
                break;

            if ( m_FirstChar == 0 && vKey < 0x80 )
            {
				if ( m_bMask ) AddCharEx( vKey ); else AddChar( vKey );
            }
            else if ( m_FirstChar == 0 )
            {
                m_FirstChar = vKey;
            }
            else
            {
                // 双字节字符
                // 检测当前状态是否为繁体，是则将该字符转为简体后输入！
                if ( useBIG5 )
                {
                    char temp[4] = { (char)m_FirstChar, (char)vKey, 0, 0 };
                    std::string result;
                    BIG5toGBK( result, temp );
                    if ( m_bMask )
                        AddCharEx( ( ( result[1] & 0xff ) << 8 ) | ( result[0] & 0xff ) );
                    else
                    {
                        AddChar( result[0] & 0xff );
                        AddChar( result[1] & 0xff );
                    }
                }
                else
                {
                    if ( m_bMask )
                        AddCharEx( ( vKey << 8 ) | m_FirstChar );
                    else 
                    {
                        AddChar( m_FirstChar );
                        AddChar( vKey );
                    }
                }

                m_FirstChar = 0;
            }

            m_iFlashCount = 0;
            break;
        }
    case KEY_HOLD:
    case KEY_DOWN:
        if ( !m_bMask )
        {
            if((vKey == VK_LEFT) && (m_nCurPos > 0))
            {
                if ( m_nCurPos > m_nShowCur )
                {
                    switch (edit_iswordcode(m_nLen, m_nCurPos-1, m_nMaxLen, m_cBuff))
                    {
                    case EDIT_ISTERMINATE:
                        break;
                    case EDIT_ISENDL:
                        m_nCurPos --;
                        break;
                    case EDIT_ISASCII:
                        m_nCurPos --;
                        break;
                    case EDIT_ISWORDCODE:
                        m_nCurPos --;
                        break;
                    case EDIT_ISWORDCODECENTER:
                        m_nCurPos -=2;
                        break;
                    }
                }
                else
                {
                    switch (edit_iswordcode(m_nLen, m_nCurPos-1, m_nMaxLen, m_cBuff))
                    {
                    case EDIT_ISTERMINATE:
                        break;
                    case EDIT_ISENDL:
                        m_nCurPos --;
                        m_nShowCur--;
                        break;
                    case EDIT_ISASCII:
                        m_nCurPos --;
                        m_nShowCur--;
                        break;
                    case EDIT_ISWORDCODE:
                        m_nCurPos --;
                        m_nShowCur--;
                        break;
                    case EDIT_ISWORDCODECENTER:
                        m_nCurPos -=2;
                        m_nShowCur-=2;
                        break;
                    }
                }

                if(m_bShiftDown)
                {
                    Select(m_nCurPos, m_nEnd);
                }
                else
                    Select(0, 0);
            }
            else if((vKey == VK_RIGHT) && (m_nCurPos < m_nLen))
            {
                if ( m_nLen < m_nShowCur + maxcol || m_nCurPos < maxcol + m_nShowCur)
                {
                    switch (edit_iswordcode(m_nLen, m_nCurPos, m_nMaxLen, m_cBuff))
                    {
                    case EDIT_ISTERMINATE:
                        break;
                    case EDIT_ISENDL:
                        m_nCurPos ++;
                        break;
                    case EDIT_ISASCII:
                        m_nCurPos ++;
                        break;
                    case EDIT_ISWORDCODE:
                        m_nCurPos +=2;
                        break;
                    case EDIT_ISWORDCODECENTER:
                        m_nCurPos ++;
                        break;
                    }
                }
                else
                {
                    switch (edit_iswordcode(m_nLen, m_nCurPos, m_nMaxLen, m_cBuff))
                    {
                    case EDIT_ISTERMINATE:
                        break;
                    case EDIT_ISENDL:
                        m_nCurPos ++;
                        m_nShowCur++;
                        break;
                    case EDIT_ISASCII:
                        m_nCurPos ++;
                        m_nShowCur++;
                        break;
                    case EDIT_ISWORDCODE:
                        m_nCurPos +=2;
                        m_nShowCur+=2;
                        break;
                    case EDIT_ISWORDCODECENTER:
                        m_nCurPos ++;
                        m_nShowCur++;
                        break;
                    }

                }
                if(m_bShiftDown)
                {
                    Select(m_nEnd, m_nCurPos);
                }
                else
                    Select(0, 0);
            }
            else if(vKey == VK_UP)
            {
                ConverPos2RC(m_nLen, m_nCurPos, m_nMaxLen, m_cBuff, col, row, maxcol, maxrow);
                if (row > 0)
                {
                    row--;
                    ConverRC2Pos(m_nLen, m_nCurPos, m_nMaxLen, m_cBuff, row, col, maxrow, maxcol);
                }

                Select(0, 0);
            }
            else if(vKey == VK_DOWN)
            {
                ConverPos2RC(m_nLen, m_nCurPos, m_nMaxLen, m_cBuff, col, row, maxcol, maxrow);
                if (row+1 < maxrow)
                {
                    row++;
                    ConverRC2Pos(m_nLen, m_nCurPos, m_nMaxLen, m_cBuff, row, col, maxrow, maxcol);
                }

                Select(0, 0);
            }
            else if (vKey == VK_DELETE)
            {
                if (m_nFirst != m_nLast) 
                {
                    Delete(m_nFirst, m_nLast);
                    m_nFirst = m_nLast = 0;
                }
                else
                {
                    Delete(m_nCurPos, m_nCurPos+1);
                }
            }
            else if((iKeyState == KEY_DOWN) && ( vKey == VK_SHIFT))
            {
                m_bShiftDown = true;			
                m_nEnd = m_nCurPos;
            }
            else if(vKey == VK_HOME)
            {
                m_nCurPos = (unsigned int)m_nShowCur = 0;
                if(m_bShiftDown && m_nLen)
                {
                    Select( m_nCurPos, m_nEnd );
                }
                else
                    Select(0, 0);
            }
            else if(vKey == VK_END)
            {
                m_nCurPos =  m_nLen;
                m_nShowCur = (m_nLen >  maxcol) ? (m_nLen -  maxcol) : 0;
                if(m_bShiftDown && m_nLen)
                {
                    Select(m_nEnd, m_nCurPos);
                }
                else
                    Select(0, 0);
            }
            else if(vKey == VK_TAB)
            {
                //((CDFace*)(m_pParent->m_ChildsList.front() + 1))->Active();
            }
            else
            {
            }
            m_iFlashCount = 0;
        }
        else
        {
            if ( vKey == VK_LEFT && it != bufex.begin() )
                it --;
            else if ( vKey == VK_RIGHT && it != bufex.end() )
                it ++;
            else if ( vKey == VK_UP || vKey == VK_HOME )
                it = bufex.begin();
            else if ( vKey == VK_DOWN || vKey == VK_END )
                it = bufex.end();
            else if ( vKey == VK_DELETE )
                it = bufex.erase( it );

            m_iFlashCount = 0;
        }
        break;

    case KEY_UP:
        if (vKey == VK_SHIFT) 
            m_bShiftDown = false;
        break;

    default:
        break;
    }

	//if (m_bNumOnly)
	//	if (m_cBuff[0] == 0)
	//		SetString(0);

    return 1;
}

void CDEdit::AddCharEx( int ch )
{
    if ( ch == VK_RETURN )
        return;

    if ( ch == VK_BACK )
    {
        if ( it == bufex.begin() )
            return;

        it--;
        it = bufex.erase( it ); 
        return;
    }

	if(bufex.size() < m_nMaxLen - 1) // 与非密码输入状态保持一致，输入字符个数为maxlen - 1
		bufex.insert( it, ch ); 
}

void CDEdit::GetStringEx( char *pBuff, long size )
{
    if ( !m_bMask )
        dwt::strcpy( pBuff, m_cBuff, size );
    else
    {
        int count = 0;
        for ( std::list<wchar_t>::iterator it = bufex.begin(); it != bufex.end(); it ++ )
        {
            if ( ( *it & 0xff00 ) == 0 )
            {
                *( LPBYTE )( pBuff + count ) = ( BYTE )*it;
                count += 1;
            }
            else
            {
                *( LPWORD )( pBuff + count ) = *it;
                count += 2;
            }
        }
         pBuff[count] = 0;
    }
}

void CDEdit::SetTextColor(int nR, int nG, int nB)
{
    m_nR = nR;
    m_nG = nG;
    m_nB = nB;
}

void CDEdit::AddChar(int nChar)
{
	unsigned int row = 0, col = 0;
	unsigned int maxrow = 0, maxcol = 0;
	RECT rc = GetRECT();
	maxcol = (rc.right - rc.left)/(m_Font.GetWidth()/2);
	maxrow = (rc.bottom - rc.top)/m_Font.GetHeight();
	ConverPos2RC(m_nLen, m_nLen, m_nMaxLen, m_cBuff, col, row, maxcol, maxrow);

	if (!EDIT_IS_STATE(m_InputState, EDIT_FLAG_WAIT_SECOND_WORDCODE_BYTE))
	{
		if(m_bNumOnly)
		{
			if(((nChar < '0') || (nChar > '9')) && (nChar != VK_BACK)) 
				return;
        }

        unsigned int _row = 0, _col = 0;
        ConverPos2RC(m_nLen, m_nCurPos, m_nMaxLen, m_cBuff, _col, _row, maxcol, maxrow);

        // 是否是行数满了的情况
        if (row+1 >= maxrow)
        {
			if (nChar == VK_RETURN){
				m_nShowCur = 0;
                return;
			}
            if (nChar != VK_BACK)
            {
                unsigned len = edit_curlength(m_nLen, m_nCurPos, m_nMaxLen, m_cBuff);
                if (len != 0)
                {
                    if (m_nMaxLen == 0)
						return;
					len %= m_nMaxLen;
					if (len == 0)
						return;

					if (len + ((nChar < 0x80) ? 0 : 1) >= m_nMaxLen)
						return;
                }
            }
        }
    }

    size_t prdCurPos = m_nCurPos;
    if ( !m_bMultiline )
        edit_char(m_nLen, m_nCurPos, m_nMaxLen, m_cBuff, (char)nChar, m_InputState);
    else
        edit_char(m_nLen, m_nCurPos, m_nMaxLen < (maxrow * maxcol + 1) ? m_nMaxLen :(maxrow * maxcol + 1), m_cBuff, (char)nChar, m_InputState);

    if ( !m_bMultiline )
    {
        if ( m_nCurPos > prdCurPos)						  // 增加字
        {
            if ( m_nCurPos - m_nShowCur > maxcol )
                m_nShowCur += m_nCurPos - prdCurPos;
        }
        else if ( m_nCurPos < prdCurPos )				  // 删除字
        {
            if (m_nShowCur >= prdCurPos - m_nCurPos)
                m_nShowCur -= prdCurPos - m_nCurPos;
        }
    }

    if(m_bNumOnly)
    {
        if(((nChar < '0') || (nChar > '9')) && (nChar != VK_BACK)) 
            return;

        m_cBuff[ m_nLen ] = '\0';
		if ( ( atoi( m_cBuff ) > m_dwNumMax ) )
			SetString(m_dwNumMax);
		//if (m_cBuff[0] == 0)
		//{
		//	m_cBuff[0] = '0';
		//}
	}
}

void CDEdit::SetString( char *pBuff, long size) 
{
    if ( !m_bMask )
    {
        if (size > m_nMaxLen)
            size = m_nMaxLen;

        m_nCurPos = 0;
        edit_paste(m_nLen, m_nCurPos, m_nMaxLen, m_cBuff, 0, 
            m_nLen, dwt::strlen(pBuff, size)+1, pBuff);
    }
    else
    {
        if ( !bufex.empty() )
            bufex.clear();

        it = bufex.end();
        for ( int i=0; i<size; i++ )
        {
            if ( pBuff[i] == 0 )
                break;

            if ( ( ( ( LPBYTE )pBuff )[i] ^ 0x96 ) > 0x80 )
                AddCharEx( ( wchar_t& )pBuff[i++] ^ 0x9696 );
            else
                AddCharEx( ( ( LPBYTE )pBuff )[i] ^ 0x96 );
        }
    }
}

void CDEdit::SetString(DWORD dwNum)
{
	char cTxt[20];
	memset(cTxt, 0, 20);
	itoa(dwNum, cTxt, 10);

	int nCount = 0;
	while(cTxt[nCount++] != 0);

	SetString(cTxt, nCount);
}
    
void CDEdit::SetNumOnly(bool bNum) 
{ 
	m_bNumOnly = bNum; 
	if (0)//bNum)
	{
		m_nLen = 1;
		m_cBuff[0] = '0'; 
	}
}

void CDEdit::SetNumMax(DWORD dwNumMax)
{
	m_dwNumMax = dwNumMax;
}

void CDEdit::Update()
{
	if (m_nLen >= MAX_LEN)
		m_nLen = MAX_LEN-1;
	m_cBuff[m_nLen] = '\0';
	RECT rc = GetRECT();
	ClientToScreen(&rc);

	unsigned int maxcol = (rc.right - rc.left)/(m_Font.GetWidth()/2);

	if ( m_nLast - m_nFirst > 0 )
    {
        if ( m_nLast > maxcol + m_nFirst && m_nLast - m_nFirst > maxcol)
            g_pCanvas->Clear(rc.left, rc.top - 1, rc.right - rc.left , m_Font.GetHeight() + 2, g_RGB(22,22,222));
        else
		    g_pCanvas->Clear(rc.left + (m_nFirst - (int)m_nShowCur) * m_Font.GetWidth()/2 - 1, rc.top - 1, (m_nLast - m_nFirst) * m_Font.GetWidth()/2 + 2, m_Font.GetHeight() + 2, g_RGB(22,22,222));
    }

    if (m_bShowBord)
        DrawRect(&rc, g_RGB(133,133,133), g_RGB(243, 243, 243));

    int nW = m_Font.GetLineWidth();
    int nH = m_Font.GetLineHeight();
    int nC = m_Font.GetColor();

	m_Font.SetColor(m_nR, m_nG, m_nB);
	m_Font.SetLine(nW, nH);
    if(!m_bMask)
	{
        unsigned int row = 0, col = 0;
        ConverPos2RC(m_nLen, m_nCurPos, m_nMaxLen, m_cBuff, col, row, (rc.right - rc.left)/(m_Font.GetWidth()/2), 
            (rc.bottom - rc.top)/m_Font.GetHeight());

	    if ( m_bMultiline )
	    {
		    m_Font.SetLine((rc.right - rc.left)/(m_Font.GetWidth()/2)*(m_Font.GetWidth()/2), -1);
		    m_nShowCur = 0;
	    }

		int showlen = m_bMultiline ? m_nLen : maxcol;
        showlen = showlen > m_nMaxLen ? m_nMaxLen : showlen;
		char str[ MAX_LEN ] = { 0 };
		memcpy( str, GetString() + m_nShowCur, showlen );
		m_Font.DrawText(rc.left, rc.top, str, showlen);
	}
    else
    {
        char cBuffMask[MAX_LEN] = "";
        int count = 0;
        for ( std::list<wchar_t>::iterator it = bufex.begin(); it != bufex.end(); it ++ )
        {
            cBuffMask[count++] = '*';
            if ( *it & 0xff00 )
                cBuffMask[count++] = '*';
        }
	    cBuffMask[count] = '\0';

        m_Font.DrawText(rc.left, rc.top, cBuffMask);
    }

    m_Font.SetLine(nW, nH);
    m_Font.SetColor(nC);

    if(!m_bEnable)
        return;

    if (GetParent() != NULL)
    {
		CDFace *pParent = GetParent();
		if (pParent->isActive())
		{
			if ( pParent->m_ChildsList.front().operator -> () == this )
                ShowPosmark();
		}
        else
            Select(0, 0);
    }
}

void CDEdit::ShowPosmark()
{
    RECT rc = GetRECT();
    ClientToScreen(&rc);

    unsigned int row = 0, col = 0;
    if ( !m_bMask )	// 光标的位置
    {
        ConverPos2RC(m_nLen, m_nCurPos - m_nShowCur, m_nMaxLen, m_cBuff, col, row, (rc.right - rc.left)/(m_Font.GetWidth()/2), 
            (rc.bottom - rc.top)/m_Font.GetHeight());
    }
    else
    {
        for ( std::list<wchar_t>::iterator it = bufex.begin();
            ( it != this->it ) && ( it != bufex.end() ); it ++ )
            col += ( *it & 0xff00 ) ? 2 : 1;
    }

	if( m_iFlashCount<CURSOR_FLASHCOUNT )
	{
		//画光标
 		g_pCanvas->DrawLine(rc.left + (m_Font.GetWidth()/2)*col, 
 			rc.top  + m_Font.GetHeight()*row, 
 			rc.left + (m_Font.GetWidth()/2)*col, 
 			rc.top + m_Font.GetHeight() - 2 + m_Font.GetHeight()*row, g_RGB(m_nR, m_nG, m_nB)^0xf00f); //m_nCursorClr);// 
  		g_pCanvas->DrawLine(rc.left + 1 +(m_Font.GetWidth()/2)*col, 
  			rc.top  + m_Font.GetHeight()*row, 
  			rc.left + (m_Font.GetWidth()/2)*col + 1, 
  			rc.top + m_Font.GetHeight() - 2 + m_Font.GetHeight()*row, g_RGB(m_nR, m_nG, m_nB)^0xf00f); //m_nCursorClr);// 
	}
    m_iFlashCount ++; 
    if ( m_iFlashCount>=(CURSOR_FLASHCOUNT<<1) )
        m_iFlashCount = 0;

    extern BOOL isImeValidat;
    isImeValidat = true;
}

void CDEdit::Delete(int nFirst, int nLast)
{
    RECT rc = GetRECT();
    ClientToScreen(&rc);
    unsigned int maxcol = (rc.right - rc.left)/(m_Font.GetWidth()/2);

    if (m_bEnable)
        edit_paste(m_nLen, m_nCurPos, m_nMaxLen, m_cBuff, nFirst, nLast, 0, NULL);
    if ( m_nLen <= maxcol)
        m_nShowCur = 0;
    nFirst = nLast = m_nCurPos;
}

void CDEdit::Select(int nFirst, int nLast)
{
    m_nFirst = nFirst>nLast?nLast:nFirst;
    m_nLast = nFirst>nLast?nFirst:nLast;
}

void CDEdit::DrawRect(RECT* rc, int nClrLT, int nClrRB)
{
    g_pCanvas->DrawLine(rc->left, rc->top, rc->right, rc->top, nClrLT);
    g_pCanvas->DrawLine(rc->left, rc->top, rc->left, rc->bottom, nClrLT);
    g_pCanvas->DrawLine(rc->right, rc->top, rc->right, rc->bottom , nClrRB);
    g_pCanvas->DrawLine(rc->left, rc->bottom, rc->right, rc->bottom, nClrRB);	
}

void CDEdit::UpdateTip()
{
	if (dwt::strlen(m_sToolTip, 100) == 0)
		return;

    int nWidth = dwt::strlen(m_sToolTip, 100) * m_Font.GetWidth()/2 + 4;
    int nHeight =  m_Font.GetHeight() + 4;

    RECT rcToolTip;
    rcToolTip.left = m_x - nWidth/2; rcToolTip.top = ((m_y - nHeight) < 3?3:(m_y - nHeight)); rcToolTip.right = m_x + nWidth/2; rcToolTip.bottom = rcToolTip.top + nHeight;

    RECT rcWnd = GetRECT();
    ClientToScreen(&rcWnd);
    POINT pt;
    GetCurrentPos(pt);
    if (PtInRect(&rcWnd, pt))
    if(m_bShowToolTip && m_bInRect)
    {
        ClearAlphaWithBord(m_x - nWidth/2, (m_y - nHeight) < 3?3:(m_y - nHeight), nWidth, nHeight, CLR_TIPBACK, 8);

        int w = m_Font.GetLineWidth();
        int h = m_Font.GetLineHeight();

        int a =m_Font.GetColor();
        m_Font.SetLine(200, 20);
        m_Font.SetColor(CLR_TIPTEXT);
        m_Font.DrawText(m_x - nWidth/2 + 2, (m_y - nHeight)<3?3:(m_y - nHeight) + 2, m_sToolTip);
        //DrawRect(&rcToolTip, 3333, 6666);

        m_Font.SetColor(a);
        m_Font.SetLine(w, h);
    }

}

void CDEdit::AddToolTip(LPSTR sToolTip)
{
    if(strlen(sToolTip) >= 30) 
		return;
    strcpy(m_sToolTip, sToolTip);

    if(sToolTip && sToolTip[0] != 0)
		m_bShowToolTip = true;
}

void CDEdit::OnMouseLeave()
{
	m_bInRect = false;
}