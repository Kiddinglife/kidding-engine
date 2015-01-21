#ifndef NetworkInterface_H_
#define NetworkInterface_H_
#include "ace\pre.h"
#include "net_common.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

const int RECV_BUFFER_SIZE = 0;
const char * USE_KBEMACHINED = 0;

struct Bundle;
struct Channel;
struct Bundle;
struct Packet;
struct Nub;
struct Messages;
struct NetworkInterface
{
	typedef std::map<ACE_INET_Addr, Channel*>	ChannelMap;

	NetworkInterface();
	~NetworkInterface();
};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif