#ifndef	FlashPlayer_H
#define	FlashPlayer_H

#include "win32.h"
#include <string>

struct XENGINE_API IFlashPlayer	{

	struct IEventHandler {
        virtual	HRESULT OnReadyStateChange( DWORD newState );
        virtual	HRESULT OnProgress( DWORD percentDone );
		virtual	HRESULT OnCommand(	LPCSTR command,	LPCSTR args ) = 0;
		virtual	void AnimationEnded() =	0;
    };

	static DWORD			GetVersion();
	static IFlashPlayer*	NewInstance( LPCSTR	filename, IEventHandler	*handler, BOOL trans );
	static IFlashPlayer*    SetCapture( IFlashPlayer *fp ); // ���ú����м��̣�����¼���ֻ����ö��󡣡���

	virtual	void			Destroy() = 0;
	virtual	BOOL			Render(	int	x, int y, BOOL redraw = true ) = 0;
	virtual	void			Play( BOOL loop	) = 0;
	virtual	void			Pause() = 0;

	virtual	void			SetSize( DWORD width, DWORD	height ) = 0;
	virtual	void			SetQuality(	DWORD quality ) = 0;
	virtual	DWORD			ReadyState() = 0;
	virtual	BOOL			IsPlaying() = 0;

	virtual	BOOL			HitTest( int x, int y ) = 0;    // ͸��ͨ������
	virtual	BOOL			Focus( BOOL b ) = 0;            // ���ú�����¼��Żᱻ����, -1�ǲ��޸�����ֱ�ӻ�ȡfocus״̬

	virtual	std::string		GetVariable( LPCSTR	name ) = 0;
	virtual	void			SetVariable( LPCSTR	name, LPCSTR value ) = 0;
	virtual	BOOL			OnWindowMessage( UINT msg, WPARAM wp, LPARAM lp, HRESULT *hr ) = 0;
};

#endif
