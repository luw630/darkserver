#ifndef _JPEGSPR_H_
#define _JPEGSPR_H_
#include "TexImg.h"

class XENGINE_API JpegSpr
{
public:
	JpegSpr(void);
	~JpegSpr(void);

    BOOL Load( const char* fileName );
    BOOL Load( const void* jpgData, size_t size );
    void Free();
    void Draw( POINT pos, int z ); // pos=整图绘制坐标点，z=保留
    BOOL validate();
    DWORD GetWidth();
    DWORD GetHeight();

    struct BlkImg { DWORD x,y; BOOL ok; TexImg img; };
private:
    void *data;
    void LoadBlk( BlkImg *img );
};
#endif