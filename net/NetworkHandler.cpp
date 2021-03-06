﻿#include "net\NetworkHandler.h"
#include "common\ace_object_pool.h"
#include "net\NetworkInterface.h"
#include "net/ErrorStatsMgr.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

int TCP_Acceptor_Handler::handle_timeout(const ACE_Time_Value &current_time, const void* act)
{
	TRACE("TCP_SOCK_Handler::handle_timeout()");
	networkInterface_->on_handle_timeout(current_time, act);
	TRACE_RETURN(0);
}

int TCP_Acceptor_Handler::open(const ACE_INET_Addr &listen_addr)
{
	if( this->acceptor_.open(listen_addr, 1) == -1 )
		ACE_ERROR_RETURN(( LM_ERROR,
		ACE_TEXT("%p\n"),
		ACE_TEXT("acceptor.open") ),
		-1);
	this->acceptor_.enable(ACE_NONBLOCK);
	return this->reactor()->register_handler(this, ACE_Event_Handler::ACCEPT_MASK);
}

int TCP_Acceptor_Handler::handle_input(ACE_HANDLE fd)
{
	TCP_SOCK_Handler* client = TCP_SOCK_Handler_Pool->Ctor();
	if( this->acceptor_.accept(client->sock_) == -1 )
	{
		TCP_SOCK_Handler_Pool->Dtor(client);
		networkInterface_->nub_->pErrorReporter_->reportException(REASON_GENERAL_NETWORK);
		ACE_ERROR_RETURN(( LM_ERROR,
			"%M::TCP_Acceptor_Handler::handle_input::Failed to accept client connection(%s)",
			kbe_strerror() ), 0);
	}

	client->reactor(this->reactor());

	if( client->open() == -1 )
	{
		client->handle_close(ACE_INVALID_HANDLE,
			ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL);
	}

	Channel* pchannel = Channel_Pool->Ctor(networkInterface_, client, channelScope_);
	client->pChannel_ = pchannel;
	if( !networkInterface_->register_channel(pchannel) )
	{
		ACE_ERROR(( LM_ERROR,
			"TCP_Acceptor_Handler::handle_input {%s} is failed!\n",
			pchannel->c_str() ));
		Channel_Pool->Dtor(pchannel);
		pchannel = NULL;
	}

	return 0;
}

int TCP_Acceptor_Handler::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
{
	/// reactor will call remove_handler and then call this method anf the 
	//  this->acceptor_.get_handle() must be = ACE_INVALID_HANDLE so this handle the delte accptor
	if( this->acceptor_.get_handle() != ACE_INVALID_HANDLE )
	{
		this->reactor()->remove_handler(this, close_mask);
		this->acceptor_.close();
	}
	return 0;
}

int TCP_SOCK_Handler::handle_timeout(const ACE_Time_Value &current_time, const void* act)
{
	time_t epoch = ( (timespec_t) current_time ).tv_sec;
	ACE_DEBUG(( LM_INFO,
		ACE_TEXT("%M::TCP_SOCK_Handler::handle_timeout(%s)\n"),
		ACE_OS::ctime(&epoch) ));

	switch( (int) act )
	{
		case Channel::TIMEOUT_INACTIVITY_CHECK:
		{
			if( timestamp() - pChannel_->lastRecvTime_ >= pChannel_->inactivityExceptionPeriod_ )
			{
				pChannel_->pNetworkInterface_->on_channel_timeout(pChannel_);
			}
			break;
		}
		default:
			break;
	}

	return 0;
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

	setnonblocking(true, this->sock_);
	setnodelay(true, this->sock_);

	if( this->sock_.get_remote_addr(peer_addr) == 0 &&
		peer_addr.addr_to_string(peer_name, MAXHOSTNAMELEN) == 0 )
		ACE_DEBUG(( LM_DEBUG, "(%P|%t) Connection from %s\n", peer_name ));

	return this->reactor()->register_handler(this, ACE_Event_Handler::READ_MASK);
}

int TCP_SOCK_Handler::handle_close(ACE_HANDLE, ACE_Reactor_Mask mask)
{
	//ACE_DEBUG(( LM_INFO, ACE_TEXT("%M::TCP_SOCK_Handler::handle_close(%d)\n"), mask ));

	if( mask == ACE_Event_Handler::WRITE_MASK )
		return 0;

	if( mask == ACE_Event_Handler::TIMER_MASK )
	{
		return 0;
	}

	this->reactor()->remove_handler(this, mask);
	this->sock_.close();
	TCP_SOCK_Handler_Pool->Dtor(this);
	return 0;
}

int TCP_SOCK_Handler::handle_input(ACE_HANDLE fd)
{
	//TRACE("TCP_SOCK_Handler::handle_input()");
	/// this is to make the recv get error to reset the reactor
	if( this->process_recv(/*expectingPacket:*/true) )
	{
		while( this->process_recv(/*expectingPacket:*/false) )
		{
			/* pass */;
		}
	}
	//this->reactor()->schedule_wakeup(this, ACE_Event_Handler::WRITE_MASK);
	//TRACE_RETURN(0);
	return 0;
}

bool TCP_SOCK_Handler::process_recv(bool expectingPacket)
{
	//TRACE(" TCP_SOCK_Handler::process_recv()");

	//ACE_DEBUG(( LM_DEBUG, "%M::expectingPacket = %s\n",
	//	expectingPacket ? "true" : "false" ));

	if( !pChannel_ ) return false; //TRACE_RETURN(false);

	static Packet* pReceiveWindow = NULL;
	static int len = 0;
	static RecvState recv_state = RECV_STATE_BREAK;

	if( pChannel_->isCondemn_ )
	{
		pChannel_->on_error();
		return false;
		//	TRACE_RETURN(false);
	}

	pReceiveWindow = Packet_Pool->Ctor();

	len = sock_.recv(pReceiveWindow->osbuff_->wr_ptr(), pReceiveWindow->osbuff_->size());

	if( len > 0 )
	{
		pReceiveWindow->osbuff_->wr_ptr(len);
		// 注意:必须在大于0的时候否则DEBUG_MSG将会导致WSAGetLastError返回0从而陷入死循环
		//ACE_DEBUG(( LM_DEBUG, "%M::TCP_SOCK_Handler::process_recv(): datasize={%d}, wpos={%d}.\n", len, pReceiveWindow->osbuff_->wr_ptr() ));
	}

	if( len < 0 )
	{
		if( pChannel_ ) Packet_Pool->Dtor(pReceiveWindow);
		if( ( recv_state = pChannel_->checkSocketErrors(len, expectingPacket) ) == RecvState::RECV_STATE_INTERRUPT )
		{
			pChannel_->on_error();
			pChannel_ = NULL;
			return false;
			//TRACE_RETURN(false);
		}
		//TRACE_RETURN(recv_state == RecvState::RECV_STATE_CONTINUE);
		return ( recv_state == RecvState::RECV_STATE_CONTINUE );
	}

	/// the client log off 
	if( len == 0 )
	{
		ACE_DEBUG(( LM_DEBUG, "%M::Client logs off...\n" ));
		Packet_Pool->Dtor(pReceiveWindow);
		pChannel_->on_error();
		pChannel_ = NULL;
		return false;
		//TRACE_RETURN(false);
	}

	/*Reason ret = process_recv_packet(pReceiveWindow);*/
	pChannel_->on_packet_received(pReceiveWindow->length());
	if( pChannel_->canFilterPacket_ )
	{
		// filter
	}
	// 如果为None， 则可能是被过滤器过滤掉了(过滤器正在按照自己的规则组包解密)
	if( pReceiveWindow )
	{
		pChannel_->update_recv_window(pReceiveWindow);
	}///
	return true;
	//TRACE_RETURN(true);
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