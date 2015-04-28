#include "ResourceManager.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

ResourceManager::ResourceManager()
{
}


ResourceManager::~ResourceManager()
{
}

int ResourceManager::handle_timeout(const ACE_Time_Value &current_time, const void * data)
{
	return 0;
}

ACE_KBE_END_VERSIONED_NAMESPACE_DECL