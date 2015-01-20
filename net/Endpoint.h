#ifndef EndPoint_H_
#define EndPoint_H_

#include "ace\pre.h"

#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"
#include "ace/Log_Msg.h"
#include "ace/OS_NS_unistd.h"
#include "ace\SOCK_Dgram.h"
#include "common\common.h"
#include "net_common.h"
#include "Address.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL
/*
UDP SOCK:
EndPoint<udp> sock(20000, "127.0.0.1");
sock.int();

TCPSock：
EndPoint<udp> listensock(20000, "127.0.0.1");
sock.int();
listensock.


sock.int();
*/
template <typename SOCK_TYPE>
class EndPoint  // pool obj
{
	protected:
	Address localAddr;
	Address remoteAddr;
	typedef  typename SOCK_TYPE_TRAITS<SOCK_TYPE>::sock_type KBESOCKET;
	KBESOCKET mSockIO;
	//ACE_SOCK_Stream mSock; //default handle = invalid handle tcp
	//ACE_SOCK_Dgram mDrgram; //udp
	//ACE_SOCK_IO* mSockIO;

	public:
	EndPoint(const u_short port, const char host[ ]) : localAddr(port, host)
	{
	}

	void init()
	{
		init((SOCK_TYPE*) 0);
	}

	void init(TCP_TYPE* tcp)
	{
		ACE_DEBUG(( LM_DEBUG, "init(TCP_TYPE tcp)\n" ));
	}

	void init(UDP_TYPE* udp)
	{
		ACE_DEBUG(( LM_DEBUG, "init(UDP_TYPE tcp)\n" ));
		mSockIO.open(localAddr);
	}

	virtual ~EndPoint() { mSockIO.close(); }

	///setters and getters
	const KBESOCKET& SOCK() const { return mSockIO; }
	void  SOCK(const KBESOCKET& stream) { mSockIO = stream; }
	const Address& LocalAddr() const { return localAddr; }
	void LocalAddr(const Address& add) { localAddr = add; }
	const Address& RemoteAddr() const { return remoteAddr; }
	void RemoteAddr(const Address& add) { remoteAddr = add; }

	/// str dump adresses
	void RemoteAddrStr(std::string& str)
	{
		str = remoteAddr.c_str();
	}
	char*  RemoteAddrStr()
	{
		return remoteAddr.c_str();
	}
	void LocalAddrStr(std::string& str)
	{
		str = localAddr.c_str();
	}
	char*  LocalAddrStr()
	{
		return localAddr.c_str();
	}


	/// check
	ACE_HANDLE detach()
	{
		return detach((SOCK_TYPE*) 0);
	}

	ACE_HANDLE detach(UDP_TYPE* tcp)
	{
		return ACE_INVALID_HANDLE;
	}

	ACE_HANDLE detach(TCP_TYPE* tcp)
	{
		ACE_HANDLE ret = mSockIO.get_handle();
		mSockIO.set_handle(ACE_INVALID_HANDLE);
		return ret;
	}
	bool good() const { return mSockIO.get_handle() != ACE_INVALID_HANDLE; }


	/// socket optitons
	int setnonblocking(bool nonblocking)
	{
#if KBE_PLATFORM == PLATFORM_WIN32
		u_long val = nonblocking ? 1 : 0;
		return ::ioctlsocket(reinterpret_cast <SOCKET>( mSockIO.get_handle() ), FIONBIO, &val);
#else
		int val = nonblocking ? ACE_NONBLOCK : 0;
		ACE_OS::fcntl(mSockIO.get_handle(), F_SETFL, val);
#endif
	}

	int setbroadcast(bool broadcast)
	{
		return mSockIO.set_option(SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(bool));
	}

	int setreuseaddr(bool reuseaddr)
	{
		return mSockIO.set_option(SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(bool));
	}
	int setkeepalive(bool keepalive)
	{
		return mSockIO.set_option(SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(bool));
	}
	int setnodelay(bool nodelay = true)
	{
		return mSockIO.set_option(IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(bool));
	}


	///multi group
	int joinMulticastGroup(unsigned long networkAddr)
	{
#if KBE_PLATFORM == PLATFORM_WIN32
		return -1;
#else //unix
		struct ip_mreqn req;
		req.imr_multiaddr.s_addr = networkAddr;
		req.imr_address.s_addr = INADDR_ANY;
		req.imr_ifindex = 0;
		return ::setsockopt(socket_, SOL_IP, IP_ADD_MEMBERSHIP, &req, sizeof(req));
#endif
	}
	int quitMulticastGroup(unsigned long networkAddr)
	{
#if KBE_PLATFORM == PLATFORM_WIN32
		return -1;
#else //unix
		struct ip_mreqn req;
		req.imr_multiaddr.s_addr = networkAddr;
		req.imr_address.s_addr = INADDR_ANY;
		req.imr_ifindex = 0;
		return ::setsockopt(socket_, SOL_IP, IP_DROP_MEMBERSHIP, &req, sizeof(req));
#endif
	}

	/// cast operator
	operator KBESOCKET() const
	{
		return mSockIO;
	}
};



ACE_KBE_END_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_END_DECL
#endif