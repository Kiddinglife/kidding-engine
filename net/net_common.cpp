#include "net_common.h"
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
int g_trace_packet = 0;
bool g_trace_packet_use_logfile = false;
std::vector<std::string> g_trace_packet_disables;

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