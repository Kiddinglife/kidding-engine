#ifndef Bundlle_H_
#define Bundlle_H_

#include "ace\pre.h"
#include "common\kbe_blowfish.hpp"
#include "net_common.h"
#include "net\Packet.h"
#include "NetStats.h"
#include "common\ace_object_pool.h"
#include "Message.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

struct NetworkInterface;
struct Channel;
struct Message;

#define TCP_PACKET_MAX_CHUNK_SIZE  PACKET_MAX_SIZE_TCP - ENCRYPTTION_WASTAGE_SIZE
#define UDP_PACKET_MAX_CHUNK_SIZE PACKET_MAX_SIZE_UDP - ENCRYPTTION_WASTAGE_SIZE


#define PACKET_OUT_VALUE(v)\
if( packets_.size() <= 0 ) return *this;\
static ACE_Message_Block* block = const_cast<ACE_Message_Block*>( in.start() );\
in >> v;\
packets_[0]->buff->rd_ptr(in.rd_ptr());\
if( in.length() == 0 )\
{\
	packets_.erase(packets_.begin());\
	if( packets_.size() == 0 ) return *this;\
	block->base(packets_[0]->buff->base(),\
		packets_[0]->buff->length());\
	block->wr_ptr(packets_[0]->buff->wr_ptr());\
}

#define new_pCurrPacket(PACKET_TYPE)\
ACE_PoolPtr_Getter(ObjPool, PACKET_TYPE, ACE_Null_Mutex);\
this->pCurrPacket_ = ObjPool->Ctor();\
this->pCurrPacket_->pBundle_ = this;         


struct Bundle
{
	typedef std::vector<Packet*> Packets;
	/*所有的packet指针集合 : Packet Pointers Container*/
	Packets                  packets_;

	/*该bundle所处理packet的类型(TCP UDP) : This bundle's packets type */
	ProtocolType         pt_;

	/*通道指针 : CHANNEL*/
	Channel*               pChnnel_;

	/*该bundle处理的msg总的数量 : The number of all msgs handled in this bundle*/
	size_t		             numMessages_;

	/*该bundle每个packet的最大容量(TCP 1460, UDP 1470) : max size of packet */
	size_t                     currPacketMaxSize;

	/*当前需要处理的packet指针 : the current packet's pointer*/
	Packet*                 pCurrPacket_;

	/* 承载当前msg所需的packet数量 : how many packets needed to hold the current msg */
	ACE_UINT32          currMsgPacketCount_;

	/* 当前msg的类型 定长或变长  : the current msg type fix-length or variable-length */
	ACE_INT8            currMsgType_;

	/*The size of paading between encrytype field and payload field */
	size_t                     currPacketPaddingBeforeEncrytypeField;

	/* 当前msg的length域的位置偏移 : the current msg length field position offset */
	char*                     currMsgLengthPos_;

	/* 当前msg的id号码 : the current msg's id */
	MessageID             currMsgID_;

	/**
	 * 当前packet中的msg的payload长度(不包含id和len)，以byte为单位
	 * 如果某个msg由多于一个packet装载，该数值会被重新从0开始计算
	 * the current msg's length only in the current packet with the unit of byte
	 * it will be recalculated if there are more than one packet to hold this msg
	 */
	MessageLength1   currMsgLength_;

	/*当前msg的处理函数 : the current msg's handler */
	Message* pCurrMsg_;

	/*是否重用该bundle : whether to reuse this bundle */
	bool reuse_;

	/*用于读取包中的序列化数据 : used to read the data from the packets*/
	ACE_InputCDR in;

	Bundle(Channel * pChannel = NULL, ProtocolType pt = PROTOCOL_TCP);

	~Bundle(void) { }

	/**
	 * This method need to be called firstly before reading msg from packet
	 * 当从包中读取消息时，该方法需要被首先调用
	 * @param void
	 * @retvalue void
	 */
	void init_instream(void)
	{
		const_cast<ACE_Message_Block*>( in.start() )->base(packets_[0]->buff->base(),
			packets_[0]->buff->size());
		const_cast<ACE_Message_Block*>( in.start() )->wr_ptr(packets_[0]->buff->wr_ptr());
	}

	/**
	 * @para calpCurrPacket_
	 * 如果包含当前包的长度，则为真，否则为假
	 * true if calculating pCurrPacket_'s length, false if not.
	 * \n
	 * @retvalue size_t
	 * 返回值为该bundle中的所有包的有效长度(即msg所占的空间大小)
	 * return the all packets length
	 */
	size_t get_packets_length(void);

	/**
	 * @Brief
	 * 该方法调整当前包的空间以便装入（@para addsize）大小的消息。
	 * 一个消息如果很大的话，可能需要多个包来装载
	 * This mothod is is to ajust the space in the current packet when adding @para
	 * addsize msg. A big msg may be divided into more than one packet.
	 *
	 * @para size_t addsize
	 * 写入包中的消息大小， 不能大于包的最大容量
	 * the size of msg that will be added to the current packet
	 *
	 * @para bool inseparable
	 * 若为真，该方法会检测当前包可用空间的大小是否足以装下 addsize 大小的数据。
	 * 如果当前包不能够装下的的话，该方法会创建一个新的包。
	 * 这时，老包的剩余空间将为空。
	 * 对于基本数据类型使用inseparable = true，这样避免了将基本类型分包，
	 * 对于str数据，使用inseparable = false，这样可以有效地利用空间
	 * if inseparable is true, it tests if  the rest of space in the current packet can hold the
	 * addsize of msg, if space is not enough, it will create a new packet to hold whole msg,
	 * the old space will leave empty.
	 * basic types like int double uses inseparable = true to avoid the fragment of packets.
	 * string type uses inseparable = false to avoid wasting the memory
	 *
	 * @retvalue size_t
	 * 返回值为实际所能够写入的有效大小，
	 * 如果 @para inseparable == false, 可能会 <= @para addsize 的大小
	 * the actually-writable space size in current packet will be returned that may be <= addsize
	 *
	 * @ChangeLog
	 * 11: 26 AM, 12/01/2015:: Change "calculate_avaiable_space_of_curr_packet" to"calculateAvaiableSpaceInCurrPacket",
	 * this name can clearly show  the aim of this function.
	 */
	size_t calculate_avaiable_space_of_curr_packet(size_t addsize, bool inseparable = true);

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
	*/
	inline void create_new_curr_packet(void);

	/**
	* 该方法将会回收该bundle中所有的包到对应的对象池中，
	* 并重新初始化各个成员的值。
	* this mothod recycles all the packets in the container back to the right pool
	* it also initialize all the memeber values.
	*/
	void clear(void);

	/**
	* 该方法将会回收该bundle中所有的包到对应的对象池中
	* this mothod recycles all the packets in the container back to the right pool
	*/
	void recycle_all_packets(void);

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
	void calculate_then_fill_variable_len_field(void);

	/**
	* @Brief
	* This inline mothod is to
	* \n
	* @para const MessageHandler* msgHandler
	*
	* @retvalue void
	*/
	void start_new_curr_message(Message* msgHandler);

	/**
	* @Brief fill_curr_msg_len_field
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
	* 11: 26 AM, 12/01/2015 :: Change from [void fill_curr_msg_len_field(bool issend = true)]
	*  to "fill_curr_msg_len_field",
	* this name can clearly show  the aim of this function.
	*/
	void end_new_curr_message();

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
	inline void on_send_completed(void);

	inline void send(const NetworkInterface* networkInterface, Channel* pChannel);

	void resend(const NetworkInterface* networkInterface, Channel* pChannel);

	void tcp_send(const ACE_SOCK_Stream* ep);
	void udp_send(const ACE_SOCK_Dgram* ep, const ACE_INET_Addr* remoteaddr);

	/* stream operations */
	Bundle &operator<<( ACE_CDR::Octet value )
	{
		calculate_avaiable_space_of_curr_packet(ACE_SIZEOF_CHAR);
		ACE_OutputCDR::from_octet v(value);
		pCurrPacket_->os << v;
		return *this;
	}
	Bundle &operator>>( ACE_CDR::Octet& value )
	{
		ACE_InputCDR::to_octet v(value);
		PACKET_OUT_VALUE(v);
		return *this;
	}

	Bundle &operator<<( ACE_CDR::Char value )
	{
		calculate_avaiable_space_of_curr_packet(ACE_SIZEOF_CHAR);
		pCurrPacket_->os << value;
		return *this;
	}
	Bundle &operator>>( ACE_CDR::Char& value )
	{
		PACKET_OUT_VALUE(value)
			return *this;
	}

	Bundle &operator<<( ACE_CDR::UShort value )
	{
		calculate_avaiable_space_of_curr_packet(ACE_SIZEOF_SHORT);
		pCurrPacket_->os << value;
		return *this;
	}
	Bundle &operator>>( ACE_CDR::UShort& value )
	{
		PACKET_OUT_VALUE(value);
		return *this;
	}

	Bundle &operator<<( ACE_CDR::ULong value )
	{
		calculate_avaiable_space_of_curr_packet(ACE_SIZEOF_LONG);
		pCurrPacket_->os << value;
		return *this;
	}
	Bundle &operator>>( ACE_CDR::ULong& value )
	{
		PACKET_OUT_VALUE(value);
		return *this;
	}

	Bundle &operator<<( ACE_CDR::ULongLong value )
	{
		calculate_avaiable_space_of_curr_packet(ACE_SIZEOF_LONG_LONG);
		pCurrPacket_->os << value;
		return *this;
	}
	Bundle &operator>>( ACE_CDR::ULongLong& v )
	{
		PACKET_OUT_VALUE(v);
		return *this;
	}

	Bundle &operator<<( ACE_CDR::Short value )
	{
		calculate_avaiable_space_of_curr_packet(ACE_SIZEOF_SHORT);
		pCurrPacket_->os << value;
		return *this;
	}
	Bundle &operator>>( ACE_CDR::Short& value )
	{
		PACKET_OUT_VALUE(value);
		return *this;
	}

	Bundle &operator<<( ACE_CDR::Long value )
	{
		calculate_avaiable_space_of_curr_packet(ACE_SIZEOF_LONG);
		pCurrPacket_->os << value;
		return *this;
	}
	Bundle &operator>>( ACE_CDR::Long& value )
	{
		PACKET_OUT_VALUE(value);
		return *this;
	}

	Bundle &operator<<( ACE_CDR::LongLong value )
	{
		calculate_avaiable_space_of_curr_packet(ACE_SIZEOF_LONG_LONG);
		pCurrPacket_->os << value;
		return *this;
	}
	Bundle &operator>>( ACE_CDR::LongLong& value )
	{
		PACKET_OUT_VALUE(value);
		return *this;
	}

	Bundle &operator<<( ACE_CDR::Float value )
	{
		calculate_avaiable_space_of_curr_packet(ACE_SIZEOF_FLOAT);
		pCurrPacket_->os << value;
		return *this;
	}
	Bundle &operator>>( ACE_CDR::Float& value )
	{
		PACKET_OUT_VALUE(value);
		return *this;
	}

	Bundle &operator<<( ACE_CDR::Double value )
	{
		calculate_avaiable_space_of_curr_packet(ACE_SIZEOF_DOUBLE);
		pCurrPacket_->os << value;
		return *this;
	}
	Bundle &operator>>( ACE_CDR::Double& value )
	{
		PACKET_OUT_VALUE(value);
		return *this;
	}

	Bundle &operator<<( ACE_CDR::Boolean value )
	{
		calculate_avaiable_space_of_curr_packet(ACE_SIZEOF_CHAR);
		ACE_OutputCDR::from_boolean v(value);
		pCurrPacket_->os << v;
		return *this;
	}
	Bundle &operator>>( ACE_CDR::Boolean& value )
	{
		ACE_InputCDR::to_boolean v(value);
		PACKET_OUT_VALUE(v);
		return *this;
	}

