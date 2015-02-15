#ifndef DelayedChannelHandler_H_
#define DelayedChannelHandler_H_

#include "ace\pre.h"
#include "common\task.h"
#include "net_common.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

struct NetworkInterface;
struct Channel;
struct Nub;
struct DelayedChannelHandlers : public Task
{
	typedef std::set<ACE_INET_Addr> ChannelAddrs;

	ChannelAddrs channeladdrs_;
	NetworkInterface* pNetworkInterface_;
	ACE_INET_Addr channel_addr_;

	DelayedChannelHandlers() :Task(), channeladdrs_(), pNetworkInterface_(NULL), channel_addr_()
	{
		TRACE("DelayedChannelHandlers::Ctor()");

		TRACE_RETURN_VOID();
	}

	~DelayedChannelHandlers()
	{
		TRACE("DelayedChannelHandlers::dtor()");

		TRACE_RETURN_VOID();
	}

	void init(Nub* dispatcher, NetworkInterface* pNetworkInterface);
	void fini(Nub* dispatcher);
	void add(Channel* channel);
	void send_delayed_channel(Channel* channel);
	virtual bool process();
};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif