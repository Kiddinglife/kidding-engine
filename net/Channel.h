#ifndef Channel_H_
#define Channel_H_

#include "ace\pre.h"
#include "ace\Refcounted_Auto_Ptr.h"
#include "common\ace_object_pool.h"
#include "Bundle.h"
#include "ace\Event_Handler.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

struct PacketReader;
struct PacketReceiver;
struct PacketSender;
struct PacketFilter;

struct Channel
{
	/// INTERNAL describes the properties of channel from server to server.
	/// EXTERNAL describes the properties of a channel from client to server.
	enum ChannelScope { INTERNAL, EXTERNAL };

	/// CHANNEL_NORMAL = 普通通道
	// CHANNEL_WEB = 浏览器web通道
	enum ChannelType { CHANNEL_NORMAL, CHANNEL_WEB };

	enum ChannelRecvWinStatus
	{
		PROCESS,
		NOT_PROCESS,
		PACKET_IS_CORRUPT
	};

	enum TimeOutType
	{
		TIMEOUT_INACTIVITY_CHECK
	};

	/// 该通道所需的网络接口
	NetworkInterface*                 pNetworkInterface_;

	/// 该通道需要使用bundle来缓存接收和发送的消息
	Bundle						             bundle_;

	//@TO-DO need create struct PacketReader
	PacketReader*				         pPacketReader_; //bufferedReceives_ 

	//@TO-DO maybe can use ace_handle 
	//ACE_SOCK*					          pEndPoint_;
	ACE_Event_Handler*				 pEndPoint_;

	//@TO-DO need create struct PacketReceiver
	//PacketReceiver*				      pPacketReceiver_;
	//PacketSender*				          pPacketSender_;

	//@TO-DO need create struct PacketFilter
	bool				                          canFilterPacket_;

	/// 可以指定通道使用某些特定的消息
	/// can designate the channel to use some specific msgs
	Messages*                              pMsgs_;

	/// bundles in this channel
	typedef std::vector<Bundle*> Bundles;
	Bundles                                  bundles_;

	/// 接收到的所有包的集合：the container for the received packets
	typedef std::vector<Packet*> RecvPackets;
	RecvPackets                           recvPackets_[2];

	ChannelScope                        channelScope_;
	ChannelType				              channelType_;
	ProtocolType				          protocolType_;
	ChannelID					          channelId_;
	bool						                  isDestroyed_;
	bool						                  is_notified_send_;

	/// 如果为true，则该频道已经变得不合法
	/// if true, this channel has become unusable
	bool						                  isCondemn_;

	/// 如果是外部通道且代理了一个前端则会绑定前端代理ID
	/// if this channel is external and proxy the client, it will  binds the client id
	ENTITY_ID					              proxyID_;

	/// 该channel所在的服务器组件的id
	KBE_SRV_COMPONENT_ID	  componentID_;

	/// 扩展用, for extension
	std::string					              strextra_;

	/// tinmer id
	long                                       timerID_;

	ACE_UINT64						      inactivityExceptionPeriod_;
	ACE_UINT64                           lastRecvTime_;
	ACE_UINT32                           winSize_;
	ACE_UINT8                             recvPacketIndex_; //bufferedReceivesIdx_

	// Statistics
	ACE_UINT32						      numPacketsSent_;
	ACE_UINT32						      numPacketsReceived_;
	ACE_UINT32						      numBytesSent_;
	ACE_UINT32						      numBytesReceived_;
	ACE_UINT32						      lastTickBytesReceived_;
	ACE_UINT32                           lastTickBytesSent_;

	/// Reference count.
	//int ref_count_;

	//static void intrusive_add_ref(Channel* channel)
	//{
	//	++channel->ref_count_;
	//}

	//static void intrusive_remove_ref(Channel* channel)
	//{
	//	--channel->ref_count_;
	//	ACE_ASSERT(channel->ref_count_ >= 0 && "RefCountable:ref_count_ maybe a error!");
	//	if( !channel->ref_count_ ) delete channel;
	//}


	Channel(NetworkInterface* networkInterface = NULL,
		ACE_Event_Handler* endpoint = NULL,
		ChannelScope traits = EXTERNAL,
		ProtocolType pt = PROTOCOL_TCP,
		bool canFilterPacket = false,
		ChannelID id = CHANNEL_ID_NULL);

	virtual ~Channel(void);

	const char*  c_str(void) const;
	void startInactivityDetection(float period, float checkPeriod = 1.0f);

	int get_bundles_length(void);
	void clearBundles(void);
	void clear_channel(bool warnOnDiscard = false);

	bool initialize(ACE_INET_Addr* addr = NULL);
	bool finalise(void);
	void destroy(void);
	void reset(ACE_Event_Handler* pEndPoint, bool warnOnDiscard);

	inline void add_delayed_channel(void);
	void hand_shake(void);
	//bool process_recv(bool expectingPacket);
	void process_packets(Messages* pMsgHandlers);
	void on_packet_sent(int bytes_cnt, bool is_sent_completely);
	void Channel::update_recv_window(Packet* pPacket);
	void Channel::on_packet_received(int bytes);
	void send(Bundle * pBundle = NULL);
	void tcp_send_single_bundle(TCP_SOCK_Handler* pEndpoint, Bundle* pBundle);
	void Channel::udp_send_single_bundle(UDP_SOCK_Handler* pEndpoint,
		Bundle* pBundle, ACE_INET_Addr& addr);
	bool process_send(void);
	inline void on_error(void);
	inline void on_bundles_sent_completely(void);
	inline void Channel::set_channel_condem();
};
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif
