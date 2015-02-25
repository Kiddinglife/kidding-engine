#include "NetStats.h"
#include "NetworkHandler.h"
#include "Message.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

/**
* @Brief
* 该方法会更新该消息的stats，之后会调用所有的回掉函数来记录相关的网络数据
* This method eill update the stats of the msg and then call all callbacks to
* record the related network stats.
* /n
* @para OPTION op
* is this sent msg or received msg 发送的消息还是接受的消息
* @para Message* msg
* msg pointer 消息指针
* @para ACE_UINT32 size
* the length of this msg including all fields in it
* 该消息的总长度包括包含所有的域
* /n
* @ret void
*/
void NetStats::trackMessage(OPTION op, Message* msg, ACE_UINT32 size)
{
	TRACE("%M::NetStats :: trackMessage()");
	if( op == SEND )
	{
		msg->send_size_ += size;
		msg->send_count_++;
	} else
	{
		msg->recv_size_ += size;
		msg->recv_count_++;
	}

	std::vector<MsgTraceHandler*>::iterator iter = handlers_.begin();
	for( ; iter != handlers_.end(); ++iter )
	{
		if( op == SEND )
			( *iter )->onSendMessage(msg, size);
		else
			( *iter )->onRecvMessage(msg, size);
	}
	TRACE_RETURN_VOID();
}

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL