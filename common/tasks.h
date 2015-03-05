#ifndef KBE_TASKS_HPP
#define KBE_TASKS_HPP
#include "ace\pre.h"
#include "common/task.h"
#include "common\common.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
NETWORK_NAMESPACE_BEGIN_DECL

/**
*	ÈÎÎñÈÝÆ÷
*/
class Tasks
{
	public:
	Tasks() : container_() { }
	~Tasks() { }

	void add(Task * pTask)
	{
		container_.push_back(pTask);
	}
	bool cancel(Task * pTask)
	{
		Container::iterator iter = std::find(container_.begin(), container_.end(), pTask);
		if( iter != container_.end() )
		{
			container_.erase(iter);
			return true;
		}

		return false;
	}
	void process()
	{
		static Container::iterator iter = container_.begin();
		iter = container_.begin();
		while( iter != container_.end() )
		{
			static Task * pTask = *iter;
			pTask = *iter;
			if( !pTask->process() )
				iter = container_.erase(iter);
			else
				++iter;
		}
	}
	private:

	typedef std::vector<Task *> Container;
	Container container_;
};

NETWORK_NAMESPACE_END_DECL
ACE_KBE_END_VERSIONED_NAMESPACE_DECL
#endif // KBE_TASKS_HPP
