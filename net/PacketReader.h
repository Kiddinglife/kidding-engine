#ifndef PacketReader_H_
#define PacketReader_H_

#include "ace\pre.h"
#include "net\Packet.h"
#include "net\Message.h"
#include "net\Channel.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

enum FragmentType
{
	FRAGMENT_DATA_UNKNOW,
	FRAGMENT_DATA_MESSAGE_ID,
	FRAGMENT_DATA_MESSAGE_LENGTH,
	FRAGMENT_DATA_MESSAGE_LENGTH1,
	FRAGMENT_DATA_MESSAGE_BODY
};

struct PacketReader
{
	ACE_Byte*             pFragments; //pFragmentDatas_
	ACE_UINT32		    pFragmentsWpos_; //pFragmentDatasWpos_;
	ACE_UINT32			pFragmentsRemainning; //pFragmentDatasRemain_;
	FragmentType      fragmentsFlag_; //fragmentDatasFlag_

	Channel*			    pChannel_;
	Packet*				    pFragmentPacket_; //pFragmentStream_
	MessageID			currMsgID_;
	MessageLength1  currMsgLen_;

	PacketReader(Channel* pChannel = NULL);
	virtual ~PacketReader();
	void reset();
};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif