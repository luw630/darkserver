#ifndef _RTTRENDER_H_
#define _RTTRENDER_H_

#include "3dx_Mgr.h"

class RttRender : public iRenderObj 
{
public:
	static XENGINE_API RttRender* NewInstance( void * pTex );
    virtual void DrawCover( int _x0, int _y0,int _z ) = 0; 
    virtual void FreeTex() = 0; // 由于是RTT硬件对象，在设备丢失的时候需要做对应的资源清除
};

#endif