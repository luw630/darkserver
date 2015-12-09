#ifndef TexImg_H
#define TexImg_H

class XENGINE_API TexImg
{
public:
	LPVOID	    imageData;
    LPVOID      texImg; // 3Dʹ�õĶ�����
	WORD		rw, rh, ew, eh;
	BYTE		ox;
	BYTE		oy;
	BYTE		type;   // ���ݸ�ʽ���� 0:4444 1:565->1555 2:1555 3:8888

	TexImg();
	~TexImg();
	LPVOID		Init( DWORD raw_w, DWORD raw_h, DWORD entity_w, DWORD entity_h, DWORD offset_x, DWORD offset_y, int type );
	void		Free();
	void		Draw(int nX, int nY, DWORD ex_argb );
	void		DrawZoom(RECT *rect, DWORD ex_argbTop, DWORD ex_argbBottom );
	// ֻ����һ���֣����ڽ�����
	void		DrawProgress(int nX, int nY, DWORD len, DWORD ex_argb );
};

#endif
