#pragma once

#define DeclareXcpt(XcptName, ErrCode, ErrInfo) class XcptName : public Xcpt { \
    public : XcptName() : Xcpt( ErrCode ) {} LPCSTR GetErrInfo() { return ErrInfo; } };

namespace lite
{

    struct Xcpt
    {
        virtual LPCSTR GetErrInfo() = 0;

        Xcpt( DWORD errCode ) : errCode( errCode ) {}

        DWORD errCode;
    };

    DeclareXcpt( NullPointer,    0x00000000, "非法使用空指针" )
    DeclareXcpt( FullBuffer,     0x00000001, "缓冲区已满" )
    DeclareXcpt( InvalidConvert, 0x00000002, "无效的转换" )
    DeclareXcpt( NotInitialize,  0x00000003, "使用的对象没有初始化" )
    DeclareXcpt( CurrentIsEnd,   0x00000004, "当前已经是结尾了" )
    DeclareXcpt( BadDataSize,    0x00000005, "Variant转换时，数据大小异常" )
    DeclareXcpt( BadString,      0x00000006, "Variant转换时，传入的字符串异常" )
    DeclareXcpt( InvalidBuffer,  0x00000007, "无效的缓冲区" )
    DeclareXcpt( BadArrayType,   0x00000008, "序列化数组只支持固定长度类型" )
};
