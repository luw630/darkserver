//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	EDDraw.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef EDDraw_H
#define EDDraw_H
//---------------------------------------------------------------------------
#define FULLSCREEN			0
#define WINDOWMODE			1
#define RGB_555 			0x7fff
#define RGB_565 			0xffff

#define	WND_INIT_WIDTH		800
#define	WND_INIT_HEIGHT		600

//---------------------------------------------------------------------------
class XENGINE_API EDirectDraw
{
private:
	LPDIRECTDRAW		m_lpDirectDraw;
	LPDIRECTDRAWSURFACE	m_lpDDSPrimary;
	LPDIRECTDRAWSURFACE	m_lpDDSBackBuf;
	LPDIRECTDRAWCLIPPER m_lpClipper;

private:
	DWORD		m_dwScreenMode;
	DWORD		m_dwScreenWidth;
	DWORD		m_dwScreenHeight;
	DWORD		m_dwScreenPitch;
	DWORD		m_dwRGBBitCount;
	DWORD		m_dwRGBBitMask16;
	DWORD		m_dwRGBBitMask32;
    HWND        m_hPrevWndClip;

private:
	BOOL		CreateDirectDraw();
	BOOL		CreateClipper();
	BOOL		CreateSurface();
	BOOL		GetSurfaceDesc();
	BOOL		GetDisplayMode();
	BOOL		SetDisplayMode();
	void		SetWindowStyle();

public:
	EDirectDraw();
	~EDirectDraw();
	void		Mode(BOOL bFullScreen, int nWidth, int nHeight);
	BOOL		Init();
	void		Exit();
	void		WaitForVerticalBlankBegin();
	void		WaitForVerticalBlankEnd();
	BOOL		RestoreSurface();
	void		SetClipperHWnd(HWND hWnd);
	PVOID		LockPrimaryBuffer();
	void		UnLockPrimaryBuffer();
	PVOID		LockBackBuffer();
	void		UnLockBackBuffer();
	void		FillBackBuffer(DWORD dwColor);
	void		UpdateScreen(LPRECT lpRect);
	void		UpdateScreenZoom(LPRECT lpRect);
    void        RestoreDisplayMode();
public:
	DWORD		GetScreenMode(){return m_dwScreenMode;};
	DWORD		GetScreenWidth(){return m_dwScreenWidth;};
	DWORD		GetScreenHeight(){return m_dwScreenHeight;};
	DWORD		GetScreenPitch(){return m_dwScreenPitch;};
	DWORD		GetRGBBitCount(){return m_dwRGBBitCount;};
	DWORD		GetRGBBitMask16(){return m_dwRGBBitMask16;};
	DWORD		GetRGBBitMask32(){return m_dwRGBBitMask32;};
};
extern ENGINE_API EDirectDraw* g_pDirectDraw;
//---------------------------------------------------------------------------
#endif
