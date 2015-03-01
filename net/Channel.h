/**
 * @Date writing: 10:22 AM, 01/03/2015
 * @TO-DO initlize()
 * should let the networrkinterface class handle the udp sock handler's creation
 */
#ifndef Channel_H_
#define Channel_H_

#include "ace\pre.h"
#include "ace\Event_Handler.h"
#include "Bundle.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

struct Channel
{
	/// 超时检查的目的标志，例如这是一个非活动通道的检查
	/// This is the waht to be checked when timeout
	enum TimeOutType
	{
		TIMEOUT_INACTIVITY_CHECK
	};

	/**
	 * 我超时或者废弃时，回调网络接口的相关函数
	 * @param[pNetworkInterface_] have the call baks
	 * to handle the timout or condem of this channel
	 */
	NetworkInterface*                 pNetworkInterface_;

	/**
	 * 解包逻辑类，获取具体的消息
	 * this channel : pPacketReader_ = 1 : 1 & pool obj
	 * Decode the messages grom the received packets
	 * Initialized in void Channel::hand_shake(void) line 52 in cpp
	 * Used in void Channel::process_packets(Messages* pMsgHandlers) line 344 in cpp
	 * 	pPacketReader_->processMessages(pMsgHandlers, recvPackets_[idx]);
	 */
	PacketReader*				         pPacketReader_;

	/**
	 * 发送可以有我们主动发送，而接受只能依赖reactor事件机制，因此
	 * void Channel::send(Bundle * pBundle)用于主动发送bundles里的包，
	 * 只能在pEndPoint_.handle_input()回调函数中收包.
	 *
	 * We can actively and manually send packets in all bundles by calling
	 * void Channel::send(Bundle * pBundle). but we can only receive the packets
	 * from the call back of ACE_Event_Handler::handle_input()
	 *
	 * Inilised in int TCP_Acceptor_Handler::handle_input(ACE_HANDLE fd)
	 * from NetworkHnadler.cpp. Mainly used in bool Channel::process_send()
	 * to send all packets in bundles
	 *
	 * this channel : pEndPoint_ = 1 : 1  & pool obj
	 * @replace in KBE
	 * PacketReceiver*				      pPacketReceiver_;
	 * PacketSender*				          pPacketSender_;
	 */
	ACE_Event_Handler*				 pEndPoint_;

	//@TO-DO :: when finishing encrypt_filter.h
	bool				                          canFilterPacket_;

	/**
	 * 可以指定该通道使用某些特定的消息
	 * we can designate some specific msgs to this channel
	 *
	 * @Inilized by the caller
	 * @Used
	 * void Channel::process_packets(Messages* pMsgHandlers)
	 * if( this->pMsgs_ != NULL )
	 *	{
	 *	pMsgHandlers = this->pMsgs_;
	 *	}
	 */
	Messages*                              pMsgs_;

	/**
	 * 该通道会缓存一定量的发送包，这是send-batch的优化
	 * 每个bundle包含了许多满载包
	 *
	 */
	typedef std::vector<Bundle*> Bundles;
	Bundles                                  bundles_;
	Bundle                                    buffered_sending_bundle_;
	/**
	 * A channel will cache the received packets.
	 * 通道会缓存一定量的接受包，这是recv-batch优化
	 * int TCP_SOCK_Handler::handle_input(ACE_HANDLE fd)检测到有数据
	 * 可接受后，会调用bool TCP_SOCK_Handler::process_recv(bool expectingPacket)函数
	 * 来接受数据，该函数会从pool里取出来一个新的packet来装载数据，之后该函数会调用
	 * Channel::update_recv_window()从而将该包insert到recvPackets_中去
	 */
	typedef std::vector<Packet*> RecvPackets;
	RecvPackets                           recvPackets_;


	/// INTERNAL describes the properties of channel from server to server.
	/// EXTERNAL describes the properties of a channel from client to server.
	enum ChannelScope { INTERNAL, EXTERNAL };
	ChannelScope                        channelScope_;


	/// CHANNEL_NORMAL 普通通道
	/// CHANNEL_WEB 浏览器web通道
	enum ChannelType { CHANNEL_NORMAL, CHANNEL_WEB };
	ChannelType				              channelType_;

	/**
	 * Mark used to tell this is tcp channel or udp channel
	 * If protocolType_ == Protocol_TCP, pEndpoint_ type will be ACE_SOCK_STREAM
	 * If protocolType_ == Protocol_UDP, pEndpoint_ type will be ACE_SOCK_SGRAM
	 */
	ProtocolType				          protocolType_;

	/// Mark used to tell this channel
	ChannelID					          channelId_;

	/**
	 * It is quite often that the bundles we sent by calling Channel::send(Bundle* bundle)
	 * are not sent completely in one time in which case we can let the reactor event
	 * framework help to sent the rest of bundles or packets. If it is true, is_notified_send_
	 * will be setup as true to hightlight it.
	 */
	bool						                  is_notified_send_;

	/**
	 * isCondemn_和isDestroyed_的不同在于，一个通道是isCondemn_但不一定是isDestroyed_
	 * 通道为isDestroyed_一定也是isCondemn_
	 */
	/// 如果为true，则该频道已经在内存中被销毁或者回收
	/// if true, this channel has become destoryed by recycling it baxk to the pool
	/// or deleted in memeory
	bool						                  isDestroyed_;
	/// 如果为true，则该频道已经变得不合法
	/// if true, this channel has become unusable
	bool						                  isCondemn_;

	/// 如果是外部通道且代理了一个前端则会绑定前端代理ID
	/// if this channel is external and proxy the client, it will  binds the client id
	ENTITY_ID					              proxyID_;

	/// 该channel所在的服务器组件的id
	/// the server component where this channel resides
	KBE_SRV_COMPONENT_ID	  componentID_;

	/// 扩展用, for extension
	std::string					              strextra_;

	/// tinmer id used to canncel the timmer
	long                                       timerID_;

	// Statistics
	ACE_UINT32						      numPacketsSent_;
	ACE_UINT32						      numPacketsReceived_;
	ACE_UINT32						      numBytesSent_;
	ACE_UINT32						      numBytesReceived_;
	ACE_UINT32						      lastTickBytesReceived_;
	ACE_UINT32                           lastTickBytesSent_;

	ACE_UINT64						      inactivityExceptionPeriod_;
	ACE_UINT64                           lastRecvTime_;
	ACE_UINT32                           winSize_;


	Channel(NetworkInterface* networkInterface = NULL,
		ACE_Event_Handler* endpoint = NULL,
		ChannelScope traits = EXTERNAL,
		ProtocolType pt = PROTOCOL_TCP,
		bool canFilterPacket = false,
		ChannelID id = CHANNEL_ID_NULL);

	~Channel(void);

	void startInactivityDetection(float period, float checkPeriod = 1.0f);
	int get_bundles_length(void);

	/**
	 * @TO-DO
	 * should let the networrkinterface class handle the udp sock handler's creation
	 */
	inline bool initialize(ACE_INET_Addr* addr = NULL);
	inline void destroy(void);
	void clearBundles(void);
	void clear_channel(bool warnOnDiscard = false);
	inline void add_delayed_channel(void);
	void hand_shake(void);
	void process_packets(Messages* pMsgHandlers);
	void update_recv_window(Packet* pPacket);
	void on_packet_received(int bytes);

	/// send stuff
	void send(Bundle * pBundle = NULL);
	void send_buffered_bundle();
	bool process_send(void);
	void on_packet_sent(int bytes_cnt, bool is_sent_completely);
	inline void on_bundles_sent_completely(void);

	/**
	* This method is called when the user lofs off or some network errors happens
	*/
	inline void on_error(void);
	inline void set_channel_condem();

	///@TO-DO move to 
	void tcp_send_single_bundle(TCP_SOCK_Handler* pEndpoint, Bundle* pBundle);
	void udp_send_single_bundle(UDP_SOCK_Handler* pEndpoint, Bundle* pBundle, ACE_INET_Addr& addr);

	inline const char*  c_str(void) const;
};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif
