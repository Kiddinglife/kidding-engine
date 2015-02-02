#include "DelayedChannelHandler.h"
#include "ace\Trace.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

DelayedChannelHandlers::DelayedChannelHandlers()
{
}


DelayedChannelHandlers::~DelayedChannelHandlers()
{
	TRACE("DelayedChannelHandlers::dtor()");
	///
	TRACE_RETURN_VOID();
}

void DelayedChannelHandlers::init(Nub* dispatcher, NetworkInterface* pNetworkInterface)
{
	TRACE("DelayedChannelHandlers::init()");
	///
	TRACE_RETURN_VOID();
}

void DelayedChannelHandlers::fini(Nub* dispatcher)
{
	TRACE("DelayedChannelHandlers::fini()");
	///
	TRACE_RETURN_VOID();
}

void DelayedChannelHandlers::add(Channel* channel)
{
	TRACE("DelayedChannelHandlers::add()");
	///
	TRACE_RETURN_VOID();
}

void DelayedChannelHandlers::sendIfDelayed(Channel* channel)
{
	TRACE("DelayedChannelHandlers::sendIfDelayed()");
	///
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