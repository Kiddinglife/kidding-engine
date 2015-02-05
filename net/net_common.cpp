#include "net_common.h"
#include "Message.h"
#include "Packet.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

namespace UDP /*ÒÔºóÀ©Õ¹ÓÃ*/
{
}

namespace TCP
{
}

bool g_debugEntity = false;
bool g_appPublish = 1;

bool g_packetAlwaysContainLength = false;
int g_trace_packet = 1;
bool g_trace_packet_use_logfile = true;
std::vector<std::string> g_trace_packet_disables;

static const char* name = GET_KBE_SRV_COMPONENT_TYPE_NAME(g_componentType);
static std::string n(name);
std::ofstream normal(( n + ".log" ).c_str());

static  std::ofstream packetlogos("packetlogs.log");

void TRACE_MESSAGE_PACKET(bool isrecv, Packet* pPacket,
	Message* pCurrMsgHandler, size_t length, const char* addr)
{
	TRACE("TRACE_MESSAGE_PACKET");
	//if( !g_trace_packet )	return;

	if( g_trace_packet_use_logfile )
	{
		ACE_DEBUG(( LM_INFO,
			"%M::TRACE_MESSAGE_PACKET::Log file is setup to packetlogs.log, "
			"Please see packetlogs.log for the packet dump results.\n" ));
		ACE_LOG_MSG->msg_ostream(&packetlogos);
	}

	bool isprint = true;
	if( pCurrMsgHandler )
	{
		std::vector<std::string>::iterator iter = std::find(g_trace_packet_disables.begin(),
			g_trace_packet_disables.end(), pCurrMsgHandler->name_);

		if( iter != g_trace_packet_disables.end() )
		{
			ACE_DEBUG(( LM_INFO,
				"%M::TRACE_MESSAGE_PACKET::This packet is setup untrackable\n" ));
			isprint = false;
		} else
		{
			ACE_DEBUG(( LM_INFO,
				"\n%M::{%s} msgname:{%s}, msgID:{%d}, currMsgPayloadLength:{%d}, addr:{%s}\n",
				( isrecv == true ) ? "====>" : "<====",
				pCurrMsgHandler->name_.c_str(), pCurrMsgHandler->msgID_, length, addr ));
		}
	}

	if( isprint )
	{
		ACE_DEBUG(( LM_INFO,
			"%M::The curr packet rd pos = %d, wr pos = %d\n",
			pPacket->buff->rd_ptr(), pPacket->buff->wr_ptr() ));
		ACE_HEX_DUMP(( LM_INFO, pPacket->buff->rd_ptr(), pPacket->length() ));
	}

	if( g_trace_packet_use_logfile )
	{
		ACE_LOG_MSG->msg_ostream(&normal);
		ACE_DEBUG(( LM_INFO,
			"%M::TRACE_MESSAGE_PACKET::Log file is set back to %s\n",
			( n + ".log" ).c_str() ));
	}

	TRACE_RETURN_VOID();
}

float g_channelInternalTimeout = 60.f;
float g_channelExternalTimeout = 60.f;

ACE_INT8 g_channelExternalEncryptType = 0;

ACE_UINT32 g_SOMAXCONN = 1024;

// network stats
ACE_UINT64						g_numPacketsSent = 0;
ACE_UINT64						g_numPacketsReceived = 0;
ACE_UINT64						g_numBytesSent = 0;
ACE_UINT64						g_numBytesReceived = 0;

ACE_UINT32						g_receiveWindowMessagesOverflowCritical = 32;
ACE_UINT32						g_intReceiveWindowMessagesOverflow = 65535;
ACE_UINT32						g_extReceiveWindowMessagesOverflow = 256;
ACE_UINT32						g_intReceiveWindowBytesOverflow = 0;
ACE_UINT32						g_extReceiveWindowBytesOverflow = 65535;

// Í¨µÀ·¢ËÍ³¬Ê±ÖØÊÔ
ACE_UINT32						g_intReSendInterval = 10;
ACE_UINT32						g_intReSendRetries = 0;
ACE_UINT32						g_extReSendInterval = 10;
ACE_UINT32						g_extReSendRetries = 3;

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL