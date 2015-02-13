#ifndef PacketReceiver_H_
#define PacketReceiver_H_

#include "ace\pre.h"
#include "common\ace_object_pool.h"
#include "net\net_common.h"
#include "net\Packet.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

struct Channel;
struct NetworkInterface;
struct Nub;

struct TCP_Packet_Receiver
{
	NetworkInterface* pNetworkInterface_;
	ACE_SOCK_Stream* pEndpoint_;

	enum RecvState
	{
		RECV_STATE_INTERRUPT = -1,
		RECV_STATE_BREAK = 0,
		RECV_STATE_CONTINUE = 1
	};

	enum PACKET_RECEIVER_TYPE
	{
		TCP_PACKET_RECEIVER = 0,
		UDP_PACKET_RECEIVER = 1
	};

	TCP_Packet_Receiver();
	~TCP_Packet_Receiver() { }

	virtual Reason processPacket(Channel* pChannel, Packet * pPacket);
	virtual Reason processFilteredPacket(Channel* pChannel, Packet * pPacket) = 0;
	virtual bool processRecv(bool expectingPacket) = 0;
	virtual RecvState checkSocketErrors(int len, bool expectingPacket) = 0;
	virtual Channel* getChannel();
};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif