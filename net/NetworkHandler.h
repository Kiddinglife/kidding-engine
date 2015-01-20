﻿#ifndef NETWORK_HANDLER_H_
#define NETWORK_HANDLER_H_
#include "ace\pre.h"
#include "ace\Event_Handler.h"
#include "ace/SOCK_Stream.h"
#include "net_common.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

struct Channel;
struct Message;

/**
 * 此类接口用于接收h和发送普通的Network输入和输出消息
 * This handler is used to handle network events
 */
struct TCP_SOCK_Handler : public ACE_Event_Handler
{
	ACE_SOCK_Stream sock_;

	//FUZZ: disable check_for_lack_ACE_OS
	int open(void);
	//FUZZ: enable check_for_lack_ACE_OS

	// Get this handler's I/O handle.
	virtual ACE_HANDLE get_handle(void) const
	{ return this->sock_.get_handle(); }

	// Called when input is available from the client.
	virtual int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);

	// Called when output is possible.
	virtual int handle_output(ACE_HANDLE fd = ACE_INVALID_HANDLE);

	// Called when this handler is removed from the ACE_Reactor.
	virtual int handle_close(ACE_HANDLE handle,
		ACE_Reactor_Mask close_mask);
};
struct UDP_SOCK_Handler : public ACE_Event_Handler
{
	ACE_SOCK_Dgram sock_;

	//FUZZ: disable check_for_lack_ACE_OS
	int open(void);
	//FUZZ: enable check_for_lack_ACE_OS

	// Get this handler's I/O handle.
	virtual ACE_HANDLE get_handle(void) const
	{ return this->sock_.get_handle(); }

	// Called when input is available from the client.
	virtual int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);

	// Called when output is possible.
	virtual int handle_output(ACE_HANDLE fd = ACE_INVALID_HANDLE);

	// Called when this handler is removed from the ACE_Reactor.
	virtual int handle_close(ACE_HANDLE handle,
		ACE_Reactor_Mask close_mask);
};

/** 此类接口用于接收一个网络通道超时消息*/
struct ChannelTimeOutHandler : public ACE_Event_Handler
{
	virtual int handle_timeout(const ACE_Time_Value &current_time, const void * = 0);
};

/** 此类接口用于接收一个内部网络通道取消注册*/
struct ChannelDeregisterHandler
{
	virtual void onChannelDeregister(Channel * pChannel) = 0;
};
typedef void(*OnChannelDeregister)( Channel * pChannel );
extern OnChannelDeregister g_onChannelDeregister;

/* 此类接口用于监听消息跟踪事件 */
struct MsgTraceHandler
{
	virtual void onSendMessage(const Message* msgHandler, int size) = 0;
	virtual void onRecvMessage(const Message* msgHandler, int size) = 0;
};
typedef void(*OnSendMessage)( const Message& msgHandler, int size );
typedef void(*OnRecvMessage)( const Message& msgHandler, int size );
extern OnSendMessage g_onSendMessage;
extern OnRecvMessage g_onRecvMessage;

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif