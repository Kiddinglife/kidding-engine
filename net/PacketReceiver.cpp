#include "PacketReceiver.h"
#include "net\NetworkInterface.h"
#include "net\Channel.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

TCP_Packet_Receiver::TCP_Packet_Receiver() :
pNetworkInterface_(NULL),
pEndpoint_(NULL)
{

}

Reason processPacket(Channel* pChannel, Packet * pPacket)
{
	TRACE("TRACE::TCP_Packet_Receiver::processPacket(Channel* pChannel, Packet* pPacket)");
	
	TRACE_RETURN(Reason::REASON_CHANNEL_CONDEMN);
}

Channel* TCP_Packet_Receiver::getChannel()
{
	TRACE("TCP_Packet_Receiver::getChannel()");
	TRACE_RETURN(pNetworkInterface_->channel(pEndpoint_->get_handle()));
}

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL