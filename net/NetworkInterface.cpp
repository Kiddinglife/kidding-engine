#include "NetworkInterface.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

/**
 * ctor creates external and internal listenning socket both are tcp
 */
 NetworkInterface::NetworkInterface(
 Nub* pDispatcher,
 ACE_INT16 extlisteningPort_min,
 ACE_INT16 extlisteningPort_max,
 const char * extlisteningInterface,
 ACE_UINT32 extrbuffer,
 ACE_UINT32 extwbuffer,
 ACE_INT16 intlisteningPort,
 const char * intlisteningInterface,
 ACE_UINT32 intrbuffer,
 ACE_UINT32 intwbuffer)
 :
 channelMap_(),
 nub_(pDispatcher),
 pExtensionData_(NULL),
 pExtListenerReceiver_(NULL),
 pIntListenerReceiver_(NULL),
 pDelayedChannels_(new DelayedChannelHandlers()),
 pChannelTimeOutHandler_(NULL),
 pChannelDeregisterHandler_(NULL),
 isExternal_(extlisteningPort_min != -1),
 numExtChannels_(0)
{
	/// first listen on external interface
	if( isExternal_ )
	{
		ACE_DEBUG(( LM_DEBUG, "D::NetworkInterface::setup external interface\n" ));
		pExtListenerReceiver_ = new TCP_Acceptor_Handler(
			Channel::ChannelScope::EXTERNAL, this);
		pExtListenerReceiver_->reactor(nub_->rec);
		char* pEndPointName = "External";

		/// if extlisteningInterface is null, we need ask KBEMachined for ip adress
		if( ( extlisteningInterface == NULL || extlisteningInterface[0] == 0 ) )
		{
			ACE_DEBUG(( LM_WARNING,
				"NetworkInterface::@if1::Couldn't parse interface spec '{%s, %s}'"
				"will use wild interface\n",
				pEndPointName, "empty ip addr" ));
		} else if( !strcmp(extlisteningInterface, USE_KBEMACHINED) )
		{
			ACE_DEBUG(( LM_INFO,
				"NetworkInterface::Querying KBEMachined for interface\n" ));
		} else ///noy nil, parse it to get the right ip adress string
		{
			ACE_DEBUG(( LM_DEBUG, "D::NetworkInterface::@if2\n" ));
			char ifname[IFNAMSIZ] = { 0 };  /// hold host name like eth0 or localhost
			if( is_ip_addr_valid(extlisteningInterface, ifname) )
			{
				ACE_DEBUG(( LM_DEBUG, "D::NetworkInterface::@if2.1\n" ));
				bool found_port = false;
				ACE_UINT16 listen_port = extlisteningPort_min;
				ACE_INET_Addr addr(listen_port, extlisteningInterface);

				if( extlisteningPort_min != extlisteningPort_max )
				{
					ACE_DEBUG(( LM_DEBUG, "D::NetworkInterface::@if2.1.1::not enqual port\n" ));
					for( ACE_UINT16 i = extlisteningPort_min; i < extlisteningPort_max; ++i )
					{
						listen_port = i;
						addr.set_port_number(listen_port);
						if( pExtListenerReceiver_->open(addr) == -1 )
						{
							continue;
						} else
						{
							found_port = true;
							break;
						}
					}
				} else
				{
					ACE_DEBUG(( LM_DEBUG, "D::NetworkInterface::@if2.1.1::enqual port\n" ));
					if( pExtListenerReceiver_->open(addr) == 0 )
					{
						found_port = true;
					}
				}

				if( !found_port )
					ACE_ERROR(( LM_ERROR,
					"NetworkInterface::Couldn't listen on {%s}:{%s}:{%d}\n",
					pEndPointName, extlisteningInterface, listen_port ));
				else
					ACE_DEBUG(( LM_INFO,
					"NetworkInterface::listen on interface {%s, %s, %s, %d}\n",
					pEndPointName, ifname, extlisteningInterface, listen_port ));

				///Setup socket options
				setnonblocking(true, pExtListenerReceiver_->acceptor_);
				setnodelay(true, pExtListenerReceiver_->acceptor_);

				if( extrbuffer > 0 )
				{
					setbuffersize(SO_RCVBUF, extrbuffer, pExtListenerReceiver_->acceptor_);
				}

				if( extwbuffer > 0 )
				{
					setbuffersize(SO_SNDBUF, extrbuffer, pExtListenerReceiver_->acceptor_);
				}
			}
		}
	}

	/// secondly listen on internal interface
	if( intlisteningPort != 0 )
	{
		ACE_DEBUG(( LM_DEBUG, "D::NetworkInterface::setup internal interface\n" ));

		char* pEndPointName = "Internal";
		/// if extlisteningInterface is null, we need ask KBEMachined for ip adress
		if( ( intlisteningInterface == NULL || intlisteningInterface[0] == 0 ) )
		{
			ACE_DEBUG(( LM_WARNING,
				"NetworkInterface::@if1::Couldn't parse interface spec '{%s, %s}'"
				"will use wild interface\n",
				pEndPointName, "empty ip addr" ));
		} else if( !strcmp(intlisteningInterface, USE_KBEMACHINED) )
		{
			ACE_DEBUG(( LM_INFO,
				"NetworkInterface::Querying KBEMachined for interface\n" ));
		} else ///noy nil, parse it to get the right ip adress string
		{
			ACE_DEBUG(( LM_DEBUG, "D::NetworkInterface::starts valid address\n" ));

			char ifname[IFNAMSIZ] = { 0 };  /// hold host name like eth0 or localhost
			if( is_ip_addr_valid(intlisteningInterface, ifname) )
			{
				ACE_DEBUG(( LM_DEBUG, "D::NetworkInterface::qualified address\n" ));

				ACE_INET_Addr addr(intlisteningPort, intlisteningInterface);

				pIntListenerReceiver_ = new TCP_Acceptor_Handler(Channel::ChannelScope::INTERNAL, this);
				pIntListenerReceiver_->reactor(nub_->rec);
				if( pIntListenerReceiver_->open(addr) != 0 )
				{
					ACE_ERROR(( LM_ERROR,
						"NetworkInterface::Couldn't listen on {%s}:{%s}:{%s}:{%d}\n",
						pEndPointName, ifname, intlisteningInterface, intlisteningPort ));
				}

				ACE_DEBUG(( LM_INFO,
					"NetworkInterface::listen on interface {%s, %s, %s, %d}\n",
					pEndPointName, ifname, intlisteningInterface, intlisteningPort ));

				///Setup socket options
				setnonblocking(true, pIntListenerReceiver_->acceptor_);
				setnodelay(true, pIntListenerReceiver_->acceptor_);
				setreuseaddr(true, pIntListenerReceiver_->acceptor_);

				if( extrbuffer > 0 )
				{
					setbuffersize(SO_RCVBUF, extrbuffer, pIntListenerReceiver_->acceptor_);
				}

				if( extwbuffer > 0 )
				{
					setbuffersize(SO_SNDBUF, extrbuffer, pIntListenerReceiver_->acceptor_);
				}
			}
		}
	}
}

/**
 * release all resouces and clear the channels map
 */
NetworkInterface::~NetworkInterface()
{
	//TRACE("NetworkInterface::dtor()");

	deregister_all_channels();

	if( nub_ )
	{
		pDelayedChannels_->fini(nub_);
		pDelayedChannels_ = NULL;
	}

	SAFE_RELEASE(pDelayedChannels_);
	SAFE_RELEASE(pExtListenerReceiver_);
	SAFE_RELEASE(pIntListenerReceiver_);

	//TRACE_RETURN_VOID();
}

/**
 * This method is used to handle the timout event in network interface
 * It just simply print the internal and external interface infos
 */
int NetworkInterface::on_handle_timeout(const ACE_Time_Value& tv, const void* arg)
{
	TRACE("NetworkInterface::handle_timeout()");

	static ACE_INET_Addr intaddr;
	static ACE_INET_Addr extaddr;
	static char intaddrstr[128];
	static char extaddrstr[128];

	pIntListenerReceiver_->acceptor_.get_local_addr(intaddr);
	intaddr.addr_to_string(intaddrstr, 128);

	pExtListenerReceiver_->acceptor_.get_local_addr(extaddr);
	extaddr.addr_to_string(extaddrstr, 128);

	ACE_DEBUG(( LM_INFO,
		"NetworkInterface::handleTimeout: EXTERNAL({%s}), INTERNAL({%s}).\n",
		extaddrstr, intaddrstr ));

	TRACE_RETURN(0);
}

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
bool NetworkInterface::is_ip_addr_valid(const char* spec, char* name)
{
	// start with it cleared
	name[0] = 0;

	/// make sure there's something there
	if( spec == NULL || spec[0] == 0 )
		ACE_ERROR_RETURN(( LM_ERROR,
		"NetworkInterface::get_ip_addr_str::spec is empty string.\n" ), false);

	/// temp varivales
	char iftemp[IFNAMSIZ + 16] = { 0 };

	/// firstly copy the first 16 chars in extlisteningInterface into ipTemp buffer
	/// extlisteningInterface =  192.168.2.4/24 -> ipTemp = 192.168.2.4
	strncpy(iftemp, spec, IFNAMSIZ);
	iftemp[IFNAMSIZ] = 0;

	if( !strcmp(iftemp, "localhost") || !strcmp(iftemp, "127.0.0.1") )
	{
		ACE_OS::strcpy(name, "localhost");
		ACE_DEBUG(( LM_DEBUG,
			"NetworkInterface::get_ip_addr_str::@if1"
			"host name = %s, ip adress = %s\n",
			name, spec ));
		return true;
	}

	/// specified ip address 
	/// redundant but instructive otherwise it will go to else and return error
	char host_name[256];
	if( ACE_OS::hostname(host_name, 256) != 0 )
		ACE_ERROR_RETURN(( LM_ERROR,
		"NetworkInterface::get_ip_addr_str::hostname return false.\n" ), false);

	struct hostent * myhost = ACE_OS::gethostbyname(host_name);
	/// need to miminus locakhost
	ACE_INT8 len = myhost->h_length - 1 - 1;
	while( len >= 0 )
	{
		if( !ACE_OS::strcmp(
			ACE_OS::inet_ntoa(*(in_addr*) &( ( ( struct in_addr* )( myhost->h_addr_list[len] ) )->s_addr )), iftemp) )
		{
			ACE_DEBUG(( LM_DEBUG, "NetworkInterface::get_ip_addr_str:: found it!\n" ));
			ACE_OS::strcpy(name, "eth0");
			ACE_DEBUG(( LM_DEBUG,
				"NetworkInterface::get_ip_addr_str::"
				"host name = %s, ip adress = %s\n",
				name, spec ));
			return true;
		}
		--len;
	}

	return false;

}

/// These three methods are used to register and deregister the channel
bool NetworkInterface::register_channel(Channel* pChannel)
{
	//TRACE("NetworkInterface::registerChannel");
	/// get the current channel's address
	static ACE_INET_Addr addr;
	pChannel->protocolType_ == PROTOCOL_TCP ?
		( (TCP_SOCK_Handler*) pChannel->pEndPoint_ )->sock_.get_remote_addr(addr) :
		( (UDP_SOCK_Handler*) pChannel->pEndPoint_ )->sock_.get_local_addr(addr);

	//ACE_ASSERT(addr.get_ip_address() != 0);
	ACE_ASSERT(pChannel->pNetworkInterface_ == this);

	/// check if this channel has already been added
	ChannelMap::iterator iter = channelMap_.find(addr);
	Channel * pExisting = ( iter != channelMap_.end() ? iter->second : NULL );

	if( pExisting )
	{
		ACE_ERROR_RETURN(( LM_CRITICAL,
			"NetworkInterface::registerChannel: channel {%s} is exist.\n",
			pChannel->c_str() ), false);
	}

	/// if not added  then add it
	channelMap_[addr] = pChannel;

	///if it is external channel, increment by 1
	if( pChannel->channelScope_ = Channel::EXTERNAL )
		numExtChannels_++;

	return true;
	//TRACE_RETURN(true);
}
bool NetworkInterface::deregister_channel(Channel* pChannel)
{
	//TRACE("NetworkInterface::deregisterChannel");

	/// get the current channel's address
	static ACE_INET_Addr addr;
	pChannel->protocolType_ == PROTOCOL_TCP ?
		( (TCP_SOCK_Handler*) pChannel->pEndPoint_ )->sock_.get_remote_addr(addr) :
		( (UDP_SOCK_Handler*) pChannel->pEndPoint_ )->sock_.get_local_addr(addr);
	//pChannel->pEndPoint_-> ->get_local_addr(localAddr);

	///if it is external channel, decrement by 1
	if( pChannel->channelScope_ = Channel::EXTERNAL )
		numExtChannels_--;

	if( !channelMap_.erase(addr) )
	{
		ACE_DEBUG(( LM_ERROR, "NetworkInterface::deregisterChannel: "
			"Channel not found {}!\n",
			pChannel->c_str() ));
		return false;
		//TRACE_RETURN(false);
	}


	if( pChannelDeregisterHandler_ )
	{
		( *pChannelDeregisterHandler_ )( pChannel );
	}

	return true;
	//TRACE_RETURN(true);
}
bool NetworkInterface::deregister_all_channels()
{
	//TRACE("NetworkInterface::deregisterAllChannels");

	ChannelMap::iterator iter = channelMap_.begin();
	while( iter != channelMap_.end() )
	{
		ChannelMap::iterator oldIter = iter++;
		oldIter->second->destroy();
	}

	channelMap_.clear();
	numExtChannels_ = 0;

	return true;
	//TRACE_RETURN(true);
}

void NetworkInterface::add_delayed_channel(Channel* channel)
{
	//TRACE("NetworkInterface::delayedSend()");
	pDelayedChannels_->add(channel);
	//TRACE_RETURN_VOID();
}
void NetworkInterface::send_delayed_channel(Channel* channel)
{
	//TRACE("NetworkInterface::send_on_delayed()");
	pDelayedChannels_->send_delayed_channel(channel);
	//TRACE_RETURN_VOID();
}

/// These twp methods are used to find the channel 
Channel * NetworkInterface::channel(const ACE_INET_Addr& addr)
{
	//TRACE("NetworkInterface::findChannel(const ACE_INET_Addr&)");

	if( !addr.get_ip_address() ) return NULL;
	ChannelMap::iterator iter = channelMap_.find(addr);
	return ( ( iter != channelMap_.end() ) ? iter->second : NULL );

	//TRACE_RETURN(( iter != channelMap_.end() ) ? iter->second : NULL);
}
Channel * NetworkInterface::channel(ACE_HANDLE handle)
{
	//TRACE("NetworkInterface::findChannel(ACE_HANDLE)");

	ChannelMap::iterator iter = channelMap_.begin();
	for( ; iter != channelMap_.end(); ++iter )
	{
		/// test if the end point in the current channel has the handle that equals to handle
		if( iter->second->pEndPoint_ && iter->second->pEndPoint_->get_handle() == handle )
			return  ( iter->second );
		//TRACE_RETURN(iter->second);
	}
	return NULL;
	//TRACE_RETURN(NULL);
}

/// channel cb
void NetworkInterface::on_channel_left(Channel* pChannel)
{
	TRACE("NetworkInterface::onChannelGone()");
	// channel dtor has been called so we cannot call it again
	/// thisi is the last chance for us to do some clear works the pchannel is valid
	/// at this moment
	TRACE_RETURN_VOID();
}
void NetworkInterface::on_channel_timeout(Channel* pChannel)
{
	TRACE("NetworkInterface::onChannelTimeOut()");
	//Channel_Pool->Dtor(pChannel); should use this one but for test we use the secpnd one 
	/// tp cancel the timer
	pChannel->pEndPoint_->reactor()->cancel_timer(pChannel->timerID_);
	//if( pChannelTimeOutHandler_ )
	//{
	//	( *pChannelTimeOutHandler_ )( pChannel );
	//} else
	//{
	//	ACE_ERROR(( LM_ERROR,
	//		"NetworkInterface::onChannelTimeOut: "
	//		"Channel {%s} timed out but no handler is registered.\n",
	//		pChannel->c_str() ));
	//}

	TRACE_RETURN_VOID();
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

void NetworkInterface::close_listenning_sockets(void)
{
	TRACE("NetworkInterface::close_socket()");
	if( pExtListenerReceiver_ )
		pExtListenerReceiver_->handle_close(pExtListenerReceiver_->get_handle(),
		ACE_Event_Handler::ACCEPT_MASK | ACE_Event_Handler::DONT_CALL);

	if( pIntListenerReceiver_ )
		pIntListenerReceiver_->handle_close(pExtListenerReceiver_->get_handle(),
		ACE_Event_Handler::ACCEPT_MASK | ACE_Event_Handler::DONT_CALL);

	TRACE_RETURN_VOID();
}

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL