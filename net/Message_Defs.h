#ifndef Message_Defs_H_
#define Message_Defs_H_

#include "ace\pre.h"
#include "common\common.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

#define MESSAGES_DECL_BEGIN(INAME); \
namespace INAME{extern Network::MessageHandlers messageHandlers;			
#define MESSAGES_DECL_END(); }


#define MESSAGE_DEFI(DOMAIN, NAME, HANDLER_TYPE, MSG_LENGTH, ARG_N);           \
HANDLER_TYPE*  p##NAME=  static_cast<HANDLER_TYPE*>(messageHandlers.add_msg  \
(#DOMAIN"::"#NAME, new NAME##Args, MSG_LENGTH, new HANDLER_TYPE)); \
HANDLER_TYPE& NAME = *p##NAME;

#define MESSAGE_DECL(DOMAIN, NAME, HANDLER_TYPE, MSG_LENGTH, ARG_N);           \
extern HANDLER_TYPE*  p##NAME;                                                                                   \
extern HANDLER_TYPE& NAME;	


#define MESSAGE_EXPOSED_DECL(DOMAIN, NAME);
#define MESSAGE_EXPOSED_DEFI(DOMAIN, NAME);            \
bool p##DOMAIN##NAME##_exposed =                             \
messageHandlers.add_exposed_msg(#DOMAIN"::"#NAME);			


#define MESSAGE_ARGS_DELC(NAME);
#define MESSAGE_ARGS_DEFI(NAME);                                                                                           \
struct NAME##Args : public MessageArgs                                                                                      \
{                                                                                                                                                     \
	NAME##Args() : MessageArgs() { }                                                                                              \
	~NAME##Args() { }                                                                                                                     \
	virtual MessageLength1 args_bytes_count() { return NETWORK_VARIABLE_MESSAGE; }                \
	virtual MessageArgs::MESSAGE_ARGS_TYPE type() { return MESSAGE_ARGS_TYPE_VARIABLE; }    \
	virtual void fetch_args_from(Packet* p) { }                                                                                   \
	virtual void add_args_to(Packet* p) { }                                                                                         \
};


#define MESSAGE_AND_MESSAGE_ARGS_DELC(DOMAIN, NAME, HANDLER_TYPE, MSG_LENGTH)\




NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif