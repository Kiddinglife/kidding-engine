#include "Bundle.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

/**

* @para calpCurrPacket_
* 如果包含当前包的长度，则为真，否则为假
* true if calculating pCurrPacket_'s length, false if not.
* \n
* @retvalue size_t
* 返回值为该bundle中的所有包的有效长度(即msg所占的空间大小)
* return the all packets length
*/
size_t Bundle::get_packets_length()
{
	size_t len = 0;
	Packets::iterator iter = packets_.begin();
	for( ; iter != packets_.end(); iter++ )
	{
		len += ( *iter )->length();
	}

	return len;
}

/**
* 该方法将会回收该bundle中所有的包到对应的对象池中，
* 并重新初始化各个成员的值。
* this mothod recycles all the packets in the container back to the right pool
* it also initialize all the memeber values.
*/
void Bundle::clear()
{
	///清空当前packet
	///clear the current packet
	if( pCurrPacket_ != NULL )
	{
		pCurrPacket_ = NULL;
	}

	///遍历所有的packet并将其回收至对应的内存池
	///loop all packets and recycle it the right pool
	Packets::iterator iter = packets_.begin();
	for( ; iter != packets_.end(); iter++ )
	{
		ACE_PoolPtr_Getter(ObjPool, Packet, ACE_Null_Mutex);
		ObjPool->Dtor(*iter);
	}

	///清空元素但不回收空间，提高效率
	///clear the packet container without recycling the allocated memory
	packets_.clear();

	///初始化成员初值
	reuse_ = false;
	pChnnel_ = NULL;
	numMessages_ = 0;
	currMsgID_ = 0;
	currMsgPacketCount_ = 0;
	currMsgLength_ = 0;
	currMsgLengthPos_ = 0;
	currMsgType_ = 0;
	pCurrMsg_ = NULL;
}

/**
* @Brief
* 该方法调整当前包的空间以便装入（@para addsize）大小的消息。
* 一个消息如果很大的话，可能需要多个包来装载
* This mothod is is to ajust the space in the current packet when adding @para
* addsize msg. A big msg may be divided into more than one packet.
* \n
* @para size_t addsize
* 写入包中的消息大小， 不能大于包的最大容量
* the size of msg that will be added to the current packet
* \n
* @para bool inseparable
* 若为真，该方法会检测当前包可用空间的大小是否足以装下当前消息。
* 如果当前包不能够装下的的话，该方法会创建一个新的包来装下整条消息。
* 这时，老包的剩余空间将为空。
* if inseparable is true, it tests if  the rest of space in the current packet can hold the
* addsize of msg, if space is not enough, it will create a new packet to hold whole msg,
* the old space will leave empty.
* \n
* @retvalue size_t
* 返回值为实际所能够写入的有效大小，
* 如果 @para inseparable == false, 可能会 <= @para addsize 的大小
* the actually-writable space size in current packet will be returned that may be <= addsize
*
* @ChangeLog
* 11: 26 AM, 12/01/2015 ::
* Change "on_packet_append" to "calculate_avaiable_space_of_curr_packet",
* this name can clearly show  the aim of this function.
*/
size_t Bundle::calculate_avaiable_space_of_curr_packet(size_t addsize, bool inseparable)
{
	//ACE_DEBUG(( LM_DEBUG,
	//	"calculate_avaiable_space_of_curr_packet(size_t addsize=%d, bool inseparable=%d)\n",
	//	addsize, inseparable ));

	static char* fwpos = NULL;
	static size_t space = 0;
	static size_t remainsize = 0;
	static size_t taddsize = 0;
	fwpos = NULL;
	space = remainsize = taddsize = 0;

	//获得当前packet已用空间大小
	space = pCurrPacket_->length();
	//ACE_DEBUG(( LM_DEBUG, "calculate_avaiable_space_of_curr_packet:: used space=%d \n", space ));

	//获取当前packet的写位置
	fwpos = pCurrPacket_->buff->wr_ptr();
	//ACE_DEBUG(( LM_DEBUG, "calculate_avaiable_space_of_curr_packet:: fwpos=%d \n", fwpos ));

	//ACE_DEBUG(( LM_DEBUG, "///检测当前包剩余空间+addsize的数据后是否溢出 \n" ));
	///检测当前包剩余空间+addsize的数据后是否溢出
	if( inseparable )
	{
		fwpos += addsize;
		//ACE_DEBUG(( LM_DEBUG, "calculate_avaiable_space_of_curr_packet:: fwpos=%d\n", fwpos ));
	}

	//ACE_DEBUG(( LM_DEBUG, "calculate_avaiable_space_of_curr_packet:: capability = %d, size=%d,pCurrPacket_->buff->end()=%d, pCurrPacket_->buff->base()=%d\n", pCurrPacket_->buff->capacity(),
	//	pCurrPacket_->buff->size(), pCurrPacket_->buff->end(), pCurrPacket_->buff->base() ));

	///当前包已满，将其保存起来，若设置了inseparable，那么 该包剩余空间会被忽略
	//ACE_DEBUG(( LM_DEBUG, "///当前包已满，将其保存起来，若设置了inseparable，那么 该包剩余空间会被忽略\n" ));
	if( fwpos >= pCurrPacket_->buff->base() + currPacketMaxSize )
	{
		//ACE_DEBUG(( LM_DEBUG, "fwpos >= pCurrPacket_->buff->end() - 1 - offset,\n当前packet已满，将其保存起来，以便以后发送\n" ));
		//TRACE_BUNDLE_DATA(false, pCurrPacket_, pCurrMsg_, totalsize, "None");
		//当前packet已满，将其保存起来，以便以后发送
		++currMsgPacketCount_;
		create_new_curr_packet();
		space = 0;
		//ACE_DEBUG(( LM_DEBUG, "calculate_avaiable_space_of_curr_packet:: create new packet, now currMsgPacketCount_=%d, space=0\n", currMsgPacketCount_ ));
	}

	remainsize = currPacketMaxSize - space;
	taddsize = addsize;
	//ACE_DEBUG(( LM_DEBUG, "calculate_avaiable_space_of_curr_packet:: taddsize=%d, remainsize=%d, currPacketMaxSize=%d\n", taddsize, remainsize, currPacketMaxSize ));

	//ACE_DEBUG(( LM_DEBUG, "// 如果当前包剩余空间小于要添加的字节则本次填满此包\n" ));
	// 如果当前包剩余空间小于要添加的字节则本次填满此包
	if( remainsize < addsize )
	{
		taddsize = remainsize;
		//ACE_DEBUG(( LM_DEBUG, "calculate_avaiable_space_of_curr_packet:: remainsize < addsize, now taddsize=%d\n", taddsize ));
	}

	currMsgLength_ += taddsize;
	//ACE_DEBUG(( LM_DEBUG,
	//	"calculate_avaiable_space_of_curr_packet::  currMsgLength_=%d, length = %d\n\n",
	//	currMsgLength_, pCurrPacket_->length() ));

	//TRACE_RETURN(taddsize);
	return taddsize;
}

/**
* @Brief
* This inline mothod is to create a new current packet from the right object pool
* it will determine it is tcp packet ot udp packet. It will update the value of
* @member Packet* pCurrPacket_.
* 该方法用从对应的对象池中构造出一个新的tcp包或者udp包，
* 该方法成功后，会更新 @member Packet* pCurrPacket_的值
* \n
* @para void
* @retvalue void
* @TO-DO
* Add algorism to choose existing packt pointer from packets_ container
* Possible way vould be using another packets_ contaner to store the packets
* in @m on_send_complete, just do packets_.clear().
*/
void Bundle::create_new_curr_packet(void)
{
	static MessageID msgid = 0;
	ACE_PoolPtr_Getter(pool, Packet, ACE_Null_Mutex);
	if( pt_ == PROTOCOL_TCP )
	{
		this->pCurrPacket_ = pool->Ctor();
	} else
	{
		this->pCurrPacket_ = pool->Ctor<MessageID, ProtocolType>(msgid, pt_);
	}
	this->pCurrPacket_->pBundle_ = this;
	packets_.push_back(pCurrPacket_);
}

/**
* @Brief
* 该方法用于填充当前变长消息的长度域，该长度有可能会超过
* This method is use to calculate the variable-len message's length field
* NETWORK_MESSAGE_MAX_SIZE 65535
* \n
* @para void
* @retvalue void
* @ChangeLog
*/
void Bundle::calculate_then_fill_variable_len_field(void)
{
	ACE_DEBUG(( LM_DEBUG,
		"Bundle::calculate_then_fill_variable_len_field()::@1::"
		"Need setup the length of the varaible-length msg\n" ));

	//由于有些msg会分包装载，因此我们必须找到第一个包然后在其上设置
	//消息长度，因此要确保pPacket指向容器中的第一个packet。
	//because we must setup msg length in the first packet that hold this msg, 
	//we have to ensure that the pPacket points to the first packet in the container.

	ACE_DEBUG(( LM_DEBUG,
		"Bundle::calculate_then_fill_variable_len_field()::@A::"
		"All packets pointer in pakctes_:\n" ));
	for each ( Packet* var in packets_ )
	{

		ACE_DEBUG(( LM_DEBUG,
			"packet pointer = %d\n", var ));
	}

	Packet* pPacket = pCurrPacket_;

	ACE_DEBUG(( LM_DEBUG,
		"Bundle::calculate_then_fill_variable_len_field()::@2::"
		"pPacket pointer = %d\n",
		pPacket ));

	if( currMsgPacketCount_ > 0 )
		pPacket = packets_[packets_.size() - currMsgPacketCount_];

	ACE_DEBUG(( LM_DEBUG,
		"Bundle::calculate_then_fill_variable_len_field()::@3::"
		"now pPacket pointer = %d\n",
		pPacket ));

	//当前消息的长度已知，减去消息id域和消息长度域，得到消息的有效信息长度(payload size)
	//the total length of the current msg is known, so we minuse id field size and the 
	// msg len field size, then we get the actual payload size of the current msg.
	ACE_DEBUG(( LM_DEBUG,
		"Bundle::calculate_then_fill_variable_len_field()::@4::"
		"total currMsgLength_ = %d\n",
		currMsgLength_ ));
	currMsgLength_ -= NETWORK_MESSAGE_ID_SIZE;
	currMsgLength_ -= NETWORK_MESSAGE_LENGTH_SIZE;
	ACE_DEBUG(( LM_DEBUG,
		"Bundle::calculate_then_fill_variable_len_field()::@5::"
		"payload currMsgLength_ = %d\n",
		currMsgLength_ ));

	// 按照设计一个包最大也不可能超过NETWORK_MESSAGE_MAX_SIZE
	// A msg will never exceed the max number of NETWORK_MESSAGE_MAX_SIZE

	ACE_DEBUG(( LM_DEBUG,
		"Bundle::calculate_then_fill_variable_len_field()::@6::"
		"g_componentType = %d, g_componentType = %d\n",
		g_componentType, g_componentType ));

	if( g_componentType == KBE_BOTS_TYPE || g_componentType == CLIENT_TYPE )
	{
		ACE_ASSERT(currMsgLength_ <= NETWORK_MESSAGE_MAX_SIZE);
	}

	// 如果消息长度大于等于NETWORK_MESSAGE_MAX_SIZE
	// 使用扩展消息长度机制, 向消息长度后面再填充4字节 用于描述更大的长度
	// if the size of a msg >= NETWORK_MESSAGE_MAX_SIZE, we use msg extension that
	// adds another 4 bytes behind the previous len field to hold more data.
	if( currMsgLength_ >= NETWORK_MESSAGE_MAX_SIZE )
	{
		ACE_DEBUG(( LM_DEBUG,
			"Bundle::calculate_then_fill_variable_len_field()::@7::"
			"currMsgLength_= %d >= NETWORK_MESSAGE_MAX_SIZE = %d\n",
			currMsgLength_, NETWORK_MESSAGE_MAX_SIZE ));

		MessageLength msgLen = NETWORK_MESSAGE_MAX_SIZE;
		MessageLength1 ex_msg_length = currMsgLength_;

		/* 先将MessageLength len写入到该packet中去 */
		ACE_ASSERT(pPacket->os.replace(msgLen, currMsgLengthPos_));

		ACE_DEBUG(( LM_DEBUG,
			"Bundle::calculate_then_fill_variable_len_field()::@7.1::"
			"currMsgLengthPos_ = %d\n",
			currMsgLengthPos_ ));

		// 更新长度域写位置
		// update the position just after msgLen field 
		currMsgLengthPos_ += NETWORK_MESSAGE_LENGTH_SIZE;

		ACE_DEBUG(( LM_DEBUG,
			"Bundle::calculate_then_fill_variable_len_field()::@7.2::"
			"currMsgLengthPos_ = %d\n",
			currMsgLengthPos_ ));

		/* 再将MessageLength1 ex_msg_length写入到len后边 */

		// 检测该包中是否有多余夫人4个字节来保存 ex_msg_length
		// test if there are 4 bytes space in this packet to store ex_msg_length

		ACE_DEBUG(( LM_DEBUG,
			"Bundle::calculate_then_fill_variable_len_field()::@7.2::"
			"currPacketMaxSize = %d\n"
			"pPacket->buff->length() = %d\n",
			currPacketMaxSize,
			pPacket->buff->length() ));

		int num = NETWORK_MESSAGE_LENGTH1_SIZE -
			( currPacketMaxSize - pPacket->buff->length() );

		ACE_DEBUG(( LM_DEBUG,
			"Bundle::calculate_then_fill_variable_len_field()::@7.2.1::"
			"num = %d\n",
			num ));

		if( num > 0 )
		{
			ACE_DEBUG(( LM_DEBUG,
				"Bundle::calculate_then_fill_variable_len_field()::@7.2.1::"
				"num = %d > 0, less than 4 bytes space in this packet.\n"
				"Start to move memory\n",
				num ));
			//Packet* pOldPacket = pPacket;
			ACE_DEBUG(( LM_DEBUG,
				"Bundle::calculate_then_fill_variable_len_field()::@7.2.2::"
				"now pPacket = %d\n",
				pPacket ));

			//resize 该包大小
			ACE_DEBUG(( LM_DEBUG,
				"Bundle::calculate_then_fill_variable_len_field()::@7.2.3::"
				"before resize, size = %d\n"
				"rd pos = %d, wr_pos = %d\n",
				pPacket->buff->size(), pPacket->buff->rd_ptr(), pPacket->buff->wr_ptr() ));

			pPacket->buff->size(pPacket->buff->size() + num);

			ACE_DEBUG(( LM_DEBUG,
				"Bundle::calculate_then_fill_variable_len_field()::@7.2.4::"
				"After resize, size = %d, length = %d, \n"
				"rd pos = %d, wr_pos = %d\n",
				pPacket->buff->size(), pPacket->buff->length(),
				pPacket->buff->rd_ptr(), pPacket->buff->wr_ptr() ));

			ACE_HEX_DUMP(( LM_DEBUG, pPacket->buff->base(), pPacket->buff->size(),
				"Before memmove::Result: \n" ));

			/// 将currMsgLengthPos_以后的内存向后移动num个bytes，为ex msg疼出来位置
			currMsgLengthPos_ = pPacket->buff->rd_ptr() + NETWORK_MESSAGE_ID_SIZE + NETWORK_MESSAGE_LENGTH_SIZE;
		} else
		{
			ACE_DEBUG(( LM_DEBUG,
				"Bundle::calculate_then_fill_variable_len_field()::@7.2.1::"
				"num = %d < 0, >= 4 bytes space in this packet, not resize()\n"
				"Start to move memory\n",
				num ));
		}

		ACE_OS::memmove(currMsgLengthPos_ + NETWORK_MESSAGE_LENGTH1_SIZE, currMsgLengthPos_, pPacket->buff->length() + ENCRYPTTION_WASTAGE_SIZE);

		/// advance wr pos
		pPacket->buff->wr_ptr(NETWORK_MESSAGE_LENGTH1_SIZE);

		///// anti-advance rd pos to the base()
		//pPacket->buff->rd_ptr(NETWORK_MESSAGE_LENGTH1_SIZE);
		ACE_HEX_DUMP(( LM_DEBUG,
			pPacket->buff->base(), pPacket->buff->size(),
			"after  memmove::Result: \n" ));

		/// 写入ex msg len
		*( (MessageLength1*) currMsgLengthPos_ ) = ex_msg_length;

		ACE_HEX_DUMP(( LM_DEBUG,
			pPacket->buff->base(), pPacket->buff->size(),
			"after  write to ex len::Result: \n" ));

	} else
	{
		//ACE_DEBUG(( LM_DEBUG,
		//	"Bundle::calculate_then_fill_variable_len_field()::@7::"
		//	"currMsgLength_= %d < NETWORK_MESSAGE_MAX_SIZE = %d\n",
		//	currMsgLength_, NETWORK_MESSAGE_MAX_SIZE ));

		//写入msglen
		ACE_ASSERT(pPacket->os.replace((MessageLength) currMsgLength_, currMsgLengthPos_));
	}
}

/**
* @Brief
* @deprate name = fill__curr_msg_end
* This mothod is is the last step before finish handling a msg. That is adding the length field
* to the variable-long msg or the fixed-long msg when g_packetAlwaysContainLength setup.
* It will handle two cases where a variable-length msg's length >= or <
* NETWORK_MESSAGE_MAX_SIZE(65535).
* 该方法是变长消息处理的最后一步， 即向当前变长消息添加长度值。其有效信息以及头部都以处理完毕
* 一共有两种情况，变长消息长度>= 或者<NETWORK_MESSAGE_MAX_SIZE(65535)
* 该方法在结束时将初始化所有与该消息相关的状态信息：
* currMsgID_ = currMsgPacketCount_ = currMsgLength_ = 0;
* currMsgLengthPos_ = NULL;
* \n
* @para bool issend
* whether to send this bundle or not after finishing handling the current msg
* when true, it will initialize all the memebers related the current msg at the end of this call
* 该参数用于表明是否该方法调用结束后发送该bundle，若为真，该方法会将当前包加入到发送队列中去
* \n
* @retvalue void
*
* @ChangeLog
* 11: 26 AM, 12/01/2015 :: Change "finish" to "fill_curr_msg_len_field",
* this name can clearly show  the aim of this function.
*/
//void Bundle::fill_curr_msg_len_field(bool issend)
void Bundle::end_new_curr_message(void)
{
	TRACE("Bundle::end_new_curr_message()");

	// 对消息进行跟踪 trace the msg
	if( numMessages_ >= 1 )
	{
		ACE_DEBUG(( LM_DEBUG,
			"end_new_curr_message :: @2 :: pCurrMsg_ = %@, numMessages_ = %d\n",
			pCurrMsg_, numMessages_ ));

		/// 更新该消息stats并回调跟踪函数
		/// update this msg's stats and call its callback method
		ACE_Singleton<NetStats, ACE_Null_Mutex>::instance()->
			trackMessage(NetStats::SEND, pCurrMsg_, currMsgLength_);

	}

	// 此处对于非固定长度的消息来说需要设置它的最终长度信息
	// need setup the length of the varaible-length msg
	if( currMsgType_ == NETWORK_VARIABLE_MESSAGE || g_packetAlwaysContainLength )
	{
		calculate_then_fill_variable_len_field();
	}

	/// dump all packets in this msg
	Packets::iterator iter = packets_.begin();
	for( ; iter != packets_.end(); iter++ )
	{
		ACE_HEX_DUMP(( LM_DEBUG,
			( *iter )->buff->base(),
			( *iter )->buff->length(),
			"end_new_curr_message(void):: dump result: \n" ));
	}

	ACE_DEBUG(( LM_DEBUG,
		"end_new_curr_message::pCurrMsg_ = %d, currMsgHandlerLength_= %d"
		"pCurrPacket_ = %@, currMsgID_= %d, currMsgLengthPos_ = %@,\n"
		"currMsgPacketCount_ = %d, currMsgLength_ = %d\n",
		pCurrMsg_,
		currMsgType_, pCurrPacket_,
		currMsgID_, currMsgLengthPos_,
		currMsgPacketCount_, currMsgLength_ ));

#if defined (_DEBUG)
	dumpMsgs();
#endif

	////清理该msg的相关变量值
	//currMsgType_ = currMsgID_ =
	//	currMsgPacketCount_ = currMsgLength_ = 0;
	//currMsgLengthPos_ = NULL;
	//pCurrMsg_ = NULL;

	TRACE_RETURN_VOID();
}

/**
* @Brief
* This inline method is to
* \n
* @para const MessageHandler* msg
*
* @retvalue void
*
* @Q1
* ///  这里出现了fill__curr_msg_begin， 有点不解，
* /// fill__curr_msg_begin函数我的理解是为了添加消息长度信息，
* /// 而这时的currMsgLength_应该为0，，那么fill__curr_msg_begin在这里到底有什么用？
* @Q1 - Answer
* 该方法会跟踪当前msg以及初始化所有的成员函数， 此时跟踪当前msg相当可以看到当前msg，
* 就能够看到该msg的长度域的值，相当于调试了。
*/
void Bundle::start_new_curr_message(Message* msg)
{
	ACE_DEBUG(( LM_DEBUG,
		"@1 void Bundle::start_new_curr_message(const MessageHandler* msg)\n" ));

	//清理上一个msg的相关变量值
	currMsgType_ = currMsgID_ = currMsgPacketCount_ = currMsgLength_ = 0;
	currMsgLengthPos_ = NULL;
	pCurrMsg_ = NULL;

	/// 若当前包为空，则构造一个新的包
	if( pCurrPacket_ == NULL ) this->create_new_curr_packet();

	//ACE_HEX_DUMP(( LM_DEBUG,
	//	pCurrPacket_->buff->base(), pCurrPacket_->buff->length(),
	//	"Bundle::start_new_curr_message()::@A\n" ));

	/// 将消息id写入到当前包中
	( *this ) << msg->msgID_;

	/// 再更新当前包的msgid
	pCurrPacket_->msgID_ = msg->msgID_;

	/// 这种指向const对象的指针只是限制不能修改p指向对象的数值，
	/// 而不是限制p指向什么对象
	pCurrMsg_ = msg;

	// 此处对于非固定长度的消息来说需要先设置它的消息长度位为0， 到最后需要填充长度
	if( pCurrMsg_->msgType_ == NETWORK_VARIABLE_MESSAGE )
	{
		ACE_DEBUG(( LM_DEBUG, "Bundle::start_new_curr_message():: it is variable msg\n" ));
		calculate_avaiable_space_of_curr_packet(ACE_SIZEOF_SHORT);
		currMsgLengthPos_ = pCurrPacket_->os.write_short_placeholder();

	}

	ACE_HEX_DUMP(( LM_DEBUG, pCurrPacket_->buff->base(), pCurrPacket_->buff->size(),
		"write variable len placeholder \n" ));

	++numMessages_;
	currMsgID_ = msg->msgID_;
	currMsgType_ = msg->msgType_;
	++currMsgPacketCount_;
}

/**
* @Brief
* 该方法在该bundle发送后会被回掉，当reuse为真时, 该方法直接返回，
* 该bundle所有的状态信息都维持不变。包括 当前包，当前处理的消息以及包容器
* This inline mothod is call back after this bundle gets sent.
* when reuse = true, we do nothing but return so that all the states of this bundle
* will remain unchanged including the current msg, current packet and the container
* \n
* @para void
* @retvalue void
*/
void Bundle::on_send_completed(void)
{
	/// All packets are sent and so we need setup current packet pointer to NULL
	/// otherwise, pCurrPacket_ will pointer to the packet that has been recycled 
	pCurrPacket_ = NULL;

	/// 如果重用该bundle，直接返回
	/// if it is reuasable bundle, nothing to be done and directly return
	if( reuse_ )
		return;


	/// 回收所有的包 recycle each packet
	Packets::iterator iter = packets_.begin();
	for( ; iter != packets_.end(); ++iter )
	{
		ACE_PoolPtr_Getter(ObjPool, Packet, ACE_Null_Mutex);
		ObjPool->Dtor(*iter);
	}
	/// 清空元素，但不释放内存 此时 size=0
	packets_.clear();

}

inline void  Bundle::send(const NetworkInterface* networkInterface, Channel* pChannel)
{
	/// 更新当前通道指针 update curr channel
	this->pChnnel_ = pChannel;
	//networkInterface.send(*this, pChannel);
}

void  Bundle::resend(const NetworkInterface* networkInterface, Channel* pChannel)
{

}

void Bundle::tcp_send(const ACE_SOCK_Stream* ep)
{
	//使用最后一条msg的 handler进行跟踪
	//TRACE_BUNDLE_DATA(false, pCurrPacket_, pCurrMsg_, this->totalSize(),
	//	( pChannel_ != NULL ? pChannel_->c_str() : "None" ));

	/// initialize all the variables used locally in this method
	static int retries = 0;
	retries = 0;
	static Reason reason = REASON_SUCCESS;
	reason = REASON_SUCCESS;
	static ssize_t sent_cnt = 0;
	sent_cnt = 0;
	static size_t sent_cnt_error = 0;
	sent_cnt_error = 0;
	const static ACE_Time_Value wait(0, 10 * 1000);

	/// add msg length field and add the current packet to send queue before sending
	/// this->fill__curr_msg_end();

	Packets::iterator iter = packets_.begin();
	for( ; iter != packets_.end(); ++iter )
	{
		retries = 0;
		reason = REASON_SUCCESS;
		sent_cnt = 0;
		sent_cnt_error = 0;

		Packet* pPacket = ( *iter );
		while( true )
		{
			++retries;

			/** @name Counted send/receive methods
			*
			* The counted send/receive methods attempt to transfer a specified number
			* of bytes even if they must block and retry the operation in order to
			* transfer the entire amount. The time spent blocking for the entire
			* transfer can be limited by a specified ACE_Time_Value object which is
			* a relative time (i.e., a fixed amount of time, not an absolute time
			* of day). These methods return the count of transferred bytes, or -1
			* if an error occurs or the operation times out before the entire requested
			* amount of data has been transferred. In error or timeout situations it's
			* possible that some data was transferred before the error
			* or timeout. The @c bytes_transferred parameter is used to obtain the
			* count of bytes transferred before the error or timeout occurred. If the
			* total specified number of bytes is transferred without error, the
			* method return value should equal the value of @c bytes_transferred.
			*
			* @param buf      The buffer to write from or receive into.
			* @param iov      An I/O vector containing a specified number of
			*                 count/pointer pairs directing the data to be transferred.
			* @param iovcnt   The number of I/O vectors to be used from @a iov.
			* @param len      The number of bytes to transfer.
			* @param flags    Flags that will be passed through to the @c recv()
			*                 system call.
			* @param timeout  Indicates how long to blocking trying to transfer data.
			*                 If no timeout is supplied (timeout == 0) the method will
			*                 wait indefinitely or until an error occurs for the
			*                 specified number of bytes to be transferred.
			*                 To avoid any waiting, specify a timeout value with
			*                 0 seconds. Note that the timeout period restarts on
			*                 each retried operation issued; therefore, an operation
			*                 that requires multiples retries may take longer than the
			*                 specified timeout to complete.
			* @param bytes_transferred If non-0, points to a location which receives
			*                 the total number of bytes transferred before the method
			*                 returns, even if it's less than the number requested.
			*
			* @retval      len, the complete number of bytes transferred.
			* @retval      0  EOF, i.e., the peer closed the connection.
			* @retval      -1 an error occurred before the entire amount was
			*                 transferred. Check @c errno for more information.
			*                 If the @a timeout period is reached, errno is ETIME.
			*
			* On partial transfers, i.e., if any data is transferred before
			* timeout/error/EOF, *@a bytes_transferred will contain the number of
			* bytes transferred.
			*/
			sent_cnt = ep->send_n(pPacket->buff->base() + pPacket->sentSize,
				pPacket->buff->length() - pPacket->sentSize,
				&wait, &sent_cnt_error);

			if( sent_cnt > 0 )
				pPacket->sentSize += sent_cnt;

			if( sent_cnt == -1 )
			{
				pPacket->sentSize += sent_cnt_error;

				if( kbe_lasterror() == ETIME )
				{
					ACE_DEBUG(( LM_WARNING, "timeout before send all data...\n" ));
				} else if( kbe_lasterror() == EWOULDBLOCK || kbe_lasterror() == WSAEWOULDBLOCK )
				{
					ACE_DEBUG(( LM_WARNING, "send buff full..\n" ));
				} else
				{
					ACE_DEBUG(( LM_WARNING, "unkonow error happened when sending..\n" ));
				}

				if( retries > 60 && kbe_lasterror() != REASON_SUCCESS )
				{
					ACE_DEBUG(( LM_ERROR, "Packet discarded with reason %d \n", kbe_lasterror() ));
					break;
				}
				continue;
			} else
			{
				if( pPacket->sentSize == pPacket->length() ) break;
				continue;
			}
		}
	}
}

void  Bundle::udp_send(const ACE_SOCK_Dgram* ep, const ACE_INET_Addr* remoteaddr)
{
	/// initialize all the variables used locally in this method
	static int retries = 0;
	retries = 0;
	static Reason reason = REASON_SUCCESS;
	reason = REASON_SUCCESS;
	static ssize_t sent_cnt = 0;
	sent_cnt = 0;
	const static ACE_Time_Value wait(0, 10 * 1000);

	/// add msg length field and add the current packet to send queue before sending
	/// this->fill__curr_msg_end();

	Packets::iterator iter = packets_.begin();
	for( ; iter != packets_.end(); ++iter )
	{
		retries = 0;
		reason = REASON_SUCCESS;
		sent_cnt = 0;

		Packet* pPacket = ( *iter );
		while( true )
		{
			++retries;

			/**
			* Wait up to @a timeout amount of time to send a datagram to
			* @a buf.  The ACE_Time_Value indicates how long to blocking
			* trying to receive.  If @a timeout == 0, the caller will block
			* until action is possible, else will wait until the relative time
			* specified in *@a timeout elapses).  If <send> times out a -1 is
			* returned with @c errno == ETIME.  If it succeeds the number of
			* bytes sent is returned.
			*/
			sent_cnt = ep->send(pPacket->buff->base() + pPacket->sentSize,
				pPacket->buff->length() - pPacket->sentSize, *remoteaddr,
				0, &wait);

			if( sent_cnt > 0 )
				pPacket->sentSize += sent_cnt;

			if( sent_cnt == -1 )
			{
				if( kbe_lasterror() == ETIME )
				{
					ACE_DEBUG(( LM_WARNING, "timeout before send all data...\n" ));
				} else if( kbe_lasterror() == EWOULDBLOCK || kbe_lasterror() == WSAEWOULDBLOCK )
				{
					ACE_DEBUG(( LM_WARNING, "send buff full..\n" ));
				} else
				{
					ACE_DEBUG(( LM_WARNING, "unkonow error happened when sending..\n" ));
				}

				if( retries > 60 && kbe_lasterror() != REASON_SUCCESS )
				{
					ACE_DEBUG(( LM_ERROR, "Packet discarded with reason %d \n", kbe_lasterror() ));
					break;
				}
				continue;
			} else
			{
				if( pPacket->sentSize == pPacket->length() ) break;
				continue;
			}
		}
	}
}

void Bundle::dumpMsgs()
{
	TRACE("Bundle::dumpMsgs()");

	if( !pCurrMsg_ ) return;

	Packets packets;
	packets.insert(packets.end(), packets_.begin(), packets_.end());

	ACE_PoolPtr_Getter(pool, Packet, ACE_Null_Mutex);
	Packet* temppacket = pool->Ctor();
	temppacket->buff->size(512);

	char* base = in.start()->base();
	size_t size = in.start()->size();
	char* w = in.start()->wr_ptr();
	char* r = in.start()->rd_ptr();

	MessageID msgid = 0;
	MessageLength msglen = 0;
	MessageLength1 msglen1 = 0;
	const Message* pCurrMsgHandler = NULL;

	// 0:读取消息ID， 1：读取消息长度， 2：读取消息扩展长度, 3:读取内容
	enum { id = 0, len, len1, body };
	int state = id;
	for( Packets::iterator iter = packets.begin(); iter != packets.end(); iter++ )
	{
		Packet* pPacket = ( *iter );
		if( pPacket->length() == 0 ) continue;

		char* rpos = pPacket->buff->rd_ptr();
		char* wpos = pPacket->buff->wr_ptr();

		const_cast<ACE_Message_Block*>( in.start() )->base(pPacket->buff->base(),
			pPacket->buff->size());
		const_cast<ACE_Message_Block*>( in.start() )->wr_ptr(wpos);

		ACE_DEBUG(( LM_DEBUG, "pPacket->length() = %d\n", pPacket->length() ));

		while( pPacket->length() > 0 )
		{
			if( state == id )
			{
				ACE_DEBUG(( LM_DEBUG, " @1::state == id\n" ));
				// 一些sendto操作的包导致, 这类包也不需要追踪
				if( pPacket->length() < NETWORK_MESSAGE_ID_SIZE )
				{
					ACE_DEBUG(( LM_DEBUG,
						"@2::pPacket->length() < NETWORK_MESSAGE_ID_SIZE\n" ));
					pPacket->on_read_packet_done();
					continue;
				}

				in >> msgid;
				pPacket->buff->rd_ptr(in.rd_ptr());
				ACE_DEBUG(( LM_DEBUG, "msgid = %d\n", msgid ));
				state = len;
				continue;

			} else if( state == len )
			{
				ACE_DEBUG(( LM_DEBUG, " @2::state == len\n" ));

				pCurrMsgHandler = pCurrMsg_->pMsgs_->find(msgid);

				// 一些sendto操作的包导致找不到MsgHandler, 这类包也不需要追踪
				if( !pCurrMsgHandler )
				{
					ACE_DEBUG(( LM_DEBUG, " @3::!pCurrMsgHandler\n" ));
					pPacket->on_read_packet_done();
					continue;
				}

				if( pCurrMsgHandler->msgType_ == NETWORK_VARIABLE_MESSAGE || g_packetAlwaysContainLength )
				{
					ACE_DEBUG(( LM_DEBUG, " @4::NETWORK_VARIABLE_MESSAGE\n" ));
					in >> msglen;
					pPacket->buff->rd_ptr(in.rd_ptr());
					temppacket->os << msglen;

					if( msglen == NETWORK_MESSAGE_MAX_SIZE )
						state = len1;
					else
						state = body;

				} else
				{
					ACE_DEBUG(( LM_DEBUG, " @4::NETWORK_FIXED_MESSAGE\n" ));
					msglen = pCurrMsgHandler->msgArgsBytesCount_;
					temppacket->os << msglen;
					state = 3;
				}

				MessageLength len = 0;
				memcpy(&len, temppacket->buff->base(), sizeof(MessageLength));
				ACE_DEBUG(( LM_DEBUG, "msglen = %d\n", len ));
				continue;

			} else if( state == len1 )
			{
				ACE_DEBUG(( LM_DEBUG, " @5::state == len1\n" ));
				in >> msglen1;
				pPacket->buff->rd_ptr(in.rd_ptr());
				temppacket->os << msglen1;
				state = body;

				MessageLength1 len1 = 0;
				memcpy(&len1, temppacket->buff->base() + sizeof(MessageLength), sizeof(MessageLength1));
				ACE_DEBUG(( LM_DEBUG, "msglen1 = %d\n", len1 ));

				continue;

			} else if( state == body )
			{
				ACE_DEBUG(( LM_DEBUG, " @6::state == body\n" ));
				MessageLength1 totallen = msglen1 > 0 ? msglen1 : msglen;
				if( pPacket->length() >= totallen - temppacket->length() )
				{
					temppacket->os.write_char_array(pPacket->buff->rd_ptr(), totallen);
					pPacket->buff->rd_ptr(totallen);
				} else
				{
					temppacket->os.write_char_array(pPacket->buff->rd_ptr(), pPacket->length());
					pPacket->on_read_packet_done();
				}

				if( temppacket->length() - NETWORK_MESSAGE_ID_SIZE - NETWORK_MESSAGE_LENGTH_SIZE == totallen )
				{
					state = 0;
					msglen1 = 0;
					msglen = 0;
					msgid = 0;
					//TRACE_MESSAGE_PACKET(false, pMemoryStream, pCurrMsgHandler, pMemoryStream->length(),
					//	( pChannel_ != NULL ? pChannel_->c_str() : "None" ));
					temppacket->reset();
					continue;
				}
			}
		};

		pPacket->buff->rd_ptr(rpos);
		pPacket->buff->wr_ptr(wpos);

		ACE_HEX_DUMP(( LM_DEBUG,
			temppacket->buff->base(),
			temppacket->buff->length(),
			"dump msg result:\n" ));
	}

	const_cast<ACE_Message_Block*>( in.start() )->base(base, size);
	const_cast<ACE_Message_Block*>( in.start() )->wr_ptr(w);
	const_cast<ACE_Message_Block*>( in.start() )->rd_ptr(r);

	pool->Dtor(temppacket);

	TRACE_RETURN_VOID();
}

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL