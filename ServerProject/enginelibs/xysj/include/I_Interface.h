#pragma once
#include "I_Common.h"
#include <list>
#include <map>
#include <string>

class I_Effect;
class I_Part;
class I_AnimSet;
class I_Anim;
class I_Obj;
class I_Scene;
class I_SceneManager;
class I_Engine;
class I_CameraControl;
class I_CameraData;
class I_Camera;
class I_CameraManager;

/** 全局监听
*/
class XYSJ_DLL_EXPORT I_GlobalListener {
public:
	/** 在删除一个obj是触发
		参数：
			_scene			所在的场景
			_obj			删除的对象
		返回：
			<无>
	*/
	virtual void _notifyDeleteObj( I_Scene* _scene, I_Obj* _obj ) {}
	
	/** 在删除一个effect是触发
		参数：
			_scene			所在的场景
			_effect			删除的对象
		返回：
			<无>
	*/
	virtual void _notifyDeleteEffect( I_Scene* _scene, I_Effect* _effect ) {}
};

class XYSJ_DLL_EXPORT I_FrameListener {
public:
	virtual bool I_FrameStarted( const I_FrameEvent& _evt ) = 0;
	virtual bool I_FrameEnded( const I_FrameEvent& _evt ) = 0;
};

class XYSJ_DLL_EXPORT I_IEffect {
public:
	virtual const char*			I_getName() = 0;
	virtual void 				I_setEffectTemplate( const char* temlate ) = 0;
	virtual void				I_setScale( Point3 scale ) = 0;
	virtual void				I_setPosition( Point3 pos ) = 0;
	virtual void				I_setQuaternion( Point4 rot ) = 0;
	virtual void				I_setScale( float _x, float _y, float _z ) = 0;
	virtual void				I_setPosition( float _x, float _y, float _z ) = 0;
	virtual void				I_setQuaternion( float _w, float _x, float _y, float _z ) = 0;
	virtual bool				I_active() = 0;

	virtual void				I_putInScene() = 0;
	virtual void				I_bindTo( I_Obj* parent, const char* bone, Point3& offsetPos, Point4 rot ) = 0;
	virtual void				I_unBind() = 0;
};

class XYSJ_DLL_EXPORT I_Part {
public:
	virtual const char*			I_getName() = 0;
	virtual void				I_setMaterial( const char* material ) = 0;
	virtual void				I_setMesh( const char* mesh, const char* material = 0 ) = 0;
};

class XYSJ_DLL_EXPORT I_Anim {
public:
	virtual bool 				I_setAnimName( const char* anim ) = 0;
    virtual const char*			I_getAnimName() = 0;
	virtual void				I_setSpeed( float speed ) = 0;
    virtual float				I_getTimePosition() = 0;
    virtual void				I_setTimePosition( float timePos ) = 0;
    virtual float				I_getLength() = 0;
    virtual void				I_setLength( float len ) = 0;
    virtual float				I_getWeight() = 0;
    virtual void				I_setWeight( float weight ) = 0;
    virtual void				I_addTime( float offset ) = 0;
	virtual bool				I_hasEnded() = 0;
    virtual bool				I_getEnabled() = 0;
    virtual void				I_setEnabled( bool enabled ) = 0;
    virtual void				I_setLoop( bool loop ) = 0;
    virtual bool				I_getLoop() = 0;
	virtual I_AnimSet*			I_getParent() = 0;
	virtual bool				I_hasActive() = 0;
	virtual void				I_setManualUpdat( bool bmanual = true ) = 0;
	virtual bool				I_getManualUpdat() = 0;
};

class XYSJ_DLL_EXPORT I_AnimSetListener {
	virtual char* I_getSkelFile( const std::string& _AnimName ) = 0;
};

class XYSJ_DLL_EXPORT I_AnimSet {
public:
	virtual I_Anim*				I_createAnim( const char* name, const char* animName = 0 ) = 0;
	virtual I_Anim*				I_getAnim( const char* name ) = 0;
	virtual bool				I_hasAnim( const char* name ) = 0;
	virtual bool				I_hasAnimName( const char* _anim_name ) = 0;
	virtual void				I_removeAnim( const char* name ) = 0;
	virtual void				I_removeAllAnim() = 0;
	virtual void				I_getAllAnimName( std::vector<std::string>& _anims ) = 0;
	virtual void				I_addSkelFile( const char* anim, const char* file_name ) = 0;
	virtual bool				I_animEnable( const char* _anim_name ) = 0;
	virtual float				I_getAnimTimeLength( const char* _anim_name ) = 0;
	virtual bool				I_animActive() = 0;
	virtual void				I_setListener( I_AnimSetListener* _Listener ) = 0;
};

class XYSJ_DLL_EXPORT I_Band {
public:
	virtual const char*			I_getName() = 0;
	virtual void				I_setMaterialName( const char* _material ) = 0;
	virtual void				I_setAffectPoint( const char* _boneA, const char* _boneB, Point3 _offsA, Point3 _offsB, Point4 _rot ) = 0;
	virtual void				I_setEnable( bool _enable ) = 0;
	virtual bool				I_getEnable() = 0;
};
// 飘带管理器
class XYSJ_DLL_EXPORT I_BandSet {
public:
	virtual I_Band*				I_createBand( const char* _name ) = 0;
	virtual I_Band*				I_getBand( const char* _name ) = 0;
	virtual void				I_removeBand( const char* _name ) = 0;
	virtual void				I_clearBand() = 0;
};

class XYSJ_DLL_EXPORT I_Obj {
public:
	virtual const char*			I_getName() = 0;
	// part
	virtual	I_Part*				I_createPart( const char* name, const char* mesh = 0 ) = 0;
	virtual I_Part*				I_getPart( const char* name ) = 0;
	virtual bool				I_hasPart( const char* name ) = 0;
	virtual void				I_delPart( const char* name ) = 0;
	virtual void				I_removeAllPart() = 0;
	// get
	virtual I_AnimSet*			I_getAnimSet() = 0;
	virtual I_BandSet*			I_getBandSet() = 0;
	// control
	virtual void				I_setAABB( Point3& min, Point3& max ) = 0;
	// bind
	virtual void				I_putInScene() = 0;
	virtual void				I_bindTo( I_Obj* parent, const char* bone ) = 0;
	virtual void 				I_unBind() = 0;
	virtual void				I_setPosition( Point3& pos ) = 0;
	virtual void				I_setPosition( float x, float y, float z ) = 0;
	virtual Point3				I_getPosition() = 0;
	virtual void				I_setOrientation( Point4& rot ) = 0;
	virtual void				I_setOrientation( float w, float x, float y, float z ) = 0;
	virtual void				I_setScale( Point3& scale ) = 0;
	virtual void				I_setScale( float x, float y, float z ) = 0;
	virtual void			    I_setVisible( bool visible ) = 0;
	virtual bool			    I_getVisible() = 0;
	// other
	/** 该obj是否被鼠标选中
		有可能出现这样一种情况，鼠标同时选中多个obj，
		而有时候又需要这样的处理，比如：鼠标同时选中 
		Npc Player Item 以前的逻辑是默认选定一个离摄
		像机最近的那个obj。现在根据实际的使用需求，
		返回选中的优先级（按离射线机的距离来定的）
		参数：
			<无>
		返回：
			int				0: 没有被选中
							1: 第一个被选中
							2: 第二个被选中
							3: （依此类推）
	*/
	virtual int					I_isSelect() = 0;
	virtual bool				I_isSelect( int x, int y ) = 0;
	virtual void				I_setColor( float r, float g, float b ) = 0;
	virtual void				I_setColor( int r, int g, int b ) = 0;
	virtual void				I_setMaterialType( bool base ) = 0;
	virtual BOOL			    I_makeDummy( RENDER_DUMMY funcDummy, LPARAM param ) = 0;
	virtual Point2				I_getCenterXY() = 0;
	/** 获取Obj的世界坐标点。
		计算方式：x z 取包围盒xz的中心点，y 去包围盒y的最高点
		参数：
			<无>
		返回：
			Point3				当前帧的世界坐标点
	*/
	virtual Point3				I_getCenterWorld() = 0;

	virtual void				I_setFrameListener( I_FrameListener* _frame_listener ) = 0;
	/** 设置是否开启实时阴影
		这个阴影是纹理调试阴影
		计算方式：x z 取包围盒xz的中心点，y 去包围盒y的最高点
		参数：
			_enable				是否开启
		返回：
			<无>
	*/
	virtual void				I_setEnableShadow( bool _enable ) = 0;
};

class XYSJ_DLL_EXPORT I_Scene {
public:
	virtual const char*			I_getName() = 0;
	virtual bool				I_active() = 0;
	virtual void				I_setActive( bool active = true ) = 0;
	virtual void				I_setDefaultCamera( I_Camera* pCamera ) = 0;
// obj
	virtual I_Obj* 				I_createOBJ( const std::string& _Name ) = 0;
	virtual I_Obj* 				I_getOBJ( const std::string& _Name ) = 0;
	virtual bool 				I_hasOBJ( const std::string& _Name ) = 0;
	virtual void				I_deleteOBJ( const std::string& _Name ) = 0;
	virtual void 				I_removeAllOBJ() = 0;
//	virtual unsigned int		I_getSelectId() = 0;

// efect
	virtual I_IEffect* 			I_createEffect( const std::string& _Name ) = 0;
	virtual I_IEffect* 			I_createEffect() = 0;
	virtual I_IEffect* 			I_getEffect( const std::string& _Name ) = 0;
	virtual void				I_removeEffect( const std::string& _Name ) = 0;
	virtual void				I_removeAllEffect() = 0;
	virtual int					I_getSelectObjs( I_Obj** _seclect, int _max = 10 ) = 0;

	/** 设置是否允许多线程读取资源
		单线程读取一般只有再CG场景中才使用了哦
	*/
	virtual void				I_setEnableMultiThread( bool _enable ) = 0;
	virtual bool				I_getEnableMultiThread() = 0;
};

class XYSJ_DLL_EXPORT I_SceneManager {
public:
	virtual I_Scene* 			I_createScene( const char* name ) = 0;
	virtual I_Scene* 			I_getScene( const char* name ) = 0;
	virtual void 				I_delScene( const char* name ) = 0;
	virtual void 				I_removeAll() = 0;

	virtual void				I_setSky( bool enable, const char* material, float distance ) = 0;
	/** 描述：
			添加一个帧监听，在每帧渲染前和渲染后都调用一次，
		参数：
			_frame_listener：		帧监听对象指针
			_update_after：			在更新前还是更新后 _update_after=true 更新后 _update_after=false 更新前
		返回：
			<无>
	*/
	virtual void				I_addFrameListener( I_FrameListener* _frame_listener, bool _update_after = true ) = 0;
};

class XYSJ_DLL_EXPORT I_Engine {
public:
	static I_Engine* 			I_Instance();
	static void					I_Destroy();
	
	virtual bool 				I_begin( const sEngineConfig& cf) = 0;
	virtual bool 				I_update() = 0;
	virtual void 				I_end() = 0;
	
	virtual void* 				I_getDevice() = 0;
	virtual I_SceneManager* 	I_getSceneMgr() = 0;
	virtual I_CameraManager*	I_getCameraMgr() = 0;
	virtual void 				I_setGraphicsOP() = 0;
	virtual void 				I_getGraphicsOP() = 0;
	
	virtual bool 				I_initData() = 0;
	virtual void 				I_zoorm( float precent ) = 0;
	virtual int 				I_getAntiAliasing( char* sv ) = 0;
	
