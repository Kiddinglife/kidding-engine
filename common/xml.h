/*
xml 读写：
例子:
<root>
<server>
<ip>172.16.0.12</ip>
<port>6000</port>
</server>
</root>
--------------------------------------------------------------------------------
XML* xml = new XML("KBEngine.xml");
TiXmlNode* node = xml->getRootNode("server");

XML_FOR_BEGIN(node)
{
printf("%s--%s\n", xml->getKey(node).c_str(), xml->getValStr(node->FirstChild()).c_str());
}
XML_FOR_END(node);

delete xml;
输出:
---ip---172.16.0.12
---port---6000


例子2:
XML* xml = new XML("KBEngine.xml");
TiXmlNode* serverNode = xml->getRootNode("server");

TiXmlNode* node;
node = xml->enterNode(serverNode, "ip");
printf("%s\n", xml->getValStr(node).c_str() );

node = xml->enterNode(serverNode, "port");
printf("%s\n", xml->getValStr(node).c_str() );

输出:
172.16.0.12
6000
*/
#ifndef Address_H_
#define Address_H_

#include "ace\pre.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include "tinyxml\tinyxml.h"
#include "common\common.h"
#include "common\strulti.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

// windows include	
#if KBE_PLATFORM == PLATFORM_WIN32
#else
// linux include
#endif

#define XML_FOR_BEGIN(node) \
do{if(node->Type() != TiXmlNode::TINYXML_ELEMENT)	 continue;				

#define XML_FOR_END(node)     } while((node = node->NextSibling()));         

struct  XML : public Intrusive_Auto_Ptr
{
	TiXmlDocument* txdoc_;
	TiXmlElement* rootElement_;
	bool isGood_;

	XML(void) :
		txdoc_(NULL),
		rootElement_(NULL),
		isGood_(false)
	{
	}

	XML(const char* xmlFile) :
		txdoc_(NULL),
		rootElement_(NULL),
		isGood_(false)
	{
		openSection(xmlFile);
	}

	~XML(void)
	{
		if( txdoc_ )
		{
			txdoc_->Clear();
			delete txdoc_;
			txdoc_ = NULL;
			rootElement_ = NULL;
		}
	}

	bool isGood()const { return isGood_; }

	bool openSection(const char* xmlFile)
	{
		char pathbuf[MAX_PATH];
		kbe_snprintf(pathbuf, MAX_PATH, "%s", xmlFile);

		txdoc_ = new TiXmlDocument((char*) &pathbuf);

		if( !txdoc_->LoadFile() )
		{
#if KBE_PLATFORM == PLATFORM_WIN32
			printf("TiXmlNode::openXML: {%s}, is error!\n", pathbuf);
#endif
			//if( DebugHelper::isInit() )
			//{
			ACE_DEBUG(( LM_ERROR, "TiXmlNode::openXML: {%s}, is error!\n", pathbuf ));
			//	}

			isGood_ = false;
			return false;
		}

		rootElement_ = txdoc_->RootElement();
		isGood_ = true;
		return true;
	}

	/**获取根元素*/
	TiXmlElement* getRootElement(void) { return rootElement_; }

	/**获取根节点， 带参数key为范围根节点下的某个子节点根*/
	TiXmlNode* getRootNode(const char* key = "")
	{
		if( rootElement_ == NULL )
			return rootElement_;

		if( strlen(key) > 0 )
		{
			TiXmlNode* node = rootElement_->FirstChild(key);
			if( node == NULL )
				return NULL;
			return node->FirstChild();
		}
		return rootElement_->FirstChild();
	}

	/**直接返回要进入的key节点指针*/
	TiXmlNode* enterNode(TiXmlNode* node, const char* key)
	{
		do
		{
			if( node->Type() != TiXmlNode::TINYXML_ELEMENT )
				continue;

			if( getKey(node) == key )
				return node->FirstChild();

		} while( ( node = node->NextSibling() ) );

		return NULL;
	}

	/**是否存在这样一个key*/
	bool hasNode(TiXmlNode* node, const char* key)
	{
		do
		{
			if( node->Type() != TiXmlNode::TINYXML_ELEMENT )
				continue;

			if( getKey(node) == key )
				return true;

		} while( ( node = node->NextSibling() ) );

		return false;
	}

	TiXmlDocument* getTxdoc()const { return txdoc_; }

	std::string getKey(const TiXmlNode* node)
	{
		if( node == NULL )
			return "";
		std::string str(node->Value());
		STRUTIL::kbe_trim(str, " ");
		return str;
	}

	std::string getValStr(const TiXmlNode* node)
	{
		const TiXmlText* ptext = node->ToText();
		if( ptext == NULL )
			return "";
		//return strutil::kbe_trim(ptext->Value());
		std::string str(ptext->Value());
		STRUTIL::kbe_trim(str, " ");
		return str;
	}

	std::string getVal(const TiXmlNode* node)
	{
		const TiXmlText* ptext = node->ToText();
		if( ptext == NULL )
			return "";

		return ptext->Value();
	}

	int getValInt(const TiXmlNode* node)
	{
		const TiXmlText* ptext = node->ToText();
		if( ptext == NULL )
			return 0;
		//return atoi(StrUltil::kbe_trim(ptext->Value()).c_str());
		std::string str(ptext->Value());
		STRUTIL::kbe_trim(str, " ");
		return ACE_OS::atoi(str.c_str());
	}

	double getValFloat(const TiXmlNode* node)
	{
		const TiXmlText* ptext = node->ToText();
		if( ptext == NULL )
			return 0.f;
		//return atof(strutil::kbe_trim(ptext->Value()).c_str());
		std::string str(ptext->Value());
		STRUTIL::kbe_trim(str, " ");
		return ACE_OS::atof(str.c_str());
	}
};

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include "ace\post.h"
#endif