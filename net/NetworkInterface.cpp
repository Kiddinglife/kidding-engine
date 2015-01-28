﻿#include "NetworkInterface.h"
#include "net\Channel.h"
#include "net\NetworkHandler.h"
#include "net\DelayedChannelHandler.h"
#include "net\Bundle.h"
#include "net\Nub.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL


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
				ACE_INET_Addr addr(extlisteningInterface, listen_port);

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

				ACE_INET_Addr addr(intlisteningInterface, intlisteningPort);
				pIntListenerReceiver_ = new UDP_SOCK_Handler(addr,
					Channel::ChannelScope::INTERNAL, this);
				pIntListenerReceiver_->reactor(nub_->rec);

				ACE_DEBUG(( LM_INFO,
					"NetworkInterface::listen on interface {%s, %s, %s, %d}\n",
					pEndPointName, ifname, intlisteningInterface, intlisteningPort ));

				///Setup socket options
				setnonblocking(true, pIntListenerReceiver_->sock_);
				setnodelay(true, pIntListenerReceiver_->sock_);
				setreuseaddr(true, pIntListenerReceiver_->sock_);
				if( extrbuffer > 0 )
				{
					setbuffersize(SO_RCVBUF, extrbuffer, pIntListenerReceiver_->sock_);
				}
				if( extwbuffer > 0 )
				{
					setbuffersize(SO_SNDBUF, extrbuffer, pIntListenerReceiver_->sock_);
				}
			}
		}
	}
}


NetworkInterface::~NetworkInterface()
{
	TRACE("NetworkInterface::deregisterAllChannels\n");

	deregisterAllChannels();
	if( nub_ )
	{
	
	}
	SAFE_RELEASE(pDelayedChannels_);
	SAFE_RELEASE(pExtListenerReceiver_);
	SAFE_RELEASE(pIntListenerReceiver_);

	TRACE_RETURN_VOID();
}

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

bool NetworkInterface::registerChannel(Channel* pChannel)
{
	TRACE("NetworkInterface::registerChannel\n");
	/// get the current channel's address
	ACE_INET_Addr localAddr;
	pChannel->pEndPoint_->get_local_addr(localAddr);

	ACE_ASSERT(localAddr.get_ip_address() != 0);
	ACE_ASSERT(pChannel->pNetworkInterface_ == this);

	/// check if this channel has already been added
	ChannelMap::iterator iter = channelMap_.find(localAddr);
	Channel * pExisting = iter != channelMap_.end() ? iter->second : NULL;
	if( pExisting )
	{
		ACE_ERROR_RETURN(( LM_CRITICAL,
			"NetworkInterface::registerChannel: channel {%s} is exist.\n",
			pChannel->c_str() ), false);
	}

	/// if not added  then add it
	channelMap_[localAddr] = pChannel;

	///if it is external channel, increment by 1
	if( pChannel->channelScope_ = Channel::EXTERNAL )
		numExtChannels_++;

	TRACE_RETURN(true);
}

bool NetworkInterface::deregisterChannel(Channel* pChannel)
{
	TRACE("NetworkInterface::deregisterChannel\n");

	/// get the current channel's address
	ACE_INET_Addr localAddr;
	pChannel->pEndPoint_->get_local_addr(localAddr);

	///if it is external channel, decrement by 1
	if( pChannel->channelScope_ = Channel::EXTERNAL )
		numExtChannels_--;

	if( !channelMap_.erase(localAddr) )
	{
		ACE_DEBUG(( LM_ERROR, "NetworkInterface::deregisterChannel: "
			"Channel not found {}!\n",
			pChannel->c_str() ));
		return false;
	}


	if( pChannelDeregisterHandler_ )
	{
		pChannelDeregisterHandler_->onChannelDeregister(pChannel);
	}

	TRACE_RETURN(true);
}

bool NetworkInterface::deregisterAllChannels()
{
	TRACE("NetworkInterface::deregisterAllChannels\n");

	ChannelMap::iterator iter = channelMap_.begin();
	while( iter != channelMap_.end() )
	{
		ChannelMap::iterator oldIter = iter++;
		oldIter->second->destroy();
	}

	channelMap_.clear();
	numExtChannels_ = 0;

	TRACE_RETURN(true);
}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL