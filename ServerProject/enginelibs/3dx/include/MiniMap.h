#ifndef		MiniMap_H
#define	    MiniMap_H
#include "3dx_Mgr.h"

class XENGINE_API TexImg;
class iTexBlender : public iRenderObj {
public:
	static XENGINE_API iTexBlender* NewInstance( TexImg *layer0, TexImg *layer1 );
	virtual void DrawCover( int x0, int y0, int x1, int y1, int z ) = 0;
};

#endif