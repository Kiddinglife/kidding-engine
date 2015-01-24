#include "NetworkInterface.h"
#include "net\Channel.h"
#include "net\NetworkHandler.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

struct DelayedChannels { };

NetworkInterface::NetworkInterface(
	Nub* pDispatcher,
	ACE_INT32 extlisteningPort_min,
	ACE_INT32 extlisteningPort_max,
	const char * extlisteningInterface,
	ACE_UINT32 extrbuffer,
	ACE_UINT32 extwbuffer,
	ACE_INT32 intlisteningPort,
	const char * intlisteningInterface,
	ACE_UINT32 intrbuffer,
	ACE_UINT32 intwbuffer)
	:
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
		pExtListenerReceiver_ = new TCP_Acceptor_Handler(Channel::ChannelScope::EXTERNAL,
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
			char ipaddr[IFNAMSIZ];
			get_ip_addr_str(extlisteningInterface, ipaddr);
		}

	}
}


NetworkInterface::~NetworkInterface()
{
}

bool NetworkInterface::get_ip_addr_str(const char* spec, char* name)
{
	// start with it cleared
	name[0] = 0;

	/// make sure there's something there
	if( spec == NULL || spec[0] == 0 ) return false;

	/// temp varivales
	char * slash = NULL;
	int netmaskbits = 32;
	char iftemp[IFNAMSIZ + 16] = { 0 };
	unsigned long addr = 0;

	/// firstly copy the first 16 chars in extlisteningInterface into ipTemp buffer
	/// extlisteningInterface =  192.168.2.4/24 -> ipTemp = 192.168.2.4
	strncpy(iftemp, spec, IFNAMSIZ);
	iftemp[IFNAMSIZ] = 0;

	// see if it's a netmask
	if( ( slash = const_cast<char *>( strchr(spec, '/') ) ) && slash - spec <= 16 )
	{
		/// change "/" to '\0' to have ip adrr like this 192.168.2.4
		iftemp[slash - spec] = 0;
		netmaskbits = ACE_OS::atoi(slash + 1);
		if( !( netmaskbits > 0 && netmaskbits <= 32 &&
			ACE_OS::inet_aton(iftemp, ( struct in_addr* )&addr) ) )
		{
			ACE_ERROR_RETURN(( LM_ERROR,
				"NetworkInterface::get_ip_addr_str::netmask match {%s} length {%s} is not valid.\n",
				iftemp, slash + 1 ),
				false);
		}
	} else if( !strcmp(iftemp, "eth0") )
	{
		char host_name[256];
		if( !ACE_OS::hostname(host_name, 256) )
			ACE_ERROR_RETURN(( LM_ERROR,
			"NetworkInterface::get_ip_addr_str::hostname return false.\n" ), false);

		struct hostent * myhost = ACE_OS::gethostbyname(host_name);
		if( !myhost )
			ACE_ERROR_RETURN(( LM_ERROR,
			"NetworkInterface::get_ip_addr_str::gethostbyname return false.\n" ), false);
		addr = ( ( struct in_addr* )( myhost->h_addr_list[0] ) )->s_addr;
		ACE_OS::strncpy(name, iftemp, IFNAMSIZ);
	} else if( !strcmp(name, "lo") )
	{
		addr = ACE_HTONL(0x7F000001);
		ACE_OS::strncpy(name, iftemp, IFNAMSIZ);
	} else if( ACE_OS::inet_aton(iftemp, ( struct in_addr* )&addr) )
	{
		// specified ip address
		netmaskbits = 32; // redundant but instructive
	} else
	{
		ACE_ERROR_RETURN(( LM_ERROR,
			"NetworkInterface::get_ip_addr_str::"
			"No interface matching interface spec {%s} found\n", spec ),
			false);
	}

	///  if we haven't set a name yet then we're supposed to look up the ip address
	if( name[0] == 0 )
	{
		ACE_DEBUG((LM_DEBUG, "STILL NOT FIND A NAME\n"));
		int netmaskshift = 32 - netmaskbits;
		unsigned long netmaskmatch = ACE_NTOHL(addr);
		std::vector< std::string > interfaceNames;

	}

}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL