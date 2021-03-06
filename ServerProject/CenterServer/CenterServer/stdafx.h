// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: 在此处引用程序要求的附加头文件
#include <mmsystem.h>
#include <map>
//#include <string>
#include <list>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "pub/thread.h"
#include "pub/traceinfo.h"
#include "pub/objectmanager.h"
#include "pub/dwt.h"

#include "networkmodule\netprovidermodule\netprovidermodule.h"
#include "networkmodule\playertypedef.h"
