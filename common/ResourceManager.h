#ifndef ResourceManager_h_
#define ResourceManager_h_

#include "ace/pre.h"
#include "ace/Event_Handler.h"
#include "common/common.h"
#include "ResourceObject.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

#define RESOURCE_NORMAL	0x00000000
#define RESOURCE_RESIDENT 0x00000001
#define RESOURCE_READ 0x00000002
#define RESOURCE_WRITE 0x00000004
#define RESOURCE_APPEND 0x00000008

// 引擎环境变量
struct KBEEnv
{
	std::string root;
	std::string res_path;
	std::string bin_path;
};

struct ResourceManager : public ACE_Event_Handler
{
	static ACE_UINT64 respool_timeout;
	static ACE_UINT32 respool_buffersize;
	static ACE_UINT32 respool_checktick;

	bool isInit_;
	KBEEnv kb_env_;
	std::vector<std::string> respaths_;
	UnorderedMap< std::string, ResourceObject > respool_;

	ResourceManager();
	virtual ~ResourceManager();

	virtual int handle_timeout(const ACE_Time_Value &current_time, const void * data = 0);
};

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include <ace/post.h>
#endif
