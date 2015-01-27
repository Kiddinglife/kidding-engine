#ifndef NetworkInterface_H_
#define NetworkInterface_H_

#include "ace\pre.h"
#include "ace/SOCK_Acceptor.h"
#include "net_common.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

const int RECV_BUFFER_SIZE = 16 * 1024 * 1024; // 16MB
const char* const USE_KBEMACHINED = "kbemachine";

struct Bundle;
struct Channel;
struct DelayedChannels;
struct ChannelTimeOutHandler;
struct ChannelDeregisterHandler;
struct Packet;
struct Nub;
struct Messages;
struct TCP_Acceptor_Handler;
struct UDP_SOCK_Handler;
struct DelayedChannelHandlers;

struct NetworkInterface
{
	typedef std::map<ACE_INET_Addr, Channel*>	ChannelMap;
	ChannelMap                                                    channelMap_;

	/// Acceptor TCP SOCK
	//ACE_SOCK_Acceptor                                         extEndpoint_;

	/// UDP SOCK
	//ACE_SOCK_DGRAM                                          intEndpoint_;

	///事件分发
	Nub*                                                                nub_;

	/// 数据指针
	void*                                                                pExtensionData_;

	/// 
	TCP_Acceptor_Handler*                                    pExtListenerReceiver_;
	UDP_SOCK_Handler*                                        pIntListenerReceiver_;

	DelayedChannelHandlers* 						        pDelayedChannels_;
	/// 超时的通道可被这个句柄捕捉， 例如告知上层client断开
	ChannelTimeOutHandler*					                pChannelTimeOutHandler_;
	ChannelDeregisterHandler*				                pChannelDeregisterHandler_;

	const bool								                        isExternal_;
	ACE_INT32									                    numExtChannels_;

	NetworkInterface(
		Nub*              pDispatcher = NULL,
		ACE_INT16     extlisteningPort_min = 0,
		ACE_INT16     extlisteningPort_max = 0,
		const char *    extlisteningInterface = "NONE",
		ACE_UINT32   extrbuffer = 0,
		ACE_UINT32   extwbuffer = 0,
		ACE_INT16      intlisteningPort = 0,
		const char *    intlisteningInterface = "NONE",
		ACE_UINT32   intrbuffer = 0,
		ACE_UINT32   intwbuffer = 0);

	~NetworkInterface();

	/**
	* This method will extacrt the host or domain name and ip adress based on the given string
	*
	* @param {in} spec ip address string such as 192.168.2.5 127.0.0.1 and so on
	*
	* @param {out} host name or domain name such as localhost or eth0
	* @ret bool true if success false if fail
	*
	* @responsibility the caller needs to ensure @param spec like ip addr host name  does exist
	*
	* gethostbyname()
	* return ip adrr given host name or domain name
	* 通过域名或者主机命返回IP地址. 传进去的参数是一个域名或者主机名.
	* 返回值是一个Hostent指针结构.(如传进去的是一个空字符串,那么返回的是本机的主机名与IP地址)
	*
	* gethostname()
	* get host name or domain name
	* 得到本机主机名或者域名.有两个参数,一个是用来存放主机名或者域名的变量,一个是缓冲区的大小.
	*/
	bool is_ip_addr_valid(const char* spec, char* name);
};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif