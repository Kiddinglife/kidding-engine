#ifndef ENTITY_DEF_H_
#define ENTITY_DEF_H_

#include "ace\pre.h"
#include <common\common.h>
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

#if KBE_PLATFORM == PLATFORM_WIN32
#pragma warning (disable : 4910)
#pragma warning (disable : 4251)
#else
//UNIX
#endif
enum PropagateFlag;
typedef std::map<std::string, PropagateFlag> PropagateFlagMap;
extern PropagateFlagMap gPropagateFlags;	// entity 的flag字符串映射表

////////////////////////////////// entity的数据传输特性标记 //////////////////////////////////////////////
enum PropagateFlag
{
	CELL_PUBLIC = 0x00000001, // 相关所有cell广播
	CELL_PRIVATE = 0x00000002, // 当前cell
	OWN_CLIENT = 0x00000010, // 当前cell和客户端
	OTHER_CLIENTS = 0x00000080, // cell广播和其他客户端
	ALL_CLIENTS = 0x00000004, // cell广播与所有客户端
	CELL_PUBLIC_AND_OWN = 0x00000008, // cell广播与自己的客户端

	BASE = 0x00000040, // 当前base
	BASE_AND_CLIENT = 0x00000020, // base和客户端

	UNKOWN = 0x00000000, // 未定义
};
#define ALLPropagateFlags  CELL_PUBLIC | CELL_PRIVATE | ALL_CLIENTS \
CELL_PUBLIC_AND_OWN | OWN_CLIENT | BASE_AND_CLIENT | BASE | OTHER_CLIENTS
inline std::string PropagateFlagToString(PropagateFlag flag)
{
	switch( flag )
	{
		case CELL_PUBLIC:
			return "CELL_PUBLIC";
			break;
		case CELL_PRIVATE:
			return "CELL_PRIVATE";
			break;
		case ALL_CLIENTS:
			return "ALL_CLIENTS";
			break;
		case CELL_PUBLIC_AND_OWN:
			return "CELL_PUBLIC_AND_OWN";
			break;
		case OWN_CLIENT:
			return "OWN_CLIENT";
			break;
		case BASE_AND_CLIENT:
			return "BASE_AND_CLIENT";
			break;
		case BASE:
			return "BASE";
			break;
		case OTHER_CLIENTS:
			return "OTHER_CLIENTS";
			break;
		default:
			return "UNKOWN";
			break;
	}
}

////////////////////////////// 相当于对entity数据传输类别的一个总体的定义 /////////////////////////////
enum EntityDataFlag
{
	// 所有与baseapp有关系的标志
	ENTITY_BASE_DATA_FLAGS = BASE | BASE_AND_CLIENT,

	// 所有与cellapp有关系的标志
	ENTITY_CELL_DATA_FLAGS = CELL_PUBLIC | CELL_PRIVATE | ALL_CLIENTS | CELL_PUBLIC_AND_OWN | OTHER_CLIENTS | OWN_CLIENT,

	// 所有与client有关系的标志
	ENTITY_CLIENT_DATA_FLAGS = BASE_AND_CLIENT | ALL_CLIENTS |
	CELL_PUBLIC_AND_OWN | OTHER_CLIENTS | OWN_CLIENT,

	// 所有需要广播给其他cellapp的标志
	ENTITY_BROADCAST_CELL_FLAGS = CELL_PUBLIC | ALL_CLIENTS |
	CELL_PUBLIC_AND_OWN | OTHER_CLIENTS,

	// 所有需要广播给其他客户端(不包括自己的)的标志
	ENTITY_BROADCAST_OTHER_CLIENT_FLAGS = OTHER_CLIENTS | ALL_CLIENTS,

	// 所有需要广播给自己的客户端的标志
	ENTITY_BROADCAST_OWN_CLIENT_FLAGS = ALL_CLIENTS |
	CELL_PUBLIC_AND_OWN | OWN_CLIENT | BASE_AND_CLIENT,
};


///// mailbox类别所对应的组件类别映射， 这个表的索引个严格匹配ENTITY_MAILBOX_TYPE的值  //////
const KBE_SRV_COMPONENT_TYPE ENTITY_MAILBOX_COMPONENT_TYPE_MAPPING[ ] =
{
	CELLAPP_TYPE,
	BASEAPP_TYPE,
	CLIENT_TYPE,
	BASEAPP_TYPE,
	CELLAPP_TYPE,
	CELLAPP_TYPE,
	BASEAPP_TYPE,
};


////////////////////////////////////// 实体属性的lod广播级别范围的定义 /////////////////////////////////
typedef ACE_UINT8 DETAIL_TYPE;
#define ENTITY_PROPERTY_LOD_LEVEL_NEAR	0	// lod级别：近						
#define ENTITY_PROPERTY_LOD_LEVEL_MEDIUM	1	// lod级别：中
#define ENTITY_PROPERTY_LOD_LEVEL_FAR		2	// lod级别：远	

//////////////////////////////////////// 属性和方法的UID类别 ////////////////////////////////////////////
typedef ACE_UINT16 DATATYPE_UID;
typedef ACE_UINT16 ENTITY_SCRIPT_UID;
typedef ACE_UINT8   ENTITY_DEF_ALIASID;
typedef ACE_UINT16 ENTITY_METHOD_UID;
typedef ACE_UINT16 ENTITY_PROPERTY_UID;
enum DATA_TYPE
{
	DATA_TYPE_UNKONWN,

	DATA_TYPE_FIXEDARRAY,
	DATA_TYPE_FIXEDDICT,

	DATA_TYPE_STRING,
	DATA_TYPE_DIGIT,
	DATA_TYPE_BLOB,
	DATA_TYPE_VECTOR,
	DATA_TYPE_UNICODE,
	DATA_TYPE_MAILBOX,
	DATA_TYPE_DICT,

	DATA_TYPE_PY_DICT,
	DATA_TYPE_PY_TUPLE,
	DATA_TYPE_PY_LIST,

	DATA_TYPE_CSHARP_DICT,
	DATA_TYPE_CSHARP_TUPLE,
	DATA_TYPE_CSHARP_LIST,

	DATA_TYPE_JAVA_DICT,
	DATA_TYPE_JAVA_TUPLE,
	DATA_TYPE_JAVA_LIST,

	DATA_TYPE_LUA_DICT,
	DATA_TYPE_LUA_TUPLE,
	DATA_TYPE_LUA_LIST,

	DATA_TYPE_CSHARP,
	DATA_TYPE_JAVA,
	DATA_TYPE_PY,
	DATA_TYPE_LUA
};


///////////////////// 对entity的一些系统级别的可变属性进行编号以便网络传输时进行辨别 ///////////////
enum ENTITY_BASE_PROPERTY_UTYPE
{
	ENTITY_BASE_PROPERTY_UTYPE_POSITION_XYZ = 1,
	ENTITY_BASE_PROPERTY_UTYPE_DIRECTION_ROLL_PITCH_YAW = 2,
	ENTITY_BASE_PROPERTY_UTYPE_SPACEID = 3,
};


///////////////////// 对entity的一些系统级别的可变属性进行编号以便网络传输时进行辨别 ///////////////
enum ENTITY_BASE_PROPERTY_ALIASID
{
	ENTITY_BASE_PROPERTY_ALIASID_POSITION_XYZ = 0,
	ENTITY_BASE_PROPERTY_ALIASID_DIRECTION_ROLL_PITCH_YAW = 1,
	ENTITY_BASE_PROPERTY_ALIASID_SPACEID = 2,
	ENTITY_BASE_PROPERTY_ALIASID_MAX = 3,
};


////////////////////////////////// 被限制的系统属性，def中不允许定义 //////////////////////////////////
const char ENTITY_LIMITED_PROPERTYS[ ][32] =
{
	"id",
	"position",
	"direction",
	"spaceID",
	"autoLoad",
	"cell",
	"base",
	"client",
	"cell",
	"className",
	"databaseID",
	"isDestroyed",
	"shouldAutoArchive",
	"shouldAutoBackup",
	"__ACCOUNT_NAME__",
	"__ACCOUNT_PASSWORD__",
	"clientAddr",
	"entitiesEnabled",
	"hasClient",
	"roundTripTime",
	"timeSinceHeardFromClient",
	"allClients",
	"hasWitness",
	"isWitnessed",
	"otherClients",
	"topSpeed",
	"topSpeedY",
	"",
};

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif