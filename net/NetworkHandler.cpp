#include "net\NetworkHandler.h"
#include "common\ace_object_pool.h"
#include "net\NetworkInterface.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

//ACE_PoolPtr_Getter(TCP_SOCK_Handler_Pool, TCP_SOCK_Handler, ACE_Null_Mutex);
//ACE_PoolPtr_Declare(TCP_SOCK_Handler_Pool, TCP_SOCK_Handler, ACE_Null_Mutex);
//ACE_PoolPtr_Declare(Channel_Pool, Channel, ACE_Null_Mutex);

int TCP_Acceptor_Handler::open(const ACE_INET_Addr &listen_addr)
{
	if( this->acceptor_.open(listen_addr, 1) == -1 )
		ACE_ERROR_RETURN(( LM_ERROR,
		ACE_TEXT("%p\n"),
		ACE_TEXT("acceptor.open") ),
		-1);
	return this->reactor()->register_handler(this, ACE_Event_Handler::ACCEPT_MASK);
}

//@TO-DO
int TCP_Acceptor_Handler::handle_input(ACE_HANDLE fd)
{
	TCP_SOCK_Handler* client = TCP_SOCK_Handler_Pool->Ctor();

	if( this->acceptor_.accept(client->sock_) == -1 )
	{
		TCP_SOCK_Handler_Pool->Dtor(client);
		ACE_ERROR_RETURN(( LM_ERROR,
			ACE_TEXT("(%P|%t) %p\n"),
			ACE_TEXT("Failed to accept ")
			ACE_TEXT("client connection") ),
			-1);
	}

	client->reactor(this->reactor());

	if( client->open() == -1 )
	{
		client->handle_close(ACE_INVALID_HANDLE,
			ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL);
	}

	Channel* pchannel = Channel_Pool->Ctor(networkInterface_, client, channelScope_);
	pchannel = pchannel;

	if( !networkInterface_->register_channel(pchannel) )
	{
		ACE_ERROR(( LM_ERROR,
			"TCP_Acceptor_Handler::handle_input {%s} is failed!\n",
			pchannel->c_str() ));
	}

	return 0;
}

int TCP_Acceptor_Handler::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
{
	if( this->acceptor_.get_handle() != ACE_INVALID_HANDLE )
	{
		this->reactor()->remove_handler(this, close_mask);
		this->acceptor_.close();
	}
	return 0;
}

int TCP_SOCK_Handler::handle_timeout(const ACE_Time_Value &current_time, const void* act)
{
	TRACE("TCP_SOCK_Handler::handle_timeout()");

	time_t epoch = ( (timespec_t) current_time ).tv_sec;

	ACE_DEBUG(( LM_INFO,
		ACE_TEXT("%M::TCP_SOCK_Handler::handle_timeout(%s)\n"),
		ACE_OS::ctime(&epoch) ));

	switch( (int) act )
	{
		case Channel::TIMEOUT_INACTIVITY_CHECK:
		{
			if( timestamp() - pChannel_->lastRecvTime_ > pChannel_->inactivityExceptionPeriod_ )
			{
				networkInterface_->on_channel_timeout(pChannel_);
			}
			break;
		}
		default:
			break;
	}

	TRACE_RETURN(0);
}

bool TCP_SOCK_Handler::process_send(Channel* pChannel)
{
	TRACE(" TCP_SOCK_Handler::process_send()");

	TRACE_RETURN(true);
}

int TCP_SOCK_Handler::open(void)
{
	static ACE_TCHAR peer_name[MAXHOSTNAMELEN];
	static ACE_INET_Addr peer_addr;

	if( this->sock_.get_remote_addr(peer_addr) == 0 &&
		peer_addr.addr_to_string(peer_name, MAXHOSTNAMELEN) == 0 )

		ACE_DEBUG(( LM_DEBUG, "(%P|%t) Connection from %s\n", peer_name ));

	return this->reactor()->register_handler(this, ACE_Event_Handler::READ_MASK);
}

int TCP_SOCK_Handler::handle_close(ACE_HANDLE, ACE_Reactor_Mask mask)
{
	ACE_DEBUG(( LM_INFO, ACE_TEXT("%M::TCP_SOCK_Handler::handle_close(%d)\n"), mask ));

	if( mask == ACE_Event_Handler::WRITE_MASK )
		return 0;

	if( mask == ACE_Event_Handler::TIMER_MASK )
	{
		ACE_DEBUG(( LM_INFO, ACE_TEXT("%M::TCP_SOCK_Handler::handle_close::TIMER_MASK\n") ));
		return 0;
	}

	this->reactor()->remove_handler(this, mask);
	this->sock_.close();
	TCP_SOCK_Handler_Pool->Dtor(this);
	return 0;
}
int TCP_SOCK_Handler::handle_input(ACE_HANDLE fd)
{
	return 0;
}

int TCP_SOCK_Handler::handle_output(ACE_HANDLE fd)
{
	return 0;
}

// Called when input is available from the client.
int UDP_SOCK_Handler::handle_input(ACE_HANDLE fd)
{
	return 0;
}

// Called when output is possible.
int UDP_SOCK_Handler::handle_output(ACE_HANDLE fd)
{
	return 0;
}

// Called when this handler is removed from the ACE_Reactor.
int UDP_SOCK_Handler::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
{
	return 0;
}

bool UDP_SOCK_Handler::process_send(Channel* pChannel)
{
	TRACE(" UDP_SOCK_Handler::process_send()");

	TRACE_RETURN(true);
}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL