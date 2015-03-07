#include "Watcher.h"
#include "ace\Singleton.h"
#include "ace\Null_Mutex.h"
#include <iostream>
ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL

///////////////////////////////////////////////////////////////////////////////////////////////
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


/////////////////////////////////////////////////////////////////////////////////////////////////////
Watchers::Watchers() :watcherObjs_() { }
Watchers::~Watchers() { watcherObjs_.clear(); }

Watchers& Watchers::rootWatchers()
{
	return *ACE_Singleton<Watchers, ACE_Null_Mutex>::instance();;
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
		"Watchers::addWatcher: path={%s}, name={%s}, id={%d}\n",
		pwo->path_.c_str(), pwo->name_.c_str(), pwo->id_ ));

	return true;
}

bool Watchers::delWatcher(const std::string& name)
{
	for( auto& x : watcherObjs_ )
		std::cout << x.first << ": " << x.second << std::endl;

	if( !hasWatcher(name) )
		return false;
	watcherObjs_.erase(name);
	ACE_DEBUG(( LM_DEBUG, "2Watchers::delWatcher: {%s}\n", name.c_str() ));
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
	return *ACE_Singleton<WatcherPaths, ACE_Null_Mutex>::instance();
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
			watcherPaths->name_ = vec[0] + "-WatcherPaths";
			watcherPaths_[vec[0]].reset(watcherPaths);
			return watcherPaths->_addWatcher(path, pwo);
		}
	}
	ACE_DEBUG(( LM_DEBUG, "add watcher (%s), watcher path (%s)\n", pwo->name_.c_str(), this->name_.c_str() ));

	if( !watchers_.addWatcher(path, pwo) )
	{
		ACE_ASSERT(false && "watcher is exist!\n");
		return false;
	}

	return true;
}

Watcher* WatcherPaths::addWatcherFromStream(std::string path, std::string name,
	WATCHER_ID wid, WATCHER_VALUE_TYPE wtype, Bundle* s)
{
	Watcher* pWobj = NULL;

	std::string fullpath = "";

	if( path.size() == 0 )
		fullpath = name;
	else
		fullpath = path + "/" + name;

	pWobj = new Watcher(fullpath);

	switch( wtype )
	{
		case WATCHER_VALUE_TYPE_UINT8:
			pWobj->updateStream<ACE_CDR::Octet>(s);
			break;
		case WATCHER_VALUE_TYPE_UINT16:
			pWobj->updateStream<ACE_CDR::UShort>(s);
			break;
		case WATCHER_VALUE_TYPE_UINT32:
			pWobj->updateStream<ACE_CDR::ULong>(s);
			break;
		case WATCHER_VALUE_TYPE_UINT64:
			pWobj->updateStream<ACE_CDR::ULongLong>(s);
			break;
		case WATCHER_VALUE_TYPE_INT8:
			pWobj->updateStream<ACE_CDR::Char>(s);
			break;
		case WATCHER_VALUE_TYPE_INT16:
			pWobj->updateStream<ACE_CDR::Short>(s);
			break;
		case WATCHER_VALUE_TYPE_INT32:
			pWobj->updateStream<ACE_CDR::Long>(s);
			break;
		case WATCHER_VALUE_TYPE_INT64:
			pWobj->updateStream<ACE_CDR::LongLong>(s);
			break;
		case WATCHER_VALUE_TYPE_FLOAT:
			pWobj->updateStream<ACE_CDR::Float>(s);
			break;
		case WATCHER_VALUE_TYPE_DOUBLE:
			pWobj->updateStream<ACE_CDR::Double>(s);
			break;
		case WATCHER_VALUE_TYPE_CHAR:
			pWobj->updateStream<char*>(s);
			break;
		case WATCHER_VALUE_TYPE_STRING:
			pWobj->updateStream<std::string>(s);
			break;
		case WATCHER_VALUE_TYPE_BOOL:
			pWobj->updateStream<ACE_CDR::Boolean>(s);
			break;
		case WATCHER_VALUE_TYPE_COMPONENT_TYPE:
			pWobj->updateStream<KBE_SRV_COMPONENT_TYPE>(s);
			break;
		default:
			ACE_ASSERT(false && "no support!\n");
	};

	pWobj->id_ = wid;
	bool ret = _addWatcher(path, pWobj);
	ACE_ASSERT(ret);

	return pWobj;
}

bool WatcherPaths::hasWatcherPath(const std::string& fullpath)
{
	std::vector<std::string> vec;
	kbe_split(fullpath, '/', vec);

	if( vec.size() == 1 )
		return false;

	vec.erase(vec.end() - 1);

	WatcherPaths* pCurrWatcherPaths = this;
	for( std::vector<std::string>::iterator iter = vec.begin(); iter != vec.end(); ++iter )
	{
		WatcherPathsMap& paths = pCurrWatcherPaths->watcherPaths_;
		UnorderedMap<std::string, Shared_ptr<WatcherPaths> >::iterator fiter = paths.find(( *iter ));
		if( fiter == paths.end() )
			return false;

		pCurrWatcherPaths = fiter->second.get();
		if( pCurrWatcherPaths == NULL )
			return false;
	}

	return pCurrWatcherPaths != NULL;
}

bool WatcherPaths::delWatcher(const std::string& fullpath)
{
	if( hasWatcherPath(fullpath) == false )
	{
		ACE_DEBUG(( LM_DEBUG, "WatcherPaths::delWatcher: not found {%s}\n", fullpath ));
		return false;
	}

	std::vector<std::string> vec;
	kbe_split(fullpath, '/', vec);

	if( vec.size() == 1 )
		return false;

	std::string name = ( *( vec.end() - 1 ) );
	vec.erase(vec.end() - 1);

	WatcherPaths* pCurrWatcherPaths = this;
	for( std::vector<std::string>::iterator iter = vec.begin(); iter != vec.end(); ++iter )
	{
		WatcherPathsMap& paths = pCurrWatcherPaths->watcherPaths_;
		UnorderedMap<std::string, Shared_ptr<WatcherPaths> >::iterator fiter = paths.find(( *iter ));
		if( fiter == paths.end() )
			return false;

		pCurrWatcherPaths = fiter->second.get();
		if( pCurrWatcherPaths == NULL )
			return false;
	}

	return pCurrWatcherPaths->watchers_.delWatcher(name);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

ACE_KBE_END_VERSIONED_NAMESPACE_DECL