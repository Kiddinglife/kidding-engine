#include "PacketReader.h"
#include "common\ace_object_pool.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

ACE_PoolPtr_Getter(pool, Packet, ACE_Null_Mutex);

PacketReader::PacketReader(Channel* pChannel) :
pFragmentsWpos_(NULL), //pFragmentDatasWpos_;
pFragmentsRemainning_(0), //pFragmentDatasRemain_;
fragmentsFlag_(FRAGMENT_DATA_UNKNOW), //fragmentDatasFlag_
pChannel_(pChannel),
pFragmentPacket_(pool->Ctor()),
pCurrPacket_(NULL),
pCurrMsg_(NULL),
currMsgID_(0),
currMsgLen_(0),
currMsgType_(NETWORK_FIXED_MESSAGE),
currMsgFieldLen_(0),
in_((char*) NULL, 0),
block_(const_cast<ACE_Message_Block*>( in_.start() ))
{
	TRACE("PacketReader::ctor()");
	TRACE_RETURN_VOID();
}


PacketReader::~PacketReader()
{
	TRACE("PacketReader::dtor()");
	reset();
	TRACE_RETURN_VOID();
}

void PacketReader::reset()
{
	TRACE("PacketReader::reset()");

	fragmentsFlag_ = FRAGMENT_DATA_UNKNOW;
	currMsgLen_ = currMsgID_ = pFragmentsRemainning_ = 0;
	pFragmentsWpos_ = NULL;

	if( pFragmentPacket_ )
	{
		ACE_PoolPtr_Getter(pool, Packet, ACE_Null_Mutex);
		pool->Dtor(pFragmentPacket_);
		pFragmentPacket_ = NULL;
	}

	pChannel_ = NULL;

	TRACE_RETURN_VOID();
}

