#include "NetworkInterface.h"
#include "net\Channel.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

struct DelayedChannels { };

NetworkInterface::NetworkInterface(Nub* pDispatcher,
	ACE_INT32 extlisteningPort_min,
	ACE_INT32 extlisteningPort_max,
	const char * extlisteningInterface,
	ACE_UINT32 extrbuffer,
	ACE_UINT32 extwbuffer,
	ACE_INT32 intlisteningPort,
	const char * intlisteningInterface,
	ACE_UINT32 intrbuffer,
	ACE_UINT32 intwbuffer) :
	extEndpoint_(),
	intEndpoint_(),
	channelMap_(),
	nub_(pDispatcher),
	pExtensionData_(NULL),
	pExtListenerReceiver_(NULL),
	pIntListenerReceiver_(NULL),
	pDelayedChannels_(new DelayedChannels()),
	pChannelTimeOutHandler_(NULL),
	pChannelDeregisterHandler_(NULL),
	isExternal_(extlisteningPort_min != -1),
	numExtChannels_(0)
{
	if( isExternal_ )
	{
		pExtListenerReceiver_ = new TCP_Acceptor_Handler(ChannelScope::EXTERNAL, 
		&extEndpoint_, this);

	}
}


NetworkInterface::~NetworkInterface()
{
}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL