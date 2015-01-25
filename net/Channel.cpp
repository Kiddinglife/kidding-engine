#include "Channel.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

Channel::Channel(NetworkInterface* networkInterface,
ACE_SOCK_IO* endpoint,
ChannelScope traits,
ProtocolType pt,
PacketFilterPtr pFilter,
ChannelID id)
{
}

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL