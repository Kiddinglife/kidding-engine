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
	//char*                    pFragments_; //pFragmentDatas_
	char*		            pFragmentsWpos_; //pFragmentDatasWpos_;
	size_t			        pFragmentsRemainning_; //pFragmentDatasRemain_;
	FragmentType      fragmentsFlag_; //fragmentDatasFlag_

	Channel*			    pChannel_;
	Packet*	                pFragmentPacket_; //pFragmentStream_
	static Packet*	    pSecomdaryFragmentPacket_; //pFragmentStream_
	Packet*                pCurrPacket_;
	Message*             pCurrMsg_;
	MessageID			currMsgID_;
	MessageLength1  currMsgLen_;
	ACE_INT8             currMsgType_;
	MessageLength1  currMsgFieldLen_;

	ACE_InputCDR      in_;
	ACE_Message_Block* block_;

	char*                    curr_packet_end_pos_;
	char*                    curr_msg_end_pos_in_curr_packet;

	PacketReader(Channel* pChannel = NULL);
	virtual ~PacketReader();
	void reset();

	void writeFragmentMessage(void);

	void mergeFragmentMessage(void);

	void PacketReader::processMessages(Messages* pMsgHandlers, Packet* pPacket);
};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif