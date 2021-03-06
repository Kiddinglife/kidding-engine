﻿#include "ResourceManager.h"
#include "common/Watcher.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

ACE_UINT64 ResourceManager::respool_timeout = 0;
ACE_UINT32 ResourceManager::respool_buffersize = 0;
ACE_UINT32 ResourceManager::respool_checktick = 0;

ResourceManager::ResourceManager() :
kb_env_(),
respaths_(),
isInit_(false),
respool_(),
mutex_()
{
}

ResourceManager::~ResourceManager()
{
	respool_.clear();
}

int ResourceManager::handle_timeout(const ACE_Time_Value &current_time, const void * data)
{
	return 0;
}

bool ResourceManager::initialize_watchers()
{
	CRATE_WATCH_OBJECT("syspaths/KBE_ROOT", kb_env_.root);
	CRATE_WATCH_OBJECT("syspaths/KBE_RES_PATH", kb_env_.res_path);
	CRATE_WATCH_OBJECT("syspaths/KBE_BIN_PATH", kb_env_.bin_path);
	return true;
}

std::string ResourceManager::get_res_path(const char* res_name)
{
	std::vector<std::string>::iterator iter = respaths_.begin();

	for( ; iter != respaths_.end(); ++iter )
	{
		std::string fpath = ( ( *iter ) + res_name );

		STRUTIL::kbe_replace(fpath, "\\", "/");
		STRUTIL::kbe_replace(fpath, "//", "/");

		FILE * f = fopen(fpath.c_str(), "r");
		if( f != NULL )
		{
			fclose(f);
			return fpath;
		}
	}

	return res_name;
}

ACE_KBE_END_VERSIONED_NAMESPACE_DECL