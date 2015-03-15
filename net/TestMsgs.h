#if defined(DEFINE_IN_INTERFACE)
#undef TESTMSG_H_
#endif

#ifndef TESTMSG_H_
#define TESTMSG_H_

#include "net\Message_Defs.h"
#include "net\Message.h"
#include "net\Bundle.h"


#ifndef DEFINE_IN_INTERFACE
#define MACHINE_MESSAGE_HANDLER_ARGS2(NAME, ARG_TYPE1, ARG_NAME1, ARG_TYPE2, ARG_NAME2)\
	struct NAME##MachineMessagehandler2 : public Message \
	{\
		virtual void handle(Channel* pChannel,Packet* p) \
		{\
		ARG_TYPE1 ARG_NAME1;\
		ARG_NAME1 = *(ARG_TYPE1*) p->osbuff_->rd_ptr(); \
       ACE_DEBUG((LM_DEBUG, "%M::#NAME##MachineMessagehandler1::INT1(%d)", ARG_NAME1)); \
	   		ARG_TYPE2 ARG_NAME2;\
			p->osbuff_->rd_ptr(sizeof(ARG_NAME1));\
		ARG_NAME2 = *(ARG_TYPE2*) p->osbuff_->rd_ptr(); \
       ACE_DEBUG((LM_DEBUG, "%M::#NAME##MachineMessagehandler1::INT1(%d)", ARG_NAME2)); \
        }\
	};
#else
#define MACHINE_MESSAGE_HANDLER_ARGS2(NAME, ARG_TYPE1, ARG_NAME1,ARG_TYPE2, ARG_NAME2)
#endif																			

#define TEST_MSG_DECL_ARGS2(SRV,NAME,MSG_len, \
ARG_TYPE1, ARG_NAME1,ARG_TYPE2, ARG_NAME2) \
MACHINE_MESSAGE_HANDLER_ARGS2(NAME,ARG_TYPE1,\
ARG_NAME1,ARG_TYPE2, ARG_NAME2)\
NET_MSG_ARGS2_DECL(SRV,NAME, NAME##MachineMessagehandler2, MSG_len, ARG_TYPE1, ARG_NAME1,ARG_TYPE2, ARG_NAME2)

MSGS_DECL_NAMESPACE_BEGIN(TESTMSG)
TEST_MSG_DECL_ARGS2(SRV, msg1, NETWORK_FIXED_MESSAGE, ACE_UINT64, INT1, ACE_INT16, INT2)
MSGS_DECL_NAMESPACE_END()

#endif