	Bundle &operator<<( KBE_SRV_COMPONENT_TYPE value )
	{
		return ( *this ) << ( ACE_CDR::Octet )value;
	}
	Bundle &operator>>( KBE_SRV_COMPONENT_TYPE& value )
	{
		static ACE_CDR::Octet  v = 0;
		( *this ) >> v;
		value = (KBE_SRV_COMPONENT_TYPE) v;
		return *this;
	}

	Bundle &operator<<( ENTITY_MAILBOX_TYPE value )
	{
		return ( *this ) << ( ACE_CDR::Octet )value;
	}
	Bundle &operator>>( ENTITY_MAILBOX_TYPE& value )
	{
		static ACE_CDR::Octet  v = 0;
		( *this ) >> v;
		value = (ENTITY_MAILBOX_TYPE) v;
		return *this;
	}

	Bundle &operator<<( const char *str )
	{
		static size_t len = 0;  // +1为字符串尾部的0位置
		static size_t addtotalsize = 0;
		static size_t ilen = 0;

		len = ACE_OS::strlen(str) + 1;
		addtotalsize = ilen = 0;

		while( len > 0 )
		{
			/// get the actual writable-len in the current packet
			ilen = calculate_avaiable_space_of_curr_packet(len, false);
			/// write it 
			pCurrPacket_->os.write_char_array(str + addtotalsize, ilen);
			/// update the new write start point after witting
			addtotalsize += ilen;
			/// update the actual written-len
			len -= ilen;
		}
		return *this;
	}

	/// read char string you need to make sure you have give a big=enough arr
	Bundle &operator>>( char* str )
	{
		/*ACE_DEBUG(( LM_DEBUG, "Bundle &operator>>( char* str ) :: at begin, in.rd_pos = %d\n",
			in.rd_ptr() ));*/

		if( packets_.size() <= 0 ) return *this;
		static ACE_Message_Block* block = const_cast<ACE_Message_Block*>( in.start() );
		*str = '1';
		while( ( *str ) )
		{
			while( in.length() > 0 )
			{
				in.read_char(*str);
				packets_[0]->buff->rd_ptr(in.rd_ptr());
				if( ( *str ) == 0 )
				{
					//ACE_DEBUG(( LM_DEBUG, "Bundle &operator>>( char* str ) :: cnt = 0, break\n" ));
					break;
				}
				++str;
			}

			if( in.length() == 0 )
			{
				packets_.erase(packets_.begin());
				if( packets_.size() == 0 ) return *this;
				block->base(packets_[0]->buff->base(),
					packets_[0]->buff->length());
				block->wr_ptr(packets_[0]->buff->wr_ptr());
			}
		}
		//ACE_DEBUG(( LM_DEBUG, "Bundle &operator>>( char* str ) :: at end, in.rd_pos = %d\n", in.rd_ptr() ));

		return *this;
	}

	Bundle &operator<<( const std::string &value )
	{
		static size_t len = 0;  // +1为字符串尾部的0位置
		static size_t addtotalsize = 0;
		static size_t ilen = 0;

		len = value.size() + 1;
		addtotalsize = ilen = 0;

		while( len > 0 )
		{
			/// get the actual writable-len in the current packet
			ilen = calculate_avaiable_space_of_curr_packet(len, false);
			/// write it 
			pCurrPacket_->os.write_char_array(value.c_str() + addtotalsize, ilen);
			/// update the new write start point after writting
			addtotalsize += ilen;
			/// update the actual written-len
			len -= ilen;
		}
		return *this;
	}

