#ifndef KBE_TASK_HPP
#define KBE_TASK_HPP
#include "ace\pre.h"
#include "common\common.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

/**
*	抽象一个任务
*/
class Task
{
	public:
	virtual ~Task() { }
	virtual bool process() = 0;
};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#endif // KBE_TASK_HPP
