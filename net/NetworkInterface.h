#ifndef NetworkInterface_H_
#define NetworkInterface_H_

#include "ace\pre.h"
#include "ace/SOCK_Acceptor.h"
#include "net_common.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

const int RECV_BUFFER_SIZE = 16 * 1024 * 1024; // 16MB
const char * USE_KBEMACHINED = "kbemachine";;

struct Bundle;
struct Channel;
struct DelayedChannels;
struct ChannelTimeOutHandler;
struct ChannelDeregisterHandler;
struct Packet;
struct Nub;
struct Messages;
struct TCP_Acceptor_Handler;

struct NetworkInterface
{
	typedef std::map<ACE_INET_Addr, Channel*>	ChannelMap;
	ChannelMap                                                    channelMap_;

	/// Acceptor TCP SOCK
	ACE_SOCK_Acceptor                                         extEndpoint_;

	/// UDP SOCK
	ACE_SOCK_DGRAM                                          intEndpoint_;

	///事件分发
	Nub*                                                                nub_;

	/// 数据指针
	void*                                                                pExtensionData_;

	/// 
	TCP_Acceptor_Handler*                                    pExtListenerReceiver_;
	TCP_Acceptor_Handler*                                    pIntListenerReceiver_;

	DelayedChannels * 						                    pDelayedChannels_;
	/// 超时的通道可被这个句柄捕捉， 例如告知上层client断开
	ChannelTimeOutHandler*					                pChannelTimeOutHandler_;
	ChannelDeregisterHandler*				                pChannelDeregisterHandler_;

	const bool								                        isExternal_;
	ACE_INT32									                    numExtChannels_;

	NetworkInterface(Nub* pDispatcher,
		ACE_INT32 extlisteningPort_min = -1,
		ACE_INT32 extlisteningPort_max = -1,
		const char * extlisteningInterface = "NONE",
		ACE_UINT32 extrbuffer = 0,
		ACE_UINT32 extwbuffer = 0,
		ACE_INT32 intlisteningPort = 0,
		const char * intlisteningInterface = "NONE",
		ACE_UINT32 intrbuffer = 0,
		ACE_UINT32 intwbuffer = 0);

	~NetworkInterface();

};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif