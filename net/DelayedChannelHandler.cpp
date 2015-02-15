#include "DelayedChannelHandler.h"
#include "Nub.h"
#include "Channel.h"
#include "NetworkInterface.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

void DelayedChannelHandlers::init(Nub* dispatcher, NetworkInterface* pNetworkInterface)
{
	TRACE("DelayedChannelHandlers::init()");
	pNetworkInterface_ = pNetworkInterface;
	dispatcher->frequentTasks_.add(this);
	TRACE_RETURN_VOID();
}

void DelayedChannelHandlers::fini(Nub* dispatcher)
{
	TRACE("DelayedChannelHandlers::fini()");
	dispatcher->frequentTasks_.cancel(this);
	TRACE_RETURN_VOID();
}

void DelayedChannelHandlers::add(Channel* channel)
{
	TRACE("DelayedChannelHandlers::add()");

	channel->protocolType_ == PROTOCOL_TCP ?
		( (TCP_SOCK_Handler*) channel->pEndPoint_ )->sock_.get_remote_addr(channel_addr_) :
		( (UDP_SOCK_Handler*) channel->pEndPoint_ )->sock_.get_local_addr(channel_addr_);

	channeladdrs_.insert(channel_addr_);
	TRACE_RETURN_VOID();
}

void DelayedChannelHandlers::send_delayed_channel(Channel* channel)
{
	TRACE("DelayedChannelHandlers::sendIfDelayed()");

	channel->protocolType_ == PROTOCOL_TCP ?
		( (TCP_SOCK_Handler*) channel->pEndPoint_ )->sock_.get_remote_addr(channel_addr_) :
		( (UDP_SOCK_Handler*) channel->pEndPoint_ )->sock_.get_local_addr(channel_addr_);

	if( channeladdrs_.erase(channel_addr_) > 0 )
	{
		channel->send();
	}
	TRACE_RETURN_VOID();
}

bool DelayedChannelHandlers::process()
{
	TRACE("DelayedChannelHandlers::process()");

	ChannelAddrs::iterator iter = channeladdrs_.begin();
	while( iter != channeladdrs_.end() )
	{
		Channel * pChannel = pNetworkInterface_->channel(( *iter ));
		if( pChannel && ( pChannel->isCondemn_ || !pChannel->isDestroyed_ ) )
		{
			pChannel->send();
		}

		++iter;
	}
	channeladdrs_.clear();
	return true;

	TRACE_RETURN(true);
}

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL