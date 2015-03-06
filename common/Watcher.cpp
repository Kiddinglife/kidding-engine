#include "Watcher.h"
#include "ace\Singleton.h"
#include "ace\Null_Mutex.h"
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
///////////////////////////////////////////////////////////////////////////////////////////////

Watchers* pWatchers = NULL;
WatcherPaths* pWatcherPaths = NULL;

Watcher::Watcher(std::string path) :
path_(path),
name_(),
strval_(),
id_(0),
numWitness_(0)
{
	std::string::size_type fi = path.find_first_of('/');
	if( fi == std::string::npos )
	{
		name_ = path;
		path_ = "";
	} else
	{
		std::vector<std::string> vec;
		kbe_split(path, '/', vec);

		std::vector<std::string>::size_type size = vec.size();
		name_ = vec[size - 1];
		path_ = path.erase(path.size() - name_.size() - 1, path.size());
	}
}
Watcher::~Watcher()
{
}
////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////
Watchers::Watchers() :watcherObjs_() { }
Watchers::~Watchers() { watcherObjs_.clear(); }

Watchers& Watchers::rootWatchers()
{
	if( !pWatchers ) pWatchers = ACE_Singleton<Watchers, ACE_Null_Mutex>::instance();
	return *pWatchers;
}

void Watchers::addToStream(Bundle* s)
{
	WatchersMap::iterator iter = watcherObjs_.begin();
	for( ; iter != watcherObjs_.end(); ++iter )
	{
		iter->second->addToStream(s);
	}
}

void Watchers::updateStream(Bundle* s)
{
}

bool Watchers::hasWatcher(const std::string& name)
{
	WatchersMap::iterator iter = watcherObjs_.find(name);
	return iter != watcherObjs_.end();
}

bool Watchers::addWatcher(const std::string& path, Watcher* pwo)
{
	if( hasWatcher(pwo->name_) )
	{
		return false;
	}

	watcherObjs_[pwo->name_].reset(pwo);

	ACE_DEBUG(( LM_DEBUG,
		"Watchers::addWatcher: path={}, name={}, id={}\n",
		pwo->path_.c_str(), pwo->name_.c_str(), pwo->id_ ));

	return true;
}

bool Watchers::delWatcher(const std::string& name)
{
	if( !hasWatcher(name) )
		return false;
	watcherObjs_.erase(name);
	ACE_DEBUG(( LM_DEBUG, "Watchers::delWatcher: {}\n", name ));
	return true;
}

void Watchers::readWatchers(Bundle* s)
{
	WatchersMap::iterator iter = watcherObjs_.begin();
	WatchersMap::iterator end = watcherObjs_.end();
	for( ; iter != end; ++iter )
	{
		iter->second->addToInitStream(s);
	}
}

Shared_ptr< Watcher > Watchers::getWatcher(const std::string& name)
{
	WatchersMap::iterator iter = watcherObjs_.find(name);
	if( iter != watcherObjs_.end() )
		return iter->second;

	return Shared_ptr< Watcher >((Watcher*) NULL);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////
WatcherPaths::WatcherPaths() : watcherPaths_(), watchers_() { }
WatcherPaths::~WatcherPaths() { watcherPaths_.clear(); }

WatcherPaths& WatcherPaths::root()
{
	if( pWatcherPaths == NULL )
	{
		pWatcherPaths = ACE_Singleton<WatcherPaths, ACE_Null_Mutex>::instance();
	}
	return *pWatcherPaths;
}

void WatcherPaths::addToStream(Bundle* s)
{
}

void WatcherPaths::updateStream(Bundle* s)
{
}

bool WatcherPaths::addWatcher(std::string path, Watcher* pwo)
{
	std::string szpath, name;
	std::string::size_type fi = path.find_first_of('/');
	if( fi == std::string::npos )
	{
		name = path;
		szpath = "";
	} else
	{
		std::vector<std::string> vec;
		kbe_split(path, '/', vec);

		std::vector<std::string>::size_type size = vec.size();
		name = vec[size - 1];
		szpath = path.erase(path.size() - name.size() - 1, path.size());
	}

	static WATCHER_ID id = 1;
	pwo->id_ = id++;

	return _addWatcher(szpath, pwo);
}

bool WatcherPaths::_addWatcher(std::string path, Watcher* pwo)
{
	if( path.size() > 0 )
	{
		std::vector<std::string> vec;
		kbe_split(path, '/', vec);

		path.erase(0, vec[0].size() + 1);

		WatcherPathsMap::iterator iter = watcherPaths_.find(vec[0]);
		if( iter != watcherPaths_.end() )
		{
			return iter->second->_addWatcher(path, pwo);
		} else
		{
			WatcherPaths* watcherPaths = new WatcherPaths();
			watcherPaths_[vec[0]].reset(watcherPaths);
			return watcherPaths->_addWatcher(path, pwo);
		}
	}

	if( !watchers_.addWatcher(path, pwo) )
	{
		ACE_ASSERT(false && "watcher is exist!\n");
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

ACE_KBE_END_VERSIONED_NAMESPACE_DECL