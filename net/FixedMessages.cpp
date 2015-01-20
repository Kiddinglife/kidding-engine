#include "FixedMessages.h"


ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

/**
 * @brief
 * if the msg with the specific name is stored in map, it is  fixed-len msg
 * \n
 * @param msgName
 *\n
 * @ret NULL if it is variable-len msg the pointer if it is fixed-len msg
 */
 FixedMessages::MSGInfo* FixedMessages::isFixed(const std::string& msgName)
{
	MSGINFO_MAP::iterator iter = infomap_.find(msgName);
	return iter != infomap_.end() ? &iter->second : NULL;
}

/**
* @brief
* if the msg with the specific msgid is stored in map, it is  fixed-len msg
*\n
* @param msgid
*\n
* @ret false if it is variable-len msg or true if it is fixed-len msg
*/
bool FixedMessages::isFixed(MessageID msgid)
{
	MSGINFO_MAP::iterator iter = infomap_.begin();
	while( iter != infomap_.end() )
	{
		FixedMessages::MSGInfo& infos = iter->second;
		if( infos.msgid == msgid )
			return true;
		++iter;
	}
	return false;
}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL