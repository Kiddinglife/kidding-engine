/**
* Reviewed by Jackie Zhang on 21/07/2015
*/

#ifndef ResourceManager_h_
#define ResourceManager_h_

#include "ace/pre.h"
#include "ace/Mutex.h"
#include "ace/Event_Handler.h"
#include "common/common.h"
#include "ResourceObject.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

#define RESOURCE_NORMAL	 0x00000000
#define RESOURCE_RESIDENT   0x00000001
#define RESOURCE_READ          0x00000002
#define RESOURCE_WRITE        0x00000004
#define RESOURCE_APPEND     0x00000008

struct ResourceManager : public ACE_Event_Handler
{
	// 引擎环境变量
	struct KBEEnv
	{
		std::string root;
		std::string res_path;
		std::string bin_path;
	};

	static ACE_UINT64 respool_timeout;
	static ACE_UINT32 respool_buffersize;
	static ACE_UINT32 respool_checktick;

	bool isInit_;
	KBEEnv kb_env_;
	ACE_Thread_Mutex mutex_;
	std::vector<std::string> respaths_;
	UnorderedMap< std::string, ResourceObject > respool_;

	ResourceManager();
	virtual ~ResourceManager();

	virtual int handle_timeout(const ACE_Time_Value &current_time, const void * data = 0);

	bool initialize_watchers();

	/// 从资源路径中(环境变量中指定的)匹配到完整的资源地址
	/// get resource complete path based on the given resource name
	std::string get_res_path(const std::string& res_name) { return get_res_path(res_name.c_str()); };
	std::string get_res_path(const char* res_name);
};

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include <ace/post.h>
#endif
