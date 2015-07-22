#include "ResourceManager.h"
#include "common/Watcher.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

ResourceObject::ResourceObject(const char* res, ACE_UINT32 flags) :
res_name_(res),
flags_(flags),
invalid_(false),
timeout_(0)
{
	update();
}

ResourceObject::~ResourceObject()
{
	if( ResourceManager::respool_timeout > 0 )
	{
		ACE_DEBUG(( LM_DEBUG, "ResourceObject::~ResourceObject(): {%s}\n", res_name_ ));
	}
}

void ResourceObject::update()
{
	timeout_ = timestamp() + ResourceManager::respool_timeout * stampsPerSecond();
}

bool ResourceObject::valid() const
{
	return invalid_ || timestamp() < timeout_;
}

FileObject::FileObject(const char* res, ACE_UINT32 flags, const char* model) :
ResourceObject(res, flags)
{
	fd_ = ACE_OS::fopen(res, model);
	if( fd_ == NULL )
	{
		invalid_ = true;
		ACE_DEBUG(( LM_ERROR,
			"FileObject::FileObject(): open({%s}) {%s} error!\n", model, res_name_ ));
	}
}

FileObject::~FileObject()
{
	ACE_OS::fclose(fd_);
}

bool FileObject::seek(ACE_UINT32 idx, int flags)
{
	if( invalid_ || fd_ == NULL )
	{
		ACE_ERROR_RETURN(( LM_ERROR, "FileObject::seek: { %s } invalid!\n", res_name_ ), false);
	}
	update();
	return ACE_OS::fseek(fd_, idx, flags) != -1;
}

ACE_UINT32 FileObject::read(char* buf, ACE_UINT32 cnt)
{
	if( invalid_ || fd_ == NULL )
	{
		ACE_ERROR_RETURN(( LM_ERROR, "FileObject::read: {%s} invalid!\n", res_name_ ), 0);
	}
	update();
	return ACE_OS::fread(buf, sizeof(char), cnt, fd_);
}

ACE_UINT32 FileObject::tell()
{
	if( invalid_ || fd_ == NULL )
	{
		ACE_ERROR_RETURN(( LM_ERROR, "FileObject::tell: {%s} invalid!\n", res_name_ ), 0);
	}
	update();
	return ACE_OS::ftell(fd_);
}

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
	TRACE("ResourceManager::get_res_path");
	FILE * f = NULL;
	std::string fpath;
	std::vector<std::string>::iterator iter = respaths_.begin();

	for( ; iter != respaths_.end(); ++iter )
	{
		fpath = ( ( *iter ) + res_name );

		STRUTIL::kbe_replace(fpath, "\\", "/");
		STRUTIL::kbe_replace(fpath, "//", "/");

		f = ACE_OS::fopen(fpath.c_str(), "r");
		if( f != NULL )
		{
			fclose(f);
			TRACE_RETURN(fpath);
			//return fpath;
		}
	}
	TRACE_RETURN(res_name);
	//return res_name;
}

const bool ResourceManager::if_res_exist(const std::string& res)
{
	FILE * f = NULL;
	std::string fpath;
	std::vector<std::string>::iterator iter = respaths_.begin();

	for( ; iter != respaths_.end(); ++iter )
	{
		fpath = ( ( *iter ) + res );

		STRUTIL::kbe_replace(fpath, "\\", "/");
		STRUTIL::kbe_replace(fpath, "//", "/");

		f = ACE_OS::fopen(fpath.c_str(), "r");
		if( f != NULL )
		{
			fclose(f);
			return true;
		}
	}

	return false;
}

FILE* ResourceManager::open_res(const std::string res, const char* mode)
{
	return nullptr;
}

ACE_KBE_END_VERSIONED_NAMESPACE_DECL