#include "PacketSender.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

PacketSender::PacketSender(
ProtocolType pt,
ACE_SOCK* endpoint,
NetworkInterface* networkInterface) :
processSend_(pt == PROTOCOL_TCP ? TCP_ProcessSend : UDP_ProcessSend),
endpoint_(endpoint),
networkInterface_(networkInterface)
{
	TRACE("PacketSender::CTOR()");
	TRACE_RETURN_VOID();
}


PacketSender::~PacketSender()
{
	TRACE("PacketSender::DTOR()");
	TRACE_RETURN_VOID();
}

bool TCP_ProcessSend(Channel* pChannel)
{
	TRACE("PacketSender::TCP_ProcessSend()");
	TRACE_RETURN(true);
}

bool UDP_ProcessSend(Channel* pChannel)
{
	TRACE("PacketSender::UDP_ProcessSend()");
	TRACE_RETURN(true);
}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL