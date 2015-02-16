#ifndef Packet_H_
#define Packet_H_

#include "ace\pre.h"
#include "ace/CDR_Stream.h"
#include "net_common.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

struct Bundle;
struct Packet
{
	ACE_OutputCDR os;
	ACE_Message_Block* buff;
	//ACE_InputCDR in;

	MessageID msgID_;
	Bundle* pBundle_;
	bool encrypted_;
	ACE_UINT32 sentSize;

	~Packet()
	{
		//ACE_DEBUG(( LM_DEBUG, "DEBUG::virtual ~Packet() called\n" ));
		reset();
	}

	Packet(MessageID msgID = 0, ProtocolType pt = PROTOCOL_TCP) :
		os(pt == PROTOCOL_TCP ?
		PACKET_MAX_SIZE_TCP - ACE_CDR::MAX_ALIGNMENT : PACKET_MAX_SIZE_UDP - ACE_CDR::MAX_ALIGNMENT),
		buff(const_cast<ACE_Message_Block*>( os.begin() ))
		/*,in((char*)NULL, 0)*/
	{
		//ACE_DEBUG(( LM_DEBUG, "DEBUG:: come Packet()\n" ));
		msgID_ = msgID;
		encrypted_ = false;
		pBundle_ = NULL;
		sentSize = 0;
		//const_cast<ACE_Message_Block*>( in.start() )->base(buff->base(), buff->size());
		//ACE_DEBUG(( LM_DEBUG, "DEBUG:: leave Packet() \n" ));
	}

	/// THIS ctor us used in packetreader::processpackets to get a reasoanle size of 
	/// packet to hold the too long payload of a msg
	Packet(ACE_UINT32 SIZE) :
		os(SIZE),
		buff(const_cast<ACE_Message_Block*>( os.begin() ))
	{
	}

	void reset(void)
	{
		// It is tempting not to remove the memory, 
		//but we need to do so to release any potential user buffers chained in the continuation field.
		// we do not need reset os because we do not use any cont memeory
		//os.reset();
		buff->reset();
		encrypted_ = false;
		sentSize = 0;
		msgID_ = 0;
		pBundle_ = NULL;
	}

	/* Make the read ptr = write ptr to show the read is done */
	void on_read_packet_done()
	{
		/// advance the rd position in this packet
		this->buff->rd_ptr(this->buff->wr_ptr());
	}

	const size_t length() const { return buff->length(); }
	const bool empty() const { return length() > 0; }

	size_t recv(const ACE_SOCK_Stream& dgram)
	{
		size_t recv_cnt = dgram.recv_n(buff->wr_ptr(), buff->end() - buff->wr_ptr());
		buff->wr_ptr(buff->wr_ptr() + recv_cnt);
		return recv_cnt;
	}
	size_t recv(const ACE_SOCK_Dgram& dgram, ACE_INET_Addr& remote_addr)
	{
		size_t recv_cnt = dgram.recv(buff->wr_ptr(), buff->space(), remote_addr);
		buff->wr_ptr(buff->wr_ptr() + recv_cnt);
		return recv_cnt;
	}
};
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif

