#include <iostream>
#include <time.h>
#include <string>
#include "gtest\gtest.h"
#include "ace/Log_Msg.h"
#include "ace/mytrace.h"
#include  "ace/Refcounted_Auto_Ptr.h"
#include "ace/Auto_Ptr.h"
#include "ace/ACE.h"
#include "ace/SOCK_Connector.h"
#include "common/ace_object_pool.h"
#include "net\Message.h"
#include "net\PacketReader.h"
#include "net\NetworkInterface.h"

//TEST(PacketReaderTests, ctor_dtor_test)
//{
//	struct msgarg : public MessageArgs
//	{
//		virtual MessageLength1 args_bytes_count(void)
//		{
//			return 12;
//		}
//		virtual void fetch_args_from(Packet* p)
//		{
//			INT32  para1 = *(INT32*) p->buff->rd_ptr();
//			p->buff->rd_ptr(4);
//			INT32  para2 = *(INT32*) p->buff->rd_ptr();
//			p->buff->rd_ptr(4);
//			INT32  para3 = *(INT32*) p->buff->rd_ptr();
//			ACE_DEBUG(( LM_DEBUG, "(%d)(%d)(%d)\n", para1, para2, para3 ));
//		}
//
//		virtual void add_args_to(Packet* p)
//		{
//		}
//	};
//
//	struct msgarg_variable : public MessageArgs
//	{
//		virtual MessageLength1 args_bytes_count(void)
//		{
//			return 0;
//		}
//		virtual void fetch_args_from(Packet* p)
//		{
//			for( int i = 0; i < 4; i++ )
//			{
//				INT64  para1 = *(INT64*) p->buff->rd_ptr();
//				p->buff->rd_ptr(8);
//				ACE_DEBUG(( LM_DEBUG, "(%d)", para1 ));
//			}
//
//			ACE_DEBUG(( LM_DEBUG, "\n" ));
//		}
//
//		virtual void add_args_to(Packet* p)
//		{
//		}
//	};
//
//	g_channelExternalEncryptType = 0;
//
//	Nub              pDispatcher;
//
//	ACE_INT32     extlisteningPort_min = 20001;
//	ACE_INT32     extlisteningPort_max = 20005;
//	const char *    extlisteningInterface = "192.168.2.47";
//	//const char *    extlisteningInterface = "";
//	//const char *    extlisteningInterface = "127.0.0.1";
//	//const char *    extlisteningInterface = USE_KBEMACHINED;
//	ACE_UINT32   extrbuffer = 512;
//	ACE_UINT32   extwbuffer = 512;
//	ACE_INT32      intlisteningPort = 20006;
//	const char *    intlisteningInterface = "192.168.2.47";
//	ACE_UINT32   intrbuffer = 512;
//	ACE_UINT32   intwbuffer = 512;
//
//	NetworkInterface in(&pDispatcher,
//		extlisteningPort_min,
//		extlisteningPort_max,
//		extlisteningInterface,
//		extrbuffer,
//		extwbuffer,
//		intlisteningPort,
//		intlisteningInterface,
//		intrbuffer,
//		intwbuffer);
//
//	ACE_INET_Addr addr(20006, "192.168.2.47");
//	TCP_SOCK_Handler dg(&in);
//	dg.reactor(pDispatcher.rec);
//
//	Channel channel(&in, &dg);
//	dg.pChannel_ = &channel;
//
//	in.register_channel(&channel);
//
//	ACE_PoolPtr_Getter(pool, Bundle, ACE_Null_Mutex);
//	ACE_PoolPtr_Getter(poolmsgarg, msgarg, ACE_Null_Mutex);
//	ACE_PoolPtr_Getter(poolmsgarg_variable, msgarg_variable, ACE_Null_Mutex);
//	ACE_PoolPtr_Getter(poolmsg, Message, ACE_Null_Mutex);
//	ACE_PoolPtr_Getter(poolpacket, Packet, ACE_Null_Mutex);
//
//	Bundle* p = pool->Ctor();
//	Messages msgs;
//	msgarg* ag = poolmsgarg->Ctor();
//	msgarg_variable* ag_va = poolmsgarg_variable->Ctor();
//
//	/// first msg is fixed msg
//	Message* currhandler1 = poolmsg->Ctor();
//	//FixedMessages::MSGInfo info1 = { 1 };
//	//ACE_Singleton<FixedMessages, ACE_Null_Mutex>::instance()->infomap_.insert(pair<std::string, FixedMessages::MSGInfo>("currhandler1", info1));
//	msgs.add_msg("currhandler1", ag, NETWORK_FIXED_MESSAGE, currhandler1);
//	p->start_new_curr_message(currhandler1);
//	*p << (INT32) -7;
//	*p << (INT32) -7;
//	*p << (INT32) -7;
//	p->end_new_curr_message();
//	//p->dumpMsgs();
//
//	/// second msg is variable msg
//	Message* currhandler2 = poolmsg->Ctor();
//	msgs.add_msg("currhandler2", ag_va, NETWORK_VARIABLE_MESSAGE, currhandler2);
//	p->start_new_curr_message(currhandler2);
//	*p << (UINT64) 2;
//	*p << (UINT64) 2;
//	*p << (UINT64) 2;
//	*p << (UINT64) 2;
//	p->end_new_curr_message();
//	//p->dumpMsgs();
//
//	///// second msg is variable msg
//	Message* currhandler3 = poolmsg->Ctor();
//	msgs.add_msg("currhandler3", ag, NETWORK_FIXED_MESSAGE, currhandler3);
//	p->start_new_curr_message(currhandler3);
//	*p << (INT32) -7;
//	*p << (INT32) -7;
//	*p << (INT32) -7;
//	p->end_new_curr_message();
//	//p->dumpMsgs();
//
//	///// second msg is variable msg
//	Message* currhandler4 = poolmsg->Ctor();
//	msgs.add_msg("currhandler4", ag_va, NETWORK_VARIABLE_MESSAGE, currhandler4);
//	p->start_new_curr_message(currhandler4);
//	*p << (UINT64) 2;
//	*p << (UINT64) 2;
//	*p << (UINT64) 2;
//	*p << (UINT64) 2;
//	p->end_new_curr_message();
//
//	Bundle::Packets::iterator iter = p->packets_.begin();
//	for( ; iter != p->packets_.end(); iter++ )
//	{
//		channel.recvPackets_[channel.recvPacketIndex_].push_back(( *iter ));
//	}
//	channel.process_packets(&msgs);
//
//	//PacketReader pr(&channel);
//	//pr.processMessages(&msgs, p->packets_);
//
//	pool->Dtor(p);
//
//}
TEST(PacketReaderTests, ctor_dtor_test)
{
	ACE_INET_Addr addr;
	addr.set(20001, ACE_LOCALHOST);
	ACE_SOCK_Connector logConnector;
	ACE_Time_Value timeout(10);
	ACE_SOCK_Stream log;
	if( logConnector.connect(log, addr, &timeout) == -1 )
	{
		if( ACE_OS::last_error() == ETIME )
		{
			ACE_DEBUG(( LM_DEBUG,
				ACE_TEXT("(%P|%t) Timeout while ")
				ACE_TEXT("connecting to log server\n") ));
		} else
		{
			ACE_ERROR(( LM_ERROR,
				ACE_TEXT("%p\n"),
				ACE_TEXT("log") ));
		}
	}
}