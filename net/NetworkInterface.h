#ifndef NetworkInterface_H_
#define NetworkInterface_H_

#include "ace\pre.h"
#include "ace/SOCK_Acceptor.h"
#include "net_common.h"
#include "net\Channel.h"
#include "net\NetworkHandler.h"
#include "net\DelayedChannelHandler.h"
#include "net\Bundle.h"
#include "net\Nub.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

const int RECV_BUFFER_SIZE = 16 * 1024 * 1024; // 16MB
const char* const USE_KBEMACHINED = "kbemachine";

struct Bundle;
struct Channel;
struct DelayedChannels;
struct Packet;
struct Nub;
struct Messages;
struct TCP_Acceptor_Handler;
struct UDP_SOCK_Handler;
struct DelayedChannelHandlers;

/**
 * struct NetworkInterface
 * @Brief
 * This class is the abstraction of the network interface in the host machina.
 * It is listenning on the external and internal socket to wait the connection request.
 * When a connection comes, the nub create new socket handler to handle it and will call back
 * register function to register a new channel
 */
struct NetworkInterface
{
	/// it stores external and internal channel 
	typedef std::map<ACE_INET_Addr, Channel*>	ChannelMap;
	ChannelMap                                                    channelMap_;

	/// nub pointer it is actually the event dispatcher 
	Nub*                                                                nub_;

	/// extention data pointer
	void*                                                                pExtensionData_;

	/// listenning acceptor for external and internal interfaces
	TCP_Acceptor_Handler*                                    pExtListenerReceiver_;
	TCP_Acceptor_Handler*                                    pIntListenerReceiver_;

	DelayedChannelHandlers* 						        pDelayedChannels_;

	/// this handler is run when some channel delays, telling the cup layer client to disconnect
	ChannelTimeOutHandler					                pChannelTimeOutHandler_;

	/// this handler is run when some channel is deredistered
	ChannelDeregisterHandler				                pChannelDeregisterHandler_;

	/// mark to indicate if this network interface has external interface 
	const bool								                        isExternal_;

	/// the number of external channels stored in this network interface
	ACE_INT32									                    numExtChannels_;

	/// ctor creates external and internal listenning socket both are tcp
	NetworkInterface(
		Nub*              pDispatcher = NULL,
		ACE_INT16     extlisteningPort_min = 0,
		ACE_INT16     extlisteningPort_max = 0,
		const char *    extlisteningInterface = "NONE",
		ACE_UINT32   extrbuffer = 0,
		ACE_UINT32   extwbuffer = 0,
		ACE_INT16      intlisteningPort = 0,
		const char *    intlisteningInterface = "NONE",
		ACE_UINT32   intrbuffer = 0,
		ACE_UINT32   intwbuffer = 0);

	/// release all resouces and clear the map
	~NetworkInterface();

	/**
	* This method will extacrt the host or domain name and ip adress based on the given string
	*
	* @param {in} spec ip address string such as 192.168.2.5 127.0.0.1 and so on
	*
	* @param {out} host name or domain name such as localhost or eth0
	* @ret bool true if success false if fail
	*
	* @responsibility the caller needs to ensure @param spec like ip addr host name  does exist
	*
	* gethostbyname()
	* return ip adrr given host name or domain name
	* 通过域名或者主机命返回IP地址. 传进去的参数是一个域名或者主机名.
	* 返回值是一个Hostent指针结构.(如传进去的是一个空字符串,那么返回的是本机的主机名与IP地址)
	*
	* gethostname()
	* get host name or domain name
	* 得到本机主机名或者域名.有两个参数,一个是用来存放主机名或者域名的变量,一个是缓冲区的大小.
	*/
	bool is_ip_addr_valid(const char* spec, char* name);

	/// Send methods
	void add_delayed_channel(Channel* channel);
	void send_delayed_channel(Channel* channel);

	/// These three methods are used to register and deregister the channel
	bool register_channel(Channel* pChannel);
	bool deregister_channel(Channel* pChannel);
	bool deregister_all_channels();

	/// These twp methods are used to find the channel 
	Channel* channel(const ACE_INET_Addr& addr);
	Channel* channel(ACE_HANDLE  handle);

	/// call back functon when the specific channel goes away
	void on_channel_left(Channel * pChannel);
	void on_channel_timeout(Channel * pChannel);

	/**
	* @Unused
	* This method is used to handle the timout event in network interface
	* It just simply print the internal and external interface infos
	*/
	int on_handle_timeout(const ACE_Time_Value &tv, const void *arg);

	/// this method will go through all the channels and process its packets
	inline void process_all_channels_packets(Messages* pMsgHandlers);
	inline int process_all_channels_buffered_sending_packets(void);

	/// close listenning sockets by remove the handlers from the reactor
	void close_listenning_sockets(void);

};

int NetworkInterface::process_all_channels_buffered_sending_packets()
{
	//TRACE("NetworkInterface::process_all_channels_buffered_sending_packets()");

	static ChannelMap::iterator iter = channelMap_.begin();
	static ChannelMap::iterator end = channelMap_.end();

	iter = channelMap_.begin();
	end = channelMap_.end();
	while( iter != end )
	{
		iter->second->send_buffered_bundle();
		++iter;
	}
	return 0;
	//TRACE_RETURN(0);
}

/// this method will go through all the channels and process its packets
void NetworkInterface::process_all_channels_packets(Messages* pMsgHandlers)
{
	TRACE("NetworkInterface::process_all_channels_packets()");
	ChannelMap::iterator iter = channelMap_.begin();
	while( iter != channelMap_.end() )
	{
		Channel* pChannel = iter->second;

		if( pChannel->isDestroyed_ || pChannel->isCondemn_ )
		{
			++iter;
			deregister_channel(pChannel);
			pChannel->destroy();
		} else
		{
			pChannel->process_packets(pMsgHandlers);
			++iter;
		}
	}
	TRACE_RETURN_VOID();
}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif