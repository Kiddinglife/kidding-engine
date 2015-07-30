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
env_(),
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
	update_respool();
	return 0;
}

bool ResourceManager::initialize_watchers()
{
	CRATE_WATCH_OBJECT("syspaths/ZMD_ROOT", env_.root_path);
	CRATE_WATCH_OBJECT("syspaths/ZMD_RES_PATH", env_.all_res_paths);
	CRATE_WATCH_OBJECT("syspaths/ZMD_BIN_PATH", env_.bin_path);
	return true;
}

bool ResourceManager::init()
{
	// 获取引擎环境配置 get the env config
	env_.root_path = ACE_OS::getenv("ZMD_ROOT") == NULL ?
		"" : ACE_OS::getenv("KBE_ROOT");
	env_.all_res_paths =
		ACE_OS::getenv("ZMD_RES_PATH") == NULL ? "" : ACE_OS::getenv("KBE_RES_PATH");
	env_.bin_path =
		ACE_OS::getenv("ZMD_BIN_PATH") == NULL ? "" : ACE_OS::getenv("KBE_BIN_PATH");

	/**
	env_.root_path			    = "/home/kbengine/";
	env_.bin_path		= "/home/kbengine/kbe/bin/server/";
	env_.all_res_paths		=
	"/home/kbengine/kbe/res/;/home/kbengine/assets/;
	/home/kbengine/assets/scripts/;
	/home/kbengine/assets/res/";
	*/

	adjust_paths();
	return false;
}

void ResourceManager::adjust_paths()
{
	char ch;

	///// root_path 是一个非空的字符串 //////
	// root_path is not a null string
	if( env_.root_path.size() > 0 )
	{
		// 得到末尾字符， 看情况是否需要添加 "/"
		// get the last char to see if it is needed to add "/"
		ch = env_.root_path.at(env_.root_path.size() - 1);
		if( ch != '/' && ch != '\\' ) env_.root_path += "/";

		// 将所有的 \\ 替换为 /
		// repalce all \\ to /
		STRUTIL::kbe_replace(env_.root_path, "\\", "/");
		STRUTIL::kbe_replace(env_.root_path, "//", "/");

		ACE_DEBUG(( LM_DEBUG, "env_.root_path {}\n", env_.root_path.c_str() ));
	}

	///// bin_path 是一个非空的字符串 /////
	// bin_path is not a null string
	if( env_.bin_path.size() > 0 )
	{
		// 得到末尾字符， 看情况是否需要添加 "/"
		// get the last char to see if it is needed to add "/"
		ch = env_.bin_path.at(env_.bin_path.size() - 1);
		if( ch != '/' && ch != '\\' ) env_.bin_path += "/";

		// 将所有的 \\ 替换为 /
		// repalce all \\ to /
		STRUTIL::kbe_replace(env_.bin_path, "\\", "/");
		STRUTIL::kbe_replace(env_.bin_path, "//", "/");

		ACE_DEBUG(( LM_DEBUG, "env_.bin_path {}\n", env_.bin_path.c_str() ));
	}

	// 以 ; 分割所有的字符串，并将其把存在respaths_中
	// slipt the string using ;, and store the result into @param respaths_
	char splitFlag = ';';
	respaths_.clear();
	std::string tbuf = env_.all_res_paths;
	STRUTIL::kbe_split<char>(tbuf, splitFlag, respaths_);

	// windows用户不能分割冒号， 可能会把盘符给分割了
#if KBE_PLATFORM != PLATFORM_WIN32
	if( respaths_.size() < 2 )
	{
		respaths_.clear();
		splitFlag = ':';
		STRUTIL::kbe_split<char>(tbuf, splitFlag, respaths_);
	}
#endif

	env_.all_res_paths = "";
	std::vector<std::string>::iterator iter = respaths_.begin();
	for( ; iter != respaths_.end(); ++iter )
	{
		if( ( *iter ).size() <= 0 )
			continue;

		ch = ( *iter ).at(( *iter ).size() - 1);
		if( ch != '/' && ch != '\\' )
			( *iter ) += "/";

		env_.all_res_paths += ( *iter );
		env_.all_res_paths += splitFlag;
		STRUTIL::kbe_replace(env_.all_res_paths, "\\", "/");
		STRUTIL::kbe_replace(env_.all_res_paths, "//", "/");
	}

	if( env_.all_res_paths.size() > 0 )
		env_.all_res_paths.erase(env_.all_res_paths.size() - 1);
}
void ResourceManager::update_respool()
{
	TRACE("ResourceManager::get_res_path");

	ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
	if( !guard.locked() )
	{
		// handle error ...
		ACE_DEBUG(( LM_ERROR, "guard error\n" ));
	} else
	{
		// perform critical operation requiring lock to be held ...
		UnorderedMap< std::string, ResourceObjectRefAutoPtr>::iterator iter = respool_.begin();
		for( ; iter != respool_.end(); )
		{
			if( !iter->second->valid() )
			{
				respool_.erase(iter++);
			} else
			{
				iter++;
			}
		}
	}

	TRACE_RETURN_VOID();
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

bool ResourceManager::if_res_exist(const std::string& res)
{
	TRACE("ResourceManager::if_res_exist");

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
			TRACE_RETURN(true);
			//return true;
		}
	}

	TRACE_RETURN(false);
	//return false;
}

FILE* ResourceManager::open_res(const std::string res_name, const char* mode)
{
	TRACE("ResourceManager::open_res");

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
			TRACE_RETURN(f);
			//return f;
		}
	}

	TRACE_RETURN(NULL);
	//return NULL;
}

void ResourceManager::set_env_res_paths()
{
	TRACE("ResourceManager::set_env_res_path");

	char path[MAX_PATH];
	if( ACE_OS::getcwd(path, MAX_PATH) == NULL )
	{
		ACE_DEBUG(( LM_ERROR, "getcwd fails\n" ));
		return;
	}

	std::string s = path;
	ACE_DEBUG(( LM_DEBUG, "cwd path {%s}\n", s.c_str() ));
	std::string::size_type pos1 = s.find("\\zmd\\bins\\");

	if( pos1 == std::string::npos )
		pos1 = s.find("/zmd/bins/");

	if( pos1 == std::string::npos )
	{
		ACE_DEBUG(( LM_ERROR, "not find \\zmd\\bins\\ \n" ));
		return;
	}

	// get root_path paths
	s = s.substr(0, pos1 + 1);
	env_.root_path = s;
	ACE_DEBUG(( LM_DEBUG, "env_.root_path {%s}\n", s.c_str() ));

	env_.all_res_paths =
		env_.root_path + "zmd/resources/;" +
		env_.root_path + "zmd/assets/;" +
		env_.root_path + "assets/scripts/;" +
		env_.root_path + "assets/resources/";

	ACE_DEBUG(( LM_DEBUG, "env_.all_res_paths {%s}\n", env_.all_res_paths.c_str() ));

	system("pause");
	TRACE_RETURN_VOID();
}

ACE_KBE_END_VERSIONED_NAMESPACE_DECL