#ifndef Address_H_
#define Address_H_

#include "ace\pre.h"
#include "common\common.h"
#include "net_common.h"
#include "ace/INET_Addr.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL
class Address 
{
	public:
	static const Address NONE;

	private:
	ACE_INET_Addr addr;

	static char s_stringBuf[2][32];
	static int s_currStringBuf;
	static char * nextStringBuf();

	public:
	Address() :addr() { }
	Address(const u_short port_number, const char host_name[ ]) :
		addr(port_number, host_name) { }

	Address(const char port_name[ ], const char host_name[ ]) :
		addr(port_name, host_name) { }

	virtual ~Address() { }

	operator char*( ) const { return this->c_str(); }

	char * c_str() const;
	const char * ipAsString() const;
	bool isNone() const { return this->addr.get_addr_size(); }

	public:
	ACE_INET_Addr& Addr()
	{
		return this->addr;
	}
	inline bool operator==( const Address & b )
	{
		return this->addr == b.addr;
	}

	inline bool operator!=( const Address & b )
	{
		return this->addr != b.addr;
	}

	inline bool operator<( const Address & b )
	{
		return this->addr < b.addr;
	}
	inline bool operator>(const Address & b)
	{
		return !( this->addr < b.addr ) && !( this->addr == b.addr );
	}
};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL

#endif
