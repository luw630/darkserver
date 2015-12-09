//---------------------------------------------------------------------------
//  Engine (c) 1999-2000 by Dreamwork
//
// File:	Codec.h
// Date:	2003.07.08
// Code:	Dreamwork
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef Codec_H
#define Codec_H
//---------------------------------------------------------------------------
#define CODEC_NONE		0
#define CODEC_LHA		1
#define CODEC_LZO		2
//---------------------------------------------------------------------------
typedef struct {
	PBYTE		lpData;			// ��ѹ��&ѹ��ǰ ����ָ��
	DWORD		dwDataLen;		// ��ѹ��&ѹ��ǰ ���ݳ���
	PBYTE		lpPack;			// ѹ����&ѹ���� ����ָ��
	DWORD		dwPackLen;		// ѹ����&ѹ���� ���ݳ���
} TCodeInfo;
//---------------------------------------------------------------------------
class ENGINE_API Codec
{
public:
	virtual DWORD	GetPackLen(DWORD dwDataLen);
	virtual BOOL	Encode(TCodeInfo* pCodeInfo);
	virtual BOOL	Decode(TCodeInfo* pCodeInfo);
	//lpData;dwDataLen;lpPack;������ȷ���ã�dwPackLen��Ϊ0����	
};
//---------------------------------------------------------------------------
ENGINE_API void	g_InitCodec(Codec** ppCodec, int nCompressMethod);
ENGINE_API void	g_FreeCodec(Codec** ppCodec, int nCompressMethod);
//---------------------------------------------------------------------------
#endif
