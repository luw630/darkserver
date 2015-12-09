#pragma once
#define WIN32_LEAN_AND_MEAN		// �� Windows ͷ���ų�����ʹ�õ�����
#include "windows.h"

#include <assert.h>
#include "d3d9.h"
#include "..\DirectX9\Include\d3dx9.h"
//#include "d3dx9.h"
#include "win32.h"

// TODO: �ڴ˴����ó���Ҫ��ĸ���ͷ�ļ�

// ������ȹ���
// ���ڻ����㷨���ص㣬�������Ƶ�˳����Ѿ�������Ϊ�����Ϣ��
// ���ǣ�������Ȼ��������޵ģ�����������ƺ����ļ�ͼ�β���ȫ��ʹ�ú���һ��������ͬ�����

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
    // x��y��z�Ͳ�˵�ˣ�ani�Ǹ�ģ�Ͷ�Ӧ�Ķ������ݣ�scale������Ϊ��λ�������ţ�f֡����С��λ������֡���ֵ���㡣����
    virtual void Show( int x, int y, DWORD z, iAnimation *ani, float f, DWORD scale, DWORD color ) = 0;
};

struct XENGINE_API i3dxMgr {

    static i3dxMgr* GetInstance();
    static IDirect3DDevice9* GetDevice();

    // �����Ļ�ͼ�߼���ǰ������л�����Ϊ��ֻ���ڼ�¼��������
    virtual BOOL Present( BOOL zClr = false ) = 0;

    // ��ʼ���豸
    virtual HRESULT Initialize( IDirect3DDevice9 *dev ) = 0;

    // ����豸��ʧ�����⣬ �´�����豸�����ǿ��õ��豸�������ڲ��������豸���´�����
    virtual BOOL OnDeviceLost() = 0;

    // 1 �� �����Ƿ�ʹ��2��Pass�����alpha͸������˳������⣡
    // 2 �� ���ü��þ��Σ����ڸù��ܵ����ԣ�ֱ�ӵ��������z������Ĳ�һ����
    //      ���ԣ������ø������ǣ����Զ�����һ������״̬���Ƚϣ������һ�£�����ǰ������ʵ�Ļ����߼�
    //      ע�⣬����ļ��þ����ǰ������±�Ե�ߵģ���ԭ2DEngine�ġ�0,0,799,599�����߼���1��λ�Ĳ�࣡
    virtual BOOL SetOption( DWORD type, LPARAM param ) = 0;

    // ����һ��SPR�ļ�
    virtual iLogicalSpr* LoadSpr( LPCSTR filename ) = 0;

    // ͨ������ֱ�Ӵ�������typeΪ�������ͣ�1��565��
    virtual iLogicalImg* CreateImage( DWORD raw_w, DWORD raw_h, DWORD entity_w, DWORD entity_h, DWORD offset_x, DWORD offset_y, LPVOID &data, LPVOID &alpha, LPVOID &pal, DWORD type ) = 0;

    // ���л��Ƽ�ͼ�β�����ֻ�Ǽ�¼�����������Present��ʱ��һ���Ի��Ƶ���Ļ
    // ��������ֵ������һ�������������ȡ�
    virtual void DrawLine( int x1, int y1, int x2, int y2, DWORD color ) = 0;
    virtual void DrawBox( int x1, int y1, int x2, int y2, DWORD color ) = 0;

    // ���ֵĹ���ʹ�����������棬�����Ϣ����ͬ��ͼ��һ����������һ��������������ֵ
    virtual void DrawText( DWORD code, int x, int y, DWORD type, DWORD color ) = 0;

    // �ⲿ����Ļ���
    virtual void DrawObj( iRenderObj *obj ) = 0;

    // ��չ���ܣ�3Dģ�ͺͶ����ļ�������ƣ�
    virtual iMesh* LoadMesh( LPCSTR meshfile, LPCSTR bonefile ) = 0;
    virtual iAnimation* LoadAnimation( LPCSTR filename ) = 0;
};

#define RGB565_2_ARGB4444(a4bit,c)  ( WORD )(((a4bit)<<12)|(((c)&0xf000)>>4)|(((c)&0x780)>>3)|(((c)&0x1f)>>1))
#define RGB565_2_ARGB1555(a1bit,c)  ( WORD )(((a1bit)<<15)|(((c)&0xf800)>>1)|(((c)&0x7c0)>>1)|((c)&0x1f))
#define RGB565_2_ARGB8888(a8bit,c)  ( DWORD )(((a8bit)<<24)|(((c)&0xf800)>>8)|(((c)&0x7c0)<<5)|(((c)&0x1f)<<19))
//#define RGB565_2_ARGB8888EX(a8bit,c)  ( DWORD )( ((a8bit)<<24) | ((((c)&0xf800)>>8)|(((c)&0x7c0)<<5)|(((c)&0x1f)<<19)) | ((((((c)&0xf800)>>8)|(((c)&0x7c0)<<5)|(((c)&0x1f)<<19))>>5)&0x70707) )
#define RGB565_2_ARGB8888EX(a8bit,c)( RGB565_2_ARGB8888(a8bit,c) ) | ( ( RGB565_2_ARGB8888(a8bit,c)>>5 ) & 0x70707 )
#define RGB4444_2_ARGB8888(c)       ( DWORD )(((((c)&0xf000)<<16)|(((c)&0xf00)<<12)|(((c)&0xf0)<<8)|(((c)&0xf)<<4))|((((c)&0xf000)<<12)|(((c)&0xf00)<<8)|(((c)&0xf0)<<4)|(((c)&0xf)<<0)))
