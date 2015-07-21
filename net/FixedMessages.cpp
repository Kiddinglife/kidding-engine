#include "FixedMessages.h"
#include "common\xml.h"
#include "common\ResourceManager.h"

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

	if( loaded_ ) return true;

	ResourceManager* instance = ACE_Singleton<ResourceManager, ACE_Null_Mutex>::instance();
	const char* name = instance->get_res_path(fileName).c_str();

	TiXmlNode* node = NULL;
	TiXmlNode* rootNode = NULL;

	XML* xml = new XML(name);
	if( !xml->isGood() )
	{
		loaded_ = false;
		ACE_ERROR_RETURN(( LM_ERROR,
			"[ERROR]: FixedMessages::loadConfig: load { %s } is failed!\n", fileName.c_str() ), false);
	} else
	{
		loaded_ = true;
	}

	rootNode = xml->getRootNode();
	if( rootNode != NULL )
	{
		XML_FOR_BEGIN(rootNode)
		{
			node = xml->enterNode(rootNode->FirstChild(), "id");

			FixedMessages::MSGInfo info;
			info.msgid = xml->getValInt(node);
			info.msgname = xml->getKey(rootNode);

			infomap_[info.msgname] = info;
		}
		XML_FOR_END(rootNode);
	} else
	{
		// root节点下没有子节点了
		return true;
	}

	delete xml;

	TRACE_RETURN(true);
}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL