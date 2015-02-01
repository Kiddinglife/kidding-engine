#include "PacketReader.h"
#include "common\ace_object_pool.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

PacketReader::PacketReader(Channel* pChannel)
:
pFragments_(NULL), //pFragmentDatas_
pFragmentsWpos_(0), //pFragmentDatasWpos_;
pFragmentsRemainning(0), //pFragmentDatasRemain_;
fragmentsFlag_(FRAGMENT_DATA_UNKNOW), //fragmentDatasFlag_
pChannel_(pChannel),
pFragmentPacket_(NULL), //pFragmentStream_
currMsgID_(0),
currMsgLen_(0)
/*,in((char*) NULL, 0)*/
{
	TRACE("PacketReader::ctor()");
	TRACE_RETURN_VOID();
}


PacketReader::~PacketReader()
{
	TRACE("PacketReader::dtor()");
	reset();
	pChannel_ = NULL;
	TRACE_RETURN_VOID();
}

void PacketReader::reset()
{
	TRACE("PacketReader::reset()");

	fragmentsFlag_ = FRAGMENT_DATA_UNKNOW;
	currMsgLen_ = currMsgID_ = pFragmentsRemainning = pFragmentsWpos_ = 0;

	SAFE_RELEASE_ARRAY(pFragments_);

	if( pFragmentPacket_ )
	{
		ACE_PoolPtr_Getter(pool, Packet, ACE_Null_Mutex);
		pool->Dtor(pFragmentPacket_);
		pFragmentPacket_ = NULL;
	}

	TRACE_RETURN_VOID();
}

void PacketReader::processMessages(Messages* pMsgs, Packet* pPacket)
{
	TRACE("PacketReader::processMessages()");
	while( pPacket->length() > 0 || pFragmentPacket_ != NULL )
	{
		if( fragmentsFlag_ != FRAGMENT_DATA_UNKNOW )
		{

			ACE_DEBUG(( LM_DEBUG,
				"PacketReader::processMessages()::@1\n" ));

			/// when this message's fragment type is determined, go this branch
			mergeFragmentMessage(pPacket);

		} else
		{

			ACE_DEBUG(( LM_DEBUG,
				"PacketReader::processMessages()::@2\n" ));

			// firsly read currMsgID_ if it is 0
			if( currMsgID_ == 0 )
			{
				ACE_DEBUG(( LM_DEBUG, "PacketReader::processMessages()::@3\n" ));

				/**
				 * If the length of current pakcket is less than the size of msg id
				 * we have to cache it for the construction with the next packet
				 * we can stop the while loop at this point
				 */
				if( pPacket->length() < NETWORK_MESSAGE_ID_SIZE )
				{
					ACE_DEBUG(( LM_DEBUG, "PacketReader::processMessages()::@4\n" ));

					writeFragmentMessage(FRAGMENT_DATA_MESSAGE_ID, pPacket, NETWORK_MESSAGE_ID_SIZE);
					break;
				}

				/// read msg id from this packet to currMsgID_ and reset the msgID_
				ACE_InputCDR in(pPacket->buff);
				in >> currMsgID_;
				pPacket->buff->rd_ptr(in.rd_ptr());
				pPacket->msgID_ = currMsgID_;
				//const_cast<ACE_Message_Block*>( in.start() )->base(pPacket->buff->base(),
				//	pPacket->buff->size());
				//const_cast<ACE_Message_Block*>( in.start() )->rd_ptr(pPacket->buff->rd_ptr());
			}

			// find the msg based on currMsgID_
			Message* pMsg = pMsgs->find(currMsgID_);
			if( pMsg == NULL )
			{
				ACE_DEBUG(( LM_DEBUG, "PacketReader::processMessages()::@5::{pMsg == NULL}\n" ));
				Packet* pPacket1 = pFragmentPacket_ != NULL ? pFragmentPacket_ : pPacket;

			}
		}
	}
	TRACE_RETURN_VOID();
}

void PacketReader::writeFragmentMessage(FragmentType fragmentsFlag, Packet* pPacket,
	ACE_UINT32 datasize)
{
	TRACE("PacketReader::writeFragmentMessage()");

	ACE_ASSERT(pFragments_ == NULL);

	/// get the avaiable space in this packet [wpos_ - rpos_]
	size_t opsize = pPacket->length();

	/// the amounts of data left in the  buffer
	pFragmentsRemainning = datasize - opsize;

	/// assign @param dattasize bytes of space to the pFragments
	pFragments_ = new char[opsize + pFragmentsRemainning + 1];

	fragmentsFlag_ = fragmentsFlag;

	/// update the write position
	pFragmentsWpos_ = opsize;

	if( opsize > 0 )
	{
		/// copy the bytes in the packet into this buffer 
		memcpy(pFragments_, pPacket->buff->rd_ptr(), opsize);
		/// advance the rd position in this packet
		pPacket->on_read_packet_done();
	}

	TRACE_RETURN_VOID();
}

void PacketReader::mergeFragmentMessage(Packet* pPacket)
{
	TRACE("PacketReader::mergeFragmentMessage()");

	/// the payload size in this packet
	size_t opsize = pPacket->length();
	if( !opsize ) return;

	/// the left space in pFragments_ buffer <= the written bytes in the packet
	if( pPacket->length() >= pFragmentsRemainning )
	{
		/// first, fillout with the left space in the pFragments_ buffer 
		memcpy(pFragments_ + pFragmentsWpos_,
			pPacket->buff->rd_ptr(), pFragmentsRemainning);

		ACE_ASSERT(pFragmentPacket_ == NULL);

		///
		switch( fragmentsFlag_ )
		{
			case FRAGMENT_DATA_MESSAGE_ID:			// 消息ID信息不全
				memcpy(&currMsgID_, pFragments_, NETWORK_MESSAGE_ID_SIZE);
				break;

			case FRAGMENT_DATA_MESSAGE_LENGTH:		// 消息长度信息不全
				memcpy(&currMsgLen_, pFragments_, NETWORK_MESSAGE_LENGTH_SIZE);
				break;

			case FRAGMENT_DATA_MESSAGE_LENGTH1:		// 消息长度信息不全
				memcpy(&currMsgLen_, pFragments_, NETWORK_MESSAGE_LENGTH1_SIZE);
				break;

			case FRAGMENT_DATA_MESSAGE_BODY:		// 消息内容信息不全
				ACE_PoolPtr_Getter(pool, Packet, ACE_Null_Mutex);
				pFragmentPacket_ = pool->Ctor();
				pFragmentPacket_->os.write_char_array(pFragments_, currMsgLen_);
				//pFragmentStream_ = MemoryStream::ObjPool().createObject();
				//pFragmentStream_->append(pFragmentDatas_, currMsgLen_);
				break;
		}

		ACE_DEBUG(( LM_DEBUG,
			"PacketReader::mergeFragmentMessage({%s}): channel[{%d}], fragmentsFlag={%d},"
			"currMsgID={%d}, currMsgLen={%d}, completed!\n",
			pChannel_->c_str(),
			(void*) pChannel_,
			fragmentsFlag_,
			currMsgID_,
			currMsgLen_ ));

		fragmentsFlag_ = FRAGMENT_DATA_UNKNOW;
		pFragmentsRemainning = 0;
		SAFE_RELEASE_ARRAY(pFragments_);

	} else /// the left space in pFragments_ buffer > the written bytes in the packet
	{
		memcpy(pFragments_ + pFragmentsWpos_, pPacket->buff->rd_ptr(), opsize);
		pFragmentsRemainning -= opsize;
		pFragmentsWpos_ += opsize;
		pPacket->buff->rd_ptr(opsize);

		ACE_DEBUG(( LM_DEBUG,
			"PacketReader::mergeFragmentMessage({%s}): channel[{%d}], fragmentsFlag={%d},"
			"remainsize={%d}, currMsgID={%d}, currMsgLen={%d}\n",
			pChannel_->c_str(),
			(void*) pChannel_,
			fragmentsFlag_,
			pFragmentsRemainning,
			currMsgID_,
			currMsgLen_ ));
	}

	TRACE_RETURN_VOID();
}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL