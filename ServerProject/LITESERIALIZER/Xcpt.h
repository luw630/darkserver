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

    DeclareXcpt( NullPointer,    0x00000000, "�Ƿ�ʹ�ÿ�ָ��" )
    DeclareXcpt( FullBuffer,     0x00000001, "����������" )
    DeclareXcpt( InvalidConvert, 0x00000002, "��Ч��ת��" )
    DeclareXcpt( NotInitialize,  0x00000003, "ʹ�õĶ���û�г�ʼ��" )
    DeclareXcpt( CurrentIsEnd,   0x00000004, "��ǰ�Ѿ��ǽ�β��" )
    DeclareXcpt( BadDataSize,    0x00000005, "Variantת��ʱ�����ݴ�С�쳣" )
    DeclareXcpt( BadString,      0x00000006, "Variantת��ʱ��������ַ����쳣" )
    DeclareXcpt( InvalidBuffer,  0x00000007, "��Ч�Ļ�����" )
    DeclareXcpt( BadArrayType,   0x00000008, "���л�����ֻ֧�̶ֹ���������" )
};
