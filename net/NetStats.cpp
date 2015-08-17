#include "NetStats.h"
#include "Message.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

void NetStats::trackMessage(OPTION op, Message* msg, ACE_UINT32 size)
{
	TRACE("NetStats :: trackMessage()");
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

inline void NetStats::addHandler(MsgTraceHandler* pHandler) { handlers_.push_back(pHandler); }

void NetStats::removeHandler(MsgTraceHandler* pHandler)
{
	std::vector<MsgTraceHandler*>::iterator iter = handlers_.begin();
	for( ; iter != handlers_.end(); ++iter )
	{
		if( ( *iter ) == pHandler )
		{
			handlers_.erase(iter);
			break;
		}
	}
}

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL