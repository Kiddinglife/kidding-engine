#ifndef FixedMessages_H_
#define FixedMessages_H_

#include "ace\pre.h"
#include "net_common.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL
/**
* FixedMessages
* 用来对中间协议(前端与后端之间)进行强制约定。
* 没有使用到kbe整套协议自动绑定机制的前端可以使用此处来强制约定协议。
*/
struct FixedMessages
{
	struct MSGInfo
	{
		MessageID msgid;
	};
	
	typedef std::tr1::unordered_map<std::string, MSGInfo> MSGINFO_MAP;
	MSGINFO_MAP infomap_;
	bool loaded_;

	FixedMessages() : infomap_(), loaded_(false)
	{

	}
	~FixedMessages()
	{
		infomap_.clear();
	}
	//@todo
	bool loadConfig(std::string fileName);
	//@done
	FixedMessages::MSGInfo* isFixed(const std::string& msgName);
	bool isFixed(MessageID msgid);
};

#define FixedMessagesPtrGetter(FixedMessagesName) \
FixedMessages* FixedMessagesName = \
ACE_Singleton<FixedMessages, ACE_Null_Mutex>::instance();

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif