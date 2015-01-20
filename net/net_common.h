﻿#ifndef NET_COMMON_H_
#define NET_COMMON_H_

#include "ace\pre.h"
#include "common\common.h"
#include "ace/SOCK_Stream.h"
#include "ace\SOCK_Dgram.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

template <typename SOCK_TYPE>
struct SOCK_TYPE_TRAITS
{
	typedef typename SOCK_TYPE::sock_type  sock_type;//定义一个traits实现  
	typedef typename SOCK_TYPE::tcp_upd_type  tcp_upd_type;//定义一个traits实现
};
struct TCP_TYPE
{
	typedef  ACE_SOCK_Stream sock_type;
	typedef  TCP_TYPE  tcp_upd_type;
};
struct UDP_TYPE
{
	typedef ACE_SOCK_Dgram sock_type;
	typedef  UDP_TYPE  tcp_upd_type;
};

namespace UDP /*以后扩展用*/
{

#define PACKET_MAX_SIZE_UDP					1472 
}

namespace TCP
{

}
//========================================================
/*这个开关设置数据包是否总是携带长度信息， 这样在某些前端进行耦合时提供一些便利
如果为false则一些固定长度的数据包不携带长度信息， 由对端自行解析*/
extern bool g_packetAlwaysContainLength;

/**
是否需要将任何接收和发送的包以文本输出到log中提供调试
g_trace_packet:
0: 不输出
1: 16进制输出
2: 字符流输出
3: 10进制输出
use_logfile:
是否独立一个log文件来记录包内容，文件名通常为
appname_packetlogs.log
g_trace_packet_disables:
关闭某些包的输出
*/
extern int g_trace_packet;
extern std::vector<std::string> g_trace_packet_disables;
extern bool g_trace_packet_use_logfile;

/*是否输出entity的创建， 脚本获取属性， 初始化属性等调试信息*/
extern bool g_debugEntity;

/**
apps发布状态, 可在脚本中获取该值
0 : debug
1 : release
*/
extern bool g_appPublish;
//========================================================

// listen监听队列最大值
extern ACE_UINT32 g_SOMAXCONN;


//========================================================
const ACE_UINT32 BROADCAST = 0xFFFFFFFF; //255.255.255.255
const ACE_UINT32 LOCALHOST = 0x0100007F; //127.0.0.1
//========================================================


//========================================== kbe machine端口
#define KBE_PORT_START						20000
#define KBE_MACHINE_BRAODCAST_PORT		    KBE_PORT_START + 86 ///machine接收广播的端口
#define KBE_PORT_BROADCAST_DISCOVERY		KBE_PORT_START + 87
#define KBE_MACHINE_TCP_PORT				KBE_PORT_START + 88
#define KBE_BILLING_TCP_PORT				30099
// kbe machine端口 =========================================


//========================================== network stats
extern ACE_UINT64						g_numPacketsSent;
extern ACE_UINT64						g_numPacketsReceived;
extern ACE_UINT64						g_numBytesSent;
extern ACE_UINT64						g_numBytesReceived;
// network stats =========================================


//==================================================== msg
/*typedefs*/
typedef ACE_UINT16	MessageID;	/// message id; 消息的ID
/// 消息长度，目前长度有2种，默认消息长度最大MessageLength
/// 当超过这个数时需要扩展长度，底层使用MessageLength1
typedef ACE_UINT16	MessageLength;// 最大65535
typedef ACE_UINT32	MessageLength1;// 最大4294967295
typedef ACE_UINT16	PacketLength;//最大65535

/*msg sizes*/
#define ENCRYPTTION_WASTAGE_SIZE			(1 + 7) /// 加密额外存储的信息占用字节(长度+填充)

#define PACKET_MAX_SIZE						1500
#define PACKET_MAX_SIZE_TCP					1460 //因为使用ace_output(4) 时， 会自动加上8个字节对齐 因此我们在这里减去他
#define PACKET_MAX_SIZE_UDP					1472
#define PACKET_LENGTH_BYTE_SIZE				sizeof(PacketLength)

#define NETWORK_MESSAGE_ID_SIZE				sizeof(MessageID)
#define NETWORK_MESSAGE_LENGTH_SIZE			sizeof(MessageLength)
#define NETWORK_MESSAGE_LENGTH1_SIZE		sizeof(MessageLength1)
#define NETWORK_MESSAGE_MAX_SIZE			65535
#define NETWORK_MESSAGE_MAX_SIZE1			4294967295

#define GAME_PACKET_MAX_SIZE_TCP PACKET_MAX_SIZE_TCP - NETWORK_MESSAGE_ID_SIZE - \
		NETWORK_MESSAGE_LENGTH_SIZE - ENCRYPTTION_WASTAGE_SIZE /// 游戏内容可用包大小

/* 网络消息类型
如果需要自定义长度则在
NETWORK_INTERFACE_DECLARE_BEGIN中声明时填入长度即可 */
#ifndef NETWORK_FIXED_MESSAGE
#define NETWORK_FIXED_MESSAGE 0 ///定长消息
#endif
#ifndef NETWORK_VARIABLE_MESSAGE
#define NETWORK_VARIABLE_MESSAGE -1 ///变长消息
#endif

enum MESSAGE_CATEGORY
{
	MESSAGE_CATEGORY_COMPONENT = 0,	// 组件消息
	MESSAGE_CATEGORY_ENTITY = 1	// entity消息
};

enum ProtocolType
{
	PROTOCOL_TCP,
	PROTOCOL_UDP
};

enum Reason
{
	REASON_SUCCESS = 0,				 ///< No reason.
	REASON_TIMER_EXPIRED = -1,		 ///< Timer expired.
	REASON_NO_SUCH_PORT = -2,		 ///< Destination port is not open.
	REASON_GENERAL_NETWORK = -3,	 ///< The network is stuffed.
	REASON_CORRUPTED_PACKET = -4,	 ///< Got a bad packet.
	REASON_NONEXISTENT_ENTRY = -5,	 ///< Wanted to call a null function.
	REASON_CHANNEL_WINDOW_OVERFLOW = -6,///< Channel send window overflowed.
	REASON_CHANNEL_INACTIVITY = -7,			 ///< Channel inactivity timeout.
	REASON_RESOURCE_UNAVAILABLE = -8,///< Corresponds to EAGAIN
	REASON_CLIENT_DISCONNECTED = -9, ///< Client disconnected voluntarily.
	REASON_TRANSMIT_QUEUE_FULL = -10,///< Corresponds to ENOBUFS
	REASON_CHANNEL_LOST = -11,		 ///< Corresponds to channel lost
	REASON_SHUTTING_DOWN = -12,		 ///< Corresponds to shutting down app.
	REASON_HTML5_ERROR = -13,		 ///< html5 error.
	REASON_CHANNEL_CONDEMN = -14	 ///< condemn error.
};

inline const char* reasonToString(Reason reason)
{
	const char* reasons[ ] =
	{
		"REASON_SUCCESS: No reason",
		"REASON_TIMER_EXPIRED: Timer expired",
		"REASON_NO_SUCH_PORT: Destination port is not open",
		"REASON_GENERAL_NETWORK: The network is stuffed",
		"REASON_CORRUPTED_PACKET: Got a bad packet",
		"REASON_NONEXISTENT_ENTRY: Wanted to call a null function",
		"REASON_CHANNEL_WINDOW_OVERFLOW: Channel send window overflowed",
		"REASON_CHANNEL_INACTIVITY_TIMEOUT: Channel inactivity timeout",
		"REASON_RESOURCE_UNAVAILABLE: Corresponds to EAGAIN",
		"REASON_CLIENT_DISCONNECTED: Client disconnected voluntarily",
		"REASON_TRANSMIT_QUEUE_FULL: Corresponds to ENOBUFS",
		"REASON_CHANNEL_LOST: Corresponds to channel lost",
		"REASON_SHUTTING_DOWN: Corresponds to shutting down app",
		"REASON_HTML5_ERROR: html5 error",
		"REASON_CHANNEL_CONDEMN: condemn error"
	};

	unsigned int index = -reason;

	if( index < sizeof(reasons) / sizeof(reasons[0]) )
	{
		return reasons[index];
	}

	return "REASON_UNKNOWN";
}

//msg ====================================================


//================================================ channel
typedef ACE_UINT32	ChannelID;
const ChannelID CHANNEL_ID_NULL = 0;

/*通道超时时间*/
extern float g_channelInternalTimeout;
extern float g_channelExternalTimeout;

/*通道发送超时重试*/
extern ACE_UINT32 g_intReSendInterval;
extern ACE_UINT32 g_intReSendRetries;
extern ACE_UINT32 g_extReSendInterval;
extern ACE_UINT32 g_extReSendRetries;

///外部通道加密类别
extern ACE_INT8 g_channelExternalEncryptType;

///不做通道超时检查
#define CLOSE_CHANNEL_INACTIVITIY_DETECTION() {Network::g_channelExternalTimeout = \
		Network::g_channelInternalTimeout = -1.0f;}	

/*包接收窗口溢出*/
extern ACE_UINT32 g_receiveWindowMessagesOverflowCritical;
extern ACE_UINT32 g_intReceiveWindowMessagesOverflow;
extern ACE_UINT32 g_extReceiveWindowMessagesOverflow;
extern ACE_UINT32 g_intReceiveWindowBytesOverflow;
extern ACE_UINT32 g_extReceiveWindowBytesOverflow;
//channel =============================================== 


inline bool initializeWatcher()
{
	//WATCH_OBJECT("network/numPacketsSent", g_numPacketsSent);
	//WATCH_OBJECT("network/numPacketsReceived", g_numPacketsReceived);
	//WATCH_OBJECT("network/numBytesSent", g_numBytesSent);
	//WATCH_OBJECT("network/numBytesReceived", g_numBytesReceived);

	//std::vector<MessageHandlers*>::iterator iter = MessageHandlers::messageHandlers().begin();
	//for( ; iter != MessageHandlers::messageHandlers().end(); iter++ )
	//{
	//	if( !( *iter )->initializeWatcher() )
	//		return false;
	//}

	return true;
}
inline void finalise(void)
{
	//#ifdef ENABLE_WATCHERS
	//			WatcherPaths::finalise();
	//#endif
	//
	//			MessageHandlers::finalise();
	//
	//			Network::destroyObjPool();
}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#endif