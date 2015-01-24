#ifndef DelayedChannelHandler_H_
#define DelayedChannelHandler_H_

#include "ace\pre.h"
#include "net_common.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

class DelayedChannelHandler
{
	public:
	DelayedChannelHandler();
	~DelayedChannelHandler();
};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif