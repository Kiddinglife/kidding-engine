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
	char*                    pFragments_; //pFragmentDatas_
	ACE_UINT32		    pFragmentsWpos_; //pFragmentDatasWpos_;
	ACE_UINT32			pFragmentsRemainning; //pFragmentDatasRemain_;
	FragmentType      fragmentsFlag_; //fragmentDatasFlag_

	Channel*			    pChannel_;
	Packet*				    pFragmentPacket_; //pFragmentStream_

	Message*             pCurrMsg_;
	MessageID			currMsgID_;
	MessageLength1  currMsgLen_;
	ACE_INT8             currMsgType_;

	ACE_InputCDR      in_;
	ACE_Message_Block* block_;

	char*                    packet_end_pos_;
	char*                    packet_payload_end_pos_;

	PacketReader(Channel* pChannel = NULL);
	virtual ~PacketReader();
	void reset();

	void writeFragmentMessage(FragmentType fragmentFlag, Packet* pPacket,
		ACE_UINT32 datasize);

	void mergeFragmentMessage(Packet* pPacket);

	void PacketReader::processMessages(Messages* pMsgHandlers, Packet* pPacket);
};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif