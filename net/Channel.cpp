#include "Channel.h"
#include "net\NetworkInterface.h"
#include "net\PacketSender.h"
#include "net\PacketReader.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

ACE_PoolPtr_Getter(BundlePool, Bundle, ACE_Null_Mutex);
ACE_PoolPtr_Getter(PacketSender_Pool, PacketSender, ACE_Null_Mutex);

Channel::
Channel(
NetworkInterface* networkInterface,
ACE_Event_Handler* endpoint, //ACE_SOCK* endpoint,
ChannelScope traits,
ProtocolType pt,
PacketFilterPtr pFilter,
ChannelID id) :

pNetworkInterface_(networkInterface),
channelScope_(traits),
protocolType_(pt),
channelId_(id),
inactivityTimerHandle_(),
inactivityExceptionPeriod_(0),
lastRecvTime_(0),
bundle_(),
recvPacketIndex_(0),
pPacketReader_(NULL),
isDestroyed_(false),
numPacketsSent_(0),
numPacketsReceived_(0),
numBytesSent_(0),
numBytesReceived_(0),
lastTickBytesReceived_(0),
lastTickBytesSent_(0),
pFilter_(pFilter),
pEndPoint_(endpoint /*NULL*/),
//pPacketReceiver_(NULL),
//pPacketSender_(NULL),
sending_(false),
isCondemn_(false),
proxyID_(0),
strextra_(),
channelType_(CHANNEL_NORMAL),
componentID_(UNKNOWN_COMPONENT_TYPE),
pMsgs_(NULL)
{
	intrusive_add_ref(this);
	clearBundles();
	initialize();
}

Channel::~Channel()
{
	finalise();
}

void Channel::clearState(bool warnOnDiscard /*=false*/)
{
	/// clear the buffered recv packets
	for( int i = 0; i < 2; ++i )
	{

	}
}

void Channel::add_delayed_channel(void)
{
	pNetworkInterface_->add_delayed_channel(this);
}

void Channel::reset(const ACE_Event_Handler* pEndPoint, bool warnOnDiscard)
{
	/// 如果地址没有改变则不重置
	/// if adrr does not change, we do not change it
	if( pEndPoint == pEndPoint_ ) return;
	// 让网络接口下一个tick处理自己
	/// handle in next tick 
	if( pNetworkInterface_ ) pNetworkInterface_->send_delayed_channel(this);

}

int Channel::handle_timeout(const ACE_Time_Value &current_time, const void *act)
{
	TRACE("Channel::handle_timeout()");

	switch( *( (int*) act ) )
	{
		case TIMEOUT_INACTIVITY_CHECK:
		{
			if( timestamp() - lastRecvTime_ > inactivityExceptionPeriod_ )
			{
				pNetworkInterface_->on_channel_timeout(this);
			}
			break;
		}
		default:
			break;
	}

	TRACE_RETURN(0);
}

const char * Channel::c_str() const
{
	//TRACE("Channel::c_str()");
	//TRACE_RETURN("Channel::c_str()");
	return "channael::c_str()";
}

void Channel::clearBundles(void)
{
	TRACE("Channel::clearBundle()");

	Bundles::iterator iter = bundles_.begin();
	for( ; iter != bundles_.end(); ++iter )
	{
		BundlePool->Dtor(( *iter ));
	}
	bundles_.clear();

	TRACE_RETURN_VOID();
}

bool Channel::initialize(ACE_INET_Addr* addr)
{
	TRACE("Channel::initialize_udp_sock_handler()");

	if( protocolType_ == PROTOCOL_UDP )
	{
		pEndPoint_ = UDP_SOCK_Handler_Pool->Ctor(*addr, pNetworkInterface_);
		( (UDP_SOCK_Handler*) pEndPoint_ )->pChannel_ = this;
	}
	//startInactivityDetection(( traits_ == INTERNAL ) ? g_channelInternalTimeout :
	//	g_channelExternalTimeout);

	TRACE_RETURN(true);
}

bool Channel::finalise(void)
{
	TRACE("Channel::finalise()");

	if( pNetworkInterface_ != NULL && pEndPoint_ != NULL && !isDestroyed_ )
	{
		pNetworkInterface_->on_channel_left(this);
	}

	if( pPacketReader_ )
	{
		PacketReader_Pool->Dtor(pPacketReader_);
		pPacketReader_ = NULL;
	}

	if( protocolType_ == PROTOCOL_UDP )
		pEndPoint_->handle_close(ACE_INVALID_HANDLE, ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL);

	TRACE_RETURN(true);
}

void Channel::destroy(void)
{
	TRACE("Channel::destroy()");
	TRACE_RETURN_VOID();
}

void Channel::process_packets(Messages* pMsgHandlers)
{
	TRACE("Channel::process_packets()");
	TRACE_RETURN_VOID();
}

void Channel::send(Bundle * pBundle)
{
	TRACE("Channel::send(Bundle * pBundle)");

	/// check to see if the current channel is avaiable to use
	if( isDestroyed_ )
	{
		ACE_DEBUG(( LM_ERROR, "Channel::send({%s}): channel has destroyed.\n", this->c_str() ));
		clearBundles();
		if( pBundle ) BundlePool->Dtor(pBundle);
		return;
	}

	if( isCondemn_ )
	{
		ACE_DEBUG(( LM_ERROR,
			"Channel::send: is error, reason={%s}, from {%s}.\n", reasonToString(REASON_CHANNEL_CONDEMN),
			c_str() ));
		clearBundles();
		if( pBundle ) BundlePool->Dtor(pBundle);
		return;
	}

	/// Add this bundle to the array
	if( pBundle ) bundles_.push_back(pBundle);

	/// if no bundle to send, we just stop here
	size_t bundles_count = bundles_.size();
	if( !bundles_count ) return;

	if( !sending_ )
	{
		protocolType_ == PROTOCOL_TCP ? ( (TCP_SOCK_Handler*) pEndPoint_ )->process_send(this) : ( (UDP_SOCK_Handler*) pEndPoint_ )->process_send(this);

		// 如果不能立即发送到系统缓冲区，那么交给poller处理
		if( bundles_.size() && !isCondemn_ && !isDestroyed_ )
		{
			sending_ = true;
			pNetworkInterface_->nub_->rec->register_handler(pEndPoint_, ACE_Event_Handler::WRITE_MASK);
		}
	}

	TRACE_RETURN_VOID();
}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL