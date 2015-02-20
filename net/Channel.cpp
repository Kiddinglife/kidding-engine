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
timerID_(-1),
channelType_(CHANNEL_NORMAL),
componentID_(UNKNOWN_COMPONENT_TYPE),
pMsgs_(NULL)
{
	clearBundles();
	initialize();
}

Channel::~Channel()
{
	clear_channel(false);
}

void Channel::clear_channel(bool warnOnDiscard /*=false*/)
{
	TRACE("Channel::clear_channe()");
	/// Stop Inactivity Detection
	if( timerID_ != -1 && pEndPoint_ ) pEndPoint_->reactor()->cancel_timer(timerID_);

	/// clear the bundles
	this->clearBundles();

	/// clear the unprocessed recv packets
	for( int i = 0; i < 2; ++i )
	{
		if( recvPackets_[i].size() > 0 )
		{
			RecvPackets::iterator iter = recvPackets_[i].begin();
			int hasDiscard = 0;

			for( ; iter != recvPackets_[i].end(); ++iter )
			{
				if( ( *iter )->length() > 0 ) hasDiscard++;
				Packet_Pool->Dtor(( *iter ));
			}

			if( hasDiscard > 0 && warnOnDiscard )
			{
				ACE_DEBUG(( LM_WARNING,
					"Channel::clear_channel( {%s} ): "
					"Discarding {%d} buffered packet(s)\n",
					this->c_str(), hasDiscard ));
			}

			recvPackets_[i].clear();
		}
	}

	/// reset all variables values
	lastRecvTime_ = ::timestamp();
	isDestroyed_ = false;
	isCondemn_ = false;
	numPacketsSent_ = 0;
	numPacketsReceived_ = 0;
	numBytesSent_ = 0;
	numBytesReceived_ = 0;
	lastTickBytesReceived_ = 0;
	proxyID_ = 0;
	strextra_ = "";
	channelType_ = CHANNEL_NORMAL;
	recvPacketIndex_ = 0;

	/// clear the pendpoint
	if( !isDestroyed_ )
	{
		if( protocolType_ == PROTOCOL_TCP )
		{
			if( sending_ ) sending_ = false;

			if( pNetworkInterface_ )
			{
				pNetworkInterface_->on_channel_left(this);
			}
		} else if( protocolType_ == PROTOCOL_UDP )
		{
			/// 由于pEndPoint通常由外部给入，必须释放，频道重新激活时会重新赋值
			pEndPoint_->handle_close(ACE_INVALID_HANDLE, ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL);
		}
		pEndPoint_ = NULL;
	}

	if( pPacketReader_ )
	{
		PacketReader_Pool->Dtor(pPacketReader_);
		pPacketReader_ = NULL;
	}

	TRACE_RETURN_VOID();
}

void Channel::add_delayed_channel(void)
{
	pNetworkInterface_->add_delayed_channel(this);
}

void Channel::reset(ACE_Event_Handler* pEndPoint, bool warnOnDiscard)
{
	/// if adrr does not change, we do not change it
	if( pEndPoint == pEndPoint_ ) return;

	/// let pNetworkInterface_ the handle irself in next tick 
	if( pNetworkInterface_ ) pNetworkInterface_->send_delayed_channel(this);

	this->clear_channel(warnOnDiscard);
	pEndPoint_ = pEndPoint;
}

const char * Channel::c_str() const
{
	TRACE("Channel::c_str()");

	static char dodgyString[MAX_BUF] = { "None" };
	static ACE_INET_Addr addr;
	char tdodgyString[MAX_BUF] = { 0 };

	if( pEndPoint_ )
	{
		if( protocolType_ == PROTOCOL_TCP )
		{
			( (TCP_SOCK_Handler*) pEndPoint_ )->sock_.get_remote_addr(addr);

		} else
		{
			( (UDP_SOCK_Handler*) pEndPoint_ )->sock_.get_local_addr(addr);
		}

		addr.addr_to_string(tdodgyString, MAX_BUF);
		kbe_snprintf(
			dodgyString,
			MAX_BUF,
			"Addr(%s), channelId_(%d), isCondemn_(%d), isDestroyed_(%d)\n",
			tdodgyString, channelId_,
			isCondemn_, isDestroyed_);
	}

	TRACE_RETURN(dodgyString);
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

void Channel::startInactivityDetection(float period, float checkPeriod)
{
	if( timerID_ != -1 ) this->pEndPoint_->reactor()->cancel_timer(timerID_);

	if( period > 0.f )
	{
		inactivityExceptionPeriod_ = (ACE_UINT64) ( period * stampsPerSecond() );
		lastRecvTime_ = timestamp();
		ACE_Time_Value interval;
		interval.set_msec(( ACE_UINT64(checkPeriod * 1000) ));
		timerID_ = this->pEndPoint_->reactor()->schedule_timer(pEndPoint_,
			(void*) TIMEOUT_INACTIVITY_CHECK, ACE_Time_Value::zero, interval);
	}
}

bool Channel::initialize(ACE_INET_Addr* addr)
{
	TRACE("Channel::initialize()");

	if( protocolType_ == PROTOCOL_UDP )
	{
		pEndPoint_ = UDP_SOCK_Handler_Pool->Ctor(*addr, pNetworkInterface_);
		( (UDP_SOCK_Handler*) pEndPoint_ )->pChannel_ = this;
	}

	startInactivityDetection(( channelScope_ == INTERNAL ) ? g_channelInternalTimeout : g_channelExternalTimeout);

	TRACE_RETURN(true);
}

bool Channel::finalise(void)
{
	TRACE("Channel::finalise()");

	this->clear_channel();

	if( pPacketReader_ )
	{
		PacketReader_Pool->Dtor(pPacketReader_);
		pPacketReader_ = NULL;
	}


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