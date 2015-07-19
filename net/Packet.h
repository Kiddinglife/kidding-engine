#ifndef Packet_H_
#define Packet_H_

#include "ace\pre.h"
#include "ace/CDR_Stream.h"
#include "net_common.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

/**
* struct Packet
*
* @Brief
* This struct represents some memery buffer
* that consists some msgs in the form of binary bytes
* 本质上包就是一段内存缓冲，用于装载一个或者多个二进制的消息
*
* @Notes
* @config.h file in ace
* os and is will not only align the buffer start osition,
* but also the bytes stored in the buffer and so we have to
* setup this macro to stop automatic alignment including start
* position alignment and bytes alignment inside the buffer
* #define ACE_LACKS_CDR_ALIGNMENT
*/
struct Packet
{
	ACE_OutputCDR os;
	ACE_Message_Block* osbuff_;

	ACE_InputCDR in;
	ACE_Message_Block* inbuff_;

	MessageID msgID_;
	Bundle* pBundle_;
	ACE_UINT32 sentSize;

	bool encrypted_;

	~Packet()
	{
		//ACE_DEBUG(( LM_DEBUG, "DEBUG::virtual ~Packet() called\n" ));
		reset();
	}

	/**
	 * when os and in are constructed, each of them has its own
	 * ACE_Message_Block member and ACE_Data_Block memeber.
	 * However, these two memembers points to the same buffer
	 * so sometimes you have to update each of the rd and wr
	 * positions when one of them are changed
	 *
	 * see CDR_Stream.cpp ACE_OutputCDR constructor line 26
	 * start_(( size ? size : (size_t) ACE_CDR::DEFAULT_BUFSIZE ) +
	 * ACE_CDR::MAX_ALIGNMENT, we can see ace automatically +
	 * 8 bytes and so we have to minium it actively.
	 */
	Packet(MessageID msgID = 0, ProtocolType pt = PROTOCOL_TCP) :
		os(pt == PROTOCOL_TCP ?
		PACKET_MAX_SIZE_TCP - ACE_CDR::MAX_ALIGNMENT : PACKET_MAX_SIZE_UDP - ACE_CDR::MAX_ALIGNMENT),
		osbuff_(const_cast<ACE_Message_Block*>( os.begin() )),
		in(osbuff_->base(), osbuff_->size()),
		inbuff_(const_cast<ACE_Message_Block*>( in.start() )),
		msgID_(msgID),
		encrypted_(false),
		pBundle_(NULL),
		sentSize(0)
	{
		//ACE_DEBUG(( LM_DEBUG, "DEBUG:: come Packet()\n" ));
		// ACE_DEBUG(( LM_DEBUG, "DEBUG:: leave Packet() \n" ));
	}

	/// This ctor  used in packetreader::processpackets to get a reasoanle
	/// size of packet to buffer the too long payload of a msg
	Packet(ACE_UINT32 SIZE) :
		os(SIZE - ACE_CDR::MAX_ALIGNMENT),
		osbuff_(const_cast<ACE_Message_Block*>( os.begin() )),
		in(osbuff_->base(), osbuff_->size())
	{
	}

	void reset(void)
	{
		// @Deprecated
		// Reset all of them in the next time in the ctor from pool ctor() instead of here
		//encrypted_ = false;
		//sentSize = 0;
		//msgID_ = 0;
		//pBundle_ = NULL;

		// It is tempting not to remove the memory, but we need to do so 
		// to release any potential user buffers chained in the continuation 
		// field. however we do not need reset os because we do not use
		// any cont memeory by os.reset(). so we only need to reset the osbuff_ 
		// by osbuff_->reset()
		osbuff_->reset();
	}

	/// Advance the rd position in odbuffer_ of this packet to show the read is done
	void on_read_packet_done() const { this->osbuff_->rd_ptr(this->osbuff_->wr_ptr()); }

	/// Length is (wr_ptr - rd_ptr).
	const size_t length() const { return osbuff_->length(); }

	/// Empty means wr_ptr = rd_ptr
	const bool empty() const { return length() > 0; }

	const size_t recv(const ACE_SOCK_Stream& dgram)
	{
		size_t recv_cnt = dgram.recv_n(osbuff_->wr_ptr(), osbuff_->space());
		osbuff_->wr_ptr(recv_cnt);
		inbuff_->wr_ptr(recv_cnt);
		return recv_cnt;
	}

	const size_t recv(const ACE_SOCK_Dgram& dgram, ACE_INET_Addr& remote_addr)
	{
		size_t recv_cnt = dgram.recv(osbuff_->wr_ptr(), osbuff_->space(), remote_addr);
		osbuff_->wr_ptr(recv_cnt);
		inbuff_->wr_ptr(recv_cnt);
		return recv_cnt;
	}
};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif

