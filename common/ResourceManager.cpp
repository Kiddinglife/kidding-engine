#include "ResourceManager.h"
#include "common/Watcher.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

ACE_UINT64 ResourceManager::respool_timeout = 0;
ACE_UINT32 ResourceManager::respool_buffersize = 0;
ACE_UINT32 ResourceManager::respool_checktick = 0;


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
		ACE_DEBUG(( MY_DEBUG"ResourceObject::~ResourceObject(): {%s}\n", res_name_ ));
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
		ACE_DEBUG(( MY_ERROR"FileObject::FileObject(): open({%s}) {%s} error!\n", model, res_name_ ));
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
		ACE_ERROR_RETURN(( MY_ERROR"FileObject::seek: { %s } invalid!\n", res_name_ ), false);
	}
	update();
	return ACE_OS::fseek(fd_, idx, flags) != -1;
}

ACE_UINT32 FileObject::read(char* buf, ACE_UINT32 cnt)
{
	if( invalid_ || fd_ == NULL )
	{
		ACE_ERROR_RETURN(( MY_ERROR"FileObject::read: {%s} invalid!\n", res_name_ ), 0);
	}
	update();
	return ACE_OS::fread(buf, sizeof(char), cnt, fd_);
}

ACE_UINT32 FileObject::tell()
{
	if( invalid_ || fd_ == NULL )
	{
		ACE_ERROR_RETURN(( MY_ERROR"FileObject::tell: {%s} invalid!\n", res_name_ ), 0);
	}
	update();
	return ACE_OS::ftell(fd_);
}



ResourceManager::ResourceManager() :
env_(),
all_used_paths_vector_(),
isInit_(false),
respool_(),
mutex_()
{
	init();
	//ACE_OS::system("pause");
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

bool ResourceManager::init_watchers()
{
	CRATE_WATCH_OBJECT("env/ZMD_ROOT_PATH", env_.root_path);
	CRATE_WATCH_OBJECT("env/ZMD_ALL_USED_PATHS", env_.all_used_paths);
	CRATE_WATCH_OBJECT("env/ZMD_BINS_PATH", env_.bin_path);
	CRATE_WATCH_OBJECT("env/USER_SCRIPTS_PATH", env_.python_user_scripts_path);
	CRATE_WATCH_OBJECT("env/USER_RESOURCES_PATH", env_.python_user_res_path);
	CRATE_WATCH_OBJECT("env/PYTHON_NATIVE_LIBS_PATH", env_.python_native_libs_path);
	return true;
}

bool ResourceManager::init()
{
	// 获取引擎环境配置 get the env config
	if( ( env_.root_path = ACE_OS::getenv("ZMD_ROOT") == NULL ?
		"" : ACE_OS::getenv("ZMD_ROOT") ) == "" ||
		( env_.all_used_paths = ACE_OS::getenv("ZMD_RESOURCES_PATH") == NULL ?
		"" : ACE_OS::getenv("ZMD_RESOURCES_PATH") ) == "" ||
		( env_.bin_path = ACE_OS::getenv("ZMD_BINS_PATH") == NULL ?
		"" : ACE_OS::getenv("ZMD_BINS_PATH") ) == "" )
	{
		auto_set_env_res_paths();
	}

	adjust_paths();
	isInit_ = true;
	respool_.clear();
	return true;
}

void ResourceManager::adjust_paths()
{
	TRACE("ResourceManager::adjust_paths");

	char ch;

	///// root_path 是一个非空的字符串 //////
	// root_path is not a null string
	if( env_.root_path.size() > 0 )
	{
		// 得到末尾字符， 看情况是否需要添加 "/"
		// get the last char to see if it is needed to add "/"
		ch = env_.root_path.at(env_.root_path.size() - 1);
		if( ch != '/' && ch != '\\' && ch != ';' )
			env_.root_path += "/";
		else if( ch == ';' )
		{

		}
		// 将所有的 \\ 替换为 /
		// repalce all \\ to /
		STRUTIL::kbe_replace(env_.root_path, "\\", "/");
		STRUTIL::kbe_replace(env_.root_path, "//", "/");

		//ACE_DEBUG(( LM_DEBUG, "env_.root_path {%s}\n", env_.root_path.c_str() ));
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

		//ACE_DEBUG(( LM_DEBUG, "env_.bin_path {%s}\n", env_.bin_path.c_str() ));
	}

	///// python_native_libs_path 是一个非空的字符串 /////
	// python_native_libs_path is not a null string
	if( env_.python_native_libs_path.size() > 0 )
	{
		// 得到末尾字符， 看情况是否需要添加 "/"
		// get the last char to see if it is needed to add "/"
		ch = env_.python_native_libs_path.at(env_.python_native_libs_path.size() - 1);
		if( ch != '/' && ch != '\\' ) env_.python_native_libs_path += "/";

		// 将所有的 \\ 替换为 /
		// repalce all \\ to /
		STRUTIL::kbe_replace(env_.python_native_libs_path, "\\", "/");
		STRUTIL::kbe_replace(env_.python_native_libs_path, "//", "/");

		//ACE_DEBUG(( LM_DEBUG, "env_.python_native_libs_path {%s}\n", env_.python_native_libs_path.c_str() ));
	}

	///// python_user_scripts_path 是一个非空的字符串 /////
	// python_user_scripts_path is not a null string
	if( env_.python_user_scripts_path.size() > 0 )
	{
		// 得到末尾字符， 看情况是否需要添加 "/"
		// get the last char to see if it is needed to add "/"
		ch = env_.python_user_scripts_path.at(env_.python_user_scripts_path.size() - 1);
		if( ch != '/' && ch != '\\' ) env_.python_user_scripts_path += "/";

		// 将所有的 \\ 替换为 /
		// repalce all \\ to /
		STRUTIL::kbe_replace(env_.python_user_scripts_path, "\\", "/");
		STRUTIL::kbe_replace(env_.python_user_scripts_path, "//", "/");

		//ACE_DEBUG(( LM_DEBUG, "env_.python_user_scripts_path {%s}\n", env_.python_user_scripts_path.c_str() ));
	}

	///// python_user_res_path 是一个非空的字符串 /////
	// python_user_res_path is not a null string
	if( env_.python_user_res_path.size() > 0 )
	{
		// 得到末尾字符， 看情况是否需要添加 "/"
		// get the last char to see if it is needed to add "/"
		ch = env_.python_user_res_path.at(env_.python_user_res_path.size() - 1);
		if( ch != '/' && ch != '\\' ) env_.python_user_res_path += "/";

		// 将所有的 \\ 替换为 /
		// repalce all \\ to /
		STRUTIL::kbe_replace(env_.python_user_res_path, "\\", "/");
		STRUTIL::kbe_replace(env_.python_user_res_path, "//", "/");

		//ACE_DEBUG(( LM_DEBUG, "env_.python_user_res_path {%s}\n",
		//	env_.python_user_res_path.c_str() ));
	}
	// 以 ; 分割所有的字符串，并将其把存在all_used_paths_vector_中
	// slipt the string using ;, and store the result into @param all_used_paths_vector_
	char splitFlag = ';';
	all_used_paths_vector_.clear();
	std::string tbuf = env_.all_used_paths;
	STRUTIL::kbe_split<char>(tbuf, splitFlag, all_used_paths_vector_);

	// windows用户不能分割冒号， 可能会把盘符给分割了
#if KBE_PLATFORM != PLATFORM_WIN32
	if( all_used_paths_vector_.size() < 2 )
	{
		all_used_paths_vector_.clear();
		splitFlag = ':';
		STRUTIL::kbe_split<char>(tbuf, splitFlag, all_used_paths_vector_);
	}
#endif

	env_.all_used_paths = "";
	std::vector<std::string>::iterator iter = all_used_paths_vector_.begin();
	for( ; iter != all_used_paths_vector_.end(); ++iter )
	{
		if( ( *iter ).size() <= 0 )
			continue;

		ch = ( *iter ).at(( *iter ).size() - 1);
		if( ch != '/' && ch != '\\' )
			( *iter ) += "/";

		STRUTIL::kbe_replace(( *iter ), "\\", "/");
		STRUTIL::kbe_replace(( *iter ), "//", "/");

		env_.all_used_paths += ( *iter );
		env_.all_used_paths += splitFlag;
	}

	if( env_.all_used_paths.size() > 0 ) env_.all_used_paths.erase(env_.all_used_paths.size() - 1);
	//ACE_DEBUG(( LM_DEBUG, "env_.all_res_paths {%s}\n", env_.all_res_paths.c_str() ));

	TRACE_RETURN_VOID();
}

void ResourceManager::update_respool()
{
	TRACE("ResourceManager::get_res_path");

	UnorderedMap< std::string, ResourceObjectRefAutoPtr>::iterator iter;
	ACE_Guard<ACE_Thread_Mutex> guard(mutex_);

	if( !guard.locked() )
	{
		// handle error ...
		ACE_DEBUG(( MY_ERROR"guard error\n" ));
	} else
	{
		// perform critical operation requiring lock to be held ...
		iter = respool_.begin();
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
	std::vector<std::string>::iterator iter = all_used_paths_vector_.begin();

	for( ; iter != all_used_paths_vector_.end(); ++iter )
	{
		fpath = ( ( *iter ) + res_name );

		STRUTIL::kbe_replace(fpath, "\\", "/");
		STRUTIL::kbe_replace(fpath, "//", "/");

		f = ACE_OS::fopen(fpath.c_str(), "r");
		if( f != NULL )
		{
			ACE_OS::fclose(f);
			TRACE_RETURN(fpath);
			//return fpath;
		}
	}
	TRACE_RETURN(res_name);
	//return res_name;
}

bool ResourceManager::exist(const std::string res)
{
	TRACE("ResourceManager::exist");

	FILE * f = NULL;
	std::string fpath;
	std::vector<std::string>::iterator iter = all_used_paths_vector_.begin();

	for( ; iter != all_used_paths_vector_.end(); ++iter )
	{
		fpath = ( ( *iter ) + res );
		f = ACE_OS::fopen(fpath.c_str(), "r");
		if( f != NULL )
		{
			ACE_OS::fclose(f);
			TRACE_RETURN(true);
		}
	}
	TRACE_RETURN(false);;
}

FILE* ResourceManager::open_res_fd(const std::string res_name, const char* mode)
{
	TRACE("ResourceManager::open_res");

	FILE * f = NULL;
	std::string fpath;
	std::vector<std::string>::iterator iter = all_used_paths_vector_.begin();

	for( ; iter != all_used_paths_vector_.end(); ++iter )
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

ResourceObjectRefAutoPtr ResourceManager::open_res_obj(const char* res, const char* model,
	ACE_UINT32 flags)
{
	TRACE("ResourceManager::open_res");

	std::string res_path = get_res_path(res);
	if( ResourceManager::respool_checktick == 0 )
	{
		ResourceObjectRefAutoPtr ptr(new FileObject(res_path.c_str(), flags, model));
		TRACE_RETURN(ptr);
	}

	UnorderedMap< std::string, ResourceObjectRefAutoPtr>::iterator iter;
	ACE_Guard<ACE_Thread_Mutex> guard(mutex_);
	if( !guard.locked() )
	{
		// handle error ...
		ACE_DEBUG(( MY_ERROR"guard error\n" ));
	} else
	{
		// perform critical operation requiring lock to be held ...
		iter = respool_.find(res_path);
		if( iter == respool_.end() )
		{
			FileObject* fobj = new FileObject(res_path.c_str(), flags, model);
			respool_[res_path].reset(fobj);
			fobj->update();
			TRACE_RETURN(respool_[res_path]);
		} else
		{
			iter->second->update();
			TRACE_RETURN(iter->second);
		}
	}
}

void ResourceManager::auto_set_env_res_paths()
{
	TRACE("ResourceManager::set_env_res_path");

	char path[MAX_PATH];
	if( ACE_OS::getcwd(path, MAX_PATH) == NULL )
	{
		ACE_DEBUG(( MY_ERROR"getcwd fails\n" ));
		return;
	}

	std::string s = path;
	//ACE_DEBUG(( LM_DEBUG, "cwd path {%s}\n", s.c_str() ));
	std::string::size_type pos1 = s.find("\\zmd\\bins\\");

	if( pos1 == std::string::npos )
		pos1 = s.find("/zmd/bins/");

	if( pos1 == std::string::npos )
	{
		ACE_DEBUG(( MY_ERROR"not find \\zmd\\bins\\ \n" ));
		return;
	}

	// get root_path paths
	s = s.substr(0, pos1 + 1);
	env_.root_path = s;
	env_.bin_path = env_.root_path + "zmd/bins/";
	env_.python_native_libs_path = env_.root_path + "zmd/resources/scripts/common/Lib/";
	env_.python_user_scripts_path = env_.root_path + "assets/scripts/";
	env_.python_user_res_path = env_.root_path + "assets/resources/";
	env_.all_used_paths =
		env_.root_path + "zmd/resources/;" +
		env_.root_path + "zmd/resources/scripts/;" +
		env_.root_path + "zmd/resources/key/;" +
		env_.root_path + "zmd/resources/configs/;" +
		env_.root_path + "assets/;" +
		env_.root_path + "assets/scripts/;" +
		env_.root_path + "assets/tools/;" +
		env_.root_path + "assets/resources/;" +
		env_.root_path + "assets/resources/server/;" +
		env_.root_path + "assets/resources/spaces";

	TRACE_RETURN_VOID();
}

void ResourceManager::print_env_paths(void)
{
	ACE_DEBUG(( MY_DEBUG
		"ZMD_ROOT_PATH { %s }\n"
		"ZMD_BINS_PATH { %s }\n"
		"ZMD_ALL_USED_PATHS { %s }\n"
		"USER_SCRIPTS_PATH { %s }\n"
		"USER_RESOURCES_PATH { %s }\n"
		"PYTHON_NATIVE_LIBS_PATH { %s }\n",
		env_.root_path.c_str(),
		env_.bin_path.c_str(),
		env_.all_used_paths.c_str(),
		env_.python_user_scripts_path.c_str(),
		env_.python_user_res_path.c_str(),
		env_.python_native_libs_path.c_str() ));
}

ACE_KBE_END_VERSIONED_NAMESPACE_DECL