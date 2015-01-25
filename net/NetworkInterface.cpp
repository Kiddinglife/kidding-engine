#include "NetworkInterface.h"
#include "net\Channel.h"
#include "net\NetworkHandler.h"
#include "net\DelayedChannelHandler.h"
#include "net\Bundle.h"
#include "net\Nub.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL


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
channelMap_(),
extEndpoint_(),
intEndpoint_(),
nub_(pDispatcher),
pExtensionData_(NULL),
pExtListenerReceiver_(NULL),
pIntListenerReceiver_(NULL),
pDelayedChannels_(new DelayedChannelHandlers()),
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
				ACE_DEBUG(( LM_INFO,
				"NetworkInterface::@1::Querying KBEMachined for interface\n" ));
		} else ///noy nil, parse it to get the right ip adress string
		{
			ACE_DEBUG(( LM_DEBUG, "NetworkInterface::@2\n" ));
			/// char buf to hold str like 192.168.2.4
			char ifname[IFNAMSIZ] = { 0 };
			unsigned long ipaddr_long = INADDR_ANY;
			if( get_net_interface_info(extlisteningInterface, ifname, ipaddr_long) )
			{

			}

		}

	}
}


NetworkInterface::~NetworkInterface()
{
}

bool NetworkInterface::get_net_interface_info(const char* spec, char* name, unsigned long& addr)
{
	ACE_DEBUG(( LM_DEBUG,
		"NetworkInterface::get_ip_addr_str::@1::"
		"spec = %s, name = %s, addr = %d\n",
		spec, name, addr ));
	// start with it cleared
	name[0] = 0;

	/// make sure there's something there
	if( spec == NULL || spec[0] == 0 ) return false;

	/// temp varivales
	char * slash = NULL;
	int netmaskbits = 32;
	char iftemp[IFNAMSIZ + 16] = { 0 };

	/// firstly copy the first 16 chars in extlisteningInterface into ipTemp buffer
	/// extlisteningInterface =  192.168.2.4/24 -> ipTemp = 192.168.2.4
	strncpy(iftemp, spec, IFNAMSIZ);
	iftemp[IFNAMSIZ] = 0;

	ACE_DEBUG(( LM_DEBUG, "NetworkInterface::get_ip_addr_str::@2::iftemp = %s\n", iftemp ));

	// @if1 see if it's a netmask
	if( ( slash = const_cast<char *>( strchr(spec, '/') ) ) && slash - spec <= 16 )
	{
		ACE_DEBUG(( LM_DEBUG,
			"NetworkInterface::get_ip_addr_str::@if1::slash - spec = %d\n", slash - spec ));

		/// change "/" to '\0' to have ip adrr like this 192.168.2.4
		iftemp[slash - spec] = 0;

		ACE_DEBUG(( LM_DEBUG,
			"NetworkInterface::get_ip_addr_str::@if1::@1:: now iftemp = %s\n", iftemp ));

		netmaskbits = ACE_OS::atoi(slash + 1);

		ACE_DEBUG(( LM_DEBUG,
			"NetworkInterface::get_ip_addr_str::@if1::@2::  netmaskbits = %d\n", netmaskbits ));

		if( !( netmaskbits > 0 && netmaskbits <= 32 &&
			ACE_OS::inet_aton(iftemp, ( struct in_addr* )&addr) ) )
		{
			ACE_ERROR_RETURN(( LM_ERROR,
				"NetworkInterface::get_ip_addr_str::netmask match {%s} length {%s} is not valid.\n",
				iftemp, slash + 1 ),
				false);
		}

		ACE_DEBUG(( LM_DEBUG,
			"NetworkInterface::get_ip_addr_str::@if2{eth0}"
			"host name = %s, ip adress = %s\n",
			*name == 0 ? "None" : name, ACE_OS::inet_ntoa(*(in_addr*) &addr) ));

	} else if( !strcmp(iftemp, "eth0") || !strcmp(iftemp, "any") )
	{
		char host_name[256];
		if( ACE_OS::hostname(host_name, 256) != 0 )
			ACE_ERROR_RETURN(( LM_ERROR,
			"NetworkInterface::get_ip_addr_str::hostname return false.\n" ), false);

		ACE_DEBUG(( LM_DEBUG,
			"NetworkInterface::get_ip_addr_str::@if2{eth0}"
			"host name = %s,\n",
			host_name));

		struct hostent * myhost = ACE_OS::gethostbyname(host_name);
		if( !myhost )
			ACE_ERROR_RETURN(( LM_ERROR,
			"NetworkInterface::get_ip_addr_str::gethostbyname return false.\n" ), false);
		addr = ( ( struct in_addr* )( myhost->h_addr_list[0] ) )->s_addr;
		ACE_OS::strncpy(name, "eth0", IFNAMSIZ);

		ACE_DEBUG(( LM_DEBUG,
			"NetworkInterface::get_ip_addr_str::@if2{eth0}"
			"host name = %s, ip adress = %s\n",
			name, ACE_OS::inet_ntoa(*(in_addr*) &addr) ));

	} else if( !strcmp(iftemp, "lo") )
	{
		addr = ACE_HTONL(0x7F000001);
		ACE_OS::strncpy(name, iftemp, IFNAMSIZ);

		ACE_DEBUG(( LM_DEBUG,
			"NetworkInterface::get_ip_addr_str::@if3{lo}"
			"host name = %s, ip adress = %s\n",
			name, ACE_OS::inet_ntoa(*(in_addr*) &addr) ));

	} else if( ACE_OS::inet_aton(iftemp, ( struct in_addr* )&addr) )
	{
		ACE_DEBUG(( LM_DEBUG, "NetworkInterface::get_ip_addr_str::@if4{ip}\n" ));
		/// specified ip address 
		/// redundant but instructive otherwise it will go to else and return error
		netmaskbits = 32;
		unsigned long addrtemp;
		struct hostent * myhost;
		const char* const n[ ] = { "eth0", "eth1", "lo" };
		int i = 0;
		while( i < 3 )
		{
			myhost = ACE_OS::gethostbyname(n[i]);
			addrtemp = ( ( struct in_addr* )( myhost->h_addr_list[0] ) )->s_addr;
			ACE_DEBUG(( LM_DEBUG, "NetworkInterface::get_ip_addr_str::@if4{ip}::"
				"n = %s\n", n[i] ));
			if( addr == addrtemp )
			{
				ACE_OS::strcpy(name, n[i]);
				ACE_DEBUG(( LM_DEBUG,
					"NetworkInterface::get_ip_addr_str::@if2{eth0}"
					"host name = %s, ip adress = %s\n",
					*name == 0 ? "None" : name, ACE_OS::inet_ntoa(*(in_addr*) &addr) ));
				return true;
			}
			++i;
		}

		ACE_ERROR_RETURN(( LM_ERROR,
			"NetworkInterface::get_ip_addr_str::"
			"No interface matching interface spec {%s} found\n", spec ),
			false);

	} else
	{
		ACE_ERROR_RETURN(( LM_ERROR,
			"NetworkInterface::get_ip_addr_str::"
			"No interface matching interface spec {%s} found\n", spec ),
			false);
	}

	///  if we haven't set a name yet then we're supposed to look up the ip address
	//if( name[0] == 0 )
	//{
	//	ACE_DEBUG(( LM_DEBUG, "host name is emopty, starts find host name...\n" ));
	//	int netmaskshift = 32 - netmaskbits;
	//	unsigned long netmaskmatch = ACE_NTOHL(addr);
	//	std::vector< std::string > interfaceNames;
	//}

	return true;

}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL