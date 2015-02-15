#ifndef PacketSender_H_
#define PacketSender_H_

#include "ace\pre.h"
#include "net\Packet.h"
#include "net\Message.h"
#include "net\Channel.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

typedef bool(*ProcessSendFunc)( Channel* pChannel );
extern bool TCP_ProcessSend(Channel* pChannel);
extern bool UDP_ProcessSend(Channel* pChannel);

struct PacketSender
{
	ProcessSendFunc processSend_;
	ACE_SOCK* endpoint_;
	NetworkInterface* networkInterface_;

	PacketSender(ProtocolType pt = PROTOCOL_TCP,
		ACE_SOCK* endpoint = NULL,
		NetworkInterface* networkInterface = NULL);
	~PacketSender();
};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif
