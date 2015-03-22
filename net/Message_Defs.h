#if defined(DEFINE_IN_INTERFACE)
#undef Message_Defs_H_
#endif

#ifndef Message_Defs_H_
#define Message_Defs_H_

#include "ace\pre.h"
#include "common\common.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

///////////////////////////////// MSGS_DECL_NAMESPACE_BEGIN ////////////////////////////////
#ifndef DEFINE_IN_INTERFACE
#define MSGS_DECL_NAMESPACE_BEGIN(INAME) \
namespace INAME{extern Messages messageHandlers;			
#else
#define MSGS_DECL_NAMESPACE_BEGIN(INAME) \
namespace INAME{Messages messageHandlers;			
#endif

#define MSGS_DECL_NAMESPACE_END() }
/////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////// MSG_TYPE ////////////////////////////////////////////
#ifndef DEFINE_IN_INTERFACE
#define ADD_MSG(DOMAINNAME, NAME, MSG_TYPE, MSG_LENGTH, ARG_N)           \
extern MSG_TYPE*  p##NAME;                                                                                   \
extern MSG_TYPE& NAME;	
#else
#define ADD_MSG(DOMAINNAME, NAME, MSG_TYPE, MSG_LENGTH, ARG_N)           \
MSG_TYPE*  p##NAME=  static_cast<MSG_TYPE*>(messageHandlers.add_msg  \
(#DOMAINNAME"::"#NAME, new NAME##Args##ARG_N, MSG_LENGTH, new MSG_TYPE)); \
MSG_TYPE& NAME = *p##NAME;
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////// MSG_EXPOSED_DECL //////////////////////////////////////
#ifndef DEFINE_IN_INTERFACE
#define MSG_EXPOSED_DECL(DOMAINNAME, NAME)
#else
#define MSG_EXPOSED_DEFI(DOMAINNAME, NAME)            \
bool p##DOMAINNAME##NAME##_exposed =                             \
messageHandlers.add_exposed_msg(#DOMAINNAME"::"#NAME);		
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////// MSG_ARGS_STREAM //////////////////////////////////////////
#ifdef DEFINE_IN_INTERFACE
#define MSG_ARGS_STREAM(NAME)
#else
#define MSG_ARGS_STREAM(NAME) \
struct NAME##Args_Stream : public MessageArgs                                                                           \
{                                                                                                                                                     \
	NAME##Args_Stream() : MessageArgs() { }                                                                                  \
	~NAME##Args_Stream() { }                                                                                                         \
	virtual MessageLength1 args_bytes_count() { return NETWORK_VARIABLE_MESSAGE; }                \
	virtual MessageArgs::MESSAGE_ARGS_TYPE type() { return MESSAGE_ARGS_TYPE_VARIABLE; }    \
	virtual void fetch_args_from(Packet* p) { }                                                                                   \
	virtual void add_args_to(Packet* p) { }                                                                                         \
};
#endif
#define NET_MSG_STREAM_DELC(DOMAINNAME, NAME, MSG_TYPE, MSG_LENGTH)\
ADD_MSG(DOMAINNAME, NAME, MSG_TYPE, MSG_LENGTH, _Stream)  \
MSG_ARGS_STREAM(NAME)
////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////// 0 Args Msg  ////////////////////////////////////////////////
#ifdef DEFINE_IN_INTERFACE
#define MSG_ARGS0(NAME)
#else
#define MSG_ARGS0(NAME)									                                                            \
struct  NAME##Args0 : public MessageArgs					                                                    \
{																                                                                        \
	NAME##Args0():MessageArgs(){}						                                                            \
	~NAME##Args0(){}											                                                            \
	static void staticAddToBundle(Bundle& s){ }													                \
	static void staticAddToStream(Packet& s){ } 	                                                                \
	virtual MessageLength1 args_bytes_count() { return 0;}                                                  \
	virtual void fetch_args_from(Packet* p) { }                                                                      \
	virtual void add_args_to(Packet* p) { }     														                \
};																
#endif
#define NET_MSG_ARGS0_DECL(DOMAINNAME, NAME, MSG_TYPE,MSG_LENGTH)\
ADD_MSG(DOMAINNAME, NAME, MSG_TYPE, MSG_LENGTH, 0) \
MSG_ARGS0(NAME)
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////// 1 Args Msg  ////////////////////////////////////////////////
#ifdef DEFINE_IN_INTERFACE
#define MSG_ARGS1(NAME, ARG_TYPE1, ARG_NAME1)
#else
#define MSG_ARGS1(NAME, ARG_TYPE1, ARG_NAME1)                                                    \
struct  NAME##Args1 : public MessageArgs                                                                      \
{                                                                                                                                       \
	ARG_TYPE1 ARG_NAME1;                                                                                             \
	NAME##Args1() :MessageArgs() { strArgsTypes.push_back(#ARG_TYPE1); }                    \
	NAME##Args1(ARG_TYPE1 init_##ARG_NAME1) :                                                         \
		MessageArgs(),                                                                                                        \
		ARG_NAME1(init_##ARG_NAME1)                                                                            \
	{                                                                                                                                  \
		strArgsTypes.push_back(#ARG_TYPE1);                                                                     \
	}                                                                                                                                  \
	~NAME##Args1() { }                                                                                                    \
	static void staticAddToBundle(Bundle& s, ARG_TYPE1 init_##ARG_NAME1)                  \
	{                                                                                                                                 \
		s << init_##ARG_NAME1;                                                                                       \
	}                                                                                                                                 \
	static void staticAddToStream(Packet& s, ARG_TYPE1 init_##ARG_NAME1)                 \
	{                                                                                                                                 \
		s.os << init_##ARG_NAME1;                                                                                   \
	}                                                                                                                                \
	virtual MessageLength1 args_bytes_count() { return sizeof(ARG_TYPE1); }                    \
	virtual void fetch_args_from(Packet* p)                                                                      \
	{ ARG_NAME1 = *(ARG_TYPE1*) p->osbuff_->rd_ptr(); }                                                  \
	virtual void add_args_to(Packet* p) { p->os << ARG_NAME1; }                                   \
};
#endif
#define NET_MSG_ARGS1_DECL(DOMAINNAME,NAME,MSG_TYPE,MSG_LENGTH,ARG_TYPE1,ARG_NAME1) \
ADD_MSG(DOMAINNAME, NAME, MSG_TYPE, MSG_LENGTH, 1)  \
MSG_ARGS1(NAME,ARG_TYPE1,ARG_NAME1)	 
///////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////// MSG_ARGS2 ///////////////////////////////////////////////
#ifdef DEFINE_IN_INTERFACE
#define MSG_ARGS2(NAME,ARG_TYPE1, ARG_NAME1, ARG_TYPE2)	                                 			    
#else
#define MSG_ARGS2(NAME,ARG_TYPE1, ARG_NAME1, ARG_TYPE2)                                   \
struct  NAME##Args2 : public MessageArgs					                                                     \
{																                                                                         \
	ARG_TYPE1 ARG_NAME1;										                                                         \
	ARG_TYPE2 ARG_NAME2;				                                                                                 \
	NAME##Args2():MessageArgs()	                                                                                     \
		{                                                                                                                                \
	  strArgsTypes.push_back(#ARG_TYPE1);                                                                         \
	  strArgsTypes.push_back(#ARG_TYPE2);                                                                         \
	    }                                                                                                                                \
	NAME##Args2(ARG_TYPE1 init_##ARG_NAME1, ARG_TYPE2 init_##ARG_NAME2):         \
	MessageArgs(),										                                                                         \
	ARG_NAME1(init_##ARG_NAME1),									                                             \
	ARG_NAME2(init_##ARG_NAME2)									                                                 \
		{                                                                                                                                \
	  strArgsTypes.push_back(#ARG_TYPE1);                                                                         \
	  strArgsTypes.push_back(#ARG_TYPE2);                                                                         \
	    }                                                                                                                                \
	~NAME##Args2(){}											                                                             \
	static void staticAddToBundle(Bundle& s, ARG_TYPE1 init_##ARG_NAME1,                    \
	ARG_TYPE2 init_##ARG_NAME2)                                                                                    \
	    {                                                                                                                                \
	   s << init_##ARG_NAME1;									                                                         \
	   s << init_##ARG_NAME2;									                                                         \
	    }                                                                                                                                \
	static void staticAddToStream(Packet& s, ARG_TYPE1 init_##ARG_NAME1,	                 \
    ARG_TYPE2 init_##ARG_NAME2)                                                                                    \
		{                                                                                                                                \
	   s.os <<  init_##ARG_NAME1;                                                                                      \
	   s.os <<  init_##ARG_NAME2;                                                                                      \
		}                                                                                                                                \
	virtual MessageLength1 args_bytes_count()                                                                    \
		{                                                                                                                                \
      return sizeof(ARG_TYPE1)+ sizeof(ARG_TYPE2);                                                            \
	    }                                                                                                                                \
	virtual void fetch_args_from(Packet* p)                                                                          \
		{                                                                                                                                \
      ARG_NAME1 = *(ARG_TYPE1*) p->osbuff_->rd_ptr();                                                        \
	  p->osbuff_->rd_ptr(sizeof(ARG_NAME1));                                                                         \
	  ARG_NAME2 = *(ARG_TYPE2*) p->osbuff_->rd_ptr();                                                        \
	    }                                                                                                                                \
	virtual void add_args_to(Packet* p)                                                                                \
	    {                                                                                                                                \
    p->os << ARG_NAME1;                                                                                                \
	p->os << ARG_NAME2;                                                                                                \
	    }                                                                                                                               \
};

#define NET_MSG_ARGS2_DECL(DOMAINNAME,NAME,MSG_TYPE,MSG_LENGTH,ARG_TYPE1,ARG_NAME1,ARG_TYPE2,ARG_NAME2) \
ADD_MSG(DOMAINNAME, NAME, MSG_TYPE, MSG_LENGTH, 2)  \
MSG_ARGS2(NAME,ARG_TYPE1,ARG_NAME1,ARG_TYPE2,ARG_NAME2)	 
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////// MSG_ARGS3 ////////////////////////////////////////
#ifdef DEFINE_IN_INTERFACE
#define MSG_ARGS3(NAME,ARG_TYPE1, ARG_NAME1, ARG_TYPE2, ARG_NAME2,ARG_TYPE3, ARG_NAME3)			    
#else
#define MSG_ARGS3(NAME,ARG_TYPE1, ARG_NAME1, ARG_TYPE2, ARG_NAME2,ARG_TYPE3, ARG_NAME3)\
struct  NAME##Args3 : public MessageArgs					                                                     \
{																                                                                         \
	ARG_TYPE1 ARG_NAME1;										                                                         \
	ARG_TYPE2 ARG_NAME2;				                                                                                 \
	ARG_TYPE3 ARG_NAME3;					                                                                             \
	NAME##Args3():MessageArgs()	                                                                                     \
		{                                                                                                                                \
	  strArgsTypes.push_back(#ARG_TYPE1);                                                                         \
	  strArgsTypes.push_back(#ARG_TYPE2);                                                                         \
	  strArgsTypes.push_back(#ARG_TYPE3);                                                                        \
	    }                                                                                                                                \
	NAME##Args3(ARG_TYPE1 init_##ARG_NAME1, ARG_TYPE2 init_##ARG_NAME2,           \
    ARG_TYPE3 init_##ARG_NAME3):                                                                                   \
	MessageArgs(),										                                                                         \
	ARG_NAME1(init_##ARG_NAME1),									                                             \
	ARG_NAME2(init_##ARG_NAME2),									                                             \
	ARG_NAME3(init_##ARG_NAME3)									                                                 \
		{                                                                                                                                \
	  strArgsTypes.push_back(#ARG_TYPE1);                                                                         \
	  strArgsTypes.push_back(#ARG_TYPE2);                                                                         \
	  strArgsTypes.push_back(#ARG_TYPE3);                                                                         \
	    }                                                                                                                                \
	~NAME##Args3(){}											                                                             \
	static void staticAddToBundle(Bundle& s, ARG_TYPE1 init_##ARG_NAME1,                    \
	ARG_TYPE2 init_##ARG_NAME2,ARG_TYPE3 init_##ARG_NAME3)                                   \
	    {                                                                                                                                \
	   s << init_##ARG_NAME1;									                                                         \
	   s << init_##ARG_NAME2;									                                                         \
	   s << init_##ARG_NAME3;									                                                         \
	    }                                                                                                                                \
	static void staticAddToStream(Packet& s, ARG_TYPE1 init_##ARG_NAME1,	                 \
    ARG_TYPE2 init_##ARG_NAME2,ARG_TYPE3 init_##ARG_NAME3)                                   \
		{                                                                                                                                \
	   s.os <<  init_##ARG_NAME1;                                                                                      \
	   s.os <<  init_##ARG_NAME2;                                                                                      \
	   s.os <<  init_##ARG_NAME3;                                                                                      \
		}                                                                                                                                \
	virtual MessageLength1 args_bytes_count()                                                                    \
		{                                                                                                                                \
      return sizeof(ARG_TYPE1)+ sizeof(ARG_TYPE2)+                                                          \
      sizeof(ARG_TYPE3);                                                                                                      \
	    }                                                                                                                                \
	virtual void fetch_args_from(Packet* p)                                                                          \
		{                                                                                                                                \
      ARG_NAME1 = *(ARG_TYPE1*) p->osbuff_->rd_ptr();                                                        \
	  p->osbuff_->rd_ptr(sizeof(ARG_NAME1));                                                                        \
	  ARG_NAME2 = *(ARG_TYPE2*) p->osbuff_->rd_ptr();                                                       \
	  p->osbuff_->rd_ptr(sizeof(ARG_NAME2));                                                                        \
	  ARG_NAME3 = *(ARG_TYPE3*) p->osbuff_->rd_ptr();                                                       \
	    }                                                                                                                               \
	virtual void add_args_to(Packet* p)                                                                               \
	    {                                                                                                                               \
    p->os << ARG_NAME1;                                                                                               \
	p->os << ARG_NAME2;                                                                                               \
	p->os << ARG_NAME3;                                                                                               \
	    }                                                                                                                              \
};
#endif

#define NET_MSG_ARGS3_DECL(DOMAINNAME,NAME,MSG_TYPE,MSG_LENGTH,ARG_TYPE1,ARG_NAME1,ARG_TYPE2,ARG_NAME2,ARG_TYPE3,ARG_NAME3) \
ADD_MSG(DOMAINNAME, NAME, MSG_TYPE, MSG_LENGTH, 3) \
MSG_ARGS3(NAME,ARG_TYPE1, ARG_NAME1, ARG_TYPE2, ARG_NAME2,ARG_TYPE3, ARG_NAME3)
///////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////// MSG_ARGS4 ///////////////////////////////////////////////
#ifdef DEFINE_IN_INTERFACE
#define MSG_ARGS4(NAME,ARG_TYPE1, ARG_NAME1, ARG_TYPE2, ARG_NAME2,ARG_TYPE3, ARG_NAME3,ARG_TYPE4, ARG_NAME4)	\			    
#else
#define MSG_ARGS4(NAME,ARG_TYPE1, ARG_NAME1, ARG_TYPE2, ARG_NAME2,ARG_TYPE3, ARG_NAME3,ARG_TYPE4, ARG_NAME4)\
struct  NAME##Args4 : public MessageArgs					                                                     \
{																                                                                         \
	ARG_TYPE1 ARG_NAME1;										                                                         \
	ARG_TYPE2 ARG_NAME2;				                                                                                 \
	ARG_TYPE3 ARG_NAME3;					                                                                             \
	ARG_TYPE4 ARG_NAME4;				                                                                                 \
	NAME##Args4():MessageArgs()	                                                                                     \
	{                                                                                                                                    \
	  strArgsTypes.push_back(#ARG_TYPE1);                                                                         \
	  strArgsTypes.push_back(#ARG_TYPE2);                                                                         \
	  strArgsTypes.push_back(#ARG_TYPE3);                                                                         \
	  strArgsTypes.push_back(#ARG_TYPE4);                                                                         \
    }                                                                                                                                    \
	NAME##Args4(ARG_TYPE1 init_##ARG_NAME1, ARG_TYPE2 init_##ARG_NAME2,           \
    ARG_TYPE3 init_##ARG_NAME3, ARG_TYPE4 init_##ARG_NAME4):                                 \
	MessageArgs(),										                                                                         \
	ARG_NAME1(init_##ARG_NAME1),									                                             \
	ARG_NAME2(init_##ARG_NAME2),									                                             \
	ARG_NAME3(init_##ARG_NAME3),									                                             \
	ARG_NAME4(init_##ARG_NAME4)									                                                 \
	{                                                                                                                                    \
	  strArgsTypes.push_back(#ARG_TYPE1);                                                                         \
	  strArgsTypes.push_back(#ARG_TYPE2);                                                                         \
	  strArgsTypes.push_back(#ARG_TYPE3);                                                                         \
	  strArgsTypes.push_back(#ARG_TYPE4);                                                                         \
    }                                                                                                                                    \
	~NAME##Args4(){}											                                                             \
	static void staticAddToBundle(Bundle& s, ARG_TYPE1 init_##ARG_NAME1,                    \
	ARG_TYPE2 init_##ARG_NAME2,ARG_TYPE3 init_##ARG_NAME3,                                   \
    ARG_TYPE4 init_##ARG_NAME4)                                                                                    \
    {                                                                                                                                    \
	   s << init_##ARG_NAME1;									                                                         \
	   s << init_##ARG_NAME2;									                                                         \
	   s << init_##ARG_NAME3;									                                                         \
	   s << init_##ARG_NAME4;									                                                         \
    }                                                                                                                                    \
	static void staticAddToStream(Packet& s, ARG_TYPE1 init_##ARG_NAME1,	                 \
    ARG_TYPE2 init_##ARG_NAME2,ARG_TYPE3 init_##ARG_NAME3,                                   \
    ARG_TYPE4 init_##ARG_NAME4)                                                                                    \
	{                                                                                                                                    \
	   s.os <<  init_##ARG_NAME1;                                                                                      \
	   s.os <<  init_##ARG_NAME2;                                                                                      \
	   s.os <<  init_##ARG_NAME3;                                                                                      \
	   s.os <<  init_##ARG_NAME4;                                                                                      \
	}                                                                                                                                    \
	virtual MessageLength1 args_bytes_count()                                                                    \
	{                                                                                                                                    \
      return sizeof(ARG_TYPE1)+ sizeof(ARG_TYPE2)                                                             \
      sizeof(ARG_TYPE4)+ sizeof(ARG_TYPE4);                                                                      \
    }                                                                                                                                    \
	virtual void fetch_args_from(Packet* p)                                                                          \
	{                                                                                                                                    \
      ARG_NAME1 = *(ARG_TYPE1*) p->osbuff_->rd_ptr();                                                        \
	  p->osbuff_->rd_ptr(sizeof(ARG_NAME1));                                                                         \
	  ARG_NAME2 = *(ARG_TYPE2*) p->osbuff_->rd_ptr();                                                        \
	  p->osbuff_->rd_ptr(sizeof(ARG_NAME2));                                                                         \
	  ARG_NAME3 = *(ARG_TYPE3*) p->osbuff_->rd_ptr();                                                        \
	  p->osbuff_->rd_ptr(sizeof(ARG_NAME3));                                                                         \
	  ARG_NAME4 = *(ARG_TYPE4*) p->osbuff_->rd_ptr();                                                        \
    }                                                                                                                                    \
	virtual void add_args_to(Packet* p)                                                                                \
    {                                                                                                                                    \
    p->os << ARG_NAME1;                                                                                                \
	p->os << ARG_NAME2;                                                                                                \
	p->os << ARG_NAME3;                                                                                                \
	p->os << ARG_NAME4;                                                                                                \
    }                                                                                                                                   \
};
#endif

#define NET_MSG_ARGS4_DECL(DOMAINNAME,NAME,MSG_TYPE,MSG_LENGTH, \
ARG_TYPE1,ARG_NAME1,\
ARG_TYPE2,ARG_NAME2,\
ARG_TYPE3,ARG_NAME3,\
ARG_TYPE4,ARG_NAME4) \
ADD_MSG(DOMAINNAME, NAME, MSG_TYPE, MSG_LENGTH, 4)  \
MSG_ARGS4(NAME,ARG_TYPE1,ARG_NAME1,ARG_TYPE2,ARG_NAME2,ARG_TYPE3,ARG_NAME3,ARG_TYPE4,ARG_NAME4)	
///////////////////////////////////////////////////////////////////////////////////////////////////
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif