#ifndef _JOE_JPEG_
#define _JOE_JPEG_
/*******************************************
Ԥ��֪ʶ��
    MCU  = ��С���뵥Ԫ����YUV�Ĳ����ʾ��������ؿ��С
    sryh = Y��ˮƽ�������� * 8 = ����MCU��ˮƽ��������
    sryv = Y�Ĵ�ֱ�������� * 8 = ����MCU�Ĵ�ֱ��������
    Y    = ����
    U    = Cb = ��ɫ������ֵ
    V    = Cr = ��ɫ������ֵ

��ʽת����
    ������ע��MCU�ص�������ʱ����õõ���
    ע��������ֵ��-128~127����������������ʱ��Ҫ��128����
    Y    = 0.299R + 0.587G + 0.114B;    Cb = 0.564(B �� Y );    Cr = 0.713(R �� Y );
    R    = Y + 1.402Cr;   G = Y - 0.344Cb - 0.714Cr;    B = Y + 1.772Cb;

ʹ��˵����
0�� ���JpegDecodeData
    ֱ��ʹ��ջ�ϵ����ݣ����߷�����������ݶ�����
    JpegDecodeData data;            or
    JpegDecodeData *dataptr = malloc(sizeof JpegDecodeData);

1�� ����Jpeg�ļ�ͷ��Ϣ�����Զ���ʼ��JpegDecodeData
    InitTags( �ļ��ڴ�ָ��, �ļ��ڴ��С, �Ƿ��ȡԤ��ͼ, dataptr )
    ע���жϷ���ֵ����ʼ��ʧ�ܵ�JpegDecodeData�ǲ������ڽ���ġ�

2�� ע��YUV�����������ݺ󣬿�ʼ����
    ��Ҫ���������� canvas, w, h, pitch, callback/format
    canvas, w, h, pitch ��4��������������������䴦��ģ�����
    callback/format 2ѡ1���������format�Ļ���Ĭ��ѡ�����õ���亯��
    FullDecode( JpegDecodeData );   or
    PartDecode( JpegDecodeData,x,y,w,h );

3�� ���ʹ�ù�PartDecode������Ҫ����MCU�������ݣ�
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

// JpegData ���е������ڳ�ʼ�����Ϊֻ��
typedef struct /*JpegData*/ {
    LPCBYTE lpJpegBuf;                  // �ļ��ڴ�ָ��
    DWORD JpegBufSize;                  // �ļ���С
    LPCBYTE lpDataSegment;              // ѹ��������ʼ��ַ
    DWORD ImgWidth, ImgHeight;          // ͼ���С
    DWORD MCU_width, MCU_height;        // ���뵥Ԫ�����ؿ��
    DWORD MCU_count_H, MCU_count_V;     // ��ˮƽ�ʹ�ֱ�����ϵı��뵥Ԫ����
    DWORD MCU_in_Entity;                // ÿ����С���뵥Ԫ�е�huffman�������ݵ�Ԫ����
    DWORD preview;                      // ��ǰ�����Ƿ���Ԥ��ͼ����

    //////////////////////////////////////////////////////////
    short SampRate_Y_H, SampRate_Y_V,   // Ԥ�ȼ������� 
        SampRate_U_H, SampRate_U_V,     // YUV �����������
        SampRate_V_H, SampRate_V_V,
        Y_in_MCU, U_in_MCU, V_in_MCU,   // ����MCU��YUV��DU��Ԫ����
        restart;   

    HuffmanTables huf_tabs[4];          // 0x40����
    short qt_table[3][64];              // 0x640����
    BYTE YDcIndex, YAcIndex, UVDcIndex, UVAcIndex, 
        YQtIndex, UQtIndex, VQtIndex, comp_num;
} JpegData;

typedef struct /*JpegDecodeData*/ {
    void  *canvas;
    DWORD w, h, pitch;
    DWORD format; // 0 �Զ����ʽ(DecodeCallbackMCU), 1 R5G6B5, 2 A1R5G5B5, 3 X8R8G8B8
    DWORD dummy;
    QTZZ  YUV[64*6];
    DecodeCallbackMCU callback; // ����ŵ�����棬��Ҫ�����������ʱ�ܹ���ʱ����
    void *MCU_index;            // ÿ�����뵥Ԫ�ĵ�ƫ���������ڿ���ָ���������
    DWORD sryh_bit, sryv_bit;
    JpegData jdata;
} JpegDecodeData;

static const int FUNC_OK            = 0;
static const int FUNC_MEMORY_ERROR  = -1;
static const int FUNC_FILE_ERROR    = -2;
static const int FUNC_FORMAT_ERROR  = -3;

// ����Jpeg�ļ����ݣ���ʼ�����ݱ�
int XENGINE_API InitTags( const void *jpegFileData, DWORD jpegFileSize, BOOL preview, JpegDecodeData *jdata );
// �����������ļ�ͼ��
int XENGINE_API FullDecode( JpegDecodeData *ddata ); 
// ֻ����һ���ֵ�ͼ�񣬵�һ�ν���ʱ�����Զ�����MCU������col/rowΪMCU�������꣬w/hΪMCU��λ
int XENGINE_API PartDecode( JpegDecodeData *ddata, DWORD col, DWORD row, DWORD w, DWORD h ); 
// ����MCU������
void XENGINE_API PartTheEnd( JpegDecodeData *ddata ); 
// Ӧ���� DecodeCallbackMCU �е�ȡYUV��ָ��
#define GetMCUYUV( _Y, _UV, _sryh, _sryv, _x, _y, _yy, _cb, _cr ) { \
    _cr = ((i>>sryv)*8)+(j>>sryh); \
    _yy = _Y[(((_y/8)<<_sryh)+(_x/8))*64+(_y&7)*8+(_x&7)] + 128; \
    _cb = _UV[_cr]; \
    _cr = _UV[_cr+64]; }

#endif

#define TEST 0 // ���Ʋ��Դ���
