#include "Address.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

char Address::s_stringBuf[2][32] = { { 0 }, { 0 } };
int Address::s_currStringBuf = 0;
const Address Address::NONE;

char * Address::nextStringBuf()
{
	s_currStringBuf = ( s_currStringBuf + 1 ) % 2;
	return s_stringBuf[s_currStringBuf];
}




char * Address::c_str() const
{
	char * buf = Address::nextStringBuf();

	//ACE_UINT32 ip = this->addr.get_ip_address();
	//u_short port = this->addr.get_port_number();
	//kbe_snprintf(buf, 32,
	//	"%d.%d.%d.%d:%d",
	//	ip >> 24,
	//	ip >> 16,
	//	ip >> 8,
	//	ip,
	//	port);

	this->addr.addr_to_string(buf, 32);
	return buf;
}

const char * Address::ipAsString() const
{
	char * buf = Address::nextStringBuf();

	ACE_UINT32 ip = this->addr.get_ip_address();
	kbe_snprintf(buf, 32,
		"%d.%d.%d.%d:%d",
		ip >> 24,
		ip >> 16,
		ip >> 8,
		ip);

	return buf;
}

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_END_DECL