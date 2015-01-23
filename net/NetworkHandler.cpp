#include "net\NetworkHandler.h"
#include "common\ace_object_pool.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

int TCP_Acceptor_Handler::open(const ACE_INET_Addr &listen_addr)
{
	if( this->acceptor_->open(listen_addr, 1) == -1 )
		ACE_ERROR_RETURN(( LM_ERROR,
		ACE_TEXT("%p\n"),
		ACE_TEXT("acceptor.open") ),
		-1);
	return this->reactor()->register_handler
		(this, ACE_Event_Handler::ACCEPT_MASK);
}

//@TO-DO
int TCP_Acceptor_Handler::handle_input(ACE_HANDLE fd)
{
	ACE_PoolPtr_Getter(pool, TCP_SOCK_Handler, ACE_Null_Mutex);
	TCP_SOCK_Handler* client = pool->Ctor();
	if( this->acceptor_->accept(client->sock_) == -1 )
	{
		pool->Dtor(client);
		ACE_ERROR_RETURN(( LM_ERROR,
			ACE_TEXT("(%P|%t) %p\n"),
			ACE_TEXT("Failed to accept ")
			ACE_TEXT("client connection") ),
			-1);
	}
	client->reactor(this->reactor());
	if( client->open() == -1 ) client->handle_close(ACE_INVALID_HANDLE, 0);

	Channel* pchannel = new Channel(networkInterface_, &client->sock_,
		channelScope_, PROTOCOL_TCP);
	//if( !networkInterface_.registerChannel(pchannel) )
	//{
	//	ERROR_MSG(fmt::format("ListenerReceiver::handleInputNotification:registerChannel({}) is failed!\n",pchannel->c_str()));
	//}
	return 0;
}

int TCP_Acceptor_Handler::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
{
	if( this->acceptor_->get_handle() != ACE_INVALID_HANDLE )
	{
		ACE_Reactor_Mask m = ACE_Event_Handler::ACCEPT_MASK | ACE_Event_Handler::DONT_CALL;
		this->reactor()->remove_handler(this, m);
		this->acceptor_->close();
	}
	return 0;
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
	if( mask == ACE_Event_Handler::WRITE_MASK ) return 0;
	mask = ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL;
	this->reactor()->remove_handler(this, mask);
	this->sock_.close();
	//this->output_queue_.flush();
	ACE_PoolPtr_Getter(pool, TCP_SOCK_Handler, ACE_Null_Mutex);
	pool->Dtor(this);
	return 0;
}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL