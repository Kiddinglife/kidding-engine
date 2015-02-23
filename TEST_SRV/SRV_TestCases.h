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

using namespace ACE_KBE_VERSIONED_NAMESPACE_NAME;
using namespace NETWORK;

TEST(PacketReaderTests, ctor_dtor_test)
{

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

	inport_msgs();
	//g_msgs.add_msg("currhandler1", new msgarg, NETWORK_FIXED_MESSAGE, new Message);
	//g_msgs.add_msg("currhandler2", new msgarg_variable, NETWORK_VARIABLE_MESSAGE, new Message);
	//g_msgs.add_msg("currhandler3", new msgarg, NETWORK_FIXED_MESSAGE, new Message);
	//g_msgs.add_msg("currhandler4", new msgarg_variable, NETWORK_VARIABLE_MESSAGE, new Message);

	pDispatcher.startLoop();
}