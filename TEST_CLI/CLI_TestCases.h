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
	ACE_TEST_ASSERT(TESTMSG::pmsg1 != NULL);
	Bundle* p = Bundle_Pool->Ctor();
	p->start_new_curr_message(TESTMSG::pmsg1);
	*p << (ACE_UINT64) 123456789 << (ACE_INT16) 1;
	p->end_new_curr_message();

	////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////

	g_channelExternalEncryptType = 0;
	ACE_INET_Addr addr;
	addr.set(20001, "192.168.2.47");
	ACE_SOCK_Connector logConnector;
	ACE_Time_Value timeout(30);
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
	setnonblocking(true, log);

	ssize_t recv_cnt, send_cnt;
	Bundle::Packets::iterator iter = p->packets_.begin();
	for( ; iter != p->packets_.end(); iter++ )
	{
		ACE_HEX_DUMP(( LM_DEBUG, ( *iter )->osbuff_->rd_ptr(), ( *iter )->length() ));
		send_cnt = log.send(( *iter )->osbuff_->rd_ptr(), ( *iter )->length());
		if( send_cnt == -1 && ACE_OS::last_error() != EWOULDBLOCK )
			ACE_ERROR(( LM_ERROR,
			ACE_TEXT("(%P|%t) %p\n"),
			ACE_TEXT("send") ),
			0);
	}

	Sleep(1000);

	Bundle_Pool->Dtor(p);
	Packet* pReceiveWindow = Packet_Pool->Ctor();
	int i = 1;
	while( i > 0 )
	{
		i--;
		size_t len = log.recv(pReceiveWindow->osbuff_->wr_ptr(), pReceiveWindow->osbuff_->size());
		if( len == pReceiveWindow->osbuff_->size() )
		{
			pReceiveWindow->osbuff_->wr_ptr(len);
			ACE_HEX_DUMP(( LM_DEBUG, pReceiveWindow->osbuff_->rd_ptr(), pReceiveWindow->osbuff_->length() ));
			pReceiveWindow->osbuff_->reset();
		} else if( len > 0 )
		{
			pReceiveWindow->osbuff_->wr_ptr(len);
			ACE_HEX_DUMP(( LM_DEBUG, pReceiveWindow->osbuff_->rd_ptr(), pReceiveWindow->osbuff_->length() ));
		}
	}
	Packet_Pool->Dtor(pReceiveWindow);
	Sleep(15000);
	log.close();
}