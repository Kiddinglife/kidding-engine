#ifndef PacketReceiver_H_
#define PacketReceiver_H_

#include "ace\pre.h"
#include "net\Packet.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

struct Channel;
struct NetworkInterface;
struct Nub;

struct PacketReceiver
{
	NetworkInterface* pNetworkInterface_;
	PacketReceiver();
	~PacketReceiver();
};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif