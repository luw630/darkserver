#ifndef _JOE_JPEG_
#define _JOE_JPEG_
/*******************************************
预备知识：
    MCU  = 最小编码单元，由YUV的采样率决定其像素块大小
    sryh = Y的水平采样次数 * 8 = 单个MCU的水平像素数量
    sryv = Y的垂直采样次数 * 8 = 单个MCU的垂直像素数量
    Y    = 亮度
    U    = Cb = 蓝色分量差值
    V    = Cr = 红色分量差值

格式转换：
    在自行注册MCU回调函数的时候会用得到。
    注意解出来的值是-128~127，所以亮度在运算时需要加128处理
    Y    = 0.299R + 0.587G + 0.114B;    Cb = 0.564(B － Y );    Cr = 0.713(R － Y );
    R    = Y + 1.402Cr;   G = Y - 0.344Cb - 0.714Cr;    B = Y + 1.772Cb;

使用说明：
0） 获得JpegDecodeData
    直接使用栈上的数据，或者分配出来的数据都可以
    JpegDecodeData data;            or
    JpegDecodeData *dataptr = malloc(sizeof JpegDecodeData);

1） 加载Jpeg文件头信息，会自动初始化JpegDecodeData
    InitTags( 文件内存指针, 文件内存大小, 是否获取预览图, dataptr )
    注意判断返回值，初始化失败的JpegDecodeData是不能用于解码的。

2） 注册YUV处理函数和数据后，开始解码
    需要填充的数据有 canvas, w, h, pitch, callback/format
    canvas, w, h, pitch 这4个数据是用来做像素填充处理的，必填
    callback/format 2选1，如果是填format的话，默认选择内置的填充函数
    FullDecode( JpegDecodeData );   or
    PartDecode( JpegDecodeData,x,y,w,h );

3） 如果使用过PartDecode，则需要销毁MCU索引数据！
    PartTheEnd( JpegDecodeData )
*******************************************/
#include "Win32.h"
typedef void ( __stdcall *DecodeCallbackMCU )( void *decodedata, DWORD x, DWORD y );
typedef int QTZZ, *LPQTZZ;
typedef const BYTE *LPCBYTE;

typedef struct /*HuffmanTables*/ {
    BYTE code_value_table[256];
    WORD code_pos_table[16];
    WORD huf_max_value[16];
    WORD huf_min_value[16];
    WORD code_len_table[16];
} HuffmanTables;

// JpegData 所有的数据在初始化后均为只读
typedef struct /*JpegData*/ {
    LPCBYTE lpJpegBuf;                  // 文件内存指针
    DWORD JpegBufSize;                  // 文件大小
    LPCBYTE lpDataSegment;              // 压缩数据起始地址
    DWORD ImgWidth, ImgHeight;          // 图像大小
    DWORD MCU_width, MCU_height;        // 编码单元的像素宽高
    DWORD MCU_count_H, MCU_count_V;     // 在水平和垂直方向上的编码单元数量
    DWORD MCU_in_Entity;                // 每个最小编码单元中的huffman编码数据单元数量
    DWORD preview;                      // 当前数据是否是预览图数据

    //////////////////////////////////////////////////////////
    short SampRate_Y_H, SampRate_Y_V,   // 预先计算数据 
        SampRate_U_H, SampRate_U_V,     // YUV 横纵向采样率
        SampRate_V_H, SampRate_V_V,
        Y_in_MCU, U_in_MCU, V_in_MCU,   // 单个MCU中YUV的DU单元个数
        restart;   

    HuffmanTables huf_tabs[4];          // 0x40对齐
    short qt_table[3][64];              // 0x640对齐
    BYTE YDcIndex, YAcIndex, UVDcIndex, UVAcIndex, 
        YQtIndex, UQtIndex, VQtIndex, comp_num;
} JpegData;

typedef struct /*JpegDecodeData*/ {
    void  *canvas;
    DWORD w, h, pitch;
    DWORD format; // 0 自定义格式(DecodeCallbackMCU), 1 R5G6B5, 2 A1R5G5B5, 3 X8R8G8B8
    DWORD dummy;
    QTZZ  YUV[64*6];
    DecodeCallbackMCU callback; // 这个放到最后面，主要是用于在溢出时能够及时出错！
    void *MCU_index;            // 每个编码单元的的偏移量，用于快速指定区域编码
    DWORD sryh_bit, sryv_bit;
    JpegData jdata;
} JpegDecodeData;

static const int FUNC_OK            = 0;
static const int FUNC_MEMORY_ERROR  = -1;
static const int FUNC_FILE_ERROR    = -2;
static const int FUNC_FORMAT_ERROR  = -3;

// 解析Jpeg文件数据，初始化数据表
int XENGINE_API InitTags( const void *jpegFileData, DWORD jpegFileSize, BOOL preview, JpegDecodeData *jdata );
// 解码完整的文件图像
int XENGINE_API FullDecode( JpegDecodeData *ddata ); 
// 只解码一部分的图像，第一次解码时，会自动创建MCU索引表，col/row为MCU行列坐标，w/h为MCU单位
int XENGINE_API PartDecode( JpegDecodeData *ddata, DWORD col, DWORD row, DWORD w, DWORD h ); 
// 销毁MCU索引表
void XENGINE_API PartTheEnd( JpegDecodeData *ddata ); 
// 应用于 DecodeCallbackMCU 中的取YUV宏指令
#define GetMCUYUV( _Y, _UV, _sryh, _sryv, _x, _y, _yy, _cb, _cr ) { \
    _cr = ((i>>sryv)*8)+(j>>sryh); \
    _yy = _Y[(((_y/8)<<_sryh)+(_x/8))*64+(_y&7)*8+(_x&7)] + 128; \
    _cb = _UV[_cr]; \
    _cr = _UV[_cr+64]; }

#endif

#define TEST 0 // 控制测试代码
