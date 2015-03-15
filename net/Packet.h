#ifndef Packet_H_
#define Packet_H_

#include "ace\pre.h"
#include "ace/CDR_Stream.h"
#include "net_common.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

struct Packet
{
	ACE_OutputCDR os;
	ACE_Message_Block* osbuff_;

	ACE_InputCDR in;
	ACE_Message_Block* inbuff_;

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
		osbuff_(const_cast<ACE_Message_Block*>( os.begin() )),
		in(osbuff_->base(), osbuff_->size()),
		inbuff_(const_cast<ACE_Message_Block*>( in.start() ))
	{
		//ACE_DEBUG(( LM_DEBUG, "DEBUG:: come Packet()\n" ));
		msgID_ = msgID;
		encrypted_ = false;
		pBundle_ = NULL;
		sentSize = 0;
		//ACE_DEBUG(( LM_DEBUG, "DEBUG:: leave Packet() \n" ));
	}

	/// THIS ctor us used in packetreader::processpackets to get a reasoanle size of 
	/// packet to hold the too long payload of a msg
	Packet(ACE_UINT32 SIZE) :
		os(SIZE),
		osbuff_(const_cast<ACE_Message_Block*>( os.begin() ))
		, in(osbuff_->base(), osbuff_->size())
	{
	}

	void reset(void)
	{
		// It is tempting not to remove the memory, 
		//but we need to do so to release any potential user buffers chained in the continuation field.
		// we do not need reset os because we do not use any cont memeory
		//os.reset();
		osbuff_->reset();

		/// will reset akk of them in the next time in the ctor from pool ctor()
		//encrypted_ = false;
		//sentSize = 0;
		//msgID_ = 0;
		//pBundle_ = NULL;
	}

	/* Make the read ptr = write ptr to show the read is done */
	void on_read_packet_done()
	{
		/// advance the rd position in this packet
		this->osbuff_->rd_ptr(this->osbuff_->wr_ptr());
	}

	const size_t length() const { return osbuff_->length(); }
	const bool empty() const { return length() > 0; }

	size_t recv(const ACE_SOCK_Stream& dgram)
	{
		size_t recv_cnt = dgram.recv_n(osbuff_->wr_ptr(), osbuff_->end() - osbuff_->wr_ptr());
		osbuff_->wr_ptr(osbuff_->wr_ptr() + recv_cnt);
		return recv_cnt;
	}
	size_t recv(const ACE_SOCK_Dgram& dgram, ACE_INET_Addr& remote_addr)
	{
		size_t recv_cnt = dgram.recv(osbuff_->wr_ptr(), osbuff_->space(), remote_addr);
		osbuff_->wr_ptr(osbuff_->wr_ptr() + recv_cnt);
		return recv_cnt;
	}
};

inline extern ACE_Export ACE_CDR::Boolean operator>> ( ACE_InputCDR &is,
	std::string& value )
{
	TRACE("string >>");
	value.clear();
	static char c;
	while( is.length() > 0 )
	{
		in.read_char(c);
		if( c == 0 )
			break;
		value.push_back(c);
	}
	TRACE_RETURN(is.good_bit());
}

inline extern ACE_Export ACE_CDR::Boolean operator<< ( ACE_OutputCDR &os,
	std::string& str )
{
	TRACE("string <<");
	str.length() ? os.write_char_array(str.c_str(), str.length()) : os.write_char(0);
	TRACE_RETURN(os.good_bit());
}


inline extern ACE_Export ACE_CDR::Boolean operator>> ( ACE_InputCDR &in,
	ACE_CDR::Char* str )
{
	TRACE("char* >>");
	static char c;
	while( in.length() > 0 )
	{
		in.read_char(c);
		if( c == 0 )
			break;

		*( str++ ) = c;
	}
	*str = '\0';
	TRACE_RETURN(in.good_bit());
}

inline extern ACE_Export ACE_CDR::Boolean operator<< ( ACE_OutputCDR &os,
	const ACE_CDR::Char* x )
{
	TRACE("char* <<");
	x ? os.write_char_array(x, strlen(x) + 1) : os.write_char(0);
	TRACE_RETURN(os.good_bit());
}

inline extern ACE_Export ACE_CDR::Boolean operator>> ( ACE_InputCDR &in,
	ACE_CDR::Octet& value )
{
	TRACE("ACE_CDR::Octet value >>");
	ACE_InputCDR::to_octet v(value);
	in >> v;
	TRACE_RETURN(in.good_bit());
}

inline extern ACE_Export ACE_CDR::Boolean operator<< ( ACE_OutputCDR &os,
	ACE_CDR::Octet value )
{
	TRACE("ACE_CDR::Octet value  <<");
	ACE_OutputCDR::from_octet v(value);
	os << v;
	TRACE_RETURN(os.good_bit());
}

inline extern ACE_Export ACE_CDR::Boolean operator>> ( ACE_InputCDR &in,
	KBE_SRV_COMPONENT_TYPE& x )
{
	TRACE("KBE_SRV_COMPONENT_TYPE* >>");
	static ACE_CDR::Octet  v = 0;
	in >> v;
	x = (KBE_SRV_COMPONENT_TYPE) v;
	TRACE_RETURN(in.good_bit());
}

inline extern ACE_Export ACE_CDR::Boolean operator<< ( ACE_OutputCDR &os,
	KBE_SRV_COMPONENT_TYPE x )
{
	TRACE("KBE_SRV_COMPONENT_TYPE* <<");
	os << ( ACE_CDR::Octet ) x;
	TRACE_RETURN(os.good_bit());
}

inline extern ACE_Export ACE_CDR::Boolean operator>> ( ACE_InputCDR &in,
	ACE_CDR::Boolean& value )
{
	TRACE("KBE_SRV_COMPONENT_TYPE* >>");
	ACE_InputCDR::to_boolean v(value);
	in >> v;
	TRACE_RETURN(in.good_bit());
}

inline extern ACE_Export ACE_CDR::Boolean operator<< ( ACE_OutputCDR &os,
	ACE_CDR::Boolean value )
{
	TRACE("KBE_SRV_COMPONENT_TYPE* <<");
	ACE_OutputCDR::from_boolean v(value);
	os << v;
	TRACE_RETURN(os.good_bit());
}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif

