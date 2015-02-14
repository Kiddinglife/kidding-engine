#include "Channel.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

Channel::Channel(NetworkInterface* networkInterface,
ACE_SOCK* endpoint,
ChannelScope traits,
ProtocolType pt,
PacketFilterPtr pFilter,
ChannelID id)
:
pNetworkInterface_(networkInterface),
channelScope_(traits),
protocolType_(pt),
channelId_(id),
inactivityTimerHandle_(),
inactivityExceptionPeriod_(0),
lastRecvTime_(0),
bundle_(),
recvPacketIndex_(0),
pPacketReader_(0),
isDestroyed_(false),
numPacketsSent_(0),
numPacketsReceived_(0),
numBytesSent_(0),
numBytesReceived_(0),
lastTickBytesReceived_(0),
lastTickBytesSent_(0),
pFilter_(pFilter),
pEndPoint_(NULL),
pPacketReceiver_(NULL),
pPacketSender_(NULL),
sending_(false),
isCondemn_(false),
proxyID_(0),
strextra_(),
channelType_(CHANNEL_NORMAL),
componentID_(UNKNOWN_COMPONENT_TYPE),
pMsgs_(NULL)
{
	intrusive_add_ref(this);
	this->clearBundle();
	this->pEndPoint_ = endpoint;
	this->initialize();
}

const char * Channel::c_str() const
{
	//TRACE("Channel::c_str()");
	//TRACE_RETURN("Channel::c_str()");
	return "channael::c_str()";
}

void Channel::clearBundle()
{
	//TRACE("Channel::clearBundle()");
	//TRACE_RETURN_VOID();
}

bool Channel::initialize()
{
	//TRACE("Channel::initialize()");
	//TRACE_RETURN(true);
	return true;
}

void Channel::destroy(void)
{
	//TRACE("Channel::destroy()");
	//TRACE_RETURN_VOID();
}

void Channel::process_packets(Messages* pMsgHandlers)
{
	TRACE("Channel::process_packets()");
	TRACE_RETURN_VOID();
}

void Channel::send(Bundle * pBundle)
{
	TRACE("Channel::send(Bundle * pBundle)");
	TRACE_RETURN_VOID();
}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL