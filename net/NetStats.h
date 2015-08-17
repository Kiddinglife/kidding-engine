/**
* Reviewed by Jackie Zhang on 17/08/2015
*/
#ifndef NetStats_H_
#define NetStats_H_

#include "ace\pre.h"
#include "net_common.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

struct Message;
struct MsgTraceHandler;

/* 此类接口用于监听消息跟踪事件 */
struct MsgTraceHandler
{
	virtual void onSendMessage(const Message* msg, int size) = 0;
	virtual void onRecvMessage(const Message*, int size) = 0;
};

/// this struct is used to represent and store the stat of msg
struct Stat
{
	std::string name;
	ACE_UINT32 send_size;
	ACE_UINT32 send_count;
	ACE_UINT32 recv_size;
	ACE_UINT32 recv_count;

	Stat()
	{
		name = "None";
		send_count = 0;
		send_size = 0;
		recv_size = 0;
		recv_count = 0;
	}
};

/**
 * @Brief
 * 该类主要用于记录network流量等信息
 * 一个消息可能会有多余一个handler来记录与该消息相关的不同方面的数据
 * 例如，发送流量或者是发送频率，这些handler由使用者提供。
 * This class is used to record nwtwork statastics such as network usage and so on
 * a msg can have more than one handler to record the different kinds of stats related to the mg
 * for example, sending amounts or sending frequency, these handlers are provided by the user
 */
struct NetStats
{
	/// sending msg will call onSendMsg() to record stats
	/// receiving msg will call onRecvMsg() to record stats
	enum OPTION { SEND, RECV };

	/// stats container
	typedef UnorderedMap<std::string, Stat> Stats;
	Stats stats_;

	/// msg handler container, it has more than one trace handler to trace different stats 
	/// for all the msg recv and sent
	std::vector<MsgTraceHandler*> handlers_;

	NetStats() : stats_(), handlers_() { }
	~NetStats() { }

	/**
	 * @Brief
	 * 该方法会更新该消息的stats，之后会调用所有的回掉函数来记录相关的网络数据
	 * This method eill update the stats of the msg and then call all callbacks to
	 * record the related network stats.
	 * @para OPTION op
	 * is this sent msg or received msg 发送的消息还是接受的消息
	 * @para Message* msg
	 * msg pointer 消息指针
	 * @para ACE_UINT32 size
	 * the length of this msg including all fields in it
	 * 该消息的总长度包括包含所有的域
	 * @ret void
	 */
	void trackMessage(OPTION op, Message* msg, ACE_UINT32 size);

	/// add and remove msg trace handler
	void addHandler(MsgTraceHandler* pHandler);
	void removeHandler(MsgTraceHandler* pHandler);
};
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif