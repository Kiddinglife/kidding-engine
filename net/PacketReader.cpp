#include "PacketReader.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

PacketReader::PacketReader(Channel* pChannel)
:
pFragments(NULL), //pFragmentDatas_
pFragmentsWpos_(0), //pFragmentDatasWpos_;
pFragmentsRemainning(0), //pFragmentDatasRemain_;
fragmentsFlag_(FRAGMENT_DATA_UNKNOW), //fragmentDatasFlag_
pChannel_(pChannel),
pFragmentPacket_(NULL), //pFragmentStream_
currMsgID_(0),
currMsgLen_(0)
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
	TRACE_RETURN_VOID();
}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL