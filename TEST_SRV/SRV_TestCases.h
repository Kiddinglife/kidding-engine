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
#include "net\Message_Defs.h"
#include "net\TestMsgs.h"

TEST(PacketReaderTests, ctor_dtor_test)
{
	ACE_TEST_ASSERT(TESTMSG::pmsg1 != NULL);

	g_channelExternalEncryptType = 0;

	Nub              pDispatcher;

	ACE_INT32     extlisteningPort_min = 20001;
	ACE_INT32     extlisteningPort_max = 20005;
	const char *    extlisteningInterface = "192.168.2.47";
	ACE_UINT32   extrbuffer = 512 * 1024;
	ACE_UINT32   extwbuffer = 512 * 1024;
	ACE_INT32      intlisteningPort = 20006;
	const char *    intlisteningInterface = "192.168.2.47";
	ACE_UINT32   intrbuffer = 0;
	ACE_UINT32   intwbuffer = 0;

	NetworkInterface in(
		&pDispatcher,
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