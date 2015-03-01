#if defined(DEFINE_IN_INTERFACE)
#undef TESTMSG_H_
#endif

#ifndef TESTMSG_H_
#define TESTMSG_H_

#include "net\Message_Defs.h"
#include "net\Message.h"
#include "net\Bundle.h"


#ifndef DEFINE_IN_INTERFACE
#define MACHINE_MESSAGE_HANDLER_ARGS1(NAME, ARG_TYPE1, ARG_NAME1)\
	struct NAME##MachineMessagehandler1 : public Message \
	{\
		virtual void handle(Channel* pChannel,Packet* p) \
		{\
		ARG_TYPE1 ARG_NAME1;\
		ARG_NAME1 = *(ARG_TYPE1*) p->buff->rd_ptr(); \
       ACE_DEBUG((LM_DEBUG, "%M::#NAME##MachineMessagehandler1::INT1(%d)", ARG_NAME1)); \
        }\
	};
#else
#define MACHINE_MESSAGE_HANDLER_ARGS1(NAME, ARG_TYPE1, ARG_NAME1)
#endif																			

#define TEST_MSG_DECL_ARGS1(SRV,NAME,MSG_len,ARG_TYPE1, ARG_NAME1) \
MACHINE_MESSAGE_HANDLER_ARGS1(NAME,ARG_TYPE1,ARG_NAME1)\
NET_MSG_ARGS1_DECL(SRV,NAME, NAME##MachineMessagehandler1, MSG_len, ARG_TYPE1, ARG_NAME1)

MSGS_DECL_NAMESPACE_BEGIN(TESTMSG)
TEST_MSG_DECL_ARGS1(SRV, msg1, NETWORK_FIXED_MESSAGE, ACE_UINT64, INT1)
MSGS_DECL_NAMESPACE_END()

#endif