#include "FixedMessages.h"


ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL


const FixedMessages::MSGInfo* FixedMessages::isFixed(const std::string& msgName)
{
	MSGINFO_MAP::iterator iter = infomap_.find(msgName);
	return iter != infomap_.end() ? &iter->second : NULL;
}

const bool FixedMessages::isFixed(const MessageID msgid)
{
	MSGINFO_MAP::iterator iter = infomap_.begin();
	while( iter != infomap_.end() )
	{
		if( iter->second.msgid == msgid ) return true;
		++iter;
	}
	return false;
}

const bool FixedMessages::loadConfig(const std::string fileName)
{
	TRACE("FixedMessages::loadConfig()");
	TRACE_RETURN(true);
}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL