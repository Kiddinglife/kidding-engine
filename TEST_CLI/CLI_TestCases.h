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
#include "net\TestMsgs.h"

TEST(PacketReaderTests, ctor_dtor_test)
{

	//inport_msgs();

	//Bundle* p = Bundle_Pool->Ctor();
	//p->start_new_curr_message(g_msgs.find(1));
	//*p << (INT32) -7;
	//*p << (INT32) -7;
	//*p << (INT32) -7;
	//p->end_new_curr_message();

	///// second msg is variable msg
	//p->start_new_curr_message(g_msgs.find(2));
	//*p << (UINT64) 2;
	//*p << (UINT64) 2;
	//*p << (UINT64) 2;
	//*p << (UINT64) 2;
	//p->end_new_curr_message();

	/////// second msg is variable msg
	//p->start_new_curr_message(g_msgs.find(3));
	//*p << (INT32) -7;
	//*p << (INT32) -7;
	//*p << (INT32) -7;
	//p->end_new_curr_message();

	/////// second msg is variable msg
	//p->start_new_curr_message(g_msgs.find(4));
	//*p << (UINT64) 2;
	//*p << (UINT64) 2;
	//*p << (UINT64) 2;
	//*p << (UINT64) 2;
	//p->end_new_curr_message();

	//ACE_TEST_ASSERT(TESTMSG::pmsg1 != NULL);
	//Bundle* p = Bundle_Pool->Ctor();
	//p->start_new_curr_message(TESTMSG::pmsg1);
	//*p << (ACE_UINT64) 1;
	//p->end_new_curr_message();

	ACE_TEST_ASSERT(TESTMSG::pmsg1 != NULL);
	Bundle* p = Bundle_Pool->Ctor();
	p->start_new_curr_message(TESTMSG::pmsg1);
	*p << (ACE_UINT64) 1;
	p->end_new_curr_message();
	////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////
	g_channelExternalEncryptType = 0;
	ACE_INET_Addr addr;
	addr.set(20001, "192.168.2.47");
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
	ssize_t recv_cnt, send_cnt;
	Bundle::Packets::iterator iter = p->packets_.begin();
	for( ; iter != p->packets_.end(); iter++ )
	{
		ACE_HEX_DUMP(( LM_DEBUG, ( *iter )->buff->rd_ptr(), ( *iter )->length() ));
		send_cnt = log.send(( *iter )->buff->rd_ptr(), ( *iter )->length());
		if( send_cnt == -1 && ACE_OS::last_error() != EWOULDBLOCK )
			ACE_ERROR(( LM_ERROR,
			ACE_TEXT("(%P|%t) %p\n"),
			ACE_TEXT("send") ),
			0);
	}

	Bundle_Pool->Dtor(p);

	ACE_Time_Value wait(3);
	Packet* pReceiveWindow = pReceiveWindow = Packet_Pool->Ctor();
	size_t len = log.recv(pReceiveWindow->buff->wr_ptr(),
		pReceiveWindow->buff->size(), &wait);
	if( len > 0 )
	{
		pReceiveWindow->buff->wr_ptr(len);
		// 注意:必须在大于0的时候否则DEBUG_MSG将会导致WSAGetLastError返回0从而陷入死循环
		ACE_DEBUG(( LM_DEBUG,
			"%M::TCP_SOCK_Handler::process_recv(): datasize={%d}, wpos={%d}.\n",
			len, pReceiveWindow->buff->wr_ptr() ));
	}

	ACE_HEX_DUMP(( LM_DEBUG, pReceiveWindow->buff->rd_ptr(), pReceiveWindow->buff->length() ));

	Packet_Pool->Dtor(pReceiveWindow);
	log.close();
}