	Bundle &operator>>( std::string &value )
	{

		if( packets_.size() <= 0 ) return *this;

		///Erases the contents of the string, 
		/// which becomes an empty string (with a length of 0 characters).
		value.clear();

		static ACE_Message_Block* block = const_cast<ACE_Message_Block*>( in.start() );

		static char cnt = '1';
		cnt = '1';
		ACE_DEBUG(( LM_DEBUG, "@1::char = %c \n", cnt ));
		while( cnt )
		{
			ACE_DEBUG(( LM_DEBUG, "@2::char = %c \n", cnt ));

			while( in.length() > 0 )
			{
				in >> cnt;
				packets_[0]->buff->rd_ptr(in.rd_ptr());
				if( cnt == 0 )
				{
					ACE_DEBUG(( LM_DEBUG, "@3::char = %c = 0\n", cnt ));
					ACE_DEBUG(( LM_DEBUG, "@4::size = %d \n", value.size() ));
					break;
				}
				value += cnt;
				//ACE_DEBUG(( LM_DEBUG, "@4 :: char = %c \n", cnt ));
				//ACE_DEBUG(( LM_DEBUG, "@4 :: size = %d \n", value.size() ));
			}

			//ACE_DEBUG(( LM_DEBUG, "@4 :: packets size = %d \n", packets_.size() ));

			if( in.length() == 0 )
			{
				/*ACE_DEBUG(( LM_DEBUG, "@7::go to next packet  because in.length == 0 but cnt = %c\n", (int) cnt ));*/
				packets_.erase(packets_.begin());
				if( packets_.size() == 0 ) return *this;
				block->base(packets_[0]->buff->base(),
					packets_[0]->buff->length());
				block->wr_ptr(packets_[0]->buff->wr_ptr());
			}
		}

		return *this;
	}

	Bundle &operator<<( Bundle& bundle )
	{
		if( &bundle == this )
			return *this;

		Packets::iterator iter = bundle.packets_.begin();
		for( ; iter != bundle.packets_.end(); ++iter )
		{
			write_char_arr(( *iter )->buff->base(), ( *iter )->length());
		}

		return  bundle.pCurrPacket_ == NULL ? *this :
			write_char_arr(bundle.pCurrPacket_->buff->base(), bundle.pCurrPacket_->length());

	}

	//该方法复制packet中的数据，即 length()区域内的数据
	Bundle& operator<<( Packet& packet )
	{
		if( packet.length() > 0 )
			write_char_arr(packet.buff->rd_ptr(), packet.length());
		else
			return *this;
	}

	// with blob len written befre the actual data in the packet 
	Bundle& write_blob(const char* blob, ArraySize len)
	{
		return ( ( *this ) << len ).write_char_arr(blob, len);
	}

	Bundle& read_blob(char* blob, ArraySize len)
	{
		if( packets_.size() <= 0 ) return *this;

		static ACE_Message_Block* block = const_cast<ACE_Message_Block*>( in.start() );
		static size_t advance = 0;
		advance = 0;

		//ACE_HEX_DUMP(( LM_DEBUG, in.start()->rd_ptr(), in.length(), "read_blob :: before read :: Result: \n" )); 
		while( in.length() < len )
		{
			//ACE_DEBUG(( LM_DEBUG, "blob = %d\n", blob ));
			len -= in.length();
			advance = in.length();
			in.read_char_array(blob, in.length());
			packets_[0]->buff->rd_ptr(in.rd_ptr());

			packets_.erase(packets_.begin());
			block->base(packets_[0]->buff->base(),
				packets_[0]->buff->length());
			block->wr_ptr(packets_[0]->buff->wr_ptr());

			blob += advance;
		}
		//ACE_DEBUG(( LM_DEBUG, "after advance, blob = %d\n", blob ));
		in.read_char_array(blob, len);
		packets_[0]->buff->rd_ptr(in.rd_ptr());
		return *this;
	}

	Bundle& write_blob_string(const std::string& str, ArraySize len)
	{
		return this->write_blob(str.data(), str.size());
	}

	Bundle& write_char_arr(const char *str, size_t len)
	{
		static size_t addtotalsize = 0;
		static size_t ilen = 0;
		addtotalsize = ilen = 0;

		while( len > 0 )
		{
			/// get the actual writable-len in the current packet
			ilen = calculate_avaiable_space_of_curr_packet(len, false);
			/// write it 
			pCurrPacket_->os.write_char_array(str + addtotalsize, ilen);
			/// update the new write start point after witting
			addtotalsize += ilen;
			/// update the actual written-len
			len -= ilen;
		}
		return *this;
	}

	void dumpMsgs();
};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif
