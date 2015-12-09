#pragma once
#include <string>
#include <vector>
#include <map>
#include <windows.h>


#define XYSJ_DLL_EXPORT __declspec(dllexport)
typedef void ( __stdcall *RS_LISTENER )( const std::string&, const std::map<std::string,std::string>* );	
typedef void ( __stdcall *RENDER_DUMMY )( LPARAM param );

struct sEngineConfig {
	int hWnd;             // 窗口句柄
	HINSTANCE hInstance;   // 示例句柄
	unsigned int width;           // 窗口的宽
	unsigned int height;          // 窗口的高
	bool bFullScreen;      // 是否全屏
	bool bPresent;
	RS_LISTENER rsListener;

	char level[128];
	char plugin[128];
	char config[128];
	char log[128];
	char resource[128];
};

struct Point4 {
	//Point4(){}
	//Point4( float _w, float _x, float _y, float _z ):
	//w(_w),
	//x(_x),
	//y(_y),
	//z(_z)
	//{
	//}

	bool operator == ( Point4& _p )
	{
		return
			x==_p.x&&
			y==_p.y&&
			z==_p.z&&
			w==_p.w;
	}
	
	Point4 operator * ( Point4& _p )
	{
		Point4 __p;

		__p.w = w * _p.w - x * _p.x - y * _p.y - z * _p.z,
		__p.x = w * _p.x + x * _p.w + y * _p.z - z * _p.y,
		__p.y = w * _p.y + y * _p.w + z * _p.x - x * _p.z,
		__p.z = w * _p.z + z * _p.w + x * _p.y - y * _p.x;

		return __p;
	}

	float w,x,y,z;
};

struct XYSJ_DLL_EXPORT Point3 {
	bool operator == ( Point3& _p )
	{ return x==_p.x&&y==_p.y&&z==_p.z;}

	Point3 operator +( Point3 &_pos )
	{
		Point3 __rp;
		__rp.x = x  + _pos.x;
		__rp.y = y  + _pos.y;
		__rp.z = z  + _pos.z;

		return __rp;
	}

	float x,y,z;
};

struct Point2 {
	float x,y;
};

struct ColourValue {
	float r,g,b,a;
};

enum eProjectionType {
    EPT_ORTHOGRAPHIC,
    EPT_PERSPECTIVE
};

enum ePolygonMode {
    EPM_POINTS = 1,
    EPM_WIREFRAME = 2,
    EPM_SOLID = 3
};

enum eRenderTargetType {
	RTT_D3D,
	RTT_TEX,
	RTT_OTHER
};
enum eCameraType {
	CT_D3D,
	CT_D2D
};

enum eLogLevel {
	ELL_LOW = 1,
	ELL_NORMAL = 2,
	ELL_BOREME = 3
};

enum eExceptionCodes {
    EERR_CANNOT_WRITE_TO_FILE,
    EERR_INVALID_STATE,
    EERR_INVALIDPARAMS,
    EERR_RENDERINGAPI_ERROR,
    EERR_DUPLICATE_ITEM,
    EERR_ITEM_NOT_FOUND,
    EERR_FILE_NOT_FOUND,
    EERR_INTERNAL_ERROR,
    EERR_RT_ASSERTION_FAILED, 
	EERR_NOT_IMPLEMENTED
};

struct TexData{
	void** data;
	WORD w,h;
	WORD paix;
};

struct I_FrameEvent {
	float timeSinceLastEvent;
	float timeSinceLastFrame;
};
/////////////////////////////////////////////////////////////////////////////////////
extern "C" {
	sEngineConfig XYSJ_DLL_EXPORT I_getDefaultEngineConfig();
	Point4 XYSJ_DLL_EXPORT I_getDir8( int idx, bool _idx = 0 );
	Point4 XYSJ_DLL_EXPORT I_getDir360( int idx, bool _idx = 0 );
	void XYSJ_DLL_EXPORT I_throw( eExceptionCodes& _codes, std::string& _description, std::string _function );
	void XYSJ_DLL_EXPORT I_gLog( std::string _description, eLogLevel _level = ELL_NORMAL );
	void XYSJ_DLL_EXPORT I_split( std::string _str, std::string _type, std::vector<std::string>& _vectors );

	Point4 XYSJ_DLL_EXPORT I_pitch( Point4 _p4, int _angle );
	Point4 XYSJ_DLL_EXPORT I_yaw( Point4 _p4, int _angle );
	Point4 XYSJ_DLL_EXPORT I_roll( Point4 _p4, int _angle );
}