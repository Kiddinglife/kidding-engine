#ifndef ResourceManager_h_
#define ResourceManager_h_

#include "ace/pre.h"
#include "common/common.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

#define RESOURCE_NORMAL	0x00000000
#define RESOURCE_RESIDENT 0x00000001
#define RESOURCE_READ 0x00000002
#define RESOURCE_WRITE 0x00000004
#define RESOURCE_APPEND 0x00000008

struct ResourceManager
{
	ResourceManager();
	virtual ~ResourceManager();
};

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include <ace/post.h>
#endif
