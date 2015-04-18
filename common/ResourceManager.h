#ifndef ResourceManager_h_
#define ResourceManager_h_

#include "ace/pre.h"
#include "common/common.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

struct ResourceManager
{
	ResourceManager();
	virtual ~ResourceManager();
};

ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#include <ace/post.h>
#endif