	virtual void 				I_setRenderOption( const char* aalevel, unsigned int width, unsigned int height ) = 0;
	virtual void                I_addResourceLocation( const char* name, const char* type, const char* group ) = 0;
	virtual	void				I_setAmbientLight( const Point3& color ) = 0;
	virtual void				I_setDirLight( const Point3& color, const Point3& dir, bool _EnableShadow = false ) = 0;
	virtual void				I_setSpotLight( int _innerAngle, int _outerAngle, Point3 _pos, Point3 _dir, Point3 _colour ) = 0;
	virtual void				I_setClearEveryFrame( bool value = true ) = 0;

	virtual void				I_setLogLevel( eLogLevel level ) = 0;
	virtual void				I_zoom( float w, float h, I_Camera* _pCamera = 0 ) = 0;
	virtual void				I_resize( int _w, int _h, bool _scale = false) = 0;

	// other
	virtual const std::map<std::string,bool>& I_getEffectScriptList() = 0;
};

class XYSJ_DLL_EXPORT I_CameraControl {
public:
	virtual void				I_move( const Point3& pos ) = 0;
	virtual void				I_moveRelative( const Point3& vec ) = 0;
	virtual void				I_setPosition( const Point3& pos ) = 0;
	virtual void				I_setPosition( const float x, const float y, const float z ) = 0;
	virtual Point3				I_getPosition() = 0;
	virtual void				I_setDirection( const Point3& dir ) = 0;
	virtual Point3				I_getDirection() = 0;
	virtual void				I_lookAt( const Point3& dest ) = 0;
	virtual void				I_lookAt( const float x, const float y, const float z ) = 0;
	virtual void				I_roll( float angle ) = 0;
	virtual void				I_yaw( float angle ) = 0;
	virtual void				I_pitch( float angle) = 0;
	virtual void				I_rotate( const Point4& q ) = 0;
	virtual void				I_setFixedYawAxis( bool useFixed, const Point3& fixedAxis ) = 0;
	virtual void				I_zoom( float z ) = 0;
	virtual void				I_setAspectRatio( float _aspect_ratio ) = 0;
};

class XYSJ_DLL_EXPORT I_CameraData {
public:
	virtual void				I_lock( TexData* data ) = 0;
	virtual void				I_unLock() = 0;
};

class XYSJ_DLL_EXPORT I_Camera {
public:
	virtual const char*			I_getName() = 0;
	virtual I_CameraControl*	I_getCameraControl() = 0;
	virtual I_CameraData*		I_getCameraData() = 0;

	virtual void				I_setProjectionType( eProjectionType pt ) = 0;
	virtual void				I_setNearClipDistance( float fNear ) = 0;
	virtual void				I_setFarClipDistance( float fFar ) = 0;
	virtual void				I_setWH( int w, int h ) = 0;
	virtual void				I_setPolygonMode( ePolygonMode pm ) = 0;
	virtual void				I_setRenderTargetType( eRenderTargetType rtt ) = 0;
	virtual void				I_setBackColor( const Point3& color ) = 0;
	virtual void				I_setClearEveryFrame( const bool value = true ) = 0;
	virtual void				I_setCustomProjectionMatrix( void* matrix ) = 0;

	virtual void				I_setMousePos( int x, int y ) = 0;
	virtual Point2				I_getMousePos() = 0;
	virtual void*               I_GetRttTex() = 0;
	virtual void				I_setBackTex(const char* texName) = 0;
};

class XYSJ_DLL_EXPORT I_CameraManager {
public:
	virtual I_Camera*			I_createCamera( const char* name ) = 0;
	virtual I_Camera*			I_getCamera( const char* name ) = 0;
	virtual void				I_delCamera( const char* name ) = 0;
	virtual	void				I_removeAllCamrea() = 0;
};                                                            



#define I_engine I_Engine::I_Instance()