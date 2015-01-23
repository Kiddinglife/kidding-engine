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
		pExtListenerReceiver_->reactor(nub_->rec);

		/// if extlisteningInterface is null, we need ask KBEMachined for ip adress
		if( ( extlisteningInterface == NULL || extlisteningInterface[0] == 0 ) )
		{
			if( strcmp(extlisteningInterface, USE_KBEMACHINED) == 0 )
				ACE_DEBUG(( LM_INFO, "NetworkInterface::Querying KBEMachined for interface\n" ));
		} else ///noy nil, parse it to get the right ip adress string
		{
			/// char buf to hold str like 192.168.2.4
			char ipaddr[16] = { 0 };

			/// temp varivales
			char* slash;
			ACE_UINT8 netMaskBitsCount = 32;
			char ipTemp[32];
			ACE_UINT32 integerAddr = 0;

			/// firstly copy the first 16 chars in extlisteningInterface into ipTemp buffer
			/// extlisteningInterface =  192.168.2.4/24 -> ipTemp = 192.168.2.4
			ACE_OS::strncpy(ipTemp, extlisteningInterface, 16);
			ipTemp[16] = 0;

			///check if extlisteningInterface includes maks number
			if( ( slash = const_cast<char *>( strchr(extlisteningInterface, '/') ) ) && 
			slash - extlisteningInterface <= 16 )
			{

			}
		}

	}
}


NetworkInterface::~NetworkInterface()
{
}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL