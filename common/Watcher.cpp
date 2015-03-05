#include "Watcher.h"

ACE_KBE_BEGIN_VERSIONED_NAMESPACE_DECL
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
ACE_KBE_END_VERSIONED_NAMESPACE_DECL