void PacketReader::processMessages(Messages* pMsgs, Packet* pCurrPacket)
{
	TRACE("PacketReader::processMessages()");

	/// make this param globl in this class scope to improve the efficienc
	pCurrPacket_ = pCurrPacket;

	/// reset the related values in the block based on the current packet
	block_->base(pCurrPacket_->buff->base(), pCurrPacket_->buff->size());
	block_->wr_ptr(pCurrPacket_->buff->wr_ptr());
	block_->rd_ptr(pCurrPacket_->buff->rd_ptr());

	while( pCurrPacket_->length() || pFragmentPacket_->length() )
	{
		if( fragmentsFlag_ != FRAGMENT_DATA_UNKNOW )
		{
			/// when this message's fragment type is determined, go this branch
			mergeFragmentMessage();
		} else
		{
			//ACE_DEBUG(( LM_DEBUG, "%M::%T::@if(fragmentsFlag_ == FRAGMENT_DATA_UNKNOW )\n" ));

			/// start to reaqd msg id from the current packet
			if( currMsgID_ == 0 )
			{
				//ACE_DEBUG(( LM_DEBUG, "%M::%T::@if(currMsgID_ == 0)\n" ));

				/**
				 * If the length of current pakcket is less than the size of msg id
				 * we have to cache it for the construction with the next packet
				 * we can stop the while loop at this point
				 */
				if( pCurrPacket_->length() < NETWORK_MESSAGE_ID_SIZE )
				{
					//ACE_DEBUG(( LM_DEBUG, "%M::%T::@if(pCurrPacket_->length() < NETWORK_MESSAGE_ID_SIZE)" ));
					fragmentsFlag_ = FRAGMENT_DATA_MESSAGE_ID;
					currMsgFieldLen_ = NETWORK_MESSAGE_ID_SIZE;
					writeFragmentMessage();
					break;
				}

				/// read msg id from this packet to currMsgID_ and reset the msgID_
				in_ >> currMsgID_;
				pCurrPacket_->buff->rd_ptr(NETWORK_MESSAGE_ID_SIZE);
				pCurrPacket_->msgID_ = currMsgID_;

				//ACE_DEBUG(( LM_DEBUG,
				//	"%M::%T::PacketReader::processMessages()::currMsgID_(%d)"
				//	"pCurrPacket_->length(%d), pFragmentPacket_(%d),"
				//	"rd_pos(%d), wr_pos(%d)\n",
				//	currMsgID_, pCurrPacket_->length(), pFragmentPacket_,
				//	pCurrPacket_->buff->rd_ptr(), pCurrPacket_->buff->wr_ptr() ));
			}

			// find the msg based on currMsgID_
			pCurrMsg_ = pMsgs->find(currMsgID_);

			/// it must be something wrong if pCurrMsg_ == NULL. user will be told this situation
			if( pCurrMsg_ == NULL )
			{
				//ACE_DEBUG(( LM_DEBUG, "%M::%T::@if(pCurrMsg_ == NULL)\n" ));

				/**
				* change the read position to the begainning of the packet
				* for the convience of trace of this packet, when trace is done,
				* read position will be changed back to the original value.
				*/
				Packet* pPacket1 = !pFragmentPacket_->length() ? pFragmentPacket_ : pCurrPacket_;
				TRACE_MESSAGE_PACKET(true, pPacket1, pCurrMsg_, pPacket1->length(), pChannel_->c_str());

				char* rpos = pPacket1->buff->rd_ptr();
				pPacket1->buff->rd_ptr(pPacket1->buff->base());

				TRACE_MESSAGE_PACKET(true, pPacket1, pCurrMsg_, pPacket1->length(), pChannel_->c_str());

				pPacket1->buff->rd_ptr(rpos);

				//ACE_ERROR(( LM_ERROR,
				//	"%M::%T::PacketReader::processMessages::"
				//	"not found msg with ID(%d), msglen(%d), from(%s),"
				//	"set this channel to condem\n",
				//	currMsgID_, pPacket1->length(), pChannel_->c_str() ));

				/// reset related values
				currMsgID_ = 0;
				currMsgLen_ = 0;
				pChannel_->isCondemn_ = true;
				break;
			}

			/**
			 * get the message length. there are two branches
			 * @1::Extract the msg length from the packet if the msg is variable msg
			 * or g_packetAlwaysContainLength is setup
			 * @2::for fixed message, currMsgLen_ = pCurrMsg_->msgArgsBytesCount_
			 */
			if( !currMsgLen_ )
			{
				//ACE_DEBUG(( LM_DEBUG, "%M::%T::@if(!currMsgLen_)\n" ));

				if( pCurrMsg_->msgType_ == NETWORK_VARIABLE_MESSAGE || g_packetAlwaysContainLength )
				{
					//ACE_DEBUG(( LM_DEBUG, "%M::%T::::@if(variable msg)\n" ));

					/// if msg length field is incomplate, wait for the next packet 
					if( pCurrPacket_->length() < NETWORK_MESSAGE_LENGTH_SIZE )
					{
						//ACE_DEBUG(( LM_DEBUG, "%M::%T::msglen incomplate, wait next packet\n" ));
						fragmentsFlag_ = FRAGMENT_DATA_MESSAGE_LENGTH;
						currMsgFieldLen_ = NETWORK_MESSAGE_LENGTH_SIZE;
						writeFragmentMessage();
						break;
					}

					//ACE_DEBUG(( LM_DEBUG, "%M::%T::msglen complate, start to read msg len\n" ));
					/// read msg length from the packet
					in_ >> *(MessageLength*) &currMsgLen_;
					pCurrPacket_->buff->rd_ptr(NETWORK_MESSAGE_LENGTH_SIZE);

					//ACE_DEBUG(( LM_DEBUG, "%M::%T::variable msglen(%d)\n", currMsgLen_ ));

					/// update this msg's stats and call its callback method
					ACE_Singleton<NetStats, ACE_Null_Mutex>::instance()->
						trackMessage(NetStats::RECV, pCurrMsg_,
						currMsgLen_ + NETWORK_MESSAGE_ID_SIZE + NETWORK_MESSAGE_LENGTH_SIZE);

					// 如果长度占满说明使用了扩展长度，我们还需要等待扩展长度信息
					if( currMsgLen_ == NETWORK_MESSAGE_MAX_SIZE )
					{
						if( pCurrPacket_->length() < NETWORK_MESSAGE_LENGTH1_SIZE )
						{
							//ACE_DEBUG(( LM_DEBUG, "%M::%T::msglen1 incomplete, wait next packet\n" ));
							// 如果长度信息不完整，则等待下一个包处理
							fragmentsFlag_ = FRAGMENT_DATA_MESSAGE_LENGTH1;
							currMsgFieldLen_ = NETWORK_MESSAGE_LENGTH1_SIZE;
							writeFragmentMessage();
							break;
						}

						//ACE_DEBUG(( LM_DEBUG, "%M::%T::msglen1 complete, start to read msg len1\n" ));
						/// read msg length1 from the packet
						in_ >> currMsgLen_;
						pCurrPacket_->buff->rd_ptr(NETWORK_MESSAGE_LENGTH1_SIZE);
						//ACE_DEBUG(( LM_DEBUG, "%M::%T::variable msglen1(%d)\n", currMsgLen_ ));

						/// update this msg's stats and call its callback method
						ACE_Singleton<NetStats, ACE_Null_Mutex>::instance()->
							trackMessage(NetStats::RECV, pCurrMsg_,
							currMsgLen_ + NETWORK_MESSAGE_ID_SIZE + NETWORK_MESSAGE_LENGTH_SIZE);
					}
				} else /// NETWORK_FIXED_MESSAGE
				{
					//ACE_DEBUG(( LM_DEBUG, "%M::%T::@if(fixed msg)\n" ));

					currMsgLen_ = pCurrMsg_->msgArgsBytesCount_;

					//ACE_DEBUG(( LM_DEBUG, "%M::%T::fix mslen(%d)\n", currMsgLen_ ));

					/// 更新该消息stats并回调跟踪函数
					/// update this msg's stats and call its callback method
					ACE_Singleton<NetStats, ACE_Null_Mutex>::instance()->
						trackMessage(NetStats::RECV, pCurrMsg_, currMsgLen_);
				}
			}

			/// This situation is like an error where the client send the packet exceeds the max
			/// but we need remind the user this happened and will discard the rest of bytes in this packet
			if( pChannel_->channelScope_ == Channel::ChannelScope::EXTERNAL&&
				g_componentType != KBE_BOTS_TYPE &&
				g_componentType != CLIENT_TYPE &&
				currMsgLen_ > NETWORK_MESSAGE_MAX_SIZE )
			{
				//ACE_DEBUG(( LM_DEBUG, "%M::%T::@if(currMsgLen_ > NETWORK_MESSAGE_MAX_SIZE)\n", currMsgLen_ ));
				/**
				* change the read position to the begainning of the packet
				* for the convience of trace of this packet, when trace is done,
				* read position will be changed back to the original value.
				*/
				Packet* pPacket1 = !pFragmentPacket_->length() ? pFragmentPacket_ : pCurrPacket_;
				TRACE_MESSAGE_PACKET(true, pPacket1, pCurrMsg_, pPacket1->length(), pChannel_->c_str());

				char* rpos = pPacket1->buff->rd_ptr();
				pPacket1->buff->rd_ptr(pPacket1->buff->base());
				TRACE_MESSAGE_PACKET(true, pPacket1, pCurrMsg_, pPacket1->length(), pChannel_->c_str());

				pPacket1->buff->rd_ptr(rpos);

				//ACE_DEBUG(( LM_WARNING,
				//	"%M::%T::PacketReader::processMessages::"
				//	"msglen exceeds the limit with ID(%d), msg len(%d-%d), maxlen(%d) from(%s),"
				//	"set this channel to condem\n",
				//	currMsgID_, pPacket1->length(), currMsgLen_, NETWORK_MESSAGE_MAX_SIZE, pChannel_->c_str() ));

				currMsgLen_ = 0;
				pChannel_->isCondemn_ = true;
				break;
			}

			if( !pFragmentPacket_->length() )
			{
				if( pCurrPacket_->length() < currMsgLen_ )
				{
					//ACE_DEBUG(( LM_DEBUG, "%M::%T::@if( pCurrPacket_->length() < currMsgLen_)\n" ));
					fragmentsFlag_ = FRAGMENT_DATA_MESSAGE_BODY;
					currMsgFieldLen_ = currMsgLen_;
					writeFragmentMessage();
					break;
				} else
				{
					//ACE_DEBUG(( LM_DEBUG, "%M::%T::@if( pFragmentPacket_->length() > 0)\n" ));
					/// because there maybe more than one msg in this packet
					/// we need setup the wr and rd position to pick it out and then trace it
					curr_packet_end_pos_ = pCurrPacket_->buff->wr_ptr();
					curr_msg_end_pos_in_curr_packet = pCurrPacket_->buff->rd_ptr() + currMsgLen_;
					pCurrPacket_->buff->wr_ptr(curr_msg_end_pos_in_curr_packet);

					TRACE_MESSAGE_PACKET(true, pCurrPacket_, pCurrMsg_, currMsgLen_, pChannel_->c_str());

					pCurrMsg_->handle(pChannel_, pCurrPacket_);

					//ACE_DEBUG(( LM_DEBUG, "%M::%T::frpos(%d)\n", curr_msg_end_pos_in_curr_packet ));

					/// the remote function this message stands for could have no parameters
					/// if so, we do not need to check it. 
					if( currMsgLen_ > 0 )
					{
						///if handler does not process all the data in this packet
						if( curr_msg_end_pos_in_curr_packet != pCurrPacket_->buff->rd_ptr() )
						{
							ACE_DEBUG(( LM_ERROR,
								"PacketReader::processMessages(%s): rpos(%d) invalid, expect(%d). msgID(%d), msglen(%d).\n",
								pCurrMsg_->name_.c_str(),
								pCurrPacket_->buff->rd_ptr(), curr_msg_end_pos_in_curr_packet,
								currMsgID_, currMsgLen_ ));

							///if handler does not process all the data in this packet
							/// but we have to the rd pos into the expected position in order
							/// to avoid impact the next msg
							pCurrPacket_->buff->rd_ptr(curr_msg_end_pos_in_curr_packet);
						}
					}

					/// set the wr and rd positions back to the orifinal
					pCurrPacket_->buff->wr_ptr(curr_packet_end_pos_);
					block_->rd_ptr(currMsgLen_);
				}
			} else
			{
				TRACE_MESSAGE_PACKET(true, pFragmentPacket_, pCurrMsg_, currMsgLen_, pChannel_->c_str());
				pCurrMsg_->handle(pChannel_, pFragmentPacket_);
				pFragmentPacket_->buff->reset();
			}

			/// this message is processed completely at this point and so reset msgid and msglen to 0
			/// for the process of the next message
			currMsgID_ = 0;
			currMsgLen_ = 0;
		}
	}

	TRACE_RETURN_VOID();
}

void PacketReader::writeFragmentMessage()
{
	TRACE("PacketReader::writeFragmentMessage()");

	/// the current number of bytes in this packet
	opsize = in_.length();
	if( opsize > 0 )
	{
		switch( fragmentsFlag_ )
		{
			case FRAGMENT_DATA_MESSAGE_ID:			// 消息ID信息不全
				/// copy the existing bytes in the packet into pFragments_
				in_.read_char_array((char*) &currMsgID_, opsize);
				pCurrPacket_->buff->rd_ptr(opsize);
				pFragmentsWpos_ += opsize;

				break;

			case FRAGMENT_DATA_MESSAGE_LENGTH:		// 消息长度信息不全
				/// copy the existing bytes in the packet into pFragments_
				in_.read_char_array((char*) &currMsgLen_, opsize);
				pCurrPacket_->buff->rd_ptr(opsize);
				pFragmentsWpos_ += opsize;
				break;

			case FRAGMENT_DATA_MESSAGE_LENGTH1:		// 消息长度信息不全
				in_.read_char_array((char*) &currMsgLen_, opsize);
				pCurrPacket_->buff->rd_ptr(opsize);
				pFragmentsWpos_ += opsize;
				break;

			case FRAGMENT_DATA_MESSAGE_BODY:		// 消息内容信息不全
				/// copy the existing bytes in the packet into pFragments_
				in_.read_char_array(pFragmentPacket_->buff->wr_ptr(), opsize);
				/// update the write position
				pFragmentPacket_->buff->wr_ptr(opsize);
				/// the number of bytes needed in the furture
				pFragmentsRemainning_ = currMsgFieldLen_ - opsize;
				break;
		}
	}

	ACE_DEBUG(( LM_DEBUG,
		"PacketReader::writeFragmentMessage({%s}): channel(%d), fragmentDatasFlag={%d},"
		"remainsize={%d}, currMsgID={%d}, currMsgPayloadLen={%d}.\n",
		pChannel_->c_str(), pChannel_, fragmentsFlag_,
		pFragmentsRemainning_, currMsgID_, currMsgLen_ ));

	TRACE_RETURN_VOID();
}

void PacketReader::mergeFragmentMessage()
{
	TRACE("PacketReader::mergeFragmentMessage()");

	/// the payload size in this packet
	size_t opsize = in_.length();

	/// stop when this packet is empty
	if( !opsize ) return;

	/// the rest of data is fully received and so we just merge it 
	if( opsize >= pFragmentsRemainning_ )
	{
		if( fragmentsFlag_ == FRAGMENT_DATA_MESSAGE_BODY )
		{
			/// first, fillout with the left space in the pFragmentPacket_ buffer 
			in_.read_char_array(pFragmentPacket_->buff->wr_ptr(), pFragmentsRemainning_);
			pCurrPacket_->buff->rd_ptr(in_.rd_ptr());
			pFragmentPacket_->buff->wr_ptr(pFragmentsRemainning_);;
		} else
		{
			in_.read_char_array(pFragmentsWpos_, pFragmentsRemainning_);
			pCurrPacket_->buff->rd_ptr(pFragmentsRemainning_);
		}

		fragmentsFlag_ = FRAGMENT_DATA_UNKNOW;
		pFragmentsRemainning_ = 0;

	} else /// the left space in pFragments_ buffer > the written bytes in the packet
	{
		if( fragmentsFlag_ == FRAGMENT_DATA_MESSAGE_BODY )
		{
			/// copy the existing bytes in the packet into pFragments_
			in_.read_char_array(pFragmentPacket_->buff->wr_ptr(), opsize);
			/// update the write position
			pFragmentPacket_->buff->wr_ptr(opsize);
			/// update read position
			pCurrPacket_->buff->rd_ptr(in_.rd_ptr());
			/// update remainnings
			pFragmentsRemainning_ -= opsize;
		} else
		{
			/// copy the existing bytes in the packet into pFragments_
			in_.read_char_array(pFragmentsWpos_, opsize);
			pCurrPacket_->buff->rd_ptr(opsize);
			pFragmentsWpos_ += opsize;
		}

		ACE_DEBUG(( LM_DEBUG,
			"PacketReader::mergeFragmentMessage({%s}): channel[{%d}], fragmentsFlag={%d},"
			"remainsize={%d}, currMsgID={%d}, currMsgLen={%d}\n",
			pChannel_->c_str(),
			(void*) pChannel_,
			fragmentsFlag_,
			pFragmentsRemainning_,
			currMsgID_,
			currMsgLen_ ));
	}

	TRACE_RETURN_VOID();
}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL