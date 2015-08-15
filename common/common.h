#ifndef COMMON_HPP_
#define COMMON_HPP_

#include "ace/pre.h"
#include "ace/OS.h" //includes all the system headers
#include "ace/Basic_Types.h"
#include "ace/UUID.h"

//include windows headers
#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
#pragma warning(disable:4996)
#pragma warning(disable:4819)
#pragma warning(disable:4049)
#pragma warning(disable:4217)
#pragma warning(disable:4006)
#pragma warning(disable:4005)
#pragma warning(disable:4244)
#pragma warning(disable:4002)
#pragma warning(disable:4102)
#pragma warning (disable:4910)
#pragma warning (disable:4251)
#pragma warning (disable:4661)
#include <winsock2.h>		// 必须在windows.h之前包含， 否则网络模块编译会出错
#include <mswsock.h> 
#include <windows.h> 
#include <unordered_map>
#include <functional>
#include <memory>
#define _SCL_SECURE_NO_WARNINGS
#else
#include <tr1/unordered_map>
#include <tr1/functional>
#include <tr1/memory>
#endif

#include "kbe_version.h"
#include "config.h"

#include <string>  
#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <limits>
#include <algorithm>
#include <utility>
#include <functional>
#include <cctype> //used to tell a char capital or little
#include <iterator>

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
////////////////////////////////////////// 当前系统字节序 byte order of current os ///////////////////////////////////////
#define KBE_LITTLE_ENDIAN							0
#define KBE_BIG_ENDIAN								1
#if  (ACE_BYTE_ORDER == ACE_LITTLE_ENDIAN)
#define KBE_BYTE_ORDR KBE_LITTLE_ENDIAN
#else 
#define KBE_BYTE_ORDR KBE_BIG_ENDIAN
#endif 

#if (KBE_BYTE_ORDR == KBE_LITTLE_ENDIAN)
/*
* Accessing individual byte (int8) and word (int16) within word and dword (int32).
* w = word = 2 butes,
* dw = double word = 4 bytes
*/

/// Returns the high byte of a word.
#define HIGH_BYTE_W(b)		        (((b) & 0xff00) >> 8)
/// Returns the low byte of a word.
#define LOW_BYTE_W(b)		       ( (b) & 0xff)
/// Returns the high byte of a dw.
#define HIGH_BYTE_DW(b)		    (((b) & 0xff000000L) >> 24)
/// Returns the low byte of a dw.
#define LOW_BYTE_DW(b)		        ( (b) & 0xffL)

/// Returns byte 0 of a dw.
#define BYTE0DW(b)		           ( (b) & 0xffL)
/// Returns byte 1 of a dw.
#define BYTE1DW(b)		           (((b) & 0xff00L) >> 8)
/// Returns byte 2 of a dw.
#define BYTE2DW(b)		           (((b) & 0xff0000L) >> 16)
/// Returns byte 3 of a dw.
#define BYTE3DW(b)		           (((b) & 0xff000000L) >> 24)

/// Returns the high word of a dw.
#define HIGH_WORD_DW(b)		(((b) & 0xffff0000L) >> 16)
/// Returns the low word of a dw.
#define LOW_WORD_DW(b)		( (b) & 0xffffL)

//This macro takes a dword ordered 0123 and reorder it to 3210. dw = 4 bytes
#define KBE_SWAP_DW(dw)	         ACE_SWAP_LONG(dw)
#else
/* big endian macros go here */
#endif

///判断平台是否为小端字节序 
inline bool kbe_is_little_endian()
{
#if (KBE_BYTE_ORDR == KBE_LITTLE_ENDIAN)
	return true;
#else
	return false;
#endif
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////// ensure current platform and text micros //////////////////////////////////
#define PLATFORM_WIN32 0
#define PLATFORM_UNIX  1
#define PLATFORM_APPLE 2

#define UNIX_FLAVOUR 0
#define UNIX_FLAVOUR_LINUX 1
#define UNIX_FLAVOUR_BSD 2
#define UNIX_FLAVOUR_OTHER 3
#define UNIX_FLAVOUR_OSX 4

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
#define KBE_PLATFORM PLATFORM_WIN32
#define KBE_PLATFORM_TEXT "Win32"
#define KBE_MIN(a,b) min(a,b)
#define KBE_MAX(a,b) max(a,b)
#pragma warning (disable : 4910)
#pragma warning (disable : 4251)
#elif defined( __INTEL_COMPILER )
#define KBE_PLATFORM PLATFORM_INTEL
#elif defined( __APPLE_CC__ )
#define KBE_PLATFORM PLATFORM_APPLE
#undef UNIX_FLAVOUR
#define UNIX_FLAVOUR UNIX_FLAVOUR_OSX
#else
#define KBE_PLATFORM PLATFORM_UNIX
#undef UNIX_FLAVOUR
#ifdef USE_KQUEUE
#define KBE_PLATFORM_TEXT "FreeBSD"
#define UNIX_FLAVOUR UNIX_FLAVOUR_BSD
#else
#ifdef USE_KQUEUE_DFLY
#define KBE_PLATFORM_TEXT "DragonFlyBSD"
#define UNIX_FLAVOUR UNIX_FLAVOUR_BSD
#else
#define KBE_PLATFORM_TEXT "Linux"
#define UNIX_FLAVOUR UNIX_FLAVOUR_LINUX
#endif
#endif
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////// ensure current complier ////////////////////////////////////////////////
#define COMPILER_MICROSOFT 0
#define COMPILER_GNU	   1
#define COMPILER_BORLAND   2
#define COMPILER_INTEL     3
#ifdef _MSC_VER
#  define KBE_COMPILER COMPILER_MICROSOFT
#elif defined( __INTEL_COMPILER )
#  define KBE_COMPILER COMPILER_INTEL
#elif defined( __BORLANDC__ )
#  define KBE_COMPILER COMPILER_BORLAND
#elif defined( __GNUC__ )
#  define KBE_COMPILER COMPILER_GNU
#else
#  pragma error "FATAL ERROR: Unknown compiler."
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////// kbe debug or release ///////////////////////////////////////////////////
#ifndef KBE_CONFIG
#ifdef _DEBUG
#define KBE_CONFIG "Debug"
#else
#define KBE_CONFIG "Release"
#endif
#endif
/*kbe build 64 or 32 processor*/
#ifdef X64
#define KBE_ARCH "X64"
#else
#define KBE_ARCH "X86"
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////// Print Formate ///////////////////////////////////////////////////////////////
#define PRTime													ACE_INT64_FORMAT_SPECIFIER
#define PRIAPPID													ACE_UINT64_FORMAT_SPECIFIER
#define PRIDBID													ACE_UINT64_FORMAT_SPECIFIER
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////// socket typedefs ///////////////////////////////////////////////////////////
typedef ACE_SOCKET                                          KBE_SOCKET;
typedef ACE_SOCKET_LEN                                 KBE_SOCKET_LEN;
#ifndef IFF_UP
enum
{
	IFF_UP = 0x1,
	IFF_BROADCAST = 0x2,
	IFF_DEBUG = 0x4,
	IFF_LOOPBACK = 0x8,
	IFF_POINTOPOINT = 0x10,
	IFF_NOTRAILERS = 0x20,
	IFF_RUNNING = 0x40,
	IFF_NOARP = 0x80,
	IFF_PROMISC = 0x100,
	IFF_MULTICAST = 0x1000
};
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////// 跨平台宏 ///////////////////////////////////////////////////////////
#define ARRAYSZ(v)					(sizeof(v) / sizeof(v[0]))
#define ARRAY_SIZE(v)				(sizeof(v) / sizeof(v[0]))

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
#define KBE_MIN(a,b) min(a,b)
#define KBE_MAX(a,b) max(a,b)
#define NOMINMAX
#else
#define KBE_MIN(a,b) std::min(a,b)
#define KBE_MAX(a,b) std::max(a,b)
#endif

// 所有名称字符串的最大长度
#define MAX_NAME 256	
// ip字符串的最大长度
#define MAX_IP 50
// 常规的buf长度
#define MAX_BUF 256
#ifndef MAX_PATH
#define MAX_PATH 260
#endif

//获取用户UID
inline uid_t  kbe_get_uid()
{
	return ACE_OS::getuid();
}
//获取用户名 
inline const char * kbe_get_username()
{
#if KBE_PLATFORM == PLATFORM_WIN32
	DWORD dwSize = MAX_NAME;
	static char username[MAX_NAME];
	memset(username, 0, MAX_NAME);
	::GetUserNameA(username, &dwSize);
	return username;
#else
	char * pUsername = cuserid(NULL);
	return pUsername ? pUsername : "";
#endif
}
//获取进程ID
inline pid_t kbe_get_pid()
{
	return ACE_OS::getpid();
}
//获取系统时间 
inline ACE_UINT64 kbe_get_sys_time()
{
	ACE_UINT64 t;
	ACE_OS::gettimeofday().msec(t);
	return t;
}
//获取2个系统时间差 
inline ACE_UINT64 kbe_get_sys_time_diff(ACE_UINT64& oldTime, ACE_UINT64& newTime)
{
	// getSystemTime() have limited data range and this is case when it overflow in this tick
	if( oldTime > newTime )
		return ( 0xFFFFFFFF - oldTime ) + newTime;
	else
		return newTime - oldTime;
}
inline void kbe_sleep(ACE_UINT32 ms)
{
	ACE_OS::sleep(ACE_Time_Value(0, ms * 1000));
}

//错误号码以及描述
inline void kbe_lasterror(int ierrorno)
{
	ACE_OS::last_error(ierrorno);
}
inline int kbe_lasterror()
{
	return ACE_OS::last_error();
}
inline char* kbe_strerror(int ierrorno = 0)
{
	return ACE_OS::strerror(ierrorno);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////// UUID64 //////////////////////////////////////////////////////
inline ACE_Utils::UUID* kbe_gen_uuid64()
{
	ACE_Utils::UUID_GENERATOR::instance()->init();
	ACE_Utils::UUID* uuid = ACE_Utils::UUID_GENERATOR::instance()->generate_UUID();
	return uuid;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////  释放 ptr //////////////////////////////////////////////////////
/// 安全的释放一个指针内存 
#define SAFE_RELEASE(i) \
if (i)	{ delete i; i = NULL; }

/// 安全的释放一个指针数组内存 
#define SAFE_RELEASE_ARRAY(i) \
if (i)	{ delete[] i; i = NULL; }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////  typedef 组件类型 //////////////////////////////////////////////////////////////
typedef ACE_TCHAR                               TCHAR; //字符宽度
typedef ACE_UINT16								ENTITY_TYPE;// entity的类别类型定义支持0-65535个类别
typedef ACE_INT32								ENTITY_ID;// entityID的类型
typedef ACE_UINT32								SPACE_ID;//  一个space的id
typedef ACE_UINT32								CALLBACK_ID;//  一个callback由CallbackMgr分配的id
typedef ACE_UINT64								KBE_SRV_COMPONENT_ID;//  一个服务器组件的id
typedef ACE_INT32								KBE_SRV_COMPONENT_ORDER;// 一个组件的启动顺序
typedef	ACE_UINT64								TIMER_ID;// 一个timer的id类型
typedef ACE_UINT8								MAIL_TYPE;// mailbox 所投递的mail类别的类别
typedef ACE_UINT32								GAME_TIME;
typedef ACE_UINT32								GameTime;
typedef ACE_INT32								ScriptID;
typedef ACE_UINT32								ArraySize;// 任何数组的大小都用这个描述
typedef ACE_UINT64								DBID;//  一个在数据库中的索引用来当做某ID
typedef ACE_UINT32								CELL_ID;// CELL的id

#define ENTITY_ID_SIZE                        4
#define charptr									char*
#define const_charptr							const char*
#define PyObject_ptr							PyObject*
#define UnorderedMap                            std::tr1::unordered_map
#define Shared_ptr						    std::tr1::shared_ptr
#define ENTITY_TYPE_MAX                         ACE_UINT16_MAX
#define IFNAMSIZ                                16
#define CELL_DEF_MIN_AREA_SIZE		            500.0f /// 一个cell的默认的边界或者最小大小
#define SPACE_CHUNK_SIZE					    100    /// 一个空间的一个chunk大小
typedef UnorderedMap<std::string, std::string>  SPACE_DATA;// space中存储的数据

////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////// server typedefs /////////////////////////////////
/// 账号类别 
enum ACCOUNT_TYPE
{
	ACCOUNT_TYPE_NORMAL = 1,	/// 普通账号
	ACCOUNT_TYPE_MAIL = 2,		    /// email账号(需激活)
	ACCOUNT_TYPE_SMART = 3		    /// 智能识别
};
/// 账号状态
enum ACCOUNT_FLAGS
{
	ACCOUNT_FLAG_NORMAL = 0x00000000,
	ACCOUNT_FLAG_LOCK = 0x000000001,
	ACCOUNT_FLAG_NOT_ACTIVATED = 0x000000002
};

/** entity的mailbox类别 */
enum ENTITY_MAILBOX_TYPE
{
	MAILBOX_TYPE_CELL = 0,
	MAILBOX_TYPE_BASE = 1,
	MAILBOX_TYPE_CLIENT = 2,
	MAILBOX_TYPE_CELL_VIA_BASE = 3,
	MAILBOX_TYPE_BASE_VIA_CELL = 4,
	MAILBOX_TYPE_CLIENT_VIA_CELL = 5,
	MAILBOX_TYPE_CLIENT_VIA_BASE = 6
};

/// mailbox的类型 => 字符串，和 ENTITY_MAILBOX_TYPE 索引匹配
const char ENTITY_MAILBOX_TYPE_NAMES[ ][8] =
{
	"cell",
	"base",
	"client",
	"cell",
	"base",
	"client",
	"client",
};

/// 定义服务器组件类型
enum KBE_SRV_COMPONENT_TYPE
{
	UNKNOWN = 0,
	DBMGR = 1,
	LOGIN = 2,
	BASEMGR = 3,
	CELLMGR = 4,
	CELL = 5,
	BASE = 6,
	CLIENT = 7,
	MACHINE = 8,
	CONSOLE = 9,
	MSGLOG = 10,
	BOTS = 11,
	WATCHER = 12,
	BILLING = 13,
	End = 14,
};
/// 定义服务器各组件类型对应str名称 
const char KBE_SRV_COMPONENT_TYPE_NAMES[ ][255] =
{
	"unknown", //0
	"dbmgr",//1
	"login",//2
	"basemgr",//3
	"cellmgr",//4
	"cell",//5
	"base",//6
	"client",//7
	"machine",//8
	"console",//9
	"msglog",//10
	"bots",//11
	"watcher",//12
	"billing",//13
	"end"//14
};
/// 定义服务器各组件类型对应str名称 
const char KBE_SRV_COMPONENT_TYPE_NAMES1[ ][255] =
{
	"unknown", //0
	"dbmgr",//1
	"login",//2
	"basemgr",//3
	"cellmgr",//4
	"cell",//5
	"base",//6
	"client",//7
	"machine",//8
	"console",//9
	"msglog",//10
	"bots",//11
	"watcher",//12
	"billing",//13
	"end"//14
};

inline const char*  GET_KBE_SRV_COMPONENT_TYPE_NAME(KBE_SRV_COMPONENT_TYPE TYPE)
{
	if( TYPE < 0 || TYPE >= End )
	{
		return KBE_SRV_COMPONENT_TYPE_NAMES[UNKNOWN];
	}
	return KBE_SRV_COMPONENT_TYPE_NAMES[TYPE];
}
inline const char* GET_KBE_SRV_COMPONENT_TYPE_NAME_EX_1(KBE_SRV_COMPONENT_TYPE TYPE)
{
	if( TYPE < 0 || TYPE >= End )
	{
		return KBE_SRV_COMPONENT_TYPE_NAMES1[UNKNOWN];
	}

	return KBE_SRV_COMPONENT_TYPE_NAMES1[TYPE];
}
inline KBE_SRV_COMPONENT_TYPE KBE_SRV_COMPONENT_TYPE2NAME(const char* name)
{
	for( int i = 0; i < (int) End; i++ )
	{
		if( kbe_stricmp(KBE_SRV_COMPONENT_TYPE_NAMES[i], name) == 0 )
			return (KBE_SRV_COMPONENT_TYPE) i;
	}
	return UNKNOWN;
}

// 所有的组件列表
const KBE_SRV_COMPONENT_TYPE ALL_COMPONENT_TYPES[ ] =
{
	BASEMGR,
	CELLMGR,
	DBMGR,
	CELL,
	BASE,
	LOGIN,
	MACHINE,

	CONSOLE,
	MSGLOG,
	WATCHER,
	BILLING,
	BOTS,

	UNKNOWN
};
// 所有的后端组件列表
const KBE_SRV_COMPONENT_TYPE ALL_SERVER_COMPONENT_TYPES[ ] =
{
	BASEMGR,
	CELLMGR,
	DBMGR,
	CELL,
	BASE,
	LOGIN,
	MACHINE,

	MSGLOG,
	WATCHER,
	BILLING,
	BOTS,

	UNKNOWN
};
// 所有的后端组件列表
const KBE_SRV_COMPONENT_TYPE ALL_GAME_SERVER_COMPONENT_TYPES[ ] =
{
	BASEMGR,
	CELLMGR,
	DBMGR,
	CELL,
	BASE,
	LOGIN,
	BILLING,

	UNKNOWN
};
// 所有的辅助性组件
const KBE_SRV_COMPONENT_TYPE ALL_HELPER_COMPONENT_TYPE[ ] =
{
	MSGLOG,
	UNKNOWN
};

// 返回是否是一个有效的组件
#define ISVALIDCOMPONENT(C_TYPE) ((C_TYPE) > 0 && (C_TYPE) < End)

/// 检查是否为一个游戏服务端组件类别 
inline bool isGameServerComponentType(KBE_SRV_COMPONENT_TYPE componentType)
{
	int i = 0;
	while( true )
	{
		KBE_SRV_COMPONENT_TYPE t = ALL_GAME_SERVER_COMPONENT_TYPES[i++];
		if( t == UNKNOWN )
			break;

		if( t == componentType )
			return true;
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////// client type typedefs ///////////////////////////////////////////////
/// 前端应用的类别, All client type
enum KBE_CLIENT_TYPE
{
	KBE_UNKNOWN_CLIENT_COMPONENT_TYPE = 0,

	// 移动类，手机，平板电脑
	// Mobile, Phone, Pad(Allowing does not contain Python-scripts and entitydefs analysis, can be imported protocol from network)
	KBE_CLIENT_TYPE_MOBILE = 1,

	// 独立的Windows/Linux/Mac应用程序(包含python脚本，entitydefs解析与检查entitydefs的MD5，原生的)
	// Windows/Linux/Mac Application program (Contains the Python-scripts, entitydefs parsing and check entitydefs-MD5, Native)
	KBE_CLIENT_TYPE_PC = 2,

	// 不包含Python脚本，entitydefs协议可使用网络导入
	// Web, HTML5, Flash
	KBE_CLIENT_TYPE_BROWSER = 3,

	// 包含Python脚本，entitydefs解析与检查entitydefs的MD5，原生的
	// bots	 (Contains the Python-scripts, entitydefs parsing and check entitydefs-MD5, Native)
	KBE_CLIENT_TYPE_BOTS = 4,

	// 轻端类, 可不包含python脚本，entitydefs协议可使用网络导入
	// Mini-Client(Allowing does not contain Python-scripts and entitydefs analysis, can be imported protocol from network)
	KBE_CLIENT_TYPE_MINI = 5,

	// End
	KBE_CLIENT_TYPE_END = 6
};
/// 定义前端应用类别名称 
const char KBE_CLIENT_COMPONENT_TYPE_NAMES[ ][255] =
{
	"KBE_UNKNOWN_CLIENT_COMPONENT_TYPE",
	"KBE_CLIENT_TYPE_MOBILE",
	"KBE_CLIENT_TYPE_PC",
	"KBE_CLIENT_TYPE_BROWSER",
	"KBE_CLIENT_TYPE_BOTS",
	"KBE_CLIENT_TYPE_MINI",
};
// 所有前端应用的类别
const KBE_CLIENT_TYPE ALL_KBE_CLIENT_TYPES[ ] =
{
	KBE_CLIENT_TYPE_MOBILE,
	KBE_CLIENT_TYPE_PC,
	KBE_CLIENT_TYPE_BROWSER,
	KBE_CLIENT_TYPE_BOTS,
	KBE_CLIENT_TYPE_MINI,
	KBE_UNKNOWN_CLIENT_COMPONENT_TYPE
};
typedef ACE_INT8 KBE_CLIENT_CTYPE;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////// validations ///////////////////////////////////////////////////////////
/// 检查用户名合法性 
inline bool validName(const char* name, int size)
{
	if( size >= 256 )
		return false;

	for( int i = 0; i < size; i++ )
	{
		char ch = name[i];
		if( ( ch >= 'a' && ch <= 'z' ) || ( ch >= 'A' && ch <= 'Z' ) || ( ch >= '0' && ch <= '9' ) || ( ch == '_' ) )
			continue;

		return false;
	}

	return true;
}
inline bool validName(const std::string& name)
{
	return validName(name.c_str(), name.size());
}
/** 检查email地址合法性
严格匹配请用如下表达式
[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?
*/
inline bool email_isvalid(const char *address)
{
#if defined (ACE_HAS_REGEX)
	std::tr1::regex _mail_pattern("([a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?)");
	return std::tr1::regex_match(accountName, _mail_pattern);
#else
	int len = strlen(address);
	if( len <= 3 )
		return false;

	char ch = address[len - 1];
	if( !( ( ch >= 'a' && ch <= 'z' ) || ( ch >= 'A' && ch <= 'Z' ) || ( ch >= '0' && ch <= '9' ) ) )
		return false;

	int        count = 0;
	const char *c, *domain;
	static const char *rfc822_specials = "()<>@,;:\\\"[]";

	/* first we validate the name portion (name@domain) */
	for( c = address; *c; c++ )
	{
		if( *c == '\"' && ( c == address || *( c - 1 ) == '.' || *( c - 1 ) ==
			'\"' ) )
		{
			while( *++c )
			{
				if( *c == '\"' ) break;
				if( *c == '\\' && ( *++c == ' ' ) ) continue;
				if( *c <= ' ' || *c >= 127 ) return false;
			}
			if( !*c++ ) return false;
			if( *c == '@' ) break;
			if( *c != '.' ) return false;
			continue;
		}
		if( *c == '@' ) break;
		if( *c <= ' ' || *c >= 127 ) return false;
		if( strchr(rfc822_specials, *c) ) return false;
	}
	if( c == address || *( c - 1 ) == '.' ) return false;

	/* next we validate the domain portion (name@domain) */
	if( !*( domain = ++c ) ) return false;
	do
	{
		if( *c == '.' )
		{
			if( c == domain || *( c - 1 ) == '.' ) return false;
			count++;
		}
		if( *c <= ' ' || *c >= 127 ) return false;
		if( strchr(rfc822_specials, *c) ) return false;
	} while( *++c );

	return ( count >= 1 );
#endif
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// GLOBALS 当前服务器组件类别和ID 
extern KBE_SRV_COMPONENT_TYPE                     g_componentType;
extern KBE_SRV_COMPONENT_ID                         g_componentID;
extern KBE_SRV_COMPONENT_ORDER                  g_componentGlobalOrder;
extern KBE_SRV_COMPONENT_ORDER                  g_componentGroupOrder;
extern GAME_TIME                                               g_kbetime;  /// kbe时间 

struct Intrusive_Auto_Ptr
{
	inline void incRef(void) const
	{
		++refCount_;
	}

	inline void decRef(void) const
	{

		int currRef = --refCount_;
		ACE_ASSERT(currRef >= 0 && "RefCountable:currRef maybe a error!");
		if( 0 >= currRef )
			onRefOver();											// 引用结束了
	}

	virtual void onRefOver(void) const
	{
		delete const_cast<Intrusive_Auto_Ptr*>( this );
	}

	void setRefCount(int n)
	{
		refCount_ = n;
	}

	int getRefCount(void) const
	{
		return refCount_;
	}

	Intrusive_Auto_Ptr(void) : refCount_(0)
	{
	}

	virtual ~Intrusive_Auto_Ptr(void)
	{
		ACE_ASSERT(0 == refCount_ && "RefCountable:currRef maybe a error!");
	}

	volatile mutable long refCount_;
};
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace/post.h"
#endif
//