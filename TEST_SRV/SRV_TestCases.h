#include <iostream>
#include <time.h>
#include <string>
#include "gtest\gtest.h"
#include "ace/Log_Msg.h"
#include "ace/mytrace.h"
#include  "ace/Refcounted_Auto_Ptr.h"
#include "ace/Auto_Ptr.h"
#include "ace/ACE.h"
#include "common/ace_object_pool.h"
#include "net\Message.h"
#include "net\PacketReader.h"
#include "net\NetworkInterface.h"

TEST(PacketReaderTests, ctor_dtor_test)
{
	struct msgarg : public MessageArgs
	{
		virtual MessageLength1 args_bytes_count(void)
		{
			return 12;
		}
		virtual void fetch_args_from(Packet* p)
		{
			INT32  para1 = *(INT32*) p->buff->rd_ptr();
			p->buff->rd_ptr(4);
			INT32  para2 = *(INT32*) p->buff->rd_ptr();
			p->buff->rd_ptr(4);
			INT32  para3 = *(INT32*) p->buff->rd_ptr();
			ACE_DEBUG(( LM_DEBUG, "(%d)(%d)(%d)\n", para1, para2, para3 ));
		}

		virtual void add_args_to(Packet* p)
		{
		}
	};

	struct msgarg_variable : public MessageArgs
	{
		virtual MessageLength1 args_bytes_count(void)
		{
			return 0;
		}
		virtual void fetch_args_from(Packet* p)
		{
			for( int i = 0; i < 4; i++ )
			{
				INT64  para1 = *(INT64*) p->buff->rd_ptr();
				p->buff->rd_ptr(8);
				ACE_DEBUG(( LM_DEBUG, "(%d)", para1 ));
			}

			ACE_DEBUG(( LM_DEBUG, "\n" ));
		}

		virtual void add_args_to(Packet* p)
		{
		}
	};

	g_channelExternalEncryptType = 0;

	Nub              pDispatcher;

	ACE_INT32     extlisteningPort_min = 20001;
	ACE_INT32     extlisteningPort_max = 20005;
	const char *    extlisteningInterface = "127.0.0.1";
	//const char *    extlisteningInterface = "";
	//const char *    extlisteningInterface = "127.0.0.1";
	//const char *    extlisteningInterface = USE_KBEMACHINED;
	ACE_UINT32   extrbuffer = 0;
	ACE_UINT32   extwbuffer = 0;
	ACE_INT32      intlisteningPort = 20006;
	const char *    intlisteningInterface = "127.0.0.1";
	ACE_UINT32   intrbuffer = 0;
	ACE_UINT32   intwbuffer = 0;

	NetworkInterface in(&pDispatcher,
		extlisteningPort_min,
		extlisteningPort_max,
		extlisteningInterface,
		extrbuffer,
		extwbuffer,
		intlisteningPort,
		intlisteningInterface,
		intrbuffer,
		intwbuffer);

	pDispatcher.startLoop();
}