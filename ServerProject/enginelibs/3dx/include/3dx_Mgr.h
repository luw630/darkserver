#pragma once
#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
#include "windows.h"

#include <assert.h>
#include "d3d9.h"
#include "..\DirectX9\Include\d3dx9.h"
//#include "d3dx9.h"
#include "win32.h"

// TODO: 在此处引用程序要求的附加头文件

// 关于深度管理：
// 由于画家算法的特点，本来绘制的顺序就已经可以做为深度信息了
// 但是，由于深度缓存是有限的，所以纹理绘制后续的简单图形操作全部使用和上一个纹理相同的深度

class Sprite;
struct Float3 { float x, y, z; };
struct Float2 { float x, y; };

struct iRenderObj {
    virtual void AddRef() = 0;
    virtual void Release() = 0;
    virtual void Show( struct i3dxMgr *mgr ) = 0;
};

struct iLogicalImg {
    virtual void AddRef() = 0;
    virtual void Release() = 0;
    virtual SIZE GetSize() = 0;
    virtual DWORD GetPixelAlpha( int x, int y ) = 0;
    virtual void Show( int x, int y, DWORD z, DWORD color ) = 0;
    virtual void Show( RECT *rect, DWORD z, DWORD colorTop, DWORD colorBottom ) = 0;
    virtual void ShowProgress( int x, int y, DWORD len, DWORD z, DWORD color ) = 0;
};

struct iLogicalSpr {
    virtual void AddRef() = 0;
    virtual void Release() = 0;
    virtual Sprite* GetRawSpr() = 0;
    virtual DWORD GetPixelAlpha( int x, int y, DWORD nFrame ) = 0;
    virtual void Show( int x, int y, DWORD z, DWORD f, DWORD color ) = 0;
    virtual void ShowCenter( int x, int y, DWORD z, DWORD f, DWORD color ) = 0;
    virtual void Show( RECT *rect, DWORD z, DWORD f, DWORD colorTop, DWORD colorBottom ) = 0;
    virtual void ShowProgress( int x, int y, DWORD len, DWORD z, DWORD f, DWORD color ) = 0;
};

struct iAnimation {
    virtual void AddRef() = 0;
    virtual void Release() = 0;
    virtual DWORD GetMaxFrame() = 0;
};

struct iMesh {
    virtual void AddRef() = 0;
    virtual void Release() = 0;
    // x、y、z就不说了，ani是该模型对应的动画数据，scale以像数为单位进行缩放，f帧数的小数位用于做帧间插值运算。。。
    virtual void Show( int x, int y, DWORD z, iAnimation *ani, float f, DWORD scale, DWORD color ) = 0;
};

struct XENGINE_API i3dxMgr {

    static i3dxMgr* GetInstance();
    static IDirect3DDevice9* GetDevice();

    // 真正的绘图逻辑，前面的所有绘制行为都只是在记录操作而已
    virtual BOOL Present( BOOL zClr = false ) = 0;

    // 初始化设备
    virtual HRESULT Initialize( IDirect3DDevice9 *dev ) = 0;

    // 解决设备丢失的问题， 新传入的设备必须是可用的设备（函数内部不负责设备重新创建）
    virtual BOOL OnDeviceLost() = 0;

    // 1 ： 设置是否使用2次Pass来解决alpha透明绘制顺序的问题！
    // 2 ： 设置剪裁矩形，由于该功能的特性，直接导致了最后z轴排序的不一致性
    //      所以，在设置该属性是，会自动和上一个剪裁状态做比较，如果不一致，则提前调用真实的绘制逻辑
    //      注意，这里的剪裁矩形是包含右下边缘线的，和原2DEngine的『0,0,799,599』的逻辑有1单位的差距！
    virtual BOOL SetOption( DWORD type, LPARAM param ) = 0;

    // 加载一个SPR文件
    virtual iLogicalSpr* LoadSpr( LPCSTR filename ) = 0;

    // 通过数据直接创建纹理，type为数据类型（1：565）
    virtual iLogicalImg* CreateImage( DWORD raw_w, DWORD raw_h, DWORD entity_w, DWORD entity_h, DWORD offset_x, DWORD offset_y, LPVOID &data, LPVOID &alpha, LPVOID &pal, DWORD type ) = 0;

    // 进行绘制简单图形操作，只是记录操作，到最后Present的时候一次性绘制到屏幕
    // 这里的深度值沿用上一次纹理操作的深度。
    virtual void DrawLine( int x1, int y1, int x2, int y2, DWORD color ) = 0;
    virtual void DrawBox( int x1, int y1, int x2, int y2, DWORD color ) = 0;

    // 文字的管理，使用文字纹理缓存，深度信息还是同简单图形一样，沿用上一次纹理操作的深度值
    virtual void DrawText( DWORD code, int x, int y, DWORD type, DWORD color ) = 0;

    // 外部表面的绘制
    virtual void DrawObj( iRenderObj *obj ) = 0;

    // 扩展功能，3D模型和动画的加载与绘制！
    virtual iMesh* LoadMesh( LPCSTR meshfile, LPCSTR bonefile ) = 0;
    virtual iAnimation* LoadAnimation( LPCSTR filename ) = 0;
};

#define RGB565_2_ARGB4444(a4bit,c)  ( WORD )(((a4bit)<<12)|(((c)&0xf000)>>4)|(((c)&0x780)>>3)|(((c)&0x1f)>>1))
#define RGB565_2_ARGB1555(a1bit,c)  ( WORD )(((a1bit)<<15)|(((c)&0xf800)>>1)|(((c)&0x7c0)>>1)|((c)&0x1f))
#define RGB565_2_ARGB8888(a8bit,c)  ( DWORD )(((a8bit)<<24)|(((c)&0xf800)>>8)|(((c)&0x7c0)<<5)|(((c)&0x1f)<<19))
//#define RGB565_2_ARGB8888EX(a8bit,c)  ( DWORD )( ((a8bit)<<24) | ((((c)&0xf800)>>8)|(((c)&0x7c0)<<5)|(((c)&0x1f)<<19)) | ((((((c)&0xf800)>>8)|(((c)&0x7c0)<<5)|(((c)&0x1f)<<19))>>5)&0x70707) )
#define RGB565_2_ARGB8888EX(a8bit,c)( RGB565_2_ARGB8888(a8bit,c) ) | ( ( RGB565_2_ARGB8888(a8bit,c)>>5 ) & 0x70707 )
#define RGB4444_2_ARGB8888(c)       ( DWORD )(((((c)&0xf000)<<16)|(((c)&0xf00)<<12)|(((c)&0xf0)<<8)|(((c)&0xf)<<4))|((((c)&0xf000)<<12)|(((c)&0xf00)<<8)|(((c)&0xf0)<<4)|(((c)&0xf)<<0)))
