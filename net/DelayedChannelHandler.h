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
	void init(Nub* dispatcher, NetworkInterface* pNetworkInterface);
	void fini(Nub* dispatcher);

	void add(Channel* channel);

	void sendIfDelayed(Channel* channel);

	virtual bool process();

	typedef std::set<ACE_INET_Addr> ChannelAddrs;
	ChannelAddrs channeladdrs_;

	NetworkInterface* pNetworkInterface_;

	DelayedChannelHandlers();
	~DelayedChannelHandlers();
};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif