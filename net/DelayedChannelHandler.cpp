#include "DelayedChannelHandler.h"
#include "Nub.h"
#include "Channel.h"
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
	channel->pEndPoint_->get_local_addr(channel_local_addr_);
	channeladdrs_.insert(channel_local_addr_);
	TRACE_RETURN_VOID();
}

void DelayedChannelHandlers::sendIfDelayed(Channel* channel)
{
	TRACE("DelayedChannelHandlers::sendIfDelayed()");
	channel->pEndPoint_->get_local_addr(channel_local_addr_);
	if( channeladdrs_.erase(channel_local_addr_) > 0 )
	{
		channel->send();
	}
	TRACE_RETURN_VOID();
}

bool DelayedChannelHandlers::process()
{
	TRACE("DelayedChannelHandlers::process()");
	///
	TRACE_RETURN(true);
}

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL