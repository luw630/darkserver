/****************************************************************
//  FileName:   DButton.cpp
//  Author:		YG
//  Create:		2003.4
//  Company:    DreamWork
//  说明：普通按钮类，需先new一个该类的指针，然后在Create()中创建和再入资源
//　　　　参数通过SParameter结构传入，为４个图片文件名，Bitmap格式
****************************************************************/
#include "StdAfx.h"


#include "Engine.h"
#include "Canvas.h"
#include "dflashwnd.h"
#include "Colors.h"

CDFlashWnd *CDFlashWnd::activeFlash = NULL;
CDFlashWnd *CDFlashWnd::prevFlash = NULL;

CDFlashWnd::CDFlashWnd(void) : CDFace(IID_CDFLASHWND)
{
    flash = NULL;
    rebuild = 0;
    tickSkip = 0;
    curSkip = 0;
    trans = 0;
    ctrl = 0;
    border = 0;
}

CDFlashWnd::~CDFlashWnd(void)
{
    if ( flash )
        flash->Destroy();
}

int	 CDFlashWnd::InitByBinary(LPARAM lParam)
{
	return 0;
}

int	CDFlashWnd::PtInFace(POINT &pt)
{
    if ( flash == NULL )
        return false;

    RECT rc = GetRECT();
    ClientToScreen( &rc );
    return flash->HitTest( pt.x - rc.left, pt.y - rc.top );
}

void CDFlashWnd::Show(bool bShow)
{
    CDFace::Show( bShow );
    if ( flash ) {
        if ( !bShow )
            flash->Pause();
        else
            flash->Play( true );
    }
}

bool CDFlashWnd::SetParameter(LPCSTR szParameterName, LPARAM lParam)
{
    if(strcmp(szParameterName, "movie") == 0) {
        if ( movie != (LPSTR)lParam ) {
            movie = (LPSTR)lParam;
            rebuild = true;
        }
        return true;
    }
    else if(strcmp(szParameterName, "tickskip") == 0) {
        tickSkip = atoi( (LPSTR)lParam );
    }
    else if(strcmp(szParameterName, "trans") == 0) {
        trans = atoi( (LPSTR)lParam );
    }
    else if(strcmp(szParameterName, "ctrl") == 0) {
        ctrl = atoi( (LPSTR)lParam );
    }
    else if(strcmp(szParameterName, "border") == 0) {
        border = atoi( (LPSTR)lParam );
    }

    if(stricmp(szParameterName, "Size") == 0)
        rebuild = true;

    return CDFace::SetParameter(szParameterName, lParam);
}

int  CDFlashWnd::OnCreate(LPARAM lParam)
{
	if ( !lParam )
		return 0;

	return 1;
}

int CDFlashWnd::KeyMsAction(KEY_STATE iKeyState,int vKey,int x,int y)
{
    return RET_NONE;
    /*
	int nRet = RET_NONE;

	RECT rcWnd = GetRECT();
	ClientToScreen(&rcWnd);
	POINT pt;
	pt.x = x;
	pt.y = y;

    if ( flash && flash->IsPlaying() ) {
        HRESULT hr = 0;
        extern UINT   g_uMsg;
        extern WPARAM g_wParam;
        extern LPARAM g_lParam;
        flash->OnWindowMessage( g_uMsg, g_wParam, g_lParam, &hr );
    }

	if ( PtInRect(&rcWnd,pt) )
	{
	}
	else
	{
	}
	return nRet;
    */
}

//鼠标移出窗口（控件）矩形框（在Create中设置的）时触发
void  CDFlashWnd::OnMouseLeave()
{
}

void CDFlashWnd::Update()
{
    if ( rebuild ) {
        if ( m_nWidth != 0 && m_nHeight != 0 ) {
            rebuild = false;
            if ( flash )
                flash->Destroy();
            flash = IFlashPlayer::NewInstance( movie.c_str(), NULL, trans );
            flash->SetSize( m_nWidth, m_nHeight );
            flash->Play( true );
        }
    } else if ( flash != NULL ) {
        if ( ctrl && isActive() )
            CDFlashWnd::activeFlash = this;
        RECT rc = GetRECT();
        ClientToScreen( &rc );
        curSkip ++;
        flash->Render( rc.left, rc.top, tickSkip ? ( ( curSkip % tickSkip ) == 0 ) : TRUE );
        if ( border ) {
            rc.top-=1;
            rc.left-=1;
            rc.bottom+=1;
            rc.right+=1;
            if ( CDFlashWnd::prevFlash == this )
                DrawRect( &rc, 0xfff0, 0xfff0 );
            else 
                DrawRect( &rc, 0x7bef, 0x7bef ); 
        }
    }
}

void CDFlashWnd::UpdateTip()
{
}

void CDFlashWnd::AddToolTip(char* sToolTip)
{
}

