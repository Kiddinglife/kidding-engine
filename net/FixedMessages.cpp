﻿#include "FixedMessages.h"
#include "common\xml.h"
#include "common\ResourceManager.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL


const FixedMessages::MSGInfo* FixedMessages::isFixed(const char* msgName)
{
	TRACE("FixedMessages::isFixed(const char* msgName)");
	MSGINFO_MAP::iterator iter = infomap_.find(msgName);
	TRACE_RETURN(iter != infomap_.end() ? &( iter->second ) : NULL);
}

const bool FixedMessages::isFixed(const MessageID msgid)
{
	TRACE("FixedMessages::isFixed(const MessageID msgid)");

	MSGINFO_MAP::iterator iter = infomap_.begin();
	while( iter != infomap_.end() )
	{
		if( iter->second.msgid == msgid ) return true;
		++iter;
	}

	TRACE_RETURN(false);
}

const bool FixedMessages::loadConfig(const std::string fileName)
{
	TRACE("FixedMessages::loadConfig()");

	if( loaded_ ) 	TRACE_RETURN(true);

	TiXmlNode* node = NULL;
	TiXmlNode* rootNode = NULL;
	ResourceManagerPtr(instance);
	XML xml(instance->get_res_path(fileName).c_str());

	if( !xml.isGood() )
	{
		loaded_ = false;
		ACE_ERROR_RETURN(( MY_ERROR
			"[ERROR]: FixedMessages::loadConfig: load { %s } is failed!\n",
			fileName.c_str() ),
			false);
	} else
	{
		loaded_ = true;
	}

	rootNode = xml.getRootNode();
	if( rootNode != NULL )
	{
		std::string msgname;
		XML_FOR_BEGIN(rootNode)
		{
			node = xml.enterNode(rootNode->FirstChild(), "id");
			msgname = xml.getKey(rootNode);
			infomap_[msgname].msgid = xml.getValInt(node);
			infomap_[msgname].msgname = msgname;
			infomap_[msgname].print();
		}
		XML_FOR_END(rootNode);
	} else
	{
		// root节点下没有子节点了
		TRACE_RETURN(true);
	}

	TRACE_RETURN(true);
}
NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL