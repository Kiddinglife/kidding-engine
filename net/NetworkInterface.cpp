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
ACE_INT16 extlisteningPort_min,
ACE_INT16 extlisteningPort_max,
const char * extlisteningInterface,
ACE_UINT32 extrbuffer,
ACE_UINT32 extwbuffer,
ACE_INT16 intlisteningPort,
const char * intlisteningInterface,
ACE_UINT32 intrbuffer,
ACE_UINT32 intwbuffer)
:
channelMap_(),
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
		pExtListenerReceiver_ = new TCP_Acceptor_Handler(
			Channel::ChannelScope::EXTERNAL, this);
		pExtListenerReceiver_->reactor(nub_->rec);

		/// if extlisteningInterface is null, we need ask KBEMachined for ip adress
		if( ( extlisteningInterface == NULL || extlisteningInterface[0] == 0 ) )
		{
			ACE_DEBUG(( LM_WARNING,
				"NetworkInterface::@if1::Couldn't parse interface spec '{%s, %s}'"
				"will use wild interface\n",
				"External", "empty ip addr" ));
		} else if( !strcmp(extlisteningInterface, USE_KBEMACHINED) )
		{
			ACE_DEBUG(( LM_INFO,
				"NetworkInterface::Querying KBEMachined for interface\n" ));
		} else ///noy nil, parse it to get the right ip adress string
		{
			ACE_DEBUG(( LM_DEBUG, "D::NetworkInterface::@if2\n" ));
			char ifname[IFNAMSIZ] = { 0 };  /// hold host name like eth0 or localhost
			if( is_ip_addr_valid(extlisteningInterface, ifname) )
			{
				ACE_DEBUG(( LM_DEBUG, "D::NetworkInterface::@if2.1\n" ));
				bool found_port = false;
				ACE_UINT16 listen_port = extlisteningPort_min;
				ACE_INET_Addr addr(extlisteningInterface, listen_port);

				if( extlisteningPort_min != extlisteningPort_max )
				{
					ACE_DEBUG(( LM_DEBUG, "D::NetworkInterface::@if2.1.1::not enqual port\n" ));
					for( ACE_UINT16 i = extlisteningPort_min; i < extlisteningPort_max; ++i )
					{
						listen_port = i;
						addr.set_port_number(listen_port);
						if( pExtListenerReceiver_->open(addr) == -1 )
						{
							continue;
						} else
						{
							found_port = true;
							break;
						}
					}
				} else
				{
					ACE_DEBUG(( LM_DEBUG, "D::NetworkInterface::@if2.1.1::enqual port\n" ));
					if( pExtListenerReceiver_->open(addr) == 0 )
					{
						found_port = true;
					}
				}

				if( !found_port )
					ACE_ERROR(( LM_ERROR,
					"NetworkInterface::recreateListeningSocket({ }) : "
					"Couldn't bind the socket to {%s}:{%s}:{%d}\n",
					"External", extlisteningInterface, listen_port ));
				else
					ACE_DEBUG(( LM_INFO,
					"NetworkInterface::listen on interface {%s, %s, %s, %d}\n",
					"External", ifname, extlisteningInterface, listen_port ));
					
				///Setup socket options
				setnonblocking(true, pExtListenerReceiver_->acceptor_);
				setnodelay(true, pExtListenerReceiver_->acceptor_);
				if( extrbuffer > 0 )
				{
					
				}

			}

		}

	}
}


NetworkInterface::~NetworkInterface()
{

}

bool NetworkInterface::is_ip_addr_valid(const char* spec, char* name)
{
	// start with it cleared
	name[0] = 0;

	/// make sure there's something there
	if( spec == NULL || spec[0] == 0 )
		ACE_ERROR_RETURN(( LM_ERROR,
		"NetworkInterface::get_ip_addr_str::spec is empty string.\n" ), false);

	/// temp varivales
	char iftemp[IFNAMSIZ + 16] = { 0 };

	/// firstly copy the first 16 chars in extlisteningInterface into ipTemp buffer
	/// extlisteningInterface =  192.168.2.4/24 -> ipTemp = 192.168.2.4
	strncpy(iftemp, spec, IFNAMSIZ);
	iftemp[IFNAMSIZ] = 0;

	if( !strcmp(iftemp, "localhost") || !strcmp(iftemp, "127.0.0.1") )
	{
		ACE_OS::strcpy(name, "localhost");
		ACE_DEBUG(( LM_DEBUG,
			"NetworkInterface::get_ip_addr_str::@if1"
			"host name = %s, ip adress = %s\n",
			name, spec ));
		return true;
	}

	/// specified ip address 
	/// redundant but instructive otherwise it will go to else and return error
	char host_name[256];
	if( ACE_OS::hostname(host_name, 256) != 0 )
		ACE_ERROR_RETURN(( LM_ERROR,
		"NetworkInterface::get_ip_addr_str::hostname return false.\n" ), false);

	struct hostent * myhost = ACE_OS::gethostbyname(host_name);
	/// need to miminus locakhost
	ACE_INT8 len = myhost->h_length - 1 - 1;
	while( len >= 0 )
	{
		if( !ACE_OS::strcmp(
			ACE_OS::inet_ntoa(*(in_addr*) &( ( ( struct in_addr* )( myhost->h_addr_list[len] ) )->s_addr )), iftemp) )
		{
			ACE_DEBUG(( LM_DEBUG, "NetworkInterface::get_ip_addr_str:: found it!\n" ));
			ACE_OS::strcpy(name, "eth0");
			ACE_DEBUG(( LM_DEBUG,
				"NetworkInterface::get_ip_addr_str::"
				"host name = %s, ip adress = %s\n",
				name, spec ));
			return true;
		}
		--len;
	}

	return false;

}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL