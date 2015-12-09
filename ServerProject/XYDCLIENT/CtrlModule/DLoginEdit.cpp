/****************************************************************
//  FileName:   DButton.cpp
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
****************************************************************/
#include "StdAfx.h"

#include "dedit.h"
#include "dloginedit.h"
#include <Colors.h>

#define CURSOR_FLASHCOUNT 10
#define CLR_TIPTEXT g_RGB(222,222,222)
#define CLR_TIPBACK g_RGB(44,44,44)

#include "pub/dwt.h"

#define EDIT_ISTERMINATE        0
#define EDIT_ISENDL             1
#define EDIT_ISASCII            2
#define EDIT_ISWORDCODE         3
#define EDIT_ISWORDCODECENTER   4

#define EDIT_FLAG_WAIT_SECOND_WORDCODE_BYTE     (0x01)
#define EDIT_FLAG_SUPPORT_ENTER                 (0x01 << 8)
#define EDIT_FLAG_SUPPORT_TABLE                 (0x02 << 8)

#define EDIT_IS_STATE(_state, _flag) (_state & _flag)
#define EDIT_SET_STATE(_state, _flag) (_state |= _flag)
#define EDIT_CLEAR_STATE(_state, _flag) (_state &= ~_flag)

#define EDIT_SET_STATE_HIWORD(_state, _word) (_state &= 0xffff, _state |= ((unsigned __int32)_word << 16))
#define EDIT_GET_STATE_HIWORD(_state) (unsigned __int16)(_state >> 16)

extern HWND g_hMainWnd;

extern bool useBIG5;

extern BOOL GBKtoBIG5( std::string &result, LPCSTR str );
extern BOOL BIG5toGBK( std::string &result, LPCSTR str );

extern BOOL UNICODEtoGBK( std::string &result, LPCWSTR wstr );
extern BOOL GBKtoUNICODE( std::basic_string< wchar_t > &result, LPCWSTR wstr );

extern BOOL GBK_simple_to_traditional( std::string &result, LPCSTR str, BOOL useColor = false);

extern bool edit_init(unsigned __int32 &len, 
               unsigned __int32 &pos, 
               unsigned __int32 &status, 
               const unsigned __int32 size, 
               __int8 *pbuffer);

extern int edit_segline(const unsigned __int32 len, 
                 const unsigned __int32 pos,
                 const unsigned __int32 size, 
                 const __int8 *pbuffer);

extern int edit_endline(const unsigned __int32 len, 
                 const unsigned __int32 pos,
                 const unsigned __int32 size, 
                 const __int8 *pbuffer);

extern int edit_length(const unsigned __int32 size, 
                const __int8 *pbuffer, 
                const unsigned __int32 seg, 
                const unsigned __int32 end);

extern int edit_curlength(const unsigned __int32 len, 
                   const unsigned __int32 pos,
                   const unsigned __int32 size, 
                   const __int8 *pbuffer);

extern bool edit_copy(unsigned __int32 &len, 
               unsigned __int32 &pos, 
               const unsigned __int32 size, 
               __int8 *pbuffer, 
               unsigned __int32 seg, 
               unsigned __int32 end, 
               unsigned __int32 rsize, 
               __int8 *prbuffer);

extern bool edit_paste(unsigned __int32 &len, 
                unsigned __int32 &pos, 
                const unsigned __int32 size, 
                __int8 *pbuffer, 
                unsigned __int32 seg, 
                unsigned __int32 end, 
                unsigned __int32 rsize, 
                __int8 *prbuffer);

extern bool edit_char(unsigned __int32 &len, 
               unsigned __int32 &pos, 
               const unsigned __int32 size, 
               __int8 *pbuffer, 
               __int8 ch,
               unsigned __int32 &status);

extern int _edit_iswordcode(const unsigned __int32 len, 
                        const unsigned __int32 pos, 
                        const unsigned __int32 size, 
                        const __int8 *pbuffer);

extern bool _ConverPos2RC(const unsigned __int32 len, 
                    const unsigned __int32 pos, 
                    const unsigned __int32 size, 
                    const __int8 *pbuffer, 
                    unsigned __int32 &col,
                    unsigned __int32 &row,
                    const unsigned __int32 col_max,
                    const unsigned __int32 row_max);

extern bool _ConverRC2Pos(const unsigned __int32 len, 
                    unsigned __int32 &pos, 
                    const unsigned __int32 size, 
                    const __int8 *pbuffer, 
                    const unsigned __int32 _row,
                    const unsigned __int32 _col,
                    const unsigned __int32 row_max,
                    const unsigned __int32 col_max);

extern int _edit_iswordcode(const unsigned __int32 len, 
                        const unsigned __int32 pos, 
                        const unsigned __int32 size, 
                        const __int8 *pbuffer);

extern bool _ConverPos2RC( UINT len, UINT pos, UINT size, LPCSTR pbuffer, UINT &col, UINT &row, UINT col_max, UINT row_max );

extern bool _ConverRC2Pos(const unsigned __int32 len, 
                    unsigned __int32 &pos, 
                    const unsigned __int32 size, 
                    const __int8 *pbuffer, 
                    const unsigned __int32 _row,
                    const unsigned __int32 _col,
                    const unsigned __int32 row_max,
                    const unsigned __int32 col_max);

extern int edit_iswordcode(const unsigned __int32 len, 
                const unsigned __int32 pos, 
                const unsigned __int32 size, 
                const __int8 *pbuffer);

extern bool ConverRC2Pos(const unsigned __int32 len, 
                unsigned __int32 &pos, 
                const unsigned __int32 size, 
                const __int8 *pbuffer, 
                const unsigned __int32 _row,
                const unsigned __int32 _col,
                const unsigned __int32 row_max,
                const unsigned __int32 col_max);

extern bool ConverPos2RC(const unsigned __int32 len, 
                const unsigned __int32 pos, 
                const unsigned __int32 size, 
                const __int8 *pbuffer, 
                unsigned __int32 &col,
                unsigned __int32 &row,
                const unsigned __int32 col_max,
                const unsigned __int32 row_max);

CDLoginEdit::CDLoginEdit(int fontsize) : 
    CDFace(IID_CDLoginEdit, fontsize)
{
	m_nNowPos = 0;
	m_nShowCur = 0;
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

    m_nMaxLen = 64;
    edit_init(m_nLen, m_nCurPos, m_InputState, m_nMaxLen, m_cBuff);

    m_bEnable = true;

    m_bCapture = false;

	m_nR = m_nG = m_nB = 222;

	m_dwNumMax = 0xffffffff;

	m_bInRect = m_bShowToolTip = false;
	memset(m_sToolTip, 0, 31);

	m_nMaxWriteLen = 0;
}

CDLoginEdit::~CDLoginEdit(void)
{
}

int	CDLoginEdit::InitByBinary(LPARAM lParam)
{
    return 1;
}
   
bool CDLoginEdit::SetParameter(LPCSTR szParameterName, LPARAM lParam)
{
	if (strcmp(szParameterName, "ToolTip") == 0)
		AddToolTip((LPSTR)lParam);
	// 允许输入的字符串长度
	else if( strcmp(szParameterName, "MaxLine") == 0 )
		m_nMaxWriteLen = atoi( ( LPSTR )lParam );

    return CDFace::SetParameter(szParameterName, lParam);
}

int	CDLoginEdit::OnCreate(LPARAM lParam)
{
    if (lParam == 1)
        EDIT_SET_STATE(m_InputState, EDIT_FLAG_SUPPORT_ENTER);

    RECT rc = GetRECT();

	int nLinecount = (rc.bottom - rc.top) / m_Font.GetHeight();
	if (nLinecount < 1)
		nLinecount = 1;

    m_nMaxLen = ((rc.right - rc.left) / (m_Font.GetWidth()/2)) * nLinecount + 1;
    if (m_nMaxLen > MAX_LEN) 
        m_nMaxLen = MAX_LEN;

    return 1;
}

void CDLoginEdit::SetRECT(int left, int top, int right, int bottom)
{
	int nLinecount = (bottom - top) / m_Font.GetHeight();
	if (nLinecount < 1)
		nLinecount = 1;

    m_nMaxLen = ((right - left) / (m_Font.GetWidth()/2)) * nLinecount + 1;
    if (m_nMaxLen > MAX_LEN) 
        m_nMaxLen = MAX_LEN;

	CDFace::SetRECT(left, top, right, bottom);
}

void CDLoginEdit::SetFontSize(int size)
{
    CDFace::SetFontSize(size);

    RECT rc = GetRECT();

    m_nMaxLen = ((rc.right - rc.left) / (m_Font.GetWidth()/2)) * ((rc.bottom - rc.top) / m_Font.GetHeight()) + 1;
    if (m_nMaxLen > MAX_LEN) 
        m_nMaxLen = MAX_LEN;
}

int CDLoginEdit::KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y)
{
    if ( !m_bEnable )
        return 1;

    RECT rc = GetRECT();
    ClientToScreen(&rc); 
	
	m_x = x;
	m_y = y;

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
            int nPosTo = 2*(x - rc.left)/m_Font.GetWidth() + (bAddOne?1:0);
            if (edit_iswordcode(m_nLen, nPosTo-1, m_nMaxWriteLen, m_cBuff) == EDIT_ISWORDCODE)
            {
                nPosTo += 1;
            }

            m_nCurPos = (nPosTo > (int)m_nLen) ? m_nLen : nPosTo;

            m_nEnd = m_nCurPos;

            SetCapture();
            m_bCapture = true;
        }
        else if (iKeyState == MS_MOVE)
        {
            if (m_bCapture)
            {
                bool bAddOne = (2*(x - rc.left)%m_Font.GetWidth()) > (m_Font.GetWidth()/2);
                int nPosTo = 2*(x - rc.left)/m_Font.GetWidth() + (bAddOne?1:0);
                if (edit_iswordcode(m_nLen, nPosTo-1, m_nMaxWriteLen, m_cBuff) == EDIT_ISWORDCODE)
                {
                    nPosTo += 1;
                }

                nPosTo = nPosTo < 0 ? 0 : nPosTo;
                m_nCurPos = (nPosTo > (int)m_nLen) ? m_nLen : nPosTo;
                
                if ((int)m_nCurPos > m_nEnd)
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
                int nPosTo = 2*(x - rc.left)/m_Font.GetWidth() + (bAddOne?1:0);
                if (edit_iswordcode(m_nLen, nPosTo-1, m_nMaxWriteLen, m_cBuff) == EDIT_ISWORDCODE)
                {
                    nPosTo += 1;
                }
                nPosTo = nPosTo < 0 ? 0 : nPosTo;

                m_nCurPos = (nPosTo > (int)m_nLen) ? m_nLen : nPosTo;

                if ((int)m_nCurPos > m_nEnd)
                    Select(m_nEnd, m_nCurPos);
                else
                    Select(m_nCurPos, m_nEnd);

                m_bCapture = false;
                ReleaseCapture();
            }     
        }
        return 0;
    }

    unsigned int row = 0, col = 0;
    unsigned int maxrow = 0, maxcol = 0;
    maxcol = (rc.right - rc.left)/(m_Font.GetWidth()/2);
    maxrow = (rc.bottom - rc.top)/m_Font.GetHeight();

    switch(iKeyState)
    {
    case KEY_CHAR:
        /*
        if (!EDIT_IS_STATE(m_InputState, EDIT_FLAG_WAIT_SECOND_WORDCODE_BYTE))
        {
            switch (vKey)
            {
            case VK_CANCEL: // ctrl + c
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
                    hData = GlobalAlloc(GMEM_FIXED, len+1);
                    pData = (char *)GlobalLock(hData);

                    memcpy(pData, &m_cBuff[m_nFirst], len);
                    pData[len] = 0;

                    GlobalUnlock(hData);
                    SetClipboardData(CF_TEXT, hData);
                    CloseClipboard();
                }
                return 1;

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
                    hData = GlobalAlloc(GMEM_FIXED, len+1);
                    pData = (char *)GlobalLock(hData);

                    memcpy(pData, &m_cBuff[m_nFirst], len);
                    pData[len] = 0;

                    GlobalUnlock(hData);
                    SetClipboardData(CF_TEXT, hData);
                    CloseClipboard();

                    Delete(m_nFirst, m_nLast);
                    m_nFirst = m_nLast = 0;
                }
                return 1;

            case 0x16: // ctrl + v
                if (IsClipboardFormatAvailable(CF_TEXT))
                {
                    OpenClipboard(g_hMainWnd);
                    HANDLE hData = GetClipboardData(CF_TEXT);
                    int len = (int)GlobalSize(hData);

                    char *pBuf = (char*)GlobalLock(hData);
					len = dwt::strlen(pBuf, len) + 1;

                    ConverPos2RC(m_nLen, m_nLen, m_nMaxLen, m_cBuff, col, row, maxcol, maxrow);

                    unsigned int maxlen = m_nLen + (maxrow - (row+1)) * maxcol + (maxcol - col) + 1;
                    if (maxlen > m_nMaxLen) 
                        maxlen = m_nMaxLen;

                    if ((m_nFirst == m_nLast) && (m_nFirst != m_nCurPos))
                        m_nFirst = m_nLast = m_nCurPos;

                    edit_paste(m_nLen, m_nCurPos, maxlen, m_cBuff, m_nFirst, m_nLast, len, pBuf);
                    m_nFirst = m_nLast = 0;
                    
                    GlobalUnlock(hData);
                    CloseClipboard();
                }
                return 1;
            }

            if (m_nFirst != m_nLast)
            {
                Delete(m_nFirst, m_nLast);
                m_nFirst = m_nLast = 0;
            }

            if ( m_FirstChar != 0 )
            {
            }
        }

        AddChar(vKey);
        */

        switch (vKey)
        {
		case VK_ESCAPE:
			return TRUE;
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

                ConverPos2RC(m_nLen, m_nLen, m_nMaxWriteLen, m_cBuff, col, row, maxcol, maxrow);

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
                {
                    edit_paste(m_nLen, m_nCurPos, m_nMaxWriteLen, m_cBuff, m_nFirst, m_nLast, len, (LPSTR)result.c_str() );
					m_nCurPos = 0;
					m_nShowCur = 0;
					m_nNowPos = 0;
                }

                m_nFirst = m_nLast = 0;
                
                GlobalUnlock(hData);
                CloseClipboard();
            }
            return 1;
        }

		if (m_nFirst != m_nLast)
		{
			Delete(m_nFirst, m_nLast);
			m_nFirst = m_nLast = 0;
		}
		else if( vKey == VK_BACK )
		{
			if( m_nCurPos >= 0 && m_nCurPos <= m_nLen )
			{
				int nAdd = 1;
				if( m_nShowCur > 0 )
					m_nShowCur -= nAdd;
				else
				{
					if( m_nNowPos > 0 )
						m_nNowPos -= nAdd;
				}
				AddChar(VK_BACK);
			}
		}
		else if ( m_FirstChar == 0 && vKey < 0x80 )
		{
			if( m_nLen >= m_nMaxWriteLen - 1 )
				break;

			if( AddChar(vKey) )
			{
				if( m_nNowPos >= maxcol )
					m_nShowCur ++;
				else
					m_nNowPos ++;
			}
		}
        else if ( m_FirstChar == 0 )
        {
            m_FirstChar = vKey;
        }
		else
		{
			/*
			// 双字节字符
			// 检测当前状态是否为繁体，是则将该字符转为简体后输入！
			if( m_nLen >= m_nMaxWriteLen - 2 )
				break;

			if ( useBIG5 )
			{
				char temp[4] = { (char)m_FirstChar, (char)vKey, 0, 0 };
				std::string result;
				BIG5toGBK( result, temp );
				AddChar(result[0] & 0xff);
				AddChar(result[1] & 0xff);
			}
			else
			{
				AddChar(m_FirstChar);
				AddChar(vKey);
			}

			if( m_nCurPos > 0 && m_nCurPos <= m_nLen )
			{
				int nAdd = 0;
				switch (edit_iswordcode(m_nLen, m_nCurPos-1, m_nMaxWriteLen, m_cBuff))
				{
				case EDIT_ISWORDCODECENTER:
					nAdd = 2;
					break;
				case EDIT_ISENDL:
					nAdd = 1;
					break;
				case EDIT_ISASCII:
					nAdd = 1;
					break;
				case EDIT_ISWORDCODE:
					nAdd = 1;
					break;
				}

				if( m_nLen > maxcol && m_nLen < m_nMaxWriteLen - 1 )
				{
					m_nShowCur += nAdd;
					m_nNowPos = maxcol;
				}
				else
				{
					m_nNowPos += nAdd;
					if( m_nNowPos > maxcol )
						m_nNowPos = maxcol;
				}
			}
			*/
			m_FirstChar = 0;
		}

        m_iFlashCount = 0;
        break;

    case KEY_HOLD:
    case KEY_DOWN:
		if(vKey == VK_DELETE)
		{
			if (m_nFirst != m_nLast)
		{
			Delete(m_nFirst, m_nLast);
			m_nFirst = m_nLast = 0;
		}
		}
        if((vKey == VK_LEFT))
		{
			if( m_nCurPos > 0 )
			{
				int nDel = 1;
				/*
				switch (edit_iswordcode(m_nLen, m_nCurPos-1, m_nMaxWriteLen, m_cBuff))
				{
				case EDIT_ISTERMINATE:
					break;
				case EDIT_ISENDL:
					nDel = 1;
					break;
				case EDIT_ISASCII:
					nDel = 1;
					break;
				case EDIT_ISWORDCODE:
					nDel = 1;
					break;
				case EDIT_ISWORDCODECENTER:
					nDel = 2;
					break;
				}
				*/

				int showlen = m_nLen <= maxcol ? m_nCurPos : ( m_nLen == m_nCurPos ? maxcol : maxcol - ( m_nLen - m_nCurPos ) % maxcol );
				if( m_nNowPos == 0 && m_nShowCur > 0 )
					m_nShowCur -= nDel;

				if( (int)m_nNowPos >= nDel )
					m_nNowPos -= nDel;
				m_nCurPos -= nDel;

				if(m_bShiftDown)
					Select(m_nCurPos, m_nEnd);
				else
					Select(0, 0);
			}
        }
        else if((vKey == VK_RIGHT) && (m_nCurPos < m_nLen))
		{
			if( m_nCurPos < m_nLen )
			{
				int nAdd = 1;
				/*
				switch (edit_iswordcode(m_nLen, m_nCurPos, m_nMaxWriteLen, m_cBuff))
				{
				case EDIT_ISTERMINATE:
					break;
				case EDIT_ISENDL:
					nAdd = 1;
					break;
				case EDIT_ISASCII:
					nAdd = 1;
					break;
				case EDIT_ISWORDCODE:
					nAdd = 2;
					break;
				case EDIT_ISWORDCODECENTER:
					nAdd = 1;
					break;
				}
				*/

				int showlen = m_nLen <= maxcol ? m_nCurPos : ( m_nLen == m_nCurPos ? maxcol : maxcol - ( m_nLen - m_nCurPos ) % maxcol );
				if( m_nNowPos == maxcol && m_nShowCur < m_nLen - maxcol )
					m_nShowCur += nAdd;

				if( m_nNowPos + nAdd <= maxcol )
					m_nNowPos += nAdd;
				m_nCurPos += nAdd;

				if(m_bShiftDown)
					Select(m_nEnd, m_nCurPos);
				else
					Select(0, 0);
			}
        }
        else if(vKey == VK_UP)
        {
            ConverPos2RC(m_nLen, m_nCurPos, m_nMaxWriteLen, m_cBuff, col, row, maxcol, maxrow);
            if (row > 0)
            {
                row--;
                ConverRC2Pos(m_nLen, m_nCurPos, m_nMaxWriteLen, m_cBuff, row, col, maxrow, maxcol);
            }

            Select(0, 0);
        }
        else if(vKey == VK_DOWN)
        {
            ConverPos2RC(m_nLen, m_nCurPos, m_nMaxWriteLen, m_cBuff, col, row, maxcol, maxrow);
            if (row+1 < maxrow)
            {
                row++;
                ConverRC2Pos(m_nLen, m_nCurPos, m_nMaxWriteLen, m_cBuff, row, col, maxrow, maxcol);
            }

            Select(0, 0);
        }
        else if((iKeyState == KEY_DOWN) && ( vKey == VK_SHIFT))
        {
            m_bShiftDown = true;			
            m_nEnd = m_nCurPos;
        }
        else if(vKey == VK_HOME)
        {
			m_nNowPos = 0;
			m_nShowCur = 0;
            m_nCurPos = 0;

            if(m_bShiftDown && m_nLen)
                Select(m_nEnd, m_nCurPos);
            else
                Select(0, 0);
        }
        else if(vKey == VK_END)
        {
			unsigned int maxcol = 0;
			RECT rc = GetRECT();
			maxcol = (rc.right - rc.left)/(m_Font.GetWidth()/2);
			if( m_nLen > maxcol )
			{
				m_nNowPos = maxcol;
				m_nShowCur = m_nLen - maxcol;
			}
			else
			{
				m_nNowPos = m_nLen;
				m_nShowCur = 0;
			}

            m_nCurPos = m_nLen;
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

void CDLoginEdit::SetTextColor(int nR, int nG, int nB)
{
    m_nR = nR;
    m_nG = nG;
    m_nB = nB;
}

bool CDLoginEdit::AddChar(int nChar)
{
	unsigned int row = 0, col = 0;
	unsigned int maxrow = 0, maxcol = 0;
	RECT rc = GetRECT();
	maxcol = (rc.right - rc.left)/(m_Font.GetWidth()/2);
	maxrow = (rc.bottom - rc.top)/m_Font.GetHeight();
	ConverPos2RC(m_nLen, m_nLen, m_nMaxLen, m_cBuff, col, row, maxcol, maxrow);

	unsigned int _row = 0, _col = 0;
	ConverPos2RC(m_nLen, m_nCurPos, m_nMaxLen, m_cBuff, _col, _row, maxcol, maxrow);

 	if (!EDIT_IS_STATE(m_InputState, EDIT_FLAG_WAIT_SECOND_WORDCODE_BYTE))
	{
		if(m_bNumOnly)
		{
            if(((nChar < '0') || (nChar > '9')) && (nChar != VK_BACK)) 
                return false;
		}

        // 是否是行数满了的情况
        if (row+1 >= maxrow)
        {
            if (nChar == VK_RETURN)
				return false;

			if (nChar != VK_BACK)
			{
				unsigned len = edit_curlength(m_nLen, m_nCurPos, m_nMaxLen, m_cBuff);
				if (len != 0)
				{
					len %= m_nMaxWriteLen;
					if (len == 0)
						return false;

					if (len + ((nChar < 0x80) ? 0 : 1) >= m_nMaxWriteLen)
						return false;
				}
			}
		}
	}

	if( !edit_char(m_nLen, m_nCurPos, m_nMaxWriteLen, m_cBuff, (char)nChar, m_InputState) )
		return false;

    if(m_bNumOnly)
	{
        if(((nChar < '0') || (nChar > '9')) && (nChar != VK_BACK)) 
            return false;

		if ((DWORD)atoi(m_cBuff) > m_dwNumMax)
			SetString(m_dwNumMax);
		//if (m_cBuff[0] == 0)
		//{
		//	m_cBuff[0] = '0';
		//}
	}

	return true;
}

void CDLoginEdit::SetString(char *pBuff, long size) 
{
    if (size > (long)m_nMaxLen)
        size = (long)m_nMaxLen;
    m_nCurPos = 0;
    edit_paste(m_nLen, m_nCurPos, m_nMaxLen, m_cBuff, 0, 
        m_nLen, dwt::strlen(pBuff, size)+1, pBuff);

	unsigned int maxcol = 0;
	RECT rc = GetRECT();
	maxcol = (rc.right - rc.left)/(m_Font.GetWidth()/2);

	if( m_nCurPos > maxcol )
	{
		m_nNowPos = maxcol;
		m_nShowCur = m_nCurPos - maxcol;
	}
	else
		m_nNowPos = m_nCurPos;
}

void CDLoginEdit::SetString(DWORD dwNum)
{
	char cTxt[20];
	memset(cTxt, 0, 20);
	itoa(dwNum, cTxt, 10);

	int nCount = 0;
	while(cTxt[nCount++] != 0);

	SetString(cTxt, nCount);
}
    
void CDLoginEdit::SetNumOnly(bool bNum) 
{ 
	m_bNumOnly = bNum; 
	if (0)//bNum)
	{
		m_nLen = 1;
		m_cBuff[0] = '0'; 
	}
}

void CDLoginEdit::SetNumMax(DWORD dwNumMax)
{
	m_dwNumMax = dwNumMax;
}

void CDLoginEdit::Update()
{
    char cBuffMask[MAX_LEN] = ""; 
    if (m_nLen >= MAX_LEN)
        m_nLen = MAX_LEN-1;
    for(unsigned int i = 0; i < m_nLen; i ++)
        cBuffMask[i] = '*';
    m_cBuff[m_nLen] = '\0';
    cBuffMask[m_nLen] = '\0';
    RECT rc = GetRECT();
    ClientToScreen(&rc);
	unsigned int maxcol = (rc.right - rc.left)/(m_Font.GetWidth()/2);

    if (m_nLast - m_nFirst > 0)
    {
		// int nShowRectLen = m_nLast > maxcol ? maxcol : m_nLast;
		int nShowRectLen = m_nLast - m_nShowCur;
		nShowRectLen = nShowRectLen > (int)maxcol ? maxcol : nShowRectLen;
		int nfirst = ( m_nFirst == 0 ? 0 : m_nFirst - m_nShowCur );
		g_pCanvas->Clear(rc.left + nfirst * m_Font.GetWidth()/2 - 1, rc.top - 1, ( nShowRectLen - nfirst ) * m_Font.GetWidth()/2 + 2, m_Font.GetHeight() + 2, g_RGB(22,22,222));
    }

    if (m_bShowBord)
        DrawRect(&rc, g_RGB(133,133,133), g_RGB(243, 243, 243));

    int nW = m_Font.GetLineWidth();
    int nH = m_Font.GetLineHeight();
    int nC = m_Font.GetColor();

    unsigned int row = 0, col = 0;
    ConverPos2RC(m_nLen, m_nCurPos, m_nMaxLen, m_cBuff, col, row, (rc.right - rc.left)/(m_Font.GetWidth()/2), 
        (rc.bottom - rc.top)/m_Font.GetHeight());

	// 取掉分行.
	m_Font.SetLine(nW, nH);
   // m_Font.SetLine((rc.right - rc.left)/(m_Font.GetWidth()/2)*(m_Font.GetWidth()/2), -1);
    m_Font.SetColor(m_nR, m_nG, m_nB);

	// 取一行最长显示的数量
    if(!m_bMask) 
	{
		int showlen = m_nLen < maxcol ? m_nLen : maxcol;
		char str[ 50 ] = { 0 };
		memcpy( str, GetString() + m_nShowCur, showlen );
		m_Font.DrawText(rc.left, rc.top, str, showlen);
	}
    else
        m_Font.DrawText(rc.left, rc.top, cBuffMask);

    //m_Font.SetLine(nW, nH);
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

void CDLoginEdit::ShowPosmark()
{
    RECT rc = GetRECT();
    ClientToScreen(&rc);

	unsigned int maxcol = (rc.right - rc.left)/(m_Font.GetWidth()/2);
    unsigned int row = 0, col = 0;
	ConverPos2RC(m_nLen, m_nCurPos, m_nMaxWriteLen, m_cBuff, col, row, (rc.right - rc.left)/(m_Font.GetWidth()/2), 
		(rc.bottom - rc.top)/m_Font.GetHeight());

	if( m_iFlashCount<CURSOR_FLASHCOUNT )
	{
		g_pCanvas->DrawLine(rc.left + (m_Font.GetWidth()/2)*m_nNowPos, 
			rc.top, 
			rc.left + (m_Font.GetWidth()/2)*m_nNowPos, 
			rc.top + m_Font.GetHeight() - 2, g_RGB(m_nR, m_nG, m_nB)^0xf00f); //m_nCursorClr);// 
		g_pCanvas->DrawLine(rc.left + 1 +(m_Font.GetWidth()/2)*m_nNowPos, 
			rc.top, 
			rc.left + (m_Font.GetWidth()/2)*m_nNowPos + 1, 
			rc.top + m_Font.GetHeight() - 2, g_RGB(m_nR, m_nG, m_nB)^0xf00f); //m_nCursorClr);// 
	}
    m_iFlashCount ++; 
    if ( m_iFlashCount>=(CURSOR_FLASHCOUNT<<1) )
        m_iFlashCount = 0;

    extern BOOL isImeValidat;
    isImeValidat = true;
}

void CDLoginEdit::Delete(int nFirst, int nLast)
{
    if (m_bEnable)
	{
		RECT rc = GetRECT();
		ClientToScreen(&rc);
		unsigned int maxcol = (rc.right - rc.left)/(m_Font.GetWidth()/2);

        edit_paste(m_nLen, m_nCurPos, m_nMaxWriteLen, m_cBuff, nFirst, nLast, 0, NULL);

		if( m_nShowCur + maxcol > m_nLen )
		{
			if( m_nLen <= maxcol )
				m_nShowCur = 0;
			else
				m_nShowCur = m_nLen - maxcol;
		}
		m_nNowPos = m_nCurPos - m_nShowCur;

		/*
		int len = nLast - nFirst;
		if( m_nLen > maxcol )
		m_nShowCur = m_nShowCur < len ? len - m_nShowCur : m_nShowCur - len;
		else
			m_nShowCur = 0;

		if( m_nShowCur > m_nLen )
			m_nShowCur = 0;
		m_nNowPos = m_nCurPos - m_nShowCur;
		if( m_nNowPos > m_nLen )
			m_nNowPos = 0;
			*/

		nLast = nFirst = m_nEnd = 0;
	}
}

void CDLoginEdit::Select(int nFirst, int nLast)
{
    m_nFirst = nFirst>nLast?nLast:nFirst;
    m_nLast = nFirst>nLast?nFirst:nLast;
}

void CDLoginEdit::DrawRect(RECT* rc, int nClrLT, int nClrRB)
{
    g_pCanvas->DrawLine(rc->left, rc->top, rc->right, rc->top, nClrLT);
    g_pCanvas->DrawLine(rc->left, rc->top, rc->left, rc->bottom, nClrLT);
    g_pCanvas->DrawLine(rc->right, rc->top, rc->right, rc->bottom , nClrRB);
    g_pCanvas->DrawLine(rc->left, rc->bottom, rc->right, rc->bottom, nClrRB);	
}

void CDLoginEdit::UpdateTip()
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

void CDLoginEdit::AddToolTip(LPSTR sToolTip)
{
    if(strlen(sToolTip) >= 30) 
		return;
    strcpy(m_sToolTip, sToolTip);

    if(sToolTip && sToolTip[0] != 0)
		m_bShowToolTip = true;
}

void CDLoginEdit::OnMouseLeave()
{
	m_bInRect = false;
}
